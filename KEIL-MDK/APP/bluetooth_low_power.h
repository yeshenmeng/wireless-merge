#ifndef __BLUETOOTH_LOW_POWER_H__
#define __BLUETOOTH_LOW_POWER_H__
#include "main.h"
#include "low_power_manage.h"
#include "ble_init.h"


#define	BLE_MTU_SIZE		255 /* MTU大小 */

typedef enum {
	BLE_STA_IDLE,
	BLE_STA_ADV,
	BLE_STA_CONN,
	BLE_STA_DISCON,
	BLE_STA_ADV_TIMEOUT,
	BLE_STA_ADV_STOP,
	BLE_STA_CONN_ABORT,
	BLE_STA_KEEP_ALIVE,
	BLE_STA_ACT_IDLE,
}ble_state_t;

/* 广播数据结构 */
typedef struct {
	uint8_t length;
	uint8_t type;
	uint8_t value[];
}ad_struct_t;

typedef struct {
//	/* 配置参数 */
//	uint16_t MTU;
//	uint8_t SetAddress[6];
	
	/* 被动参数 */
	int8_t rssi;
	uint8_t address[6];
	
	/* 控制参数 */
	FunctionalState is_idle_enter_lp; /* DISABLE：蓝牙空闲状态不动作，ENABLE：蓝牙空闲状态进入低功耗模式 */
}ble_param_t;

typedef struct {
	ble_state_t state;
	ble_param_t param;
	
	lpm_obj_t* lpm_obj;
	
	void (*task_start)(void);
	void (*task_stop)(void);
	void (*task_operate)(void);
	ErrorStatus (*abort_conn)(void);
	ErrorStatus (*set_tx_power)(uint8_t pa_level);
	ErrorStatus (*read_rssi)(int8_t* rssi);
}ble_obj_t;

ble_obj_t* ble_init(lpm_obj_t* lpm_obj);

#endif

