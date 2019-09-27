#ifndef __SCA100T_H__
#define __SCA100T_H__
#include "main.h"


/* ���ģ��SPI�������� */
#define SCA_SW_SPI_SCK_PIN				12
#define SCA_SW_SPI_MOSI_PIN				14
#define SCA_SW_SPI_MISO_PIN				13
#define SCA_SW_SPI_CS_PIN				11

/* ���ģ��SPI�˿����� */
#define SCA_SW_SPI_SCK_PORT				P0
#define SCA_SW_SPI_MOSI_PORT			P0
#define SCA_SW_SPI_MISO_PORT			P0
#define SCA_SW_SPI_CS_PORT				P0


//========SCA100T��������
#define MEAS  0x00	//����ģʽ
#define RWTR  0x08	//��д�¶����ݼĴ���
#define RDSR  0x0A	//״̬�Ĵ���
#define RLOAD 0x0B	//��������NV���ݵ��ڴ�����Ĵ���
#define STX   0x0E	//Xͨ���Լ�
#define STY   0x0F	//Yͨ���Լ�
#define RDAX  0x10	//��Xͨ�����ٶ�����
#define RDAY  0x11	//��Yͨ�����ٶ����� 


void sca_init(void);
void sca_default(void);
void sca_write_cmd(uint8_t cmd);
uint8_t sca_read_temp(void);
uint16_t sca_read_x_channel(void);
uint16_t sca_read_y_channel(void);


#endif


