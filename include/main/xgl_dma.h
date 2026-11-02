#ifndef INCLUDE_MAIN_XGL_DMA_H
#define INCLUDE_MAIN_XGL_DMA_H

#include "shared.h"

extern PadPrefix PadData;

/* EE DMAC channel numbers: the index into the channel table (VIF1 = 1, GIF = 2). */
#define XGL_DMA_CHANNEL_VIF1 1
#define XGL_DMA_CHANNEL_GIF 2

/* DMA tag ID (bits 28..30 of the first tag word). */
#define DMA_TAG_ID_CNT 0x10000000U
#define DMA_TAG_ID_REF 0x30000000U
#define DMA_TAG_ID_CALL 0x50000000U
#define DMA_TAG_ID_END 0x70000000U
/* Clears the tag ID and IRQ bits, turning a REF tag into REFE. */
#define DMA_TAG_ID_CLEAR_MASK 0x0fffffff

/* The DMAC takes scratchpad addresses (0x7000xxxx) as an offset with the SPR flag in bit 31. */
#define DMA_SPR_ADDRESS_HIGH 0x7000U
#define DMA_SPR_OFFSET_MASK 0x3fffU
#define DMA_SPR_FLAG 0x80000000U

/* Dn_CHCR bits. */
#define DMA_CHCR_DIR 0x1
#define DMA_CHCR_MOD_CHAIN 0x4
#define DMA_CHCR_TTE 0x40
#define DMA_CHCR_STR 0x100

/* Functions and data used by other translation units. */
void xglDmaDirectNormal(u32 channel, u32 address, u32 count);
void xglDmaDirectSrcChain(u32 channel, u32 address);

#endif /* INCLUDE_MAIN_XGL_DMA_H */
