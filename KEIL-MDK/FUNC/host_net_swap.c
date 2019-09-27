#include "host_net_swap.h"


// �����ʹ�С�˻���
#define BIG_LITTLE_SWAP16(N)  (((( unsigned short)(N) & 0xff00) >> 8) | \
							   ((( unsigned short)(N) & 0x00ff) << 8))

 // �����ʹ�С�˻���
#define BIG_LITTLE_SWAP32(N)  ((((unsigned int)(N) & 0xff000000) >> 24) | \
                               (((unsigned int)(N) & 0x00ff0000) >> 8) | \
                               (((unsigned int)(N) & 0x0000ff00) << 8) | \
                               (((unsigned int)(N) & 0x000000ff) << 24))

 

/* ������˷���1��С�˷���0 */
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
 * ģ��htonl�����������ֽ���ת�����ֽ���
 * ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���
 * ������ΪС�ˣ�ת���ɴ���ٷ���
 */
unsigned long int swap_htonl(unsigned long int host)
{
    return is_cpu_big_endian() ? host : BIG_LITTLE_SWAP32(host);
}

/**
 * ģ��ntohl�����������ֽ���ת�����ֽ���
 * ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���
 * ������ΪС�ˣ���������ת����С���ٷ���
 */
unsigned long int swap_ntohl(unsigned long int net)
{
    return is_cpu_big_endian() ? net : BIG_LITTLE_SWAP32(net);
}

/**
 * ģ��htons�����������ֽ���ת�����ֽ���
 * ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���
 * ������ΪС�ˣ�ת���ɴ���ٷ���

 */
unsigned short int swap_htons(unsigned short int host)
{
    return is_cpu_big_endian() ? host : BIG_LITTLE_SWAP16(host);
}

/**
 * ģ��ntohs�����������ֽ���ת�����ֽ���
 * ������Ϊ��ˣ��������ֽ���ͬ��ֱ�ӷ���
 * ������ΪС�ˣ���������ת����С���ٷ���
 */
unsigned short int swap_ntohs(unsigned short int net)
{
    return is_cpu_big_endian() ? net : BIG_LITTLE_SWAP16(net);
}

/**
 * ��ת����
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


