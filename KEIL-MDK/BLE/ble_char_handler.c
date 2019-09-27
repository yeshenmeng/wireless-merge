#include "ble_char_handler.h"
#include "sys_param.h"
#include "ble_init.h"
#include "sw_timer_rtc.h"
#include "lora_transmission.h"
#include "iot_operate.h"
#include "calendar.h"
#include "wireless_comm_services.h"
#include "string.h"


#define BLE_TX_POWER_WRITE				0x01
#define BLE_ADV_INTERVAL_WRITE			0x02
#define BLE_ADV_TIME_WRITE				0x04
#define BLE_MIN_CONN_INTERVAL_WRITE		0x08
#define BLE_MAX_CONN_INTERVAL_WRITE		0x10
#define BLE_SLAVE_LATENCY_WRITE			0x20
#define BLE_CONN_TIMEOUT_WRITE			0x40

#define LORA_FREQ_WRITE					0x01
#define LORA_POWER_WRITE				0x02
#define LORA_BW_WRITE					0x04
#define LORA_SF_WRITE					0x08
#define LORA_CODE_RATE_WRITE			0x10
#define LORA_PREAMBLE_WRITE				0x20
#define LORA_HEADER_WRITE				0x40
#define LORA_CRC_WRITE					0x80

#define DEV_GATEWAY_ADDR_WRITE			0x01
#define DEV_LONG_ADDR_WRITE				0x02
#define DEV_SHORT_ADDR_WRITE			0x04
#define DEV_TIME_STAMP_READ				0x08

#define VERIFY_DATA(data, range, len)       			\
do                                                      \
{                                                       \
	int32_t _data = 0;									\
	switch(len)											\
	{													\
		case 1:											\
			_data = *(int8_t*)data;						\
			break;										\
		case 2:											\
			_data = *(int16_t*)data;					\
			break;										\
		case 4:											\
			_data = *(int32_t*)data;					\
			break;										\
		default:										\
			return 1;									\
	}													\
    if(_data < range.lower || _data > range.upper)      \
    {                                                   \
        return 1;                              		    \
    }                                                   \
}while(0)

typedef struct {
	int32_t lower;
	int32_t upper;
}range_t;

static uint32_t ble_param_req = 0;
static uint32_t lora_param_req = 0;
static uint32_t dev_param_req = 0;

static range_t ble_tx_power_range = {
	.lower = 0,
	.upper = 8,
};

static range_t ble_adv_interval_range = {
	.lower = 0,
	.upper = 65535,
};

static range_t ble_adv_time_range = {
	.lower = 20,
	.upper = 10240,
};

static range_t ble_min_conn_interval_range = {
	.lower = 15,
	.upper = 4000,
};

static range_t ble_max_conn_interval_range = {
	.lower = 15,
	.upper = 4000,
};

static range_t ble_slave_latency_range = {
	.lower = 0,
	.upper = 499,
};

static range_t ble_conn_timeout_range = {
	.lower = 100,
	.upper = 32000,
};

static range_t lora_freq_range = {
	.lower = 410,
	.upper = 800,
};

static range_t lora_power_range = {
	.lower = -9,
	.upper = 22,
};

static range_t lora_bw_range = {
	.lower = 0,
	.upper = 9,
};

static range_t lora_sf_range = {
	.lower = 5,
	.upper = 12,
};

static range_t lora_code_rate_range = {
	.lower = 1,
	.upper = 4,
};

static range_t lora_preamble_range = {
	.lower = 5,
	.upper = 255,
};

static range_t lora_header_range = {
	.lower = 0,
	.upper = 1,
};

static range_t lora_crc_range = {
	.lower = 0,
	.upper = 1,
};

uint8_t ble_param_tx_power_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, ble_tx_power_range, len);
	ble_param_req |= BLE_TX_POWER_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->ble_tx_power, p_data, len);
	return 0;
}

uint8_t ble_param_adv_interval_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, ble_adv_interval_range, len);
	ble_param_req |= BLE_ADV_INTERVAL_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->ble_adv_interval, p_data, len);
	return 0;
}

uint8_t ble_param_adv_time_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, ble_adv_time_range, len);
	ble_param_req |= BLE_ADV_TIME_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->ble_adv_time, p_data, len);
	return 0;
}

uint8_t ble_param_min_conn_interval_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, ble_min_conn_interval_range, len);
	ble_param_req |= BLE_MIN_CONN_INTERVAL_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->ble_min_conn_interval, p_data, len);
	return 0;
}

uint8_t ble_param_max_conn_interval_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, ble_max_conn_interval_range, len);
	ble_param_req |= BLE_MAX_CONN_INTERVAL_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->ble_max_conn_interval, p_data, len);
	return 0;
}

uint8_t ble_param_slave_latency_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, ble_slave_latency_range, len);
	ble_param_req |= BLE_SLAVE_LATENCY_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->ble_slave_latency, p_data, len);
	return 0;
}

uint8_t ble_param_conn_timeout_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, ble_conn_timeout_range, len);
	ble_param_req |= BLE_CONN_TIMEOUT_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->ble_conn_timeout, p_data, len);
	return 0;
}
	
uint8_t ble_lora_freq_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, lora_freq_range, len);
	lora_param_req |= LORA_FREQ_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->lora_freq, p_data, len);
	return 0;
}

uint8_t ble_lora_power_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, lora_power_range, len);
	lora_param_req |= LORA_POWER_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->lora_power, p_data, len);
	return 0;
}

uint8_t ble_lora_bw_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, lora_bw_range, len);
	lora_param_req |= LORA_BW_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->lora_bw, p_data, len);
	return 0;
}

uint8_t ble_lora_sf_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, lora_sf_range, len);
	lora_param_req |= LORA_SF_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->lora_sf, p_data, len);
	return 0;
}

uint8_t ble_lora_code_rate_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, lora_code_rate_range, len);
	lora_param_req |= LORA_CODE_RATE_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->lora_code_rate, p_data, len);
	return 0;
}

uint8_t ble_lora_preamble_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, lora_preamble_range, len);
	lora_param_req |= LORA_PREAMBLE_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->lora_preamble, p_data, len);
	return 0;
}

uint8_t ble_lora_header_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, lora_header_range, len);
	lora_param_req |= LORA_HEADER_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->lora_header, p_data, len);
	return 0;
}

uint8_t ble_lora_crc_write_handler(uint8_t* p_data, uint16_t len){
	VERIFY_DATA(p_data, lora_crc_range, len);
	lora_param_req |= LORA_CRC_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->lora_crc, p_data, len);
	return 0;
}

void ble_dev_gateway_write_handler(uint8_t* p_data, uint16_t len){
	dev_param_req |= DEV_GATEWAY_ADDR_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->dev_gateway_addr, p_data, len);
}

void ble_dev_long_addr_write_handler(uint8_t* p_data, uint16_t len){
	dev_param_req |= DEV_LONG_ADDR_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->dev_long_addr, p_data, len);
}

void ble_dev_short_addr_write_handler(uint8_t* p_data, uint16_t len){
	dev_param_req |= DEV_SHORT_ADDR_WRITE;
	sys_param_t* param = sys_param_get_handle();
	sys_param_set((uint8_t*)&param->dev_short_addr, p_data, len);
}

void ble_dev_time_stamp_read_handler(uint8_t* p_data, uint16_t* len)
{
	dev_param_req |= DEV_TIME_STAMP_READ;
	calendar_mod_t* calendar = calendar_get_handle();
	uint32_t time_stamp = calendar->get_time_stamp();
	*len = sizeof(time_stamp);
	memcpy(p_data, (uint8_t*)&time_stamp, *len);
}

static void ble_cfg_char_change_handler(void)
{
	if(ble_param_req == 0)
	{
		return;
	}
	
	if(ble_param_req & BLE_TX_POWER_WRITE)
	{
		ble_param_req &= ~BLE_TX_POWER_WRITE;
	}
	
	if(ble_param_req & BLE_ADV_INTERVAL_WRITE)
	{
		ble_param_req &= ~BLE_ADV_INTERVAL_WRITE;
	}

	if(ble_param_req & BLE_ADV_TIME_WRITE)
	{
		ble_param_req &= ~BLE_ADV_TIME_WRITE;
	}

	if(ble_param_req & BLE_MIN_CONN_INTERVAL_WRITE ||
	   ble_param_req & BLE_MAX_CONN_INTERVAL_WRITE ||
	   ble_param_req & BLE_SLAVE_LATENCY_WRITE ||
	   ble_param_req & BLE_CONN_TIMEOUT_WRITE)
	{
		ble_param_req &= ~BLE_MIN_CONN_INTERVAL_WRITE;
		ble_param_req &= ~BLE_MAX_CONN_INTERVAL_WRITE;
		ble_param_req &= ~BLE_SLAVE_LATENCY_WRITE;
		ble_param_req &= ~BLE_CONN_TIMEOUT_WRITE;
		
		sys_param_t* param = sys_param_get_handle();
		ble_conn_params_change(param->ble_min_conn_interval,
							   param->ble_max_conn_interval,
							   param->ble_slave_latency,
							   param->ble_conn_timeout);
		
		swt_mod_t* timer = swt_get_handle();
		timer->ble_adv_led->stop();
		timer->ble_adv_led->start((param->ble_min_conn_interval+param->ble_max_conn_interval)/2);
	}
}

static void lora_cfg_char_change_handler(void)
{
	if(lora_param_req == 0)
	{
		return;
	}
	
	if(lora_param_req & LORA_FREQ_WRITE)
	{
		lora_param_req &= ~LORA_FREQ_WRITE;
	}
	
	if(lora_param_req & LORA_BW_WRITE)
	{
		lora_param_req &= ~LORA_BW_WRITE;
	}

	if(lora_param_req & LORA_SF_WRITE)
	{
		lora_param_req &= ~LORA_SF_WRITE;
	}

	if(lora_param_req & LORA_CODE_RATE_WRITE)
	{
		lora_param_req &= ~LORA_CODE_RATE_WRITE;
	}

	if(lora_param_req & LORA_PREAMBLE_WRITE)
	{
		lora_param_req &= ~LORA_PREAMBLE_WRITE;
	}

	if(lora_param_req & LORA_HEADER_WRITE)
	{
		lora_param_req &= ~LORA_HEADER_WRITE;
	}

	if(lora_param_req & LORA_CRC_WRITE)
	{
		lora_param_req &= ~LORA_CRC_WRITE;
	}		
//	
//	if(lora_param_req & LORA_POWER_WRITE)
//	{
//		lora_param_req &= ~LORA_POWER_WRITE;
//	}

//	if(lora_param_req & LORA_BW_WRITE ||
//	   lora_param_req & LORA_SF_WRITE ||
//	   lora_param_req & LORA_CODE_RATE_WRITE)
//	{
//		lora_param_req &= ~LORA_BW_WRITE;
//		lora_param_req &= ~LORA_SF_WRITE;
//		lora_param_req &= ~LORA_CODE_RATE_WRITE;
//	}

//	if(lora_param_req & LORA_PREAMBLE_WRITE ||
//	   lora_param_req & LORA_HEADER_WRITE ||
//	   lora_param_req & LORA_CRC_WRITE)
//	{
//		lora_param_req &= ~LORA_PREAMBLE_WRITE;
//		lora_param_req &= ~LORA_HEADER_WRITE;
//		lora_param_req &= ~LORA_CRC_WRITE;
//	}
}

static void dev_cfg_char_change_handler(void)
{
	if(dev_param_req == 0)
	{
		return;
	}	
	
	if(dev_param_req & DEV_GATEWAY_ADDR_WRITE ||
	   dev_param_req & DEV_LONG_ADDR_WRITE ||
	   dev_param_req & DEV_SHORT_ADDR_WRITE)
	{
		dev_param_req &= ~DEV_GATEWAY_ADDR_WRITE;
		dev_param_req &= ~DEV_LONG_ADDR_WRITE;
		dev_param_req &= ~DEV_SHORT_ADDR_WRITE;
		sys_param_t* param = sys_param_get_handle();
		iot_write_long_addr(param->dev_long_addr);
		iot_write_short_addr(param->dev_short_addr);
		lora_reset();
		
		/* 对象版本改变后保存参数复位设备 */
		if(param->object_version != param->dev_long_addr[0] &&
		   (param->dev_long_addr[0] == INCLINOMETER_VERSION ||
			param->dev_long_addr[0] == COLLAPSE_VERSION))
		{
			param->object_version = param->dev_long_addr[0];
			param->update_flag = 1;
			param->save_param_to_flash();
			APP_ERROR_CHECK(1);
		}
	}
	
	if(dev_param_req & DEV_TIME_STAMP_READ)
	{
		dev_param_req &= ~DEV_TIME_STAMP_READ;
	}
}

void ble_char_req_handler(void)
{
	ble_cfg_char_change_handler();
	lora_cfg_char_change_handler();
	dev_cfg_char_change_handler();
}













































