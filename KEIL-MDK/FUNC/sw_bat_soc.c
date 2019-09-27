#include "sw_bat_soc.h"
#include "calendar.h"
#include "sys_param.h"
#include "flash.h"
#include "string.h"


/**************************常量区**************************/
const float lora_tx_fail_power_tb[] = { 			//LORA在不同功率下发送失败的功耗
	0, /*-9*/  0, /*-8*/  0, /*-7*/  0, /*-6*/
	0, /*-5*/  0, /*-4*/  0, /*-3*/  0, /*-2*/
	0, /*-1*/  0, /*+0*/  0, /*+1*/  0, /*+2*/
	0, /*+3*/  0, /*+4*/  0, /*+5*/  0, /*+6*/
	0, /*+7*/  0, /*+8*/  0, /*+9*/  0, /*+10*/
	0, /*+11*/ 0, /*+12*/ 0, /*+13*/ 0, /*+14*/
	0, /*+15*/ 0, /*+16*/ 0, /*+17*/ 0, /*+18*/
	0, /*+19*/ 0.002, /*+20*/ 0, /*+21*/ 0, /*+22*/
};

const float lora_tx_success_power_tb[] = { 			//LORA在不同功率下发送成功的功耗
	0, /*-9*/  0, /*-8*/  0, /*-7*/  0, /*-6*/
	0, /*-5*/  0, /*-4*/  0, /*-3*/  0, /*-2*/
	0, /*-1*/  0, /*+0*/  0, /*+1*/  0, /*+2*/
	0, /*+3*/  0, /*+4*/  0, /*+5*/  0, /*+6*/
	0, /*+7*/  0, /*+8*/  0, /*+9*/  0, /*+10*/
	0, /*+11*/ 0, /*+12*/ 0, /*+13*/ 0, /*+14*/
	0, /*+15*/ 0, /*+16*/ 0, /*+17*/ 0, /*+18*/
	0, /*+19*/ 0.001, /*+20*/ 0, /*+21*/ 0, /*+22*/
};

const float bat_cap = 8500; 						//电池容量
const float ble_pre_power = 4;						//蓝牙单位时间功耗
const float standby_per_power = 0.02; 				//设备待机单位时间功耗
const float sca_power = 0.0006; 					//倾角采样任务一次消耗的电量
const float off_power = 0; 							//电池无法给设备供电时软件计算的最后一次剩余容量

/**************************静态变量区**************************/
static float lora_tx_fail_power;					//LORA发送失败一次消耗的电量，与发送功率有关
static float lora_tx_success_power;					//LORA发送成功一次消耗的电量，与发送功率有关
static uint32_t m_ble_run_time;						//蓝牙运行时间
static uint32_t m_time_base;						//保存设备掉电时的运行时间
static uint8_t m_power_update_flag = 0;				//电量更新标志
static uint8_t m_ble_time_mtx = 0;
static uint8_t m_pre_cap_percent = 0;
static sw_bat_soc_t m_sw_bat_soc;
static sw_bat_soc_mod_t sw_bat_soc_mod;

static float sw_bat_soc_lora_tx_fail_power_get(int8_t power_level)
{
	if(!IS_VALID_POWER_LEVEL(power_level))
	{
		return 0;
	}

	float power;
	int8_t index = power_level - LORA_MIN_POWER_LEVEL;
	for(int i=0;;i++)
	{
		if((index + i) <= (sizeof(lora_tx_fail_power_tb) - 1) &&
			lora_tx_fail_power_tb[index+i] != 0)
		{
			power = lora_tx_fail_power_tb[index+i];
			break;
		}
		
		if((index - i) >= 0 && lora_tx_fail_power_tb[index-i] != 0)
		{
			power = lora_tx_fail_power_tb[index-i];
			break;
		}
		
		if((index + i) >= sizeof(lora_tx_fail_power_tb) && (index - i) < 0)
		{
			power = 0;
			break;
		}
	}
	return power;
}

static float sw_bat_soc_lora_tx_success_power_get(int8_t power_level)
{
	if(!IS_VALID_POWER_LEVEL(power_level))
	{
		return 0;
	}

	float power;
	int8_t index = power_level - LORA_MIN_POWER_LEVEL;
	for(int i=0;;i++)
	{
		if((index + i) <= (sizeof(lora_tx_success_power_tb) - 1) &&
			lora_tx_success_power_tb[index+i] != 0)
		{
			power = lora_tx_success_power_tb[index+i];
			break;
		}
		
		if((index - i) >= 0 && lora_tx_success_power_tb[index-i] != 0)
		{
			power = lora_tx_success_power_tb[index-i];
			break;
		}
		
		if((index + i) >= sizeof(lora_tx_success_power_tb) && (index - i) < 0)
		{
			power = 0;
			break;
		}
	}
	return power;
}

static void sw_bat_soc_power_calc(void)
{
	m_sw_bat_soc.rest_cap = bat_cap - 
		m_sw_bat_soc.ble_total_cap - m_sw_bat_soc.sca_total_cap -
		m_sw_bat_soc.lora_tx_fail_total_cap - m_sw_bat_soc.lora_tx_success_total_cap - 
		m_sw_bat_soc.standby_total_cap;
	
	if(m_sw_bat_soc.rest_cap < 0)
	{
		m_sw_bat_soc.rest_cap = 0;
		m_sw_bat_soc.actual_cap = 0;
	}
	else
	{
		m_sw_bat_soc.actual_cap = bat_cap * (m_sw_bat_soc.rest_cap - off_power) / (bat_cap - off_power);
	}
	
	m_sw_bat_soc.cap_percent = (uint8_t)(m_sw_bat_soc.actual_cap * 100 / bat_cap);
	
	if((m_pre_cap_percent - m_sw_bat_soc.cap_percent) >= 1)
	{
		m_power_update_flag = 1;
		m_sw_bat_soc.data_save_flag = 1;
	}
	
	m_pre_cap_percent = m_sw_bat_soc.cap_percent;
}

static void sw_bat_soc_sca_power_add(void)
{
	m_sw_bat_soc.sca_total_cap += sca_power;
	sw_bat_soc_power_calc();
}

static void sw_bat_soc_lora_tx_fail_power_add(void)
{
	sys_param_t* param = sys_param_get_handle();
	lora_tx_fail_power = sw_bat_soc_lora_tx_fail_power_get(param->lora_power);
	m_sw_bat_soc.lora_tx_fail_total_cap += lora_tx_fail_power;
	m_sw_bat_soc.lora_tx_fail_times++;
	sw_bat_soc_power_calc();
}

static void sw_bat_soc_lora_tx_success_power_add(void)
{
	sys_param_t* param = sys_param_get_handle();
	lora_tx_success_power = sw_bat_soc_lora_tx_success_power_get(param->lora_power);
	m_sw_bat_soc.lora_tx_success_total_cap += lora_tx_success_power;
	m_sw_bat_soc.lora_tx_success_times++;
	sw_bat_soc_power_calc();
}

static void sw_bat_soc_standby_power_add(void)
{
	m_sw_bat_soc.standby_total_cap = standby_per_power * m_sw_bat_soc.dev_run_time / 3600;
	sw_bat_soc_power_calc();
}

static void sw_bat_soc_ble_power_add(void)
{
	m_sw_bat_soc.ble_total_cap = ble_pre_power * m_sw_bat_soc.ble_run_time / 3600;
	sw_bat_soc_power_calc();
}

static void sw_bat_soc_dev_run_time_add(void)
{
	calendar_mod_t* calendar = calendar_get_handle();
	m_sw_bat_soc.dev_run_time = m_time_base + calendar->get_run_time();
}

static void sw_bat_soc_ble_run_time_start(void)
{
	if(m_ble_time_mtx == 0)
	{
		m_ble_time_mtx = 1;
		calendar_mod_t* calendar = calendar_get_handle();
		m_ble_run_time = calendar->get_run_time();
	}
}

static void sw_bat_soc_ble_run_time_add(void)
{
	m_ble_time_mtx = 0;
	calendar_mod_t* calendar = calendar_get_handle();
	m_ble_run_time = calendar->get_run_time() - m_ble_run_time;
	m_sw_bat_soc.ble_run_time += m_ble_run_time;
}

static uint8_t sw_bat_soc_get_update_flag(void)
{
	uint8_t flag = m_power_update_flag;
	m_power_update_flag = 0;
	return flag;
}

sw_bat_soc_mod_t* sw_bat_soc_init(void)
{
	flash_read(SW_BAT_SOC_FLASH_PAGE_ADDR, (uint8_t *)&m_sw_bat_soc, sizeof(m_sw_bat_soc));
	
	if(m_sw_bat_soc.data_save_flag == 0XFF)
	{
		memset(&m_sw_bat_soc, 0X00, sizeof(m_sw_bat_soc));
		m_sw_bat_soc.cap_percent = 100;
		m_sw_bat_soc.rest_cap = bat_cap;
		m_sw_bat_soc.actual_cap = bat_cap;
	}
	m_sw_bat_soc.data_save_flag = 0;
	
	sw_bat_soc_mod.dev_run_time_add = sw_bat_soc_dev_run_time_add;
	sw_bat_soc_mod.ble_run_time_start = sw_bat_soc_ble_run_time_start;
	sw_bat_soc_mod.ble_run_time_add = sw_bat_soc_ble_run_time_add;
	sw_bat_soc_mod.sca_power_add = sw_bat_soc_sca_power_add;
	sw_bat_soc_mod.lora_tx_fail_power_add = sw_bat_soc_lora_tx_fail_power_add;
	sw_bat_soc_mod.lora_tx_success_power_add = sw_bat_soc_lora_tx_success_power_add;
	sw_bat_soc_mod.standby_power_add = sw_bat_soc_standby_power_add;
	sw_bat_soc_mod.ble_power_add = sw_bat_soc_ble_power_add;
	sw_bat_soc_mod.get_update_flag = sw_bat_soc_get_update_flag;
	
	m_pre_cap_percent = m_sw_bat_soc.cap_percent;
	m_time_base = m_sw_bat_soc.dev_run_time;
	m_power_update_flag = 1;
	
	return &sw_bat_soc_mod;
}

sw_bat_soc_mod_t* sw_bat_soc_get_handle(void)
{
	return &sw_bat_soc_mod;
}

uint8_t sw_bat_soc_param_to_flash(void)
{
	if(m_sw_bat_soc.data_save_flag == 1)
	{
		m_sw_bat_soc.data_save_flag = 0;
		return flash_write(SW_BAT_SOC_FLASH_PAGE_ADDR,
						  (uint32_t*)&m_sw_bat_soc,
						   sizeof(m_sw_bat_soc)%4==0?sizeof(m_sw_bat_soc)/4:(sizeof(m_sw_bat_soc)/4+1));
	}
	return 0;
}



