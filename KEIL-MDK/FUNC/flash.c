/********************************************************************************
  * �ļ����� flash.c 
  * ������   flash��д����
  * ��汾�� V1.1.1
  * ���ڣ�   2019.9.19
  * ���ߣ�   HYP
  * ���£�
  * 		 1. �޸�Ϊnordic��д�� ��hyp 2019.9.18)
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
	
	if(p_evt->result != NRF_SUCCESS) //FS��������
	{
		return;
	}
	
	switch((uint8_t)p_evt->id)
	{
		case NRF_FSTORAGE_EVT_READ_RESULT: //������¼�
			break;
		
		case NRF_FSTORAGE_EVT_WRITE_RESULT: //д����¼�
			break;
		
		case NRF_FSTORAGE_EVT_ERASE_RESULT: //��������¼�
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
* Description    : flash�ĳ�ʼ������
* Input          : ��
* Return         : ��
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
* Description    : flash��д����
* Input          : u32 pageStartAddr: ��дflashҳ����ʼ��ַ
* 				   u32 *p_data: ��ҳ��д�����������
* 				   u32 size: д�����ݵĸ���
* Return         : 0: ��ȷִ��,
* 				   1: ��������
* 				   2: write error
* Attention		 : ��������һ����u32 ��ָ�룬������һ���ǰ���4�ֽڶ���д��ġ����ԣ�sizeҲ��u32�ĸ������ֽ�����4��֮һ��
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
* Description    : flash�Ķ�����
* Input          : u32 startAddr,u32 *p_data,u32 size
* Attention		 : ��������һ����u32 ��ָ�룬������һ���ǰ���4�ֽڶ���д��ġ����ԣ�sizeҲ��u32�ĸ������ֽ�����4��֮һ��
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
