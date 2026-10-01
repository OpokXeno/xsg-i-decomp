#ifndef INCLUDE_OV01_BATTLE_ACTOR_H
#define INCLUDE_OV01_BATTLE_ACTOR_H

#include "shared.h"

struct CalcUnitParam;

/* ACT_create's actor pool supplies both ObjectTask.work and the unit's
 * separate motion actor. calcUPGet reaches up through work at +0x84;
 * dataUnitFileLoadMdl reaches the model pointers through motionActor.
 * Only fields used by these consumers are named. */
typedef struct BattleActor {
    int flags;
    void (*update)(struct BattleActor *self);
    void (*draw)(struct BattleActor *self);
    unsigned char unmodeled_0c[0x84 - 0x0C];
    struct CalcUnitParam *up; /* +0x84 */
} BattleActor;

/* Rendered and equipment actors extend the same head with model storage.
 * The calc-only scratch actor stops at the head and must not acquire this
 * larger extent (calc.c's 0x110-byte temporary allocation). */
typedef struct BattleModelActor {
    BattleActor state;
    unsigned char unmodeled_88[0x8D0 - 0x88];
    void *modelAdr; /* +0x8D0 */
    void *animationAdr; /* +0x8D4 */
    void *textureAdr; /* +0x8D8 */
    /* dataUnitFileLoadMotSp2 indexes from +0x8DC; the extent is not yet
     * recovered. The overlapping motionAdr word is entry 1 at +0x8E0. */
    union {
        int slot[2];
        struct {
            int unmodeled_8dc;
            int motionAdr;
        } weapon;
    } motion;
} BattleModelActor;

#define BATTLE_ACTOR_FLAG_ENEMY 0x40
#define CALC_UNIT_FLAG_AGWS 0x40
/* unitLoad uses this same range to skip the ordinary sound setup. */
#define SPECIAL_MODEL_CHARACTER_ID_BEGIN 0xBB
#define SPECIAL_MODEL_CHARACTER_ID_END 0xC3

#endif
