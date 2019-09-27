#ifndef __LORA_TRANSMISSION_H__
#define __LORA_TRANSMISSION_H__
#include "main.h"
#include "low_power_manage.h"


/* LORA通信状态 */
#define LORA_OUT_STATE_OFFLINE					0X01 //未连接
#define LORA_OUT_STATE_DISCON					0X02 //从连接态断开
#define LORA_OUT_STATE_LINK						0X03 //从未连接到连接态
#define LORA_OUT_STATE_CONNECT					0X04 //连接态

#if (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_0)
/* LORA模块引脚配置 */
#define LORA_POWER_PIN							9 //LORA模块供电引脚
#define LORA_TRANSMIT_PIN						7 //LORA发送
#define LORA_RECEIVE_PIN						8 //LORA接收
#define LORA_RESET_PIN							25 //LORA复位引脚
#define LORA_BUSY_PIN							26 //LORA BUSY引脚
#define LORA_IRQ_PIN							27 //LORA中断引脚

/* LORA_SPI引脚配置 */
#define LORA_SPI_CS_PIN							11 //LORA_SPI片选引脚
#define LORA_SPI_SCK_PIN    					12 //LORA_SPI时钟引脚
#define LORA_SPI_MISO_PIN   					14 //LORA_SPI主入从出引脚
#define LORA_SPI_MOSI_PIN   					13 //LORA_SPI主出从入引脚
#elif (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_1)
/* LORA模块引脚配置 */
#define LORA_POWER_PIN							9 //LORA模块供电引脚
#define LORA_TRANSMIT_PIN						2 //LORA发送
#define LORA_RECEIVE_PIN						3 //LORA接收
#define LORA_RESET_PIN							8 //LORA复位引脚
#define LORA_BUSY_PIN							30 //LORA BUSY引脚
#define LORA_IRQ_PIN							31 //LORA中断引脚

/* LORA_SPI引脚配置 */
#define LORA_SPI_CS_PIN							4 //LORA_SPI片选引脚
#define LORA_SPI_SCK_PIN    					5 //LORA_SPI时钟引脚
#define LORA_SPI_MISO_PIN   					7 //LORA_SPI主入从出引脚
#define LORA_SPI_MOSI_PIN   					6 //LORA_SPI主出从入引脚
#endif

/* LORA引脚功能状态宏定义 */
#define LORA_POWER_ENABLE()						nrf_gpio_pin_set(LORA_POWER_PIN) //使能LORA模块
#define LORA_POWER_DISABLE()					nrf_gpio_pin_clear(LORA_POWER_PIN) //失能LORA模块
#define LORA_TRANSMIT_ENABLE()					nrf_gpio_pin_set(LORA_TRANSMIT_PIN) //使能LORA发送
#define LORA_TRANSMIT_DISABLE()					nrf_gpio_pin_clear(LORA_TRANSMIT_PIN) //失能LORA发送
#define LORA_RECEIVE_ENABLE()					nrf_gpio_pin_set(LORA_RECEIVE_PIN) //使能LORA接收
#define LORA_RECEIVE_DISABLE()					nrf_gpio_pin_clear(LORA_RECEIVE_PIN) //失能LORA接收
#define LORA_RESET_ENABLE()						nrf_gpio_pin_clear(LORA_RESET_PIN) //使能LORA复位
#define LORA_RESET_DISABLE()					nrf_gpio_pin_set(LORA_RESET_PIN) //失能LORA复位
#define LORA_READ_STATUS()						nrf_gpio_pin_read(LORA_BUSY_PIN) //读取LORA忙碌状态
#define LORA_READ_IRQ_STATUS()					nrf_gpio_pin_read(LORA_IRQ_PIN) //读取中断引脚状态
#define LORA_SPI_CS_ENABLE()					nrf_gpio_pin_clear(LORA_SPI_CS_PIN) //LORA SPI片选使能
#define LORA_SPI_CS_DISABLE()					nrf_gpio_pin_set(LORA_SPI_CS_PIN) //LORA SPI片选失能

typedef enum {
	LORA_IDLE,
	LORA_ACTIVE,
	LORA_TX_SUCCESS,
	LORA_TX_FAIL,
	LORA_STOP,
}lora_state_t;

typedef struct {
	uint32_t task_time_slice;
	uint32_t rx_timeout_base;
	uint32_t tx_max_fail_times;
	
	uint32_t delay_base_time;
	uint32_t random_delay_upper;
	uint32_t random_delay_lower;
	uint32_t tx_max_delay_time;
	
	/* 控制参数 */
	FunctionalState is_idle_enter_lp; //DISABLE：LORA空闲状态不动作，ENABLE：LORA空闲状态进入低功耗模式
	
	/* 被动参数 */
	uint32_t tx_fail_times;
}Lora_Param_t;

typedef struct {
	Lora_Param_t param;
	lora_state_t state;
	
	lpm_obj_t* lpm_obj;
	
	void (*task_start)(void);
	void (*task_stop)(void);
	void (*set_tx_data)(uint8_t* p_data, uint8_t size);
	void (*task_operate)(void);
	int8_t (*get_rssi)(void);
}lora_obj_t;

lora_obj_t* lora_task_init(lpm_obj_t* lpm_obj);
void lora_reset(void);
void lora_spi_transmit_receive(uint8_t* tx_buffer, uint8_t tx_length, uint8_t* rx_buffer, uint8_t rx_length);

#endif


