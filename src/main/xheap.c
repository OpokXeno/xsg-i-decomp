#include "common.h"
#include "xheap.h"

INCLUDE_ASM("asm/main/nonmatchings/xheap", xmemchk);

INCLUDE_ASM("asm/main/nonmatchings/xheap", xheap_init);

INCLUDE_ASM("asm/main/nonmatchings/xheap", xheap_current_clear);

void *xheap_push(void) {
    frame_stack -= 1;
    frame_stack->free_block = freeBlock;
    frame_stack->thread = jthreadCurrent;
    return freeBlock;
}

INCLUDE_ASM("asm/main/nonmatchings/xheap", xheap_pop);

INCLUDE_ASM("asm/main/nonmatchings/xheap", xheap_info);

INCLUDE_ASM("asm/main/nonmatchings/xheap", infoMemory);

INCLUDE_ASM("asm/main/nonmatchings/xheap", xmalloc);

INCLUDE_ASM("asm/main/nonmatchings/xheap", xfree);
