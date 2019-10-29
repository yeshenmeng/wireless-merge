#include "lora_lost_rate_test.h"
#include "ble_init.h"
#include "sw_timer_rtc.h"
#include "sys_param.h"
#include "calendar.h"
#include "string.h"


static llrt_mod_t llrt_mod;
static uint32_t llrt_tx_cnt;			//发送次数
static uint32_t llrt_rx_cnt;			//接收次数
static uint8_t llrt_start_flag;
static llrt_status_t llrt_status;
static uint32_t test_start_time = 0;

static uint8_t collapse_max_wait_reply_len = 4 + 1 + 2 + 1 + 7 + 8 + 2 + 1 + 4 + 4 + 2 + 2 + 2;
static uint8_t clinometer_max_wait_reply_len = 4 + 1 + 2 + 1 + 8 + 8 + 2 + 1 + 4 + 4 + 4 + 4 + 4 + 2;

void swt_llrt_timer_cb(void* param)
{
	llrt_start_flag = 0;
}

void llrt_init(void)
{
	sys_param_t* param = sys_param_get_handle();
	if(param->object_version == INCLINOMETER_VERSION)
	{
		llrt_mod.payload_len = clinometer_max_wait_reply_len - 4 - 1 - 8 - 8 - 2;
	}
	else if(param->object_version == COLLAPSE_VERSION)
	{
		llrt_mod.payload_len = collapse_max_wait_reply_len - 4 - 1 - 8 - 8 - 2;
	}
	
	llrt_mod.counting_nums = 30;
	llrt_mod.timer_time = 150;
	llrt_mod.comm_interval = 5;
	llrt_mod.comm_ctrl = LLRT_CTRL_NULL;
	llrt_mod.dev_comm_ctrl = LLRT_CTRL_NULL;
	llrt_mod.test_progress = 0;
	
	llrt_tx_cnt = 0;
	llrt_rx_cnt = 0;
	llrt_start_flag = 0;
	llrt_status = LLRT_STOP;
}

void llrt_param_set(uint8_t* param, uint8_t* value, uint8_t len)
{
	memcpy(param, value, len);
}

llrt_status_t llrt_status_get(void)
{
	return llrt_status;
}

void llrt_status_set(llrt_status_t status)
{
	llrt_status = status;
}

void llrt_start(void)
{
	if(llrt_status == LLRT_ACTIVE)
	{
		llrt_tx_cnt = 0;
		llrt_rx_cnt = 0;
		llrt_start_flag = 1;
		llrt_status = LLRT_RUN;
		
		if(llrt_mod.counting_nums == 0 && llrt_mod.timer_time != 0)
		{
			swt_mod_t* timer = swt_get_handle();
			timer->llrt_timer->start(llrt_mod.timer_time*1000);
			calendar_mod_t* calendar = calendar_get_handle();
			test_start_time = calendar->get_run_time();
		}
	}
}

llrt_ctrl_t llrt_comm_ctrl_status(void)
{
	return llrt_mod.dev_comm_ctrl;
}

void llrt_stop(void)
{
	llrt_start_flag = 0;
	
	if(llrt_mod.counting_nums == 0 && llrt_mod.timer_time != 0)
	{
		swt_mod_t* timer = swt_get_handle();
		timer->llrt_timer->stop();
	}
}

void llrt_tx_add(void)
{
	llrt_tx_cnt++;
	
	if(llrt_mod.counting_nums != 0 && llrt_tx_cnt >= llrt_mod.counting_nums)
	{
		llrt_start_flag = 0;
	}
}

void llrt_rx_add(void)
{
	llrt_rx_cnt++;
}

void llrt_lost_rate_calc(void)
{
	llrt_mod.lost_rate = (llrt_tx_cnt - llrt_rx_cnt) * 100.0 / llrt_tx_cnt;
	ble_char_update_t* ble_char_update_handle = ble_char_update_handle_get();
	ble_char_update_handle->misc_lost_rate_notify(llrt_mod.lost_rate);
	
	if(llrt_mod.counting_nums != 0)
	{
		uint32_t test_cnt = llrt_tx_cnt;
		(test_cnt >= llrt_mod.counting_nums) ? (test_cnt=llrt_mod.counting_nums) : (1);
		llrt_mod.test_progress = test_cnt * 100.0 / llrt_mod.counting_nums;
		ble_char_update_handle->misc_test_progress_notify(llrt_mod.test_progress);
	}
	else if(llrt_mod.counting_nums == 0 && llrt_mod.timer_time != 0)
	{
		calendar_mod_t* calendar = calendar_get_handle();
		uint32_t time = calendar->get_run_time() - test_start_time;
		(time >= llrt_mod.timer_time) ? (time=llrt_mod.timer_time) : (1);
		llrt_mod.test_progress = time * 100.0 / llrt_mod.timer_time;
		ble_char_update_handle->misc_test_progress_notify(llrt_mod.test_progress);
	}
	
	if(llrt_start_flag == 0)
	{
		llrt_status = LLRT_STOP;
		llrt_mod.comm_ctrl = LLRT_CTRL_NULL;
		llrt_mod.dev_comm_ctrl = LLRT_CTRL_NULL;
		ble_char_update_handle->misc_comm_ctrl_notify(llrt_mod.dev_comm_ctrl);
	}
}

llrt_mod_t* llrt_get_handle(void)
{
	return &llrt_mod;
}
















