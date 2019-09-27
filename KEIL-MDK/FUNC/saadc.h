#ifndef __SAADC_H__
#define __SAADC_H__
#include "main.h"

/* �ڵ���ģʽ�£�SAADC������SAADC_SAMPLE_IN_BUFFER��Сʱֹͣ����
   ������ģʽ�£�SAADC������SAADC_SAMPLE_IN_BUFFER��Сʱ�������� */
#define SAADC_SAMPLE_MODE			0 //����ģʽ��0����ģʽ��1����ģʽ

/* �������ݻ����С����SAADC������˴�С����ʱ����һ�β�������¼�
   ���������в���ҪCPU���� */
#define SAADC_SAMPLE_IN_BUFFER 		5 //���ݻ����С

/* ���ò����������SAADC������һ�����ݺ�����ʱ�������һ������ */
#define SAADC_SAMPLE_TIME_INTERVAL 	1000 //�����������ݼ��ʱ����

#if (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_0)
#define SAADC_INPUT_AIN				NRF_SAADC_INPUT_AIN0 //P0.02����(ģ������ͨ��)
#elif (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_1)
#define SAADC_INPUT_AIN				NRF_SAADC_INPUT_AIN5 //P0.29����(ģ������ͨ��)
#endif

void saadc_init(void);
void saadc_unint(void);
void saadc_sample_start(void);
int16_t saadc_sample_result_get(void);

#endif


