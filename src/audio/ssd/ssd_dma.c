#include "ssd/ssd_dma.h"
#include "ssd/ssd_err.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_mem.h"

void SsdResetDmaManager(void);
void SsdDmaStartBuffer(SsdGlobalWork *work);
SsdDmaBuffer *SsdGetDmaQueueBuffer(SsdGlobalWork *work);
void SsdDmaStartTransfer(SsdGlobalWork *work, unsigned int spu_address,
                         void *memory, unsigned int size,
                         SsdDmaCallback callback, void *argument, int operation);

void SsdInitDmaManager(void)
{
    SsdWork.dma_buffers = SsdNewPtr(SSD_DMA_BUFFER_COUNT * sizeof(SsdDmaBuffer));
    SsdResetDmaManager();
}
void SsdSetPcmStreamCallback(SsdDmaCallback callback)
{
    SsdWork.pcm_callback = callback;
}
void SsdSetVagStreamCallback(SsdDmaCallback callback)
{
    SsdWork.vag_callback = callback;
}
void SsdSetIrqCallback(SsdDmaCallback callback)
{
    SsdWork.irq_callback = callback;
}
void SsdSpuDmaWrite(unsigned int spu_address, void *memory, unsigned int size,
                    SsdDmaCallback callback, void *argument)
{
    SsdDmaStartTransfer(&SsdWork, spu_address, memory, size, callback, argument, SSD_DMA_OP_WRITE);
}
void SsdSpuDmaRead(unsigned int spu_address, void *memory, unsigned int size,
                   SsdDmaCallback callback, void *argument)
{
    SsdDmaStartTransfer(&SsdWork, spu_address, memory, size, callback, argument, SSD_DMA_OP_READ);
}
/*
 * Queues a transfer of size bytes between memory and the SPU. The transfer starts
 * at once when the queue was empty; otherwise it is appended to the queue.
 */
void SsdDmaStartTransfer(SsdGlobalWork *work, unsigned int spu_address,
                         void *memory, unsigned int size,
                         SsdDmaCallback callback, void *argument, int operation)
{
    unsigned short flags = work->flags;
    SsdDmaBuffer *buffer = SsdGetDmaQueueBuffer(work);
    SsdDmaBuffer *tail;
    if (!(flags & (SSD_FLAG_IN_TRANSFER_INTR | SSD_FLAG_IN_IRQ_INTR))) {
        while (!buffer)
            buffer = SsdGetDmaQueueBuffer(work);
        CpuDisableIntr();
    }
    if (!buffer) {
        SsdSeqErrorProc(SSD_ERROR_DMA_QUEUE_FULL, (int)memory);
        return;
    }
    buffer->operation = operation & 15;
    buffer->progress = 0;
    buffer->memory = memory;
    buffer->spu_address = spu_address;
    buffer->size = size;
    buffer->callback = callback;
    buffer->next = 0;
    if (!work->dma_queue) {
        work->dma_queue = buffer;
        SsdDmaStartBuffer(work);
    } else {
        tail = work->dma_queue;
        while (tail->next)
            tail = tail->next;
        tail->next = buffer;
    }
    if (!(flags & (SSD_FLAG_IN_TRANSFER_INTR | SSD_FLAG_IN_IRQ_INTR)))
        CpuEnableIntr();
}
/* Returns the operation of the queued transfer, or 0 when the queue is empty. Bit 0 of wait makes it poll until DMA is not busy. */
int SsdSpuDmaCompleted(int wait)
{
    if (wait & 1) {
        /* The transfer interrupt clears SSD_FLAG_DMA_BUSY (bit 1), which ends this poll. */
        volatile unsigned short *status = (volatile unsigned short *)&SsdWork.flags;
        while (*status & SSD_FLAG_DMA_BUSY)
            ;
    }
    if (SsdWork.dma_queue != 0) {
        return SsdWork.dma_queue->operation;
    }
    return 0;
}
/* Returns the first DMA buffer that is not in use, or 0 when all are in use. */
SsdDmaBuffer *SsdGetDmaQueueBuffer(SsdGlobalWork *work)
{
    SsdDmaBuffer *buffer = work->dma_buffers;
    int i;
    for (i = 0; i < SSD_DMA_BUFFER_LIMIT; i++, buffer++) {
        if (!buffer->operation)
            return buffer;
    }
    return 0;
}
/* Interrupt handler for a finished wave transfer: runs its callback and starts the next queued transfer. */
int SsdWaveTransferInterruptProcess(int channel, void *argument)
{
    SsdGlobalWork *work = argument;
    SsdDmaBuffer *buffer;
    work->wave_interrupt_count++;
    work->flags |= SSD_FLAG_IN_TRANSFER_INTR;
    buffer = work->dma_queue;
    if (buffer) {
        if (buffer->callback)
            buffer->callback(SSD_DMA_RESULT_DONE,buffer->callback_argument);
        buffer->operation = 0;
        work->flags &= ~SSD_FLAG_DMA_BUSY;
        work->dma_queue = buffer->next;
        if (work->dma_queue)
            SsdDmaStartBuffer(work);
    }
    work->flags &= ~SSD_FLAG_IN_TRANSFER_INTR;
    return 0;
}
/* Interrupt handler for the PCM stream: calls the PCM callback with the channel. */
int SsdStreamInterruptProcess(int channel, void *argument)
{
    SsdGlobalWork *work = argument;
    work->stream_interrupt_count++;
    work->flags |= SSD_FLAG_IN_TRANSFER_INTR;
    if (work->pcm_callback)
        work->pcm_callback(channel, work->pcm_argument);
    work->flags &= ~SSD_FLAG_IN_TRANSFER_INTR;
    return 0;
}
/* Starts the transfer of the head of the DMA queue. */
void SsdDmaStartBuffer(SsdGlobalWork *work)
{
    SsdDmaBuffer *buffer = work->dma_queue;
    work->flags |= SSD_FLAG_DMA_BUSY;
    switch (buffer->operation) {
    case SSD_DMA_OP_NONE:
        break;
    case SSD_DMA_OP_WRITE:
        sceSdVoiceTrans(0, SD_TRANS_MODE_WRITE, buffer->memory, buffer->spu_address, buffer->size);
        break;
    case SSD_DMA_OP_READ:
        sceSdVoiceTrans(0, SD_TRANS_MODE_READ,buffer->memory, buffer->spu_address, buffer->size);
        break;
    }
}
/* Interrupt handler for the IRQ: calls the IRQ callback with the channel minus one. */
int SsdIrqInterruptProcess(int channel, void *argument)
{
    SsdGlobalWork *work = argument;
    channel--;
    work->flags |= SSD_FLAG_IN_IRQ_INTR;
    work->irq_interrupt_count++;
    if (work->irq_callback)
        work->irq_callback(channel, work->irq_argument);
    work->flags &= ~SSD_FLAG_IN_IRQ_INTR;
    return 0;
}
/* Clears every DMA buffer, empties the queue and clears the busy flag. */
void SsdResetDmaManager(void)
{
    SsdDmaBuffer *buffer = SsdWork.dma_buffers;
    int i;
    for (i = SSD_DMA_BUFFER_COUNT - 1; i >= 0; i--, buffer++)
        buffer->operation = 0;
    SsdWork.dma_queue = 0;
    SsdWork.flags &= ~SSD_FLAG_DMA_BUSY;
}
