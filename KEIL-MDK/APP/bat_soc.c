#include "bat_soc.h"
#include "sw_timer_rtc.h"
#include "sys_param.h"
#include "calendar.h"
#include "saadc.h"


static bat_soc_state_t bat_soc_state;
static bat_soc_obj_t bat_soc_obj;

__weak void bat_soc_task_stop_handler(void* param);


static void bat_soc_task_start(void)
{
	if(bat_soc_obj.state == BAT_SOC_IDLE)
	{
		bat_soc_obj.lpm_obj->task_set_stat(BAT_SOC_TASK_ID, LPM_TASK_STA_RUN);
		bat_soc_state = BAT_SOC_ACTIVE;
		bat_soc_obj.state = BAT_SOC_ACTIVE;
	}
}

static void bat_soc_task_stop(void)
{
	if(bat_soc_obj.state != BAT_SOC_IDLE)
	{
		bat_soc_state = BAT_SOC_STOP;
	}
}

void swt_bat_soc_cb(void)
{
	bat_soc_state = BAT_SOC_TIMEOUT;
}

void saadc_sample_cplt_cb(void* param)
{
	bat_soc_state = BAT_SOC_STOP;
}

static void bat_soc_operate(void)
{
	swt_mod_t* timer = swt_get_handle();
	
	switch((uint8_t)bat_soc_state)
	{
		case BAT_SOC_ACTIVE:
		{
			sys_param_t* param = sys_param_get_handle();
			calendar_mod_t* calendar = calendar_get_handle();
			bat_soc_state = BAT_SOC_IDLE;
			bat_soc_obj.state = BAT_SOC_ACTIVE;
			uint32_t time_stamp = calendar->get_time_stamp();
			uint32_t sample_interval = param->iot_clinometer.iot_sample_interval;
			if(param->object_version == COLLAPSE_VERSION)
			{
				sample_interval = param->iot_collapse.iot_period;
			}
			bat_soc_obj.sleep_time = sample_interval - time_stamp % sample_interval;
			(bat_soc_obj.sleep_time>=2) ? (bat_soc_obj.sleep_time-=1) : (1); //提前1秒醒来进行电量计算
			timer->bat_soc->start(bat_soc_obj.sleep_time*1000);
			bat_soc_obj.lpm_obj->task_set_stat(BAT_SOC_TASK_ID, LPM_TASK_STA_LP);
			break;
		}
		
 		/* 电池电量定时器超时事件 */
		case BAT_SOC_TIMEOUT:
		{
			saadc_init();
			saadc_sample_start();
			break;
		}
		
		/* 停止电池电量任务 */
		case BAT_SOC_STOP:
		{
			bat_soc_state_t state_tmp = bat_soc_state;
			bat_soc_state = BAT_SOC_IDLE;
			timer->bat_soc->stop();
			bat_soc_obj.state = BAT_SOC_IDLE;
			saadc_unint();
			bat_soc_obj.data.gas_gauge = saadc_sample_result_get();
			bat_soc_obj.data.update_flag = 1;
			bat_soc_obj.lpm_obj->task_set_stat(BAT_SOC_TASK_ID, LPM_TASK_STA_STOP);
			bat_soc_task_stop_handler((void *)&state_tmp); /* 电池电量任务停止处理 */
			break;
		}
	}
}

bat_soc_obj_t* bat_soc_task_init(lpm_obj_t* lpm_obj)
{
#if (BAT_SOC_DET_HW == 1)
	saadc_init();
	saadc_sample_start();
#endif	
	
	bat_soc_state = BAT_SOC_IDLE;
	bat_soc_obj.state = BAT_SOC_IDLE;	
	bat_soc_obj.sleep_time = 3000;
	bat_soc_obj.data.gas_gauge = 0;
	bat_soc_obj.data.update_flag = 0;
	
	bat_soc_obj.lpm_obj = lpm_obj;
	
	bat_soc_obj.task_start = bat_soc_task_start;
	bat_soc_obj.task_stop = bat_soc_task_stop;
	bat_soc_obj.task_operate = bat_soc_operate;
	
	bat_soc_obj.lpm_obj->task_reg(BAT_SOC_TASK_ID);
	
	return &bat_soc_obj;
}

/* 电池电量任务停止处理 */
__weak void bat_soc_task_stop_handler(void* param)
{
	return;
}

