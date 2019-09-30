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

/* ��ʼ�������ʱ�� */
static void timers_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

/* rtcʱ������ */
static void lfclk_cfg(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
}

/**
 * @brief  ��ʼ��������Э�����Գ���
 * @param  sensor: ָ��iot_object_t�ṹ�ľ��
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

/* ����͹���ʱ�Ĵ��� */
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
		NRF_POWER->DCDCEN = 0; //�͹��Ĺر�DCDC
	}

	if(m_ble->state == BLE_STA_IDLE)
	{
		LIGHT_OFF();
	}
}

/* �˳��͹���ʱ�Ĵ��� */
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
		NRF_POWER->DCDCEN = 1; //�˳��͹��Ĵ�DCDC
	}
}

/**@brief Function for application main entry.
 */
int main(void)
{
	/**********************************Ӳ���豸��ʼ��**********************************/
	NRF_POWER->DCDCEN = 1; 									//��DCDC
//	NRF_CLOCK->TASKS_HFCLKSTART = 1; //�����ⲿʱ��Դ
//	while(!NRF_CLOCK->EVENTS_HFCLKSTARTED); //�ȴ��ⲿʱ��Դ�������
	lfclk_cfg(); 											//RTCʱ��Դ����
	light_init(); 											//�豸ָʾ�Ƴ�ʼ��
	timers_init(); 											//��ʱ����ʼ������RTC1
	fs_flash_init();						 				//flash��ʼ��
	
	/**********************************����ģ���ʼ��**********************************/
	sys_param_init(); 										//ϵͳ������ʼ��
	iot_object_t *sensor = createSensorHandler(); 			//�����������ʼ��
	iot_set_prop(sensor);
	sensor->init();
	wireless_comm_svc = createWirelessCommServiceHandler(); //����ͨ�ŷ����ʼ��
	wireless_comm_svc->setSensorHandler(sensor);
#if (SIGNAL_DET_HW == 1)
	hw_signal_detect_init(); 								//Ӳ���źż�⹦�ܳ�ʼ��
#endif
	swt_init(); 											//�����ʱ����ʼ��
	calendar_init(); 										//�������ܳ�ʼ����ʼ��
	rng_lpm_init(); 										//�͹����������������ʼ��
#if (BAT_SOC_DET_SW == 1)
	sw_bat_soc_init();										//�����ص�������ʼ��
#endif

	/**********************************BLE��ʼ��**********************************/
	ble_softdev_init();
	
	/**********************************Ӧ�ó�ʼ��**********************************/
	m_lpm = lpm_init(); 									//�͹��Ĺ����ʼ��
	m_ble = ble_init(m_lpm); 								//BLE�����ʼ��	
#if (SIGNAL_DET_SW == 1)
	m_sw_signal_detect = sw_signal_detect_task_init(m_lpm); //����źż�������ʼ��
#endif
	m_lora = lora_task_init(m_lpm); 						//LORA�����ʼ��
	m_slp = slp_task_init(m_lpm); 							//ϵͳ�͹��������ʼ��
	m_collapse = collapse_init(m_lpm);						//�����������ʼ��
	iot_init(sensor, m_collapse, m_lora); 					//������Э�����Գ�ʼ��
	
	/**********************************ϵͳ��������**********************************/
	sys_task_t task;
	task.ble_task = m_ble;
	task.slp_task = m_slp;
	task.lora_task = m_lora;
	task.signal_detect_task = m_sw_signal_detect;
	task.collapse_task = m_collapse;
	sys_task_init(&task); 									//ϵͳ�����ʼ��
	sys_startup(); 											//ϵͳ����
	LIGHT_OFF();
	
//	uart_init();

	while(1)
	{
		/* ϵͳ������� */
		sys_task_schd();
		
		/* �͹��Ĺ����������� */
		m_lpm->task_operate(lpm_enter_handler, lpm_exit_handler);
		
		/* BLE�������� */
		m_ble->task_operate();
		
		/* LORA�������� */
		m_lora->task_operate();
		
		/* ϵͳ�͹����������� */
		m_slp->task_operate();
		
		/* �������������� */
		m_collapse->task_operate();
		
		/* ����źż���������� */
#if (SIGNAL_DET_SW == 1)
		m_sw_signal_detect->task_operate();
#endif
		
		/* ������Э�鴦�� */
		iot_operate();
		
		/* ϵͳ���ݴ洢 */
		sys_save_param_to_flash();
		
		/* ����������ݴ洢 */
#if (BAT_SOC_DET_SW == 1)
		sw_bat_soc_param_to_flash();
#endif
		
		/* ��������Э�鴦�� */
		ble_char_req_handler();

		sys_run_cnt++;		
//		uart_run();
	}
}


/**
 * @}
 */
