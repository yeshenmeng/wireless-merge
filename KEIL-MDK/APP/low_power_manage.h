#ifndef __LOW_POWER_MANAGE_H__
#define __LOW_POWER_MANAGE_H__
#include "main.h"
#include "sw_timer_rtc.h"


#define RTC_STATUS_GET() 					(NRF_RTC1->INTENSET != 0)

#define LPM_TASK_STA_INVALID				0
#define LPM_TASK_STA_STOP					1
#define LPM_TASK_STA_RUN					2
#define LPM_TASK_STA_LP						3
#define LPM_TASK_NULL						4
#define LPM_TASK_REGISTER					5
#define LPM_TASK_MAX_NUMBER					10

#define IS_LPM_TASK_STATE(status) 			(((status) == LPM_TASK_STA_STOP) || \
											((status) == LPM_TASK_STA_RUN)  || \
											((status) == LPM_TASK_STA_LP))

typedef void (*lpm_handler_t)(void);

typedef struct {
	uint8_t task_id;
	uint8_t task_stat;
}lpm_task_t;

typedef enum {
	RTC_STOP = 0,
	RTC_RUN = 1,
}rtc_stat_t;

typedef struct
{
	ErrorStatus (*task_reg)(uint8_t task_id);
	ErrorStatus (*task_cancel_reg)(uint8_t task_id);
	ErrorStatus (*task_set_stat)(uint8_t task_id, uint8_t state);
	uint8_t (*task_qry_stat)(uint8_t task_id);
	FlagStatus (*is_task_run)(void);
	
	void (*task_operate)(lpm_handler_t enter, lpm_handler_t exit);
}lpm_obj_t;

lpm_obj_t* lpm_init(void);

#endif




