#include "calendar.h"
#include "time.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"
#include "nrfx_rtc.h"
#include "string.h"


static calendar_mod_t m_calendar;
static date_t m_time;
static uint8_t m_time_count_1s = 0;
static volatile time_t m_timestamp = 0;
static volatile uint32_t m_run_time = 0;

const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(2); /**< Declaring an instance of nrf_drv_rtc for RTC2. */
const char Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#if RTC_SECONED_INT_EN != 1	
volatile uint32_t m_ticks = 0;
volatile uint32_t m_pre_count = 0;
static uint32_t m_timestamp_base = 0;
#endif


/* time_stamp:0->1970-1-1-8:0:0 */
void calendar_time_stamp_to_date(time_t time, date_t* date)
{
	unsigned int Pass4year;
	int hours_per_year;

	/* ������ʱ��ȸ�������ʱ���8��Сʱ */
	time += (3600 * 8);
	
	date->second=(int)(time % 60); //ȡ��ʱ��
	time /= 60;
	
	date->minute=(int)(time % 60); //ȡ����ʱ��
	time /= 60;
	
	Pass4year=((unsigned int)time / (1461L * 24L)); //ȡ��ȥ���ٸ����꣬ÿ������ 1461*24 Сʱ
	date->year=(Pass4year << 2) + 1970; //�������
	time %= 1461L * 24L; //������ʣ�µ�Сʱ��
	
	//У������Ӱ�����ݣ�����һ����ʣ�µ�Сʱ��
	for (;;)
	{
		hours_per_year = 365 * 24; //һ���Сʱ��
		
		//�ж�����
		if ((date->year & 3) == 0)
		{
			hours_per_year += 24; //�����꣬һ�����24Сʱ����һ��
		}
		
		if (time < hours_per_year)
		{
			break;
		}
		
		date->year++;
		time -= hours_per_year;
	}
	
	date->hour=(int)(time % 24); //Сʱ��
	
	time /= 24; //һ����ʣ�µ�����
	time++; //�ٶ�Ϊ����
	
	//У��������������·ݣ�����
	if((date->year & 3) == 0)
	{
		if (time > 60)
		{
			time--;
		}
		else
		{
			if (time == 60)
			{
				date->month = 1;
				date->day = 29;
				return ;
			}
		}
	}
	
	//��������
	for (date->month = 0; Days[date->month] < time;date->month++)
	{
		time -= Days[date->month];
	}
	
	date->month += 1;
	date->day = (int)(time);
 
	return;
}

static time_t mon_yday[2][12] =
{
	{0,31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
	{0,31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
};

int is_leap(int year)
{
	return (year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0);
}

/* 1970-1-1-8:0:0->time_stamp:0 */
time_t calendar_date_to_time_stamp(date_t* date)
{
	time_t result;
	int i =0;
	
	//��ƽ��ʱ����������
	result = (date->year - 1970) * 365 * 24 * 3600 +
			 (mon_yday[is_leap(date->year)][date->month-1] + date->day - 1) * 24 * 3600 +
			  date->hour * 3600 + date->minute * 60 + date->second;
	
	for(i=1970; i < date->year; i++) // �������������
	{
		if(is_leap(i))
		{
			result += 24 * 3600;
		}
	}
	
	/* ������ʱ��ȸ�������ʱ���8��Сʱ */
	result -= (3600 * 8);
	
	return(result);
}

/** @brief: Function for handling the RTC2 interrupts.
 * Triggered on TICK and COMPARE0 match.
 */
static void calendar_rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
	if(int_type == NRF_DRV_RTC_INT_COMPARE0)
	{
	}
	else if (int_type == NRF_DRV_RTC_INT_TICK)
	{
		if(m_time_count_1s >= 7 )//?D???��?��125ms��??����?125*8 =1000s
		{
			m_time_count_1s = 0;
			m_timestamp++;
			m_run_time++;
			calendar_time_stamp_to_date(m_timestamp, &m_time);
		}
		else
		{
			m_time_count_1s++;
		}
	}
}

#if RTC_SECONED_INT_EN != 1
static uint32_t ticks_elapse_get(void)
{
	uint32_t count = NRF_RTC2->COUNTER & 0x00FFFFFF;
	uint32_t elapse = count - m_pre_count;
	m_pre_count = count;
	if(count > 0X00EFFFFF)
	{
		m_pre_count = 0;
		nrf_drv_rtc_counter_clear(&rtc);
	}
	return elapse;
}
#endif

#if RTC_SECONED_INT_EN == 1	
static time_t calendar_get_time_stamp(void)
{
	return m_timestamp;
}
#else
/* �ڷ��ж�ģʽ�£����ô˺�������ʱ��� */
time_t calendar_get_time_stamp(void)
{
	uint32_t elapse = ticks_elapse_get(); //������ȥ��ʱ��
	m_run_time += elapse;
	m_ticks += elapse;
	m_timestamp = m_timestamp_base + m_ticks / 8; //ʱ���׼+�Ѿ���ȥ����ʱ��=��ǰʱ���
	calendar_time_stamp_to_date(m_timestamp, &m_time); //��������
	return m_timestamp;
}
#endif

#if RTC_SECONED_INT_EN == 1	
static void calendar_set_time_stamp(time_t time_stamp)
{
	m_timestamp = time_stamp;
}
#else
static void calendar_set_time_stamp(time_t time_stamp)
{
	m_run_time += ticks_elapse_get(); //������ȥ��ʱ��
	m_ticks = 0;
	m_pre_count = 0;
	nrf_drv_rtc_counter_clear(&rtc);
	m_timestamp_base = time_stamp; //ʱ���׼����
}
#endif

static uint32_t calendar_get_run_time(void)
{
#if RTC_SECONED_INT_EN == 0
	calendar_get_time_stamp();
#endif
	return m_run_time / 8;
}

/** @brief Function initialization and configuration of RTC driver instance.
 */
static void calendar_rtc2_cfg(void)
{
	uint32_t err_code;
			
	//Initialize RTC instance
	nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
	config.prescaler = 4095; 
	
	err_code = nrf_drv_rtc_init(&rtc, &config, calendar_rtc_handler);
	APP_ERROR_CHECK(err_code);

#if RTC_SECONED_INT_EN == 1	
	//Enable tick event & interrupt
	nrf_drv_rtc_tick_enable(&rtc, true);
#endif
	
	//Power on RTC instance
	nrf_drv_rtc_enable(&rtc);
}

void calendar_init(void)
{
	calendar_rtc2_cfg();
	
	m_calendar.date = &m_time;
	m_calendar.get_time_stamp = calendar_get_time_stamp;
	m_calendar.set_time_stamp = calendar_set_time_stamp;
	m_calendar.get_run_time = calendar_get_run_time;
}

calendar_mod_t* calendar_get_handle(void)
{
	return &m_calendar;
}



