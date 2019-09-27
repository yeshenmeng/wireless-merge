/*
 * function.c
 *
 *  Created on: 2019年3月20日
 *      Author: xxk
 */
#include "function.h"
#include "lora_transmission.h"

//spi收发数据
uint8_t spi_rw(uint8_t data)
{
	uint8_t txBuf[1] = {data};
	uint8_t rxBuf[1];
	lora_spi_transmit_receive(txBuf,1,rxBuf,1);
	return rxBuf[0];
}

//设置模块复位信号
void rst_pin_set (uint8_t val)
{
	if (val == 1) 
	{
		LORA_RESET_DISABLE();
	}
	else if (val == 0) 
	{
		LORA_RESET_ENABLE();
	}
}

//设置片选信号
void sel_pin_set (uint8_t val)
{
	if (val == 1) 
	{
		LORA_SPI_CS_DISABLE();
	}
	else if (val == 0) 
	{
		LORA_SPI_CS_ENABLE();
	}
}

//busy
void check_busy(void)
{
	while(LORA_READ_STATUS() == 1){};
}
	
//返回DIO1的电平状况
uint8_t dio1_pin_read (void)
{
	return LORA_READ_IRQ_STATUS();
}

//毫秒延时
void delay_ms(uint16_t ms)
{
	nrf_delay_ms(ms);
}




