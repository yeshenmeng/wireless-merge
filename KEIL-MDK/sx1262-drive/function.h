/*
 * function.h
 *
 *  Created on: 2019��3��20��
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

//spi2�շ�����
uint8_t spi_rw(uint8_t data);

//����ģ�鸴λ�ź�
void 	rst_pin_set (uint8_t val);

//����Ƭѡ�ź�
void 	sel_pin_set (uint8_t val);

//����DIO0�ĵ�ƽ״��
uint8_t dio1_pin_read (void);

//busy
void check_busy(void);

//��ʱ����
void delay_ms(uint16_t ms);

#endif /* FUNCTION_H_ */
