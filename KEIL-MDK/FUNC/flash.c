/********************************************************************************
  * 文件名： flash.c 
  * 描述：   flash读写操作
  * 库版本： V1.1.1
  * 日期：   2019.9.19
  * 作者：   HYP
  * 更新：
  * 		 1. 修改为nordic的写法 （hyp 2019.9.18)
  * 
  *
********************************************************************************/  

/* Includes ------------------------------------------------------------------*/
#include "flash.h"
#include "nrf_fstorage.h"
#include "math.h"

#ifdef SOFTDEVICE_PRESENT
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_fstorage_sd.h"
#else
#include "nrf_drv_clock.h"
#include "nrf_fstorage_nvmc.h"
#include "nrf_nvmc.h"
#endif


static void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage)
{
    /* While fstorage is busy, sleep and wait for an event. */
    while (nrf_fstorage_is_busy(p_fstorage));
}

static uint32_t nrf_flash_end_addr_get()
{
    uint32_t const bootloader_addr = NRF_UICR->NRFFW[0];
    uint32_t const page_sz         = NRF_FICR->CODEPAGESIZE;
    uint32_t const code_sz         = NRF_FICR->CODESIZE;

    return (bootloader_addr != 0xFFFFFFFF ?
            bootloader_addr : (code_sz * page_sz));
}

static void fstorage_evt_handler(nrf_fstorage_evt_t* p_evt)
{
	
	if(p_evt->result != NRF_SUCCESS) //FS操作错误
	{
		return;
	}
	
	switch((uint8_t)p_evt->id)
	{
		case NRF_FSTORAGE_EVT_READ_RESULT: //读完成事件
			break;
		
		case NRF_FSTORAGE_EVT_WRITE_RESULT: //写完成事件
			break;
		
		case NRF_FSTORAGE_EVT_ERASE_RESULT: //擦除完成事件
			break;
		default: break;
	}
}

NRF_FSTORAGE_DEF(nrf_fstorage_t nrf_flash_write) = {
	.evt_handler = fstorage_evt_handler,
};	

#ifdef SOFTDEVICE_PRESENT
nrf_fstorage_api_t* p_fs_api = &nrf_fstorage_sd;
#else
nrf_fstorage_api_t* p_fs_api = &nrf_fstorage_nvmc;
#endif

/*******************************************************************************
* Function Name  : fs_flash_init
* Description    : flash的初始化函数
* Input          : 无
* Return         : 无
*******************************************************************************/
void fs_flash_init(void)
{
	uint32_t err_code;
	err_code = nrf_fstorage_init(&nrf_flash_write, p_fs_api, NULL);
	nrf_flash_write.start_addr = 0x0;
	nrf_flash_write.end_addr = nrf_flash_end_addr_get();
	APP_ERROR_CHECK(err_code);
}

#ifdef SOFTDEVICE_PRESENT
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
uint8_t flash_write(uint32_t pageStartAddr, uint32_t *pData, uint32_t size)
{
	uint32_t err_code;
	nrf_flash_write.start_addr = pageStartAddr;
	nrf_flash_write.end_addr = nrf_flash_end_addr_get();

	uint32_t endAddr = pageStartAddr + 4*size;
	uint32_t page_size = pow(2,TH_FLASH_PAGE_SIZE_2_POWER);
	uint8_t pageNb = (((endAddr - pageStartAddr) % page_size) > 0) ?
					 (((endAddr - pageStartAddr) >> TH_FLASH_PAGE_SIZE_2_POWER) + 1) :
					  ((endAddr - pageStartAddr) >> TH_FLASH_PAGE_SIZE_2_POWER);	
	
	err_code = nrf_fstorage_erase(&nrf_flash_write, pageStartAddr, pageNb, NULL);
	APP_ERROR_CHECK(err_code);
	
	err_code = nrf_fstorage_write(&nrf_flash_write, pageStartAddr, pData, 4*size, NULL);
	APP_ERROR_CHECK(err_code);
	
	wait_for_flash_ready(&nrf_flash_write);

	return err_code;
}

/*******************************************************************************
* Function Name  : flash_read
* Description    : flash的读函数
* Input          : u32 startAddr,u32 *p_data,u32 size
* Attention		 : 输入数据一定是u32 的指针，即数据一定是按照4字节对齐写入的。所以：size也是u32的个数（字节数的4分之一）
*******************************************************************************/
void flash_read(uint32_t startAddr, uint8_t *pData, uint32_t size)
{
	uint32_t err_code;
	err_code = nrf_fstorage_read(&nrf_flash_write, startAddr, pData, size);
	APP_ERROR_CHECK(err_code);
}
#else
uint8_t flash_write(uint32_t pageStartAddr, uint32_t *pData, uint32_t size)
{
	uint32_t endAddr = pageStartAddr + 4*size;
	uint8_t pageNb = (((endAddr - pageStartAddr) % TH_FLASH_PAGE_SIZE_2_POWER) > 0) ?
					 (((endAddr - pageStartAddr) >> TH_FLASH_PAGE_SIZE_2_POWER) + 1) :
					 ((endAddr - pageStartAddr) >> TH_FLASH_PAGE_SIZE_2_POWER);
	
	for(uint8_t i=0; i<pageNb; i++)
	{
		nrf_nvmc_page_erase(pageStartAddr + i * TH_FLASH_PAGE_SIZE_2_POWER);
	}

	nrf_nvmc_write_words(pageStartAddr, pData, size);

	return 0;
}

void flash_read(uint32_t startAddr,uint8_t *pData, uint32_t size)
{
	uint8_t i;
	for(i = 0; i<size; i++)
	{
		pData[i] = *((uint8_t*) startAddr);
		startAddr++;
	}
}
#endif

/********************** (C) COPYRIGHT 2019 TOHOLED *****END OF FILE*********************/
