#ifndef __FLASH_H__
#define __FLASH_H__
#include "main.h"


#define TH_FLASH_PAGE_SIZE_2_POWER 12	//4096bytes = 2^12 bytes


void fs_flash_init(void);

/*******************************************************************************
* Function Name  : flash_write
* Description    : flash��д����
* Input          : u32 pageStartAddr: ��дflashҳ����ʼ��ַ
* 				   u32 *p_data: ��ҳ��д�����������
* 				   u32 size: д�����ݵĸ���
* Return         : 0: ��ȷִ��,
* 				   1: ��������
* 				   2: write error
* Attention		 : ��������һ����u32 ��ָ�룬������һ���ǰ���4�ֽڶ���д��ġ����ԣ�sizeҲ��u32�ĸ������ֽ�����4��֮һ��
*******************************************************************************/
uint8_t flash_write(uint32_t pageStartAddr, uint32_t *pData, uint32_t size);

//����byte��ȡ
void flash_read(uint32_t startAddr, uint8_t *pData, uint32_t size);

/*******************************************************************************
* Function Name  : flash_read
* Description    : flash�Ķ�����
* Input          : u32 startAddr,u32 *p_data,u32 size
* Output         : 0����ȷִ��,	 ��0������
* Attention		 : ��������һ����u32 ��ָ�룬������һ���ǰ���4�ֽڶ���д��ġ����ԣ�sizeҲ��u32�ĸ������ֽ�����4��֮һ��
*******************************************************************************/
//void flash_read(uint32_t StartAddr,uint32_t *pData,uint32_t size);
#endif
