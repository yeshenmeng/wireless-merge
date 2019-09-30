#include "uart_svc.h"
#include "nrf_uart.h"
#include "app_uart.h"
#include "app_timer.h"


#define UART_TX_BUF_SIZE 256       //串口发送缓存大小（字节数）
#define UART_RX_BUF_SIZE 256       //串口接收缓存大小（字节数）

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
	//通讯错误事件
	if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
	{
		APP_ERROR_HANDLER(p_event->data.error_communication);
	}
	//FIFO错误事件
	else if (p_event->evt_type == APP_UART_FIFO_ERROR)
	{
		APP_ERROR_HANDLER(p_event->data.error_code);
	}
	//串口接收事件
	else if (p_event->evt_type == APP_UART_DATA_READY)
	{
		app_uart_get(&UartRxBuf[UartRxBufSize]);
		UartRxBufSize++;
		uart_rec_flag = 1;
		UartRxBufSize = 0;
//		app_timer_stop(uart_id);
//		app_timer_start(uart_id, APP_TIMER_TICKS(20), NULL);
	}
	//串口发送完成事件
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

//串口配置
void uart_init(void)
{
	uint32_t err_code;
	
	//定义串口通讯参数配置结构体并初始化
  const app_uart_comm_params_t comm_params =
  {
    UART_RX_PIN,//定义uart接收引脚
    UART_TX_PIN,//定义uart发送引脚
    UART_RTS_PIN,//定义uart RTS引脚，流控关闭后虽然定义了RTS和CTS引脚，但是驱动程序会忽略，不会配置这两个引脚，两个引脚仍可作为IO使用
    UART_CTS_PIN,//定义uart CTS引脚
    APP_UART_FLOW_CONTROL_DISABLED,//关闭uart硬件流控
    false,//禁止奇偶检验
    NRF_UART_BAUDRATE_115200//uart波特率设置为115200bps
  };
	
  //初始化串口，注册串口事件回调函数
	//函数会启动一次接收，如果接收引脚不接会报错
  APP_UART_FIFO_INIT(&comm_params,
                     UART_RX_BUF_SIZE,
                     UART_TX_BUF_SIZE,
                     uart_error_handle,
                     APP_IRQ_PRIORITY_LOWEST,
                     err_code);

  APP_ERROR_CHECK(err_code);
	
	uart_timer_init();
}

















