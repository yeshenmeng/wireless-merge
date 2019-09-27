#ifndef __EASY_FIFO_H__
#define __EASY_FIFO_H__
#include "main.h"
#include "stdbool.h"


#define EASY_FIFO_LENGTH 		2800 //4 * 7 * 100

bool easy_fifo_write(unsigned char* buffer, uint16_t wr_len );
bool easy_fifo_read(unsigned char* buffer, uint16_t rd_len );

#endif








