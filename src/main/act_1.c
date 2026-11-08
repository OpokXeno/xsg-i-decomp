#include "common.h"
#include "main/act_1.h"
#include "act_1.h"
#include "main/act_2.h"
#include "main/exm_2.h"
#include "main/game_init_camera.h"
#include "main/rsrc.h"
#include "main/xgl_studio.h"
#include "quadword.h"

/* PadData's pressed-button halfword layout as ACT_info reads it. */
typedef struct ActInfoPad {
    u8 pad_00[0x28];
    u16 half_28;
    u16 half_2a;
    u16 half_2c;
} ActInfoPad;

extern ActInfoPad PadData;

extern void *memset(void *destination, int value, unsigned int count);

/*
 * ACT_draw calls the static ACT_info below through this alias of its
 * address; a direct call changes the linked image's symbol table.
 */
extern void ACT_info_00306090(void);
extern void LOOK_target_doit(void);

extern void ACT_updateMotionPause(Actor *actor);

extern void UnduParamInit(void *param);
extern void EXM_InitMovedHair(Actor *actor);
extern void DB_params(const char *params);
static RsrcManager *actMatrixHeap = 0;

static RsrcManager matrixHeap;

static ActMatrixArena actMatrix;

#define ACT_MATRIX_SIZE 64 /* one 4x4 float joint matrix */
#define ACT_MATRIX_HEAP_BOUND 0xC0000
#define ACT_MATRIX_HEAP_MAX 64
#define ACTOR_PIXEL_ALPHA_DEFAULT 117
#define ACTOR_BODY_RADIUS_DEFAULT 0.35f
#define ANIM_SLOT_FLAG_PAUSED 0x80000
#define ACT_INFO_PAGE_COUNT 3

static RsrcItem matrixHeapBlock[ACT_MATRIX_HEAP_MAX];

static ActInfoState actInfo;

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
/*
 * ACT_info's text. actorEffectControl is the 16-byte control string ACT_info hands to
 * DB_params before any line (its first five bytes are 13, 2, 14, 2, 2, the
 * rest zero); the others are the format strings of its DB_println calls,
 * in order of first use (the three shortest, "%8.3f", "ANIM" and "MDL",
 * stay string literals in the code because they live in .sdata).
 */
/* Text: {effect 2}{effect params} */
const char actorEffectControl[16] = "\r\002\016\002\002";

const char actorIndexFormat[] = "ACT[%02x] %04x";

const char actorFlagsFormat[] = "FLAGS %08x";

const char actorPosRotLabel[] = "     POS      ROT";

const char actorPositionValueFormat[] = "%8.3f %8.3f";

const char actorSclLabel[] = "     SCL";

const char actorMotionNumberFormat[] = "NO    %4d";

const char actorSpeedFormat[] = "SPEED %8.3f";

const char actorResourceFormat[] = "RSRC[%d] %p";

const char modelPartCountFormat[] = "numParts %d";

const char actorMinMaxLabel[] = "     MIN      MAX";

const char actorCenterLabel[] = "  CENTER";

const char actorParentFormat[] = "PARENT %d";

const char actorChildFormat[] = "CHILD[%d] %d";

/*
 * Allocates the joint matrices, the previous joint matrices and, for kind 1
 * actors, the value record areas of self. A negative count uses the move's
 * matrixCount (1 without a move). Returns the count, or -1 when the matrix
 * heap is not set up.
 */
int ACT_allocMatrix(Actor *self, int count)
{
    unsigned char *block;
    int extra;
    ActMove *move;

    if (count < 0) {
        count = 1;
        move = self->move;
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
            extra = count * ACT_MATRIX_SIZE;
        }
        block = RSRC_alloc(actMatrixHeap, count * (2 * ACT_MATRIX_SIZE) + extra, (int)self);
        if (block != 0) {
            self->jointMatrices = block;
            self->previousJointMatrices = block + count * ACT_MATRIX_SIZE;
            if (self->kind == 1) {
                block += count * (2 * ACT_MATRIX_SIZE);
                self->staticValueRecords = block;
                self->valueRecords = block + count * 32;
                memset(block, 0, count * ACT_MATRIX_SIZE);
            }
            ACT_resetMatrix(self);
        }
    }
    return count;
}

/* Allocates blockCount 64-byte blocks from the actor matrix heap under tag. */
void *ACT_allocBlock(int tag, int blockCount)
{
    return RSRC_alloc(actMatrixHeap, blockCount << 6, tag);
}

/*
 * Sets up the shared actor matrix heap that ACT_allocMatrix and ACT_allocBlock
 * use through actMatrixHeap: a resource arena over actMatrix with its own
 * table of ACT_MATRIX_HEAP_MAX items, which it clears.
 */
void ACT_matrixInit(void)
{
    matrixHeap.heap_bound = ACT_MATRIX_HEAP_BOUND;
    matrixHeap.item_capacity = ACT_MATRIX_HEAP_MAX;
    matrixHeap.item_count = 0;
    actMatrixHeap = &matrixHeap;
    matrixHeap.heap_origin = actMatrix.bytes;
    matrixHeap.items = matrixHeapBlock;
    matrixHeap.heap_cursor = actMatrix.bytes;
    memset(matrixHeapBlock, 0, sizeof(matrixHeapBlock));
}

/*
 * Sets up the matrix heap, then resets every actor entry to its initial state
 * and initializes the wind.
 */
void ACT_init(void)
{
    Actor *self;
    int index;
    int count;
    signed char *byte;
    void **resource;
    Actor **child;
    float (*quad)[4];

    ACT_matrixInit();
    for (index = 0; index < ACTOR_COUNT; index++) {
        self = &actor[index];
        self->flags = ACTOR_FLAGS_INITIAL;
        self->drawEnv.pixelAlpha = ACTOR_PIXEL_ALPHA_DEFAULT;
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
        self->globalPosition.w = 1.0f;
        self->velocity.w = 0;
        self->acceleration.w = 0;
        self->drawEnv.flags = 0;
        self->drawEnv.renderLevel = 0;
        self->drawEnv.partAlphaCount = 0;
        self->drawEnv.cameraCullDistance = 0;
        self->sortOffset = 0;
        self->transitionFrames = 0;
        self->holdFrames = -1;
        self->humanKind = 0;
        self->attachmentCount = 0;
        self->attachmentIndex = -1;
        self->pauseCounter = 0;
        /* Clear the attachment flags, light colors, resource slots and child links. */
        byte = self->attachmentUsed + 7;
        count = 7;
        do {
            count -= 1;
            *byte = 0;
            byte -= 1;
        } while (count >= 0);
        quad = self->drawEnv.lightColor;
        count = 2;
        do {
            QuadwordZero(quad);
            count -= 1;
            quad += 1;
        } while (count >= 0);
        count = 10;
        resource = ACTOR_RESOURCES(self) + ACTOR_RESOURCE_COUNT - 1;
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
        /* Reset the undulation and look-at state. */
        UnduParamInit(&self->undulation);
        self->windowId = 0;
        self->look_eye_control = 0;
        self->look_mode = 1;
        self->look_target = 0;
        QuadwordZero(&self->lookState[0]);
        QuadwordZero(&self->lookState[4]);
        QuadwordZero(&self->lookState[8]);
        self->look_speed = 1.0f;
        self->look_eye_speed = 1.0f;
        self->shadowClipScale = 1.0f;
        self->mapShadowPartCount = 0;
        EXM_InitMovedHair(self);
        self->statea60 = 0;
    }
    EXM_InitWind();
}

/*
 * Claims an actor entry for id. A slot of 0 or more picks that entry, -1 the
 * first free entry and -2 the last free one. Returns the entry, or 0 when no
 * entry is available.
 */
Actor *ACT_create(int slot, int id)
{
    Actor *self;
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
    self->flags = ACTOR_FLAGS_INITIAL;
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
        self->shadowSize = 16;
    } else {
        self->shadow_kind = 1;
        self->shadowSize = 80;
    }
    self->talk_message = 0;
    self->body_radius = ACTOR_BODY_RADIUS_DEFAULT;
    self->touch_message = 0;
    EXM_InitMovedHair(self);
    self->statea60 = 0;
    return self;
}

/*
 * Full teardown of one actor entry (ACT_dispose2 with quick set to 0).
 * ActorAndResourceDispose tail-calls this.
 */
void ACT_dispose(Actor *self)
{
    ACT_dispose2(self, 0);
}

/*
 * Releases one actor entry: retires its matrix-heap resources, detaches a
 * parent link and clears the in-use id, hooks and flags. A nonzero quick
 * leaves the ten resource slots at +0x8d4..+0x8f8 as they are.
 */
void ACT_dispose2(Actor *self, int quick)
{
    Actor *parent;
    void **resource;
    int count;

    RSRC_inactiveSource(actMatrixHeap, (int)self);
    parent = self->parent;
    if (parent != 0) {
        ACT_resetParent(self, parent);
    }
    self->inUseId = 0;
    self->update = 0;
    self->draw = 0;
    self->flags = 0;
    if (!quick) {
        count = 10;
        resource = ACTOR_RESOURCES(self) + ACTOR_RESOURCE_COUNT - 1;
        do {
            count -= 1;
            *resource = 0;
            resource -= 1;
        } while (count >= 0);
    }
}

/*
 * Steps the shake/wind simulation, runs the debug actor printer, then draws
 * every in-use actor.
 */
void ACT_draw(void)
{
    Actor *cursor;
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

/*
 * Flushes the active camera, resets the debug print origin with DB_reset(8, 8),
 * runs each in-use actor's update hook when it is set, then drives the look-at
 * target.
 */
void ACT_update(void)
{
    Actor *cursor;
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

/*
 * Runs the motion-pause update for each in-use, visible actor while every
 * animation slot carries the paused flag.
 */
void ACT_pauseUpdate(void)
{
    int index;

    xglStudioFlushActiveCamera();
    for (index = 0; index < ACTOR_COUNT; index++) {
        actor[index].animation.slot.flags |= ANIM_SLOT_FLAG_PAUSED;
    }
    for (index = 0; index < ACTOR_COUNT; index++) {
        if (actor[index].inUseId != 0 && (actor[index].flags & ACTOR_FLAG_HIDDEN) == 0) {
            ACT_updateMotionPause(&actor[index]);
        }
    }
    for (index = 0; index < ACTOR_COUNT; index++) {
        actor[index].animation.slot.flags &= ~ANIM_SLOT_FLAG_PAUSED;
    }
}

/*
 * Debug actor printer, shown while actInfo.visible bit 0 is set and toggled by
 * SELECT with L2. The d-pad picks the actor (left/right) and page (up/down):
 * page 0 shows position, rotation, scale and animation, page 1 the non-null
 * resource pointers and model bounds, page 2 the parent and children.
 */
static void ACT_info(void)
{
    ActInfoPad *pad = &PadData;
    ActInfoState *info = &actInfo;
    Actor *self;
    ActModel *model;
    Actor *parent;
    unsigned int buttons;
    int index;

    if ((pad->half_28 & PAD_SELECT) != 0 && (pad->half_2a & PAD_L2) != 0) {
        info->visible ^= 1;
    }
    if ((info->visible & 1) == 0) {
        return;
    }
    buttons = pad->half_2c;
    if ((buttons & PAD_RIGHT) != 0) {
        info->actorIndex += 1;
    }
    if ((buttons & PAD_LEFT) != 0) {
        info->actorIndex -= 1;
    }
    if ((buttons & PAD_UP) != 0) {
        info->page += 1;
    }
    if ((buttons & PAD_DOWN) != 0) {
        info->page -= 1;
    }
    /* Wrap the selection into range. */
    if (info->actorIndex < 0) {
        info->actorIndex = ACTOR_COUNT - 1;
    }
    if (info->actorIndex >= ACTOR_COUNT) {
        info->actorIndex = 0;
    }
    if (info->page < 0) {
        info->page = ACT_INFO_PAGE_COUNT - 1;
    }
    if (info->page >= ACT_INFO_PAGE_COUNT) {
        info->page = 0;
    }
    self = &actor[info->actorIndex];
    /* Print the selected actor's header, then the lines of the current page. */
    DB_reset(112, 32);
    DB_params(actorEffectControl);
    DB_println(actorIndexFormat, info->actorIndex, self->inUseId);
    switch (info->page) {
    case 0:
        DB_println(actorFlagsFormat, self->flags);
        DB_println(actorPosRotLabel);
        DB_println(actorPositionValueFormat, (double)self->position.x,
                   (double)(self->rotation.x / 3.1415927f * 180.0f));
        DB_println(actorPositionValueFormat, (double)self->position.y,
                   (double)(self->rotation.y / 3.1415927f * 180.0f));
        DB_println(actorPositionValueFormat, (double)self->position.z,
                   (double)(self->rotation.z / 3.1415927f * 180.0f));
        DB_println(actorSclLabel);
        DB_println("%8.3f", (double)self->scale.x);
        DB_println("%8.3f", (double)self->scale.y);
        DB_println("%8.3f", (double)self->scale.z);
        DB_println("ANIM");
        DB_println(actorMotionNumberFormat, self->animation.slot.currentDataId);
        DB_println(actorFlagsFormat, self->animation.slot.flags);
        DB_println(actorSpeedFormat, (double)(self->animation.slot.speed / (1.0f / 30.0f)));
        break;
    case 1:
        for (index = 0; index < ACTOR_RESOURCE_COUNT; index++) {
            if (ACTOR_RESOURCES(self)[index] != 0) {
                DB_println(actorResourceFormat, index, ACTOR_RESOURCES(self)[index]);
            }
        }
        model = self->modelResource;
        if (model != 0) {
            DB_println("MDL");
            DB_println(modelPartCountFormat, model->numParts);
            DB_println(actorMinMaxLabel);
            DB_println(actorPositionValueFormat, (double)model->boxMin[0], (double)model->boxMax[0]);
            DB_println(actorPositionValueFormat, (double)model->boxMin[1], (double)model->boxMax[1]);
            DB_println(actorPositionValueFormat, (double)model->boxMin[2], (double)model->boxMax[2]);
            DB_println(actorCenterLabel);
            DB_println("%8.3f", (double)model->center[0]);
            DB_println("%8.3f", (double)model->center[1]);
            DB_println("%8.3f", (double)model->center[2]);
            DB_println("%8.3f", (double)model->center[3]);
        }
        break;
    case 2:
        parent = self->parent;
        if (parent != 0) {
            DB_println(actorParentFormat, parent->number);
        }
        if (self->childCount > 0) {
            for (index = 0; index < self->childCount; index++) {
                DB_println(actorChildFormat, index, self->children[index]->number);
            }
        }
        break;
    }
}
