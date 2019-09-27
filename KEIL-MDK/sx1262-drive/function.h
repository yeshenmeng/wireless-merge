/*
 * function.h
 *
 *  Created on: 2019年3月20日
 *      Author: xxk
 */

#ifndef FUNCTION_H_
#define FUNCTION_H_
#include "sx1262_regs.h"
#include <stdint.h>
#include "nrf_gpio.h"
#include "nrf_delay.h"

//extern  sx1262_CADParams_t __sx1262_CAD_param;
//extern  sx1262_lora_param_set_t __sx1262_lora_param;

void gpio_init(void);

//spi2收发数据
uint8_t spi_rw(uint8_t data);

//设置模块复位信号
void 	rst_pin_set (uint8_t val);

//设置片选信号
void 	sel_pin_set (uint8_t val);

//返回DIO0的电平状况
uint8_t dio1_pin_read (void);

//busy
void check_busy(void);

//延时函数
void delay_ms(uint16_t ms);

#endif /* FUNCTION_H_ */
