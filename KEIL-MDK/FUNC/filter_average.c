#include <stdlib.h>
#include "filter_average.h"

/* @param��convert��������ת��
 * exp��	float UInt32_Convert(void* value)
 {
 return *(uint32_t *)(value);
 }
 */
/**
 * @brief  ���ݾ�ֵ�˲�
 * @param  pData: ����
 * @param  size: ���ݴ�С
 * @param  width: ���ݵ�λ
 * @param  convert: ��������ת���ӿ�
 * 			                       ��������ֵ�������Ǹ����ͣ�float Float_Convert(void* value) {return *(float *)(value);}
 * @retval ƽ��ֵ
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

