#ifndef INCLUDE_MAIN_DATA_BUFFER_H
#define INCLUDE_MAIN_DATA_BUFFER_H

#include "shared.h"

typedef unsigned char DataBufferByte;

struct DataBuffer {
    DataBufferByte *base;
    DataBufferByte *position;
    DataBufferByte *limit;
    int length;
    DataBufferReader reader;
};

#endif /* INCLUDE_MAIN_DATA_BUFFER_H */
