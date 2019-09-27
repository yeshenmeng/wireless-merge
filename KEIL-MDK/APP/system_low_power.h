#ifndef __SYSTEM_LOW_POWER_H__
#define __SYSTEM_LOW_POWER_H__
#include "main.h"
#include "low_power_manage.h"


typedef enum {
	SLP_IDLE,
	SLP_ACTIVE,
	SLP_TIMEOUT,
	SLP_STOP,
}slp_state_t;

typedef struct {
	uint32_t sleep_time;
	slp_state_t state;
	
	lpm_obj_t* lpm_obj;
	
	void (*task_start)(void);
	void (*task_stop)(void);
	void (*task_operate)(void);
}slp_obj_t;

slp_obj_t* slp_task_init(lpm_obj_t* lpm_obj);

#endif


