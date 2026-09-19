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
typedef struct MenuModelSubWindow {
    unsigned char unmodeled_00[1];
    signed char closeFlag;
} MenuModelSubWindow;

typedef struct MenuModelUnit {
    unsigned char unmodeled_00[0x13];
    unsigned char extFuncFlag;            /* +0x13: cleared whenever the ext functions below are (re)installed */
    unsigned char unmodeled_14[0x0C];
    MenuModelActor *actor;                /* +0x20: the unit's own model, passed to MenuModelDrawTypeSet */
    unsigned char unmodeled_24[0x10];
    MenuModelWeapon *weapon[3];           /* +0x34: one slot per MenuModelUnitOpen weapon loop iteration */
    void *extFunc1;                       /* +0x40: MenuModelExtFuncSet's second argument */
    void *extFunc2;                       /* +0x44: MenuModelExtFuncSet's third argument */
    unsigned char unmodeled_48[4];
    MenuModelSubWindow *subWindow;        /* +0x4C: tested/forced by MenuModelSubWindowMain/Break, processed by MenuModelSubWindowMainSub */
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
struct MenuModelActor {
    unsigned char unmodeled_00[0x120];
    float transparency; /* +0x120 */
};

extern XglTaskScheduler *MenuModelTask;
extern int MenuModelFlag;

void MenuModelDrawTypeSet(MenuModelActor *actor, int drawType);
void MenuModelWeaponOpen(MenuModelUnit *unit, int drawType, int weaponIndex);
void MenuModelResourceMain(void);
void MenuModelResourceInit(void);
void MenuModelSubWindowinit(void);
void MenuModelResourceCancel(void);
void MenuModelResourceCancel3(MenuModelUnit *unit);
void MenuModelSubWindowMainSub(MenuModelUnit *unit);
void ActorAndResourceDispose(MenuModelActor **actorSlot, void **resourceSlot, int mode);
/* Builds a task list of `capacity` 0x80-byte tasks at `pool` and returns the
 * first byte after them (0 when capacity is 0), see 0x0021c528. */
void *xglTaskInitial(void *pool, int capacity, int flags);

/* nml_model_set.c (main); no header is published for it yet. */
extern void nmlModelSetToumei(int enabled);
extern void nmlModelSetTransparency(float transparency);
extern void nmlModelSetZwrite(int enabled);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelMemorySet);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelMemoryGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelMemoryInit);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelResourceInit);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelMenuMotionCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelMenuMotionGet);

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
    MenuModelSubWindow *subWindow;

    subWindow = unit->subWindow;
    if (subWindow != 0) {
        subWindow->closeFlag = -1;
        MenuModelSubWindowMainSub(unit);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelSubWindowSet);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelSubWindowinit);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", ActorAndResourceDispose);

void MenuModelWeaponDispose(MenuModelWeapon *weapon)
{
    if (weapon != 0) {
        ActorAndResourceDispose(&weapon->actor, &weapon->resource, 2);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelUnitDispose);

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

INCLUDE_ASM("asm/main/nonmatchings/menu_model", ModelDrawTypeOpen01);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", ModelDrawTypeClose01);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", ModelDrawTypeClose02);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelDrawTypeSet);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelWeaponOpen);

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

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelUnitBreak);

INCLUDE_ASM("asm/main/nonmatchings/menu_model", CreateInit);

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

INCLUDE_ASM("asm/main/nonmatchings/menu_model", MenuModelAllBreak);

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
