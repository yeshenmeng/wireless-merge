#ifndef __SAADC_H__
#define __SAADC_H__
#include "main.h"

/* 在单次模式下，SAADC采样完SAADC_SAMPLE_IN_BUFFER大小时停止采样
   在连续模式下，SAADC采样完SAADC_SAMPLE_IN_BUFFER大小时继续采样 */
#define SAADC_SAMPLE_MODE			0 //采样模式：0单次模式，1连续模式

/* 设置数据缓存大小，当SAADC采样完此大小数据时产生一次采样完成事件
   采样过程中不需要CPU参与 */
#define SAADC_SAMPLE_IN_BUFFER 		5 //数据缓存大小

/* 设置采样间隔，当SAADC采样完一个数据后间隔此时间采样下一个数据 */
#define SAADC_SAMPLE_TIME_INTERVAL 	1000 //采样两个数据间的时间间隔

#if (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_0)
#define SAADC_INPUT_AIN				NRF_SAADC_INPUT_AIN0 //P0.02引脚(模拟输入通道)
#elif (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_1)
#define SAADC_INPUT_AIN				NRF_SAADC_INPUT_AIN5 //P0.29引脚(模拟输入通道)
#endif

void saadc_init(void);
void saadc_unint(void);
void saadc_sample_start(void);
int16_t saadc_sample_result_get(void);

#endif


