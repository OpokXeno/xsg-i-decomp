#include "common.h"

/*
 * libkernel syscall stubs (main:0x00208fe0 / main:0x00209028).
 */
extern int DIntr(void);
extern int EIntr(void);

INCLUDE_ASM("asm/main/nonmatchings/ssd_4", SsdInitMemoryManager);

/*
 * iSsdNewMemoryPtr is this TU's own low-address block allocator (its body is
 * still scaffold below, main:0x00242150). `tag` is stored verbatim at +0xc of
 * the returned block's header; nothing recovered in this TU reads it back.
 */
extern void *iSsdNewMemoryPtr(int size, int tag);

void *SsdNewMemoryPtr(int size, int tag)
{
    void *ptr;

    DIntr();
    ptr = iSsdNewMemoryPtr(size, tag);
    EIntr();
    return ptr;
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_4", iSsdNewMemoryPtr);

/*
 * iSsdNewMemoryPtr2 is this TU's own high-address block allocator (its body
 * is still scaffold below, main:0x00242278); same header convention as
 * iSsdNewMemoryPtr.
 */
extern void *iSsdNewMemoryPtr2(int size, int tag);

void *SsdNewMemoryPtr2(int size, int tag)
{
    void *ptr;

    DIntr();
    ptr = iSsdNewMemoryPtr2(size, tag);
    EIntr();
    return ptr;
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_4", iSsdNewMemoryPtr2);

/*
 * iSsdDisposeMemoryPtr is this TU's own block-release routine (its body is
 * still scaffold below, main:0x002423b0): `ptr` minus the fixed 0x40-byte
 * header size is the block header, returned to the free list.
 */
extern void iSsdDisposeMemoryPtr(void *ptr);

void SsdDisposeMemoryPtr(void *ptr)
{
    DIntr();
    iSsdDisposeMemoryPtr(ptr);
    EIntr();
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_4", iSsdDisposeMemoryPtr);

INCLUDE_ASM("asm/main/nonmatchings/ssd_4", SsdGetBlockMemorySize);

INCLUDE_ASM("asm/main/nonmatchings/ssd_4", SsdGetMemoryFreeSize);

INCLUDE_ASM("asm/main/nonmatchings/ssd_4", SsdGetMemoryBlocks);

INCLUDE_ASM("asm/main/nonmatchings/ssd_4", SsdCopyMemory);

INCLUDE_ASM("asm/main/nonmatchings/ssd_4", SsdClearMemory);
