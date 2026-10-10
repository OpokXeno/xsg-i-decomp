#include "common.h"

#include "act_1.h"

static MatrixHeap matrixHeap;

static unsigned char actMatrix[0xC0000];

static unsigned char matrixHeapBlock[0x400];

static MatrixHeap *actMatrixHeap = 0;

extern void *memset(void *destination, int value, unsigned int count);

/*
 * RSRC_alloc is defined by another TU (src/main/rsrc.c, still unrecovered)
 * and is not yet declared in a shared header, so its parameters are
 * declared here from this call site: main:0x00305a84 shifts the block count
 * left by 6 (multiplying by 64) into the size argument, and main:0x00305a7c
 * moves this function's own tag argument into the third argument unchanged.
 */

extern void *RSRC_alloc(MatrixHeap *heap, int size, int tag);

/* Returns the new block's storage (RSRC_alloc returns the item's +0x08 data). */

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
 * The scaffold reserves 0x20 bytes for matrixHeap. Later fields past offset
 * 0x14 remain unknown; the tail records that established object extent without
 * assigning names or invented semantics to those bytes.
 */

extern void RSRC_inactiveSource(MatrixHeap *heap, ActRecord *actor);

extern void ACT_resetParent(ActRecord *self, ActRecord *parent);

void ACT_dispose2(ActRecord *self, int quick);

/*
 * The plain default-options entry: ActorAndResourceDispose (main:0x0027fec0)
 * tail-calls it, and it always asks ACT_dispose2 for the full teardown
 * (main:0x00305e64..0x00305e6c).
 */

/*
 * Releases one actor entry: RSRC_inactiveSource retires its matrix-heap
 * resources, a live parent link is handed to ACT_resetParent before it is
 * cleared, and the entry's own hooks and in-use id go back to zero. `quick`
 * skips the ten-word block immediately before the parent link
 * (+0x8d4..+0x8f8) when it is nonzero; ACT_dispose always passes zero (full
 * teardown).
 */

extern void EXM_StepShakeWind(void);

extern void ACT_info_00306090(void);

extern void ACT_modelDraw(ActRecord *actor);

/*
 * Steps the wind simulation, refreshes the debug actor printer, then draws
 * every in-use actor (ACT_info_00306090's own "ACT[%02x]"/in-use reading
 * confirms the same test; see the ActRecord comment in act_1.h).
 */

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

extern void ACT_updateMotionPause(ActRecord *actor);

extern void ACT_resetMatrix(ActRecord *self);

extern void UnduParamInit(void *param);

extern void EXM_InitMovedHair(ActRecord *actor);

extern void EXM_InitWind(void);

/* PadData's pressed-button halfword layout as ACT_info reads it. */

typedef struct ActInfoPad {
    u8 pad_00[0x28];
    u16 half_28;
    u16 half_2a;
    u16 half_2c;
} ActInfoPad;

extern ActInfoPad PadData;

extern ActInfoState actInfo;

extern void DB_params(const char *params);

extern void DB_println(const char *format, ...);

/*
 * ACT_info's text. D_004D1708 is the 16-byte control string ACT_info hands to
 * DB_params before any line (its first five bytes are 13, 2, 14, 2, 2, the
 * rest zero); the others are the format strings of its DB_println calls,
 * in order of first use (the three shortest, "%8.3f", "ANIM" and "MDL",
 * stay string literals in the code because they live in .sdata).
 */

const char D_004D1708[16] = "\r\002\016\002\002";

const char D_004D1718[] = "ACT[%02x] %04x";

const char D_004D1728[] = "FLAGS %08x";

const char D_004D1738[] = "     POS      ROT";

const char D_004D1750[] = "%8.3f %8.3f";

const char D_004D1760[] = "     SCL";

const char D_004D1770[] = "NO    %4d";

const char D_004D1780[] = "SPEED %8.3f";

const char D_004D1790[] = "RSRC[%d] %p";

const char D_004D17A0[] = "numParts %d";

const char D_004D17B0[] = "     MIN      MAX";

const char D_004D17C8[] = "  CENTER";

const char D_004D17D8[] = "PARENT %d";

const char D_004D17E8[] = "CHILD[%d] %d";

/*
 * The debug actor printer, shown while bit 0 of actInfo.visible is set
 * (toggled by the pad's 0x100 + 0x1 chord) and driven by the d-pad bits
 * of the pad's third halfword (0x2000/0x8000 move the printed slot,
 * 0x1000/0x4000 the page). Page 0 prints flags, position and rotation (in
 * degrees), scale, the motion number, the second flags word and the speed;
 * page 1 prints the eleven resource pointers and the model's part count and
 * bounds; page 2 prints the parent and the children.
 */

int ACT_allocMatrix(ActRecord *self, int count)
{
    unsigned char *block;
    int extra;
    ActMove *move;

    if (count < 0) {
        count = 1;
        move = self->resource[2];
        if (move != 0) {
            count = move->matrixCount;
        }
    }
    if (actMatrixHeap == 0) {
        return -1;
    }
    if (count > 0) {
        extra = 0;
        if (self->kind == 1) {
            extra = count * 64;
        }
        block = RSRC_alloc(actMatrixHeap, count * 128 + extra, (int)self);
        if (block != 0) {
            self->matrixBlock = block;
            self->matrixBlockEnd = block + count * 64;
            if (self->kind == 1) {
                block += count * 128;
                self->matrixExtra = block;
                self->matrixExtraEnd = block + count * 32;
                memset(block, 0, count * 64);
            }
            ACT_resetMatrix(self);
        }
    }
    return count;
}

void *ACT_allocBlock(int tag, int blockCount)
{
    return RSRC_alloc(actMatrixHeap, blockCount << 6, tag);
}

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

void ACT_init(void)
{
    ActRecord *self;
    int index;
    int count;
    unsigned char *byte;
    void **resource;
    ActRecord **child;
    float *quad;

    ACT_matrixInit();
    for (index = 0; index < ACTOR_COUNT; index++) {
        self = &actor[index];
        self->flags = 0x20;
        self->pixel_alpha = 117;
        self->state84 = 0;
        self->childCount = 0;
        self->inUseId = 0;
        self->update = 0;
        self->draw = 0;
        self->parent = 0;
        self->scale.x = 1.0f;
        self->scale.y = 1.0f;
        self->scale.z = 1.0f;
        self->scale.w = 1.0f;
        self->position.w = 1.0f;
        self->global_position.w = 1.0f;
        self->velocity.w = 0;
        self->acceleration.w = 0;
        self->render_flags = 0;
        self->render_command = 0;
        self->pixel_alpha_parts = 0;
        self->state9d0 = 0;
        self->sort_offset = 0;
        self->hair_stop_a = 0;
        self->hair_stop_b = -1;
        self->state83 = 0;
        self->state698 = 0;
        self->state69c = -1;
        self->state696 = 0;
        byte = self->state6e0 + 7;
        count = 7;
        do {
            count -= 1;
            *byte = 0;
            byte -= 1;
        } while (count >= 0);
        quad = self->state970;
        count = 2;
        do {
            __asm__ __volatile__("sq $0, 0(%0)" : : "r"(quad) : "memory");
            count -= 1;
            quad += 4;
        } while (count >= 0);
        count = 10;
        resource = self->resource + 10;
        do {
            count -= 1;
            *resource = 0;
            resource -= 1;
        } while (count >= 0);
        count = 3;
        child = self->children + 3;
        do {
            count -= 1;
            *child = 0;
            child -= 1;
        } while (count >= 0);
        UnduParamInit(&self->undulation);
        self->state676 = 0;
        self->look_eye_control = 0;
        self->look_mode = 1;
        self->look_target = 0;
        __asm__ __volatile__("sq $0, 0(%0)" : : "r"(&self->lookState[0]) : "memory");
        __asm__ __volatile__("sq $0, 0(%0)" : : "r"(&self->lookState[4]) : "memory");
        __asm__ __volatile__("sq $0, 0(%0)" : : "r"(&self->lookState[8]) : "memory");
        self->look_speed = 1.0f;
        self->look_eye_speed = 1.0f;
        self->shadow_clip_scale = 1.0f;
        self->shadow_map = 0;
        EXM_InitMovedHair(self);
        self->statea60 = 0;
    }
    EXM_InitWind();
}

ActRecord *ACT_create(int slot, int id)
{
    ActRecord *self;
    int index;

    if (slot < 0) {
        if (slot == -1) {
            for (index = 0; index < ACTOR_COUNT; index++) {
                if (actor[index].inUseId == 0) {
                    slot = index;
                    break;
                }
            }
        } else if (slot == -2) {
            for (index = ACTOR_COUNT - 1; index >= 0; index--) {
                if (actor[index].inUseId == 0) {
                    slot = index;
                    break;
                }
            }
        }
    }
    if (slot < 0) {
        return 0;
    }
    if ((unsigned int)slot >= ACTOR_COUNT) {
        return 0;
    }
    self = &actor[slot];
    self->kind = id >> 16;
    self->flags = 0x20;
    self->number = slot;
    self->inUseId = id;
    self->childCount = 0;
    self->scale.x = 1.0f;
    self->scale.y = 1.0f;
    self->scale.z = 1.0f;
    self->scale.w = 1.0f;
    self->parent = 0;
    self->draw = 0;
    if ((id & 0xf000) == 0) {
        self->shadow_kind = 4;
        self->shadow_size = 16;
    } else {
        self->shadow_kind = 1;
        self->shadow_size = 80;
    }
    self->talk_message = 0;
    self->state9e8 = 0.35f;
    self->touch_message = 0;
    EXM_InitMovedHair(self);
    self->statea60 = 0;
    return self;
}

void ACT_dispose(ActRecord *self)
{
    ACT_dispose2(self, 0);
}

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

void ACT_pauseUpdate(void)
{
    int index;

    xglStudioFlushActiveCamera();
    for (index = 0; index < ACTOR_COUNT; index++) {
        actor[index].runtimeFlags |= 0x80000;
    }
    for (index = 0; index < ACTOR_COUNT; index++) {
        if (actor[index].inUseId != 0 && (actor[index].flags & 8) == 0) {
            ACT_updateMotionPause(&actor[index]);
        }
    }
    for (index = 0; index < ACTOR_COUNT; index++) {
        actor[index].runtimeFlags &= ~0x80000;
    }
}

static void ACT_info(void)
{
    ActInfoPad *pad = &PadData;
    ActInfoState *info = &actInfo;
    ActRecord *self;
    ActModel *model;
    ActRecord *parent;
    unsigned int buttons;
    int index;

    if ((pad->half_28 & 0x100) != 0 && (pad->half_2a & 1) != 0) {
        info->visible ^= 1;
    }
    if ((info->visible & 1) == 0) {
        return;
    }
    buttons = pad->half_2c;
    if ((buttons & 0x2000) != 0) {
        info->actorIndex += 1;
    }
    if ((buttons & 0x8000) != 0) {
        info->actorIndex -= 1;
    }
    if ((buttons & 0x1000) != 0) {
        info->page += 1;
    }
    if ((buttons & 0x4000) != 0) {
        info->page -= 1;
    }
    if (info->actorIndex < 0) {
        info->actorIndex = 63;
    }
    if (info->actorIndex >= 64) {
        info->actorIndex = 0;
    }
    if (info->page < 0) {
        info->page = 2;
    }
    if (info->page >= 3) {
        info->page = 0;
    }
    self = &actor[info->actorIndex];
    DB_reset(112, 32);
    DB_params(D_004D1708);
    DB_println(D_004D1718, info->actorIndex, self->inUseId);
    switch (info->page) {
    case 0:
        DB_println(D_004D1728, self->flags);
        DB_println(D_004D1738);
        DB_println(D_004D1750, (double)self->position.x,
                   (double)(self->rotation.x / 3.1415927f * 180.0f));
        DB_println(D_004D1750, (double)self->position.y,
                   (double)(self->rotation.y / 3.1415927f * 180.0f));
        DB_println(D_004D1750, (double)self->position.z,
                   (double)(self->rotation.z / 3.1415927f * 180.0f));
        DB_println(D_004D1760);
        DB_println("%8.3f", (double)self->scale.x);
        DB_println("%8.3f", (double)self->scale.y);
        DB_println("%8.3f", (double)self->scale.z);
        DB_println("ANIM");
        DB_println(D_004D1770, self->motionNumber);
        DB_println(D_004D1728, self->runtimeFlags);
        DB_println(D_004D1780, (double)(self->speed / (1.0f / 30.0f)));
        break;
    case 1:
        for (index = 0; index < 11; index++) {
            if (self->resource[index] != 0) {
                DB_println(D_004D1790, index, self->resource[index]);
            }
        }
        model = self->resource[0];
        if (model != 0) {
            DB_println("MDL");
            DB_println(D_004D17A0, model->numParts);
            DB_println(D_004D17B0);
            DB_println(D_004D1750, (double)model->boxMin[0], (double)model->boxMax[0]);
            DB_println(D_004D1750, (double)model->boxMin[1], (double)model->boxMax[1]);
            DB_println(D_004D1750, (double)model->boxMin[2], (double)model->boxMax[2]);
            DB_println(D_004D17C8);
            DB_println("%8.3f", (double)model->center[0]);
            DB_println("%8.3f", (double)model->center[1]);
            DB_println("%8.3f", (double)model->center[2]);
            DB_println("%8.3f", (double)model->center[3]);
        }
        break;
    case 2:
        parent = self->parent;
        if (parent != 0) {
            DB_println(D_004D17D8, parent->number);
        }
        if (self->childCount > 0) {
            for (index = 0; index < self->childCount; index++) {
                DB_println(D_004D17E8, index, self->children[index]->number);
            }
        }
        break;
    }
}
