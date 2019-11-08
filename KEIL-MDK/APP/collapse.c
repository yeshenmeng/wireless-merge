#include "collapse.h"
#include "bma4.h"
#include "bma456.h"
#include "nrf_drv_spi.h"
#include "string.h"
#include "math.h"
#include "nrf_drv_gpiote.h"
#include "easy_fifo.h"
#include "sys_param.h"
#include "light.h"
#include "filter_average.h"
#include "data_sort.h"


#define ANY_MOTION_INT_PIN		COLLAPSE_INT1_PIN

static uint16_t bma4_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *write_data, uint16_t len);
static uint16_t bma4_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *read_data, uint16_t len);

/* Declare an instance of the BMA4xy device */
static struct bma4_dev bma456_dev = {
	.interface = BMA4_SPI_INTERFACE,
	.bus_read = bma4_read,
	.bus_write = bma4_write,
	.delay = nrf_delay_ms,
	.read_write_len = 8,
};

/* Declare an accelerometer configuration structure */
static struct bma4_accel_config accel_conf;

#if (BMA456_USE_FIFO == 1)
static struct bma4_fifo_frame fifo_frame;
/* Declare memory to store the raw FIFO buffer information */	
static uint8_t fifo_buff[BMA456_WTM_SIZE];
/* Declare instances of the sensor data structure */
static struct bma4_accel bma4_fifo_data[sizeof(fifo_buff)/7];
#endif

static struct bma4_accel bma4_data; //最近一次数据
static uint16_t bma456_evt = 0;
static sens_data_t sens_data;
static uint8_t fifo_mtx = 0;
static uint8_t wait_trigger_flag = 0;
static collapse_state_t collapse_state;
static collapse_obj_t collapse_obj;

/* 过滤器参数设置 */
#define FILTER_DATA_BUF_SIZE	40
static sens_angle_t sens_angle[FILTER_DATA_BUF_SIZE];
static const float filter_ratio_x = 0.4;
static const float filter_ratio_y = 0.6;
static const float filter_ratio_z = 0.4;
static const float filter_delta_x = 0.06;
static const float filter_delta_y = 0.07;
static const float filter_delta_z = 0.06;
static uint8_t filter_nums = 4;

static void collapse_sw_spi_cfg(void)
{
	nrf_gpio_cfg(COLLAPSE_SW_SPI_SCK_PIN,
				 NRF_GPIO_PIN_DIR_OUTPUT,
				 NRF_GPIO_PIN_INPUT_DISCONNECT,
				 NRF_GPIO_PIN_NOPULL,
				 NRF_GPIO_PIN_S0S1,
				 NRF_GPIO_PIN_NOSENSE);
	
	nrf_gpio_cfg(COLLAPSE_SW_SPI_MOSI_PIN,
				 NRF_GPIO_PIN_DIR_OUTPUT,
				 NRF_GPIO_PIN_INPUT_DISCONNECT,
				 NRF_GPIO_PIN_NOPULL,
				 NRF_GPIO_PIN_S0S1,
				 NRF_GPIO_PIN_NOSENSE);

	nrf_gpio_cfg(COLLAPSE_SW_SPI_MISO_PIN,
				 NRF_GPIO_PIN_DIR_INPUT,
				 NRF_GPIO_PIN_INPUT_CONNECT,
//				 NRF_GPIO_PIN_NOPULL,
				 NRF_GPIO_PIN_PULLDOWN,
				 NRF_GPIO_PIN_S0S1,
				 NRF_GPIO_PIN_NOSENSE);

	nrf_gpio_cfg(COLLAPSE_SW_SPI_CS_PIN,
				 NRF_GPIO_PIN_DIR_OUTPUT,
				 NRF_GPIO_PIN_INPUT_DISCONNECT,
				 NRF_GPIO_PIN_NOPULL,
				 NRF_GPIO_PIN_S0S1,
				 NRF_GPIO_PIN_NOSENSE);
}

static void collapse_sw_spi_cfg_low_power(void)
{
	COLLAPSE_SW_SPI_SCK_CLR();
	COLLAPSE_SW_SPI_MOSI_CLR();
	COLLAPSE_SW_SPI_CS_DISABLE();
}

static void collapse_sw_spi_transmit_receive(uint8_t* tx_buf, uint16_t tx_size, uint8_t* rx_buf, uint16_t rx_size)
{
	COLLAPSE_SW_SPI_CS_ENABLE();
	
	uint8_t _tx_data = 0;
	uint8_t _rx_data = 0;
	uint8_t dummy = 0XFF;
	uint16_t tx_xfer_cnt = tx_size;
	uint16_t rx_xfer_cnt = rx_size;
	
	while((tx_xfer_cnt > 0) || (rx_xfer_cnt > 0))
	{
		if(tx_xfer_cnt > 0)
		{
			_tx_data = (*tx_buf++);
			tx_xfer_cnt--;
		}
		else
		{
			_tx_data = dummy;
		}
		
		for(int i=0; i<8; i++)
		{
			COLLAPSE_SW_SPI_SCK_CLR();
			if(_tx_data & 0X80)
			{
				COLLAPSE_SW_SPI_MOSI_SET();
			}
			else
			{
				COLLAPSE_SW_SPI_MOSI_CLR();
			}
			_tx_data <<= 1;
			nrf_delay_us(2);
			COLLAPSE_SW_SPI_SCK_SET();
			nrf_delay_us(2);
			
			_rx_data <<= 1;
			if(COLLAPSE_SW_SPI_MISO_READ())
			{
				_rx_data++;
			}
		}
		
		if(rx_xfer_cnt > 0)
		{
			(*(uint8_t *)rx_buf++) = _rx_data;
			rx_xfer_cnt--;
		}
	}
	
	COLLAPSE_SW_SPI_SCK_CLR();
	COLLAPSE_SW_SPI_CS_DISABLE();
}

/* SPI收发 */
static void collapse_spi_transmit_receive(uint8_t* tx_buffer, uint16_t tx_length, uint8_t* rx_buffer, uint16_t rx_length)
{
	collapse_sw_spi_transmit_receive(tx_buffer, tx_length, rx_buffer, rx_length);
}

/* collapse配置 */
static void collapse_cfg(void)
{
	collapse_sw_spi_cfg();
	/* 给SCB引脚上升沿(切换芯片通信模式) */
	uint8_t dummy = 0XF0;
	collapse_spi_transmit_receive(&dummy, 1, NULL, 0); //切换到SPI通信
}

/* collapse配置低功耗 */
static void collapse_cfg_low_power(void)
{
	collapse_sw_spi_cfg_low_power();
}

/* BMA4写数据 */
static uint16_t bma4_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *write_data, uint16_t len)
{
	uint8_t buf[len+1];
	dev_addr = dev_addr;
	buf[0] = reg_addr;
	memcpy(&buf[1], write_data, len);
	collapse_spi_transmit_receive(buf, sizeof(buf), NULL, 0);
	return 0;
}

/* BMA4读数据 */
static uint16_t bma4_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *read_data, uint16_t len)
{
	dev_addr = dev_addr;
	uint8_t buf[len+2];
	buf[0] = reg_addr;
	collapse_spi_transmit_receive(buf, 1, &buf[1], sizeof(buf)-1);
	memcpy(read_data, &buf[2], len);
	return 0;
}

static uint16_t collapse_config_accelerometer(void)
{
	uint16_t ret = BMA4_OK;

	/* Declare an accelerometer configuration structure */
	/* Assign the desired settings */
	accel_conf.perf_mode = 0;
	accel_conf.odr = BMA4_OUTPUT_DATA_RATE_6_25HZ;
//	accel_conf.odr = BMA4_OUTPUT_DATA_RATE_400HZ;
	accel_conf.range = BMA4_ACCEL_RANGE_2G;
	accel_conf.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
	ret |= bma4_set_accel_config(&accel_conf, &bma456_dev);
	nrf_delay_ms(1);
	
#if (BMA456_USE_FIFO == 1)	
	ret |= bma4_set_fifo_self_wakeup(BMA4_ENABLE, &bma456_dev);
	nrf_delay_ms(1);
#endif
	
	ret |= bma4_set_advance_power_save(BMA4_ENABLE, &bma456_dev);
	nrf_delay_ms(1);

	return ret;
}

static uint16_t collapse_enable(uint8_t accel_en)
{
	uint16_t ret = BMA4_OK;
	 
	if(accel_en == 1)
	{
		/* Enable the accelerometer */
		ret |= bma4_set_accel_enable(1, &bma456_dev);
	}
	else if(accel_en == 0)
	{
		/* disable the accelerometer */
		ret |= bma4_set_accel_enable(0, &bma456_dev);
	}
	
	nrf_delay_ms(1);
	return ret;
}

static float collapse_read_temperature(void)
{
//	uint8_t err_cnt = 5;
	float temp = 0;
	int32_t tmp; 
	
//	while(err_cnt--)
//	{
//		/* Get temperature in degree C */
//		if(BMA4_OK != bma4_get_temperature(&tmp, BMA4_DEG, &bma456_dev))
//		{
//			continue;
//		}
//		
//		if(((tmp - 23) / BMA4_SCALE_TEMP) == 0x80) 
//		{
//			continue;
//		}
//		
//		temp = (float)tmp / (float)BMA4_SCALE_TEMP;
//		if(temp < -40 || temp > 85)
//		{
//			temp = 0;
//			continue;
//		}
//		
//		break;
//	}
	
	sd_temp_get(&tmp);
	temp = tmp * 1.0 / 4;
	
	return temp;
}

#if (BMA456_USE_FIFO == 1)
static uint16_t collapse_config_fifo_buffer(void)
{
	/* Setup and configure the FIFO buffer */
	/* Modify the FIFO buffer instance and link to the device instance */
	fifo_frame.data = fifo_buff;
	fifo_frame.length = sizeof(fifo_buff);
	fifo_frame.fifo_data_enable = BMA4_ENABLE;
	fifo_frame.fifo_header_enable = BMA4_ENABLE;
	bma456_dev.fifo = &fifo_frame;
	
	/* Disable the advanced power save mode to configure the FIFO buffer */
	uint16_t ret = bma4_set_advance_power_save(BMA4_DISABLE, &bma456_dev);
	nrf_delay_ms(1);
	
	uint16_t wtm_len = (fifo_frame.length-20 > 0) ? (fifo_frame.length-20) : (fifo_frame.length);
	ret |= bma4_set_fifo_wm(wtm_len, &bma456_dev);
	nrf_delay_ms(1);
	
	/* Configure the FIFO buffer */
	ret |= bma4_set_fifo_config((BMA4_FIFO_STOP_ON_FULL | BMA4_FIFO_ACCEL | BMA4_FIFO_HEADER), BMA4_ENABLE, &bma456_dev);
	nrf_delay_ms(1);
	
	ret |= bma4_set_fifo_config(BMA4_FIFO_TIME, BMA4_DISABLE, &bma456_dev);
	nrf_delay_ms(1);

	ret |= bma4_set_command_register(0XB0, &bma456_dev);
	nrf_delay_ms(1);

	ret |= bma4_set_advance_power_save(BMA4_ENABLE, &bma456_dev);
	nrf_delay_ms(1);
	
	return ret;
}
#endif

static void bma456_x_accel_to_angle_convert(void)
{
	float pi = atan(1.0) * 4;
	sens_data.angle.x_angle
		= atan(sens_data.accel.x_accel / (sqrt(pow(sens_data.accel.y_accel,2)+pow(sens_data.accel.z_accel,2)))) * 180 / pi;
}

static void bma456_y_accel_to_angle_convert(void)
{
	float pi = atan(1.0) * 4;
	sens_data.angle.y_angle
		= atan(sens_data.accel.y_accel / (sqrt(pow(sens_data.accel.x_accel,2)+pow(sens_data.accel.z_accel,2)))) * 180 / pi;
}

static void bma456_z_accel_to_angle_convert(void)
{
	float pi = atan(1.0) * 4;
	sens_data.angle.z_angle
		= atan(sens_data.accel.z_accel / (sqrt(pow(sens_data.accel.x_accel,2)+pow(sens_data.accel.y_accel,2)))) * 180 / pi;
}

static void bma456_d_a_convert(void)
{
	/* Get LSB per bit given the range and resolution */
	float lsb_per_mg = pow(2, accel_conf.range+2) / pow(2, bma456_dev.resolution) * 1000;
	uint16_t max_accel_value = pow(2, bma456_dev.resolution) / pow(2, accel_conf.range+1);
	bma4_data.x = (bma4_data.x > max_accel_value) ? (max_accel_value) : (bma4_data.x);
	bma4_data.y = (bma4_data.y > max_accel_value) ? (max_accel_value) : (bma4_data.y);
	bma4_data.z = (bma4_data.z > max_accel_value) ? (max_accel_value) : (bma4_data.z);
	
	sens_data.accel.x_accel = lsb_per_mg * bma4_data.x;
	sens_data.accel.y_accel = lsb_per_mg * bma4_data.y;
	sens_data.accel.z_accel = lsb_per_mg * bma4_data.z;
	bma456_x_accel_to_angle_convert();
	bma456_y_accel_to_angle_convert();
	bma456_z_accel_to_angle_convert();
	
	easy_fifo_write((uint8_t*)&sens_data, sizeof(sens_data));
}

static uint16_t collapse_init_accelerometer(void)
{
	/* Reading the chip id. */
	uint16_t ret = bma456_init(&bma456_dev);
	
	/* Performing initialization sequence. 
	   Checking the correct status of the initialization sequence.
	*/
	ret |= bma456_write_config_file(&bma456_dev);
	return ret;
}

static uint16_t collapse_reset(void)
{
	/* Triggers a soft reset */
	uint16_t ret = bma4_set_command_register(0xB6, &bma456_dev);
	nrf_delay_ms(200);
	uint8_t dummy = 0XF0;
	collapse_spi_transmit_receive(&dummy, 1, NULL, 0); //切换到SPI通信
	return ret;
}

#if (BMA456_USE_FIFO == 1)
static uint16_t bma456_read_fifo(void)
{
	uint16_t fifo_len = 0;
	memset(&bma4_data, 0X00, sizeof(bma4_data));
	bma4_get_fifo_length(&fifo_len, &bma456_dev);
	bma4_read_fifo_data(&bma456_dev);
	if(fifo_len > 0)
	{
		uint16_t n_instances = sizeof(fifo_buff) / 7;
		while(n_instances == sizeof(fifo_buff) / 7) 
		{
			bma4_extract_accel(bma4_fifo_data, &n_instances, &bma456_dev);
		}
		
		bma4_data.x = bma4_fifo_data[fifo_len/7-1].x;
		bma4_data.y = bma4_fifo_data[fifo_len/7-1].y;
		bma4_data.z = bma4_fifo_data[fifo_len/7-1].z;
	}
	return fifo_len;
}
#endif

/* 信号中断处理 */
static void gpiote_in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	uint16_t int_status = 0;
//	collapse_sw_spi_cfg();
	bma456_read_int_status(&int_status, &bma456_dev);
	if(pin == ANY_MOTION_INT_PIN && action == NRF_GPIOTE_POLARITY_LOTOHI)
	{
#if (BMA456_USE_FIFO == 0)
		LIGHT_3_ON();
		sens_data.temp_c = collapse_read_temperature();
		bma4_read_accel_xyz(&bma4_data, &bma456_dev);
		bma456_d_a_convert();
		LIGHT_3_OFF();
#endif
		
		bma456_evt = int_status;
		nrf_drv_gpiote_in_event_disable(ANY_MOTION_INT_PIN);
		
		if(bma456_evt & BMA456_ANY_NO_MOTION_INT)
		{
			if(wait_trigger_flag == 1 && collapse_obj.state == COLLAPSE_IDLE)
			{
				wait_trigger_flag = 0;
				collapse_state = COLLAPSE_ACTIVE;
			}
		}
	}
	collapse_sw_spi_cfg_low_power();
}

static void bma456_evt_handler(void)
{
	if(bma456_evt != 0)
	{
//		collapse_sw_spi_cfg();
#if (BMA456_USE_FIFO == 1)
		sens_data.temp_c = collapse_read_temperature();
		bma4_set_advance_power_save(BMA4_DISABLE, &bma456_dev);
		nrf_delay_ms(1);
		bma456_read_fifo();
		bma4_set_advance_power_save(BMA4_ENABLE, &bma456_dev);
		nrf_delay_ms(1);
		if((bma456_evt & BMA4_FIFO_FULL_INT) || (bma456_evt & BMA4_FIFO_WM_INT))
		{
			nrf_drv_gpiote_in_event_enable(ANY_MOTION_INT_PIN, true);
		}
#endif
		
		if(bma456_evt & BMA456_ANY_NO_MOTION_INT)
		{
#if (BMA456_USE_FIFO == 1)
			LIGHT_3_ON();
			bma456_d_a_convert();
			LIGHT_3_OFF();
#endif
			swt_mod_t* timer = swt_get_handle();
			timer->collapse_fifo_in->start(COLLAPSE_FIFO_IN_PERIOD);
		}

		bma456_evt = 0;
		uint16_t status = 0;
		bma456_read_int_status(&status, &bma456_dev);
		collapse_sw_spi_cfg_low_power();
	}
}

/* 外部中断配置 */
static void signal_ext_int_cfg(void)
{
	nrfx_gpiote_in_config_t nrfx_gpiote_in_config;
	
	if (!nrf_drv_gpiote_is_init())
	{
		uint32_t err_code;
		err_code = nrf_drv_gpiote_init();
		APP_ERROR_CHECK(err_code);
	}	
	
	nrfx_gpiote_in_config.sense = NRF_GPIOTE_POLARITY_LOTOHI;
	nrfx_gpiote_in_config.is_watcher = false;
	nrfx_gpiote_in_config.pull = NRF_GPIO_PIN_NOPULL;
	nrfx_gpiote_in_config.hi_accuracy = true;
	nrf_drv_gpiote_in_init(ANY_MOTION_INT_PIN,
						   &nrfx_gpiote_in_config,
						   &gpiote_in_pin_handler);
	
	nrf_drv_gpiote_in_event_enable(ANY_MOTION_INT_PIN, true);
}

static void signal_ext_cfg_default(void)
{
	nrfx_gpiote_in_event_disable(ANY_MOTION_INT_PIN);
	nrfx_gpiote_in_uninit(ANY_MOTION_INT_PIN);
}

static uint16_t collapse_config_feature(void)
{	
	struct bma4_int_pin_config int_pin_config = {0};
	int_pin_config.edge_ctrl = BMA4_EDGE_TRIGGER;
	int_pin_config.lvl = BMA4_ACTIVE_HIGH;
	int_pin_config.od = BMA4_PUSH_PULL;
	int_pin_config.output_en = BMA4_OUTPUT_ENABLE;
	int_pin_config.input_en = BMA4_INPUT_DISABLE;
	
	uint16_t ret = bma4_set_int_pin_config(&int_pin_config, 0, &bma456_dev);
	memset(&int_pin_config, 0X00, sizeof(int_pin_config));
	nrf_delay_ms(1);
	
	bma4_set_interrupt_mode(BMA4_LATCH_MODE, &bma456_dev);
	nrf_delay_ms(1);

	/* Enable/select the no-motion feature */
	ret |= bma456_feature_enable(BMA456_NO_MOTION, BMA4_DISABLE, &bma456_dev);
	nrf_delay_ms(1);
	
	/*Enable the axis as per requirement for Any/no-motion. Here all axis has been enabled */
	ret |= bma456_anymotion_enable_axis(BMA456_ALL_AXIS_EN, &bma456_dev);
	nrf_delay_ms(1);
	
	struct bma456_anymotion_config anymotion_config = {0};
	/* Slope threshold value for Any-motion No-motion detection in 5.11g format.
	   Range is 0 to 1g. Default value is 0xAA(170) = 83mg 
	   1LSB = 1000mg / 2^11 = 0.48828125mg */
	anymotion_config.threshold = (uint16_t)(collapse_obj.accel_slope_threshold * pow(2, 11) / 1000.0);
	/* Defines the number of consecutive data points for which the threshold10
	   condition must be respected, for interrupt assertion.
	   It is expressed in in 50 Hz samples (20ms). Range is 0 to 163sec. Default value is 5=100ms.  
	   1LSB = 1000ms / freq */
	anymotion_config.duration = collapse_obj.consecutive_data_points; //time = 1000ms / accel_conf.odr * duration
	anymotion_config.nomotion_sel = 0;
	ret |= bma456_set_any_motion_config(&anymotion_config, &bma456_dev);
	nrf_delay_ms(1);
	
	/* Enable/select the Any-motion feature */
	ret |= bma456_feature_enable(BMA456_ANY_MOTION, BMA4_ENABLE, &bma456_dev);
	nrf_delay_ms(1);
	
	/* Map the motion interupt to INT pin1 */
	ret |= bma456_map_interrupt(BMA4_INTR1_MAP, BMA456_ANY_NO_MOTION_INT, BMA4_ENABLE, &bma456_dev);
	nrf_delay_ms(1);
	
#if (BMA456_USE_FIFO == 1)
	ret |= bma4_map_interrupt(BMA4_INTR1_MAP, BMA4_FIFO_WM_INT, BMA4_ENABLE, &bma456_dev);
	nrf_delay_ms(1);
	
	ret |= bma4_map_interrupt(BMA4_INTR1_MAP, BMA4_FIFO_FULL_INT, BMA4_ENABLE, &bma456_dev);
	nrf_delay_ms(1);
#endif
	
	signal_ext_int_cfg();
	
	return ret;
}

/* 崩塌计任务停止处理 */
__weak void collapse_task_stop_handler(void* param){};
static void collapse_task_start(void)
{
	if(collapse_obj.state == COLLAPSE_IDLE)
	{
		collapse_obj.lpm_obj->task_set_stat(COLLAPSE_TASK_ID, LPM_TASK_STA_RUN);
		collapse_state = COLLAPSE_ACTIVE;
		collapse_obj.state = COLLAPSE_ACTIVE;
	}
}

static void collapse_task_stop(void)
{
	if(collapse_obj.state != COLLAPSE_IDLE)
	{
		collapse_state = COLLAPSE_STOP;
	}
}

void swt_collapse_fifo_out_cb(void)
{
	fifo_mtx = 0;
	if(collapse_obj.state == COLLAPSE_ACTIVE)
	{
		collapse_state = COLLAPSE_ACTIVE;
	}
}

void swt_collapse_fifo_in_cb(void)
{
	nrf_drv_gpiote_in_event_enable(ANY_MOTION_INT_PIN, true);
	uint16_t status = 0;
//	collapse_sw_spi_cfg();
	bma456_read_int_status(&status, &bma456_dev);
	collapse_sw_spi_cfg_low_power();
}

static uint16_t collapse_set_high_odr(uint8_t odr)
{
	uint16_t ret = bma4_set_accel_enable(0, &bma456_dev);
	nrf_delay_ms(1);
	accel_conf.perf_mode = 1;
	accel_conf.odr = odr;
	accel_conf.range = BMA4_ACCEL_RANGE_2G;
	accel_conf.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
	ret |= bma4_set_accel_config(&accel_conf, &bma456_dev);
	nrf_delay_ms(1);
	ret |= bma4_set_accel_enable(1, &bma456_dev);
	nrf_delay_ms(1);
	return ret;
}

static uint16_t collapse_set_low_odr(uint8_t odr)
{
	uint16_t ret = bma4_set_accel_enable(0, &bma456_dev);
	nrf_delay_ms(1);
	accel_conf.perf_mode = 0;
	accel_conf.odr = odr;
	accel_conf.range = BMA4_ACCEL_RANGE_2G;
	accel_conf.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
	ret |= bma4_set_accel_config(&accel_conf, &bma456_dev);
	nrf_delay_ms(1);
	ret |= bma4_set_advance_power_save(BMA4_ENABLE, &bma456_dev);
	nrf_delay_ms(1);
	ret |= bma4_set_accel_enable(1, &bma456_dev);
	nrf_delay_ms(1);
	return ret;
}

void bma456_da_convert(void)
{
	/* Get LSB per bit given the range and resolution */
	float lsb_per_mg = pow(2, accel_conf.range+2) / pow(2, bma456_dev.resolution) * 1000;
	uint16_t max_accel_value = pow(2, bma456_dev.resolution) / pow(2, accel_conf.range+1);
	bma4_data.x = (bma4_data.x > max_accel_value) ? (max_accel_value) : (bma4_data.x);
	bma4_data.y = (bma4_data.y > max_accel_value) ? (max_accel_value) : (bma4_data.y);
	bma4_data.z = (bma4_data.z > max_accel_value) ? (max_accel_value) : (bma4_data.z);
	
	sens_data.accel.x_accel = lsb_per_mg * bma4_data.x;
	sens_data.accel.y_accel = lsb_per_mg * bma4_data.y;
	sens_data.accel.z_accel = lsb_per_mg * bma4_data.z;
	bma456_x_accel_to_angle_convert();
	bma456_y_accel_to_angle_convert();
	bma456_z_accel_to_angle_convert();
}

static float convert_to_float(void* value) {
	return *(float *)(value);
}

static void collapse_period_data_filter(void)
{
//	collapse_sw_spi_cfg();
	LIGHT_3_ON();
	sens_data.temp_c = collapse_read_temperature();
	collapse_set_high_odr(BMA4_OUTPUT_DATA_RATE_200HZ);
	memset(&bma4_data, 0X00, sizeof(bma4_data));
	do
	{
		bma4_read_accel_xyz(&bma4_data, &bma456_dev);
	}while(bma4_data.x==0 || bma4_data.y==0 || bma4_data.z==0);
	nrf_delay_ms(10);
	
	uint8_t delay_ms = 1000 / 200 + 1;
	for(int i = 0; i < FILTER_DATA_BUF_SIZE; i++)
	{
		bma4_read_accel_xyz(&bma4_data, &bma456_dev);
		bma456_da_convert();
		
		if(i > 0)
		{
			sens_angle[i].x_angle = sens_data.angle.x_angle * filter_ratio_x + (1 - filter_ratio_x) * sens_angle[i-1].x_angle;
			sens_angle[i].y_angle = sens_data.angle.y_angle * filter_ratio_y + (1 - filter_ratio_y) * sens_angle[i-1].y_angle;
			sens_angle[i].z_angle = sens_data.angle.z_angle * filter_ratio_z + (1 - filter_ratio_z) * sens_angle[i-1].z_angle;
		}
		else
		{
			sens_angle[i].x_angle = sens_data.angle.x_angle;
			sens_angle[i].y_angle = sens_data.angle.y_angle;
			sens_angle[i].z_angle = sens_data.angle.z_angle;
		}
		
		nrf_delay_ms(delay_ms);
	}
	
	collapse_set_low_odr(BMA4_OUTPUT_DATA_RATE_6_25HZ);
	LIGHT_3_OFF();
	collapse_sw_spi_cfg_low_power();
	
	float data_buf[FILTER_DATA_BUF_SIZE];
	for(int i = 0; i < FILTER_DATA_BUF_SIZE; i++)
	{
		data_buf[i] = sens_angle[i].x_angle;
	}
	data_sort(data_buf, FILTER_DATA_BUF_SIZE, 4, 0);
	uint8_t filter_nums_tmp = filter_nums;
	for(int i = filter_nums/2; i < FILTER_DATA_BUF_SIZE/2; i++)
	{
		if(data_buf[FILTER_DATA_BUF_SIZE-i]-data_buf[i] > filter_delta_x)
		{
			filter_nums_tmp += 2;
		}
	}
	(filter_nums_tmp >= FILTER_DATA_BUF_SIZE) ? (filter_nums_tmp=FILTER_DATA_BUF_SIZE-6) : (1);
	sens_data.angle.x_angle = filter_average(&data_buf[filter_nums_tmp/2], FILTER_DATA_BUF_SIZE-filter_nums_tmp, 4, convert_to_float);
	
	for(int i = 0; i < FILTER_DATA_BUF_SIZE; i++)
	{
		data_buf[i] = sens_angle[i].y_angle;
	}
	data_sort(data_buf, FILTER_DATA_BUF_SIZE, 4, 0);
	filter_nums_tmp = filter_nums;
	for(int i = filter_nums/2; i < FILTER_DATA_BUF_SIZE/2; i++)
	{
		if(data_buf[FILTER_DATA_BUF_SIZE-i]-data_buf[i] > filter_delta_y)
		{
			filter_nums_tmp += 2;
		}
	}
	(filter_nums_tmp >= FILTER_DATA_BUF_SIZE) ? (filter_nums_tmp=FILTER_DATA_BUF_SIZE-6) : (1);
	sens_data.angle.y_angle = filter_average(&data_buf[filter_nums_tmp/2], FILTER_DATA_BUF_SIZE-filter_nums_tmp, 4, convert_to_float);
	
	for(int i = 0; i < FILTER_DATA_BUF_SIZE; i++)
	{
		data_buf[i] = sens_angle[i].z_angle;
	}
	data_sort(data_buf, FILTER_DATA_BUF_SIZE, 4, 0);
	filter_nums_tmp = filter_nums;
	for(int i = filter_nums/2; i < FILTER_DATA_BUF_SIZE/2; i++)
	{
		if(data_buf[FILTER_DATA_BUF_SIZE-i]-data_buf[i] > filter_delta_z)
		{
			filter_nums_tmp += 2;
		}
	}
	(filter_nums_tmp >= FILTER_DATA_BUF_SIZE) ? (filter_nums_tmp=FILTER_DATA_BUF_SIZE-6) : (1);
	sens_data.angle.z_angle = filter_average(&data_buf[filter_nums_tmp/2], FILTER_DATA_BUF_SIZE-filter_nums_tmp, 4, convert_to_float);
	easy_fifo_write((uint8_t*)&sens_data, sizeof(sens_data));
//	printf("%f,%f,%f,",sens_data.angle.x_angle,sens_data.angle.y_angle,sens_data.angle.z_angle);
//	bma4_read_accel_xyz(&bma4_data, &bma456_dev);
//	bma456_d_a_convert();
//	printf("%f,%f,%f\n",sens_data.angle.x_angle,sens_data.angle.y_angle,sens_data.angle.z_angle);
//	nrf_delay_ms(2000);
}

static void collapse_operate(void)
{
	bma456_evt_handler();
	
	if(collapse_state == COLLAPSE_ACTIVE)
	{
		collapse_state = COLLAPSE_IDLE;
		collapse_obj.state = COLLAPSE_ACTIVE;
		
		if(collapse_obj.mode == TRIGGER_MODE && fifo_mtx == 1)
		{
			collapse_obj.lpm_obj->task_set_stat(COLLAPSE_TASK_ID, LPM_TASK_STA_LP);
			return;
		}
		
		collapse_obj.lpm_obj->task_set_stat(COLLAPSE_TASK_ID, LPM_TASK_STA_RUN);
		if(collapse_obj.mode == PERIOD_MODE)
		{
			collapse_enable(1);
			collapse_period_data_filter();
			collapse_enable(0);
		}
		
		sens_data_t sens_data_tmp;
		if(easy_fifo_read((uint8_t*)&sens_data_tmp, sizeof(sens_data_t)) == true)
		{
			memcpy(&collapse_obj.data, &sens_data_tmp, sizeof(sens_data_t));
			collapse_obj.update_flag = 1;
			if(collapse_obj.mode == TRIGGER_MODE)
			{
				fifo_mtx = 1;
				swt_mod_t* timer = swt_get_handle();
				timer->collapse_fifo_out->start(collapse_obj.period*1000);
			}
			collapse_task_stop_handler((void*)&collapse_obj.state);
		}
		else
		{
			wait_trigger_flag = 1;
			collapse_task_stop_handler(NULL);
		}
		
		collapse_obj.state = COLLAPSE_IDLE;
		collapse_obj.lpm_obj->task_set_stat(COLLAPSE_TASK_ID, LPM_TASK_STA_STOP);
	}
	else if(collapse_state == COLLAPSE_STOP)
	{
		fifo_mtx = 0;
		collapse_state = COLLAPSE_IDLE;
		collapse_obj.state = COLLAPSE_STOP;
		swt_mod_t* timer = swt_get_handle();
		timer->collapse_fifo_out->stop();
		timer->collapse_fifo_in->stop();
		collapse_task_stop_handler((void*)&collapse_obj.state);
		collapse_obj.state = COLLAPSE_IDLE;
		collapse_obj.lpm_obj->task_set_stat(COLLAPSE_TASK_ID, LPM_TASK_STA_STOP);
	}
}

void collapse_iot_set_period(void)
{
	sys_param_t* param = sys_param_get_handle();
	collapse_obj.period = param->iot_collapse.iot_period;
}

uint16_t collapse_iot_set_accel_slope_threshold(void)
{
//	collapse_cfg();
	sys_param_t* param = sys_param_get_handle();
	collapse_obj.accel_slope_threshold = param->iot_collapse.iot_accel_slope_threshold;
	struct bma456_anymotion_config anymotion_config = {0};
	anymotion_config.threshold = (uint16_t)(collapse_obj.accel_slope_threshold * pow(2, 11) / 1000.0);
	anymotion_config.duration = collapse_obj.consecutive_data_points;
	anymotion_config.nomotion_sel = 0;
	uint16_t ret = bma456_set_any_motion_config(&anymotion_config, &bma456_dev);
	nrf_delay_ms(1);
	collapse_cfg_low_power();
	return ret;
}

uint16_t collapse_iot_set_consecutive_data_points(void)
{
//	collapse_cfg();
	sys_param_t* param = sys_param_get_handle();
	collapse_obj.consecutive_data_points = param->iot_collapse.iot_consecutive_data_points;
	struct bma456_anymotion_config anymotion_config = {0};
	anymotion_config.threshold = (uint16_t)(collapse_obj.accel_slope_threshold * pow(2, 11) / 1000.0);
	anymotion_config.duration = collapse_obj.consecutive_data_points;
	anymotion_config.nomotion_sel = 0;
	uint16_t ret = bma456_set_any_motion_config(&anymotion_config, &bma456_dev);
	nrf_delay_ms(1);
	collapse_cfg_low_power();
	return ret;
}

uint16_t collapse_iot_set_mode(void)
{
	uint16_t ret = 0;

	sys_param_t* param = sys_param_get_handle();
	collapse_obj.mode = (sens_mode_t)param->iot_collapse.iot_mode;
	
	if(collapse_obj.mode == PERIOD_MODE)
	{
//		collapse_cfg();
		
		/* 关闭硬件中断 */
		signal_ext_cfg_default();
		
		ret = bma4_set_advance_power_save(BMA4_DISABLE, &bma456_dev);
		nrf_delay_ms(1);
		
		/* 关闭所有轴的feature功能 */
		ret |= bma456_anymotion_enable_axis(BMA456_ALL_AXIS_DIS, &bma456_dev);
		nrf_delay_ms(1);
		
		/* 关闭any_motion功能 */
		ret |= bma456_feature_enable(BMA456_ANY_MOTION, BMA4_DISABLE, &bma456_dev);
		nrf_delay_ms(1);
		
		/* 关闭中断功能 */
		ret |= bma456_map_interrupt(BMA4_INTR1_MAP, BMA456_ANY_NO_MOTION_INT, BMA4_DISABLE, &bma456_dev);
		nrf_delay_ms(1);
		
#if (BMA456_USE_FIFO == 1)
		ret |= bma4_map_interrupt(BMA4_INTR1_MAP, BMA4_FIFO_WM_INT, BMA4_DISABLE, &bma456_dev);
		nrf_delay_ms(1);
		
		ret |= bma4_map_interrupt(BMA4_INTR1_MAP, BMA4_FIFO_FULL_INT, BMA4_DISABLE, &bma456_dev);
		nrf_delay_ms(1);
		
		/* 关闭FIFO */
		ret |= bma4_set_fifo_config((BMA4_FIFO_STOP_ON_FULL | BMA4_FIFO_ACCEL | BMA4_FIFO_HEADER), BMA4_DISABLE, &bma456_dev);
		nrf_delay_ms(1);
#endif
		
		ret |= bma4_set_advance_power_save(BMA4_ENABLE, &bma456_dev);
		nrf_delay_ms(1);
		
		collapse_cfg_low_power();
		collapse_obj.period = param->iot_collapse.iot_sample_period;
	}
	else if(collapse_obj.mode == TRIGGER_MODE)
	{
//		collapse_cfg();
		collapse_enable(1);
		
		/* 配置特征 */
		ret |= collapse_config_feature();
		
		/* 配置FIFO缓存 */
#if (BMA456_USE_FIFO == 1)
		ret |= collapse_config_fifo_buffer();
#endif
		
		collapse_cfg_low_power();
		collapse_obj.period = param->iot_collapse.iot_trigger_period;
	}
	
	return ret;
}

/* collapse初始化 */
collapse_obj_t* collapse_init(lpm_obj_t* lpm_obj)
{
	sys_param_t* param = sys_param_get_handle();
	collapse_obj.update_flag = 0;
	collapse_obj.mode = (sens_mode_t)param->iot_collapse.iot_mode;
	collapse_obj.period = param->iot_collapse.iot_period;
	collapse_obj.accel_slope_threshold = param->iot_collapse.iot_accel_slope_threshold;
	collapse_obj.consecutive_data_points = param->iot_collapse.iot_consecutive_data_points;
	
	if(param->object_version == INCLINOMETER_VERSION)
	{
		collapse_obj.mode = PERIOD_MODE;
	}
	
	collapse_obj.state = COLLAPSE_IDLE;
	collapse_obj.lpm_obj = lpm_obj;
	collapse_obj.task_start = collapse_task_start;
	collapse_obj.task_stop = collapse_task_stop;
	collapse_obj.task_operate = collapse_operate;
	
	collapse_obj.iot_set_mode = collapse_iot_set_mode;
	collapse_obj.iot_set_period = collapse_iot_set_period;
	collapse_obj.iot_set_accel_slope_threshold = collapse_iot_set_accel_slope_threshold;
	collapse_obj.iot_set_consecutive_data_points = collapse_iot_set_consecutive_data_points;
	
	collapse_obj.lpm_obj->task_reg(COLLAPSE_TASK_ID);
	
	collapse_cfg(); 									//使能SPI接口
	uint16_t ret = collapse_reset(); 					//复位设备
	ret |= collapse_init_accelerometer(); 				//初始化加速度计
	ret |= collapse_config_accelerometer(); 			//配置加速度计
	
	if(collapse_obj.mode == TRIGGER_MODE)
	{
		collapse_enable(1);
		ret |= collapse_config_feature(); 				//配置特征
#if (BMA456_USE_FIFO == 1)
		ret |= collapse_config_fifo_buffer(); 			//配置FIFO缓存
#endif
	}

	collapse_cfg_low_power();							//失能SPI接口
	
	return &collapse_obj;
}







