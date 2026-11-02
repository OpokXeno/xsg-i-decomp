#ifndef RSSD_RSSDRING_H
#define RSSD_RSSDRING_H

/* Ring of fixed-size blocks: a 16-byte ring header, then capacity slots of a
 * 16-byte slot header followed by blockSize bytes of data. */
#define RING_HEADER_SIZE 16
#define RING_SLOT_HEADER_SIZE 16

typedef struct RingBuffer {
    signed char capacity;
    unsigned char count;
    signed char input;
    signed char output;
    int blockSize;
} RingBuffer;

/* Functions and data used by other translation units. */
extern RingBuffer *InitRingBuffer(int blockSize, int capacity);
extern void DisposeRingBuffer(RingBuffer *ring);
extern void *GetRingInBufferPtr(RingBuffer *ring);
extern void IncRingInBufferPtr(RingBuffer *ring, int size);
extern int InRingBuffer(RingBuffer *ring, const void *source, int size);
extern void *OutRingBuffer(RingBuffer *ring, int *size);
extern void ClearRingBuffer(RingBuffer *ring);

#endif
