#include <stdlib.h>
#include "filter_average.h"

/* @param：convert数据类型转换
 * exp：	float UInt32_Convert(void* value)
 {
 return *(uint32_t *)(value);
 }
 */
/**
 * @brief  数据均值滤波
 * @param  pData: 数据
 * @param  size: 数据大小
 * @param  width: 数据单位
 * @param  convert: 数据类型转换接口
 * 			                       例如待求均值的数据是浮点型：float Float_Convert(void* value) {return *(float *)(value);}
 * @retval 平均值
 */
float filter_average(void *p_data, int size, int width, float convert(void* value)) {
	float sum = 0;
	unsigned char *p_char = p_data;

	for (int i = 0; i < size; i++) {
		sum += convert(p_char);
		p_char += width;
	}

	return sum / size;
}

