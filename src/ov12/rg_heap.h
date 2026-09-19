/*
 * TU-local declarations of ov12/tu019 (src/ov12/rg_heap.c).
 */

#ifndef SRC_OV12_RG_HEAP_H
#define SRC_OV12_RG_HEAP_H

#include "shared.h"

/*
 * RgHeap fields, evidenced by InitRgHeap/ClearRgHeap and the heap-level
 * debug print "heap = %p top = %p size = %d\n\n" (called from _Error with
 * arguments pHeap, top, size): top and size sit at 0x00/0x04. pHead is the
 * free-block list head _SetHeapHead returns and InitRgHeap/ClearRgHeap store
 * at 0x08. pAlloc is the allocated-block list head RgHeapAlloc reads and
 * writes at 0x0C, which InitRgHeap clears to NULL. sizeof(RgHeap) == 0x10
 * matches the scaffold's D_00A59AA0/D_00A5A2B0 singletons.
 */
struct RgHeap {
    void *top;
    u32 size;
    struct RgHeapBlock *pHead;
    struct RgHeapBlock *pAlloc;
};

void InitRgHeap(RgHeap *pHeap, void *pTop, u32 nBufSize);
void ClearRgHeap(RgHeap *pHeap);
RgHeap *InstanceOfRgHeap(void);
RgHeap *InstanceOfRgHeapData(void);
void RgHeapDumpBlock(RgHeap *pHeap, void *pPtr);

#endif /* SRC_OV12_RG_HEAP_H */
