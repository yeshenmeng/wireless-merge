#include "bluetooth_low_power.h"
#include "sw_timer_rtc.h"
#include "sys_param.h"
#include "light.h"
#include "sw_bat_soc.h"


static uint16_t ble_conn_time = 0;
static uint8_t ble_rssi_flag = 0;
static ble_state_t ble_state;
static ble_obj_t ble_obj;

/* 蓝牙活动停止处理 */
__weak void ble_task_stop_handler(void* param){};
	
/* 蓝牙状态错误处理 */
__weak void ble_stat_err_handler(void* param){};


static void ble_task_start(void)
{
	if(ble_obj.state == BLE_STA_IDLE)
	{
		ble_obj.lpm_obj->task_set_stat(BLE_TASK_ID, LPM_TASK_STA_RUN);
		ble_state = BLE_STA_ADV;
	}
	return;
}

static void ble_task_stop(void)
{
	if(ble_obj.state != BLE_STA_IDLE)
	{
		if(ble_obj.state == BLE_STA_ADV)
		{
			ble_state = BLE_STA_ADV_STOP;
		}
		else if(ble_obj.state == BLE_STA_CONN)
		{
			ble_state = BLE_STA_CONN_ABORT;
		}
	}
	return;
}

static ErrorStatus ble_start_adv(void)
{
	ErrorStatus status = SUCCESS;
	ble_adv_start();
	return status;
}

static ErrorStatus ble_stop_adv(void)
{
	ErrorStatus status = SUCCESS;
	ble_adv_stop();
	return status;
}

static ErrorStatus ble_abort_conn(void)
{
	ErrorStatus status = SUCCESS;
	ble_disconnect();
	return status;
}

static void ble_get_addr(uint8_t* addr)
{
	ble_get_address(addr);
	return;
}

static ErrorStatus ble_set_tx_power(uint8_t level)
{
	ErrorStatus status = SUCCESS;
	ble_tx_power_set(level);
	return status;
}

static void ble_idle_lpm(void)
{
	if(ble_obj.param.is_idle_enter_lp == 0)
	{
		return;
	}
	
	if(ble_obj.state != BLE_STA_IDLE)
	{
		ble_obj.lpm_obj->task_set_stat(BLE_TASK_ID, LPM_TASK_STA_LP); /* 设置任务低功耗状态 */
	}
}

static ErrorStatus ble_read_rssi(int8_t* rssi)
{
	ErrorStatus status = SUCCESS;
	uint8_t* ch_index;
	ble_rssi_get(rssi, ch_index);
	return status;
}

void swt_ble_adv_cb(void)
{
	ble_state = BLE_STA_ADV_TIMEOUT;
	return;
}

void swt_ble_adv_led_cb(void)
{
	if(ble_obj.state == BLE_STA_CONN)
	{
		ble_rssi_flag = 1;
	}
	
	LIGHT_2_TOGGLE();
	return;
}

void ble_conn_handler(void)
{
	ble_state = BLE_STA_CONN;
	return;
}

void ble_disconn_handler(void)
{
	ble_state = BLE_STA_DISCON;
	return;
}

static void ble_task_operate(void)
{
	ErrorStatus status = SUCCESS;
	swt_mod_t* timer = swt_get_handle();
	
	switch((uint8_t)ble_state)
	{
		/* 蓝牙开启广播 */
		case BLE_STA_ADV:
		{
#if (BAT_SOC_DET_SW == 1)
			sw_bat_soc_mod_t* sw_bat_soc_mod = sw_bat_soc_get_handle();
			sw_bat_soc_mod->ble_run_time_start();
#endif
			status = ble_start_adv(); /* 开始蓝牙广播 */
			sys_param_t* param = sys_param_get_handle();
			ble_tx_power_set(param->ble_tx_power);
			if(param->ble_adv_time != 0)
			{
				timer->ble_adv->start(param->ble_adv_time*1000); /* 启动蓝牙广播定时器 */
			}
			timer->ble_adv_led->start(param->ble_adv_interval); /* 启动BLE广播LED指示灯定时器 */
			ble_get_addr(ble_obj.param.address); /* 获取蓝牙地址 */
			ble_obj.state = ble_state;
			ble_state = BLE_STA_IDLE;
			ble_idle_lpm();
			break;
		}
		
		/* 蓝牙连接事件 */
		case BLE_STA_CONN:
		{
			timer->ble_adv->stop(); /* 停止BLE广播定时器 */
			timer->ble_adv_led->stop();
			sys_param_t* param = sys_param_get_handle();
			timer->ble_adv_led->start(ble_conn_time);
			LIGHT_2_ON();
			ble_obj.state = ble_state;
			ble_state = BLE_STA_IDLE;
			break;
		}

		/* 蓝牙被动断开连接事件 */
		case BLE_STA_DISCON:
			timer->ble_adv_led->stop();
			LIGHT_2_OFF();
			ble_obj.state = BLE_STA_IDLE; /* 更新蓝牙状态为未连接 */
//			ble_state = BLE_STA_ADV;
			ble_state = BLE_STA_ADV_STOP;
			ble_obj.lpm_obj->task_set_stat(BLE_TASK_ID, LPM_TASK_STA_RUN); 
			break;

		/* 蓝牙广播超时事件 */
		case BLE_STA_ADV_TIMEOUT:
		{
			status = ble_stop_adv(); /* 停止蓝牙广播 */
		}
		/* 停止蓝牙广播 */
		case BLE_STA_ADV_STOP:
		{
			ble_state_t state_tmp = ble_state;
			timer->ble_adv->stop(); /* 停止BLE广播定时器 */
			timer->ble_adv_led->stop(); /* 停止BLE广播LED指示灯定时器 */
			LIGHT_2_OFF();
			ble_obj.state = BLE_STA_IDLE;
			ble_state = BLE_STA_IDLE;
			ble_obj.lpm_obj->task_set_stat(BLE_TASK_ID, LPM_TASK_STA_STOP);
			ble_task_stop_handler((void *)&state_tmp);
			break;
		}
		
		/* 主动断开连接 */
		case BLE_STA_CONN_ABORT:
			status = ble_obj.abort_conn(); /* 断开蓝牙连接 */
			ble_obj.state = ble_state;
			ble_state = BLE_STA_IDLE;
			break;
	}
	
	if(status != SUCCESS)
	{
		ble_stat_err_handler((void *)&ble_state); /* 状态错误处理 */
	}
	
	if(ble_obj.state == BLE_STA_CONN && ble_rssi_flag == 1)
	{
		ble_rssi_flag = 0;
		ble_read_rssi(&ble_obj.param.rssi); /* 读取蓝牙信号强度 */
	}
}

ble_obj_t* ble_init(lpm_obj_t* lpm_obj)
{
	sys_param_t* param = sys_param_get_handle();
//	ble_conn_time = param->ble_max_conn_interval;
	ble_conn_time = 500;
	
	ble_obj.param.is_idle_enter_lp = ENABLE;
	ble_state = BLE_STA_IDLE;
	ble_obj.state = ble_state;
	
	ble_obj.lpm_obj = lpm_obj;
	
	ble_obj.task_operate = ble_task_operate;
	ble_obj.task_start = ble_task_start;
	ble_obj.task_stop = ble_task_stop;
	ble_obj.abort_conn = ble_abort_conn;
	ble_obj.read_rssi = ble_read_rssi;
	ble_obj.set_tx_power = ble_set_tx_power;
	
	ble_obj.lpm_obj->task_reg(BLE_TASK_ID);
	
	return &ble_obj;
}







