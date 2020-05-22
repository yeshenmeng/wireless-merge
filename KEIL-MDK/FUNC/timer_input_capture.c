#include "timer_input_capture.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_timer.h"


static const nrf_drv_timer_t time_timer = NRF_DRV_TIMER_INSTANCE(3);
static const nrf_drv_timer_t counter_timer = NRF_DRV_TIMER_INSTANCE(4);
static nrf_ppi_channel_t ppi_channel1;
static nrf_ppi_channel_t ppi_channel2;
static timer_ic_cplt_cb_t ic_cplt_cb = NULL;
static uint8_t ic_sig_pin = 33;
static timer_ic_t timer_ic;


static void timer_time_handler(nrf_timer_event_t event_type, void * p_context)
{
	if(event_type == NRF_TIMER_EVENT_COMPARE0)
	{
		uint32_t value = nrfx_timer_capture(&counter_timer, NRF_TIMER_CC_CHANNEL0);
		nrf_drv_timer_disable(&time_timer);
		
		if(ic_cplt_cb != NULL)
			ic_cplt_cb(value);
	}
}

static void timer_ic_init(uint8_t ic_isr_pin, timer_ic_cplt_cb_t cb)
{
	uint32_t err_code;
	
	ic_sig_pin = ic_isr_pin;
	if(ic_sig_pin >= 33) return;
	
	//PPI��ʼ��
	err_code = nrf_drv_ppi_init();
	APP_ERROR_CHECK(err_code);
	
	//GPIOTE��ʼ��
	if (!nrf_drv_gpiote_is_init())
	{
		uint32_t err_code;
		err_code = nrf_drv_gpiote_init();
		APP_ERROR_CHECK(err_code);
	}
	
	//����ʱ�䶨ʱ��
	nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
	timer_cfg.frequency = NRF_TIMER_FREQ_1MHz;
	timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
	timer_cfg.mode = NRF_TIMER_MODE_TIMER;
	timer_cfg.interrupt_priority = 4;
	err_code = nrf_drv_timer_init(&time_timer, &timer_cfg, timer_time_handler);
	APP_ERROR_CHECK(err_code);
	
	//���ü�����ʱ��
	timer_cfg.frequency = NRF_TIMER_FREQ_2MHz;
	timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
	timer_cfg.mode = NRF_TIMER_MODE_COUNTER;
	timer_cfg.interrupt_priority = 5;
	err_code = nrf_drv_timer_init(&counter_timer, &timer_cfg, NULL);
	APP_ERROR_CHECK(err_code);
	
	uint32_t evt_addr;
	uint32_t task_addr;
	nrf_drv_gpiote_in_config_t config;
	
	//�����ź��ж�
	config.sense = NRF_GPIOTE_POLARITY_LOTOHI;
	config.pull = NRF_GPIO_PIN_NOPULL;
	config.is_watcher = false;
	config.hi_accuracy = true;
	err_code = nrf_drv_gpiote_in_init(ic_sig_pin, &config, NULL);
	APP_ERROR_CHECK(err_code);
	
	//��ȡPPIͨ��
	err_code = nrf_drv_ppi_channel_alloc(&ppi_channel1);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_alloc(&ppi_channel2);
	APP_ERROR_CHECK(err_code);
	
	//ʹ��PPIͨ�������ź��жϵ�������ʱ��
	task_addr = nrf_drv_timer_task_address_get(&counter_timer, NRF_TIMER_TASK_COUNT);
	evt_addr = nrf_drv_gpiote_in_event_addr_get(ic_sig_pin);
	err_code = nrf_drv_ppi_channel_assign(ppi_channel1, evt_addr, task_addr);
	APP_ERROR_CHECK(err_code);
	
	//ʹ��PPIͨ��������ʱ�жϵ�������ʱ��ֹͣ����
	task_addr = nrf_drv_timer_task_address_get(&counter_timer, NRF_TIMER_TASK_STOP);
	evt_addr = nrfx_timer_event_address_get(&time_timer, NRF_TIMER_EVENT_COMPARE0);
	err_code = nrf_drv_ppi_channel_assign(ppi_channel2, evt_addr, task_addr);
	APP_ERROR_CHECK(err_code);
	
	//PPIͨ��ʹ��
	err_code = nrf_drv_ppi_channel_enable(ppi_channel1);
	APP_ERROR_CHECK(err_code);
	err_code = nrf_drv_ppi_channel_enable(ppi_channel2);
	APP_ERROR_CHECK(err_code);
	
	if(cb != NULL)
		ic_cplt_cb = cb;
}

static void timer_ic_start(uint32_t time)
{
	if(ic_sig_pin >= 33) return;
	uint32_t time_tick = nrf_drv_timer_ms_to_ticks(&time_timer, time);
	nrf_drv_timer_extended_compare(&time_timer, NRF_TIMER_CC_CHANNEL0, 
								   time_tick, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);	//���ö�ʱ��ʱ��
	nrf_drv_gpiote_in_event_enable(ic_sig_pin, true);
	nrf_drv_timer_clear(&time_timer);
	nrf_drv_timer_clear(&counter_timer);
	nrf_drv_timer_enable(&time_timer);
	nrf_drv_timer_enable(&counter_timer);
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
	nrf_drv_ppi_uninit();
	nrf_drv_gpiote_in_uninit(ic_sig_pin);
	nrf_drv_timer_uninit(&time_timer);
	nrf_drv_timer_uninit(&counter_timer);
	ic_sig_pin = 33;
}

timer_ic_t* timer_ic_task_init(void)
{
	timer_ic.task_init = timer_ic_init;
	timer_ic.task_uninit = timer_ic_uninit;
	timer_ic.task_start = timer_ic_start;
	timer_ic.task_stop = timer_ic_stop;
	return &timer_ic;
}

timer_ic_t* imer_ic_task_get_handle(void)
{
	return &timer_ic;
}


