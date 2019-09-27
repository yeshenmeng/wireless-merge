#ifndef __SIGNAL_DETECT_H__
#define __SIGNAL_DETECT_H__
#include "main.h"
#include "nrf_drv_lpcomp.h"


//#define SIGNAL_DET_USE_INT_COPM //使用内部比较器检测信号
#define SIGNAL_DET_USE_EXT_COPM //使用外部比较器检测信号

#ifdef SIGNAL_DET_USE_INT_COPM
	#undef SIGNAL_DET_USE_EXT_COPM
#elif defined(SIGNAL_DET_USE_EXT_COPM)
	#undef SIGNAL_DET_USE_INT_COPM
#endif

/* 使用外部中断信号 */
#ifdef SIGNAL_DET_USE_EXT_COPM
#if (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_0)
	#define SIGNAL_LINE_GPIO_PIN				29
#elif (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_1)
	#define SIGNAL_LINE_GPIO_PIN				22
#endif
	#define SIGNAL_LINE_GPIO_PORT				P0
	#define SIGNAL_DETECT_EXTI_IRQHandler		NULL
	#define SIGNAL_LINE_INT_ENABLE()			nrf_drv_gpiote_in_event_enable(SIGNAL_LINE_GPIO_PIN, true)
	#define SIGNAL_LINE_INT_DISABLE()			nrf_drv_gpiote_in_event_disable(SIGNAL_LINE_GPIO_PIN)
#endif

/* 使用内部低功耗比较器 */
#ifdef SIGNAL_DET_USE_INT_COPM					
	#define SIGNAL_DET_AIN_INPUT				NRF_LPCOMP_INPUT_5 //P0.28引脚
#endif

/* 外部信号检测电源控制 */
#if (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_0)
#define SIGNAL_DET_POWER_PIN					30
#elif (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_1)
#define SIGNAL_DET_POWER_PIN					28
#endif
#define SIGNAL_DET_POWER_PORT					P0
#define SIGNAL_DET_POWER_ENBALE()				nrf_gpio_pin_set(SIGNAL_DET_POWER_PIN)
#define SIGNAL_DET_POWER_DISABLE()				nrf_gpio_pin_clear(SIGNAL_DET_POWER_PIN)


void hw_signal_detect_init(void);

#endif


