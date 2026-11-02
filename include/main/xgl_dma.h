#ifndef INCLUDE_MAIN_XGL_DMA_H
#define INCLUDE_MAIN_XGL_DMA_H

#include "shared.h"

extern PadPrefix PadData;

/* Functions and data used by other translation units. */
void xglDmaDirectNormal(u32 channel, u32 address, u32 count);
void xglDmaDirectSrcChain(u32 channel, u32 address);

#endif /* INCLUDE_MAIN_XGL_DMA_H */
