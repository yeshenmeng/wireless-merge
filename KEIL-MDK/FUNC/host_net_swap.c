#include "host_net_swap.h"


// 短整型大小端互换
#define BIG_LITTLE_SWAP16(N)  (((( unsigned short)(N) & 0xff00) >> 8) | \
							   ((( unsigned short)(N) & 0x00ff) << 8))

 // 长整型大小端互换
#define BIG_LITTLE_SWAP32(N)  ((((unsigned int)(N) & 0xff000000) >> 24) | \
                               (((unsigned int)(N) & 0x00ff0000) >> 8) | \
                               (((unsigned int)(N) & 0x0000ff00) << 8) | \
                               (((unsigned int)(N) & 0x000000ff) << 24))

 

/* 本机大端返回1，小端返回0 */
int is_cpu_big_endian(void)
{
     union{
        unsigned long int i;
        unsigned char s[4];
     }c;

     c.i = 0x12345678;
     return (0x12 == c.s[0]);
}

/**
 * 模拟htonl函数，本机字节序转网络字节序
 * 若本机为大端，与网络字节序同，直接返回
 * 若本机为小端，转换成大端再返回
 */
unsigned long int swap_htonl(unsigned long int host)
{
    return is_cpu_big_endian() ? host : BIG_LITTLE_SWAP32(host);
}

/**
 * 模拟ntohl函数，网络字节序转本机字节序
 * 若本机为大端，与网络字节序同，直接返回
 * 若本机为小端，网络数据转换成小端再返回
 */
unsigned long int swap_ntohl(unsigned long int net)
{
    return is_cpu_big_endian() ? net : BIG_LITTLE_SWAP32(net);
}

/**
 * 模拟htons函数，本机字节序转网络字节序
 * 若本机为大端，与网络字节序同，直接返回
 * 若本机为小端，转换成大端再返回

 */
unsigned short int swap_htons(unsigned short int host)
{
    return is_cpu_big_endian() ? host : BIG_LITTLE_SWAP16(host);
}

/**
 * 模拟ntohs函数，网络字节序转本机字节序
 * 若本机为大端，与网络字节序同，直接返回
 * 若本机为小端，网络数据转换成小端再返回
 */
unsigned short int swap_ntohs(unsigned short int net)
{
    return is_cpu_big_endian() ? net : BIG_LITTLE_SWAP16(net);
}

/**
 * 翻转数组
 */
void swap_reverse(unsigned char *array, unsigned int size)
{
	unsigned int i;
    unsigned char tmp;
	unsigned int half_size = size / 2;
		
    for(i = 0 ; i < half_size; i++)
    {
		tmp = array[i]; 
		array[i] = array[size-1-i]; 
		array[size-1-i] = tmp; 
    }
}


