#ifndef SSD_DMA_H
#define SSD_DMA_H

#include "ssd/ssd.h"

typedef void (*SsdDmaCallback)(int result, void *argument);

/* SsdDmaBuffer.operation (0 = free slot) */
#define SSD_DMA_OP_NONE 0
#define SSD_DMA_OP_WRITE 1
#define SSD_DMA_OP_READ 2

/* Slots in the DMA request pool, and how many SsdGetDmaQueueBuffer will hand out. */
#define SSD_DMA_BUFFER_COUNT 8
#define SSD_DMA_BUFFER_LIMIT 6

/* Result passed to a DMA callback when the transfer interrupt arrives. */
#define SSD_DMA_RESULT_DONE 2

struct SsdDmaBuffer {
    unsigned char operation;
    unsigned char unmodeled_01[3];
    unsigned int progress;
    void *memory;
    unsigned int spu_address;
    unsigned int size;
    SsdDmaCallback callback;
    void *callback_argument;
    /* The DMA interrupt and producer share the queue links. */
    SsdDmaBuffer *volatile next;
};

/* Functions and data used by other translation units. */
void SsdInitDmaManager(void);
void SsdSetPcmStreamCallback(SsdDmaCallback callback);
void SsdSetVagStreamCallback(SsdDmaCallback callback);
void SsdSetIrqCallback(SsdDmaCallback callback);
void SsdSpuDmaWrite(unsigned int spu_address, void *memory, unsigned int size,
                    SsdDmaCallback callback, void *argument);
void SsdSpuDmaRead(unsigned int spu_address, void *memory, unsigned int size,
                   SsdDmaCallback callback, void *argument);
int SsdSpuDmaCompleted(int wait);
int SsdWaveTransferInterruptProcess(int channel, void *argument);
int SsdStreamInterruptProcess(int channel, void *argument);
int SsdIrqInterruptProcess(int channel, void *argument);

#endif
