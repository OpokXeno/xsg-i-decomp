#include "common.h"
#include "main/ssd_init.h"
#include "main/ssd_4.h"
#include "ssd_4.h"
#include "main/nml_packet_add.h"

/* canon: config/header-canon.json chose src/core/main-00240188/private.h over 1 other accepted spelling */
/*
 * RssdWork is the RSSD RPC / background-wave work area, main:0x004aa080,
 * ELF symbol size 0x200 bytes. The accepted RssdWorkFlags in
 * xeno/core/types.h (from RssdBackgroundNextWave, main:0x0023ff28) only
 * carries the leading `flags` word; this function evidences more of the same
 * object, so the type is extended here under the same tag and leading field
 * Only the evidenced members are modeled here. Bytes this TU does not access
 * stay explicit unmodeled spans.
 */
struct SsdMemoryBlock;

/* Sound memory blocks start on 64-byte boundaries and their header is one such unit. */
#define SSD_MEMORY_ALIGN 64
#define SSD_MEMORY_ALIGN_MASK 0x3f
/* SsdMemoryBlock.state: the arena's first node, a block placed at the start of a free gap, one placed at its end. */
#define SSD_MEMORY_STATE_HEAD 0x81
#define SSD_MEMORY_STATE_ALLOC 2
#define SSD_MEMORY_STATE_ALLOC_HIGH 18
/* "MTOP" read as a little-endian word, the tag of the first node. */
#define SSD_MEMORY_HEAD_TAG 0x504f544d

/*
 * View of the sentinel node SsdInitMemoryManager writes at the arena start:
 * the same 0x40-byte node as SsdMemoryBlock. Here  is only cleared as
 * a word and  holds an address (the first byte after the node).
 */
typedef struct SsdMemoryNode {
    int next;                           /* +0x00 */
    struct SsdMemoryBlock *end;         /* +0x04 */
    unsigned char state;                /* +0x08 */
    unsigned char flags;                /* +0x09 */
    unsigned char _unmodeled_0a[2];     /* +0x0a..0x0b */
    int tag;                            /* +0x0c */
    unsigned char _unmodeled_10[0x30];  /* +0x10..0x3f */
} SsdMemoryNode;

/*
 * Sends one RSSD command over SIF RPC (end function RssdSifRpcCallback):
 * `request` may be null, otherwise it is copied as above, and `size` bytes
 * of `data` are copied after the record (SsdCopyMemory into buffer +0x20).
 * Returns -1 when the rounded payload exceeds the buffer, otherwise the
 * sceSifCallRpc result.
 */
/*
 * SIF RPC end callback for the RSSD work area: clears flag bit 3, copies the
 * 32-byte receive record into RssdWork, reports a status (-1 on a nonzero
 * result, else flag bit 5) through the registered one-shot completion
 * callback, then signals the waiting thread's semaphore.
 *
 * It runs from the SIF RPC interrupt context, so it ends with the
 * ee-interrupt-handler-return primitive (docs/ps2-capabilities.md): `sync.l`
 * orders every earlier load/store, including the iSignalSema effects, before
 * `ei` re-enables interrupts on the way out.
 */
/*
 * libkernel syscall stubs (main:0x00208fe0 / main:0x00209028).
 */
extern int DIntr(void);

extern int EIntr(void);

/*
 * iSsdNewMemoryPtr is this TU's own low-address block allocator (its body is
 * still scaffold below, main:0x00242150). `tag` is stored verbatim at +0xc of
 * the returned block's header; nothing recovered in this TU reads it back.
 */
extern void *iSsdNewMemoryPtr(int size, int tag);

/*
 * iSsdNewMemoryPtr2 is this TU's own high-address block allocator (its body
 * is still scaffold below, main:0x00242278); same header convention as
 * iSsdNewMemoryPtr.
 */
extern void *iSsdNewMemoryPtr2(int size, int tag);

/*
 * iSsdDisposeMemoryPtr is this TU's own block-release routine (its body is
 * still scaffold below, main:0x002423b0): `ptr` minus the fixed 0x40-byte
 * header size is the block header, returned to the free list.
 */
extern void iSsdDisposeMemoryPtr(void *ptr);

void SsdClearMemory(void *buffer, int size);

void SsdInitMemoryManager(int address, int size)
{
    SsdMemoryNode *head;
    int start;

    start = address;
    size &= ~SSD_MEMORY_ALIGN_MASK;
    if (start & SSD_MEMORY_ALIGN_MASK) {
        start = (start + SSD_MEMORY_ALIGN - 1) & ~SSD_MEMORY_ALIGN_MASK;
        size -= SSD_MEMORY_ALIGN;
    }
    head = (SsdMemoryNode *)start;
    RssdWork.first_block = (SsdMemoryBlock *)head;
    RssdWork.memory_end = start + size;
    RssdWork.memory_size = size;
    head->state = SSD_MEMORY_STATE_HEAD;
    head->flags = 0;
    head->tag = SSD_MEMORY_HEAD_TAG;
    head->end = (SsdMemoryBlock *)(head + 1);
    head->next = 0;
}

void *SsdNewMemoryPtr(int size, int tag)
{
    void *ptr;

    DIntr();
    ptr = iSsdNewMemoryPtr(size, tag);
    EIntr();
    return ptr;
}

void *iSsdNewMemoryPtr(int size, int tag)
{
    SsdMemoryBlock *previous;
    SsdMemoryBlock *next;
    SsdMemoryBlock *block;
    void *payload;
    int needed;
    int start;
    int gap;

    needed = ((size + SSD_MEMORY_ALIGN - 1) & ~SSD_MEMORY_ALIGN_MASK) + SSD_MEMORY_ALIGN;
    previous = RssdWork.first_block;
    do {
        next = previous->next;
        if (next == 0) {
            break;
        }
        start = previous->end;
        gap = (int)next - start;
        if (gap >= needed) {
            break;
        }
        previous = next;
    } while (1);
    if (next == 0) {
        start = previous->end;
        gap = RssdWork.memory_end - start;
        if (gap < needed) {
            return 0;
        }
    }

    block = (SsdMemoryBlock *)((start + SSD_MEMORY_ALIGN - 1) & ~SSD_MEMORY_ALIGN_MASK);
    payload = block + 1;
    block->tag = tag;
    block->state = SSD_MEMORY_STATE_ALLOC;
    block->end = (int)payload + size;
    block->next = 0;
    block->flags = 0;
    SsdClearMemory(payload, size);
    block->next = previous->next;
    previous->next = block;
    return payload;
}

void *SsdNewMemoryPtr2(int size, int tag)
{
    void *ptr;

    DIntr();
    ptr = iSsdNewMemoryPtr2(size, tag);
    EIntr();
    return ptr;
}

void *iSsdNewMemoryPtr2(int size, int tag)
{
    SsdMemoryBlock *current;
    SsdMemoryBlock *next;
    SsdMemoryBlock *bestPrevious;
    SsdMemoryBlock *block;
    void *payload;
    int bestEnd;
    int needed;
    int start;
    int gap;
    int tailEnd;

    bestEnd = 0;
    bestPrevious = 0;
    needed = ((size + SSD_MEMORY_ALIGN - 1) & ~SSD_MEMORY_ALIGN_MASK) + SSD_MEMORY_ALIGN;
    current = RssdWork.first_block;
    do {
        next = current->next;
        if (next == 0) {
            tailEnd = current->end;
            gap = RssdWork.memory_end - tailEnd;
            if (gap >= needed) {
                bestPrevious = current;
                bestEnd = RssdWork.memory_end;
            }
            break;
        }
        start = current->end;
        gap = (int)next - start;
        if (gap >= needed) {
            bestPrevious = current;
            bestEnd = (int)next;
        }
        current = next;
    } while (1);

    if (bestPrevious == 0) {
        return 0;
    }
    bestEnd -= needed;
    block = (SsdMemoryBlock *)((bestEnd + SSD_MEMORY_ALIGN - 1) & ~SSD_MEMORY_ALIGN_MASK);
    payload = block + 1;
    block->state = SSD_MEMORY_STATE_ALLOC_HIGH;
    block->tag = tag;
    block->end = (int)payload + size;
    block->next = 0;
    block->flags = 0;
    SsdClearMemory(payload, size);
    block->next = bestPrevious->next;
    bestPrevious->next = block;
    return payload;
}

void SsdDisposeMemoryPtr(void *ptr)
{
    DIntr();
    iSsdDisposeMemoryPtr(ptr);
    EIntr();
}

void iSsdDisposeMemoryPtr(void *ptr)
{
    SsdMemoryBlock *block;
    SsdMemoryBlock *previous;
    SsdMemoryBlock *next;

    block = (SsdMemoryBlock *)ptr - 1;
    previous = RssdWork.first_block;
    next = previous->next;
    while (next != block) {
        if (next == 0) {
            return;
        }
        previous = next;
        next = previous->next;
    }
    previous->next = block->next;
}

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

int SsdGetMemoryFreeSize(void)
{
    SsdMemoryBlock *block;
    SsdMemoryBlock *nextBlock;
    SsdMemoryBlock *following;
    int largest;
    int gap;

    DIntr();
    largest = 0;
    block = RssdWork.first_block;
    nextBlock = block->next;
    if (nextBlock != 0) {
        do {
            gap = ((int)nextBlock - block->end) & ~SSD_MEMORY_ALIGN_MASK;
            block = nextBlock;
            following = block->next;
            if (largest < gap) {
                largest = gap;
            }
            nextBlock = following;
        } while (following != 0);
    }
    gap = (RssdWork.memory_end - block->end) & ~SSD_MEMORY_ALIGN_MASK;
    if (largest < gap) {
        largest = gap;
    }
    EIntr();
    return largest;
}

int SsdGetMemoryBlocks(void)
{
    SsdMemoryBlock *block;
    SsdMemoryBlock *nextBlock;
    int count;

    DIntr();
    count = 0;
    block = RssdWork.first_block->next;
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

void SsdCopyMemory(void *destination, void *source, int byte_count)
{
    int *destination_words = destination;
    const unsigned int *source_words = source;
    unsigned char *destination_bytes;
    const unsigned char *source_bytes;
    long copy_count;
    long long source_word0;
    long long source_word1;
    long long source_word2;
    long long source_word3;

    copy_count = byte_count >> 4;
    while (copy_count != 0) {
        source_word0 = source_words[0];
        source_word1 = source_words[1];
        source_word2 = source_words[2];
        source_word3 = source_words[3];
        source_words += 4;
        destination_words[0] = source_word0;
        destination_words[1] = source_word1;
        destination_words[2] = source_word2;
        destination_words[3] = source_word3;
        destination_words += 4;
        copy_count--;
    }

    copy_count = (byte_count >> 2) & 3;
    while (copy_count != 0) {
        *destination_words++ = (int)*source_words++;
        copy_count--;
    }

    copy_count = byte_count & 3;
    if (copy_count != 0) {
        source_bytes = (const unsigned char *)source_words;
        destination_bytes = (unsigned char *)destination_words;
        do {
            *destination_bytes++ = *source_bytes++;
            copy_count--;
        } while (copy_count != 0);
    }
}

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
