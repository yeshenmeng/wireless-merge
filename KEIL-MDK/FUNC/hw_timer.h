#ifndef __HW_TIMER_H__
#define __HW_TIMER_H__
#include "main.h"


#define HW_TIMER_TICK_MS		1

typedef struct {
	uint16_t milsecond;
	uint32_t second;
	uint32_t sumtime;
}sys_time_t;

void hw_timer_start(void);
void hw_timer_stop(void);
sys_time_t* hw_timer_get_sys_time(void);


#endif


