#include <stdlib.h>
#include "data_sort.h"

int compare_small_to_big_32(const void *data1, const void *data2)
{
	return *(int*)data1 - *(int*)data2;
}

int compare_big_to_small_32(const void *data1, const void *data2)
{
	return *(int*)data2 - *(int*)data1;
}

int compare_small_to_big_16(const void *data1, const void *data2)
{
	return *(signed short*)data1 - *(signed short*)data2;
}

int compare_big_to_small_16(const void *data1, const void *data2)
{
	return *(signed short*)data2 - *(signed short*)data1;
}

int compare_small_to_big_8(const void *data1, const void *data2)
{
	return *(signed char*)data1 - *(signed char*)data2;
}

int compare_big_to_small_8(const void *data1, const void *data2)
{
	return *(signed char*)data2 - *(signed char*)data1;
}

/**
 * @brief  数组排序
 *         支持8位、16位、32位整形数据排序
 * @param  pData: 数据
 * @param  size: 数据长度
 * @param  width: 数据单位
 * @param  dir: 0:从小到大  1:从大到小
 * @retval None
 */
void data_sort(void *p_data, int size, int width, unsigned char dir)
{
	if(width == 4)
	{
		if(!dir)
			qsort(p_data, size, width, compare_small_to_big_32);
		else
			qsort(p_data, size, width, compare_big_to_small_32);
	}
	else if(width == 2)
	{
		if(!dir)
			qsort(p_data, size, width, compare_small_to_big_16);
		else
			qsort(p_data, size, width, compare_big_to_small_16);
	}
	else if(width == 1)
	{
		if(!dir)
			qsort(p_data, size, width, compare_small_to_big_8);
		else
			qsort(p_data, size, width, compare_big_to_small_8);
	}
}






