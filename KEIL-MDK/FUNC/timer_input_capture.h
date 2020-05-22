#ifndef __TIMER_INPUT_CAPTURE_H__
#define __TIMER_INPUT_CAPTURE_H__
#include "main.h"


typedef void (*timer_ic_cplt_cb_t)(uint32_t value);
typedef struct {
	void (*task_start)(uint32_t time);
	void (*task_stop)(void);
	void (*task_init)(uint8_t ic_isr_pin, timer_ic_cplt_cb_t cb);
	void (*task_uninit)(void);
}timer_ic_t;

timer_ic_t* timer_ic_task_init(void);
timer_ic_t* imer_ic_task_get_handle(void);

#endif









