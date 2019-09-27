/*
 * iotobject.h
 * 描述所有测点的基类，包含测点的公共属性与方法定义
 * （c版本）
 *  Created on: 2019年2月1日
 *      Author: xuhui
 */

#ifndef IOTOBJECT_H_
#define IOTOBJECT_H_

#include "main.h"

#define MAX_PROP_COUNT	256			//支持最多256个属性
#define MAX_PROP_MEM_SIZE	1024	//所有属性最大1024 bytes


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

//属性号
enum {
	IOT_OBJ_INIT_ADDR, IOT_OBJ_LONG_ADDR, IOT_OBJ_SHORT_ADDR
};

//测点基类
typedef struct iot_object {
	//Public functions
	uint8_t (*isPropChanged)(uint8_t id);
	void (*resetPropChangeFlag)(uint8_t id);

	//设置属性个数
	void (*setPropCount)(uint8_t len);

	//设置属性长度
	void (*setPropLen)(uint8_t id, uint8_t len);

	uint8_t (*getPropLen)(uint8_t id);

	//初始化属性起始地址、掉电保护属性等
	void (*init)();

	//读取buf中的数据，写入本测点的某个属性中(并且判断是否有永久属性被修改了，标志一下需要写入flash)
	void (*writePropFromBuf)(uint8_t id, uint8_t *buf);

	//将某个属性的数据读到外部buf中
	void (*readPropToBuf)(uint8_t id, uint8_t *buf);

	//长地址是否一致，如果一致为1，否则为0
	uint8_t (*isLongAddrEq)(uint8_t *addr);

	//短地址是否一致，如果一致为1，否则为0
	uint8_t (*isShortAddrEq)(uint8_t *addr);

	//将长地址存储到flash中
	uint8_t (*saveLongAddr2Flash)();

	//将永久属性存储到flash中
	uint8_t (*saveProp2Flash)();

	//Private properties
	uint8_t _initAddr[2];				//initial address
	uint8_t _shortAddr[2];			//short address
	uint8_t _longAddr[8];			//long address
	uint8_t _propCount;				//total number of properties

	uint8_t _propBuffer[MAX_PROP_MEM_SIZE];	//property buffer (max: 256 properties)
	uint8_t _propLen[MAX_PROP_COUNT];			//用于存放某个property的字节长度
	uint8_t _propStartId[MAX_PROP_COUNT];//用于存放某个property在property buffer中的其实位置

	//用于存储需要掉电保护的属性号(长地址除外，默认长地址就是写保护的，并且在独立的flash页上，避免经常擦除)
	uint8_t _permanentProps[MAX_PROP_COUNT];
	uint8_t _permanentPropCount;	//the total amount of permanent properties
	uint8_t _permanentPropModifyFlag;//indicate some permanent properties have been modified
	uint8_t _propModifyFlag[MAX_PROP_COUNT];

} iot_object_t;

//创建一个智能测点
iot_object_t * createSensorHandler(void);

#endif /* IOTOBJECT_H_ */
