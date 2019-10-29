#ifndef __SYS_PARAM_H__
#define __SYS_PARAM_H__
#include "main.h"


/* 通信字节流传输格式 */
#define COMM_TRANSMISSION_FORMAT					1 			//0：低字节先传，1：高字节先传
#define CRC_TRANSMISSION_FORMAT						0 			//0：低字节先传，1：高字节先传

/* 设备名字格式 */
#define DEV_MAME_FORMAT								0			//0：使用测点长地址作为设备名字，1：使用自定义规则作为设备名字

/* 系统参数存储区 */
#define SYS_PARAM_FLASH_PAGE_ADDR					ADDR_FLASH_PAGE_80 	//系统参数存储地址

/* 蓝牙配置参数 */
#define SYS_PARAM_BLE_DEV_NAME_PREFIX				"TOE_MG_" 	//设备名字前缀
#define SYS_PARAM_BLE_TX_POWER						6 			//蓝牙发射功率[0:-40dBm,1:-20dBm,2:-16dBm,3:-12dBm,4:-8dBm,5:-4dBm,6:0dBm,7:+3dBm,8:+4dBm]
#define SYS_PARAM_BLE_ADV_INTERVAL					100 		//蓝牙广播间隔[20~10240ms]
#define SYS_PARAM_BLE_ADV_TIME						15 			//蓝牙广播时间[1~65535s,0:无限广播]
#define SYS_PARAM_BLE_MIN_CONN_INTERVAL				15 			//蓝牙连接最小间隔[15~4000ms]
#define SYS_PARAM_BLE_MAX_CONN_INTERVAL				200 		//蓝牙连接最大间隔[15~4000ms]
#define SYS_PARAM_BLE_SLAVE_LATENCY					0 			//蓝牙从机可以忽略的事件数[0~499]
#define SYS_PARAM_BLE_CONN_TIMEOUT					4000 		//蓝牙连接超时时间，超时时间设置应大于从机延时*连接最大间隔[100~32000ms]

/* LORA配置参数 */
#define SYS_PARAM_LORA_FREQ							433 		//LORA频率[410~800MHz]
#define SYS_PARAM_LORA_POWER						(int8_t)20 	//LORA发射功率[-9~22]
#define SYS_PARAM_LORA_BW							8 			/* LORA带宽[0:7.81KHz ,1:10.24KHz,2:15.63KHz,3:20.83KHz,
																	4:31.25KHz,5:41.67KHz,6:62.50KHz,7:125KHz,8:250KHz,9:500KHz] */
#define SYS_PARAM_LORA_SF							7 			//LORA扩频因子[5~12]
#define SYS_PARAM_LORA_CODE_RATE					1 			//LORA编码率[1:CR4_5,2:CR4_6,3:CR4_7,4:CR4_8]
#define SYS_PARAM_LORA_PREAMBLE						14 			//LORA前导码[5~255]
#define SYS_PARAM_LORA_HEADER						0 			//LORA报头,SF为6时只能使用隐式报头[0:显式报头,1:隐式报头]
#define SYS_PARAM_LORA_CRC							1 			//LORA校验[0:关,1:开]

/* 设备配置参数 */
#define SYS_PARAM_DEV_GATEWAY_ADDR					{0x64,0x01,0X20,0X19,0X09,0X16,0X00,0X01} 	//网关地址
#define SYS_PARAM_DEV_LONG_ADDR						{0XC8,0X01,0X20,0X19,0X08,0X12,0X00,0X02} 	//测点长地址
#define SYS_PARAM_DEV_SHORT_ADDR					{0x01,0x01} 								//测点短地址

/* IOT倾角配置参数 */
#define SYS_PARAM_IOT_I_MODE						0 			//数据采样模式[0:周期模式,1:相对阈值模式,2:绝对阈值模式]
#define SYS_PARAM_IOT_I_SAMPLE_INTERVAL				600 			//3600 //数据采样间隔，单位s
#define SYS_PARAM_IOT_I_X_ANGLE_THRESHOLD			(float)80.1	//阈值模式下的X轴角度阈值
#define SYS_PARAM_IOT_I_Y_ANGLE_THRESHOLD			(float)80.2	//阈值模式下的Y轴角度阈值
#define SYS_PARAM_IOT_I_Z_ANGLE_THRESHOLD			(float)80.3	//阈值模式下的Y轴角度阈值
	
/* IOT崩塌计配置参数 */
#define SYS_PARAM_IOT_C_MODE						0			//0：周期模式，1：触发模式
#define SYS_PARAM_IOT_C_SAMPLE_PERIOD				20 			//3600//周期模式下的采样频率，单位s
#define SYS_PARAM_IOT_C_TRIGGER_PERIOD				2 			//触发模式下持续触发时的数据推送频率，单位s
#define SYS_PARAM_IOT_C_ACCEL_SLOPE_THRESHOLD		100 		//触发模式下的加速度变化的斜率阈值
#define SYS_PARAM_IOT_C_CONSECUTIVE_DATA_POINTS		2 			//触发模式下连续数据点
#define IOT_PROTOCOL_C_WITH_ANGLE					1			//0：协议中不带角度数据，1：协议中带角度数据

/* LORA任务参数 */
#define SWT_LORA_TIME_SLICE_TIME					30*1000u	//LORA数据发送时间片时间
#define LORA_RX_TIMEOUT_BASE						3000u 		//LORA数据接收超时时间基数
#define LORA_TX_MAX_DELAY_TIME						5*1000u		//LORA数据发送失败最大延时时间
#define LORA_TX_MAX_FIAL_TIMES						~0u			//LORA数据发送失败最大次数
#define LORA_DELAY_BASE_TIME						200u		//LORA数据发送失败延时基数
#define LORA_RANDOME_DELAY_UPPER					100u		//LORA数据发送失败随机延时时间上限
#define LORA_RANDOME_DELAY_LOWER					10u			//LORA数据发送失败随机延时时间下限

/* 软件/硬件信号检测功能开关 */
#define SIGNAL_DET_SW								0 			//软件信号检测开关
#define SIGNAL_DET_HW								1 			//硬件信号检测开关
#define BAT_SOC_DET_SW								1			//软件电池电量检测开关
#define BAT_SOC_DET_HW								0 			//硬件电池电量检测开关 

/* 软件信号检测任务参数 */
#define SIGNAL_DET_TIME_SLICE						10*1000u 	//信号检测时间片

/* 设备任务ID设置 */
#define LORA_TASK_ID								1 			//LORA任务ID
#define BLE_TASK_ID									2 			//BLE任务ID
#define SLP_TASK_ID									3 			//系统低功耗任务ID
#define COLLAPSE_TASK_ID							4 			//崩塌计任务ID
#define SIGNAL_DET_TASK_ID							5 			//信号检测任务ID
#define BAT_SOC_TASK_ID								6 			//电池电量任务ID

typedef struct {
	uint8_t iot_mode;
	uint32_t iot_sample_interval;
	float iot_x_angle_threshold;
	float iot_y_angle_threshold;
	float iot_z_angle_threshold;
}iot_clinometer_t;

typedef struct {
	uint8_t iot_mode;
	uint32_t iot_sample_period;
	uint32_t iot_trigger_period;
	uint32_t iot_period;
	uint16_t iot_accel_slope_threshold;
	uint16_t iot_consecutive_data_points;
}iot_collapse_t;

typedef struct {
	uint8_t update_flag;
	uint8_t object_version;
	
	uint8_t ble_tx_power;
	uint16_t ble_adv_interval;
	uint16_t ble_adv_time;
	uint16_t ble_min_conn_interval;
	uint16_t ble_max_conn_interval;
	uint16_t ble_slave_latency;
	uint16_t ble_conn_timeout;
	
	uint16_t lora_freq;
	uint8_t lora_power;
	uint8_t lora_bw;
	uint8_t lora_sf;
	uint8_t lora_code_rate;
	uint8_t lora_preamble;
	uint8_t lora_header;
	uint8_t lora_crc;
	
	uint8_t dev_gateway_addr[8];
	uint8_t dev_long_addr[8];
	uint8_t dev_short_addr[2];
	
	iot_clinometer_t iot_clinometer;
	iot_collapse_t iot_collapse;
	
	uint8_t (*save_param_to_flash)(void);
} sys_param_t;

void sys_param_init(void);
uint8_t sys_save_param_to_flash(void);
sys_param_t* sys_param_get_handle(void);
void sys_param_set(uint8_t* param, uint8_t* value, uint8_t len);


#endif








