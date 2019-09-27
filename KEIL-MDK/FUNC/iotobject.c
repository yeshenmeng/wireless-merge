/*
 * iotobject.c
 *
 *  Created on: 2019年2月1日
 *      Author: xuhui
 */

#include "iotobject.h"
#include "flash.h"

static iot_object_t self;

static uint8_t _isPropChanged(uint8_t id) {
	return self._propModifyFlag[id];
}

static void _resetPropChangeFlag(uint8_t id) {
	self._propModifyFlag[id] = 0;
}

static void _setPropCount(uint8_t len) {
	self._propCount = len;
}

//如果是永久属性返回1，否则返回0
static uint8_t _isPermanentProp(uint8_t id) {
	uint8_t i;

	for (i = 0; i < self._permanentPropCount; i++) {
		if (id == self._permanentProps[i])
			return 1;
	}
	return 0;
}

static void _setPropLen(uint8_t id, uint8_t len) {
	if (id < self._propCount)
		self._propLen[id] = len;
}

static uint8_t _getPropLen(uint8_t id) {
	if (id < self._propCount)
		return self._propLen[id];
	else
		return 0;
}

//初始化属性个数与起始地址
static void _initPropStartIds() {
	uint8_t i = 1;
	self._propStartId[0] = 0;

	for (; i < self._propCount; i++) {
		self._propStartId[i] = self._propStartId[i - 1] + self._propLen[i - 1];
	}
}

//读取buf中的数据，写入本测点的某个属性中
static void _writePropFromBuf(uint8_t id, uint8_t *buf) {
	if (id >= self._propCount)
		return;

	uint8_t i = 0;
	uint8_t start = self._propStartId[id];

	for (; i < self._propLen[id]; i++)
		self._propBuffer[start + i] = buf[i];

	//检查一下是不是长地址或短地址, 是的话在快捷数组里面也放一份
	if (id == IOT_OBJ_LONG_ADDR) {
		for (i = 0; i < 8; i++)
			self._longAddr[i] = buf[i];
	} else if (id == IOT_OBJ_SHORT_ADDR) {
		for (i = 0; i < 2; i++)
			self._shortAddr[i] = buf[i];
	}

	//检查一下是不是改了永久属性
	if (_isPermanentProp(id))
		self._permanentPropModifyFlag = 1;

	self._propModifyFlag[id] = 1;
}

//将某个属性的数据读到外部buf中
static void _readPropToBuf(uint8_t id, uint8_t *buf) {
	if (id >= self._propCount)
		return;

	uint8_t i = 0;
	uint8_t start = self._propStartId[id];

	for (; i < self._propLen[id]; i++)
		buf[i] = self._propBuffer[start + i];
}

//长地址是否一致，如果一致为1，否则为0
static uint8_t _isLongAddrEq(uint8_t *addr) {
	uint8_t flag = 1;
	uint8_t i = 0;
	for (; i < 8; i++) {
		if (addr[i] != self._longAddr[i]) {
			flag = 0;
			break;
		}
	}

	return flag;
}

//短地址是否一致，如果一致为1，否则为0
static uint8_t _isShortAddrEq(uint8_t *addr) {
	return ((self._shortAddr[0] == addr[0]) && (self._shortAddr[1] == addr[1]))
			|| ((addr[0] == 0xff) && (addr[1] == 0xff));	//广播地址
}

static uint8_t _saveLongAddr2Flash() {
	uint32_t longAddr[2];

	longAddr[0] = self._longAddr[0] | self._longAddr[1] << 8
			| self._longAddr[2] << 16 | self._longAddr[3] << 24;

	longAddr[1] = self._longAddr[4] | self._longAddr[5] << 8
			| self._longAddr[6] << 16 | self._longAddr[7] << 24;

	return flash_write(LONG_ADDR_FLASH_PAGE_ADDR, longAddr, 2);
}

static uint8_t _saveProp2Flash() {
	if (self._permanentPropModifyFlag == 0 || self._permanentPropCount == 0)
		return 0;

	//清楚标志
	self._permanentPropModifyFlag = 0;

	uint8_t i = 0;

	uint32_t size = 0;
	//计算所需的空间(bytes)
	for (i = 0; i < self._permanentPropCount; i++) {
		size += self._propLen[self._permanentProps[i]];
	}

	uint32_t size32 = 0;
	if (size % 4 == 0)
		size32 = (size >> 2);
	else
		size32 = (size >> 2) + 1;

	uint32_t buf[size32];
	for (i = 0; i < size32; i++)
		buf[i] = 0;

	//memset(buf, 0, size32);

	uint32_t id = 0;
	uint32_t start = 0;
	uint8_t len = 0;
	uint8_t j;

	uint8_t count4 = 0;

	for (i = 0; i < self._permanentPropCount; i++) {
		start = self._propStartId[self._permanentProps[i]];
		len = self._propLen[self._permanentProps[i]];
		for (j = 0; j < len; j++) {
			if (count4 == 4) {
				id++;
				count4 = 0;
			}

			buf[id] = buf[id] | (self._propBuffer[j + start] << (8 * count4));

			count4++;
		}
	}

	//转换为32位的数组地址与个数
	return flash_write(OTHER_PROP_FLASH_PAGE_ADDR, buf, size32);
}

//从flash中读取掉电保护数据
void _loadPropFromFlash() {
	//load long address
	flash_read(LONG_ADDR_FLASH_PAGE_ADDR, self._longAddr, 8);

	uint8_t i;
	uint32_t start = 0;
	start = self._propStartId[IOT_OBJ_LONG_ADDR];
	for (i = 0; i < 8; i++)
		self._propBuffer[start + i] = self._longAddr[i];

	//Other properties
	uint8_t j;
	uint8_t len = 0;

	for (i = 0; i < self._permanentPropCount; i++) {
		start = self._propStartId[self._permanentProps[i]];
		len = self._propLen[self._permanentProps[i]];

		flash_read(OTHER_PROP_FLASH_PAGE_ADDR, &self._propBuffer[start], len);

		//short addr
		if (self._permanentProps[i] == IOT_OBJ_SHORT_ADDR) {
			for (j = 0; j < len; j++)
				self._shortAddr[j] = self._propBuffer[start + j];
		}
	}
}

static void _init() {
	_initPropStartIds();
	_loadPropFromFlash();
}

iot_object_t * createSensorHandler(void) {
	//assign function pointers
	self.isPropChanged = _isPropChanged;
	self.resetPropChangeFlag = _resetPropChangeFlag;

	self.setPropCount = _setPropCount;
	self.setPropLen = _setPropLen;
	self.getPropLen = _getPropLen;

	self.init = _init;
	self.writePropFromBuf = _writePropFromBuf;
	self.readPropToBuf = _readPropToBuf;
	self.isShortAddrEq = _isShortAddrEq;
	self.isLongAddrEq = _isLongAddrEq;

	self.saveLongAddr2Flash = _saveLongAddr2Flash;
	self.saveProp2Flash = _saveProp2Flash;

	//private properties
	self._propCount = 6;	//at least 3 properties
	self._permanentPropCount = 1;	//at least 1 permanent prop (short addr)
	self._permanentProps[0] = IOT_OBJ_SHORT_ADDR;

	self._permanentPropModifyFlag = 0;

	self._propLen[IOT_OBJ_INIT_ADDR] = 2;
	self._propLen[IOT_OBJ_LONG_ADDR] = 8;
	self._propLen[IOT_OBJ_SHORT_ADDR] = 2;

	return &self;
}
