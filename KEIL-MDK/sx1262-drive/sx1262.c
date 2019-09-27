/*
 * sx1262.c
 *
 *  Created on: 2019年6月26日
 *      Author: xxk
 */

#include "sx1262.h"
#include "sys_param.h"


sx1262_drive_t self;

#define __LORA_RSSI_OFFSET_LF                   -164
#define __LORA_RSSI_OFFSET_HF                   -157


#define SM_STATE_SET(p_this, state)   ((p_this).run_state.sm_state = state)
#define SM_STATE_GET(p_this)          ((p_this).run_state.sm_state)

#define CAD_STATE_SET(p_this, state)  ((p_this).run_state.cad_state |= state)
#define CAD_STATE_GET(p_this)         ((p_this).run_state.cad_state)
#define CAD_STATE_CLR(p_this)         ((p_this).run_state.cad_state = 0)

//设备复位
void _Reset()
{
	rst_pin_set(0);
	delay_ms(20);		//more thena 100us, delay 10ms
	rst_pin_set(1);
	delay_ms(10);		//delay 10ms
}

//设备休眠
void _SetSleep(void)
{
	uint8_t sleepConfig;

	check_busy();

	sleepConfig = 0x04;	//bit2: 1:warm start; bit0:0: RTC timeout disable

	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_SLEEP);
	spi_rw(sleepConfig);
	sel_pin_set (1);
}

//0:STDBY_RC; 1:STDBY_XOSC  设置为待机模式，等待配置设备参数
void _SetStandby(uint8_t StdbyConfig)
{
	check_busy();
	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_STANDBY);
	spi_rw(StdbyConfig);
	sel_pin_set(1);
}

//设置发送模式，和等待时间
void _SetTx(uint32_t timeout)
{
	uint8_t time_out[3];

	check_busy();
	time_out[0] = (timeout>>16)&0xFF;//MSB
	time_out[1] = (timeout>>8)&0xFF;
	time_out[2] = timeout&0xFF;//LSB

	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_TX);
	spi_rw(time_out[0]);
	spi_rw(time_out[1]);
	spi_rw(time_out[2]);
	sel_pin_set (1);
}

//设置发送超时时间
void _SetTxTime(uint32_t time)
{
	self.radio_param.tx_pkt_timeout = time;
}

//设置设备为接收模式   和等待时间
void _SetRx(bool rx_mode,uint32_t timeout)
{
	uint8_t time_out[3];

	check_busy();
	if(rx_mode == 0)
	{
		timeout = 0xffffffff;
	}

	time_out[0] = (timeout>>16)&0xFF;//MSB
	time_out[1] = (timeout>>8)&0xFF;
	time_out[2] = timeout&0xFF;//LSB

	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_RX);
	spi_rw(time_out[0]);
	spi_rw(time_out[1]);
	spi_rw(time_out[2]);
	sel_pin_set (1);
}

//设置接收超时时间
void _SetRxTime(bool rx_mode,uint32_t time)
{
	if(rx_mode == 0)
		time = 0xffffffff;

	self.radio_param.rx_mode = rx_mode;
	self.radio_param.rx_pkt_timeout = time;
}


//清除中断标志
void _ClearIrqStatus(uint16_t irq)
{
	uint16_t irq_h,irq_l;

	check_busy();

	irq_h = irq>>8;
	irq_l = irq & 0xFF;

	sel_pin_set(0);
	spi_rw(SX126X_CMD_CLEAR_IRQ_STATUS);
	spi_rw(irq_h);
	spi_rw(irq_l);
	sel_pin_set(1);
}

//外部获取设备的状态和中断状态
void _GetChipStatus(uint8_t status,uint16_t IrqStatus)
{
	check_busy();
  uint8_t temp;
	
	sel_pin_set(0);
	spi_rw(SX126X_CMD_GET_IRQ_STATUS);
	
//设备状态
	status  = spi_rw(0xFF);
	status = status;
//设备中断
	temp = spi_rw(0xFF);
	IrqStatus = temp;
	IrqStatus = IrqStatus<<8;
	temp = spi_rw(0xFF);
	IrqStatus = IrqStatus|temp;
	sel_pin_set (1);
}

//获取中断状态
uint16_t _GetIrqStatus(void)
{
	uint16_t IrqStatus;
	uint8_t temp;

	check_busy();

	sel_pin_set(0);
	spi_rw(SX126X_CMD_GET_IRQ_STATUS);
	spi_rw(0xFF);
	temp = spi_rw(0xFF);
	IrqStatus = temp;
	IrqStatus = IrqStatus<<8;
	temp = spi_rw(0xFF);
	IrqStatus = IrqStatus|temp;
	sel_pin_set (1);

	return IrqStatus;
}

//设置cad参数
void _SetCadParams(uint8_t cadSymbolNum,uint8_t cadExitMode,uint32_t timeout)
{
	self.radio_cad.cadSymbolNum = cadSymbolNum;
	self.radio_cad.cadExitMode = cadExitMode;
	self.radio_cad.cadTimeout = timeout;
}

//使能CAD参数
void _SetCad(uint8_t cadSymbolNum,uint8_t cadExitMode,uint32_t timeout)
{
	check_busy();

	_SetStandby(0);
	uint8_t  cadDetPeak;
	uint8_t time_out[3];
	cadDetPeak = self.radio_param.sf + 13;
	
	time_out[0] = (timeout>>16)&0xFF;//MSB
	time_out[1] = (timeout>>8)&0xFF;
	time_out[2] = timeout&0xFF;//LSB

	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_CAD_PARAMS);
	spi_rw(cadSymbolNum);
	spi_rw(cadDetPeak);
	spi_rw(10);
	spi_rw(cadExitMode);
	spi_rw(time_out[0]);
	spi_rw(time_out[1]);
	spi_rw(time_out[2]);
	sel_pin_set (1);
}

//设置为CAD模式
void _SetCADMode()
{
	check_busy();
	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_CAD);
	sel_pin_set (1);
}

//打开中断和dio1中断映射
void  _SetDioIrqParams(uint16_t Mask,uint16_t DIO1)
{
	check_busy();

	uint16_t Irq_Mask;
	uint8_t Irq_Mask_h,Irq_Mask_l;
	uint16_t DIO1Mask;
	uint8_t DIO1Mask_h,DIO1Mask_l;

	Irq_Mask = Mask;
	DIO1Mask = DIO1;
	

	Irq_Mask_h = Irq_Mask>>8;
	Irq_Mask_l = Irq_Mask&0xFF;
	DIO1Mask_h = DIO1Mask>>8;
	DIO1Mask_l = DIO1Mask&0xFF;

	sel_pin_set(0);

	spi_rw(SX126X_CMD_SET_DIO_IRQ_PARAMS);

	spi_rw(Irq_Mask_h);//Irq_Mask MSB
	spi_rw(Irq_Mask_l);//Irq_Mask LSB  设置中断开启位

	spi_rw(DIO1Mask_h);
	spi_rw(DIO1Mask_l);

	spi_rw(0);
	spi_rw(0);

	spi_rw(0);
	spi_rw(0);

	sel_pin_set (1);
}

//设置缓存区地址（发送缓存区地址、接收缓存区地址）
void _SetBufferBaseAddress(uint8_t TX_base_addr,uint8_t RX_base_addr)
{
	check_busy();
	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_BUFFER_BASE_ADDRESS);
	spi_rw(TX_base_addr);
	spi_rw(RX_base_addr);
	sel_pin_set (1);
}

/*
 *  CAD 相关说明：
 *  CAD模式：信道活动检测模式,旨在以尽可能高的功耗效率检测无线信道上的LoRa前导码, 使用CAD模式可降低功耗。
 *
 *         CAD模式      IDLE模式      CAD模式
 *         _____                      ______
 *  _______| t1 |________t2___________| t1 |_______
 *开启CAD->|____|<--- 主函数延时------>|__________
 *
 * CAD 检测时间t 可通过升特公司提供的计算器算出，如在 带宽250k，扩频因子：7，误码率：1 时， CAD 检测时间为1.2ms
 * 对于单次接收模式， 退出CAD模式之后进入待机模式。
 * 为了能够接收发送者每次发送的数据，发送端的前导码的时间大于 t1 + t2。
 *
 * CAD_ONLY 0x00  芯片在进行CAD操作时。一旦完成，无论通道上的活动是什么，芯片都会回到STBY_RC模式。
 * CAD_RX   0x01  芯片执行CAD操作时，如果检测到一个活动，它将停留在RX中，直到检测到数据包或计时器达到cadtimeout*15.625 us定义的超时时间为止。
 */
//打开CAD检测
//uint32_t cad_num = 0;
void _CADStart(uint8_t *p_state)
{
	check_busy();
	uint16_t reg_val;
	_SetStandby(0);
	if(self.radio_cad.cadExitMode == SX126X_CAD_GOTO_RX)//检测到前导码后跳转到RX模式
	{
		_SetBufferBaseAddress(0,0);//(TX_base_addr,RX_base_addr)
		_SetDioIrqParams(SX126X_IRQ_RX_DONE | SX126X_IRQ_CRC_ERR | SX126X_IRQ_TIMEOUT , SX126X_IRQ_RX_DONE | SX126X_IRQ_TIMEOUT);
		_SetCad(self.radio_cad.cadSymbolNum,self.radio_cad.cadExitMode,self.radio_cad.cadTimeout);
		*p_state = SX1262_CAD_RX;
		
		_SetCADMode();
		SM_STATE_SET(self, RFLR_STATE_RX_RUNNING);
	}
	else//检测到前导码后跳转到RC模式
	{
		_SetDioIrqParams(SX126X_IRQ_CAD_DONE | SX126X_IRQ_CAD_DETECTED , SX126X_IRQ_CAD_DONE);
		_SetCad(self.radio_cad.cadSymbolNum,self.radio_cad.cadExitMode,self.radio_cad.cadTimeout);

		SM_STATE_SET(self, RFLR_STATE_CAD_RUNNING);

		_SetCADMode();
		
		while(dio1_pin_read() == 0){};//等待CAD检测完成
			
		/* 判断是否是CAD中断 */
    reg_val = _GetIrqStatus();
    if (reg_val & SX126X_IRQ_CAD_DONE) 
		{
			/* 清除done中断 */
			_ClearIrqStatus(SX126X_IRQ_CAD_DONE);//Clear the IRQ CADDone flag
			
			/* 判断是否 检测到前导码 */
			reg_val = _GetIrqStatus();
			if ((reg_val & SX126X_IRQ_CAD_DETECTED) == SX126X_IRQ_CAD_DETECTED )
			{			
//				cad_num++;
			 _ClearIrqStatus(SX126X_IRQ_CAD_DETECTED);//Clear the IRQ CADDone flag
			 *p_state = SX1262_CAD_DETECT;  /* 检测到前导码 */
			 SM_STATE_SET(self, RFLR_STATE_IDLE);
			}
			else
			{
				*p_state = SX1262_CAD_NONE;
				_SetStandby(0);
			}
		}
		SM_STATE_SET(self, RFLR_STATE_IDLE);
	}
}

//0:GFSK; 1:LORA  设置传输模式
void _SetPacketType()
{
	check_busy();
	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_PACKET_TYPE);
	spi_rw(0x01);
	sel_pin_set (1);
}

//设置发射频率
//RF_Freq = freq_reg*32M/(2^25)-----> freq_reg = (RF_Freq * (2^25))/32
void _SetRfFrequency(uint32_t frequency)
{
	check_busy();
	uint8_t Rf_Freq[4];
	uint32_t RfFreq = 0;
	self.radio_param.frequency = frequency;
	
	RfFreq = (uint32_t)((double)frequency / (double)FREQ_STEP);

	Rf_Freq[0] = (RfFreq>>24)&0xFF;//MSB
	Rf_Freq[1] = (RfFreq>>16)&0xFF;
	Rf_Freq[2] = (RfFreq>>8)&0xFF;
	Rf_Freq[3] = RfFreq&0xFF;//LSB

	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_RF_FREQUENCY);
	spi_rw(Rf_Freq[0]);
	spi_rw(Rf_Freq[1]);
	spi_rw(Rf_Freq[2]);
	spi_rw(Rf_Freq[3]);
	sel_pin_set(1);
}

//设置pa状态    选择sx1268设备    设置设备能达到的最大输出功率为22dbm
void _SetPaConfig()
{
	check_busy();

	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_PA_CONFIG);
	spi_rw(SX126X_PA_CONFIG_PA_Duty);	//paDutyCycle
	spi_rw(SX126X_PA_CONFIG_HP_MAX);	//hpMax:0x00~0x07; 7:22dbm  输出功率22dbm
	spi_rw(SX126X_PA_CONFIG_SX1268);//deviceSel: 0: SX1268
	spi_rw(SX126X_PA_CONFIG_PA_LUT);	
	sel_pin_set (1);
}

//设置校准器模式      DC_DC + LDO
void _SetRegulatorMode(void)
{
	check_busy();
	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_REGULATOR_MODE);
	spi_rw(SX126X_REGULATOR_DC_DC);//regModeParam
	sel_pin_set (1);
}

/*
power:
-17(0xEF) to +14(0x0E) dBm by step of 1 dB if low power PA is selected
-9(0xF7) to +22(0x16) dBm by step of 1 dB if high power PA is selected

RampTime:
-------------------------------------
RampTime 	  | Value | RampTime
-------------------------------------
SET_RAMP_10U    0x00    10
SET_RAMP_20U    0x01    20
SET_RAMP_40U 	0x02	40
SET_RAMP_80U 	0x03	80
SET_RAMP_200U 	0x04	200
SET_RAMP_800U 	0x05	800
SET_RAMP_1700U 	0x06	1700
SET_RAMP_3400U 	0x07	3400
*/
//设置发送功率和等待时间
void _SetTxParams(uint8_t power,uint8_t RampTime)
{
	check_busy();

	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_TX_PARAMS);
	spi_rw(power);
	spi_rw(RampTime);
	sel_pin_set (1);
}

//设置设备的扩频因子、带宽、通信码率、通信模式（lora）
void _SetModulationParams(uint8_t sf, uint8_t bw, uint8_t cr)
{
	check_busy();

	self.radio_param.sf = sf;
	self.radio_param.bandwidth = bw;
	self.radio_param.coderate = cr;

	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_MODULATION_PARAMS);

	spi_rw(sf);//SF=5~12
	spi_rw(bw);//BW
	spi_rw(cr);//CR
	spi_rw(SX126X_LORA_LOW_DATA_RATE_OPTIMIZE_ON);//LDRO LowDataRateOptimize 0:OFF; 1:ON;

	spi_rw(0XFF);//
	spi_rw(0XFF);//
	spi_rw(0XFF);//
	spi_rw(0XFF);//

	sel_pin_set (1);
}

//设置负载参数
void _SetPacketParams(uint16_t preamble_len,bool header_mode,uint8_t payload_len,bool crc_on)
{
	uint16_t prea_len;
	uint8_t prea_len_h,prea_len_l;

	check_busy();

	self.radio_param.preamble_len = preamble_len;
	self.radio_param.header_mode = header_mode;
	self.radio_param.payload_len = payload_len;
	self.radio_param.crc_on = crc_on;

	prea_len = preamble_len;
	prea_len_h = prea_len>>8;
	prea_len_l = prea_len&0xFF;

	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_PACKET_PARAMS);

	spi_rw(prea_len_h);//PreambleLength MSB   设置前导码长度
	spi_rw(prea_len_l);//PreambleLength LSB
	spi_rw(header_mode);//HeaderType 0:Variable,explicit 1:Fixed,implicit   设置报头
	spi_rw(payload_len);//PayloadLength: 0x00 to 0xFF   设置负载长度
	spi_rw(crc_on);//CRCType 0:OFF 1:ON
	spi_rw(SX126X_LORA_IQ_STANDARD);//InvertIQ 0:Standard 1:Inverted
	spi_rw(0XFF);
	spi_rw(0XFF);
	spi_rw(0XFF);

	sel_pin_set(1);
}


//向FIFO中写入数据
void _WriteBuffer(uint8_t offset, uint8_t *data, uint8_t length)
{
	uint8_t i;

	if(length<1)
		return;

	check_busy();
	sel_pin_set(0);
	spi_rw(SX126X_CMD_WRITE_BUFFER);
	spi_rw(offset);
	for(i=0;i<length;i++)
	{
		spi_rw(data[i]);
	}
	sel_pin_set (1);
}

//读取fifo中的数据
void _ReadBuffer(uint8_t offset, uint8_t *data, uint8_t length)
{
	uint8_t i;

	if(length<1)
		return;
	check_busy();

	sel_pin_set(0);
	spi_rw(SX126X_CMD_READ_BUFFER);
	spi_rw(offset);
	spi_rw(0xFF);
	for(i=0;i<length;i++)
	{
		data[i]=spi_rw(0xFF);
	}
	sel_pin_set (1);
}


//获取信号强度
uint8_t _GetRssiInst(void)
{
	uint8_t rssi;
	check_busy();
	sel_pin_set(0);
	spi_rw(SX126X_CMD_GET_RSSI_INST);
	spi_rw(0xFF);
	rssi = spi_rw(0xFF);
	sel_pin_set (1);
	return rssi;
}


int _Tx_Data(uint8_t *txbuf,uint8_t payload_length)
{
	SM_STATE_SET(self, RFLR_STATE_TX_RUNNING);
	uint16_t Irq_Status;
	_SetStandby(0);//0:STDBY_RC; 1:STDBY_XOSC 配置设备参数
	_SetBufferBaseAddress(0,0);//(TX_base_addr,RX_base_addr)
	
	_WriteBuffer(0,txbuf,payload_length);//(offset,*data,length)
	
	_SetPacketParams(self.radio_param.preamble_len,self.radio_param.header_mode,payload_length,self.radio_param.crc_on);//PreambleLength;HeaderType;PayloadLength;CRCType;InvertIQ

	_SetDioIrqParams(SX126X_IRQ_TX_DONE|SX126X_IRQ_TIMEOUT , SX126X_IRQ_TX_DONE|SX126X_IRQ_TIMEOUT);//TxDone IRQ   设置DIO1位中断位
	
	//Define Sync Word value（待机模式才能更改配置参数）
	_SetTx(self.radio_param.tx_pkt_timeout);//timeout = 320000 * 15.625us = 5s

	//Wait for the IRQ TxDone or Timeout
	while(dio1_pin_read() == 0);//等待数据发送完成或时间结束

	
	Irq_Status = _GetIrqStatus();
	if((Irq_Status & SX126X_IRQ_TIMEOUT)== SX126X_IRQ_TIMEOUT)
	{
		_ClearIrqStatus(SX126X_IRQ_TIMEOUT);
		return LORA_RET_CODE_ERR;
	}
	else if((Irq_Status & SX126X_IRQ_TX_DONE)== SX126X_IRQ_TX_DONE)
	{
		_ClearIrqStatus(SX126X_IRQ_TX_DONE);
		//_SetStandby(0);//设置为待机模式
		return LORA_RET_CODE_OK;
	}
	return LORA_RET_CODE_ERR;
}

//获取读取数据负载信息
void _GetRxBufferStatus(uint8_t *payload_len, uint8_t *buf_pointer)
{
	check_busy();

	sel_pin_set(0);
	spi_rw(SX126X_CMD_GET_RX_BUFFER_STATUS);

	spi_rw(0xFF);
	*payload_len = spi_rw(0xFF);
	*buf_pointer = spi_rw(0xFF);
	sel_pin_set(1);
}

//接收模式初始化（接收数据时需要先调用该函数）
void _SetRxMode(void)
{
	SM_STATE_SET(self, RFLR_STATE_RX_RUNNING);
	_SetBufferBaseAddress(0,0);	//(TX_base_addr,RX_base_addr)
	_SetDioIrqParams(SX126X_IRQ_RX_DONE | SX126X_IRQ_CRC_ERR | SX126X_IRQ_TIMEOUT,SX126X_IRQ_RX_DONE | SX126X_IRQ_TIMEOUT);//RxDone IRQ
	_SetRx(self.radio_param.rx_mode,self.radio_param.rx_pkt_timeout);//timeout = 0
}

uint8_t _GetPtkStatus(void)
{
	check_busy();
	sel_pin_set(0);
	spi_rw(SX126X_CMD_GET_PACKET_STATUS);
	uint8_t status = spi_rw(0xFF);
	uint8_t rx_status = spi_rw(0xFF);
	uint8_t rssi_sync = spi_rw(0xFF);
	uint8_t rssi_avg = spi_rw(0xFF);
	sel_pin_set(1);
	return rssi_avg;
}

//接收数据
int _Rx_Data(uint8_t *addr,uint8_t *size)
{
	uint8_t buf_offset;
	uint16_t Irq_Status;
	Irq_Status = _GetIrqStatus();//读取中断状态
	if(Irq_Status & SX126X_IRQ_RX_DONE)
	{
		_ClearIrqStatus(SX126X_IRQ_RX_DONE);//Clear the IRQ RxDone flag

		Irq_Status = _GetIrqStatus();
		if((Irq_Status & SX126X_IRQ_CRC_ERR)== SX126X_IRQ_CRC_ERR)
		{
			_ClearIrqStatus(SX126X_IRQ_CRC_ERR);//Clear the IRQ CRC_ERR flag
			*size = 0;
			return LORA_RET_RECV_CRC_ERR;
		}

		_GetRxBufferStatus(size, &buf_offset);
		_ReadBuffer(buf_offset, addr, *size);
		self.radio_state.rssi = 0-(_GetPtkStatus())/2;

		if (self.radio_param.rx_mode == CONTINUOUS_RECV_MODE)
		{
			  SM_STATE_SET(self, RFLR_STATE_RX_RUNNING);
		}
		else
		{
		    SM_STATE_SET(self, RFLR_STATE_IDLE);
		}
		
		return LORA_RET_CODE_OK;
	}
	else
	{
		*size = 0;
		return LORA_RET_CODE_ERR;
	}
}

/*
0x00 DIO3 outputs 1.6 V to supply the TCXO
0x01 DIO3 outputs 1.7 V to supply the TCXO
0x02 DIO3 outputs 1.8 V to supply the TCXO
0x03 DIO3 outputs 2.2 V to supply the TCXO
0x04 DIO3 outputs 2.4 V to supply the TCXO
0x05 DIO3 outputs 2.7 V to supply the TCXO
0x06 DIO3 outputs 3.0 V to supply the TCXO
0x07 DIO3 outputs 3.3 V to supply the TCXO
*/

//温度补偿晶振电源输入DIO3口
void _SetDIO3AsTCXOCtrl(uint8_t tcxoVoltage)
{
	check_busy();

	sel_pin_set(0);
	spi_rw(SX126X_CMD_SET_DIO3_AS_TCXO_CTRL);
	spi_rw(tcxoVoltage);   //
	spi_rw(0x00);		   //Timeout MSB ; Timeout duration = Timeout *15.625 祍
	spi_rw(0x00);
	spi_rw(0x64);      //Timeout LSB

	sel_pin_set (1);
}

/*
 * status bytes definition
 *      6:4                         3:1
 * 0x2: STBY_RC        0x2: Data is available to host
 * 0x3: STBY_XOSC      0x3: Command timeout
 * 0x4: FS             0x4: Command processing error
 * 0x5: RX             0x5: Failure to execute command
 * 0x6: TX             0x6: Command TX done
 */
uint8_t _GetStatus(radio_state_t *p_state)
{
	   switch (SM_STATE_GET(self)) {
	    case RFLR_STATE_TX_RUNNING:
	    case RFLR_STATE_TX_DONE:
	    case RFLR_STATE_TX_INIT:
	    case RFLR_STATE_TX_TIMEOUT:
	        *p_state = TX_RUNING_ST;
	        break;

	    case RFLR_STATE_RX_INIT:
	    case RFLR_STATE_RX_RUNNING:
	    case RFLR_STATE_RX_DONE:
	    case RFLR_STATE_RX_TIMEOUT:
	        *p_state = RX_RUNING_ST;
	        break;

	    case RFLR_STATE_CAD_RUNNING:
		case RFLR_STATE_CAD_INIT:
	        *p_state = CAD_ST;
	        break;

	    case RFLR_STATE_IDLE:
	        *p_state = IDLE_ST;
	        break;

	    default:
		    *p_state = IDLE_ST;
	        break;
	    }

	    return LORA_RET_CODE_OK;
}

//设备参数配置
void _sx1262_init()
{
//在待机模式下设置lora参数
	 CAD_STATE_CLR(self);
	 SM_STATE_SET(self, RFLR_STATE_IDLE);

	check_busy();
	_SetStandby(0);//0:STDBY_RC; 1:STDBY_XOSC 设置晶振大小，并等待主机配置
	_SetRegulatorMode();//设置校准器模式
	_SetPaConfig();//选择设备设置输出功率
	
	_SetDIO3AsTCXOCtrl(self.radio_param.tcxoVoltage);//温度补偿晶体振荡器电源
	
	_SetPacketType();	 //0:GFSK; 1:LORA  选择工作模式为lora
	
	_SetRfFrequency(self.radio_param.frequency);//434M ; RF_Freq = freq_reg*32M/(2^25)  设置发射频率
	
	_SetTxParams(self.radio_param.power,SX126X_PA_RAMP_10U);//set power and ramp_time 设置发射功率和等待时间

	//设置设备的扩频因子、带宽、通信码率、通信模式（lora）
	_SetModulationParams(self.radio_param.sf,self.radio_param.bandwidth,self.radio_param.coderate);

	//设置前导码长度、报头模式、CRC校验开关(负载长度)
	_SetPacketParams(self.radio_param.preamble_len,self.radio_param.header_mode,self.radio_param.payload_len,self.radio_param.crc_on);
}

//中断调用该函数
int _lora_dio1_irq_func(uint8_t *addr,uint8_t *size)
{
    switch (SM_STATE_GET(self)) {
    case RFLR_STATE_RX_RUNNING:
       {
    	   return _Rx_Data(addr,size);
       }
    default:
        break;
    }
		return LORA_RET_CODE_ERR;
}

static uint32_t __sx1262_time_on_air_get (uint8_t pkt_len)
{
    uint32_t len, bw, dr, cr, preamble_len;
    uint64_t ts, tpreamble, tpayload, tmp;    /* unit: 1e-6 */
    uint32_t air_time = 0;

    len          = pkt_len;
    dr           = self.radio_param.sf;
    cr           = self.radio_param.coderate;
    preamble_len = self.radio_param.preamble_len;

    // REMARK: When using LoRa modem only bandwidths 125, 250 and 500 kHz are supported
    switch (self.radio_param.bandwidth) {
        case 0x0: // 7.81 kHz
            bw = 7.81 * 1000;
            break;

        case 0x8: // 10.42 kHz
            bw = 10.42 * 1000;
            break;		
		
        case 0x1: // 15.63 kHz
            bw = 15.63 * 1000;
            break;		

        case 0x9: // 20.83 kHz
            bw = 20.83 * 1000;
            break;	

        case 0x2: // 31.25 kHz
            bw = 31.25 * 1000;
            break;		
		
        case 0xA: // 41.67 kHz
            bw = 41.67 * 1000;
            break;
		
        case 0x3: // 62.5 kHz
            bw = 62.5 * 1000;
            break;
		
        case 0x04: // 125 kHz
            bw = 125 * 1000;
            break;

        case 0x05: // 250 kHz
            bw = 250 * 1000;
            break;

        case 0x06: // 500 kHz
            bw = 500 * 1000;
            break;

        default:
            return 0;
    }

    // Symbol rate : time for one symbol (secs)
    ts = ((uint64_t)(1 << dr)) * (uint64_t)1000000 / bw;

    // time of preamble
    tpreamble = (preamble_len * (uint64_t)1000000 + (uint64_t)(4.25 * 1000000)) * ts / (uint64_t)1000000;

    // Symbol length of payload and time
    tmp = (8 * len - 4 * dr + 28 +
           16 * (self.radio_param.crc_on ? 1 : 0) -
           (self.radio_param.header_mode ? 0 : 20)) * (uint64_t)(1000000)
        / (4 * (dr - ((0 > 0) ? 2 : 0)));

    /* ceil */
    if (tmp > 0) {
        if (tmp % (uint64_t)1000000 < 10000) {
            tmp = ((tmp / (uint64_t)1000000) * (cr + 4)) * (uint64_t)1000000;
        } else {
            tmp = ((1 + (tmp / (uint64_t)1000000)) * (cr + 4)) * (uint64_t)1000000;
        }
    } else {
        tmp = 0;
    }

    tpayload = (8 * (uint64_t)1000000 + tmp) * ts / (uint64_t)1000000;

    air_time = 1 + (tpreamble + tpayload) / (uint64_t)1000;

    // Time on air, ms
    return air_time;
}

int8_t _sx1262_get_rssi(void)
{
	return self.radio_state.rssi;
}

radio_drv_funcs_t __sx1262_drv_funcs[] = {
//设备初始化
		_Reset,
		_sx1262_init,
//模式设置
		_SetSleep,
		_SetStandby,
		_CADStart,
		_SetRxMode,
		_Tx_Data,
//设备参数设置
		_SetTxTime,
		_SetRxTime,
		_SetCadParams,
		_SetRfFrequency,
		_SetTxParams,
		_SetModulationParams,
		_SetPacketParams,
//获取设备状态
		_GetStatus,
		_GetChipStatus,
//其他
		_lora_dio1_irq_func,
		_SetDIO3AsTCXOCtrl,
		__sx1262_time_on_air_get,
		_sx1262_get_rssi,
};

sx1262_CADParams_t __sx1262_CAD_param = {
	SX126X_CAD_ON_16_SYMB,
	SX126X_CAD_GOTO_STDBY,
	2000,
};

sx1262_lora_param_set_t __sx1262_lora_param = {
	
	470000000,       						/* 频率  434M ; RF_Freq = freq_reg*32M/(2^25)*/
    20,            							/* 发射功率   -9 ~ 22 */
	5,              					    /*带宽 [0x00:7.81kHz,0x08:10.42kHz,0x01:15.63kHz,0x09:20.83kHz,0x02:31.25kHz,
													0x0A:41.67kHz,0x03:62.50kHz,0x04:125kHz,0x05:250kHz,0x06:500kHz,] */
    7,              						/* 扩频因子 [5:32 6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips] */
    1,              						/* 编码率 [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8] */
    1,              						/* CRC检验 [0: 关, 1: 开] */
    0,              						/* 报头模式 [0: 显式报头模式, 1: 隐式报头模式] 注：SF为6时只能使用隐式报头*/
    1,              						/* 接收模式 [0: Continuous, 1 Single] */
    320000,           						/* 发送超时 */
    0,            							/* 接收超时 */
    10,            							/* 负载长度 */
    14,             						/* 前导码长度  */
	SX126X_DIO3_OUTPUT_3_3,			  		/*晶振电源输入大小*/
};

radio_drv_funcs_t radio_sx1262_lora_init()
{
	self.p_drive     = __sx1262_drv_funcs;
	self.radio_param = __sx1262_lora_param;
	self.radio_cad   = __sx1262_CAD_param;
	
	sys_param_t* param = sys_param_get_handle();
	self.radio_param.frequency = param->lora_freq * 1000 * 1000;
	self.radio_param.power = param->lora_power;
	uint8_t lora_bw[] = {0X00, 0X08, 0X01, 0X09, 0X02, 0X0A, 0X03, 0X04, 0X05, 0X06}; 
	self.radio_param.bandwidth = lora_bw[param->lora_bw];
	self.radio_param.sf = param->lora_sf;
	self.radio_param.coderate = param->lora_code_rate;
	self.radio_param.preamble_len = param->lora_preamble;
	self.radio_param.header_mode = param->lora_header;
	self.radio_param.crc_on = param->lora_crc;	

	return *self.p_drive;
}


