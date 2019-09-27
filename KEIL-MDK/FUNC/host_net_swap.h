#ifndef __HOST_NET_SWAP_H__
#define __HOST_NET_SWAP_H__


/* 主机到网络字节序 */
unsigned long int swap_htonl(unsigned long int host);
unsigned short int swap_htons(unsigned short int host);

/* 网络到主机字节序 */
unsigned long int swap_ntohl(unsigned long int net);
unsigned short int swap_ntohs(unsigned short int net);

/* 翻转数组 */
void swap_reverse(unsigned char *array, unsigned int size);

#endif






