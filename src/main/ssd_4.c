#include "common.h"
#include "ssd_4.h"
#include "ssd_init.h"

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

/*
 * Returns the payload size recorded in a sound-memory allocation header:
 * `ptr` minus one SsdMemoryBlock header is the header itself (same
 * convention as SsdDisposeMemoryPtr above), and `end` is the absolute
 * address just past the payload.
 */
int SsdGetBlockMemorySize(void *ptr)
{
    SsdMemoryBlock *block;

    block = (SsdMemoryBlock *)ptr - 1;
    return block->end - (int)block;
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_4", SsdGetMemoryFreeSize);

int SsdGetMemoryBlocks(void)
{
    SsdMemoryBlock *block;
    SsdMemoryBlock *nextBlock;
    int count;

    DIntr();
    count = 0;
    block = ((SsdMemoryBlock *)RssdWork.first_block)->next;
    if (block != 0) {
        do {
            nextBlock = block->next;
            count++;
            block = nextBlock;
        } while (nextBlock != 0);
    }
    EIntr();
    return count;
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_4", SsdCopyMemory);

void SsdClearMemory(void *buffer, int size)
{
    unsigned long long *destination64 = buffer;
    unsigned int *destination32;
    unsigned char *destination_bytes;
    long block_count;
    long byte_count;

    block_count = size >> 5;
    if (block_count != 0) {
        do {
            destination64[3] = 0;
            destination64[2] = 0;
            destination64[1] = 0;
            destination64[0] = 0;
            destination64 += 4;
            block_count--;
        } while (block_count != 0);
    }

    destination32 = (unsigned int *)destination64;
    block_count = (size >> 4) & 1;
    if (block_count != 0) {
        do {
            destination32[3] = 0;
            destination32[2] = 0;
            destination32[1] = 0;
            destination32[0] = 0;
            destination32 += 4;
            block_count--;
        } while (block_count != 0);
    }

    destination_bytes = (unsigned char *)destination32;
    byte_count = size & 0xf;
    while (byte_count != 0) {
        *destination_bytes++ = 0;
        byte_count--;
    }
}
