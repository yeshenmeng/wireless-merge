#include "system_low_power.h"
#include "sw_timer_rtc.h"
#include "sys_param.h"
#include "calendar.h"
#include "light.h"


static slp_state_t slp_state;
static slp_obj_t slp_obj;

/* 系统低功耗任务停止处理 */
__weak void slp_task_stop_handler(void* param){};


static void slp_task_start(void)
{
	if(slp_obj.state == SLP_IDLE)
	{
		slp_obj.lpm_obj->task_set_stat(SLP_TASK_ID, LPM_TASK_STA_RUN);
		slp_state = SLP_ACTIVE;
		slp_obj.state = SLP_ACTIVE;
	}
}

static void slp_task_stop(void)
{
	if(slp_obj.state != SLP_IDLE)
	{
		slp_state = SLP_STOP;
	}
}

void swt_slp_cb(void)
{
	slp_state = SLP_TIMEOUT;
}

static void slp_operate(void)
{
	swt_mod_t* timer = swt_get_handle();
	
	switch((uint8_t)slp_state)
	{
		case SLP_ACTIVE:
		{
			sys_param_t* param = sys_param_get_handle();
			calendar_mod_t* calendar = calendar_get_handle();
			slp_state = SLP_IDLE;
			slp_obj.state = SLP_ACTIVE;
			uint32_t time_stamp = calendar->get_time_stamp();
			uint32_t sample_interval = param->iot_clinometer.iot_sample_interval;
			if(param->object_version == COLLAPSE_VERSION)
			{
				sample_interval = param->iot_collapse.iot_period;
			}
			slp_obj.sleep_time = sample_interval - time_stamp % sample_interval;
			timer->sys_low_power->start(slp_obj.sleep_time*1000);
			slp_obj.lpm_obj->task_set_stat(SLP_TASK_ID, LPM_TASK_STA_LP);
			break;
		}
		
 		/* 系统低功耗睡眠定时器超时事件 */
		case SLP_TIMEOUT:
		/* 主动停止系统睡眠活动事件 */
		case SLP_STOP:
		{
			slp_state_t state_tmp = slp_state;
			slp_state = SLP_IDLE;
			timer->sys_low_power->stop();
			slp_obj.state = SLP_IDLE;
			slp_obj.lpm_obj->task_set_stat(SLP_TASK_ID, LPM_TASK_STA_STOP);
			slp_task_stop_handler((void *)&state_tmp); /* 系统低功耗任务停止处理 */
			break;
		}
	}
}

slp_obj_t* slp_task_init(lpm_obj_t* lpm_obj)
{
	slp_state = SLP_IDLE;
	slp_obj.state = SLP_IDLE;	
	slp_obj.sleep_time = 3000;
	
	slp_obj.lpm_obj = lpm_obj;
	
	slp_obj.task_start = slp_task_start;
	slp_obj.task_stop = slp_task_stop;
	slp_obj.task_operate = slp_operate;
	
	slp_obj.lpm_obj->task_reg(SLP_TASK_ID);
	
	return &slp_obj;
}






