#include "uart_svc.h"
#include "nrf_uart.h"
#include "app_uart.h"
#include "app_timer.h"


#define UART_TX_BUF_SIZE 256       //���ڷ��ͻ����С���ֽ�����
#define UART_RX_BUF_SIZE 256       //���ڽ��ջ����С���ֽ�����

#include "inclinometer.h"
#include "string.h"
extern inclinometer_obj_t* m_inclinometer;
static uint8_t uart_rec_flag = 0;
uint8_t ble_rec_flag = 0;
uint16_t DebugSize = 0;
uint8_t DebugBuf[255];
static uint16_t UartRxBufSize = 0;
static uint8_t UartRxBuf[255];
static uint8_t cmd_flag = 0;


void uart_send(uint8_t* data, uint16_t size)
{
	for(int i=0; i<size; i++)
	{
		while(app_uart_put(data[i]) != NRF_SUCCESS);
	}
}

void uart_receive(uint8_t* buf, uint16_t size)
{
	for(int i=0; i<size; i++)
	{
		while(app_uart_get(&buf[i]) != NRF_SUCCESS);
	}
}

void ble_write_handler(uint8_t* p_data, uint16_t len)
{
	ble_rec_flag = 1;
	DebugSize = len;
	memcpy(DebugBuf, p_data, len);
}

void lora_write_handler(uint8_t* p_data, uint16_t len)
{
	ble_rec_flag = 2;
	DebugSize = len;
	memcpy(DebugBuf, p_data, len);	
}

APP_TIMER_DEF(uart_id);
void SWT_UartCallback(void* param)
{
	cmd_flag = 1;
	return;
}
void uart_timer_init(void)
{
	app_timer_create(&uart_id,
					 APP_TIMER_MODE_SINGLE_SHOT,
					 SWT_UartCallback);
}

typedef struct {
	char* power;
	char* bw;
	char* sf;
	char* ble;
}Cmd_t;

Cmd_t debug_cmd = {
	.power = "power",
	.bw = "bw",
	.sf = "sf",
	.ble = "ble"
};

int stringFind(const char *string, const char *dest) {
    int i = 0;
    int j = 0;
    while (string[i] != '\0') {
        if (string[i] != dest[0]) {
            i ++;
            continue;
        }
        j = 0;
        while (string[i+j] != '\0' && dest[j] != '\0') {
            if (string[i+j] != dest[j]) {
                break;
            }
            j ++;
        }
        if (dest[j] == '\0') return i;
        i ++;
    }
    return -1;
}

void bytes_to_chars(uint8_t* des, char* src, uint16_t size)
{
	for(int i=0; i<size; i++)
	{
		src[i] = des[i];
	}
}

void uart_cmd_test(void)
{
	if(cmd_flag == 1)
	{
		cmd_flag = 0;
		char cmd[10] = "";
		uint8_t uart_cmd_flag = 0;
		bytes_to_chars(UartRxBuf, cmd, UartRxBufSize);

		if(stringFind(cmd, debug_cmd.power) != -1)
		{
			uart_cmd_flag = 1;
		}
		else if(stringFind(cmd, debug_cmd.bw) != -1)
		{
			uart_cmd_flag = 1;
		}
		else if(stringFind(cmd, debug_cmd.bw) != -1)
		{
			uart_cmd_flag = 1;
		}
		else if(stringFind(cmd, debug_cmd.ble) != -1)
		{
			uart_cmd_flag = 1;
		}
		
		if(uart_cmd_flag == 1)
		{
			printf("send success!");
		}
		else
		{
			printf("send fail!");
		}
		
		memset(UartRxBuf, 0X00, UartRxBufSize);
		UartRxBufSize = 0;
	}
}

void uart_error_handle(app_uart_evt_t * p_event)
{
	//ͨѶ�����¼�
	if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
	{
		APP_ERROR_HANDLER(p_event->data.error_communication);
	}
	//FIFO�����¼�
	else if (p_event->evt_type == APP_UART_FIFO_ERROR)
	{
		APP_ERROR_HANDLER(p_event->data.error_code);
	}
	//���ڽ����¼�
	else if (p_event->evt_type == APP_UART_DATA_READY)
	{
		app_uart_get(&UartRxBuf[UartRxBufSize]);
		UartRxBufSize++;
		uart_rec_flag = 1;
		UartRxBufSize = 0;
//		app_timer_stop(uart_id);
//		app_timer_start(uart_id, APP_TIMER_TICKS(20), NULL);
	}
	//���ڷ�������¼�
	else if (p_event->evt_type == APP_UART_TX_EMPTY)
	{
		;
	}
}

__weak void uart_collapse_test(void)
{
	
}

void uart_test(void)
{
//	if(uart_rec_flag == 1)
//	{
//		uart_rec_flag = 0;
//		printf("X:%f Y:%f T:%f\n",m_inclinometer->Data.XAngle,
//								  m_inclinometer->Data.YAngle,
//								  m_inclinometer->Data.Temperature);
//	}
	
//	if(ble_rec_flag == 1)
//	{
//		ble_rec_flag = 0;
//		printf("ble config param:");
//		for(int i=0; i<DebugSize; i++)
//		{
//			 printf("0x%02x ", DebugBuf[i]);
//		}
//		printf("\n");
//	}
//	else if(ble_rec_flag == 2)
//	{
//		ble_rec_flag = 0;
//		printf("lora config param:");
//		for(int i=0; i<DebugSize; i++)
//		{
//			 printf("0x%02x ", DebugBuf[i]);
//		}
//		printf("\n");
//	}
	
//	uart_cmd_test();

	if(uart_rec_flag == 1)
	{
		uart_rec_flag = 0;
		printf("lora config param:");
	}
//	if(uart_rec_flag == 1)
//	{
//		uart_rec_flag = 0;
//		uart_collapse_test();
//	}
}

void uart_run(void)
{
	uart_test();
}

//��������
void uart_init(void)
{
	uint32_t err_code;
	
	//���崮��ͨѶ�������ýṹ�岢��ʼ��
  const app_uart_comm_params_t comm_params =
  {
    UART_RX_PIN,//����uart��������
    UART_TX_PIN,//����uart��������
    UART_RTS_PIN,//����uart RTS���ţ����عرպ���Ȼ������RTS��CTS���ţ����������������ԣ������������������ţ����������Կ���ΪIOʹ��
    UART_CTS_PIN,//����uart CTS����
    APP_UART_FLOW_CONTROL_DISABLED,//�ر�uartӲ������
    false,//��ֹ��ż����
    NRF_UART_BAUDRATE_115200//uart����������Ϊ115200bps
  };
	
  //��ʼ�����ڣ�ע�ᴮ���¼��ص�����
	//����������һ�ν��գ�����������Ų��ӻᱨ��
  APP_UART_FIFO_INIT(&comm_params,
                     UART_RX_BUF_SIZE,
                     UART_TX_BUF_SIZE,
                     uart_error_handle,
                     APP_IRQ_PRIORITY_LOWEST,
                     err_code);

  APP_ERROR_CHECK(err_code);
	
	uart_timer_init();
}

















