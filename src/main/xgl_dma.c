#include "common.h"
#include "shared.h"

/*
 * The DMA channel table and the MFIFO state are provided by the scaffold data
 * for this TU.  Only the register slots touched by these routines are named.
 */
typedef struct XglDmaChannel {
    volatile u32 control;
    u32 reserved_04[3];
    volatile u32 memory_address;
    u32 reserved_14[3];
    volatile u32 quadword_count;
    u32 reserved_24[3];
    volatile u32 tag_address;
} XglDmaChannel;

typedef struct XglDmaBuffer {
    u32 *start;
    u32 *current;
} XglDmaBuffer;

extern XglDmaChannel *tbl[10];
extern XglDmaChannel *mfifo_drain;

extern void FlushCache(int mode);
extern int sceGsSyncPath(int mode, int timeout);

/* EE DMAC global registers and the SPR-from channel registers. */
#define DMAC_CTRL      ((volatile u32 *)0x1000E000)
#define DMAC_STAT      ((volatile u32 *)0x1000E010)
#define DMAC_RBSR      ((volatile u32 *)0x1000E040)
#define DMAC_RBOR      ((volatile u32 *)0x1000E050)
#define D8_CHCR        ((volatile u32 *)0x1000D000)
#define D8_MADR        ((volatile u32 *)0x1000D010)
#define D8_QWC         ((volatile u32 *)0x1000D020)
#define D8_SADR        ((volatile u32 *)0x1000D080)

void xglDmaDirectSrcChain(u32 channel, u32 address);

void xglDmaDirectNormal(u32 channel, u32 address, u32 count)
{
    XglDmaChannel *dma;

    if (channel >= 3U)
        return;
    dma = tbl[channel];
    while (dma->control & 0x100U)
        ;
    dma->quadword_count = count;
    if ((address >> 16) != 0x7000U)
        dma->memory_address = address;
    else
        dma->memory_address = (address & 0x3fffU) + 0x80000000U;
    *DMAC_STAT = 1U << channel;
    dma->control = 0x141;
}

void xglDmaDirectSrcChain(u32 channel, u32 address)
{
    XglDmaChannel *dma;

    if (channel >= 3U)
        return;
    dma = tbl[channel];
    while (dma->control & 0x100U)
        ;
    dma->quadword_count = 0;
    if ((address >> 16) != 0x7000U)
        dma->tag_address = address;
    else
        dma->tag_address = (address & 0x3fffU) + 0x80000000U;
    *DMAC_STAT = 1U << channel;
    dma->control = 0x145;
}

void xglDmaBufferReset(XglDmaBuffer *buffer, u32 *address)
{
    buffer->start = address;
    buffer->current = address;
}

u32 *xglDmaBufferCnt(XglDmaBuffer *buffer, const u32 *source, u32 count)
{
    u32 *current = buffer->current;
    u32 index;

    current[0] = 0x10000000U + count;
    current[1] = 0;
    current[2] = 0;
    current[3] = 0;
    current += 4;
    for (index = 0; index < count * 4; index++)
        *current++ = *source++;
    buffer->current = current;
    return current;
}

u32 *xglDmaBufferRef(XglDmaBuffer *buffer, u32 address, u32 count)
{
    u32 *current = buffer->current;

    current[0] = 0x30000000U + count;
    current[1] = address;
    current[2] = 0;
    current[3] = 0;
    current += 4;
    buffer->current = current;
    return current;
}

u32 *xglDmaBufferCall(XglDmaBuffer *buffer, u32 address)
{
    u32 *current = buffer->current;

    current[0] = 0x50000000U;
    current[1] = address;
    current[2] = 0;
    current[3] = 0;
    current += 4;
    buffer->current = current;
    return current;
}

void xglDmaBufferRequest(XglDmaBuffer *buffer, u32 channel)
{
    u32 *current = buffer->current;

    current[0] = 0x70000000U;
    current[3] = current[2] = current[1] = 0;
    FlushCache(0);
    xglDmaDirectSrcChain(channel, (u32)buffer->start);
    buffer->current = buffer->start;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_dma", xglDmaMFIFOSetup);

INCLUDE_ASM("asm/main/nonmatchings/xgl_dma", xglDmaMFIFOKick);

void xglDmaMFIFOLeave(void)
{
    XglDmaChannel *drain;

    sceGsSyncPath(0, 0);
    *DMAC_CTRL &= ~0xcU;
    while (*D8_CHCR & 0x100U)
        ;
    drain = mfifo_drain;
    while (drain->control & 0x100U)
        ;
}

void xglDmaInitial(void)
{
}
