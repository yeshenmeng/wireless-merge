/*
 * sx1262.h
 *
 *  Created on: 2019年6月26日
 *      Author: xxk
 */

#ifndef SX1262_H_
#define SX1262_H_

#include "sx1262_regs.h"
#include "function.h"

/**
 * \brief 模块当前运行状态
 */
typedef enum {
    IDLE_ST = 0,
    SLEEP_ST,
    RX_RUNING_ST,
    TX_RUNING_ST,
	  CAD_ST
} radio_state_t;

#define SX1262_CAD_NONE       0X00    /**< \brief ???????  */
#define SX1262_CAD_DETECT     0X01    /**< \brief ?????? */
#define SX1262_CAD_RX		  0X10 
/**
 * \name grp_lora_state
 */
typedef enum {
    RFLR_STATE_IDLE,    	//空闲状态
    RFLR_STATE_RX_INIT,
    RFLR_STATE_RX_RUNNING,
    RFLR_STATE_RX_DONE,
    RFLR_STATE_RX_TIMEOUT,
    RFLR_STATE_TX_INIT,
    RFLR_STATE_TX_RUNNING,
    RFLR_STATE_TX_DONE,
    RFLR_STATE_TX_TIMEOUT,
    RFLR_STATE_CAD_INIT,
    RFLR_STATE_CAD_RUNNING,
} sx1262_lora_sm_t;

/*
 * \name grp_lora_cad_state  CAD 状态
 */
#define CAD_STATE_START         0x01
#define CAD_STATE_FINISH        0x02
#define CAD_STATE_DONE          0x04
#define CAD_STATE_DETECT        0x08


/*
 * \brief 运行状态
 *  cad_state值参考：grp_lora_cad_state
 *  sm_state 值参考：grp_lora_state
 */
typedef struct sx127x_setting {
    uint8_t          cad_state;
    sx1262_lora_sm_t sm_state;
} sx1262_run_state_t;


/*
 * \brief 函数返回值
 */
enum {
	LORA_RET_CODE_OK = 0,
	LORA_RET_CODE_ERR,
	LORA_RET_CODE_NO_CAD_DONE_IRQ,
	LORA_RET_CODE_NO_CAD_DETECT_IRQ,
	LORA_RET_CAD_RUNNING,
	LORA_RET_RECV_CRC_ERR
};

/*
 * \name  grp_lora_recv_mode
 */
#define CONTINUOUS_RECV_MODE      0
#define SINGLE_RECV_MODE          1

/*
 * \name  grp_lora_packet_mode
 */
#define EXPLICIT_HEADER         0
#define IMPLICIT_HEADER         1


/**
 * \brief LoRa参数配置
 */
typedef struct sx1262_lora_param_set {
    uint32_t frequency;              /* 频率 */
    int8_t   power;                  /* 发射功率 */
    uint8_t  bandwidth;              /* 带宽 [0: 7.8 kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
                                              5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz,  9: 500 kHz, other: Reserved] */
    uint8_t  sf;                     /* 扩频因子 [5:32，6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips] */
    uint8_t  coderate;               /* 编码率 [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8] */
    bool     crc_on;                 /* CRC检验 [0: 关闭, 1: 开启] */
    bool     header_mode;            /* 报头模式 [0: 显式报头模式, 1: 隐式报头模式] 注：SF为6时只能使用隐式报头 */
    bool     rx_mode;                /* 接收模式 [0: Continuous, 1 Single] */
    uint32_t tx_pkt_timeout;         /* 发送超时 */
    uint32_t rx_pkt_timeout;         /* 接收超时 */
    uint8_t  payload_len;            /* 负载长度 */
    uint16_t preamble_len;           /* 前导码长度 */
    uint8_t  tcxoVoltage;			 /* 晶振电源输入大小*/
} sx1262_lora_param_set_t;

/*
 * \brief packet 包信息结构体
 */
typedef struct sx1262_pkt_state {
	bool    crc_pass;
    bool    rx_done;
    bool    tx_done;
    int8_t  snr;
    int16_t rssi;
} sx1262_pkt_info_t;


typedef struct sx1262_CADParams{
	uint8_t  cadSymbolNum;
	uint8_t  cadExitMode;
	uint32_t cadTimeout;
}sx1262_CADParams_t;


/**
 * \brief 模块驱动函数指针
 */
typedef struct radio_drv_funcs {
	  void					 (*radio_reset)									(void);
	  void           (*radio_init)                  (void);

    void           (*radio_sleepmode)             (void);
    void           (*radio_standmode)							(uint8_t StdbyConfig);
    void           (*radio_cadmode) 							(uint8_t *state);
    void           (*radio_Rxmode)								(void);
    int            (*radio_TXData)								(uint8_t *txbuf,uint8_t payload_length);

    void           (*radio_setTxTime)             (uint32_t time);
    void           (*radio_setRxTime)             (bool rx_mode,uint32_t time);
    void           (*radio_setCadParam)           (uint8_t cadSymbolNum,uint8_t cadExitMode,uint32_t timeout);
    void           (*radio_setFrequency)          (uint32_t freq);
    void  		     (*radio_setTxparams)						(uint8_t power,uint8_t RampTime);
    void           (*radio_setModulationParams)		(uint8_t sf, uint8_t bw, uint8_t cr);
    void		       (*radio_setPacketParams)				(uint16_t preamble_len,bool header_mode,uint8_t payload_len,bool crc_on);

    uint8_t		     (*radio_GetStatus)							(radio_state_t *p_state);
	  void  		     (*radio_GetChipStatus)					(uint8_t status,uint16_t IrqStatus);

    int 		       (*radio_dio1_irq_func)					(uint8_t *addr,uint8_t *size);
    void		       (*radio_SetDIO3AsTCXOCtrl)			(uint8_t tcxoVoltage);  //设置补偿晶振电源输入大小
	uint32_t (*radio_time_on_air_get)(uint8_t pkt_len);
	int8_t (*radio_get_rssi)(void);
}radio_drv_funcs_t;


typedef struct sx1262_drive
{
	radio_drv_funcs_t        *p_drive;

	sx1262_lora_param_set_t  radio_param;
	sx1262_run_state_t       run_state;
	sx1262_pkt_info_t 			 radio_state;
	sx1262_CADParams_t			 radio_cad;
} sx1262_drive_t;

/**
 * \brief 驱动初始函数
 */
radio_drv_funcs_t radio_sx1262_lora_init(void);

#endif /* SX1262_H_ */
