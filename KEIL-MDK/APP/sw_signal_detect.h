#ifndef __SW_SIGNAL_DETECT_H__
#define __SW_SIGNAL_DETECT_H__
#include "main.h"
#include "low_power_manage.h"


typedef enum {
	SIGNAL_DET_IDLE,
	SIGNAL_DET_ACTIVE,
	SIGNAL_DET_TIMEOUT,
	SIGNAL_DET_STOP,
}signal_detect_state_t;

typedef struct {
	uint32_t active_time;
	signal_detect_state_t state;
	
	lpm_obj_t* lpm_obj;
	
	void (*task_start)(void);
	void (*task_stop)(void);
	void (*task_operate)(void);
}sw_signal_detect_obj_t;


sw_signal_detect_obj_t* sw_signal_detect_task_init(lpm_obj_t* lpm_obj);


#endif


