#ifndef __COLLAPSE_H__
#define __COLLAPSE_H__
#include "main.h"
#include "low_power_manage.h"


#define COLLAPSE_FIFO_IN_PERIOD				500				//��������ʱ��FIFOд�����ݵ�����

#define BMA456_USE_FIFO						1 				//0������FIFO���ܣ�1��ʹ��FIFO����
#define BMA456_WTM_SIZE						1000			//����FIFOˮӡ��С��ʵ������ֵ�ȴ�ֵС20�ֽ�

#if (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_0)
/* ���ģ��SPI�������� */
#define COLLAPSE_SW_SPI_SCK_PIN				15
#define COLLAPSE_SW_SPI_MOSI_PIN			16
#define COLLAPSE_SW_SPI_MISO_PIN			17
#define COLLAPSE_SW_SPI_CS_PIN				18
#define COLLAPSE_INT1_PIN					6 //���ٶȼ��ж��ź�1����
#define COLLAPSE_INT2_PIN					5 //���ٶȼ��ж��ź�2����
#elif (SYS_HW_VERSION == SYS_HW_VERSION_V0_1_1)
/* ���ģ��SPI�������� */
#define COLLAPSE_SW_SPI_SCK_PIN				14
#define COLLAPSE_SW_SPI_MOSI_PIN			12
#define COLLAPSE_SW_SPI_MISO_PIN			13
#define COLLAPSE_SW_SPI_CS_PIN				15
#define COLLAPSE_INT1_PIN					11 //���ٶȼ��ж��ź�1����
#define COLLAPSE_INT2_PIN					10 //���ٶȼ��ж��ź�2����
#endif

/* ���ģ��SPI�˿����� */
#define COLLAPSE_SW_SPI_SCK_PORT			P0
#define COLLAPSE_SW_SPI_MOSI_PORT			P0
#define COLLAPSE_SW_SPI_MISO_PORT			P0
#define COLLAPSE_SW_SPI_CS_PORT				P0

#define COLLAPSE_SW_SPI_SCK_SET()			nrf_gpio_pin_set(COLLAPSE_SW_SPI_SCK_PIN)
#define COLLAPSE_SW_SPI_SCK_CLR()			nrf_gpio_pin_clear(COLLAPSE_SW_SPI_SCK_PIN)
#define COLLAPSE_SW_SPI_MOSI_SET()			nrf_gpio_pin_set(COLLAPSE_SW_SPI_MOSI_PIN)
#define COLLAPSE_SW_SPI_MOSI_CLR()			nrf_gpio_pin_clear(COLLAPSE_SW_SPI_MOSI_PIN)
#define COLLAPSE_SW_SPI_CS_ENABLE()			nrf_gpio_pin_clear(COLLAPSE_SW_SPI_CS_PIN)
#define COLLAPSE_SW_SPI_CS_DISABLE()		nrf_gpio_pin_set(COLLAPSE_SW_SPI_CS_PIN)
#define COLLAPSE_SW_SPI_MISO_READ() 		nrf_gpio_pin_read(COLLAPSE_SW_SPI_MISO_PIN)

typedef enum {
	COLLAPSE_IDLE,
	COLLAPSE_ACTIVE,
	COLLAPSE_DATA,
	COLLAPSE_STOP,
}collapse_state_t;

typedef enum {
	PERIOD_MODE = 0,
	TRIGGER_MODE,
}sens_mode_t;

typedef __packed struct {
	float x_accel;
	float y_accel;
	float z_accel;
}sens_accel_t;

typedef __packed struct {
	float x_angle;
	float y_angle;
	float z_angle;
}sens_angle_t;

typedef __packed struct {
	float temp_c;
	sens_accel_t accel;
	sens_angle_t angle;
}sens_data_t;

typedef struct {
	uint8_t update_flag;
	sens_mode_t mode;
	uint32_t period;
	uint16_t accel_slope_threshold;
	uint16_t consecutive_data_points;
	sens_data_t data;
	
	collapse_state_t state;
	
	lpm_obj_t* lpm_obj;
	
	void (*task_start)(void);
	void (*task_stop)(void);
	void (*task_operate)(void);
	
	void (*iot_set_period)(void);
	uint16_t (*iot_set_accel_slope_threshold)(void);
	uint16_t (*iot_set_consecutive_data_points)(void);
	uint16_t (*iot_set_mode)(void);
}collapse_obj_t;

collapse_obj_t* collapse_init(lpm_obj_t* lpm_obj);

#endif


