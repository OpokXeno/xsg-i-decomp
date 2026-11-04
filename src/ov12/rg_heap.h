/*
 * TU-local declarations of ov12/tu019 (src/ov12/rg_heap.c).
 */

#ifndef SRC_OV12_RG_HEAP_H
#define SRC_OV12_RG_HEAP_H

#include "shared.h"
#include "ov12/rg_heap.h"

/* Size of the header in front of every block; a block must be larger than it. */
#define RG_HEAP_HEAD_SIZE 0x40
#define RG_HEAP_ALIGN_MASK 0xFU
/* The stamp length and the longest module name a block keeps (a terminator follows). */
#define RG_HEAP_MAGIC_LENGTH 0xF
#define RG_HEAP_MODULE_LENGTH 31

/*
 * RgHeap fields, evidenced by InitRgHeap/ClearRgHeap and the heap-level
 * debug print "heap = %p top = %p size = %d\n\n" (called from _Error with
 * arguments pHeap, top, size): top and size sit at 0x00/0x04. pHead is the
 * free-block list head _SetHeapHead returns and InitRgHeap/ClearRgHeap store
 * at 0x08. pAlloc is the allocated-block list head RgHeapAlloc reads and
 * writes at 0x0C, which InitRgHeap clears to NULL. sizeof(RgHeap) == 0x10
 * matches the two function-local heap singletons.
 */
struct RgHeap {
    void *top;
    u32 size;
    struct RgHeapBlock *pHead;
    struct RgHeapBlock *pAlloc;
};

/*
 * RgHeapBlock: the fixed 0x40-byte header RgHeapAlloc places immediately
 * before the block it returns (RgHeapDumpBlock reads it at pPtr - 0x40).
 * pre/next link the block into whichever circular list currently owns it
 * (RgHeap's free chain from _SetHeapHead, or its allocated chain from
 * _Link/_InsertNext), evidenced by "pre %p next %p\n". size is the block's
 * byte size and mark is set/cleared by _MarkAlloc/_UnmarkAlloc/_IsAllocated,
 * evidenced by "size %p mark %d\n" (size itself is dumped through %p in that
 * string). module and line record the RgHeapAlloc call site (module is
 * strncpy'd from its source_file argument and terminated at module[31]),
 * evidenced by "module '%s' line %d\n". magic holds the 15-character
 * s_szMagicString stamp _SetMagicString/_IsCollectMagicString read and write
 * at block+0x30. Every byte from 0x00 to 0x3F is accounted for by these six
 * members (4+4+4+2+2+32+16 == 0x40).
 */
struct RgHeapBlock {
    struct RgHeapBlock *pre;
    struct RgHeapBlock *next;
    u32 size;
    u16 mark;
    u16 line;
    char module[32];
    char magic[16];
};

#endif /* SRC_OV12_RG_HEAP_H */
