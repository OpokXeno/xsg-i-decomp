#include "common.h"
#include "shared.h"
#include "main/string_utf_get_hash.h"
#include "xheap.h"

extern void reloadClassEntry(void *heap_boundary);

static void xmemchk() {
}

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

void *xheap_push(void) {
    frame_stack -= 1;
    frame_stack->free_block = freeBlock;
    frame_stack->thread = jthreadCurrent;
    return freeBlock;
}

void *xheap_pop(void) {
    freeBlock = frame_stack->free_block;
    jthreadCurrent = frame_stack->thread;
    reloadConstString(freeBlock->next);
    reloadClassEntry(freeBlock->next);
    frame_stack += 1;
    return freeBlock;
}

INCLUDE_ASM("asm/main/nonmatchings/xheap", xheap_info);

void infoMemory(void) {
    int totals[32];
    xheap_block *block;
    int i;

    for (i = 31; i >= 0; i--) {
        totals[i] = 0;
    }

    block = heap_top;
    while (block != 0) {
        if (block->category != 0) {
            int *category_total = &totals[block->category & 0x1f];
            int size = block->size_words << 2;

            *category_total += size;
            *category_total += 12;
        }
        block = block->next;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/xheap", xmalloc);

void xfree(void *payload) {
    if (payload != 0) {
        ((xheap_block *)payload - 1)->category = 0;
    }
}
