#include "sw_signal_detect.h"
#include "sw_timer_rtc.h"
#include "nrf_drv_gpiote.h"
#include "sys_param.h"


static signal_detect_state_t signal_detect_state;
static sw_signal_detect_obj_t signal_detect_obj;

/* 信号检测任务停止处理 */
__weak void sw_signal_detect_task_stop_handler(void* param){};

static void signal_detect_task_start(void)
{
	if(signal_detect_obj.state == SIGNAL_DET_IDLE)
	{
		signal_detect_obj.lpm_obj->task_set_stat(SIGNAL_DET_TASK_ID, LPM_TASK_STA_RUN);
		signal_detect_state = SIGNAL_DET_ACTIVE;
		signal_detect_obj.state = SIGNAL_DET_ACTIVE;
	}
	return;
}

static void signal_detect_task_stop(void)
{
	if(signal_detect_obj.state != SIGNAL_DET_IDLE)
	{
		signal_detect_state = SIGNAL_DET_STOP;
	}
	return;	
}

void swt_signal_detect_time_slice_cb(void* param)
{
	signal_detect_state = SIGNAL_DET_TIMEOUT;
	return;
}

static void signal_detect_task_operate(void)
{
	if(signal_detect_obj.state == SIGNAL_DET_IDLE)
	{
		return;
	}
	
	swt_mod_t* timer = swt_get_handle();
	switch((uint8_t)signal_detect_state)
	{
		case SIGNAL_DET_ACTIVE:
			signal_detect_state = SIGNAL_DET_IDLE;
			signal_detect_obj.state = SIGNAL_DET_ACTIVE;
			timer->signal_detect_time_slice->start(signal_detect_obj.active_time);
			signal_detect_obj.lpm_obj->task_set_stat(SIGNAL_DET_TASK_ID, LPM_TASK_STA_LP);
			break;
		
		/* 信号检测定时器超时事件 */
		case SIGNAL_DET_TIMEOUT:
		/* 主动停止信号检测任务事件 */
		case SIGNAL_DET_STOP:
			signal_detect_state = SIGNAL_DET_IDLE;
			timer->signal_detect_time_slice->stop();
			signal_detect_obj.state = SIGNAL_DET_IDLE;
			signal_detect_obj.lpm_obj->task_set_stat(SIGNAL_DET_TASK_ID, LPM_TASK_STA_STOP);
			sw_signal_detect_task_stop_handler(NULL); /* 信号检测任务停止处理 */
			break;
	}
	
	return;	
}

sw_signal_detect_obj_t* sw_signal_detect_task_init(lpm_obj_t* lpm_obj)
{
	signal_detect_state = SIGNAL_DET_IDLE;
	signal_detect_obj.state = SIGNAL_DET_IDLE;	
	signal_detect_obj.active_time = SIGNAL_DET_TIME_SLICE;
	
	signal_detect_obj.lpm_obj = lpm_obj;
	
	signal_detect_obj.task_start = signal_detect_task_start;
	signal_detect_obj.task_stop = signal_detect_task_stop;
	signal_detect_obj.task_operate = signal_detect_task_operate;
	
	signal_detect_obj.lpm_obj->task_reg(SIGNAL_DET_TASK_ID);
	
	return &signal_detect_obj;
}







