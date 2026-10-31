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
/* Defined in src/ov01/calc.c (ov01/tu004), not declared in its header. */
extern int calcIdol2Chk(ObjectTask *unit);
extern int dataCidGet(int chara_id);

/*
 * Actor.cmdTarget's type: unitCmdPosAdj saves and restores the whole command
 * target with aligned ld/sd pairs (0x00a04aa4..0x00a04ab8,
 * 0x00a04aec..0x00a04af8), where a plain Vector4 (4-byte aligned) copies
 * through ldl/ldr/sdl/sdr. The u64 pair is the alignment spelling of
 * MCamVector (src/ov01/m_cam.c) and GameVector8 (src/main/game.c) and carries
 * no meaning of its own; the components are read through `vector` and the whole is copied as one 8-byte value (unitCmdPosAdj's savedTarget). Its 8-byte
 * alignment rounds the partial Actor view's sizeof from 0x76C to 0x770;
 * no offset moves and nothing in this TU takes sizeof(Actor).
 */
typedef union UnitVector {
    Vector4 vector;
    u64 pair[2];
} UnitVector;

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
    int runFrameCounter;   /* +0x70: unitCmdRun tests it modulo 3 to space
                             * out its footstep decoEffCall. */
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
    int movePhaseTimer;     /* +0x7C: unitCmdWarp zeroes it every time it
                             * advances movePhase; no claimed function here
                             * reads it. */
    ObjectTask *targetUnit; /* +0x80: unitCmdDstSet's mode-0 path reads the
                             * command target's own CalcUnitParam/table
                             * position through it. */
    unsigned char unmodeled_84[0x90 - 0x84];
    UnitVector cmdTarget; /* +0x90: unitCmdDirSet reads x/z against
                            * motionActor->position to face the pending move
                            * command; unitCmdDivSet blends x/z toward that
                            * position by a supplied ratio. y and w are
                            * untouched by any function this TU claims. */
    unsigned char unmodeled_a0[0xC0 - 0xA0];
    float transparency;   /* +0xC0: unitActdraw draws translucent below 1.0 */
    unsigned char unmodeled_c4[0xD0 - 0xC4];
    float posSnapX;        /* +0xD0: unitCmdJump's arrival check rounds it
                             * and compares against posSnapZ*0.5 rounded. */
    float hopLength;       /* +0xD4: replaces four bytes of filler
                             * (unmodeled_d4) in this existing local Actor
                             * view. unitCmdMoveChaJump2 takes its address
                             * (`addiu $19,$2,0xD4`, 0x00a040dc), stores the
                             * command distance divided by 3 (`swc1 $f13,0($19)`,
                             * 0x00a04114) and reloads it to aim each
                             * intermediate hop (`lwc1 $f0,0($19)`, 0x00a04170). */
    float posSnapZ;        /* +0xD8 */
    unsigned char unmodeled_dc[0xE0 - 0xDC];
    int motionFrame;      /* +0xE0: unitActupdate advances it before
                             ACT_updateMotion */
    unsigned char unmodeled_e4[0xF0 - 0xE4];
    int motion;            /* +0xF0: unitMotGet returns it plus one */
    float moveOriginY;     /* +0xF4: unitCmdRun/unitCmdJump save cmdTarget's
                             * y/z here before a stand/attack phase and
                             * restore them afterward; the paired x value at
                             * this same phase reuses the four bytes of
                             * `motion` above through a cast (see the .c
                             * file), never through a new field here. */
    float moveOriginZ;     /* +0xF8 */
    unsigned char unmodeled_fc[0x6F0 - 0xFC];
    int moveFlags;          /* +0x6F0: unitCmdRun/unitCmdHover/unitCmdFloat/
                              * unitCmdJump test bit 0x1000 of it to detect
                              * that a move command has reached its target. */
    float moveElapsed;      /* +0x6F4: unitCmdJump reads it as a frame count
                              * source for its arrival check. */
    float moveStepRate;      /* +0x6F8: unitCmdFloat/unitCmdJump write a
                              * per-frame interpolation rate here. */
    unsigned char unmodeled_6fc[0x714 - 0x6FC];
    float interpTime;      /* +0x714: unitSetInterpTime's write target */
    unsigned char unmodeled_718[0x760 - 0x718];
    float floatStepX;      /* +0x760: unitCmdFloat adds it to its motion
                             * actor's position.x each step while floating. */
    unsigned char unmodeled_764[0x768 - 0x764];
    float floatStepZ;      /* +0x768 */
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
    Actor *subActor;                   /* +0x2c: unitRemoveActor disposes it
                                         * first when it is set */
};

/* Defined in src/main/act_1.c on its own ActRecord spelling of the actor. */
extern void ACT_dispose(Actor *actor);

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

/*
 * src/ov01/calc.c restates the same queued-command record (its own
 * CalcCmdEntry) for calcTurnStart's own call into unitCmdSet, defined in
 * this TU; unitCmdDeadSet is this TU's own caller.
 */
typedef struct CmdEntry CmdEntry;

struct CmdEntry {
    int selector;      /* +0x00 */
    int argument;      /* +0x04 */
    int extra;         /* +0x08 */
    int itmId;         /* +0x0C: unitCmdTgtSet copies it into the TgtCmd
                         * (src/ov01/calc.c) itmId it builds for
                         * tgtFindFirst (lw $3, 0xC($7); sw $3, 0x8($29),
                         * unit_cmd/unitCmdTgtSet.s 0x00a03434/0x00a0343c). */
    int mode;          /* +0x10: unitCmdTgtSet's own TgtCmd.mode, the same
                         * offset tgtFindChk branches on for mode 3
                         * (lw $2, 0x10($7); sw $2, 0xC($29),
                         * unit_cmd/unitCmdTgtSet.s 0x00a03444/0x00a0344c). */
};

extern void unitCmdSet(ObjectTask *unit, CmdEntry *cmd);

/*
 * tgtFindFirst's search criteria (src/ov01/calc.c, still asm there, its own
 * TgtCmd restated here as a local view -- flagged for review: the single
 * owner definition should live where tgtFindFirst is defined, src/ov01/calc.c,
 * per proposals.md): unitCmdTgtSet builds one of these on its own stack from
 * its queued CmdEntry before calling it.
 */
typedef struct TgtCmd {
    int tecId;          /* +0x00 */
    ObjectTask *target; /* +0x04 */
    int itmId;          /* +0x08 */
    int mode;           /* +0x0C: 3 = item */
} TgtCmd;

extern ObjectTask *tgtFindFirst(ObjectTask *unit, TgtCmd *cmd);

/* Still asm, src/ov01/battle_init.c; unitCmdTgtSet's only other caller into
 * that TU. */
extern void cmdListCansel(ObjectTask *unit);

/* The three shorts after charaId are CalcUnitParam.weaponId[0..2]. */
extern void sefSetupPlayer(short charaId, short, short, short);
extern void sefSetupEnemy(short charaId);
extern void dataUnitFileLoad(ObjectTask *unit, int mode);
extern void dataSndSeRegLoad(ObjectTask *unit);

/* Defined in src/ov01/data_unit_org_get.c, declared there the
 * same way; unitCmdReadWait waits on the first and then calls the second. */
extern int dataCdSync(void);
extern void dataUnitFileLoad2Sub(ObjectTask *unit);

extern void ACT_updateMotion(Actor *actor);
extern float UnduGet(float x, float z);

extern void nmlModelSetTransparency(float transparency);
extern void nmlModelSetToumei(int enabled);
extern void nmlModelSetZwrite(int enabled);

extern void unitCmdExec(ObjectTask *unit);
extern void statEffProc(ObjectTask *unit);
/* Defined in src/ov01/battle_init.c; unitCmdBted calls it before the
 * declaration further down the .c file. */
extern void statEffOff(ObjectTask *unit);
/* Defined in src/ov01/battle_init.c; unitCmdFadeIn calls it before the
 * declaration further down the .c file. */
extern void statEffOn(ObjectTask *unit);

/*
 * The per-unit status-effect record CalcUnitParam.effectUnit points at
 * (include/ov01/calc.h declares it incomplete). Its owner,
 * include/ov01/battle_init.h, cannot be included here (its transWepIn/
 * transWepOut take BattleUnit *, this TU's UnitRecord *), so the one member
 * this TU touches is restated at the owner's offset: unitCmdFadeOut sets and
 * unitCmdFadeIn clears UNIT_WORK_STATUS_BLOCKED in it (lw/ori/sw 0xA8C($3)
 * at 0x00a06e7c, lw/and/sw 0xA8C($4) at 0x00a06dd4).
 */
struct UnitWork {
    unsigned char unmodeled_00[0xA8C];
    unsigned int flags; /* +0xA8C */
};

/* UnitWork.flags bit that suppresses status-effect ticks for the unit. */
#define UNIT_WORK_STATUS_BLOCKED 0x100u
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
extern void *objCmdPush(ObjectTask *task);

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
extern int unitCmdHover(ObjectTask *unit, int, int);
extern int unitCmdFloat(ObjectTask *unit, int, int);
extern int unitCmdWarp(ObjectTask *unit, int, int);
extern int unitCmdMoveNext(ObjectTask *unit, int);

/*
 * unitCmdMoveChaRun/unitCmdMovePosRun's own sibling, still asm in this TU;
 * forward-declared for the calls they make to it before its definition
 * appears later in the file.
 */
extern int unitCmdRun(ObjectTask *unit, int, int);

/*
 * unitCmdWarp's own siblings, still asm in this TU; forward-declared for the
 * calls it makes before their definitions appear later in the file.
 */
extern int unitTelIn(ObjectTask *unit);
extern int unitTelOut(ObjectTask *unit);
extern void unitMotStandSet(ObjectTask *unit);

/* Defined in a different translation unit (src/ov01/data_unit_org_get.c),
 * still asm there; unitCmdDstSet is the only claimed caller, reading the x/z
 * of the position it returns. */
extern Vector4 *dataPosTblGet(int index);

/* unitCmdDstSet's own sibling, still asm in this TU; forward-declared for the
 * call it makes before its own definition appears later in the file. */
extern float unitColiGet(ObjectTask *unit, ObjectTask *targetUnit);

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

/*
 * The 0x10-byte action request unitCmdAtk builds on its stack and hands to
 * unitCmdAtkSub, which forwards it to unitJobTypeAtk; its first word is the
 * technique id dataTecGet resolves.
 */
typedef struct UnitActRequest {
    int id;              /* +0x00: technique id */
    unsigned char unmodeled_04[0x0C];
} UnitActRequest;

/*
 * dataTecGet's record (src/ov01/data_unit_org_get.c, opaque there) as far as
 * unitCmdAtkSub reads it: the signed job type at +4 picks one of
 * unitJobTypeAtk's eight job kinds ("** JobType err" otherwise).
 */
typedef struct TecJobView {
    unsigned char unmodeled_00[4];
    signed char jobType; /* +0x04 */
} TecJobView;

extern void scenarioBatInit(void);
extern TecJobView *dataTecGet(int tecId);
extern void unitJobTypeAtk(ObjectTask *unit, UnitActRequest *request, TecJobView *tec);

/*
 * unitCmdExec dispatches the queued command's selector through this table of
 * command handlers (lui/lw %lo(unitCmdTbl) indexed by selector << 2, jalr,
 * 0x00a01dc8..0x00a01dd4); a zero return advances Actor.runFrameCounter.
 */
extern int (*unitCmdTbl[])(ObjectTask *unit);

/* Defined in src/ov01/debug_entry.c, still asm there; it reads its second
 * argument as the command selector (daddu $22,$5 at 0x00a2e530). */
extern void debugCmdPrint(ObjectTask *unit, int selector);

/* Defined in src/ov01/snd.c; unitCmdWake passes its own unit
 * as the first argument (daddu $4,$16,$0 at 0x00a06a54). */
extern void sndSeTransPlay(ObjectTask *unit, int, int);

/* Defined later in this TU; unitCmdMot calls it before its definition. */
extern void unitMtdProc(UnitRecord *unit);

/*
 * Actor.flags bit unitLiveNumGet skips as not alive and unitCmdDeadSet
 * refuses to set again; unitCmdWake clears it (and -0x81, 0x00a06a94).
 */
#define ACTOR_FLAG_DOWN 0x80u

/*
 * Actor.flags bit unitCmdGuard sets (ori 0x800 at 0x00a02764) and
 * unitCmdDef tests to skip its follow-up motion 0x1C (andi at 0x00a02fac).
 */
#define ACTOR_FLAG_GUARD 0x800u

/*
 * One row of unitCmdPosAdj's root-motion tables (adjTblPl at 0x00a3d050,
 * 0xA0 bytes = 8 rows; adjTblEn at 0x00a3d0f0, 0x348 bytes = 42 rows; both
 * end at a zero charaId): the row stride is 0x14 (addiu $16,$16,0x14 at
 * 0x00a0498c) and the four curve pointers at +4..+0x10 are the per-frame
 * float tables the data names <unit>F1/F5/B1/B5 (chaosF1, kosmosB5, ...),
 * picked by step motion 0xD, 0xF/0x11, 0x12 and 0x14/0x16.
 */
typedef struct UnitPosAdj {
    int charaId;   /* +0x00: compared with CalcUnitParam.charaId */
    float *front1; /* +0x04 */
    float *front5; /* +0x08 */
    float *back1;  /* +0x0C */
    float *back5;  /* +0x10 */
} UnitPosAdj;

extern UnitPosAdj adjTblPl[];
extern UnitPosAdj adjTblEn[];

extern int unitMotGet(ObjectTask *unit);

/* Defined in src/ov01/calc.c (ov01/tu004), still asm there; it resets every
 * status category of its unit (daddu $16,$4 then calcStatReset(unit, n, ...)). */
extern void calcStatResetAll(ObjectTask *unit);

/* Defined in src/ov01/battle_init.c (ov01/tu003), still asm there. */
extern void decoEffCall(ObjectTask *unit, int effectId);

/* Defined in src/ov01/snd.c (ov01/tu013), declared the same way by
 * src/ov01/battle_init.c and src/ov01/menu.c. */
extern int sndSysSePlay(int soundId);

#endif /* SRC_OV01_UNIT_CMD_H */
