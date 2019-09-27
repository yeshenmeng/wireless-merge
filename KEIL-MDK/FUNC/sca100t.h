#ifndef __SCA100T_H__
#define __SCA100T_H__
#include "main.h"


/* 软件模拟SPI引脚设置 */
#define SCA_SW_SPI_SCK_PIN				12
#define SCA_SW_SPI_MOSI_PIN				14
#define SCA_SW_SPI_MISO_PIN				13
#define SCA_SW_SPI_CS_PIN				11

/* 软件模拟SPI端口设置 */
#define SCA_SW_SPI_SCK_PORT				P0
#define SCA_SW_SPI_MOSI_PORT			P0
#define SCA_SW_SPI_MISO_PORT			P0
#define SCA_SW_SPI_CS_PORT				P0


//========SCA100T控制命令
#define MEAS  0x00	//测量模式
#define RWTR  0x08	//读写温度数据寄存器
#define RDSR  0x0A	//状态寄存器
#define RLOAD 0x0B	//重新载入NV数据到内存输出寄存器
#define STX   0x0E	//X通道自检
#define STY   0x0F	//Y通道自检
#define RDAX  0x10	//读X通道加速度数据
#define RDAY  0x11	//读Y通道加速度数据 


void sca_init(void);
void sca_default(void);
void sca_write_cmd(uint8_t cmd);
uint8_t sca_read_temp(void);
uint16_t sca_read_x_channel(void);
uint16_t sca_read_y_channel(void);


#endif


