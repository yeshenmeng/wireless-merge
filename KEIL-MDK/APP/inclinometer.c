#include "inclinometer.h"
#include "sys_param.h"
#include <math.h>
#include <stdlib.h>
#include "sca100t.h"
#include "nrf_delay.h"


/* �����б�ǵ�Դ�꿪�� */
#define INCLINOMETER_POWER_ENBALE()		nrf_gpio_pin_set(INCLINOMETER_POWER_PIN)
#define INCLINOMETER_POWER_DISABLE()	nrf_gpio_pin_clear(INCLINOMETER_POWER_PIN)

/* SCA100Tϵ�������� */
#define	SCA100T_D01_RESOLUTION			1638	
#define	SCA100T_D02_RESOLUTION			819
#define SCA100T_D01_DATA_VALID_MIN		205
#define SCA100T_D01_DATA_VALID_MAX		1843

#define SCA100T_READ_DELAY				5 //SCA���ݶ�ȡʱ��
#define INCLINOMETER_POWER_DELAY		20 //�ϵ�ʱ��

/* SCAϵ�������� */
const uint16_t sca_resolution[] = {SCA100T_D01_RESOLUTION, SCA100T_D02_RESOLUTION};

/* SCA���������� */
static sca_type_t sca_type = SCA_D01;

/* ��б������״̬�� */
static inclinometer_task_state_t inclinometer_task_state;

/* ��б��������� */
static inclinometer_obj_t inclinometer_obj;

__weak void inclinometer_task_stop_handler(void* param);

static int compare_uint16(const void *data1, const void *data2)
{
	return *(uint16_t*)data1 - *(uint16_t*)data2;
}

/* ��ȡ��Ǵ������¶����� */
static float inclinometer_read_temp(void)
{
	float temp_digital = (float)sca_read_temp();
	return (temp_digital - 197) / (-1.083);
}

/* ��ȡ��Ǵ�����X������ 
   �����ȡ��������С��1024(����)��������㼴��ڣ�����1024����������������㼴�Ұ� */
static float inclinometer_read_x_angle(uint8_t read_times)
{
//	SCA_WriteCommand(STX);
	sca_write_cmd(MEAS);
	
	if(read_times == 0)
	{
		return 0.0;
	}
	
	uint16_t acceleration;
	uint16_t* p_value = (uint16_t*)malloc(sizeof(uint16_t)*read_times);
	
	for(int i=0; i<read_times; i++)
	{
		acceleration  = (uint16_t)sca_read_x_channel();
		if(acceleration<SCA100T_D01_DATA_VALID_MIN || acceleration>SCA100T_D01_DATA_VALID_MAX)
		{
			if(acceleration<SCA100T_D01_DATA_VALID_MIN)
			{
				acceleration = SCA100T_D01_DATA_VALID_MIN;
			}
			else
			{
				acceleration = SCA100T_D01_DATA_VALID_MAX;
			}
		}
		*(p_value+i) = acceleration;
		nrf_delay_ms(SCA100T_READ_DELAY);
	}
	
	qsort(p_value, read_times, sizeof(uint16_t), compare_uint16);

	uint32_t temp = 0;
	for(int i=1; i<read_times-1; i++)
	{
		temp += p_value[i];
	}
	float f_acceleration = temp / (read_times-2);
	free((void*)p_value);
	p_value = NULL;

	float angle = (asin((f_acceleration - 1024) / sca_resolution[sca_type])) * 180 /3.1415926;
	return angle;
}

/* ��ȡ��Ǵ�����Y������ */
static float inclinometer_read_y_angle(uint8_t read_times)
{
//	sca_write_cmd(STY);
//	sca_write_cmd(MEAS);
	
	if(read_times == 0)
	{
		return 0.0;
	}
	
	uint16_t acceleration;
	uint16_t* p_value = (uint16_t*)malloc(sizeof(uint16_t)*read_times);
	for(int i=0; i<read_times; i++)
	{
		acceleration = sca_read_y_channel();
		
		if(acceleration<SCA100T_D01_DATA_VALID_MIN || acceleration>SCA100T_D01_DATA_VALID_MAX)
		{
			if(acceleration<SCA100T_D01_DATA_VALID_MIN)
			{
				acceleration = SCA100T_D01_DATA_VALID_MIN;
			}
			else
			{
				acceleration = SCA100T_D01_DATA_VALID_MAX;
			}
		}
		*(p_value+i) = acceleration;
		nrf_delay_ms(SCA100T_READ_DELAY);
	}
	
	qsort(p_value, read_times, sizeof(uint16_t), compare_uint16);
	
	uint32_t temp = 0;
	for(int i=1; i<read_times-1; i++)
	{
		temp += p_value[i];
	}
	float f_acceleration = temp / (read_times-2);
	free((void*)p_value);
	p_value = NULL;

	float angle = (asin((f_acceleration - 1024) / sca_resolution[sca_type])) * 180 /3.1415926;
	return angle;
}

/* ��б���������� */
static void inclinometer_task_start(void)
{
	if(inclinometer_obj.state == INCLINOMETER_TASK_IDLE)
	{
		inclinometer_obj.lpm_obj->task_set_stat(INCLINOMETER_TASK_ID, LPM_TASK_STA_RUN);
		inclinometer_task_state = INCLINOMETER_TASK_ACTIVE;
	}
	return;
}

/* ��б������ֹͣ */
static void inclinometer_task_stop(void)
{
	if(inclinometer_obj.state != INCLINOMETER_TASK_IDLE)
	{
		inclinometer_task_state = INCLINOMETER_TASK_STOP;
	}
	return;	
}

/* ��б����������״̬���� */
static void inclinometer_task_operate(void)
{
	switch((uint8_t)inclinometer_task_state)
	{
		case INCLINOMETER_TASK_ACTIVE:
		{
			sca_init();
			INCLINOMETER_POWER_ENBALE(); //����ǲ�������
			nrf_delay_ms(INCLINOMETER_POWER_DELAY);
			inclinometer_task_state = INCLINOMETER_TASK_IDLE;
			inclinometer_obj.state = INCLINOMETER_TASK_ACTIVE;
			inclinometer_obj.lpm_obj->task_set_stat(INCLINOMETER_TASK_ID, LPM_TASK_STA_RUN);
			inclinometer_obj.data.x_angle = inclinometer_read_x_angle(5);
			inclinometer_obj.data.y_angle = inclinometer_read_y_angle(5);
			inclinometer_obj.data.temp = inclinometer_read_temp();
			/* ����XY��ǶȻ�׼���������ֵģʽ */
			static uint8_t angle_base = 0;
			if(angle_base == 0)
			{
				angle_base = 1;
				inclinometer_obj.data.x_angle_base = inclinometer_obj.data.x_angle;
				inclinometer_obj.data.y_angle_base = inclinometer_obj.data.y_angle;
			}
			inclinometer_obj.data.update_flag = 1;
			inclinometer_task_state = INCLINOMETER_TASK_STOP;
			break;
		}
		
		case INCLINOMETER_TASK_STOP:
		{
			inclinometer_task_state_t stateTmp;
			INCLINOMETER_POWER_DISABLE(); //�ر���ǲ�������
			sca_default();
			stateTmp = inclinometer_task_state;
			inclinometer_task_state = INCLINOMETER_TASK_IDLE;
			inclinometer_obj.state = INCLINOMETER_TASK_IDLE;
			inclinometer_obj.lpm_obj->task_set_stat(INCLINOMETER_TASK_ID, LPM_TASK_STA_STOP);
			inclinometer_task_stop_handler((void *)&stateTmp); //��Ǵ���������ֹͣ����
			break;
		}
	}
	
	return;	
}

/* ��б��Ӳ������ */
static void inclinometer_cfg(void)
{
	nrf_gpio_cfg(INCLINOMETER_POWER_PIN,
				 NRF_GPIO_PIN_DIR_OUTPUT,
				 NRF_GPIO_PIN_INPUT_DISCONNECT,
				 NRF_GPIO_PIN_NOPULL,
				 NRF_GPIO_PIN_S0S1,
				 NRF_GPIO_PIN_NOSENSE);
	
//	INCLINOMETER_POWER_DISABLE();
	INCLINOMETER_POWER_ENBALE();
	nrf_delay_ms(INCLINOMETER_POWER_DELAY);
}

/* ��б�������ʼ�� */
inclinometer_obj_t* inclinometer_task_init(lpm_obj_t* lpm_obj)
{
	/* ��ʼ��Ӳ���봫���� */
	sca_type = SCA_D01;
	sca_init();
	inclinometer_cfg();
	sca_write_cmd(STX);
	sca_write_cmd(STY);
	INCLINOMETER_POWER_DISABLE();
	sca_default();
	
	/* ��ʼ��Ĭ�ϲ��� */
	inclinometer_task_state = INCLINOMETER_TASK_IDLE;
	inclinometer_obj.state = INCLINOMETER_TASK_IDLE;
	inclinometer_obj.data.update_flag = 0;
	inclinometer_obj.data.temp = 255;
	inclinometer_obj.data.x_angle = 255;
	inclinometer_obj.data.y_angle = 255;
	sys_param_t* param = sys_param_get_handle();
	inclinometer_obj.data.x_angle_threshold = param->iot_x_angle_threshold;
	inclinometer_obj.data.y_angle_threshold = param->iot_y_angle_threshold;
	inclinometer_obj.data.x_angle_base = 0;
	inclinometer_obj.data.y_angle_base = 0;
	inclinometer_obj.lpm_obj = lpm_obj;
	
	/* ��ʼ��APIָ�� */
	inclinometer_obj.task_start = inclinometer_task_start;
	inclinometer_obj.task_stop = inclinometer_task_stop;
	inclinometer_obj.task_operate = inclinometer_task_operate;
	inclinometer_obj.read_temp = inclinometer_read_temp;
	inclinometer_obj.read_x_angle = inclinometer_read_x_angle;
	inclinometer_obj.read_y_angle = inclinometer_read_y_angle;

	/* ע��������� */
	inclinometer_obj.lpm_obj->task_reg(INCLINOMETER_TASK_ID);
	
	/* ���������� */
	return &inclinometer_obj;
}

/* ��б������ֹͣ���� */
__weak void inclinometer_task_stop_handler(void* param)
{
	return;
}





