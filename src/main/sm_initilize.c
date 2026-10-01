#include "common.h"

/* Free-list node walked by smAlloc/smFree/smPrintInfo; nMemCell counts them. */
typedef struct MemCell {
    int tag;
    unsigned int cellCount;
    struct MemCell *prev;
    struct MemCell *next;
} MemCell;

typedef struct SmMemInfo {
    MemCell *freeList;
    unsigned int totalCells;
    unsigned int allocatedCells;
    unsigned int totalBytes;
    unsigned int allocatedBytes;
    unsigned int largestBlockBytes;
} SmMemInfo;

#define MC_TAG_FREE 0x1234
#define MC_TAG_USED 0x5678
#define SM_ALLOC_ALIGNMENT 2048U

extern MemCell *pmcHead;

extern unsigned int nMemCell;

static void smInitMemInfo(unsigned char *memory_info)
{
    SmMemInfo *info = (void *)memory_info;
    unsigned int pool_bytes = (nMemCell + 1) * sizeof(MemCell);

    info->freeList = pmcHead;
    info->totalCells = nMemCell;
    info->allocatedCells = 0;
    info->totalBytes = pool_bytes;
    info->allocatedBytes = 0;
    info->largestBlockBytes = pool_bytes;
}

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

void smPrintInfo(void);

void *smAlloc(unsigned int size)
{
    unsigned int requested_cell_count;
    unsigned int best_cell_count;
    MemCell *cell;
    MemCell *best_cell;

    size = (size + SM_ALLOC_ALIGNMENT - 1) & ~(SM_ALLOC_ALIGNMENT - 1);
    requested_cell_count = size / sizeof(MemCell);
    best_cell = 0;
    best_cell_count = nMemCell;
    cell = pmcHead;

    for (; cell != 0; cell = cell->next) {
        if (cell->tag != MC_TAG_FREE) {
            continue;
        }
        if (cell->cellCount < requested_cell_count) {
            continue;
        }
        if (cell->cellCount < best_cell_count) {
            best_cell_count = cell->cellCount;
            best_cell = cell;
            if (best_cell_count == requested_cell_count) {
                break;
            }
        }
    }

    if (best_cell == 0) {
        smPrintInfo();
        return 0;
    }

    best_cell->tag = MC_TAG_USED;
    if (best_cell->cellCount != requested_cell_count) {
        cell = &best_cell[requested_cell_count + 1];

        cell->tag = MC_TAG_FREE;
        cell->prev = best_cell;
        cell->cellCount = best_cell->cellCount - requested_cell_count - 1;
        cell->next = best_cell->next;
        if (cell->next != 0) {
            cell->next->prev = cell;
        }
        best_cell->cellCount = requested_cell_count;
        best_cell->next = cell;
    }

    return &best_cell[1];
}

void smFree(void *allocation)
{
    MemCell *cell;
    MemCell *neighbor;

    if (allocation == 0) {
        return;
    }

    cell = (MemCell *)allocation - 1;
    cell->tag = MC_TAG_FREE;

    neighbor = cell->next;
    if (neighbor != 0 && neighbor->tag == MC_TAG_FREE) {
        cell->cellCount += neighbor->cellCount + 1;
        cell->next = neighbor->next;
        if (cell->next != 0) {
            cell->next->prev = cell;
        }
    }

    neighbor = cell->prev;
    if (neighbor != 0 && neighbor->tag == MC_TAG_FREE) {
        unsigned int merged_count = neighbor->cellCount + cell->cellCount + 1;
        MemCell *successor = cell->next;

        neighbor->next = successor;
        neighbor->cellCount = merged_count;
        if (successor != 0) {
            successor->prev = neighbor;
        }
    }
}

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
