#include "common.h"

/* Free-list node walked by smAlloc/smFree/smPrintInfo; nMemCell counts them. */
typedef struct MemCell {
    int tag;
    unsigned int cellCount;
    struct MemCell *prev;
    struct MemCell *next;
} MemCell;

#define MC_TAG_FREE 0x1234
#define MC_TAG_USED 0x5678

extern MemCell *pmcHead;

INCLUDE_ASM("asm/main/nonmatchings/sm_initilize", smInitMemInfo);

static void smInitMemInfo(unsigned char *info);

extern unsigned int nMemCell;
extern unsigned char stMemInfo[];

void smInitilize(MemCell *pool, unsigned int poolSize)
{
    unsigned int totalCells = poolSize >> 4;

    nMemCell = totalCells;
    pmcHead = pool;

    pool->tag = MC_TAG_FREE;
    pool->cellCount = totalCells - 1;
    pool->prev = 0;
    pool->next = 0;

    smInitMemInfo(stMemInfo);
}

INCLUDE_ASM("asm/main/nonmatchings/sm_initilize", smAlloc);

INCLUDE_ASM("asm/main/nonmatchings/sm_initilize", smFree);

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
