#ifndef INCLUDE_MAIN_MENU_MODEL_H
#define INCLUDE_MAIN_MENU_MODEL_H

#include "shared.h"
#include "main/act_1.h"
#include "main/xgl_light.h"

/*
 * The menu's model actors are canonical Actor records (include/main/act_1.h):
 * MenuModelDrawTypeSet (0x00280dd0) configures the draw-type hook in the
 * actor's menu control (+0x124), MenuModelAlphaDraw reads the draw progress
 * at +0x120 (mtc1 0x3f800000 compared with c.lt.s against it at 0x00280018),
 * and the +0x86 halfword (inUseId) gates ACT_dispose in ActorAndResourceDispose,
 * which clears it afterward; MenuModelUnitActorSet (main:0x002800e0) also
 * extracts its top nibble as a type selector.
 */

/*
 * Partial layout of the menu model "unit" object (main/tu165), recovered from
 * this allocation's own field accesses: MenuModelUnitOpen (+0x20),
 * MenuModelExtFuncSet (+0x13, +0x40, +0x44) and MenuModelWeaponActorGet's
 * dereferenced weapon slot array (+0x34). Only these offsets are evidenced;
 * the unmodeled_XX spans are untouched by this allocation.
 */
/*
 * +0x24/+0x2C: a second (actor, resource) pair, recovered from
 * MenuModelUnitDispose, which passes &actor2/&resource2 to
 * ActorAndResourceDispose with mode 1, alongside the existing +0x20/+0x28
 * pair passed with mode 0. MenuModelUnitActorSet (main:0x002800b0) stores a
 * second ACT_create(-2) result at +0x24 (the first, ACT_create(-1), goes to
 * +0x20) and loads a pointer from +0x28. +0x14, +0x1C/+0x1D, +0x30
 * and +0x48 are recovered only as bytes CreateInit clears; no other function
 * in this allocation reads or writes them.
 */
typedef struct MenuModelUnit {
    unsigned char unmodeled_00[0x10];
    unsigned char taskPhase;              /* +0x10: forced to -1 by MenuModelUnitBreak once its pending resource request is cancelled */
    unsigned char active;                 /* +0x11: set to 1 once the unit actor is open (tskMenuModelTaskMain); UmnProcurator* return while it is zero */
    unsigned char unmodeled_12[1];
    unsigned char extFuncFlag;            /* +0x13: cleared whenever the ext functions below are (re)installed */
    /* +0x14: the model kind's data record. menu_model.c reads it as the
     * external unit-resource record (weapon motion naming); the character
     * menu (menu_para_pt_rate_get.c) reads the same pointer as the
     * character's CharParaData. */
    union {
        struct MenuModelUnitResourceData *unusedByCreateInit1; /* resource-data pointer read by MenuModelResourceLoad */
        struct CharParaData *org;
    };
    unsigned short modelId;               /* +0x18: the character number for character models */
    unsigned short resourceId;            /* +0x1A */
    unsigned char unusedByCreateInit2[2]; /* +0x1C/+0x1D */
    unsigned char unmodeled_1E[2];
    Actor *actor;                             /* +0x20: the unit's own model, passed to MenuModelDrawTypeSet */
    Actor *actor2;              /* +0x24: second model, disposed with mode 1 by MenuModelUnitDispose */
    struct MenuModelResourceAccess *resource;   /* +0x28: actor's resource slot, disposed with mode 0 by MenuModelUnitDispose */
    struct MenuModelResourceAccess *resource2;  /* +0x2C: actor2's resource slot, disposed with mode 1 by MenuModelUnitDispose */
    int skipWeaponRegionCleanup;              /* +0x30 */
    struct MenuModelWeapon *weapon[3];           /* +0x34: one slot per MenuModelUnitOpen weapon loop iteration */
    void (*extFunc1)(struct MenuModelUnit *unit, void *context); /* +0x40: MenuModelExtFuncSet's second argument */
    void *extFunc2;                       /* +0x44: MenuModelExtFuncSet's third argument */
    void **unusedByCreateInit4;           /* +0x48: address of the caller's output handle */
    struct MenuModelSubWindowAccess *subWindow;  /* +0x4C: tested/forced by MenuModelSubWindowMain/Break, processed by MenuModelSubWindowMainSub */
} MenuModelUnit;

void MenuModelMemorySet(int state);
void MenuModelMemoryInit(void *base, int size);
void MenuModelMenuMotionLoad(void);
void MenuModelMain(void);
void MenuModelAllBreak(void);
void *MenuModelInit(void *pool);

/* Functions and data used by other translation units. */
void MenuModelUnitBreak(MenuModelUnit *unit);

#endif /* INCLUDE_MAIN_MENU_MODEL_H */
