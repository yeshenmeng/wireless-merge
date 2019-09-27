#ifndef __INCLINOMETER_H__
#define __INCLINOMETER_H__
#include "main.h"
#include "low_power_manage.h"


#define INCLINOMETER_POWER_PIN			3
#define INCLINOMETER_POWER_PORT			P0

typedef enum {
	SCA_D01,
	SCA_D02,
	SCA_D07,
}sca_type_t;

typedef enum {
	INCLINOMETER_TASK_IDLE,
	INCLINOMETER_TASK_ACTIVE,
	INCLINOMETER_TASK_STOP,
}inclinometer_task_state_t;

typedef struct {
	uint8_t update_flag;
	float temp;
	float x_angle;
	float y_angle;
	float x_angle_threshold;
	float y_angle_threshold;
	float x_angle_base;
	float y_angle_base;
}inclinometer_data_t;

typedef struct {
	inclinometer_task_state_t state;
	inclinometer_data_t data;
	lpm_obj_t* lpm_obj;
	
	void (*task_start)(void);
	void (*task_stop)(void);
	void (*task_operate)(void);
	float (*read_temp)(void);
	float (*read_x_angle)(uint8_t read_times);
	float (*read_y_angle)(uint8_t read_times);
}inclinometer_obj_t;

inclinometer_obj_t* inclinometer_task_init(lpm_obj_t* lpm_obj);


#endif


