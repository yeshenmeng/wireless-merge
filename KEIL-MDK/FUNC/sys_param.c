#include "sys_param.h"
#include "string.h"
#include "flash.h"


static sys_param_t sys_param;

void sys_param_init(void)
{
	flash_read(SYS_PARAM_FLASH_PAGE_ADDR, (uint8_t *)&sys_param, sizeof(sys_param));
	
	if(sys_param.object_version != INCLINOMETER_VERSION &&
	   sys_param.object_version != COLLAPSE_VERSION)
	{
		sys_param.ble_tx_power = SYS_PARAM_BLE_TX_POWER;
		sys_param.ble_adv_interval = SYS_PARAM_BLE_ADV_INTERVAL;
		sys_param.ble_adv_time = SYS_PARAM_BLE_ADV_TIME;
		sys_param.ble_min_conn_interval = SYS_PARAM_BLE_MIN_CONN_INTERVAL;
		sys_param.ble_max_conn_interval = SYS_PARAM_BLE_MAX_CONN_INTERVAL;
		sys_param.ble_slave_latency = SYS_PARAM_BLE_SLAVE_LATENCY;
		sys_param.ble_conn_timeout = SYS_PARAM_BLE_CONN_TIMEOUT;
		
		sys_param.lora_freq = SYS_PARAM_LORA_FREQ;
		sys_param.lora_power = SYS_PARAM_LORA_POWER;
		sys_param.lora_bw = SYS_PARAM_LORA_BW;
		sys_param.lora_sf = SYS_PARAM_LORA_SF;
		sys_param.lora_code_rate = SYS_PARAM_LORA_CODE_RATE;
		sys_param.lora_preamble = SYS_PARAM_LORA_PREAMBLE;
		sys_param.lora_header = SYS_PARAM_LORA_HEADER;
		sys_param.lora_crc = SYS_PARAM_LORA_CRC;
		
		uint8_t dev_gateway_addr[8] = SYS_PARAM_DEV_GATEWAY_ADDR;
		uint8_t dev_long_addr[8] = SYS_PARAM_DEV_LONG_ADDR;
		uint8_t dev_short_addr[2] = SYS_PARAM_DEV_SHORT_ADDR;
		memcpy(sys_param.dev_gateway_addr,dev_gateway_addr,8);
		memcpy(sys_param.dev_long_addr,dev_long_addr,8);
		memcpy(sys_param.dev_short_addr,dev_short_addr,2);
		
		sys_param.iot_clinometer.iot_mode = SYS_PARAM_IOT_I_MODE;
		sys_param.iot_clinometer.iot_sample_interval = SYS_PARAM_IOT_I_SAMPLE_INTERVAL;
		sys_param.iot_clinometer.iot_x_angle_threshold = SYS_PARAM_IOT_I_X_ANGLE_THRESHOLD;
		sys_param.iot_clinometer.iot_y_angle_threshold = SYS_PARAM_IOT_I_Y_ANGLE_THRESHOLD;
		sys_param.iot_clinometer.iot_z_angle_threshold = SYS_PARAM_IOT_I_Z_ANGLE_THRESHOLD;
		
		sys_param.iot_collapse.iot_mode = SYS_PARAM_IOT_C_MODE;
		sys_param.iot_collapse.iot_sample_period = SYS_PARAM_IOT_C_SAMPLE_PERIOD;
		sys_param.iot_collapse.iot_trigger_period = SYS_PARAM_IOT_C_TRIGGER_PERIOD;
		sys_param.iot_collapse.iot_period = sys_param.iot_collapse.iot_sample_period;
		if(sys_param.iot_collapse.iot_mode == 1)
		{
			sys_param.iot_collapse.iot_period = sys_param.iot_collapse.iot_trigger_period;
		}
		sys_param.iot_collapse.iot_accel_slope_threshold = SYS_PARAM_IOT_C_ACCEL_SLOPE_THRESHOLD;
		sys_param.iot_collapse.iot_consecutive_data_points = SYS_PARAM_IOT_C_CONSECUTIVE_DATA_POINTS;
		
		sys_param.update_flag = 0;
		sys_param.object_version = sys_param.dev_long_addr[0];
		
		flash_write(SYS_PARAM_FLASH_PAGE_ADDR,
				   (uint32_t*)&sys_param,
				    sizeof(sys_param)%4==0?sizeof(sys_param)/4:(sizeof(sys_param)/4+1));
	}
	
	sys_param.update_flag = 0;
	sys_param.save_param_to_flash = sys_save_param_to_flash;
}

uint8_t sys_save_param_to_flash(void)
{
	if(sys_param.update_flag == 1)
	{
		sys_param.update_flag = 0;
		return flash_write(SYS_PARAM_FLASH_PAGE_ADDR,
						  (uint32_t*)&sys_param,
						   sizeof(sys_param)%4==0?sizeof(sys_param)/4:(sizeof(sys_param)/4+1));
	}
	
	return 0;
}

sys_param_t* sys_param_get_handle(void)
{
	return &sys_param;
}

void sys_param_set(uint8_t* param, uint8_t* value, uint8_t len)
{
	for(int i=0; i<len; i++)
	{
		if(param[i] != value[i])
		{
			memcpy(param, value, len);
			sys_param.update_flag = 1;
			break;
		}
	}
}







