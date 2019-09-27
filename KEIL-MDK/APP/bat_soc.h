#ifndef __BAT_SOC_H__
#define __BAT_SOC_H__
#include "main.h"
#include "low_power_manage.h"


typedef enum {
	BAT_SOC_IDLE,
	BAT_SOC_ACTIVE,
	BAT_SOC_TIMEOUT,
	BAT_SOC_STOP,
}bat_soc_state_t;

typedef struct {
	uint8_t update_flag;
	int16_t gas_gauge;
}bat_soc_data_t;

typedef struct {
	uint32_t sleep_time;
	bat_soc_state_t state;
	bat_soc_data_t data;
	
	lpm_obj_t* lpm_obj;
	
	void (*task_start)(void);
	void (*task_stop)(void);
	void (*task_operate)(void);
}bat_soc_obj_t;

bat_soc_obj_t* bat_soc_task_init(lpm_obj_t* lpm_obj);


#endif


