#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/sm_initilize", smInitMemInfo);

INCLUDE_ASM("asm/main/nonmatchings/sm_initilize", smInitilize);

INCLUDE_ASM("asm/main/nonmatchings/sm_initilize", smAlloc);

INCLUDE_ASM("asm/main/nonmatchings/sm_initilize", smFree);

/* Free-list node walked by smAlloc/smFree/smPrintInfo; nMemCell counts them. */
typedef struct MemCell {
    int tag;
    unsigned char unmodeled_04[8];
    struct MemCell *next;
} MemCell;

#define MC_TAG_FREE 0x1234
#define MC_TAG_USED 0x5678

extern MemCell *pmcHead;

void smPrintInfo(void)
{
    MemCell *cell = pmcHead;

    if (cell != 0) {
        do {
            if (cell->tag != MC_TAG_FREE && cell->tag != MC_TAG_USED) {
                break;
            }
            cell = cell->next;
        } while (cell != 0);
    }
}
