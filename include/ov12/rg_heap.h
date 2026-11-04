#ifndef INCLUDE_OV12_RG_HEAP_H
#define INCLUDE_OV12_RG_HEAP_H

#include "shared.h"

/* Functions and data used by other translation units. */
void InitRgHeap(RgHeap *pHeap, void *pTop, u32 nBufSize);
void ClearRgHeap(RgHeap *pHeap);
RgHeap *InstanceOfRgHeap(void);
RgHeap *InstanceOfRgHeapData(void);
void *RgHeapAlloc(RgHeap *pHeap, u32 nSize, const char *source_file, int line);
void RgHeapFree(RgHeap *pHeap, void *pPtr, const char *source_file, int line);
int RgHeapIsInSelf(RgHeap *pHeap, void *pPtr);
int RgHeapIsInvalidMemory(RgHeap *pHeap, void *pPtr);
void RgHeapDump_sub(RgHeap *pHeap, const char *comment,
                    const char *source_file, int line);
void RgHeapDumpBlock(RgHeap *pHeap, void *pPtr);

#endif /* INCLUDE_OV12_RG_HEAP_H */
