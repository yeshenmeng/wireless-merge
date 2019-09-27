#include "signal_detect.h"
#include "nrf_drv_gpiote.h"


/* 信号检测中断事件处理 */
__weak void hw_signal_detect_evt_handler(void* param){};

/* 信号检测功能电源配置 */
static void signal_detect_cfg(void)
{
	nrf_gpio_cfg(SIGNAL_DET_POWER_PIN,
				 NRF_GPIO_PIN_DIR_OUTPUT,
				 NRF_GPIO_PIN_INPUT_DISCONNECT,
				 NRF_GPIO_PIN_NOPULL,
				 NRF_GPIO_PIN_S0S1,
				 NRF_GPIO_PIN_NOSENSE);
	
	SIGNAL_DET_POWER_DISABLE();
}

/* 低功耗比较器事件处理 */
#ifdef SIGNAL_DET_USE_INT_COPM
static void lpcomp_evt_handler(nrf_lpcomp_event_t event)
{
	if (event == NRF_LPCOMP_EVENT_UP)
	{
		hw_signal_detect_evt_handler(NULL);
	}
}

/* 低功耗比较器初始化 */
static void lpcomp_init(void)
{
	uint32_t err_code;
	nrf_drv_lpcomp_config_t config;
	
	config.hal.reference = NRF_LPCOMP_REF_SUPPLY_1_16;
	config.hal.detection = NRF_LPCOMP_DETECT_UP;
	config.hal.hyst = NRF_LPCOMP_HYST_NOHYST;
	config.input = SIGNAL_DET_AIN_INPUT;
	config.interrupt_priority = LPCOMP_CONFIG_IRQ_PRIORITY;
	/* initialize LPCOMP driver, from this point LPCOMP will be active and provided
	   event handler will be executed when defined action is detected */
	err_code = nrf_drv_lpcomp_init(&config, lpcomp_evt_handler);
	APP_ERROR_CHECK(err_code);
	nrf_drv_lpcomp_enable();
}
#endif

/* 信号中断处理 */
#ifdef SIGNAL_DET_USE_EXT_COPM
static void gpiote_in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	if(pin == SIGNAL_LINE_GPIO_PIN)
	{
		if(action == GPIOTE_CONFIG_POLARITY_LoToHi)
		{
			SIGNAL_LINE_INT_DISABLE();
			SIGNAL_DET_POWER_DISABLE();
			hw_signal_detect_evt_handler(NULL);
		}
	}
}

/* 外部中断配置 */
static void signal_ext_int_cfg(void)
{
	nrfx_gpiote_in_config_t nrfx_gpiote_in_config;
	
	if (!nrf_drv_gpiote_is_init())
	{
		uint32_t err_code;
		err_code = nrf_drv_gpiote_init();
		APP_ERROR_CHECK(err_code);
	}	
	
	nrfx_gpiote_in_config.sense = NRF_GPIOTE_POLARITY_LOTOHI;
	nrfx_gpiote_in_config.is_watcher = false;
	nrfx_gpiote_in_config.pull = NRF_GPIO_PIN_PULLDOWN; //下拉
	nrfx_gpiote_in_config.hi_accuracy = true;
	nrf_drv_gpiote_in_init(SIGNAL_LINE_GPIO_PIN,
						   &nrfx_gpiote_in_config,
						   &gpiote_in_pin_handler);
	
	SIGNAL_LINE_INT_ENABLE();
}
#endif

/* 信号检测功能初始化 */
void hw_signal_detect_init(void)
{
	signal_detect_cfg();
	
#ifdef SIGNAL_DET_USE_INT_COPM
	lpcomp_init();
#endif
	
#ifdef SIGNAL_DET_USE_EXT_COPM
	SIGNAL_DET_POWER_ENBALE();
	signal_ext_int_cfg();
#endif
}





