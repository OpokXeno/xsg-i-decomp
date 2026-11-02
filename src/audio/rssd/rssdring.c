#include "rssd/rssd.h"
#include "rssd/rssdring.h"

/* Only the used prefix is known; each slot has a 16-byte header. */
typedef struct RingSlot {
    unsigned int header;
    int size;
} RingSlot;

void CopyMemoryRingBuffer(void *destination, const void *source, int size);

RingBuffer *InitRingBuffer(int blockSize, int capacity)
{
    RingBuffer *ring = AllocSysMemory(SMEM_Low, capacity * (blockSize + RING_SLOT_HEADER_SIZE) + RING_HEADER_SIZE, 0);
    if (!ring) {
        printf("Rssd lack of memory  ring buffer !\n");
        return 0;
    }
    ring->capacity = capacity;
    ring->count = 0;
    ring->input = 0;
    ring->output = 0;
    ring->blockSize = blockSize;
    return ring;
}

void DisposeRingBuffer(RingBuffer *ring)
{
    FreeSysMemory(ring);
}

void *GetRingInBufferPtr(RingBuffer *ring)
{
    unsigned int offset;
    unsigned int baseAddress = (unsigned int)ring;
    if (ring->count == ring->capacity)
        return 0;
    offset = ring->input * (ring->blockSize + RING_SLOT_HEADER_SIZE);
    return (void *)(offset + baseAddress + RING_HEADER_SIZE + RING_SLOT_HEADER_SIZE);
}

void IncRingInBufferPtr(RingBuffer *ring, int size)
{
    int input = ring->input;
    unsigned int offset = input * (ring->blockSize + RING_SLOT_HEADER_SIZE);
    unsigned int baseAddress = (unsigned int)ring;
    RingSlot *slot = (RingSlot *)(offset + baseAddress + RING_HEADER_SIZE);
    slot->size = size;
    if (++input >= ring->capacity)
        input = 0;
    ring->input = input;
    ring->count++;
}

int InRingBuffer(RingBuffer *ring, const void *source, int size)
{
    RingSlot *slot;
    int input;
    if (ring->count == ring->capacity)
        return 0;
    input = ring->input;
    slot = (RingSlot *)((char *)ring + RING_HEADER_SIZE + input * (ring->blockSize + RING_SLOT_HEADER_SIZE));
    CopyMemoryRingBuffer((char *)slot + RING_SLOT_HEADER_SIZE, source, size);
    slot->size = size;
    if (++input >= ring->capacity)
        input = 0;
    ring->input = input;
    ring->count++;
    return 1;
}

void *OutRingBuffer(RingBuffer *ring, int *size)
{
    RingSlot *slot;
    int output;
    void *data;
    if (!ring->count)
        return 0;
    output = ring->output;
    slot = (RingSlot *)((char *)ring + RING_HEADER_SIZE + output * (ring->blockSize + RING_SLOT_HEADER_SIZE));
    *size = slot->size;
    data = (char *)slot + RING_SLOT_HEADER_SIZE;
    if (++output >= ring->capacity)
        output = 0;
    ring->output = output;
    ring->count--;
    return data;
}

void ClearRingBuffer(RingBuffer *ring)
{
    ring->count = 0;
    ring->input = 0;
    ring->output = 0;
}

void CopyMemoryRingBuffer(void *destination, const void *source, int size)
{
    unsigned int *dst = destination;
    const unsigned int *src = source;
    while (size > 0) {
        unsigned int first = src[0];
        unsigned int second = src[1];
        unsigned int third = src[2];
        unsigned int fourth = src[3];
        size -= 16;
        src += 4;
        dst[0] = first;
        dst[1] = second;
        dst[2] = third;
        dst[3] = fourth;
        dst += 4;
    }
}
