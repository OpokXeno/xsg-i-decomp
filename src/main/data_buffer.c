#include "common.h"
#include "shared.h"
#include "data_buffer.h"

INCLUDE_ASM("asm/main/nonmatchings/data_buffer", DataBuffer_init);

INCLUDE_ASM("asm/main/nonmatchings/data_buffer", DataBuffer_getPos);

INCLUDE_ASM("asm/main/nonmatchings/data_buffer", DataBuffer_setPos);

INCLUDE_ASM("asm/main/nonmatchings/data_buffer", DataBuffer_seek);

int DataBuffer_getBytes(DataBuffer *buffer, DataBufferByte *destination, int length)
{
    DataBufferByte *position = buffer->position;
    DataBufferByte *end = position + length;

    if (buffer->limit < end)
        return -1;
    if (position < end) {
        do {
            *destination++ = *position++;
        } while (position < end);
    }
    buffer->position = end;
    return length;
}

DataBufferByte DataBuffer_getUByteAt(DataBuffer *buffer)
{
    DataBufferByte *position = buffer->position;
    DataBufferByte value = *position;
    buffer->position = position + 1;
    return value;
}

unsigned short DataBuffer_getUShortAt(DataBuffer *buffer)
{
    return (unsigned short)buffer->reader(buffer, 2);
}

DataBufferWord DataBuffer_getUIntAt(DataBuffer *buffer)
{
    return buffer->reader(buffer, 4);
}

DataBufferWord DataBuffer_LittleEndian_getUIntegerAt(DataBuffer *buffer, int length)
{
    DataBufferWord value = buffer->position[length - 1];
    int index = length - 2;

    while (index >= 0) {
        value = (value << 8) + buffer->position[index];
        index--;
    }
    buffer->position += length;
    return value;
}

DataBufferWord DataBuffer_BigEndian_getUIntegerAt(DataBuffer *buffer, int length)
{
    DataBufferWord value = buffer->position[0];
    int index = 1;

    while (index < length) {
        value = (value << 8) + buffer->position[index];
        index++;
    }
    buffer->position += length;
    return value;
}
