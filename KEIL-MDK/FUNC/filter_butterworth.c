#include "filter_butterworth.h"

#define FILTER_ORDER_N		6			//过滤器阶数

const double b[] = {					//常系数
	0.00000000002759597545773583,
	0.000000000165575852746415,
	0.000000000413939631866037,
	0.000000000551919509154,
	0.00000000041393963186,
	0.00000000016557585274,
	0.00000000002759597545,
};

const double a[] = {
	-5.864155410764718,
	14.32997320081044,
	-18.67794553054013,
	13.69556264791875,
	-5.356404254274295,
	0.8729693486160918,
};

static uint32_t filter_nums = 0;
static double x_buf[FILTER_ORDER_N+1] = {0};
static double y_buf[FILTER_ORDER_N+1] = {0};

float filter_butter(float value, unsigned char reset_flag)
{
	uint8_t n;
	
	if(reset_flag == 1)
	{
		filter_nums = 0;
	}
	
	(++filter_nums > FILTER_ORDER_N) ? (n = FILTER_ORDER_N + 1) : (n = filter_nums);
	
	if(filter_nums > (FILTER_ORDER_N + 1))
	{
		for(int i = 0; i < FILTER_ORDER_N; i++)
		{
			x_buf[i] = x_buf[i+1];
			y_buf[i] = y_buf[i+1];
		}
		
		x_buf[FILTER_ORDER_N] = value;
	}
	else
	{
		x_buf[filter_nums-1] = value;
	}
	
	double x_sum = 0;
	for(int i = 0; i < n; i++)
	{
		x_sum += b[i] * x_buf[n-i-1];
	}
	
	double y_sum = 0;
	for(int i = 0; i < (n-1); i++)
	{
		y_sum += a[i] * y_buf[n-i-2];
	}

	return y_buf[n-1] = x_sum - y_sum;
}

