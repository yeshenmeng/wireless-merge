#ifndef __SW_TIMER_RTC_H__
#define __SW_TIMER_RTC_H__
#include "main.h"
#include "app_timer.h"


typedef struct
{
	uint8_t timer_id;
	uint32_t process_id;
	app_timer_mode_t mode;
	app_timer_timeout_handler_t timeout_cb;
	/* time:基于RTC唤醒定时器tick(CFG_TS_TICK_VAL:500us)的定时时间500us-32.7675s 
	   取决于异步与同步分配值设置 */
	void (*start)(uint32_t time); 
	void (*stop)(void);
	void (*del)(void);
}sw_timer_t;

typedef struct {
	sw_timer_t* sys_idle;
	sw_timer_t* sys_low_power;
	sw_timer_t* ble_adv;
	sw_timer_t* ble_adv_led;
	sw_timer_t* lora_task_time_slice;
	sw_timer_t* lora_tx_timeout;
	sw_timer_t* lora_idle;
	sw_timer_t* signal_detect_time_slice;
	sw_timer_t* bat_soc;
	sw_timer_t* collapse_fifo_in;
	sw_timer_t* collapse_fifo_out;
}swt_mod_t;

void swt_init(void);
swt_mod_t* swt_get_handle(void);


#endif



