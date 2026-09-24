#include "common.h"
#include "shared.h"
#include "main/xgl_task.h"

/*
 * The model actor MenuModelDrawTypeSet (0x00280dd0) configures: it takes the
 * actor in a0 and the draw type (0..4, a jump table) in a1 and writes the
 * actor's +0x60/+0x64 fields. Only pointers to it are used here.
 */
typedef struct MenuModelActor MenuModelActor;

/*
 * Partial layout of the menu model "unit" object (main/tu165), recovered from
 * this allocation's own field accesses: MenuModelUnitOpen (+0x20),
 * MenuModelExtFuncSet (+0x13, +0x40, +0x44) and MenuModelWeaponActorGet's
 * dereferenced weapon slot array (+0x34). Only these offsets are evidenced;
 * the unmodeled_XX spans are untouched by this allocation.
 */
typedef struct MenuModelWeapon {
    unsigned char unmodeled_00[0x20];
    MenuModelActor *actor; /* +0x20: returned by MenuModelWeaponActorGet; MenuModelWeaponOpen hands it to MenuModelDrawTypeSet */
    unsigned char unmodeled_24[4];
    /*
     * +0x28: MenuModelWeaponDispose passes the address of this field to
     * ActorAndResourceDispose as the second (resource) slot, alongside
     * &actor; that callee clears whatever pointer it finds there, but nothing
     * recovered so far dereferences it, so its pointee type stays unproven.
     */
    void *resource;
} MenuModelWeapon;

/*
 * +0x1: MenuModelSubWindowBreak forces this to -1 immediately before tail-
 * calling MenuModelSubWindowMainSub (0x0027fc88), which loads the same
 * pointer from MenuModelUnit.subWindow and reads this byte to decide how to
 * tear the subwindow down.
 */
typedef struct MenuModelSubWindowState {
    unsigned char unmodeled_00[1];
    signed char closeFlag;
} MenuModelSubWindowState;

/*
 * +0x24/+0x2C: a second (actor, resource) pair, recovered from
 * MenuModelUnitDispose, which passes &actor2/&resource2 to
 * ActorAndResourceDispose with mode 1, alongside the existing +0x20/+0x28
 * pair passed with mode 0. MenuModelUnitActorSet (main:0x002800b0) stores a
 * second ACT_create(-2) result at +0x24 (the first, ACT_create(-1), goes to
 * +0x20) and loads a pointer from +0x28. +0x11, +0x14, +0x1C/+0x1D, +0x30
 * and +0x48 are recovered only as bytes CreateInit clears; no other function
 * in this allocation reads or writes them.
 */

typedef struct MenuModelUnit {
    unsigned char unmodeled_00[0x10];
    signed char taskPhase;                /* +0x10: forced to -1 by MenuModelUnitBreak once its pending resource request is cancelled */
    unsigned char unusedByCreateInit0;    /* +0x11 */
    unsigned char unmodeled_12[1];
    unsigned char extFuncFlag;            /* +0x13: cleared whenever the ext functions below are (re)installed */
    int unusedByCreateInit1;              /* +0x14 */
    unsigned char unmodeled_18[4];
    unsigned char unusedByCreateInit2[2]; /* +0x1C/+0x1D */
    unsigned char unmodeled_1E[2];
    MenuModelActor *actor;                /* +0x20: the unit's own model, passed to MenuModelDrawTypeSet */
    MenuModelActor *actor2;               /* +0x24: second model, disposed with mode 1 by MenuModelUnitDispose */
    void *resource;                       /* +0x28: actor's resource slot, disposed with mode 0 by MenuModelUnitDispose */
    void *resource2;                      /* +0x2C: actor2's resource slot, disposed with mode 1 by MenuModelUnitDispose */
    int unusedByCreateInit3;              /* +0x30 */
    MenuModelWeapon *weapon[3];           /* +0x34: one slot per MenuModelUnitOpen weapon loop iteration */
    void *extFunc1;                       /* +0x40: MenuModelExtFuncSet's second argument */
    void *extFunc2;                       /* +0x44: MenuModelExtFuncSet's third argument */
    int unusedByCreateInit4;              /* +0x48 */
    MenuModelSubWindowState *subWindow;   /* +0x4C: tested/forced by MenuModelSubWindowMain/Break, processed by MenuModelSubWindowMainSub */
} MenuModelUnit;

/*
 * The object bound to a menu model draw task, recovered from
 * MenuModelDrawTypeMain's own access. Only +0x20 is evidenced.
 */
typedef struct MenuModelDrawContext {
    unsigned char unmodeled_00[0x20];
    struct MenuModelDrawable *drawable; /* +0x20 */
} MenuModelDrawContext;

/*
 * The object MenuModelDrawTypeMain calls through, recovered from that
 * function's own access. Only +0x124 is evidenced.
 */
typedef struct MenuModelDrawable {
    unsigned char unmodeled_00[0x124];
    void (*drawFunc)(struct MenuModelDrawable *self); /* +0x124 */
} MenuModelDrawable;

/*
 * Body for the actor tag this TU's own prelude only forward-declares above.
 * MenuModelAlphaDraw reads the float at +0x120 (mtc1 0x3f800000 compared with
 * c.lt.s against it at 0x00280018) before adjusting the model's transparency;
 * no other offset of this type is evidenced by this allocation.
 */
/*
 * +0x86: nonzero gates ACT_dispose in ActorAndResourceDispose, which clears
 * it afterward. MenuModelUnitActorSet (main:0x002800e0) also reads/writes
 * this halfword and extracts its top nibble as a type selector, so it packs
 * more than a plain boolean; only the zero/nonzero state is evidenced here.
 */
struct MenuModelActor {
    unsigned char unmodeled_00[0x86];
    short flags; /* +0x86 */
    unsigned char unmodeled_88[0x98];
    float transparency; /* +0x120 */
};

extern XglTaskScheduler *MenuModelTask;
extern int MenuModelFlag;

static void MenuModelDrawTypeSet(MenuModelActor *actor, int drawType);
void MenuModelWeaponOpen(MenuModelUnit *unit, int drawType, int weaponIndex);
void MenuModelResourceMain(void);
void MenuModelResourceInit(void);
void MenuModelSubWindowinit(void);
void MenuModelResourceCancel(void);
void MenuModelResourceCancel3(MenuModelUnit *unit);
static void MenuModelSubWindowMainSub(MenuModelUnit *unit);
static void ActorAndResourceDispose(MenuModelActor **actorSlot, void **resourceSlot, int mode);
/* Builds a task list of `capacity` 0x80-byte tasks at `pool` and returns the
 * first byte after them (0 when capacity is 0), see 0x0021c528. */
void *xglTaskInitial(void *pool, int capacity, int flags);

/* nml_model_set.c (main); no header is published for it yet. */
extern void nmlModelSetToumei(int enabled);
extern void nmlModelSetTransparency(float transparency);
extern void nmlModelSetZwrite(int enabled);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelMemorySet);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelMemoryGet);

/*
 * Partial layout of the menu-model "memory" state buffer (main/tu165),
 * recovered from MenuModelMenuMotionCheck's own access: only the state code
 * at +0x0 is evidenced; the rest of the 0x1CC-byte buffer (config/symbols)
 * is untouched by this allocation.
 */
/*
 * +0x4/+0x8: work-area base and size, recovered from MenuModelMemoryInit's
 * own stores. MainMenu (main:0x00275804) passes the block GameResourceWorkAlloc(2)
 * returns in a0 and the literal 0x480000 in a1, the same constant it also
 * adds to that block for MenuModelInit's own base argument.
 */
typedef struct MenuModelMemoryStateBuffer {
    int state; /* +0x0: memory-state code; 10-12 or 20 support menu motion */
    void *base; /* +0x4 */
    int size; /* +0x8 */
    unsigned char unmodeled_0C[0x1C0];
} MenuModelMemoryStateBuffer;

extern MenuModelMemoryStateBuffer MenuModelMemoryState;

void MenuModelMemoryInit(void *base, int size) {
    memset(&MenuModelMemoryState, 0, sizeof(MenuModelMemoryState));
    MenuModelMemoryState.base = base;
    MenuModelMemoryState.size = size;
    MenuModelMemoryState.state = -1;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelResourceInit);

int MenuModelMenuMotionCheck(void) {
    if ((MenuModelMemoryState.state >= 10) && ((MenuModelMemoryState.state < 13) || (MenuModelMemoryState.state == 20))) {
        return 1;
    }
    return 0;
}

/*
 * Partial layout of the menu-model "resource" state buffer (main/tu165),
 * recovered from MenuModelMenuMotionGet's own access: only the cached
 * motion handle at +0xC8 (the last word of the 0xCC-byte buffer,
 * config/symbols) is evidenced.
 */
typedef struct MenuModelResourceStateBuffer {
    unsigned char unmodeled_00[0xC8];
    int motionHandle; /* +0xC8 */
} MenuModelResourceStateBuffer;

extern MenuModelResourceStateBuffer MenuModelResourceState;

int MenuModelMenuMotionGet(void) {
    int handle;

    handle = 0;
    if ((MenuModelMenuMotionCheck() == 1) && (MenuModelFlag & 2)) {
        handle = MenuModelResourceState.motionHandle;
    }
    return handle;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelResourceCancel);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelResourceCancel3);

void MenuModelResourceCancel2(MenuModelUnit *unit)
{
    MenuModelResourceCancel();
    MenuModelResourceCancel3(unit);
}

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelResourceLoad);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelResourceMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelResourceRequest);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelMenuMotionLoad);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelSubWindowMainSub);

void MenuModelSubWindowMain(MenuModelUnit *unit)
{
    if (unit->subWindow != 0) {
        MenuModelSubWindowMainSub(unit);
    }
}

void MenuModelSubWindowBreak(MenuModelUnit *unit)
{
    MenuModelSubWindowState *subWindow;

    subWindow = unit->subWindow;
    if (subWindow != 0) {
        subWindow->closeFlag = -1;
        MenuModelSubWindowMainSub(unit);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelSubWindowSet);

extern unsigned char MenuModelSubWindow[];

void MenuModelSubWindowinit(void) {
    memset(MenuModelSubWindow, 0, 0x30U);
}

/*
 * Partial layout of the resource object referenced by MenuModelWeapon.resource,
 * MenuModelUnit.resource and MenuModelUnit.resource2, recovered from
 * ActorAndResourceDispose's own field accesses: a status byte at +0x0 and a
 * handle at +0x2 are cleared together with the slot pointer itself.
 */
typedef struct MenuModelResource {
    unsigned char status; /* +0x0 */
    unsigned char unmodeled_01[1];
    short handle; /* +0x2 */
} MenuModelResource;

void ACT_dispose(MenuModelActor *actor);

static void ActorAndResourceDispose(MenuModelActor **actorSlot, void **resourceSlot, int mode)
{
    MenuModelActor *actor;
    MenuModelActor *disposedActor;
    MenuModelResource *resource;
    MenuModelResource *clearedResource;

    actor = *actorSlot;
    if (actor != 0) {
        if (actor->flags != 0) {
            ACT_dispose(actor);
            disposedActor = *actorSlot;
            *actorSlot = 0;
            disposedActor->flags = 0;
            goto disposeResource;
        }
    } else {
disposeResource:
        if (mode != 1) {
            resource = *resourceSlot;
            if ((resource != 0) && ((int) resource > 0)) {
                resource->status = 0;
                clearedResource = *resourceSlot;
                *resourceSlot = 0;
                clearedResource->handle = 0;
            }
        }
    }
}

void MenuModelWeaponDispose(MenuModelWeapon *weapon)
{
    if (weapon != 0) {
        ActorAndResourceDispose(&weapon->actor, &weapon->resource, 2);
    }
}

void MenuModelUnitDispose(MenuModelUnit *unit)
{
    MenuModelUnit *self;
    MenuModelWeapon **weaponSlot;
    int weaponIndex;
    MenuModelActor **actorSlot;
    void **resourceSlot;

    self = unit;
    if (self != 0) {
        weaponSlot = &self->weapon[0];
        weaponIndex = 2;
        do {
            MenuModelWeaponDispose(*weaponSlot);
            weaponSlot++;
            weaponIndex--;
        } while (weaponIndex >= 0);
        actorSlot = &self->actor;
        resourceSlot = &self->resource;
        ActorAndResourceDispose(&self->actor2, &self->resource2, 1);
        ActorAndResourceDispose(actorSlot, resourceSlot, 0);
    }
}

void MenuModelAlphaDraw(MenuModelActor *actor)
{
    if (actor->transparency < 1.0f) {
        nmlModelSetTransparency(actor->transparency);
        nmlModelSetToumei(1);
        nmlModelSetZwrite(1);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelUnitActorSet);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelWeaponActorSet);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelControl);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelControl2);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelNavelMove);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelMotionSet);

/*
 * MenuModelDrawTypeSet's own draw-type animation state, MenuModelActor+0xc0:
 * still-asm MenuModelDrawTypeSet (0x00280dd0) forms this same address first
 * (`addiu $4,$4,0xc0` at 0x00280ddc) before writing its +0x60 (progress) and
 * +0x64 (function) fields for each drawType case; ModelDrawTypeClose01/02
 * below independently re-form it (`addiu $4,$4,0xc0`) before reading or
 * writing the same two fields. +0x60 is the same byte range as `transparency`
 * above (actor+0x120): the compiler materializes this address as a real,
 * separate step rather than folding the two offsets into `actor->
 * transparency`'s single one (compiled-form receipt: the folded form compiles
 * 8 bytes short of the original, first difference at the missing `addiu
 * a0,a0,0xc0`), so the readable source keeps that step too. +0x64, 4 bytes
 * past +0x60, holds a pointer to the function itself (`sw` of
 * ModelDrawTypeOpen01/Close01/Close02's own address at 0x00280e1c/
 * 0x00280e2c/0x00280e38); ModelDrawTypeClose01/02 clear it once they finish
 * fading transparency out (`sw $0,0x64($4)`).
 */
#define MENU_MODEL_ACTOR_DRAW_STATE_OFFSET 0xc0

#define MENU_MODEL_ACTOR_DRAW_STATE(actor) \
    ((unsigned char *)(actor) + MENU_MODEL_ACTOR_DRAW_STATE_OFFSET)

#define MENU_MODEL_DRAW_STATE_PROGRESS_OFFSET 0x60
#define MENU_MODEL_DRAW_STATE_FUNC_OFFSET     0x64

#define MENU_MODEL_DRAW_STATE_PROGRESS(state) \
    ((float *)((state) + MENU_MODEL_DRAW_STATE_PROGRESS_OFFSET))
#define MENU_MODEL_DRAW_STATE_FUNC(state) \
    ((void (**)(MenuModelActor *))((state) + MENU_MODEL_DRAW_STATE_FUNC_OFFSET))

extern float D_004D7DF8;

/*
 * Clears the draw-type function pointer (+0x64) once the fade-in progress
 * (+0x60) reaches its 1.0 target, so the draw-type dispatch stops calling it.
 */
void ModelDrawTypeOpen01(MenuModelActor *actor)
{
    unsigned char *state;
    float progress;

    state = MENU_MODEL_ACTOR_DRAW_STATE(actor);
    progress = *MENU_MODEL_DRAW_STATE_PROGRESS(state) + D_004D7DF8;
    *MENU_MODEL_DRAW_STATE_PROGRESS(state) = progress;
    if (progress >= 1.0f) {
        *MENU_MODEL_DRAW_STATE_PROGRESS(state) = 1.0f;
        *MENU_MODEL_DRAW_STATE_FUNC(state) = 0;
    }
}

extern float D_004D7DFC;

void ModelDrawTypeClose01(MenuModelActor *actor)
{
    unsigned char *state;
    float progress;

    state = MENU_MODEL_ACTOR_DRAW_STATE(actor);
    progress = *MENU_MODEL_DRAW_STATE_PROGRESS(state) - D_004D7DFC;
    *MENU_MODEL_DRAW_STATE_PROGRESS(state) = progress;
    if (progress <= 0.0f) {
        *MENU_MODEL_DRAW_STATE_PROGRESS(state) = 0.0f;
        *MENU_MODEL_DRAW_STATE_FUNC(state) = 0;
    }
}

extern float D_004D7E00;
extern float D_004D7E04;

void ModelDrawTypeClose02(MenuModelActor *actor)
{
    unsigned char *state;
    float minProgress;
    float progress;

    state = MENU_MODEL_ACTOR_DRAW_STATE(actor);
    minProgress = D_004D7E04;
    progress = *MENU_MODEL_DRAW_STATE_PROGRESS(state) - D_004D7E00;
    *MENU_MODEL_DRAW_STATE_PROGRESS(state) = progress;
    if (progress <= minProgress) {
        *MENU_MODEL_DRAW_STATE_PROGRESS(state) = minProgress;
        *MENU_MODEL_DRAW_STATE_FUNC(state) = 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelDrawTypeSet);

void MenuModelWeaponOpen(MenuModelUnit *unit, int drawType, int weaponIndex) {
    MenuModelActor *actor;
    MenuModelWeapon *weapon;

    if (unit != 0) {
        weapon = unit->weapon[weaponIndex];
        if (weapon != 0) {
            actor = weapon->actor;
            if (actor != 0) {
                MenuModelDrawTypeSet(actor, drawType);
            }
        }
    }
}

void MenuModelUnitOpen(MenuModelUnit *unit, int drawType)
{
    int weaponIndex;

    MenuModelDrawTypeSet(unit->actor, drawType);
    for (weaponIndex = 0; weaponIndex < 3; weaponIndex++) {
        MenuModelWeaponOpen(unit, drawType, weaponIndex);
    }
}

void MenuModelDrawTypeMain(MenuModelDrawContext *context)
{
    MenuModelDrawable *drawable;

    drawable = context->drawable;
    if (drawable != 0) {
        if (drawable->drawFunc != 0) {
            drawable->drawFunc(drawable);
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_model", tskMenuModelWeaponTaskMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", tskMenuModelTaskMain);

void MenuModelUnitBreak(MenuModelUnit *unit) {
    if (unit != 0) {
        MenuModelResourceCancel2(unit);
        unit->taskPhase = -1;
    }
}

static void CreateInit(MenuModelUnit *unit)
{
    int weaponIndex;

    unit->taskPhase = 0;
    unit->unusedByCreateInit0 = 0;
    unit->extFunc1 = 0;
    unit->extFunc2 = 0;
    unit->unusedByCreateInit1 = 0;
    unit->subWindow = 0;
    unit->actor = 0;
    unit->resource = 0;
    unit->actor2 = 0;
    unit->resource2 = 0;
    unit->unusedByCreateInit3 = 0;
    unit->unusedByCreateInit2[0] = 0;
    unit->unusedByCreateInit2[1] = 0;
    unit->unusedByCreateInit4 = 0;
    for (weaponIndex = 2; weaponIndex >= 0; weaponIndex--) {
        unit->weapon[weaponIndex] = 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelWeaponCreate);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelCreate);

void MenuModelExtFuncSet(MenuModelUnit *unit, void *extFunc1, void *extFunc2)
{
    if (unit != 0) {
        unit->extFunc2 = extFunc2;
        unit->extFunc1 = extFunc1;
        unit->extFuncFlag = 0;
    }
}

MenuModelActor *MenuModelWeaponActorGet(MenuModelUnit *unit, int weaponIndex)
{
    MenuModelActor *actor;
    MenuModelWeapon *weapon;

    actor = 0;
    if (unit != 0) {
        weapon = unit->weapon[weaponIndex];
        if (weapon != 0) {
            actor = weapon->actor;
        }
    }
    return actor;
}

void MenuModelMain(void)
{
    MenuModelResourceMain();
    xglTaskExecute(MenuModelTask);
}

void MenuModelAllBreak(void) {
    MenuModelFlag |= 1;
    MenuModelMain();
    MenuModelFlag &= ~1;
    MenuModelResourceInit();
    MenuModelSubWindowinit();
}

/* Carves the menu model task list out of `pool` (16-byte aligned) and
 * returns the first free byte after it (UmnMain2 stores the result back
 * into its work cursor, 0x00a02b70). */
void *MenuModelInit(void *pool)
{
    pool = (void *) (((int) pool + 0xF) & ~0xF);
    MenuModelTask = pool;
    MenuModelFlag = 0;
    pool = xglTaskInitial(pool, 32, 0);
    MenuModelResourceInit();
    MenuModelSubWindowinit();
    return pool;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelDirectSendZClear);
