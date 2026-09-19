#include "common.h"
#include "act_1.h"

extern void *memset(void *destination, int value, unsigned int count);

INCLUDE_ASM("asm/main/nonmatchings/act_1", ACT_allocMatrix);

/*
 * RSRC_alloc is defined by another TU (src/main/rsrc.c, still unrecovered)
 * and is not yet declared in a shared header, so its parameters are
 * declared here from this call site: main:0x00305a84 shifts the block count
 * left by 6 (multiplying by 64) into the size argument, and main:0x00305a7c
 * moves this function's own tag argument into the third argument unchanged.
 */
extern void RSRC_alloc(MatrixHeap *heap, int size, int tag);

void ACT_allocBlock(int tag, int blockCount)
{
    RSRC_alloc(actMatrixHeap, blockCount << 6, tag);
}

/*
 * ACT_matrixInit establishes the shared actor matrix-heap state consumed by
 * ACT_allocMatrix/ACT_allocBlock through actMatrixHeap before any allocation.
 *
 * Field roles are proven by bounded callers:
 * - heap_origin (+0x00): origin subtracted from the cursor in RSRC_check
 *   (main:0x0026d780: lw v0,4(a0) / lw a2,0(a0) / subu v0,v0,a2); never
 *   advanced after init, so it anchors the heap while the cursor moves.
 * - heap_cursor (+0x04): bump cursor advanced per item in RSRC_alloc
 *   (main:0x0026d84c: lw v0,4(s1) ... addu v0,v0,v1 / sw v0,4(s1)).
 * - heap_bound (+0x08): byte bound compared in RSRC_check
 *   (main:0x0026d78c: lw v1,8(a0) / sltu v0,v0,v1); init value 0xC0000.
 * - heap_store (+0x0c): item storage indexed by RSRC_alloc (0x0026d820:
 *   lw v0,12(s1) / addu s0,v0,a2) and RSRC_info/RSRC_getDirtyItem.
 * - heap_count (+0x10): item count incremented per alloc (0x0026d824:
 *   sh a0,16(s1) with a0 = count+1); loop bound in RSRC_info.
 * - heap_max (+0x12): max items 64 (0x0026d7d8: lhu v1,18(s1) /
 *   sltu v1,v0,v1 gate).
 * Later MatrixHeap fields past offset 0x14 remain unknown; this private type
 * intentionally stops at the six evidenced offsets.
 */

void ACT_matrixInit(void)
{
    matrixHeap.heap_bound = 0xC0000;
    matrixHeap.heap_max = 64;
    matrixHeap.heap_count = 0;
    actMatrixHeap = &matrixHeap;
    matrixHeap.heap_origin = actMatrix;
    matrixHeap.heap_store = matrixHeapBlock;
    matrixHeap.heap_cursor = actMatrix;
    memset(matrixHeapBlock, 0, 0x400);
}

INCLUDE_ASM("asm/main/nonmatchings/act_1", ACT_init);

INCLUDE_ASM("asm/main/nonmatchings/act_1", ACT_create);

INCLUDE_ASM("asm/main/nonmatchings/act_1", ACT_dispose);

INCLUDE_ASM("asm/main/nonmatchings/act_1", ACT_dispose2);

INCLUDE_ASM("asm/main/nonmatchings/act_1", ACT_draw);

INCLUDE_ASM("asm/main/nonmatchings/act_1", ACT_update);

INCLUDE_ASM("asm/main/nonmatchings/act_1", ACT_pauseUpdate);

INCLUDE_ASM("asm/main/nonmatchings/act_1", ACT_info_00306090);
