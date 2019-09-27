#include "low_power_manage.h"
#include "nrf_pwr_mgmt.h"
#include "light.h"


static uint8_t lpm_task_nums = 0;
static lpm_obj_t lpm_obj;
static lpm_task_t lpm_task_tb[LPM_TASK_MAX_NUMBER];


static void lpm_task_tb_init(void)
{
	for(int i=0; i<LPM_TASK_MAX_NUMBER; i++)
	{
		lpm_task_tb[i].task_stat = LPM_TASK_NULL;
	}
	
	return;
}

static ErrorStatus lpm_task_reg(uint8_t task_id)
{
	ErrorStatus status = ERROR;
	
	if(lpm_task_nums < LPM_TASK_MAX_NUMBER)
	{
		if(lpm_task_tb[lpm_task_nums].task_stat == LPM_TASK_NULL)
		{
			lpm_task_tb[lpm_task_nums].task_id = task_id;
			lpm_task_tb[lpm_task_nums].task_stat = LPM_TASK_REGISTER;
			lpm_task_nums += 1;
			status = SUCCESS;
		}
	}
	
	return status;
}

static ErrorStatus lpm_task_cancel_reg(uint8_t task_id)
{
	uint8_t i;
	ErrorStatus status = ERROR;
	
	for(i=0; i<lpm_task_nums; i++)
	{
		if(lpm_task_tb[i].task_id == task_id)
		{
			lpm_task_tb[i].task_stat = LPM_TASK_NULL;
			status = SUCCESS;
			break;
		}
	}
	
	if(status == SUCCESS)
	{
		for(i+=1; i<lpm_task_nums; i++)
		{
			if(lpm_task_tb[i].task_stat == LPM_TASK_NULL)
			{
				break;
			}

			lpm_task_tb[i-1].task_id = lpm_task_tb[i].task_id;
			lpm_task_tb[i-1].task_stat = lpm_task_tb[i].task_stat;
		}
		
		lpm_task_nums -= 1;
	}

	return status;
}

static ErrorStatus lpm_task_set_stat(uint8_t task_id, uint8_t state)
{
	if(!IS_LPM_TASK_STATE(state))
	{
		return ERROR;
	}
	
	ErrorStatus status = ERROR;
	for(int i=0; i<lpm_task_nums; i++)
	{
		if(lpm_task_tb[i].task_id == task_id)
		{
			lpm_task_tb[i].task_stat = state;
			status = SUCCESS;
			break;
		}
	}
	
	return status;
}

static uint8_t lpm_task_qry_stat(uint8_t task_id)
{
	uint8_t status = LPM_TASK_STA_INVALID;
	
	for(int i=0; i<lpm_task_nums; i++)
	{
		if(lpm_task_tb[i].task_id == task_id)
		{
			status = lpm_task_tb[i].task_stat;
			break;
		}
	}
	
	return status;
}

static FlagStatus lpm_is_task_run(void)
{
	FlagStatus status = RESET;
	
	for(int i=0; i<lpm_task_nums; i++)
	{
		if(lpm_task_tb[i].task_stat == LPM_TASK_STA_RUN)
		{
			status = SET;
			break;
		}
	}
	
	return status;
}

void swt_idle_cb(void)
{
	return;
}

static void lpm_task_operate(lpm_handler_t enter, lpm_handler_t exit)
{
	/* 查询是否有任务正在运行 */
	if(lpm_is_task_run() == SET)
	{
		return;
	}
	
	/* 如果没有任何任务需要运行则启动空闲任务以定期唤醒系统 */
	if(RTC_STATUS_GET() == RTC_STOP)
	{
		swt_mod_t* timer = swt_get_handle();
//		timer->sys_idle->start(5000);
	}
	
	if(enter)
	{
		enter();
	}
	
	nrf_pwr_mgmt_run();
	
	if(exit)
	{
		exit();
	}
	
	return;
}

static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

lpm_obj_t* lpm_init(void)
{
	power_management_init();
	
	lpm_task_tb_init();
	lpm_obj.task_reg = lpm_task_reg;
	lpm_obj.task_cancel_reg = lpm_task_cancel_reg;
	lpm_obj.task_set_stat = lpm_task_set_stat;
	lpm_obj.task_qry_stat = lpm_task_qry_stat;
	lpm_obj.is_task_run = lpm_is_task_run;
	lpm_obj.task_operate = lpm_task_operate;
	
	return &lpm_obj;
}




