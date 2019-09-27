#ifndef __FLASH_H__
#define __FLASH_H__
#include "main.h"


#define TH_FLASH_PAGE_SIZE_2_POWER 12	//4096bytes = 2^12 bytes


void fs_flash_init(void);

/*******************************************************************************
* Function Name  : flash_write
* Description    : flash的写函数
* Input          : u32 pageStartAddr: 待写flash页的起始地址
* 				   u32 *p_data: 本页待写入的连续数据
* 				   u32 size: 写入数据的个数
* Return         : 0: 正确执行,
* 				   1: 擦除错误
* 				   2: write error
* Attention		 : 输入数据一定是u32 的指针，即数据一定是按照4字节对齐写入的。所以：size也是u32的个数（字节数的4分之一）
*******************************************************************************/
uint8_t flash_write(uint32_t pageStartAddr, uint32_t *pData, uint32_t size);

//按照byte读取
void flash_read(uint32_t startAddr, uint8_t *pData, uint32_t size);

/*******************************************************************************
* Function Name  : flash_read
* Description    : flash的读函数
* Input          : u32 startAddr,u32 *p_data,u32 size
* Output         : 0：正确执行,	 非0：出错
* Attention		 : 输入数据一定是u32 的指针，即数据一定是按照4字节对齐写入的。所以：size也是u32的个数（字节数的4分之一）
*******************************************************************************/
//void flash_read(uint32_t StartAddr,uint32_t *pData,uint32_t size);
#endif
