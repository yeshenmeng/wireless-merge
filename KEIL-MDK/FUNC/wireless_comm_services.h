/*
 * modbus_slave.h
 * ʵ�ֻ���485���ߵ���modbus˽��ͨѶЭ��
 *
 *  Created on: 2019��2��1��
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
	//�����������������������1��������ɹ������򷵻�0
	uint8_t (*parseMasterMsg)(void);

	//������յ������ݣ��������֡��־��������
	void (*wirelessRxCpltCallBack)(uint8_t* pData, uint16_t size);

	//��Frame Finish Flag��1����ʾһ֡������Ӧ�����ⲿ��ʱ���ж��е���
	void (*setFrameFinishFlag)(void);

	//���ô������sensor handler
	void (*setSensorHandler)(iot_object_t *obj);
	
	uint8_t (*isGatewayAddrEq)(void);
	
	uint16_t (*modbusRtuCRC)(uint8_t *pucFrame, uint16_t usLen);

	uint8_t _rx_buf[MAX_WIRELESS_COMM_BUF_SIZE];			//receive buffer
	uint8_t _tx_buf[MAX_WIRELESS_COMM_BUF_SIZE];			//transmit buffer
	uint8_t _frame_finish_flag;		//��־һ֡���ս���
	uint8_t _conn_short_addr_update_flag;//����ʱ��Ķ̵�ַ���±�־
	uint8_t _rx_count;			//��ǰ����֡���ֽڸ���
	uint8_t _tx_count;			//��ǰ����֡���ֽڸ���

	iot_object_t * _sensor;		//�������sensorʵ��

} wireless_comm_services_t;

wireless_comm_services_t * createWirelessCommServiceHandler(void);
wireless_comm_services_t* Wireless_CommSvcGetHandle(void);

#endif


