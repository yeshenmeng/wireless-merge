/*
 * modbus_slave.h
 * 实现基于485总线的类modbus私有通讯协议
 *
 *  Created on: 2019年2月1日
 *      Author: xuhui
 */

#ifndef __WIRELESS_COMM_SERVICES_H__
#define __WIRELESS_COMM_SERVICES_H__
#include "main.h"
#include "iotobject.h"


#define CMD_CONNECT						0x00000001
#define CMD_CONNECT_RESP				0x00000002
#define CMD_PUBLISH						0x00000003
#define CMD_PUBLISH_RESP				0x00000004

#define DATA_LEN_BYTE_ID 				4

#define MAX_WIRELESS_COMM_BUF_SIZE		255


typedef struct wireless_comm_services {
	//解析主机发过来的命令，返回1如果解析成功，否则返回0
	uint8_t (*parseMasterMsg)(void);

	//处理接收到的数据，清理接收帧标志，并计数
	void (*wirelessRxCpltCallBack)(uint8_t* pData, uint16_t size);

	//将Frame Finish Flag至1，表示一帧结束，应当在外部定时器中断中调用
	void (*setFrameFinishFlag)(void);

	//设置待处理的sensor handler
	void (*setSensorHandler)(iot_object_t *obj);
	
	uint8_t (*isGatewayAddrEq)(void);
	
	uint16_t (*modbusRtuCRC)(uint8_t *pucFrame, uint16_t usLen);

	uint8_t _rx_buf[MAX_WIRELESS_COMM_BUF_SIZE];			//receive buffer
	uint8_t _tx_buf[MAX_WIRELESS_COMM_BUF_SIZE];			//transmit buffer
	uint8_t _frame_finish_flag;		//标志一帧接收结束
	uint8_t _conn_short_addr_update_flag;//联机时候的短地址更新标志
	uint8_t _rx_count;			//当前接收帧的字节个数
	uint8_t _tx_count;			//当前发送帧的字节个数

	iot_object_t * _sensor;		//待处理的sensor实体

} wireless_comm_services_t;

wireless_comm_services_t * createWirelessCommServiceHandler(void);
wireless_comm_services_t* Wireless_CommSvcGetHandle(void);

#endif


