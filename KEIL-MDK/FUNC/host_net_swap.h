#ifndef __HOST_NET_SWAP_H__
#define __HOST_NET_SWAP_H__


/* �����������ֽ��� */
unsigned long int swap_htonl(unsigned long int host);
unsigned short int swap_htons(unsigned short int host);

/* ���絽�����ֽ��� */
unsigned long int swap_ntohl(unsigned long int net);
unsigned short int swap_ntohs(unsigned short int net);

/* ��ת���� */
void swap_reverse(unsigned char *array, unsigned int size);

#endif






