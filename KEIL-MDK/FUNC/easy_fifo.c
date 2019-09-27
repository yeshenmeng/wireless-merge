#include "easy_fifo.h"
#include "string.h"


static uint16_t front = 0;
static uint16_t rear = 0;
static unsigned char easy_fifo_buffer[ EASY_FIFO_LENGTH ];

bool easy_fifo_write(unsigned char* buffer, uint16_t wr_len )
{
	if (wr_len > EASY_FIFO_LENGTH -1)
	{
		return false;
	}
	
	uint16_t empty_len;	
	uint16_t tmp_addr;	
	uint16_t tmp_len;
	empty_len = ( front + EASY_FIFO_LENGTH - ( rear + 1) ) % EASY_FIFO_LENGTH;
	
	if ( empty_len >= wr_len )	
	{ 	 
		tmp_addr = ( rear + wr_len) % EASY_FIFO_LENGTH;		 
		
		if (tmp_addr <= rear)	 
		{
			tmp_len = wr_len - tmp_addr;			  
			memcpy(&easy_fifo_buffer[ rear ], buffer, tmp_len);		  
			memcpy(&easy_fifo_buffer[0], buffer + tmp_len, tmp_addr);    		 
		}
		else
		{
			memcpy(&easy_fifo_buffer[ rear ], buffer, wr_len);		
		}
		
		rear = tmp_addr;
		return true;
	}
	return false;
}

bool easy_fifo_read(unsigned char* buffer, uint16_t rd_len )
{
	uint16_t valid_len;
	uint16_t tmp_addr;
	uint16_t tmp_len;
	valid_len = ( rear + EASY_FIFO_LENGTH - front ) % EASY_FIFO_LENGTH;
	
	if ( valid_len >= rd_len)
	{
		tmp_addr = ( front + rd_len ) % EASY_FIFO_LENGTH;
		
		if ( tmp_addr <= front )
		{
			tmp_len = rd_len - tmp_addr;			 
			memcpy( buffer, &easy_fifo_buffer[ front ], tmp_len);			 
			memcpy( buffer + tmp_len, &easy_fifo_buffer[ 0 ], tmp_addr);              
		}
		else
		{
			memcpy( buffer, &easy_fifo_buffer[ front ], rd_len );         
		}
		
		front = tmp_addr;
		return true;
	}
	return false;
}














