typedef struct RingBuffer {
    signed char capacity;
    unsigned char count;
    signed char input;
    signed char output;
    int blockSize;
} RingBuffer;

/* Only the used prefix is known; each slot has a 16-byte header. */
typedef struct RingSlot {
    unsigned int header;
    int size;
} RingSlot;

extern void *AllocSysMemory(int mode, int size, void *address);
extern int FreeSysMemory(void *address);
extern int printf(const char *format, ...);
void CopyMemoryRingBuffer(void *destination, const void *source, int size);

RingBuffer *InitRingBuffer(int blockSize, int capacity)
{
    RingBuffer *ring = AllocSysMemory(0, capacity * (blockSize + 16) + 16, 0);
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
    offset = ring->input * (ring->blockSize + 16);
    return (void *)(offset + baseAddress + 32);
}

void IncRingInBufferPtr(RingBuffer *ring, int size)
{
    int input = ring->input;
    unsigned int offset = input * (ring->blockSize + 16);
    unsigned int baseAddress = (unsigned int)ring;
    RingSlot *slot = (RingSlot *)(offset + baseAddress + 16);
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
    slot = (RingSlot *)((char *)ring + 16 + input * (ring->blockSize + 16));
    CopyMemoryRingBuffer((char *)slot + 16, source, size);
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
    slot = (RingSlot *)((char *)ring + 16 + output * (ring->blockSize + 16));
    *size = slot->size;
    data = (char *)slot + 16;
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
