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
extern void *RSRC_alloc(MatrixHeap *heap, int size, int tag);

/* Returns the new block's storage (RSRC_alloc returns the item's +0x08 data). */
void *ACT_allocBlock(int tag, int blockCount)
{
    return RSRC_alloc(actMatrixHeap, blockCount << 6, tag);
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

extern void RSRC_inactiveSource(MatrixHeap *heap, ActRecord *actor);
extern void ACT_resetParent(ActRecord *self, ActRecord *parent);
void ACT_dispose2(ActRecord *self, int quick);

/*
 * The plain default-options entry: ActorAndResourceDispose (main:0x0027fec0)
 * tail-calls it, and it always asks ACT_dispose2 for the full teardown
 * (main:0x00305e64..0x00305e6c).
 */
void ACT_dispose(ActRecord *self)
{
    ACT_dispose2(self, 0);
}

/*
 * Releases one actor entry: RSRC_inactiveSource retires its matrix-heap
 * resources, a live parent link is handed to ACT_resetParent before it is
 * cleared, and the entry's own hooks and in-use id go back to zero. `quick`
 * skips the ten-word block immediately before the parent link
 * (+0x8d4..+0x8f8) when it is nonzero; ACT_dispose always passes zero (full
 * teardown).
 */
void ACT_dispose2(ActRecord *self, int quick)
{
    ActRecord *parent;
    int *word;
    int count;

    RSRC_inactiveSource(actMatrixHeap, self);
    parent = self->parent;
    if (parent != 0) {
        ACT_resetParent(self, parent);
    }
    self->inUseId = 0;
    self->update = 0;
    self->draw = 0;
    self->flags = 0;
    if (!quick) {
        word = (int *)((unsigned char *)self + ACTOR_LINKED_BLOCK_END_OFFSET);
        count = 10;
        do {
            count -= 1;
            *word = 0;
            word -= 1;
        } while (count >= 0);
    }
}

extern void EXM_StepShakeWind(void);
extern void ACT_info_00306090(void);
extern void ACT_modelDraw(ActRecord *actor);

/*
 * Steps the wind simulation, refreshes the debug actor printer, then draws
 * every in-use actor (ACT_info_00306090's own "ACT[%02x]"/in-use reading
 * confirms the same test; see the ActRecord comment in act_1.h).
 */
void ACT_draw(void)
{
    ActRecord *cursor;
    int remaining;

    remaining = ACTOR_COUNT - 1;
    EXM_StepShakeWind();
    ACT_info_00306090();
    cursor = actor;
    do {
        if (cursor->inUseId != 0) {
            ACT_modelDraw(cursor);
        }
        remaining -= 1;
        cursor += 1;
    } while (remaining >= 0);
}

extern void xglStudioFlushActiveCamera(void);
extern void DB_reset(int, int);
extern void LOOK_target_doit(void);

/*
 * Refreshes the active camera, resets the debug light-write window with two
 * literal arguments (the only values this TU ever passes), then runs every
 * in-use actor's own per-frame update hook (main:0x00305f94: jalr through
 * the entry's `update` member, skipped when null) before driving the
 * look-at target.
 */
void ACT_update(void)
{
    ActRecord *cursor;
    int remaining;

    remaining = ACTOR_COUNT - 1;
    xglStudioFlushActiveCamera();
    DB_reset(8, 8);
    cursor = actor;
    do {
        if (cursor->inUseId != 0) {
            if (cursor->update != 0) {
                cursor->update(cursor);
            }
        }
        remaining -= 1;
        cursor += 1;
    } while (remaining >= 0);
    LOOK_target_doit();
}

INCLUDE_ASM("asm/main/nonmatchings/act_1", ACT_pauseUpdate);

INCLUDE_ASM("asm/main/nonmatchings/act_1", ACT_info_00306090);
