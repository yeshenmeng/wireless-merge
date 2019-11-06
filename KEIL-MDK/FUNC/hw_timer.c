#include "hw_timer.h"
#include "nrf_drv_timer.h"
#include "string.h"


static sys_time_t m_sys_time = {0};
static const nrf_drv_timer_t m_timer = NRF_DRV_TIMER_INSTANCE(1);

static void timer_handler(nrf_timer_event_t event_type, void * p_context)
{
	if(m_sys_time.milsecond >= 999)
	{
		m_sys_time.milsecond = 0;
		m_sys_time.second++;
	}
	else
	{
		m_sys_time.milsecond++;
	}
	
	m_sys_time.sumtime = m_sys_time.second * 1000 + m_sys_time.milsecond;
	
	if(m_sys_time.sumtime >= 0XFFFFFFFF)
	{
		memset(&m_sys_time,0,sizeof(m_sys_time));
	}
}

static void hw_timer_init(void)
{
	/* 初始化定时器 */
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
    ret_code_t err_code = nrf_drv_timer_init(&m_timer, &timer_cfg, timer_handler);
    APP_ERROR_CHECK(err_code);

	/* 设置定时器每隔一定时间发生比较事件 */
    uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, HW_TIMER_TICK_MS);
    nrf_drv_timer_extended_compare(&m_timer,
                                   NRF_TIMER_CC_CHANNEL0,
                                   ticks,
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
                                   true);
}

void hw_timer_start(void)
{
	hw_timer_init();
    nrf_drv_timer_enable(&m_timer);
}

void hw_timer_stop(void)
{
	memset(&m_sys_time,0,sizeof(m_sys_time));
	nrfx_timer_disable(&m_timer);
	nrfx_timer_uninit(&m_timer);
}

sys_time_t* hw_timer_get_sys_time(void)
{
	return &m_sys_time;
}

