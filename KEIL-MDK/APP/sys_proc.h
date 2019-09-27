#ifndef __SYS_PROC_H__
#define __SYS_PROC_H__
#include "main.h"
#include "bluetooth_low_power.h"
#include "lora_transmission.h"
#include "system_low_power.h"
#include "collapse.h"
#include "sw_signal_detect.h"


#define SYS_TASK_EVT_NULL						0X00u
#define SYS_TASK_EVT_INIT						0X01u
#define	SYS_TASK_EVT_BLE						0X02u
#define	SYS_TASK_EVT_COLLAPSE					0X04u
#define	SYS_TASK_EVT_LORA						0X08u
#define	SYS_TASK_EVT_SYS_LP						0X10u
#define	SYS_TASK_EVT_SIGNAL_DET					0X20u

typedef struct {
	ble_obj_t* ble_task;
	lora_obj_t* lora_task;
	slp_obj_t* slp_task;
	collapse_obj_t* collapse_task;
	sw_signal_detect_obj_t* signal_detect_task;
}sys_task_t;

void sys_task_init(sys_task_t* task);
void sys_task_schd(void);
void sys_startup(void);

#endif





