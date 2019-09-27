#ifndef __IOT_OPERATE_H__
#define __IOT_OPERATE_H__
#include "main.h"
#include "sys_param.h"
#include "iotobject.h"
#include "collapse.h"
#include "lora_transmission.h"


/* 倾角IOT协议属性 */
#define IOT_I_LONG_ADDR_ID						1
#define IOT_I_SHORT_ADDR_ID						2
#define IOT_I_SAMPLE_MODE_ID					3
#define IOT_I_SAMPLE_INTERVAL_ID				4
#define IOT_I_TIME_STAMP_ID						5
#define IOT_I_BATTERY_LEVEL_ID					6
#define IOT_I_TEMPERATURE_ID					7
#define IOT_I_DATA_X_ANGLE_ID 					8
#define IOT_I_DATA_Y_ANGLE_ID 					9
#define IOT_I_DATA_Z_ANGLE_ID 					10
#define IOT_I_DATA_X_THRESHOLD_ID 				11
#define IOT_I_DATA_Y_THRESHOLD_ID 				12
#define IOT_I_DATA_Z_THRESHOLD_ID 				13

/* 崩塌计IOT协议属性 */
#define IOT_C_LONG_ADDR_ID						1
#define IOT_C_SHORT_ADDR_ID						2
#define IOT_C_SAMPLE_MODE_ID					3
#define IOT_C_SAMPLE_INTERVAL_ID				4
#define IOT_C_TIME_STAMP_ID						5
#define IOT_C_ACCEL_SLOPE_THRESHOLD_ID 			6
#define IOT_C_CONSECUTIVE_DATA_POINTS_ID		7
#define IOT_C_BATTERY_LEVEL_ID					8
#define IOT_C_TEMPERATURE_ID					9
#define IOT_C_DATA_X_ACCEL_ID 					10
#define IOT_C_DATA_Y_ACCEL_ID 					11
#define IOT_C_DATA_Z_ACCEL_ID 					12
#if (IOT_PROTOCOL_C_WITH_ANGLE == 1)
#define IOT_C_DATA_X_ANGLE_ID 					13
#define IOT_C_DATA_Y_ANGLE_ID 					14
#define IOT_C_DATA_Z_ANGLE_ID 					15
#endif

typedef struct {
	uint8_t gas_gauge_flag;
	uint8_t gas_gauge;

	iot_object_t *sensor;
	collapse_obj_t *collapse_obj;
	lora_obj_t* lora_obj;

	void (*operate)(void);
} iot_dev_t;

void iot_operate(void);
iot_dev_t * iot_init(iot_object_t *sensor, collapse_obj_t *collapse_obj, lora_obj_t* lora_obj);
void iot_write_long_addr(uint8_t* value);
void iot_write_short_addr(uint8_t* value);

#endif
