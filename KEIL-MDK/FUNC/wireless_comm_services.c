/*
 * modbus_slave.c
 * 参考张新星上一个版本，实现同禾485通讯协议
 *  Created on: 2019年2月1日
 *      Author: xuhui
 */
#include "wireless_comm_services.h"
#include "sys_param.h"

static const uint8_t aucCRCHi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
};

static const uint8_t aucCRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
};

static wireless_comm_services_t self;
static uint8_t gateway_addr[8] = {0};
static uint8_t* local_gateway_addr = NULL;

static void _setSensorHandler(iot_object_t *obj)
{
	self._sensor = obj;
}

static uint8_t _isCmdValid()
{
	uint32_t cmd = self._rx_buf[0]<<24| self._rx_buf[1] << 16 | self._rx_buf[2] << 8 |self._rx_buf[3];
	return (cmd == CMD_CONNECT_RESP) || 
		   (cmd == CMD_PUBLISH_RESP) ||
		   (cmd == CMD_TEST_RESP);
}

/*******************************************************************************
* Function Name  : ModbusRTU_CRC
* Description    : crc check
* Input          : *pucFrame,usLen
* Output         : uint16_t
* Return         : None
* Attention		 : 高位在后地位在前 PLC专用
*******************************************************************************/
static uint16_t _modbusRtuCRC(uint8_t *pucFrame, uint16_t usLen)
{
    uint8_t       ucCRCHi = 0xFF;
    uint8_t       ucCRCLo = 0xFF;
    uint16_t      iIndex = 0;

    while( usLen-- )
    {
        iIndex = ucCRCLo ^ *( pucFrame++ );
        ucCRCLo = (uint8_t)( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
//		return (uint16_t)( ucCRCLo<< 8 | ucCRCHi  );//???????? PLC??
	
	return (uint16_t)(ucCRCLo | ucCRCHi<<8);
}

static uint8_t _isCRCValid()
{
	if(self._rx_buf[DATA_LEN_BYTE_ID] + 7 > MAX_WIRELESS_COMM_BUF_SIZE)
		return 0;

	uint16_t crc = _modbusRtuCRC(self._rx_buf, self._rx_buf[DATA_LEN_BYTE_ID] + 5);
	uint8_t crc_hi = crc >> 8;
	uint8_t crc_low = crc & 0xFF;
	
#if (CRC_TRANSMISSION_FORMAT == 0)
	crc_low = crc_hi;
	crc_hi = crc & 0xFF;
#endif
	
	return self._rx_buf[self._rx_count - 2] == crc_hi
			&& self._rx_buf[self._rx_count - 1] == crc_low;
}

static void _clearFlags()
{
	//清除接收buffer
	self._rx_count = 0;

	//清除发送buffer
	self._tx_count = 0;

	self._frame_finish_flag = 0;
}

static void _setFrameFinishFlag()
{
	self._frame_finish_flag = 1;
}

#if (0)
static void _writeLongAddr()
{
	self._sensor->writePropFromBuf(IOT_OBJ_LONG_ADDR, &self._rx_buf[DATA_LEN_BYTE_ID + 1]);
	self._sensor->saveLongAddr2Flash();
}
#endif

static uint8_t _writeShortAddr()
{
	uint8_t flag = 1;

	//Check whether the long address is correct
	if(self._sensor->isLongAddrEq(&self._rx_buf[DATA_LEN_BYTE_ID + 1]))
	{
		self._sensor->writePropFromBuf(IOT_OBJ_SHORT_ADDR,
				&self._rx_buf[DATA_LEN_BYTE_ID + 17]);
	}
	else
	{
		//long address is not valid
		flag = 0;
	}
	return flag;
}

static uint8_t _writeMultiProps()
{
	/* 回复的数据中没有携带写属性数据 */
	if(self._rx_buf[DATA_LEN_BYTE_ID] == 2)
	{
		return 1;
	}
	
	//检查属性个数是否小于最大属性个数
	uint8_t nProps = self._rx_buf[DATA_LEN_BYTE_ID + 3];
	if((uint16_t)nProps > MAX_PROP_COUNT)
		return 0;

	//检查属性号是否合理, cannot be greater than max property count
	uint8_t i;
	uint8_t totalLens = nProps + 3;

	for(i = 0; i < nProps; i++)
	{
//		if(self._rx_buf[DATA_LEN_BYTE_ID + 2 + i] >= MAX_PROP_COUNT)
//			return 0;

		totalLens += self._sensor->getPropLen(self._rx_buf[DATA_LEN_BYTE_ID + 4 + i]);
	}

	//检查数据长度是否正确
	if(self._rx_buf[DATA_LEN_BYTE_ID] != totalLens)
		return 0;

	//解析数据，写入iot_object
	uint8_t propId;
	uint8_t pStart = DATA_LEN_BYTE_ID + 4 + nProps;

	for (i = 0; i < nProps; i++)
	{
		propId = self._rx_buf[DATA_LEN_BYTE_ID + 4 + i];

		self._sensor->writePropFromBuf(propId,
				&self._rx_buf[pStart]);

		pStart += self._sensor->getPropLen(propId);
	}
	return 1;	//success
}

#if (0)
static uint8_t _readMultiProps()
{
	//检查属性个数是否小于最大属性个数
	uint8_t nProps = self._rx_buf[DATA_LEN_BYTE_ID];
//	if(nProps > MAX_PROP_COUNT)
//		return 0;

	//检查属性号是否合理, cannot be greater than max property count
	uint8_t i;
	uint8_t totalLens = nProps + 1;

	for(i = 0; i < nProps; i++)
	{
//		if(self._rx_buf[DATA_LEN_BYTE_ID + 1 + i] >= MAX_PROP_COUNT)
//			return 0;

		totalLens += self._sensor->getPropLen(self._rx_buf[DATA_LEN_BYTE_ID + 1 + i]);
	}

	//返回的数据长
	self._tx_buf[DATA_LEN_BYTE_ID] = totalLens;

	//总属性个数
	self._tx_buf[DATA_LEN_BYTE_ID + 1] = nProps;

	//读取iot_object的数据到发送传输数组中
	uint8_t propId;
	uint8_t pStart = DATA_LEN_BYTE_ID + 2 + nProps;

	for (i = 0; i < nProps; i++)
	{
		propId = self._rx_buf[DATA_LEN_BYTE_ID + 1 + i];

		//Property Mask
		self._tx_buf[DATA_LEN_BYTE_ID + 2 + i] = propId;

		//Property
		self._sensor->readPropToBuf(propId,
				&self._tx_buf[pStart]
				);

		pStart += self._sensor->getPropLen(propId);
	}

	self._tx_count = (totalLens + 4);

	return 1;	//success
}
#endif

static void _setLocalGatewayAddr(uint8_t* addr)
{
	local_gateway_addr = addr;
}

static uint8_t isGatewayAddrEq(void)
{
	uint8_t flag = 0;
	if(local_gateway_addr != NULL)
	{
		flag = 1;
		for(int i=0; i<8; i++)
		{
			if(gateway_addr[i] != local_gateway_addr[i])
			{
				flag = 0;
				break;
			}
		}
	}
	
	return flag;
}

static uint8_t _parseMasterMsg(void)
{
	if(self._frame_finish_flag)
	{
		/* 检查数据长度是否正确 */
		if(self._rx_buf[DATA_LEN_BYTE_ID] + 7 != self._rx_count)
		{
			_clearFlags();
			return 0;
		}
		
		//check cmd byte
		if(_isCmdValid() == 0)
		{
			_clearFlags();
			return 0;
		}
		
		//check CRC
		if(_isCRCValid() == 0)
		{
			_clearFlags();
			return 0;
		}		
		
		uint32_t cmd = self._rx_buf[0]<<24| self._rx_buf[1] << 16 | self._rx_buf[2] << 8 | self._rx_buf[3];
		
		if(cmd == CMD_CONNECT_RESP)
		{
			if(self._sensor->isLongAddrEq(&self._rx_buf[DATA_LEN_BYTE_ID+1]) == 0)
			{
				_clearFlags();
				return 0;
			}
			
			for(int i=0; i<8; i++)
			{
				gateway_addr[i] = self._rx_buf[DATA_LEN_BYTE_ID+i+9];
			}
			
			if(isGatewayAddrEq() == 0)
			{
				_clearFlags();
				return 0;
			}
			
			if(_writeShortAddr() == 0)
			{
				_clearFlags();
				return 0;
			}
			self._conn_short_addr_update_flag = 1;
		}
		else if(cmd == CMD_PUBLISH_RESP)
		{
			if(self._sensor->isShortAddrEq(&self._rx_buf[DATA_LEN_BYTE_ID+1]) == 0)
			{
				_clearFlags();
				return 0;
			}
			
			if (_writeMultiProps() == 0)
			{
				_clearFlags();
				return 0;
			}
		}
		else if(cmd == CMD_TEST_RESP)
		{
			if(self._sensor->isLongAddrEq(&self._rx_buf[DATA_LEN_BYTE_ID+1]) == 0)
			{
				_clearFlags();
				return 0;
			}
			
			for(int i=0; i<8; i++)
			{
				gateway_addr[i] = self._rx_buf[DATA_LEN_BYTE_ID+i+9];
			}
			
			if(isGatewayAddrEq() == 0)
			{
				_clearFlags();
				return 0;
			}
			
			uint8_t payload_length = self._rx_buf[DATA_LEN_BYTE_ID] - 16;
			for(int i=0; i<payload_length; i++)
			{
				if(self._rx_buf[DATA_LEN_BYTE_ID+i+17] != 0XA5)
				{
					return 0;
				}
			}
		}

		//clear the flags and buffers
		_clearFlags();

		return 1;	//success
	}
	else
		return 0;	//no msg received
}

static void _wirelessRxCpltCallBack(uint8_t* pData, uint16_t size)
{
	/* 数据放入接收缓存区 */
	if(self._rx_count == 0 && size < MAX_WIRELESS_COMM_BUF_SIZE-1)
	{
		self._rx_count = size;
		for(int i=0; i<size; i++)
		{
			self._rx_buf[i] = pData[i];
		}
	}
	
	self._frame_finish_flag = 1;
}

wireless_comm_services_t * createWirelessCommServiceHandler(void)
{
	self._rx_count = 0;
	self._tx_count = 0;
	self._frame_finish_flag = 0;
	self._conn_short_addr_update_flag = 0;

	self.parseMasterMsg = _parseMasterMsg;
	self.wirelessRxCpltCallBack = _wirelessRxCpltCallBack;
	self.setFrameFinishFlag = _setFrameFinishFlag;
	self.setSensorHandler = _setSensorHandler;
	self.isGatewayAddrEq = isGatewayAddrEq;
	self.modbusRtuCRC = _modbusRtuCRC;

	sys_param_t* param = sys_param_get_handle();
	_setLocalGatewayAddr(param->dev_gateway_addr);		
	
	return &self;
}

wireless_comm_services_t* Wireless_CommSvcGetHandle(void)
{
	return &self;
}


