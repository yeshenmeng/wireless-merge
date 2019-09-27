#ifndef __CALENDAR_H__
#define __CALENDAR_H__
#include "main.h"


#define	RTC_SECONED_INT_EN			0 /* 0：RTC秒中断失能 1：RTC秒中断使能*/

typedef unsigned int time_t;
 
typedef struct{
	int second; /* 秒 – 取值区间为[0,59] */
	int minute; /* 分 - 取值区间为[0,59] */
	int hour; /* 时 - 取值区间为[0,23] */
	int day; /* 一个月中的日期 - 取值区间为[1,31] */
	int month; /* 月份 - 取值区间为[1,12] */
	int year; /* 年份 - 取值区间为[1970.1.1,。。。] */
}date_t;

typedef struct{
	date_t* date;
	
	time_t (*get_time_stamp)(void);
	void (*set_time_stamp)(time_t time_stamp);
	uint32_t (*get_run_time)(void);
}calendar_mod_t;

void calendar_time_stamp_to_date(time_t time, date_t* date);
time_t calendar_date_to_time_stamp(date_t* date);

void calendar_init(void);
calendar_mod_t* calendar_get_handle(void);

#endif









