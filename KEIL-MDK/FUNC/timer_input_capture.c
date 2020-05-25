#include "timer_input_capture.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_timer.h"
#include "nrf_gpiote.h"


static const nrf_drv_timer_t time_timer = NRF_DRV_TIMER_INSTANCE(3);
static const nrf_drv_timer_t counter_timer = NRF_DRV_TIMER_INSTANCE(4);
static nrf_ppi_channel_t ppi_channel1;
static nrf_ppi_channel_t ppi_channel2;
static nrf_ppi_channel_t ppi_channel3;
static nrf_ppi_channel_t ppi_channel4;
static timer_ic_cplt_cb_t ic_cplt_cb = NULL;
static uint8_t ic_sig_pin = 33;
static timer_ic_t timer_ic;
static uint32_t timer_time = 0;
static uint32_t timer_counter = 0;

static void timer_time_handler(nrf_timer_event_t event_type, void * p_context)
{
	if(event_type == NRF_TIMER_EVENT_COMPARE0)
	{
		uint32_t value = nrfx_timer_capture(&counter_timer, NRF_TIMER_CC_CHANNEL0);
		nrf_drv_timer_disable(&time_timer);
		nrf_drv_timer_disable(&counter_timer);
		nrfx_gpiote_in_event_disable(ic_sig_pin);
		
		if(ic_cplt_cb != NULL)
		{
			double freq = value / (timer_time * 1.0 / 1000);
			ic_cplt_cb(freq);
		}
	}
}

static void timer_counter_handler(nrf_timer_event_t event_type, void * p_context)
{
	if(event_type == NRF_TIMER_EVENT_COMPARE0)
	{
		uint32_t value = nrfx_timer_capture(&time_timer, NRF_TIMER_CC_CHANNEL0);
		nrf_drv_timer_disable(&time_timer);
		nrf_drv_timer_disable(&counter_timer);
		nrfx_gpiote_in_event_disable(ic_sig_pin);
		
		if(ic_cplt_cb != NULL)
		{
			double time = value * 0.0625 / 1000000;
			double freq = 1 / (time / timer_counter);
			ic_cplt_cb(freq);
		}
	}
}

static void gpiote_tiemr_in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	if(pin == ic_sig_pin)
	{
		if(action == NRF_GPIOTE_POLARITY_LOTOHI)
		{
			uint32_t time_tick = nrf_drv_timer_ms_to_ticks(&time_timer, timer_time);
			nrf_drv_timer_extended_compare(&time_timer, NRF_TIMER_CC_CHANNEL0, 
										   time_tick, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);	//设置定时器时间
			nrf_timer_cc_write(counter_timer.p_reg, NRF_TIMER_CC_CHANNEL0, 0);
			nrfx_ppi_channel_disable(ppi_channel3);
			nrfx_ppi_channel_disable(ppi_channel4);
			nrf_gpiote_int_disable(GPIOTE_INTENSET_IN0_Msk);
		}
	}
}

static void gpiote_counter_in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	if(pin == ic_sig_pin)
	{
		if(action == NRF_GPIOTE_POLARITY_LOTOHI)
		{
			nrf_drv_timer_extended_compare(&counter_timer, NRF_TIMER_CC_CHANNEL0, 
										   timer_counter, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
			nrf_timer_cc_write(time_timer.p_reg, NRF_TIMER_CC_CHANNEL0, 0);
			nrfx_ppi_channel_disable(ppi_channel3);
			nrfx_ppi_channel_disable(ppi_channel4);
			nrf_gpiote_int_disable(GPIOTE_INTENSET_IN0_Msk);
		}
	}
}

//NRF_GPIOTE_INT_PORT_MASK
static void timer_ic_timer_init(uint8_t ic_isr_pin, timer_ic_cplt_cb_t cb)
{
	uint32_t err_code;
	
	ic_sig_pin = ic_isr_pin;
	if(ic_sig_pin >= 33) return;
	
	//PPI初始化
	err_code = nrf_drv_ppi_init();
	APP_ERROR_CHECK(err_code);
	
	//GPIOTE初始化
	if (!nrf_drv_gpiote_is_init())
	{
		uint32_t err_code;
		err_code = nrf_drv_gpiote_init();
		APP_ERROR_CHECK(err_code);
	}
	
	//配置时间定时器
	nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
	timer_cfg.frequency = NRF_TIMER_FREQ_16MHz;
	timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
	timer_cfg.mode = NRF_TIMER_MODE_TIMER;
	timer_cfg.interrupt_priority = 4;
	err_code = nrf_drv_timer_init(&time_timer, &timer_cfg, timer_time_handler);
	APP_ERROR_CHECK(err_code);
	
	//配置计数定时器
	timer_cfg.frequency = NRF_TIMER_FREQ_16MHz;
	timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
	timer_cfg.mode = NRF_TIMER_MODE_COUNTER;
	timer_cfg.interrupt_priority = 5;
	err_code = nrf_drv_timer_init(&counter_timer, &timer_cfg, NULL);
	APP_ERROR_CHECK(err_code);
	
	uint32_t evt_addr;
	uint32_t task_addr;
	nrf_drv_gpiote_in_config_t config;
	
	//配置信号中断
	config.sense = NRF_GPIOTE_POLARITY_LOTOHI;
	config.pull = NRF_GPIO_PIN_NOPULL;
	config.is_watcher = false;
	config.hi_accuracy = true;
	err_code = nrf_drv_gpiote_in_init(ic_sig_pin, &config, gpiote_tiemr_in_pin_handler);
	APP_ERROR_CHECK(err_code);
	
	nrfx_gpiote_in_event_disable(ic_sig_pin);
	
	//获取PPI通道
	err_code = nrf_drv_ppi_channel_alloc(&ppi_channel1);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_alloc(&ppi_channel2);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_alloc(&ppi_channel3);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_alloc(&ppi_channel4);
	APP_ERROR_CHECK(err_code);
	
	//使用PPI通道关联信号中断到计数定时器
	task_addr = nrf_drv_timer_task_address_get(&counter_timer, NRF_TIMER_TASK_COUNT);
	evt_addr = nrf_drv_gpiote_in_event_addr_get(ic_sig_pin);
	err_code = nrf_drv_ppi_channel_assign(ppi_channel1, evt_addr, task_addr);
	APP_ERROR_CHECK(err_code);
	
	//使用PPI通道关联计时中断到计数定时器停止任务
	task_addr = nrf_drv_timer_task_address_get(&counter_timer, NRF_TIMER_TASK_STOP);
	evt_addr = nrfx_timer_event_address_get(&time_timer, NRF_TIMER_EVENT_COMPARE0);
	err_code = nrf_drv_ppi_channel_assign(ppi_channel2, evt_addr, task_addr);
	APP_ERROR_CHECK(err_code);
	
	//使用PPI通道关联信号中断到计时定时器启动
	task_addr = nrf_drv_timer_task_address_get(&time_timer, NRF_TIMER_TASK_START);
	evt_addr = nrf_drv_gpiote_in_event_addr_get(ic_sig_pin);
	err_code = nrf_drv_ppi_channel_assign(ppi_channel3, evt_addr, task_addr);
	APP_ERROR_CHECK(err_code);
	
	//使用PPI通道关联信号中断到计数定时器启动
	task_addr = nrf_drv_timer_task_address_get(&counter_timer, NRF_TIMER_TASK_START);
	evt_addr = nrf_drv_gpiote_in_event_addr_get(ic_sig_pin);
	err_code = nrf_drv_ppi_channel_assign(ppi_channel4, evt_addr, task_addr);
	APP_ERROR_CHECK(err_code);	
	
	//PPI通道使能
	err_code = nrf_drv_ppi_channel_enable(ppi_channel1);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_enable(ppi_channel2);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_enable(ppi_channel3);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_enable(ppi_channel4);
	APP_ERROR_CHECK(err_code);
	
	if(cb != NULL)
		ic_cplt_cb = cb;
}

static void timer_ic_counter_init(uint8_t ic_isr_pin, timer_ic_cplt_cb_t cb)
{
	uint32_t err_code;
	
	ic_sig_pin = ic_isr_pin;
	if(ic_sig_pin >= 33) return;
	
	//PPI初始化
	err_code = nrf_drv_ppi_init();
	APP_ERROR_CHECK(err_code);
	
	//GPIOTE初始化
	if (!nrf_drv_gpiote_is_init())
	{
		uint32_t err_code;
		err_code = nrf_drv_gpiote_init();
		APP_ERROR_CHECK(err_code);
	}
	
	//配置时间定时器
	nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
	timer_cfg.frequency = NRF_TIMER_FREQ_16MHz;
	timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
	timer_cfg.mode = NRF_TIMER_MODE_TIMER;
	timer_cfg.interrupt_priority = 4;
	err_code = nrf_drv_timer_init(&time_timer, &timer_cfg, NULL);
	APP_ERROR_CHECK(err_code);
	
	//配置计数定时器
	timer_cfg.frequency = NRF_TIMER_FREQ_16MHz;
	timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
	timer_cfg.mode = NRF_TIMER_MODE_COUNTER;
	timer_cfg.interrupt_priority = 5;
	err_code = nrf_drv_timer_init(&counter_timer, &timer_cfg, timer_counter_handler);
	APP_ERROR_CHECK(err_code);
	
	uint32_t evt_addr;
	uint32_t task_addr;
	nrf_drv_gpiote_in_config_t config;
	
	//配置信号中断
	config.sense = NRF_GPIOTE_POLARITY_LOTOHI;
	config.pull = NRF_GPIO_PIN_NOPULL;
	config.is_watcher = false;
	config.hi_accuracy = true;
	err_code = nrf_drv_gpiote_in_init(ic_sig_pin, &config, gpiote_counter_in_pin_handler);
	APP_ERROR_CHECK(err_code);
	
	nrfx_gpiote_in_event_disable(ic_sig_pin);
	
	//获取PPI通道
	err_code = nrf_drv_ppi_channel_alloc(&ppi_channel1);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_alloc(&ppi_channel2);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_alloc(&ppi_channel3);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_alloc(&ppi_channel4);
	APP_ERROR_CHECK(err_code);
	
	//使用PPI通道关联信号中断到计数定时器
	task_addr = nrf_drv_timer_task_address_get(&counter_timer, NRF_TIMER_TASK_COUNT);
	evt_addr = nrf_drv_gpiote_in_event_addr_get(ic_sig_pin);
	err_code = nrf_drv_ppi_channel_assign(ppi_channel1, evt_addr, task_addr);
	APP_ERROR_CHECK(err_code);
	
	//使用PPI通道关联比较中断到计数定时器停止任务
	task_addr = nrf_drv_timer_task_address_get(&time_timer, NRF_TIMER_TASK_STOP);
	evt_addr = nrfx_timer_event_address_get(&counter_timer, NRF_TIMER_EVENT_COMPARE0);
	err_code = nrf_drv_ppi_channel_assign(ppi_channel2, evt_addr, task_addr);
	APP_ERROR_CHECK(err_code);
	
	//使用PPI通道关联信号中断到计时定时器启动
	task_addr = nrf_drv_timer_task_address_get(&time_timer, NRF_TIMER_TASK_START);
	evt_addr = nrf_drv_gpiote_in_event_addr_get(ic_sig_pin);
	err_code = nrf_drv_ppi_channel_assign(ppi_channel3, evt_addr, task_addr);
	APP_ERROR_CHECK(err_code);
	
	//使用PPI通道关联信号中断到计数定时器启动
	task_addr = nrf_drv_timer_task_address_get(&counter_timer, NRF_TIMER_TASK_START);
	evt_addr = nrf_drv_gpiote_in_event_addr_get(ic_sig_pin);
	err_code = nrf_drv_ppi_channel_assign(ppi_channel4, evt_addr, task_addr);
	APP_ERROR_CHECK(err_code);	
	
	//PPI通道使能
	err_code = nrf_drv_ppi_channel_enable(ppi_channel1);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_enable(ppi_channel2);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_enable(ppi_channel3);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_enable(ppi_channel4);
	APP_ERROR_CHECK(err_code);
	
	if(cb != NULL)
		ic_cplt_cb = cb;
}

static void timer_ic_start(uint32_t time)
{
	if(ic_sig_pin >= 33) return;
	nrf_timer_cc_write(time_timer.p_reg, NRF_TIMER_CC_CHANNEL0, 0);
	nrf_timer_cc_write(counter_timer.p_reg, NRF_TIMER_CC_CHANNEL0, 0);
	timer_time = time;

	uint32_t err_code;
	err_code = nrf_drv_ppi_channel_enable(ppi_channel3);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_enable(ppi_channel4);
	APP_ERROR_CHECK(err_code);
	nrf_drv_gpiote_in_event_enable(ic_sig_pin, true);
}

uint32_t counter_timer_read(void)
{
	return nrf_timer_cc_read(counter_timer.p_reg, NRF_TIMER_CC_CHANNEL0);
}
static void timer_ic_counter_start(uint32_t counter)
{
	if(ic_sig_pin >= 33) return;
	nrf_timer_cc_write(time_timer.p_reg, NRF_TIMER_CC_CHANNEL0, 0);
	nrf_timer_cc_write(counter_timer.p_reg, NRF_TIMER_CC_CHANNEL0, 0);
	timer_counter = counter;
	
	uint32_t err_code;
	err_code = nrf_drv_ppi_channel_enable(ppi_channel3);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_enable(ppi_channel4);
	APP_ERROR_CHECK(err_code);
	nrf_drv_gpiote_in_event_enable(ic_sig_pin, true);
}

static void timer_ic_stop(void)
{
	if(ic_sig_pin >= 33) return;
	nrfx_gpiote_in_event_disable(ic_sig_pin);
	nrf_drv_timer_disable(&time_timer);
	nrf_drv_timer_disable(&counter_timer);
}

static void timer_ic_uninit(void)
{
	if(ic_sig_pin >= 33) return;
	timer_ic_stop();
	nrf_drv_ppi_channel_disable(ppi_channel1);
	nrf_drv_ppi_channel_disable(ppi_channel2);
	nrf_drv_ppi_channel_disable(ppi_channel3);
	nrf_drv_ppi_channel_disable(ppi_channel4);
	nrf_drv_ppi_uninit();
	nrf_drv_gpiote_in_uninit(ic_sig_pin);
	nrf_drv_timer_uninit(&time_timer);
	nrf_drv_timer_uninit(&counter_timer);
	ic_sig_pin = 33;
}

timer_ic_t* timer_ic_task_init(void)
{
	timer_ic.task_timer_init = timer_ic_timer_init;
	timer_ic.task_uninit = timer_ic_uninit;
	timer_ic.task_timer_start = timer_ic_start;
	timer_ic.task_stop = timer_ic_stop;
	
	timer_ic.task_counter_init = timer_ic_counter_init;
	timer_ic.task_counter_start = timer_ic_counter_start;
	return &timer_ic;
}

timer_ic_t* imer_ic_task_get_handle(void)
{
	return &timer_ic;
}


