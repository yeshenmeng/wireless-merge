#include <stdint.h>
#include <string.h>
#include "main.h"
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "app_timer.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_clock.h"
#include "nrf_sdh.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ble_init.h"
#include "low_power_manage.h"
#include "sw_timer_rtc.h"
#include "bluetooth_low_power.h"
#include "rng_lpm.h"
#include "lora_transmission.h"
#include "sys_proc.h"
#include "inclinometer.h"
#include "iotobject.h"
#include "wireless_comm_services.h"
#include "signal_detect.h"
#include "sw_signal_detect.h"
#include "flash.h"
#include "iot_operate.h"
#include "sys_param.h"
#include "calendar.h"
#include "uart_svc.h"
#include "light.h"
#include "ble_char_handler.h"
#include "sw_bat_soc.h"
#include "collapse.h"


#define DEAD_BEEF	0xDEADBEEF	/**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

lpm_obj_t* m_lpm;
ble_obj_t *m_ble;
lora_obj_t* m_lora;
slp_obj_t* m_slp;
inclinometer_obj_t* m_inclinometer;
collapse_obj_t* m_collapse;
sw_signal_detect_obj_t*  m_sw_signal_detect;
wireless_comm_services_t* wireless_comm_svc;
uint32_t sys_run_cnt = 0;

/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/* 初始化软件定时器 */
static void timers_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

/* rtc时钟配置 */
static void lfclk_cfg(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
}

/**
 * @brief  初始化物联网协议属性长度
 * @param  sensor: 指向iot_object_t结构的句柄
 * @retval None
 */
void iot_set_prop(iot_object_t *sensor)
{
	/* init addr, long addr, short addr are fixed. No need to set */
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		sensor->setPropCount(14);
		sensor->setPropLen(IOT_I_SAMPLE_MODE_ID, 1);
		sensor->setPropLen(IOT_I_SAMPLE_INTERVAL_ID, 4);
		sensor->setPropLen(IOT_I_TIME_STAMP_ID, 4);
		sensor->setPropLen(IOT_I_BATTERY_LEVEL_ID, 1);
		sensor->setPropLen(IOT_I_TEMPERATURE_ID, 4);
		sensor->setPropLen(IOT_I_DATA_X_ANGLE_ID, 4);
		sensor->setPropLen(IOT_I_DATA_Y_ANGLE_ID, 4);
		sensor->setPropLen(IOT_I_DATA_Z_ANGLE_ID, 4);
		sensor->setPropLen(IOT_I_DATA_X_THRESHOLD_ID, 4);
		sensor->setPropLen(IOT_I_DATA_Y_THRESHOLD_ID, 4);
		sensor->setPropLen(IOT_I_DATA_Z_THRESHOLD_ID, 4);
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
#if (IOT_PROTOCOL_C_WITH_ANGLE == 0)
		sensor->setPropCount(13);
#elif (IOT_PROTOCOL_C_WITH_ANGLE == 1)
		sensor->setPropCount(16);
#endif
	
		sensor->setPropLen(IOT_C_SAMPLE_MODE_ID, 1);
		sensor->setPropLen(IOT_C_SAMPLE_INTERVAL_ID, 4);
		sensor->setPropLen(IOT_C_TIME_STAMP_ID, 4);
		sensor->setPropLen(IOT_C_ACCEL_SLOPE_THRESHOLD_ID, 2);
		sensor->setPropLen(IOT_C_CONSECUTIVE_DATA_POINTS_ID, 2);
		sensor->setPropLen(IOT_C_BATTERY_LEVEL_ID, 1);
		sensor->setPropLen(IOT_C_TEMPERATURE_ID, 4);
		sensor->setPropLen(IOT_C_DATA_X_ACCEL_ID, 4);
		sensor->setPropLen(IOT_C_DATA_Y_ACCEL_ID, 4);
		sensor->setPropLen(IOT_C_DATA_Z_ACCEL_ID, 4);
	
#if (IOT_PROTOCOL_C_WITH_ANGLE == 1)	
		sensor->setPropLen(IOT_C_DATA_X_ANGLE_ID, 4);
		sensor->setPropLen(IOT_C_DATA_Y_ANGLE_ID, 4);
		sensor->setPropLen(IOT_C_DATA_Z_ANGLE_ID, 4);
#endif
	}
}

/* 进入低功耗时的处理 */
void lpm_enter_handler(void)
{
	if(nrf_sdh_is_enabled() == true)
	{
		sd_power_dcdc_mode_set(0);
//		sd_clock_hfclk_release();
//		uint32_t status = 1;
//		while(status == 1) sd_clock_hfclk_is_running(&status);
	}
	else
	{
		NRF_POWER->DCDCEN = 0; //低功耗关闭DCDC
	}

	if(m_ble->state == BLE_STA_IDLE)
	{
		LIGHT_OFF();
	}
}

/* 退出低功耗时的处理 */
void lpm_exit_handler(void)
{
	if(nrf_sdh_is_enabled() == true)
	{
		sd_power_dcdc_mode_set(1);
//		sd_clock_hfclk_request();
//		uint32_t status = 0;
//		while(status == 0) sd_clock_hfclk_is_running(&status);
	}
	else
	{
		NRF_POWER->DCDCEN = 1; //退出低功耗打开DCDC
	}
}

/**@brief Function for application main entry.
 */
int main(void)
{
	/**********************************硬件设备初始化**********************************/
	NRF_POWER->DCDCEN = 1; 									//打开DCDC
//	NRF_CLOCK->TASKS_HFCLKSTART = 1; //启动外部时钟源
//	while(!NRF_CLOCK->EVENTS_HFCLKSTARTED); //等待外部时钟源启动完成
	lfclk_cfg(); 											//RTC时钟源设置
	light_init(); 											//设备指示灯初始化
	timers_init(); 											//定时器初始化基于RTC1
	fs_flash_init();						 				//flash初始化
	
	/**********************************功能模块初始化**********************************/
	sys_param_init(); 										//系统参数初始化
	iot_object_t *sensor = createSensorHandler(); 			//传感器对象初始化
	iot_set_prop(sensor);
	sensor->init();
	wireless_comm_svc = createWirelessCommServiceHandler(); //无线通信服务初始化
	wireless_comm_svc->setSensorHandler(sensor);
#if (SIGNAL_DET_HW == 1)
	hw_signal_detect_init(); 								//硬件信号检测功能初始化
#endif
	swt_init(); 											//软件定时器初始化
	calendar_init(); 										//日历功能初始化初始化
	rng_lpm_init(); 										//低功耗随机数发生器初始化
#if (BAT_SOC_DET_SW == 1)
	sw_bat_soc_init();										//软件电池电量检测初始化
#endif

	/**********************************BLE初始化**********************************/
	ble_softdev_init();
	
	/**********************************应用初始化**********************************/
	m_lpm = lpm_init(); 									//低功耗管理初始化
	m_ble = ble_init(m_lpm); 								//BLE任务初始化	
#if (SIGNAL_DET_SW == 1)
	m_sw_signal_detect = sw_signal_detect_task_init(m_lpm); //软件信号检测任务初始化
#endif
	m_lora = lora_task_init(m_lpm); 						//LORA任务初始化
	m_slp = slp_task_init(m_lpm); 							//系统低功耗任务初始化
	m_collapse = collapse_init(m_lpm);						//崩塌计任务初始化
	iot_init(sensor, m_collapse, m_lora); 					//物联网协议属性初始化
	
	/**********************************系统任务设置**********************************/
	sys_task_t task;
	task.ble_task = m_ble;
	task.slp_task = m_slp;
	task.lora_task = m_lora;
	task.signal_detect_task = m_sw_signal_detect;
	task.collapse_task = m_collapse;
	sys_task_init(&task); 									//系统任务初始化
	sys_startup(); 											//系统启动
	LIGHT_OFF();
	
//	uart_init();

	while(1)
	{
		/* 系统任务调度 */
		sys_task_schd();
		
		/* 低功耗管理任务运行 */
		m_lpm->task_operate(lpm_enter_handler, lpm_exit_handler);
		
		/* BLE任务运行 */
		m_ble->task_operate();
		
		/* LORA任务运行 */
		m_lora->task_operate();
		
		/* 系统低功耗任务运行 */
		m_slp->task_operate();
		
		/* 崩塌计任务运行 */
		m_collapse->task_operate();
		
		/* 软件信号检测任务运行 */
#if (SIGNAL_DET_SW == 1)
		m_sw_signal_detect->task_operate();
#endif
		
		/* 物联网协议处理 */
		iot_operate();
		
		/* 系统数据存储 */
		sys_save_param_to_flash();
		
		/* 电量相关数据存储 */
#if (BAT_SOC_DET_SW == 1)
		sw_bat_soc_param_to_flash();
#endif
		
		/* 蓝牙数据协议处理 */
		ble_char_req_handler();

		sys_run_cnt++;		
//		uart_run();
	}
}


/**
 * @}
 */
