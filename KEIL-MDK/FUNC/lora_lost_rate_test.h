#ifndef __LORA_LOST_RATE_TEST_H__
#define __LORA_LOST_RATE_TEST_H__
#include "main.h"


typedef enum {
	LLRT_STOP,
	LLRT_ACTIVE,
	LLRT_RUN,
}llrt_status_t;

typedef enum {
	LLRT_CTRL_NULL,
	LLRT_CTRL_START,
	LLRT_CTRL_START_RETRANSMISSION,
	LLRT_CTRL_STOP,
}llrt_ctrl_t;

typedef struct {
	uint8_t payload_len;
	llrt_ctrl_t comm_ctrl;
	llrt_ctrl_t dev_comm_ctrl;
	uint32_t counting_nums;
	uint32_t timer_time;
	uint32_t comm_interval;
	float lost_rate;
	float test_progress;
}llrt_mod_t;

void llrt_init(void);
void llrt_start(void);
void llrt_stop(void);
llrt_status_t llrt_status_get(void);
llrt_ctrl_t llrt_comm_ctrl_status(void);
void llrt_status_set(llrt_status_t status);
void llrt_tx_add(void);
void llrt_rx_add(void);
void llrt_lost_rate_calc(void);
llrt_mod_t* llrt_get_handle(void);
void llrt_param_set(uint8_t* param, uint8_t* value, uint8_t len);


#endif






