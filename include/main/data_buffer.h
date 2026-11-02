#ifndef INCLUDE_MAIN_DATA_BUFFER_H
#define INCLUDE_MAIN_DATA_BUFFER_H

#include "shared.h"

struct DataBuffer {
    DataBufferByte *base;
    DataBufferByte *position;
    DataBufferByte *limit;
    int length;
    DataBufferReader reader;
};

/* Functions and data used by other translation units. */
DataBufferByte DataBuffer_getUByteAt(DataBuffer *buffer);
unsigned short DataBuffer_getUShortAt(DataBuffer *buffer);
DataBufferWord DataBuffer_getUIntAt(DataBuffer *buffer);
/*
 * Initializes buffer to read length bytes starting at base, little-endian
 * unless big_endian is set. Defined at main 0x002f5f70
 * (src/main/data_buffer.c).
 */
void DataBuffer_init(DataBuffer *buffer, DataBufferByte *base, int length, int big_endian);
void DataBuffer_seek(DataBuffer *buffer, int offset);

#endif /* INCLUDE_MAIN_DATA_BUFFER_H */
