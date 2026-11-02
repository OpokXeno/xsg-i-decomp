/*
 * TU-local declarations of main/tu230 (src/main/data_buffer.c).
 */

#ifndef SRC_MAIN_DATA_BUFFER_H
#define SRC_MAIN_DATA_BUFFER_H
#include "main/data_buffer.h"

typedef unsigned char DataBufferByte;

typedef unsigned int DataBufferWord;

typedef struct DataBuffer DataBuffer;

typedef DataBufferWord (*DataBufferReader)(DataBuffer *, int);

int DataBuffer_getBytes(DataBuffer *buffer, DataBufferByte *destination, int length);
DataBufferWord DataBuffer_LittleEndian_getUIntegerAt(DataBuffer *buffer, int length);
DataBufferWord DataBuffer_BigEndian_getUIntegerAt(DataBuffer *buffer, int length);

#endif /* SRC_MAIN_DATA_BUFFER_H */
