/*
 * TU-local declarations of ov01/tu002 (src/ov01/unit_cmd.c).
 */

#ifndef SRC_OV01_UNIT_CMD_H
#define SRC_OV01_UNIT_CMD_H

#include "shared.h"

/* calcUPGet's return type comes from its definer, ov01/tu004 calc.c. */
#include "ov01/calc.h"

extern CalcUnitParam *calcUPGet(ObjectTask *unit);
extern int calcStatGet(ObjectTask *unit, int category, int flag);
extern int dataCidGet(int chara_id);

/*
 * The battle actor object an ObjectTask's own `work` slot (include/shared.h)
 * points at once a unit is loaded; unitActupdate/unitActdraw below are its
 * `update`/`draw` callbacks. The head up to +0x70 is the Actor head main's
 * TU-local headers already spell identically (src/main/chr.h,
 * src/main/near_dir.h, src/main/set_motion.h, where the evidence for each
 * member is recorded); a TU-local header cannot be included from another TU,
 * so it is restated here verbatim. This TU adds the two members its own code
 * touches past that head: unitActdraw reads the float at +0xC0 and
 * unitActupdate advances the word at +0xE0.
 */
/*
 * unitCmdDirSet/unitCmdDivSet (0x00a05a60, 0x00a05ba8) also read and blend
 * the x/z components of the Vector4 at +0x90 against a move command's target
 * coordinate; y and w of that Vector4 are untouched by any function this TU
 * claims.
 */
typedef struct Actor {
    u32 flags;
    void (*update)(struct Actor *actor);
    void (*draw)(struct Actor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    unsigned char unmodeled_70[0x74 - 0x70];
    int moveState;         /* +0x74: unitCmdMove's own move-command step;
                             * 0 issues the move (unitCmdDstSet/unitCmdVASet
                             * and, for a jump, unitCmdDirSet) and advances to
                             * 1, which then waits for unitCmdJump/unitCmdWarp
                             * to report arrival before calling
                             * unitCmdMoveNext. */
    int movePhase;          /* +0x78: reset to 0 by unitCmdMove when a move
                             * starts; unitCmdJump/unitCmdWarp (still asm)
                             * advance it through their own per-move-kind
                             * phases. */
    unsigned char unmodeled_7c[0x90 - 0x7c];
    Vector4 cmdTarget;    /* +0x90: unitCmdDirSet reads x/z against
                            * motionActor->position to face the pending move
                            * command; unitCmdDivSet blends x/z toward that
                            * position by a supplied ratio. y and w are
                            * untouched by any function this TU claims. */
    unsigned char unmodeled_a0[0xC0 - 0xA0];
    float transparency;   /* +0xC0: unitActdraw draws translucent below 1.0 */
    unsigned char unmodeled_c4[0xE0 - 0xC4];
    int motionFrame;      /* +0xE0: unitActupdate advances it before
                             ACT_updateMotion */
    unsigned char unmodeled_e4[0xF0 - 0xE4];
    int motion;            /* +0xF0: unitMotGet returns it plus one */
    unsigned char unmodeled_f4[0x714 - 0xF4];
    float interpTime;      /* +0x714: unitSetInterpTime's write target */
} Actor;

/*
 * Partial view of the unit record beyond the published ObjectTask prefix
 * (task+work, ending at +0x14, include/shared.h): unitSetInterpTime and
 * unitMotGet both follow a pointer immediately after that prefix, and
 * unitWpnMotSet reads a 4-entry pointer array starting eight bytes further.
 * Both point at Actor objects: unitWpnMotSet forwards the entry it reads to
 * ACT_animGetData/ACT_setMotion, whose first parameter is already
 * Actor-typed (src/main/act_2.c). No other byte of this wider record is
 * claimed, and the relationship between this pointer and ObjectTask.work is
 * not evidenced here.
 */
typedef struct UnitRecord UnitRecord;

struct UnitRecord {
    ObjectTask task;                   /* +0x00..+0x13: the ObjectTask
                                         * prefix, opaque here */
    Actor *motionActor;                /* +0x14 */
    unsigned char unmodeled_018[0x1c - 0x18]; /* +0x18..+0x1b */
    Actor *wpnActor[4];                /* +0x1c */
};

/* Its first parameter is already Actor-typed (src/main/act_2.c). */
extern void *ACT_animGetData(Actor *actor, unsigned int dataId);

/* Defined in a different translation unit (src/main/act_2.c), still asm
 * there; its packed dataId argument matches ACT_animGetData's convention
 * (category << 8 | sub-id), the shape unitWpnMotSet builds before calling
 * it. */
extern void ACT_setMotion(Actor *actor, unsigned int dataId);

/*
 * unitLoad and unitActupdate both test this bit of Actor.flags to tell an
 * enemy-controlled unit from a player-controlled one: unitLoad picks
 * sefSetupEnemy over sefSetupPlayer when it is set, and unitActupdate only
 * recomputes the ground-height component of position when it is set.
 */
#define ACTOR_FLAG_ENEMY 0x40u

/*
 * unitCmdSet (src/ov01/unit_cmd.c, still asm) sets this bit of Actor.flags
 * when it queues a command whose selector is nonzero (ori 0x2 at
 * 0x00a01d5c); unitCmdFree clears it (and 0x00a01e08).
 */
#define ACTOR_FLAG_CMD_PENDING 0x2u

/* The three shorts after charaId are CalcUnitParam.sefSetupParams[0..2]. */
extern void sefSetupPlayer(short charaId, short, short, short);
extern void sefSetupEnemy(short charaId);
extern void dataUnitFileLoad(ObjectTask *unit, int mode);
extern void dataSndSeRegLoad(ObjectTask *unit);

extern void ACT_updateMotion(Actor *actor);
extern float UnduGet(float x, float z);

extern void nmlModelSetTransparency(float transparency);
extern void nmlModelSetToumei(int enabled);
extern void nmlModelSetZwrite(int enabled);

extern void unitCmdExec(ObjectTask *unit);
extern void statEffProc(ObjectTask *unit);
extern void unitClipSet(ObjectTask *unit);

/*
 * obj.c (src/ov01/obj.c, ov01/tu001) owns the unit's ObjectCommandQueue and
 * its accessors; objCmdTailGet/objCmdPtrGet are its published exact_c
 * (returning &queue->entries[queue->readIndex]/[writeIndex]) and objCmdNext
 * is still asm there. This TU's unitCmdTailGet/unitCmdPtrGet/unitCmdNext are
 * plain forwarders to them.
 */
extern void *objCmdTailGet(ObjectTask *task);
extern void *objCmdPtrGet(ObjectTask *task);
extern int objCmdNext(ObjectTask *task);

extern int unitCmdStand(ObjectTask *unit);

/* fdlibm atan2f, already declared identically in src/main/get.h. */
extern float atan2f(float y, float x);

/*
 * unitCmdMove's own siblings, still asm in this TU; forward-declared for the
 * calls unitCmdMove makes before their definitions appear later in the file.
 */
extern float unitCmdDirSet(UnitRecord *unit);
extern void unitCmdDstSet(ObjectTask *unit, int, int);
extern void unitCmdVASet(ObjectTask *unit, int, int);
extern int unitCmdJump(ObjectTask *unit, int, int, int);
extern int unitCmdWarp(ObjectTask *unit, int, int);
extern int unitCmdMoveNext(ObjectTask *unit, int);

/*
 * dataMtdRead/dataDefWpnGet/transWepIn/transWepOut are defined in
 * src/ov01/battle_init.c (ov01/tu003), which declares and calls them on its
 * own BattleUnit *, the same record this TU's UnitRecord restates (both
 * TU-local headers spell the shared Actor head identically); unitMtdProc is
 * this TU's own caller, on its own UnitRecord * spelling of that record.
 */
extern int dataMtdRead(UnitRecord *unit, int trigger);
extern int dataDefWpnGet(UnitRecord *unit);
extern void transWepIn(UnitRecord *unit, int slot);
extern void transWepOut(UnitRecord *unit, int slot);

#endif /* SRC_OV01_UNIT_CMD_H */
