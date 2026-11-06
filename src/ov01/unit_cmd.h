/*
 * TU-local declarations of ov01/tu002 (src/ov01/unit_cmd.c).
 */

#ifndef SRC_OV01_UNIT_CMD_H
#define SRC_OV01_UNIT_CMD_H

#include "shared.h"

/* calcUPGet's return type comes from its definer, ov01/tu004 calc.c. */
#include "ov01/calc.h"
/* BattleUnit (the unit record) lives in the owner header ov01/battle_init.h. */
#include "ov01/actor.h"
/* CmdEntry (the queued-command record) and unitCmdSet live in the owner header. */
#include "ov01/unit_cmd.h"

/* Defined in src/ov01/calc.c (ov01/tu004), not declared in its header. */
extern int calcIdol2Chk(BattleUnit *unit);
extern int dataCidGet(int chara_id);

/* Defined in src/main/act_1.c on its own Actor spelling of the actor. */
extern void ACT_dispose(Actor *actor);

/* Its first parameter is already Actor-typed (src/main/act_2.c). */
extern void *ACT_animGetData(Actor *actor, unsigned int dataId);

/* Defined in a different translation unit (src/main/act_2.c), still asm
 * there; its packed dataId argument matches ACT_animGetData's convention
 * (category << 8 | sub-id), the shape unitWpnMotSet builds before calling
 * it. */
extern void ACT_setMotion(Actor *actor, unsigned int dataId);

/* Defined in src/main/act_2.c on its own Actor spelling, declared the same
 * way in include/main/act_2.h; unitFaceMotSet tail-calls it
 * (j ACT_setMotion2 at 0x00a07270). */
extern void ACT_setMotion2(Actor *actor, int motionId, unsigned int flags);

/* Defined later in this TU; unitCmdAtkEnd, unitCmdDmg and unitCmdBtst call
 * it before its definition with a motion number. */
extern void unitFaceMotSet(BattleUnit *unit, int motion);


/*
 * Bit 0x1000 of the motion actor's moveFlags (Actor.moveFlags, +0x6F0): the
 * move command has reached its target; the move handlers test it (see the
 * moveFlags member above).
 */
#define MOVE_FLAG_REACHED 0x1000

/*
 * tgtFindFirst's search criteria (src/ov01/calc.c, still asm there, its own
 * TgtCmd restated here as a local view -- flagged for review: the single
 * owner definition should live where tgtFindFirst is defined, src/ov01/calc.c,
 * per proposals.md): unitCmdTgtSet builds one of these on its own stack from
 * its queued CmdEntry before calling it.
 */
typedef struct TgtCmd {
    int tecId;          /* +0x00 */
    BattleUnit *target; /* +0x04 */
    int itmId;          /* +0x08 */
    int mode;           /* +0x0C: 3 = item */
} TgtCmd;

extern BattleUnit *tgtFindFirst(BattleUnit *unit, TgtCmd *cmd);

/*
 * The item command record unitCmdItem builds on its stack and hands to
 * unitCmdItemSub: argument and itmId are copied from the queued CmdEntry,
 * target is the unit the item acts on (the sub-routine walks it with
 * tgtFindNext), and mode is 3, the item target kind.
 */
typedef struct ItemCmd {
    int argument; /* +0x00 */
    int target;   /* +0x04 */
    int itmId;    /* +0x08 */
    int mode;     /* +0x0C */
} ItemCmd;

extern void unitCmdItemSub(BattleUnit *unit, ItemCmd *buf);

/* Still asm, src/ov01/battle_init.c; unitCmdTgtSet's only other caller into
 * that TU. */
extern void cmdListCansel(BattleUnit *unit);

/* The three shorts after charaId are CalcUnitParam.weaponId[0..2]. */
extern void sefSetupPlayer(short charaId, short, short, short);
extern void sefSetupEnemy(short charaId);
/* Defined by ov01/tu005 data_unit_org_get.c: returns 0 or 1; charaId is the
 * dataUnitFileGet key. */
extern int dataUnitFileLoad(BattleUnit *unit, int charaId);
extern void dataSndSeRegLoad(BattleUnit *unit);

/* Defined in src/ov01/data_unit_org_get.c, declared there the
 * same way; unitCmdReadWait waits on the first and then calls the second. */
extern int dataCdSync(void);
extern void dataUnitFileLoad2Sub(BattleUnit *unit);

extern void ACT_updateMotion(Actor *actor);
extern float UnduGet(float x, float z);

extern void nmlModelSetTransparency(float transparency);
extern void nmlModelSetToumei(int enabled);
extern void nmlModelSetZwrite(int enabled);

extern void unitCmdExec(BattleUnit *unit);
extern void statEffProc(BattleUnit *unit);
/* Defined in src/ov01/battle_init.c; unitCmdBted calls it before the
 * declaration further down the .c file. */
extern void statEffOff(BattleUnit *unit);
/* Defined in src/ov01/battle_init.c; unitCmdFadeIn calls it before the
 * declaration further down the .c file. */
extern void statEffOn(BattleUnit *unit);

/* UnitWork.flags (include/ov01/battle_init.h) bit that suppresses status-effect ticks for the unit. */
#define UNIT_WORK_STATUS_BLOCKED 0x100u
extern void unitClipSet(BattleUnit *unit);

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

extern int unitCmdStand(BattleUnit *unit);


/*
 * unitCmdMove's own siblings, still asm in this TU; forward-declared for the
 * calls unitCmdMove makes before their definitions appear later in the file.
 */
extern float unitCmdDirSet(BattleUnit *unit);
extern void unitCmdDstSet(BattleUnit *unit, int, int);
extern void unitCmdVASet(BattleUnit *unit, int, int);
extern int unitCmdJump(BattleUnit *unit, int, int, int);
extern int unitCmdHover(BattleUnit *unit, int, int);
extern int unitCmdFloat(BattleUnit *unit, int, int);
extern int unitCmdWarp(BattleUnit *unit, int, int);
extern int unitCmdMoveNext(BattleUnit *unit, int);

/*
 * unitCmdMoveChaRun/unitCmdMovePosRun's own sibling, still asm in this TU;
 * forward-declared for the calls they make to it before its definition
 * appears later in the file.
 */
extern int unitCmdRun(BattleUnit *unit, int, int);

/*
 * unitCmdWarp's own siblings, defined later in this TU; forward-declared for
 * the calls it makes before their definitions appear in the file.
 */
extern int unitTelIn(BattleUnit *unit);
extern int unitTelOut(BattleUnit *unit);
extern void unitMotStandSet(BattleUnit *unit);

/* Defined in a different translation unit (src/ov01/data_unit_org_get.c),
 * still asm there; unitCmdDstSet is the only claimed caller, reading the x/z
 * of the position it returns. */
extern Vector4 *dataPosTblGet(int index);

/* unitCmdDstSet's own sibling, still asm in this TU; forward-declared for the
 * call it makes before its own definition appears later in the file. */
extern float unitColiGet(BattleUnit *unit, BattleUnit *targetUnit);

/*
 * dataMtdRead/dataDefWpnGet are defined in src/ov01/battle_init.c (ov01/tu003)
 * on its own BattleUnit *; unitMtdProc is this TU's own caller.
 * transWepIn/transWepOut are declared by include/ov01/battle_init.h.
 */
extern int dataMtdRead(BattleUnit *unit, int trigger);
extern int dataDefWpnGet(BattleUnit *unit);

/*
 * The 0x10-byte action request unitCmdAtk builds on its stack and hands to
 * unitCmdAtkSub, which forwards it to unitJobTypeAtk; its first word is the
 * technique id dataTecGet resolves.
 */
typedef struct UnitActRequest {
    int id;              /* +0x00: technique id, copied from CmdEntry.argument */
    BattleUnit *target;  /* +0x04: the work actor's target unit */
    int itmId;           /* +0x08: copied from CmdEntry.itmId */
    int mode;            /* +0x0C: 1 = attack request */
} UnitActRequest;

/* Defined in src/ov01/calc.c (ov01/tu004). */
extern int calcEp(BattleUnit *unit, int delta);
extern int calcBp2Tid(BattleUnit *unit, int tecId);
extern int calcTakeEpGet(BattleUnit *unit, int tecId);

/* Defined in src/ov01/data_unit_org_get.c (ov01/tu005); the TU-local view
 * here only needs the pointer, whose bytes unitCmdAtk reads as unsigned
 * shorts. */
extern void *dataSpecDataGet(int cid, int specialId);

extern void unitCmdAtkSub(BattleUnit *unit, UnitActRequest *request);

/* Defined in src/main/sef.c, src/ov01/snd.c and src/ov01/obj.c. */
extern void sefReleaseID(int effectId);
extern void sefDeleteEffect2(SchedulerState *scheduler);
extern void sefDeleteEffectWait(int characterId);
extern int sndSeRegRemove(int id);
extern void objRemove(ObjectTask *task);



/* Defined in src/ov01/data_unit_org_get.c (ov01/tu005), still asm there. */
extern void dataUnitInitSet(BattleUnit *unit, int charaId);

/* Defined in src/ov01/calc.c (ov01/tu004). */
extern int calcWct(BattleUnit *unit);
extern int calcAttWpnChk(int weaponId);

/* Defined in src/ov01/data_unit_org_get.c (ov01/tu005) as a WpnRecord
 * pointer; this TU reads only the byte at its +0x10. */
extern unsigned char *dataWpnGet(int index);

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
extern int unitJobTypeAtk(BattleUnit *unit, UnitActRequest *request, TecJobView *tec);

/*
 * unitCmdExec dispatches the queued command's selector through this table of
 * command handlers (lui/lw %lo(unitCmdTbl) indexed by selector << 2, jalr,
 * 0x00a01dc8..0x00a01dd4); a zero return advances Actor.runFrameCounter.
 */
static int (*unitCmdTbl[49])(BattleUnit *unit);

/* Defined in src/ov01/debug_entry.c, still asm there; it reads its second
 * argument as the command selector (daddu $22,$5 at 0x00a2e530). */
extern void debugCmdPrint(BattleUnit *unit, int selector);

/* Defined in src/ov01/snd.c; unitCmdWake passes its own unit
 * as the first argument (daddu $4,$16,$0 at 0x00a06a54). */
extern void sndSeTransPlay(BattleUnit *unit, int, int);

/* Defined later in this TU; unitCmdMot calls it before its definition. */
extern void unitMtdProc(BattleUnit *unit);

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

extern int unitMotGet(BattleUnit *unit);

/* Defined in src/ov01/calc.c (ov01/tu004), still asm there; it resets every
 * status category of its unit (daddu $16,$4 then calcStatReset(unit, n, ...)). */
extern void calcStatResetAll(BattleUnit *unit);

/* Defined in src/ov01/battle_init.c (ov01/tu003), still asm there. */

/* Defined in src/ov01/snd.c (ov01/tu013), declared the same way by
 * src/ov01/battle_init.c and src/ov01/menu.c. */
extern int sndSysSePlay(int soundId);

#endif /* SRC_OV01_UNIT_CMD_H */
