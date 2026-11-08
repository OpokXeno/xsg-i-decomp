#include "common.h"
#include "shared.h"
#include "main/string_utf_get_hash.h"
#include "xheap.h"
#include "main/xheap.h"

extern void reloadClassEntry(void *heap_boundary);

/* Every block carries a 12-byte header (xheap_block), and a block's category
 * (0 = free) selects one of 32 accounting buckets. */
#define XHEAP_BLOCK_HEADER_SIZE 12
#define XHEAP_CATEGORY_COUNT 32
#define XHEAP_CATEGORY_MASK (XHEAP_CATEGORY_COUNT - 1)

static xheap_block *heap_top;

static int heap_size;

static xheap_frame *frame_stack;

static xheap_block *freeBlock;

struct JThread *jthreadCurrent;

static void xmemchk() {
}

/*
 * Resets the heap to empty. A zero keep_heap installs heap and size as the new heap first;
 * otherwise the existing heap is cleared in place. The frame stack starts at the heap's end.
 */
void xheap_init(int keep_heap, xheap_block *heap, int size) {
    if (keep_heap == 0) {
        heap_top = heap;
        heap_size = size;
    }

    memset(heap_top, 0, heap_size);
    freeBlock = heap_top;
    freeBlock->size_words = 0;
    frame_stack = (xheap_frame *)((unsigned char *)heap_top + heap_size);
    freeBlock->next = heap_top;
}

/* Zeroes the unused heap between the current allocation point and the frame stack. */
void xheap_current_clear(void) {
    unsigned char *current = (unsigned char *)freeBlock->next;
    unsigned char *limit = (unsigned char *)frame_stack - 8;

    if (current < limit) {
        do {
            current[0] = 0;
            current[1] = 0;
            current[2] = 0;
            current[3] = 0;
            current += 4;
        } while (current < limit);
    }
}

/* Saves the free block and current thread on the frame stack and returns the free block. */
void *xheap_push(void) {
    frame_stack -= 1;
    frame_stack->free_block = freeBlock;
    frame_stack->thread = jthreadCurrent;
    return freeBlock;
}

/*
 * Restores the free block and thread saved by xheap_push, calls reloadConstString and
 * reloadClassEntry with the restored block's next pointer, and returns the free block.
 */
void *xheap_pop(void) {
    freeBlock = frame_stack->free_block;
    jthreadCurrent = frame_stack->thread;
    reloadConstString(freeBlock->next);
    reloadClassEntry(freeBlock->next);
    frame_stack += 1;
    return freeBlock;
}

/*
 * Totals the bytes of every allocated block (payload plus header) per category into
 * category_totals, and returns the overall total.
 */
int xheap_info(int *category_totals) {
    xheap_block *block;
    int total;
    int i;

    for (i = XHEAP_CATEGORY_COUNT - 1; i >= 0; i--) {
        category_totals[i] = 0;
    }

    total = 0;
    for (block = heap_top; block != 0; block = block->next) {
        if (block->category != 0) {
            int size = block->size_words << 2;
            int category = block->category & XHEAP_CATEGORY_MASK;

            total = total + size + XHEAP_BLOCK_HEADER_SIZE;
            category_totals[category] = category_totals[category] + size + XHEAP_BLOCK_HEADER_SIZE;
        }
    }
    return total;
}

/*
 * Walks the heap and accumulates the size of each allocated block per category. The totals
 * are not returned or read afterwards.
 */
void infoMemory(void) {
    int totals[XHEAP_CATEGORY_COUNT];
    xheap_block *block;
    int i;

    for (i = XHEAP_CATEGORY_COUNT - 1; i >= 0; i--) {
        totals[i] = 0;
    }

    block = heap_top;
    while (block != 0) {
        if (block->category != 0) {
            int *category_total = &totals[block->category & XHEAP_CATEGORY_MASK];
            int size = block->size_words << 2;

            *category_total += size;
            *category_total += XHEAP_BLOCK_HEADER_SIZE;
        }
        block = block->next;
    }
}

/*
 * Allocates size bytes, rounded up to a whole word, from the free block and tags the block with
 * category. Returns the payload, or 0 for size 0.
 */
void *xmalloc(unsigned int size, int category) {
    xheap_block *block;
    xheap_block *next;

    if (size == 0) {
        return 0;
    }

    size = ((size + 3) >> 2) << 2;
    block = freeBlock->next;
    next = (xheap_block *)((unsigned char *)block + size + XHEAP_BLOCK_HEADER_SIZE);
    block->size_words = size >> 2;
    block->category = category;
    block->next = next;
    freeBlock = block;
    xmemchk(block);
    return block + 1;
}

/* Marks the block of payload free (category 0). A null payload is ignored. */
void xfree(void *payload) {
    xheap_block *block = payload;

    if (payload != 0) {
        (block - 1)->category = 0;
    }
}
