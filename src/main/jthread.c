#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_defaultUnit);

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_defaultChr);

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_default);

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_defaultScene);

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_defaultStage);

void JTHREAD_init(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_waitFor);

/*
 * This TU's own view of the VM thread list node that src/main/chr.h (main's
 * chr TU) defines in full as `JThread`; only the `next` link this loop walks
 * is named here (chr.h's `previous` at +0x04), and the rest stays unmodeled.
 */
typedef struct JThreadNode {
    unsigned char unmodeled_00[8];
    struct JThreadNode *next;           /* +0x08 */
} JThreadNode;

extern JThreadNode *jthreadTop;

void JTHREAD_info(void)
{
    JThreadNode *thread = jthreadTop;

    if (thread != 0) {
        do {
            thread = thread->next;
        } while (thread != 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_getSymbol);

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_cntl);

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_get);
