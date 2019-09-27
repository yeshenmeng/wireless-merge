#ifndef __SW_BAT_SOC_H__
#define __SW_BAT_SOC_H__
#include "main.h"


#define LORA_MIN_POWER_LEVEL		(int)-9
#define LORA_MAX_POWER_LEVEL		(int)22
#define IS_VALID_POWER_LEVEL(x)		((x>=LORA_MIN_POWER_LEVEL)&&(x<=LORA_MAX_POWER_LEVEL))
#define SW_BAT_SOC_FLASH_PAGE_ADDR	ADDR_FLASH_PAGE_81 //电池数据存储区

typedef struct {
	uint8_t data_save_flag;
	uint8_t cap_percent;
	
	uint32_t dev_run_time;
	uint32_t ble_run_time;
	uint32_t lora_tx_fail_times;
	uint32_t lora_tx_success_times;
	
	float actual_cap;
	float rest_cap;
	float ble_total_cap;
	float sca_total_cap;
	float lora_tx_fail_total_cap;
	float lora_tx_success_total_cap;
	float standby_total_cap;
}sw_bat_soc_t;

typedef struct {
	void (*dev_run_time_add)(void);
	void (*ble_run_time_start)(void);
	void (*ble_run_time_add)(void);
	void (*sca_power_add)(void);
	void (*lora_tx_fail_power_add)(void);
	void (*lora_tx_success_power_add)(void);
	void (*standby_power_add)(void);
	void (*ble_power_add)(void);
	uint8_t (*get_update_flag)(void);
}sw_bat_soc_mod_t;

sw_bat_soc_mod_t* sw_bat_soc_init(void);
sw_bat_soc_mod_t* sw_bat_soc_get_handle(void);
uint8_t sw_bat_soc_param_to_flash(void);

#endif








