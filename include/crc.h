#ifndef  __CRC_H__
#define  __CRC_H__


#define INIT  0xffffffff
#define XOROT 0xffffffff

unsigned char crc8(unsigned char uCRC8Value, const void * pbuf, unsigned int length);
unsigned int crc16(unsigned short uCRC16Value, const void *pbuf, unsigned int length);
unsigned int crc32(unsigned int uCRC32Value, const void *pbuf, unsigned int length);

#endif