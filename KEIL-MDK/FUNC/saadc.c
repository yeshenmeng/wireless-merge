#include "saadc.h"
#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"


static const nrf_drv_timer_t m_timer = NRF_DRV_TIMER_INSTANCE(0);
static nrf_saadc_value_t m_buffer_pool[2][SAADC_SAMPLE_IN_BUFFER]; //˫����
static nrf_ppi_channel_t m_ppi_channel;
static uint8_t m_saadc_is_enable = 0;
static uint8_t m_saadc_sample_mode = 0;
static int16_t m_sample_data;

__weak void saadc_sample_cplt_cb(void* param)
{
	
}

static void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
#if (SAADC_SAMPLE_MODE == 0)
		if(m_saadc_sample_mode == 1)
		{
			return;
		}
#endif
		
        ret_code_t err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAADC_SAMPLE_IN_BUFFER);
        APP_ERROR_CHECK(err_code);

		int32_t sum = 0;
        for (int i = 0; i < SAADC_SAMPLE_IN_BUFFER; i++)
        {
			sum += p_event->data.done.p_buffer[i];
        }
		m_sample_data = (int16_t)(sum / SAADC_SAMPLE_IN_BUFFER);
		
#if (SAADC_SAMPLE_MODE == 0)
		m_saadc_sample_mode = 1;
		nrf_drv_timer_disable(&m_timer);
		nrf_saadc_task_trigger(NRF_SAADC_TASK_STOP);
#endif
		saadc_sample_cplt_cb(NULL);
    }
}

static void timer_handler(nrf_timer_event_t event_type, void * p_context)
{
	
}

static void saadc_sampling_event_init(void)
{
    ret_code_t err_code;

	/* ��ʼ��PPI */
    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

	/* ��ʼ����ʱ�� */
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;
    err_code = nrf_drv_timer_init(&m_timer, &timer_cfg, timer_handler);
    APP_ERROR_CHECK(err_code);

	/* ���ö�ʱ��ÿ��һ��ʱ�䷢���Ƚ��¼� */
    uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, SAADC_SAMPLE_TIME_INTERVAL);
    nrf_drv_timer_extended_compare(&m_timer,
                                   NRF_TIMER_CC_CHANNEL0,
                                   ticks,
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
                                   false);
	
	/* ʹ�ܶ�ʱ����ʱ�� */
    nrf_drv_timer_enable(&m_timer);

	/* ��ȡ��ʱ���Ƚ��¼���ַ */
    uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer,
                                                                                NRF_TIMER_CC_CHANNEL0);
	
	/* ��ȡSAADC�����¼���ַ */
    uint32_t saadc_sample_task_addr   = nrf_drv_saadc_sample_task_get();

	/* ���PPIͨ�� */
    err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel);
    APP_ERROR_CHECK(err_code);

	/* ����PPIͨ��������ʱ���Ƚ��¼�����ʱ����SAADC�������� */
    err_code = nrf_drv_ppi_channel_assign(m_ppi_channel,
                                          timer_compare_event_addr,
                                          saadc_sample_task_addr);
    APP_ERROR_CHECK(err_code);
}

void saadc_sample_start(void)
{
	/* ʹ��PPIͨ�� */
    ret_code_t err_code = nrf_drv_ppi_channel_enable(m_ppi_channel);
    APP_ERROR_CHECK(err_code);
	
#if (SAADC_SAMPLE_MODE == 0)
	m_saadc_sample_mode = 0;
#endif
}

int16_t saadc_sample_result_get(void)
{
	return m_sample_data;
}

void saadc_init(void)
{
    ret_code_t err_code;
	/* ����SAADC�������� */
    nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(SAADC_INPUT_AIN);

	/* SAADC��ʼ�� */
    err_code = nrf_drv_saadc_init(NULL, saadc_callback);
    APP_ERROR_CHECK(err_code);

	/* SAADCͨ����ʼ�� */
    err_code = nrf_drv_saadc_channel_init(0, &channel_config);
    APP_ERROR_CHECK(err_code);

	/* ע������ת����洢��ַ1 */
    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAADC_SAMPLE_IN_BUFFER);
    APP_ERROR_CHECK(err_code);

	/* ע������ת����洢��ַ2 */
    err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1], SAADC_SAMPLE_IN_BUFFER);
    APP_ERROR_CHECK(err_code);
	
	/* SAADC�����¼���ʼ�� */
	saadc_sampling_event_init();
	
	m_saadc_is_enable = 1;
}

void saadc_unint(void)
{
	if(m_saadc_is_enable == 0)
	{
		return;
	}
	
	/* ��ֹ���� */
	nrf_drv_saadc_abort();
	
	/* ʧ��PPIͨ�� */
	ret_code_t err_code = nrf_drv_ppi_channel_disable(m_ppi_channel);
	APP_ERROR_CHECK(err_code);
	
	/* �ͷ�PPIͨ�� */
	err_code = nrf_drv_ppi_channel_free(m_ppi_channel);
	APP_ERROR_CHECK(err_code);
	
	/* ʧ�ܶ�ʱ�� */
	nrf_drv_timer_disable(&m_timer);
	
	/* ��ʼ��PPIΪĬ������ */
    err_code = nrf_drv_ppi_uninit();
    APP_ERROR_CHECK(err_code);
	
	/* ��ʼ����ʱ��ΪĬ������ */
	nrf_drv_timer_uninit(&m_timer);
	
	/* ��ʼ��SAADCͨ��ΪĬ������ */
	err_code = nrf_drv_saadc_channel_uninit(0);
	APP_ERROR_CHECK(err_code);
	
	/* ��ʼ��SAADCΪĬ������ */
	nrf_drv_saadc_uninit();
	m_saadc_is_enable = 0;
}


























