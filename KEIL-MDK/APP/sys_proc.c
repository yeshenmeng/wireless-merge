#include "sys_proc.h"
#include "low_power_manage.h"
#include "sys_param.h"
#include "light.h"
#include "string.h"
#include "sw_bat_soc.h"
#include "signal_detect.h"
#include "nrf_drv_gpiote.h"
#include "lora_lost_rate_test.h"


#define CHECK_TASK_EVT(evt)			(sys_task_evt & evt)
#define SET_TASK_EVT(evt)			(sys_task_evt |= evt)
#define CLEAR_TASK_EVT(evt)			(sys_task_evt &= ~evt)

typedef struct {
	sys_task_t task;
}sys_obj_t;

static uint8_t lora_disconn = 0;
static uint32_t sys_task_evt;
static sys_obj_t sys_obj;

void sys_task_init(sys_task_t* task)
{
	sys_obj.task.ble_task = task->ble_task;
	sys_obj.task.lora_task = task->lora_task;
	sys_obj.task.slp_task = task->slp_task;
	sys_obj.task.collapse_task = task->collapse_task;
	sys_obj.task.signal_detect_task = task->signal_detect_task;
}

void sys_startup(void)
{
#if (SIGNAL_DET_SW == 1)
	SET_TASK_EVT(SYS_TASK_EVT_BLE);
#endif
	
	sys_param_t* param = sys_param_get_handle();
	if(*(uint64_t*)param->dev_gateway_addr!=0 &&
	   *(uint64_t*)param->dev_gateway_addr!=(~0))
	{
		SET_TASK_EVT(SYS_TASK_EVT_LORA);
	}
	else
	{
#if (SIGNAL_DET_HW == 1)
		SET_TASK_EVT(SYS_TASK_EVT_BLE);
#endif
	}
	
	SET_TASK_EVT(SYS_TASK_EVT_BLE);
//	SET_TASK_EVT(SYS_TASK_EVT_SYS_LP);
}

void sys_task_schd(void)
{
	if(CHECK_TASK_EVT(SYS_TASK_EVT_INIT))
	{
		CLEAR_TASK_EVT(SYS_TASK_EVT_INIT);
	}
	
	if(CHECK_TASK_EVT(SYS_TASK_EVT_BLE))
	{
		CLEAR_TASK_EVT(SYS_TASK_EVT_BLE);
		sys_obj.task.ble_task->task_start(); //启动蓝牙任务
	}
	
	if(CHECK_TASK_EVT(SYS_TASK_EVT_COLLAPSE))
	{
		CLEAR_TASK_EVT(SYS_TASK_EVT_COLLAPSE);
		sys_obj.task.collapse_task->task_start(); //启动崩塌计任务
	}
	
	if(CHECK_TASK_EVT(SYS_TASK_EVT_LORA))
	{
		CLEAR_TASK_EVT(SYS_TASK_EVT_LORA);
		
		sys_param_t* param = sys_param_get_handle();
		if(*(uint64_t*)param->dev_gateway_addr!=0 &&
		   *(uint64_t*)param->dev_gateway_addr!=(~0))
		{
			sys_obj.task.lora_task->task_start(); //启动LORA任务
		}
	}
	
	if(CHECK_TASK_EVT(SYS_TASK_EVT_SYS_LP))
	{
		CLEAR_TASK_EVT(SYS_TASK_EVT_SYS_LP);
		sys_obj.task.slp_task->task_start(); //启动系统低功耗任务
	}
	
	if(CHECK_TASK_EVT(SYS_TASK_EVT_SIGNAL_DET))
	{
		CLEAR_TASK_EVT(SYS_TASK_EVT_SIGNAL_DET);
		sys_obj.task.signal_detect_task->task_start(); //启动软件信号检测任务
	}
}

/* 蓝牙任务停止事件 */
void ble_task_stop_handler(void* param)
{
	SET_TASK_EVT(SYS_TASK_EVT_BLE);
	
	if(lora_disconn == 1)
	{
		ble_state_t ble_state = *(ble_state_t*)param;
		
		if(ble_state == BLE_STA_ADV_TIMEOUT)
		{
			lora_disconn = 0;
			sys_param_t* param = sys_param_get_handle();
			uint8_t default_gateway_addr[] = SYS_PARAM_DEV_GATEWAY_ADDR; //设置LORA默认网关地址
			memcpy(param->dev_gateway_addr, default_gateway_addr, sizeof(default_gateway_addr));
			SET_TASK_EVT(SYS_TASK_EVT_LORA);
		}
		else if(ble_state == BLE_STA_ADV_STOP)
		{
			SET_TASK_EVT(SYS_TASK_EVT_LORA);
		}
	}

#if (SIGNAL_DET_SW == 1)	
	SET_TASK_EVT(SYS_TASK_EVT_SIGNAL_DET); //启动软件信号检测任务
#endif
	
#if (SIGNAL_DET_HW == 1)
	SIGNAL_DET_POWER_ENBALE();
	nrf_delay_ms(1); //延时一段时间过滤掉上电时候的脉冲信号
	SIGNAL_LINE_INT_ENABLE();
#endif
	
#if (BAT_SOC_DET_SW == 1)
	sw_bat_soc_mod_t* sw_bat_soc_mod = sw_bat_soc_get_handle();
	sw_bat_soc_mod->ble_run_time_add();
	sw_bat_soc_mod->ble_power_add();
#endif
}

/* 崩塌计任务停止事件 */
void collapse_task_stop_handler(void* param)
{
	sys_param_t* sys_param = sys_param_get_handle();
	if(sys_param->object_version == INCLINOMETER_VERSION)
	{
		/* 周期模式 */
		if(sys_param->iot_clinometer.iot_mode == 0)
		{
			SET_TASK_EVT(SYS_TASK_EVT_LORA);
		}
//		/* 相对阈值模式 */
//		else if(sys_param->iot_mode == 1)
//		{
//			float x_angle = sys_obj.task.inclinometer_task->data.x_angle;
//			float x_angle_base = sys_obj.task.inclinometer_task->data.x_angle_base;
//			float x_angle_threshold = sys_obj.task.inclinometer_task->data.x_angle_threshold;
//			float y_angle = sys_obj.task.inclinometer_task->data.y_angle;
//			float y_angle_base = sys_obj.task.inclinometer_task->data.y_angle_base;
//			float y_angle_threshold = sys_obj.task.inclinometer_task->data.y_angle_threshold;
//			if(x_angle - x_angle_base > x_angle_threshold ||
//			   y_angle - y_angle_base > y_angle_threshold)
//			{
//				SET_TASK_EVT(SYS_TASK_EVT_LORA); //启动LORA传输任务
//			}
//		}
//		/* 绝对阈值模式 */
//		else if(sys_param->iot_mode == 2)
//		{
//			float x_angle = sys_obj.task.inclinometer_task->data.x_angle;
//			float x_angle_threshold = sys_obj.task.inclinometer_task->data.x_angle_threshold;
//			float y_angle = sys_obj.task.inclinometer_task->data.y_angle;
//			float y_angle_threshold = sys_obj.task.inclinometer_task->data.y_angle_threshold;
//			if(x_angle > x_angle_threshold || y_angle > y_angle_threshold)
//			{
//				SET_TASK_EVT(SYS_TASK_EVT_LORA); //启动LORA传输任务
//			}
//		}
	}
	else if(sys_param->object_version == COLLAPSE_VERSION)
	{
		if(*(collapse_state_t*)param == COLLAPSE_ACTIVE)
		{
			SET_TASK_EVT(SYS_TASK_EVT_LORA);
		}
	}
	
#if (BAT_SOC_DET_SW == 1)
	sw_bat_soc_mod_t* sw_bat_soc_mod = sw_bat_soc_get_handle();
	sw_bat_soc_mod->sca_power_add();
#endif
}

/* LORA任务停止事件 */
void lora_task_stop_handler(void* param)
{
	uint8_t lora_state = *(uint8_t*)param;
	
	/* LORA通信测试状态 */
	if(lora_state == LORA_OUT_STATE_TEST)
	{
		llrt_tx_add();
		llrt_lost_rate_calc();
		SET_TASK_EVT(SYS_TASK_EVT_SYS_LP); 
	}
	else if(lora_state == LORA_OUT_STATE_OFFLINE)
	{
		lora_disconn = 1;
		SET_TASK_EVT(SYS_TASK_EVT_BLE);						//启动蓝牙任务重新配置网关
	}
	/* LORA从连接态到未连接态 */
	else if(lora_state == LORA_OUT_STATE_DISCON)
	{
		SET_TASK_EVT(SYS_TASK_EVT_LORA); 					//重新连接网关
	}
	/* LORA从未连接态到连接态 */
	else if(lora_state == LORA_OUT_STATE_LINK)
	{
		SET_TASK_EVT(SYS_TASK_EVT_COLLAPSE); 				//连接网关成功设置崩塌计任务运行
	}
	/* LORA从连接态到连接态 */
	else if(lora_state == LORA_OUT_STATE_CONNECT)
	{
		sys_param_t* param = sys_param_get_handle();
		if(param->object_version == INCLINOMETER_VERSION)
		{
			SET_TASK_EVT(SYS_TASK_EVT_SYS_LP); 
		}
		else if(param->object_version == COLLAPSE_VERSION)
		{
			if(sys_obj.task.collapse_task->mode == TRIGGER_MODE)
			{
				SET_TASK_EVT(SYS_TASK_EVT_COLLAPSE);
			}
			else if(sys_obj.task.collapse_task->mode == PERIOD_MODE)
			{
				SET_TASK_EVT(SYS_TASK_EVT_SYS_LP);
			}
		}
	}
}

/* 硬件信号检测事件 */
void hw_signal_detect_evt_handler(void* param)
{
	SET_TASK_EVT(SYS_TASK_EVT_BLE); //检测到蓝牙信号启动蓝牙任务
}

/* 软件信号检测任务停止事件 */
void sw_signal_detect_task_stop_handler(void* param)
{
	SET_TASK_EVT(SYS_TASK_EVT_BLE); //启动蓝牙任务
}

/* 系统低功耗任务停止事件 */
void slp_task_stop_handler(void* param)
{
#if (BAT_SOC_DET_SW == 1)
	sw_bat_soc_mod_t* sw_bat_soc_mod = sw_bat_soc_get_handle();
	sw_bat_soc_mod->dev_run_time_add();
	sw_bat_soc_mod->standby_power_add();
#endif
	
	if(llrt_status_get() == LLRT_RUN)
	{
		SET_TASK_EVT(SYS_TASK_EVT_LORA);
	}
	else
	{
		SET_TASK_EVT(SYS_TASK_EVT_COLLAPSE); //启动崩塌计任务
	}
}

/* 电池电量任务停止事件 */
void bat_soc_task_stop_handler(void* param)
{
	return;
}














