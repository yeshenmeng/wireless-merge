/*
 * iotobject.h
 * �������в��Ļ��࣬�������Ĺ��������뷽������
 * ��c�汾��
 *  Created on: 2019��2��1��
 *      Author: xuhui
 */

#ifndef IOTOBJECT_H_
#define IOTOBJECT_H_

#include "main.h"

#define MAX_PROP_COUNT	256			//֧�����256������
#define MAX_PROP_MEM_SIZE	1024	//�����������1024 bytes


#ifdef TH_STM32f103C8T6
#define LONG_ADDR_FLASH_PAGE_ADDR	0x0800F800	//page 62
#define OTHER_PROP_FLASH_PAGE_ADDR	0x0800FC00	//page 63
#elif defined TH_STM32WB55
#define LONG_ADDR_FLASH_PAGE_ADDR	ADDR_FLASH_PAGE_62	//page 62
#define OTHER_PROP_FLASH_PAGE_ADDR ADDR_FLASH_PAGE_63	//page 63
#else
#define LONG_ADDR_FLASH_PAGE_ADDR	ADDR_FLASH_PAGE_85	//page 62
#define OTHER_PROP_FLASH_PAGE_ADDR	ADDR_FLASH_PAGE_86	//page 63
#endif

//���Ժ�
enum {
	IOT_OBJ_INIT_ADDR, IOT_OBJ_LONG_ADDR, IOT_OBJ_SHORT_ADDR
};

//������
typedef struct iot_object {
	//Public functions
	uint8_t (*isPropChanged)(uint8_t id);
	void (*resetPropChangeFlag)(uint8_t id);

	//�������Ը���
	void (*setPropCount)(uint8_t len);

	//�������Գ���
	void (*setPropLen)(uint8_t id, uint8_t len);

	uint8_t (*getPropLen)(uint8_t id);

	//��ʼ��������ʼ��ַ�����籣�����Ե�
	void (*init)();

	//��ȡbuf�е����ݣ�д�뱾����ĳ��������(�����ж��Ƿ����������Ա��޸��ˣ���־һ����Ҫд��flash)
	void (*writePropFromBuf)(uint8_t id, uint8_t *buf);

	//��ĳ�����Ե����ݶ����ⲿbuf��
	void (*readPropToBuf)(uint8_t id, uint8_t *buf);

	//����ַ�Ƿ�һ�£����һ��Ϊ1������Ϊ0
	uint8_t (*isLongAddrEq)(uint8_t *addr);

	//�̵�ַ�Ƿ�һ�£����һ��Ϊ1������Ϊ0
	uint8_t (*isShortAddrEq)(uint8_t *addr);

	//������ַ�洢��flash��
	uint8_t (*saveLongAddr2Flash)();

	//���������Դ洢��flash��
	uint8_t (*saveProp2Flash)();

	//Private properties
	uint8_t _initAddr[2];				//initial address
	uint8_t _shortAddr[2];			//short address
	uint8_t _longAddr[8];			//long address
	uint8_t _propCount;				//total number of properties

	uint8_t _propBuffer[MAX_PROP_MEM_SIZE];	//property buffer (max: 256 properties)
	uint8_t _propLen[MAX_PROP_COUNT];			//���ڴ��ĳ��property���ֽڳ���
	uint8_t _propStartId[MAX_PROP_COUNT];//���ڴ��ĳ��property��property buffer�е���ʵλ��

	//���ڴ洢��Ҫ���籣�������Ժ�(����ַ���⣬Ĭ�ϳ���ַ����д�����ģ������ڶ�����flashҳ�ϣ����⾭������)
	uint8_t _permanentProps[MAX_PROP_COUNT];
	uint8_t _permanentPropCount;	//the total amount of permanent properties
	uint8_t _permanentPropModifyFlag;//indicate some permanent properties have been modified
	uint8_t _propModifyFlag[MAX_PROP_COUNT];

} iot_object_t;

//����һ�����ܲ��
iot_object_t * createSensorHandler(void);

#endif /* IOTOBJECT_H_ */
