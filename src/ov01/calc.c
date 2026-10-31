/*
 * OV01 original TU 4: 0x00a10bb8..0x00a191c0 (116 functions)
 */
#include "common.h"
#include "main/xgl_2.h"
#include "ov01/battle_init.h"
#include "calc.h"

/* calcCfEncount's own callees; both are still asm in this TU. */
extern void calcPartyStat(int, int);
extern void menuTimeSet(int);

/*
 * calcStatGet (this TU, defined below beside calcStatGetNorm) as every
 * caller in this file invokes it: a status/flag lookup keyed by category
 * and bit.
 */
extern int calcStatGet(ObjectTask *unit, int category, int flag);

/*
 * The one queued-command word calcTurnStart (this TU) reads
 * through unitCmdPtrGet's return (`lw $3,0x0($2)`), and the two further
 * words calcTurnStart writes through unitCmdSet's second argument
 * (`sw $2,0x0($29)` / `sw $3,0x4($29)` / `sw $0,0x8($29)` at 0x00a12c00,
 * both still asm in src/ov01/unit_cmd.c).
 */
typedef struct CalcCmdEntry CalcCmdEntry;

struct CalcCmdEntry {
    int selector;      /* +0x00 */
    int argument;      /* +0x04 */
    int extra;         /* +0x08: calcTurnStart always sets it to 0 */
    unsigned char unmodeled_0c[8]; /* +0x0C: unwritten by the one evidenced
                                     * call; only its total size (the local's
                                     * stack reservation before unitCmdSet)
                                     * is evidenced here. */
};

extern int calcAp(ObjectTask *unit, int ap);
extern CalcCmdEntry *unitCmdPtrGet(ObjectTask *unit);
extern void unitCmdSet(ObjectTask *unit, CalcCmdEntry *cmd);

/* calcDeadUnitNum: calcSpecChk's own callee, still asm in this TU. */
extern int calcDeadUnitNum(ObjectTask *unit, int flag);

/* calcBattleChk: calcWctDec's own callee, defined below in this TU. */
extern int calcBattleChk(ObjectTask *unit);

/* calcTrg2PE (this TU, defined below): calcDeadUnitNum's own callee. */
extern int calcTrg2PE(ObjectTask *unit, int trgFlags);

/*
 * calcWct/calcStatTurn/rnd: calcTurnEnd's own callees, defined later in
 * this TU (calcWct and rnd below, calcStatTurn still asm).
 */
extern int calcWct(ObjectTask *unit);
extern void calcStatTurn(ObjectTask *unit);
extern int rnd(int max);

/*
 * dataWpnGet's return type, as far as calcAttWpnChk reads it: a byte at
 * +0x10 that it tests for zero/nonzero (data_unit_org_get.c, dataWpnGet's
 * own TU, still asm, owns the rest of the layout).
 */
typedef struct WpnData WpnData;

struct WpnData {
    short attack;                      /* +0x00: calcWpnAtkGet's own base term,
                                       * `lh $19,0x0($2)` (0x00a13438) */
    unsigned char unmodeled_02[0x0F - 0x02];
    signed char apCost;               /* +0x0F: calcWpnEnableChk's `lb $4,0xF($17)`
                                       * against the AP count calcAp keeps */
    u8 equipped;                      /* +0x10: calcAttWpnChk's own check */
    unsigned char unmodeled_11[0x14 - 0x11];
    short tecId;                      /* +0x14: calcWpnEnableChk's `lh $4,0x14($17)`
                                       * passed to dataTecGet/calcTakeEpGet */
    unsigned char unmodeled_16[0x18 - 0x16];
    short dblAct;                     /* +0x18: calcDblActChk requires nonzero */
    unsigned char unmodeled_1a[0x1C - 0x1A];
    short phyDefense;                 /* +0x1C: calcPhyDefGet's accumulated term */
};

extern WpnData *dataWpnGet(int weaponId);

/*
 * The curve descriptor calcSpline (this TU) advances every frame and hands
 * to calcLinerSub/calcBezierSub/calcSplineSub/calcHermiteSub; calcSplineInit
 * sets it up with the control points, their count, the duration in frames
 * as a float and a fixed 0.5 tension, the frame counter starting at zero.
 * The types are the readers' own: calcSpline loads +0x00 as the point
 * array it indexes by 16 (`lw $5,0x0($16)`, `sll $3,$6,4`, `lwc1 $f0,0x0($3)`)
 * and +0x04 as the point count (`lw $4,0x4($16)`, `addiu $6,$4,-0x1`);
 * calcHermiteSub scales its tangents by +0x0C (`lwc1 $f2,0xC($4)`).
 */
typedef struct CalcSpline {
    Vector4 *points;   /* +0x00 */
    int count;         /* +0x04 */
    float duration;    /* +0x08 */
    float tension;     /* +0x0C */
    int frame;         /* +0x10 */
} CalcSpline;

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcCopyParaMake);

/*
 * calcTotalParaMenuSub/calcSkillEquChkAll's (this TU) own fake unit: a
 * stack-local ObjectTask/CalcActorRecord pair whose `work`/`up` chain
 * (calcUPGet's own traversal, 0x00a11010..0x00a1101c) is pointed at a
 * temporary CalcUnitParam-compatible record instead of a live battle unit,
 * so the two functions can reuse calcSkillInit/calcWpnAtkGet/calcPhyDefGet/
 * calcMagDefGet/calcSkillEquChk for a unit that has no ObjectTask;
 * calcPara2Org (below) uses the same pair for calcPara2OrgSub.
 * calcTotalParaMenuSub allocates 0x80/0x110 bytes for the pair
 * (`addiu v0,sp,128`, sp+0x190 for the outer's own end), matching the
 * ObjectTask/CalcActorRecord prefixes it actually writes through plus the
 * task-specific payload xglTaskEntryNext callers leave unwritten here.
 */
typedef struct CalcScratchActor {
    CalcActorRecord actor;      /* +0x00 */
    unsigned char unmodeled_88[0x110 - 0x88];
} CalcScratchActor;

typedef struct CalcScratchUnit {
    ObjectTask task;            /* +0x00 */
    unsigned char unmodeled_14[0x80 - 0x14];
} CalcScratchUnit;

/* unitTblGet (src/ov01/unit_cmd.c): writes the active unit table's base
 * address through its second argument and returns its count. */
extern int unitTblGet(int type, ObjectTask ***table);

/* calcPara2OrgSub (this TU, below): writes one unit's HP/EP back. */
extern void calcPara2OrgSub(ObjectTask *unit);

/*
 * Writes every unit's HP/EP back to its persistent origin record, and the
 * linked unit's through calcTotalParaMenuSub's fake-unit pattern; a unit
 * carrying CalcUnitParam flag 0x40 hands its own currentEp to the linked
 * unit first.
 */
void calcPara2Org(void)
{
    CalcScratchUnit fakeUnit;
    CalcScratchActor fakeActor;
    ObjectTask **table;
    int count;
    int i;

    count = unitTblGet(0, &table);
    for (i = 0; i < count; i++) {
        if (table[i] != 0) {
            calcPara2OrgSub(table[i]);
            if (calcUPGet(table[i])->flags & CALC_UNIT_FLAG_IS_AGWS) {
                fakeUnit.task.work = &fakeActor;
                fakeActor.actor.up = calcUPGet(table[i])->linkedUnit;
                fakeActor.actor.flags = 0;
                fakeActor.actor.up->currentEp = calcUPGet(table[i])->currentEp;
                calcPara2OrgSub(&fakeUnit.task);
            } else if (calcUPGet(table[i])->linkedUnit != 0) {
                fakeUnit.task.work = &fakeActor;
                fakeActor.actor.up = calcUPGet(table[i])->linkedUnit;
                fakeActor.actor.flags = 0;
                calcPara2OrgSub(&fakeUnit.task);
            }
        }
    }
}

/* dataUnitOrgGet (src/ov01/data_unit_org_get.c, still asm), this TU's own
 * private view (see CalcCharParaData, src/ov01/calc.h). */
extern CalcCharParaData *dataUnitOrgGet(int charaId);

void calcPara2OrgSub(ObjectTask *unit) {
    CalcUnitParam *param;
    CalcCharParaData *origin;

    param = calcUPGet(unit);
    origin = dataUnitOrgGet(param->charaId);
    origin->hp = param->currentHp;
    origin->ep = param->currentEp;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcDeadReset);

CalcUnitParam *calcUPGet(ObjectTask *unit)
{
    return ((CalcActorRecord *)unit->work)->up;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHpDmg);

/*
 * calcTotalParaMenuSub (this TU, still asm, 0x00a11128): calcTotalParaMenu's
 * tail-called worker, given the same three arguments plus the per-charaId
 * table below.
 */
extern CalcCharParaData *calcTotalParaMenuSub(int charaId, int *attack, int *defense, unsigned char *table);

/* calcCopyParaMake copies twelve 0x20-byte units into this scratch table;
 * calcTotalParaMenuSub then reads the CalcCharParaData and CalcUnitParam
 * views from the same storage. */
static unsigned char D_00A57CE8[0x180];

CalcCharParaData *calcTotalParaMenu(int charaId, int *attack, int *defense)
{
    return calcTotalParaMenuSub(charaId, attack, defense, D_00A57CE8);
}

/* calcCopyParaMake/calcSkillInit/calcWpnAtkGet/calcPhyDefGet/calcMagDefGet:
 * calcTotalParaMenuSub's own callees, still asm in this TU except
 * calcSkillInit and calcMagDefGet (this TU, below); the latter four each read their argument
 * through calcUPGet, so calcTotalParaMenuSub hands them its own fake unit
 * above. */
extern void calcCopyParaMake(CalcCharParaData *origin, unsigned char *table);
/* calcSkillInit returns int: its exit `addiu $2,$0,0x1` (0x00a11d38). */
extern int calcSkillInit(ObjectTask *unit);
extern int calcWpnAtkGet(ObjectTask *unit, int slot);
extern int calcPhyDefGet(ObjectTask *unit);
extern int calcMagDefGet(ObjectTask *unit);

CalcCharParaData *calcTotalParaMenuSub(int charaId, int *attack, int *defense, unsigned char *table)
{
    CalcScratchUnit fakeUnit;
    CalcScratchActor fakeActor;
    CalcCharParaData *scratch;
    CalcCharParaData *origin;
    int i;

    scratch = (CalcCharParaData *) table;
    fakeUnit.task.work = &fakeActor;
    calcCopyParaMake(dataUnitOrgGet(charaId), table);
    fakeActor.actor.up = (CalcUnitParam *) table;
    calcSkillInit(&fakeUnit.task);

    origin = dataUnitOrgGet(charaId);
    if (scratch->maxHp < origin->hp) {
        origin->hp = scratch->maxHp;
    }
    if (origin->ep > scratch->maxEp) {
        origin->ep = scratch->maxEp;
    }
    if (attack != 0) {
        for (i = 0; i < 3; i++) {
            attack[i] = calcWpnAtkGet(&fakeUnit.task, i);
        }
    }
    if (defense != 0) {
        defense[0] = calcPhyDefGet(&fakeUnit.task);
        defense[1] = calcMagDefGet(&fakeUnit.task);
    }
    return scratch;
}

/*
 * The record calcTakeEpGetMenu builds calcTotalParaMenu's recalculated
 * record into; nothing else in this TU fills the rest of it.
 */
typedef struct CalcTakeEpPara {
    unsigned char unmodeled_00[0x84];
    CalcCharParaData *totalPara; /* +0x84: calcTotalParaMenu's own return value */
    unsigned char unmodeled_88[0x110 - 0x88];
} CalcTakeEpPara;

/*
 * The context block calcTakeEpGetMenu builds for its only call to
 * calcTakeEpGet (this TU, 0x00a15670); nothing else in this TU
 * fills the rest of it.
 */
typedef struct CalcTakeEpArg {
    ObjectTask task;           /* +0x00: task.work (+0x10) is the record above */
    unsigned char unmodeled_14[0x80 - 0x14];
} CalcTakeEpArg;

/* calcTakeEpGet takes a unit: calcWpnEnableChk (this TU) passes its own
 * unit argument (`daddu $4,$20,$0` at 0x00a16474). */
extern int calcTakeEpGet(ObjectTask *unit, int tecId);

/* dataEthGet (src/ov01/data_unit_org_get.c, still asm), this TU's own
 * private view (see CalcEtherStatus, src/ov01/calc.h). */
extern CalcEtherStatus *dataEthGet(int etherId);

int calcTakeEpGetMenu(int charaId, int etherId) {
    CalcTakeEpArg arg;
    CalcTakeEpPara para;

    arg.task.work = &para;
    para.totalPara = calcTotalParaMenu(charaId, 0, 0);
    return calcTakeEpGet(&arg.task, dataEthGet(etherId)->tecId);
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcUseEtherMenu);

/*
 * dataItmGet's return, as far as calcUseItemMenu, calcItmEnableChk,
 * calcDItemChk, calcHitItem and calcDmgItem read it:
 * the item's target-selection flags at +0x08 (`lbu $5,0x8($2)`), handed on
 * to calcTgtUnitTbl, and a flag byte at +0x09 whose bit 0 calcDItemChk
 * tests (`lbu $3,0x9($2)`, `andi 0x1`, `andi 0xFF`) and whose bit 1
 * calcDmgItem tests (`lbu $3,0x9($18)`, `andi $3,$3,0x2`). It is a plain
 * byte, not bit-fields: with the u16 bit-fields at +0x0C the record is
 * 2-aligned and gcc reads +0x09 bit-fields as `lhu 0x8`/`andi 0x100`.
 */
typedef struct ItmTrgView {
    unsigned char unmodeled_00[0x08];
    u8 trgFlags;        /* +0x08 */
    u8 useFlags;        /* +0x09 */
    u8 power;           /* +0x0A: calcHitItem/calcDmgItem's `lbu 0xA` */
    unsigned char unmodeled_0b[0x0C - 0x0B];
    /*
     * +0x0C: the effect bits calcHitItem and calcDmgItem test from one
     * `lhu 0xC` (bit 0 as `andi 0x1`/`andi 0xFFFF`, a bit-field read, the
     * others as a plain `andi` of the same register). Each name is the
     * hit mode calcHitItem returns for the bit, as calcHpDmg (jtbl_00A44360)
     * applies it: mode 1 calcHp(-amount), 2 calcHp(+amount), 3/4 the same
     * through calcEp, 5 and 6 neither; the amount is calcDmgItem's.
     */
    u16 recoverHp : 1;      /* bit 0: mode 2, amount power * 50 */
    u16 recoverEp : 1;      /* bit 1: mode 4, amount power * 10 */
    u16 resultMode6 : 1;    /* bit 2: calcHitItem's mode 6 only */
    u16 recoverHpRate : 1;  /* bit 3: mode 0x802 (low byte 2), amount
                             * maxHp * power / 20 */
    u16 resultMode5 : 1;    /* bit 4: calcHitItem's mode 5 only */
    u16 resultMode5Alt : 1; /* bit 5: calcHitItem's mode 5 only */
    u16 : 2;
    u16 hpToOne : 1;        /* bit 8: with power 1, mode 1 and amount
                             * currentHp - 1; otherwise mode 6, amount 0 */
    u16 : 4;
    /*
     * bit 13: calcDmgItem gives it bit 3's amount (maxHp * power / 20);
     * calcHitItem does not test it, so such an item falls to mode 1
     * (calcHp(-amount)): the name follows that pairing.
     */
    u16 damageHpRate : 1;
} ItmTrgView;

extern ItmTrgView *dataItmGet(int itmId);

/* calcUseItem (this TU, still asm): the item effect of itmId used by user
 * on target; calcUseItemMenu passes no user (`daddu $4,$0,$0`). */
extern int calcUseItem(ObjectTask *user, ObjectTask *target, int itmId);

/*
 * Uses item itmId from the menu on charaId, or on every party member when
 * the item's trgFlags carry CALC_TRG_BIT4 (`andi $3,$3,0x10`, 0x00a11508).
 * Each target's origin record stands in as the unit parameter block of a
 * fake unit, with maxHp/maxEp and statAltMask temporarily replaced by
 * calcTotalParaMenu's recalculated values (`lhu`/`sh` at +0x0/+0x2, the
 * eight halfwords from +0xBC) and restored after calcUseItem.
 * targetIds is set in each branch: both branches compute the address into
 * $16 (`addiu $16,$29,0x190` in the branch delay slot, 0x00a11510), so the
 * PartyAllPartyGet call loads $5 before `daddu $4,$16,$0`; one assignment
 * before the `if` puts `daddu $4,$16,$0` first.
 */
int calcUseItemMenu(int charaId, int itmId)
{
    CalcScratchUnit fakeUnit;
    CalcScratchActor fakeActor;
    int partyIds[8];
    u16 savedStatAlt[8];
    ItmTrgView *item;
    int *targetIds;
    CalcCharParaData *origin;
    CalcCharParaData *total;
    short maxHp;
    short maxEp;
    int count;
    int member;
    int i;

    fakeUnit.task.work = &fakeActor;
    item = dataItmGet(itmId);
    if (item->trgFlags & CALC_TRG_BIT4) {
        targetIds = partyIds;
        count = PartyAllPartyGet(targetIds, 0);
    } else {
        targetIds = partyIds;
        count = 1;
        targetIds[0] = charaId;
    }
    for (member = 0; member < count; member++) {
        origin = dataUnitOrgGet(targetIds[member]);
        total = calcTotalParaMenu(targetIds[member], 0, 0);
        maxHp = origin->maxHp;
        maxEp = origin->maxEp;
        for (i = 0; i < 8; i++) {
            savedStatAlt[i] = origin->statAltMask[i];
        }
        origin->maxHp = total->maxHp;
        origin->maxEp = total->maxEp;
        for (i = 0; i < 8; i++) {
            origin->statAltMask[i] = total->statAltMask[i];
        }
        fakeActor.actor.up = (CalcUnitParam *) origin;
        calcUseItem(0, &fakeUnit.task, itmId);
        origin->maxHp = maxHp;
        origin->maxEp = maxEp;
        for (i = 0; i < 8; i++) {
            origin->statAltMask[i] = savedStatAlt[i];
        }
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcUseItem);

/* calcSkillInitSub (this TU, defined below): one equipped skill id of unit. */
extern void calcSkillInitSub(ObjectTask *unit, int skillId);
extern AccessoryData *dataAccGet(int accessoryId);

/*
 * Hands calcSkillInitSub every skill the unit carries: the skill of each
 * of its three accessories, then its three own skill ids. A zero accessory
 * id is skipped (`lh $3,0x4($2)`/`beqz $3`, 0x00a11cc8..0x00a11ccc), as is
 * a zero skill id from the accessory (`lhu $2,0xE($2)`/`beqz $2`,
 * 0x00a11ce0..0x00a11ce4) or from the unit (`lh $2,0x6($2)`/`beqz $2`,
 * 0x00a11d1c..0x00a11d20). Always returns 1 (`addiu $2,$0,0x1`, 0x00a11d38).
 */
int calcSkillInit(ObjectTask *unit)
{
    CalcUnitParam *up;
    int i;
    int accId;
    int skillId;

    for (i = 0; i < 3; i++) {
        up = calcUPGet(unit);
        accId = up->accessoryId[i];
        if (accId != 0) {
            skillId = dataAccGet(accId)->skillId;
            if (skillId != 0) {
                calcSkillInitSub(unit, skillId);
            }
        }
    }
    for (i = 0; i < 3; i++) {
        up = calcUPGet(unit);
        skillId = up->skillId[i];
        if (skillId != 0) {
            calcSkillInitSub(unit, skillId);
        }
    }
    return 1;
}

/*
 * dataSklGet's return record (src/ov01/data_unit_org_get.c, still asm), as
 * far as calcSkillInitSub and calcSkillSub read it: two halfword bit sets.
 */
typedef struct SkillData SkillData;

struct SkillData {
    u16 typeFlags;   /* +0x00: calcSkillSub's own mask test; calcSkillInitSub's
                      * SkillType bits, the low byte a status category */
    u16 levelFlags;  /* +0x02: calcSkillSub returns bits 0-6 of this field;
                      * calcSkillInitSub's SkillParam bits */
};

extern SkillData *dataSklGet(int skillId);

/*
 * SkillData.typeFlags bits calcSkillInitSub tests, in its own order
 * (`andi $2,$5,0x8000`, `andi $2,$5,0x800`, `andi $2,$5,0x400`).
 */
enum SkillType {
    SKILL_TYPE_STAT_SET = 0x400,     /* tail call to calcStatSet(..., 1) */
    SKILL_TYPE_STAT_PROTECT = 0x800, /* tail call to calcStatProtectSet */
    SKILL_TYPE_PARAM_UP = 0x8000     /* raises the SkillParam members below */
};

/*
 * SkillData.levelFlags bits of a SKILL_TYPE_PARAM_UP skill: each selects the
 * CalcUnitParam member calcSkillInitSub raises by the skill's level (bits
 * 0-6), tested in this order from 0x8000 down to 0x80.
 */
enum SkillParam {
    SKILL_PARAM_MAX_EP = 0x80,        /* maxEp, by level percent, max 99 */
    SKILL_PARAM_MAX_HP = 0x100,       /* maxHp, by level percent, max 9999 */
    SKILL_PARAM_AGILITY = 0x200,      /* agility, max 99 */
    SKILL_PARAM_MAG_DEFENSE = 0x400,  /* magDefense, max 999 */
    SKILL_PARAM_MAG_ATTACK = 0x800,   /* magAttack, max 999 */
    SKILL_PARAM_EVADE = 0x1000,       /* evadeStat, max 99 */
    SKILL_PARAM_HIT = 0x2000,         /* hitStat, max 99 */
    SKILL_PARAM_PHY_DEFENSE = 0x4000, /* phyDefense, max 999 */
    SKILL_PARAM_ATTACK = 0x8000       /* baseAtk, max 999 */
};

/* calcStatSet (this TU, still asm): calcStatSetChk's own arguments;
 * calcStatProtectSet is defined later in this TU. */
extern int calcStatSet(ObjectTask *unit, int category, int flag, int force);
extern void calcStatProtectSet(ObjectTask *unit, int group, int mask);

/*
 * Applies one equipped skill to the unit: a parameter skill raises the
 * members its levelFlags select by its level, each clamped (`slti` 0x3E8,
 * 0x64, 0x2710 then the store of 999, 99, 9999); a protect or status skill
 * tail-calls calcStatProtectSet or calcStatSet with the category in the low
 * byte of typeFlags (`andi $5,$5,0xFF`).
 */
void calcSkillInitSub(ObjectTask *unit, int skillId)
{
    SkillData *skill;
    CalcUnitParam *up;
    int level;

    skill = dataSklGet(skillId);
    up = calcUPGet(unit);
    if (skill->typeFlags & SKILL_TYPE_PARAM_UP) {
        level = skill->levelFlags & 0x7F;
        if (skill->levelFlags & SKILL_PARAM_ATTACK) {
            up->baseAtk += level;
            if (up->baseAtk >= 1000) {
                up->baseAtk = 999;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_PHY_DEFENSE) {
            up->phyDefense += level;
            if (up->phyDefense >= 1000) {
                up->phyDefense = 999;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_HIT) {
            up->hitStat += level;
            if ((signed char) up->hitStat >= 100) {
                up->hitStat = 99;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_EVADE) {
            up->evadeStat += level;
            if ((signed char) up->evadeStat >= 100) {
                up->evadeStat = 99;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_MAG_ATTACK) {
            up->magAttack += level;
            if (up->magAttack >= 1000) {
                up->magAttack = 999;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_MAG_DEFENSE) {
            up->magDefense += level;
            if (up->magDefense >= 1000) {
                up->magDefense = 999;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_AGILITY) {
            up->agility += level;
            if (up->agility >= 100) {
                up->agility = 99;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_MAX_HP) {
            up->maxHp += up->maxHp * level / 100;
            if (up->maxHp >= 10000) {
                up->maxHp = 9999;
            }
        }
        if (skill->levelFlags & SKILL_PARAM_MAX_EP) {
            up->maxEp += up->maxEp * level / 100;
            if (up->maxEp >= 100) {
                up->maxEp = 99;
            }
        }
    } else if (skill->typeFlags & SKILL_TYPE_STAT_PROTECT) {
        calcStatProtectSet(unit, skill->typeFlags & 0xFF, skill->levelFlags);
    } else if (skill->typeFlags & SKILL_TYPE_STAT_SET) {
        calcStatSet(unit, skill->typeFlags & 0xFF, skill->levelFlags, 1);
    }
}

/* calcSkillSub/AccessoryData.skillId (both this TU, defined below): the
 * per-skill type/level mask test and the accessory's granted skill id. */
extern int calcSkillSub(ObjectTask *unit, int skillId, int typeMask, int levelMask);
extern AccessoryData *dataAccGet(int accessoryId);

/*
 * The sum of calcSkillSub over the same skills calcSkillInit walks: a zero
 * accessory id is skipped (`lh $3,0x4($2)`/`beqz $3`, 0x00a12050..
 * 0x00a12054), as is a zero skill id from the accessory (`beqz $2`,
 * 0x00a12074) or from the unit (`lh $2,0x6($2)`/`beqz $2`, 0x00a120b4..
 * 0x00a120c0); such slots add nothing.
 */
int calcSkill(ObjectTask *unit, int kind, int mask)
{
    CalcUnitParam *up;
    int result;
    int i;
    int accId;
    int skillId;

    result = 0;
    for (i = 0; i < 3; i++) {
        up = calcUPGet(unit);
        accId = up->accessoryId[i];
        if (accId != 0) {
            skillId = dataAccGet(accId)->skillId;
            if (skillId != 0) {
                result += calcSkillSub(unit, skillId, kind, mask);
            }
        }
    }
    for (i = 0; i < 3; i++) {
        up = calcUPGet(unit);
        skillId = up->skillId[i];
        if (skillId != 0) {
            result += calcSkillSub(unit, skillId, kind, mask);
        }
    }
    return result;
}

int calcSkillSub(ObjectTask *unit, int skillId, int typeMask, int levelMask) {
    SkillData *skill;
    unsigned short levelFlags;
    int result;

    skill = dataSklGet(skillId);
    calcUPGet(unit);
    result = 0;
    if (skill->typeFlags & typeMask) {
        levelFlags = skill->levelFlags;
        result = (levelFlags & levelMask) != 0 ? (levelFlags & 0x7F) : 0;
    }
    return result;
}

int calcAccEquChk(ObjectTask *unit, int accId)
{
    CalcUnitParam *param;
    int i;

    for (i = 0; i < 3; i++)
    {
        param = calcUPGet(unit);
        if (accId == param->accessoryId[i])
        {
            return 1;
        }
    }
    return 0;
}

/* dataAccGet (src/ov01/data_unit_org_get.c, still asm), this TU's own
 * private view (see AccessoryData, src/ov01/calc.h). */
extern AccessoryData *dataAccGet(int accessoryId);

int calcSkillEquChk(ObjectTask *unit, int skillId)
{
    CalcUnitParam *param;
    int i;
    short accessoryId;

    for (i = 0; i < 3; i++)
    {
        param = calcUPGet(unit);
        accessoryId = param->accessoryId[i];
        if (accessoryId != 0)
        {
            if (skillId == dataAccGet(accessoryId)->skillId)
            {
                return 1;
            }
        }
    }

    for (i = 0; i < 3; i++)
    {
        param = calcUPGet(unit);
        if (skillId == param->skillId[i])
        {
            return 1;
        }
    }

    return 0;
}

/* calcSkillEquChk: calcSkillEquChkAll's own callee (this TU). Its second
 * call below hands it the same fake unit calcMagDefGet (this TU) takes. */
extern int calcSkillEquChk(ObjectTask *unit, int skillId);

int calcSkillEquChkAll(int skillId)
{
    CalcScratchUnit fakeUnit;
    CalcScratchActor fakeActor;
    ObjectTask **table;
    ObjectTask *unit;
    int count;
    int i;
    int found;

    i = 0;
    count = unitTblGet(0, &table);
    found = 0;
    if (count > 0) {
        for (; i < count; i++) {
            unit = table[i];
            if (unit != 0) {
                found = 1;
                if (calcSkillEquChk(unit, skillId) == 0) {
                    if (calcUPGet(table[i])->flags & CALC_UNIT_FLAG_IS_AGWS) {
                        fakeUnit.task.work = &fakeActor;
                        fakeActor.actor.up = calcUPGet(table[i])->linkedUnit;
                        if (calcSkillEquChk(&fakeUnit.task, skillId) != 0) {
                            return 1;
                        }
                    }
                } else {
                    return found;
                }
            }
        }
        return 0;
    }
    return found;
}

int calcAp(ObjectTask *unit, int ap)
{
    int current = calcUPGet(unit)->dblActKind;

    if (ap < 0 && current < -ap) {
        return 0;
    }
    current += ap;
    if (current > 6) {
        current = 6;
    }
    calcUPGet(unit)->dblActKind = current;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHp);

int calcEp(ObjectTask *unit, int delta)
{
    int ep;

    ep = (short) calcUPGet(unit)->currentEp;
    ep += delta;
    if (ep < 0)
    {
        ep = 0;
    }
    else if (calcUPGet(unit)->maxEp < ep)
    {
        ep = calcUPGet(unit)->maxEp;
    }
    calcUPGet(unit)->currentEp = ep;
    return 1;
}

/*
 * calcBc (this TU, defined below) is published as `int calcBc(BattleUnit *unit,
 * int delta)` and calcBp's extern was published as `(ObjectTask *unit, ...)`;
 * BattleUnit is include/ov01/battle_init.h's record. The cast at the call
 * bridges those two published declarations, as calcBc/calcBoost cast the other
 * way for calcUPGet.
 */
int calcBc(BattleUnit *unit, int delta);

/*
 * Adds bp to the unit's boost-point gauge while its boost count is below
 * the limit; every 50 points (`slti $4,$3,0x32`, `div $0,$3,$2` with
 * $2 = 0x32) become one boost through calcBc, and a full count empties the
 * gauge. Returns 1 on every path (`addiu $2,$0,0x1` at 0x00a12630), so
 * the extern spelled `void calcBp(ObjectTask *unit, int tid)` while it was
 * asm is now this int definition; calcBp2Tid discards the result.
 */
int calcBp(ObjectTask *unit, int bp)
{
    CalcUnitParam *up;

    up = calcUPGet(unit);
    if (up->bcValue < up->bcLimit) {
        up->bpGauge += bp;
        if (up->bpGauge >= 50) {
            up->bpGauge %= 50;
            calcBc((BattleUnit *) unit, 1);
        }
    }
    if (up->bcValue >= up->bcLimit) {
        up->bpGauge = 0;
    }
    return 1;
}

/*
 * dataTecGet's return, as far as calcBp2Tid reads it: a byte at +0x11
 * selecting the BP timeline (data_unit_org_get.c, dataTecGet's own TU,
 * still asm, owns the rest of the layout).
 */
typedef struct TecKindView TecKindView;

struct TecKindView {
    /*
     * +0x00: on origin/master this view (calc.c's own, private bounded view
     * of dataTecGet's record; no header owns it) begins with
     * `unsigned char unmodeled_00[1];`. That one byte is named here, same
     * size and offset, because calcAtkChk (this TU, now C) reads it:
     * `lbu $3,0x0($2)`/`andi $16,$3,0x3` (0x00a16024..0x00a16028) for its
     * range class (TEC_ATTR_RANGE, defined beside calcAtkChk). It is still
     * needed by that function and by nothing else.
     */
    u8 attr;
    signed char epCost;     /* +0x01: calcTakeEpGet's base term, `lb $16,0x1($2)` */
    u16 flags;              /* +0x02: bit 0x800 rules out a counter */
    signed char effectType; /* +0x04: calcStatClr's `lb $4,0x4($18)`, 4 clears statuses */
    /*
     * +0x05: calcStat's `lb $3,0x5($2)`: 1 makes the hit double *damage
     * while the target carries status (0, 0x20), which calcStat then resets.
     */
    signed char doubleDamageKind;
    /*
     * +0x06: calcPowLevGet's base term, `lbu $18,0x6($2)` (0x00a13684,
     * unsigned byte load). TecRecord (src/ov01/data_unit_org_get.c) owns
     * the rest of this layout and does not yet name this byte or the next
     * one either (its unmodeled_00[0x11] covers both); see
     * OUTDIR/proposals.md for the same-size split that would.
     */
    u8 powLevBase;
    signed char agility;    /* +0x07: calcPlusAglGet's base term, `lb $18,0x7($2)` */
    unsigned char unmodeled_08[0x0C - 0x08];
    signed char statCategory; /* +0x0C: calcStatClr's `lb $3,0xC($18)` */
    unsigned char unmodeled_0d[0x0E - 0x0D];
    u16 statMask;           /* +0x0E: calcStatClr's `lhu $2,0xE($18)` */
    unsigned char unmodeled_10[0x11 - 0x10];
    /*
     * +0x11: calcBp2Tid's timeline selector. calcBp2Tid.s (0x00a1267c)
     * reads it with `lb $2,0x11($2)` (signed byte load), hence `signed char`.
     */
    signed char kind;
};

/*
 * Bits of TecKindView.flags (+0x02, `lhu $3,0x2($2)`/`andi $3,$3,<bit>`),
 * each named by the effect its one test has; the field stays u16.
 */
enum TecFlag {
    /* futtobiChk (`lhu $3,0x2($5)`/`andi $3,$3,0x100`, 0x00a14600..0x00a14604,
     * on its technique argument): clear, no knockback (result 0). */
    TEC_FLAG_KNOCKBACK = 0x100,
    /* calcHitRate (0x00a130bc): set, the hit roll is skipped (result 0). */
    TEC_FLAG_NO_HIT_ROLL = 0x200,
    /* calcHitRate (0x00a1310c): set, the first roll's result 2 is blocked. */
    TEC_FLAG_NO_HIT_RESULT_2 = 0x400
};

extern TecKindView *dataTecGet(int tecId);
extern int menuTimeGet(void);
extern const char D_00A443D8[]; /* "** BP up: EventTimer G (%d)\n" */

int calcBp2Tid(ObjectTask *unit, int tecId)
{
    BattleActor *actor;
    int kind;
    int tid;

    actor = (BattleActor *) unit->work;
    if (actor->flags & CALC_ACTOR_FLAG_ENEMY) {
        return 0;
    }

    kind = dataTecGet(tecId)->kind;
    if (kind == 0) {
        tid = 4;
    } else if (kind == 1) {
        tid = 6;
    } else if (kind == 2) {
        tid = 10;
    } else {
        tid = 0;
    }

    if (menuTimeGet() == 2) {
        tid *= 3;
        printf(D_00A443D8, tid);
    }

    calcBp(unit, tid);
    return 1;
}

int calcBc(BattleUnit *unit, int delta)
{
    int bc;

    bc = calcUPGet((ObjectTask *) unit)->bcValue;
    bc += delta;
    if (bc < 0)
    {
        bc = 0;
    }
    else if (calcUPGet((ObjectTask *) unit)->bcLimit < bc)
    {
        bc = calcUPGet((ObjectTask *) unit)->bcLimit;
    }
    calcUPGet((ObjectTask *) unit)->bcValue = bc;
    return 1;
}

int calcCounter(ObjectTask *unit, int tecId)
{
    int rate;

    if (dataTecGet(tecId)->flags & 0x800) {
        return 0;
    }
    rate = calcUPGet(unit)->counterRate;
    rate += calcSkill(unit, 0x4000, 0x4000);
    if (rate > 100) {
        rate = 100;
    }
    return rnd(100) < rate;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcBoostChk);

extern int *actStockGet(void);
/* calcBc returns int, not void: its original asm sets $2 = 1
 * (`addiu $2,$0,0x1`, build/ov01/asm/nonmatchings/ov01/calc/calcBc.s) right
 * before the epilogue on every path. calcBoost (below, this TU) is the only
 * caller and discards the result. */
int calcBc(BattleUnit *unit, int delta);
void decoEffCall(BattleUnit *unit, int effectId);
int sndSysSePlay(int soundId);

int calcBoost(BattleUnit *unit, int mode) {
    CalcActorRecord *work;

    if (mode == 1) {
        calcBc(unit, -1);
    }
    work = (CalcActorRecord *) ((ObjectTask *) unit)->work;
    work->flags |= 0x400;
    ((CalcActStock *) actStockGet())->slot[mode] = (ObjectTask *) unit;
    decoEffCall(unit, 0x64);
    return sndSysSePlay(9);
}

int calcIdol2Chk(ObjectTask *unit)
{
    int hp = (short) calcUPGet(unit)->currentHp;

    if ((((BattleActor *) unit->work)->flags & CALC_ACTOR_FLAG_ENEMY) == 0) {
        if (hp < calcUPGet(unit)->maxHp / 5) {
            return 1;
        }
    }
    return 0;
}

int calcTuigekiChk(ObjectTask *unit)
{
    return calcStatGet(unit, 7, 0x80) != 0;
}

int calcSpecChk(ObjectTask *unit, int specId)
{
    int result;

    if (specId == 0x34 && calcDeadUnitNum(unit, 0x40) <= 0)
    {
        return 0;
    }

    if (specId != 0x5A || (result = calcStatGet(unit, 7, 2)) != 0)
    {
        if (specId != 0x59 || (result = calcStatGet(unit, 7, 1)) != 0)
        {
            result = 1;
        }
    }

    return result;
}

extern int calcTgtUnitTbl(ObjectTask *unit, int trgFlags, ObjectTask **table);
extern int cfEventGet(void);
extern int dataItmBoxChk(int itmId);

int calcDItemChk(ObjectTask *unit, int itmId)
{
    if (calcUPGet(unit)->dblActKind >= 6
        && (dataItmGet(itmId)->trgFlags & CALC_TRG_BIT4) == 0
        && dataItmBoxChk(itmId) >= 2
        && (dataItmGet(itmId)->useFlags & 1)) {
        return 1;
    }
    return 0;
}

void calcTurnStart(ObjectTask *unit)
{
    int ap = 4;
    int apForSelect;
    CalcActorRecord *work = (CalcActorRecord *) unit->work;

    if (calcStatGet(unit, 2, 0x10) != 0 || calcStatGet(unit, 6, 0x1000) != 0)
    {
        ap = 6;
    }

    if (calcStatGet(unit, 0, 0x40) != 0 || calcStatGet(unit, 4, 0x20) != 0)
    {
        ap /= 2;
    }

    apForSelect = ap;
    calcAp(unit, calcStatGet(unit, 7, 0x20) ? apForSelect + 1 : ap);

    if (((CalcActorRecord *) unit->work)->flags & 0x800)
    {
        CalcCmdEntry *cmd = unitCmdPtrGet(unit);

        if (cmd->selector == 0)
        {
            CalcCmdEntry newCmd;

            newCmd.selector = 0xD;
            newCmd.argument = 0x1C;
            newCmd.extra = 0;
            unitCmdSet(unit, &newCmd);
        }

        work->flags &= ~0x800;
    }
}

void calcTurnEnd(ObjectTask *unit)
{
    CalcUnitParam *up;
    short wct;

    ((CalcActorRecord *) unit->work)->flags &= ~0x6418;

    up = calcUPGet(unit);
    calcUPGet(unit)->waitCount = wct = calcWct(unit);
    up->waitCountBase = wct;

    calcUPGet(unit)->turnBias = 0;

    calcStatTurn(unit);
}

int calcWct(ObjectTask *unit)
{
    int base;
    int accum = 0;

    base = (calcUPGet(unit)->turnBias - calcUPGet(unit)->agility) * 9 + 0xB4;

    if (calcStatGet(unit, 0, 0x1000) != 0 || calcStatGet(unit, 4, 0x800) != 0)
    {
        accum = base / 2;
    }

    if (calcStatGet(unit, 2, 0x100) != 0 || calcStatGet(unit, 6, 0x4000) != 0)
    {
        accum -= base / 4;
    }

    if (calcStatGet(unit, 2, 0x80) != 0 || calcStatGet(unit, 6, 0x2000) != 0)
    {
        accum -= base / 2;
    }

    return base + accum;
}

int calcWctDec(ObjectTask *unit)
{
    int wait = calcUPGet(unit)->waitCount;

    if (calcBattleChk(unit) != 0)
    {
        int decremented = wait - ((rnd(100) < 30) ? 1 : 0);

        wait = decremented - 1;
        if (wait < 0)
        {
            wait = 0;
        }
    }

    calcUPGet(unit)->waitCount = wait;
    return wait;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHitMotChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHitDmg);

/*
 * The battle command tgtFindFirst (this TU, still asm) passes on to
 * tgtFindChk, as far as either reads it: tgtFindFirst resolves +0x00
 * through dataTecGet and +0x08 through dataItmGet, takes the target from
 * +0x04, and branches on mode 3 (an item, as in sndConvSpid's mode 3).
 * calcHitRate (below) reads the same +0x00 (`lw $4,0x0($17)` before
 * dataTecGet) and +0x04 (`lw $4,0x4($17)` before calcUPGet).
 */
typedef struct TgtCmd {
    int tecId;          /* +0x00 */
    ObjectTask *target; /* +0x04 */
    int itmId;          /* +0x08 */
    int mode;           /* +0x0C: 3 = item */
} TgtCmd;

/* calcHitParaGet (defined below): the attacker's hit rate. */
int calcHitParaGet(ObjectTask *unit);

/*
 * The attacker's hit rate (calcHitParaGet) less the target's evadeStat gives
 * margin; the evade is halved when the target carries the statuses tested
 * first, or raised by a quarter when the attacker carries the third one.
 * Early returns: 0 when the target's HP is 0; 1 when its actor carries
 * CALC_ACTOR_FLAG_BIT11; 0 when the technique carries TEC_FLAG_NO_HIT_ROLL.
 * Then a first roll, rnd(100) - margin (`jal rnd` 0x00a130cc, `subu`
 * 0x00a130d4), compared with 90 (`slti $2,$2,0x5A`, 0x00a130d8): at 90 or
 * more it returns 2 unless the target carries CALC_UNIT_FLAG_NO_HIT_RESULT_2,
 * the technique carries TEC_FLAG_NO_HIT_RESULT_2, or calcBattleChk of the
 * target is 0. Failing that, a second, independent roll (`jal rnd`
 * 0x00a13128) compared with 85 (`slti $2,$2,0x55`, 0x00a13134): at 85 or
 * more it returns 1 unless the target carries CALC_UNIT_FLAG_NO_HIT_RESULT_1
 * or calcBattleChk is 0; otherwise 0.
 */
int calcHitRate(ObjectTask *unit, TgtCmd *cmd)
{
    int hitRate;
    int evade;
    int margin;

    hitRate = calcHitParaGet(unit);
    evade = (signed char) calcUPGet(cmd->target)->evadeStat;
    if (calcStatGet(cmd->target, 0, 0x200) != 0 || calcStatGet(cmd->target, 4, 0x100) != 0) {
        evade /= 2;
    } else if (calcStatGet(unit, 2, 0x20) != 0) {
        evade += evade / 4;
    }

    if ((short) calcUPGet(cmd->target)->currentHp == 0) {
        return 0;
    }
    /* target->work points to its BattleActor record. */
    if (((BattleActor *) cmd->target->work)->flags & CALC_ACTOR_FLAG_BIT11) {
        return 1;
    }
    if (dataTecGet(cmd->tecId)->flags & TEC_FLAG_NO_HIT_ROLL) {
        return 0;
    }

    margin = hitRate - evade;
    if (rnd(100) - margin >= 90) {
        if (!(calcUPGet(cmd->target)->flags & CALC_UNIT_FLAG_NO_HIT_RESULT_2)
            && !(dataTecGet(cmd->tecId)->flags & TEC_FLAG_NO_HIT_RESULT_2)
            && calcBattleChk(cmd->target)) {
            return 2;
        }
    }
    if (rnd(100) - margin >= 85
        && !(calcUPGet(cmd->target)->flags & CALC_UNIT_FLAG_NO_HIT_RESULT_1)
        && calcBattleChk(cmd->target)) {
        return 1;
    }
    return 0;
}

int calcHitParaGet(ObjectTask *unit) {
    int hitRate;

    hitRate = (signed char) calcUPGet(unit)->hitStat;
    if (calcStatGet(unit, 0, 0x400) != 0 || calcStatGet(unit, 4, 0x200) != 0) {
        hitRate /= 2;
    } else if (calcStatGet(unit, 2, 0x40) != 0) {
        hitRate += hitRate / 4;
    }
    return hitRate;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHit);

/*
 * dataAttGet's return (src/ov01/data_unit_org_get.c, still opaque there:
 * `typedef struct AttRecord { unsigned char opaque[0x0E]; } AttRecord;`),
 * this TU's own bounded view: calcWpnAtkGet's attachment attack term,
 * `lh $19,0x0($2)` (0x00a13410). AttRecord does not name this field and is
 * not published, so this is a second definition of the same record; see
 * OUTDIR/proposals.md (a same-size split of AttRecord's `opaque[0x0E]` names
 * it on the owner side instead).
 */
typedef struct CalcAttData {
    short attack; /* +0x00 */
} CalcAttData;

extern CalcAttData *dataAttGet(int attachmentId);
extern const char D_00A44450[]; /* "** calcWpnAtkGet: Att equip err! %d\n" */
extern const char D_00A44478[]; /* "** calcWpnAtkGet: wpnAtkAdj=%d\n" */

/*
 * calcWpnAtkGet's attack-boost percentage, the word at +0x17C of the unit's
 * CalcUnitParam: a nonzero value gates the whole bonus block (`lw $3,0x17C($2)`,
 * 0x00a13444) and is later divided into atk*pct/100 (`lw $3,0x17C($2)`,
 * 0x00a134cc). CalcUnitParam (above) ends at 0x17C (effectUnit at +0x158 plus
 * statEffUnits[8] reaches exactly +0x17C), so a trailing member would change
 * the struct's size; that extent/alignment decision is not this function's to
 * make (see OUTDIR/proposals.md), so the term is read through this named
 * offset, not a member.
 */
#define CALC_PARAM_ATK_BOOST_PCT_OFF 0x17C

int calcWpnAtkGet(ObjectTask *unit, int slot)
{
    BattleActor *actor;
    short weaponId;
    short attachmentId;
    int atk;

    atk = 0;
    if (slot != -1)
    {
        weaponId = calcUPGet(unit)->weaponId[slot];
        if (weaponId != 0)
        {
            if (calcAttWpnChk(weaponId))
            {
                attachmentId = calcUPGet(unit)->attachmentId[slot];
                if (attachmentId != 0)
                {
                    atk = dataAttGet(attachmentId)->attack;
                }
                else
                {
                    printf(D_00A44450, weaponId);
                }
            }
            else
            {
                atk = dataWpnGet(weaponId)->attack;
            }
        }

        if (*(int *)((char *)calcUPGet(unit) + CALC_PARAM_ATK_BOOST_PCT_OFF) != 0)
        {
            /* An extra calcUPGet(unit) call the original makes and
             * discards (`jal calcUPGet`/`daddu $4,$17,$0`, 0x00a13450)
             * right before reading unit->work below. */
            calcUPGet(unit);
            actor = (BattleActor *) unit->work;
            if (!(actor->flags & CALC_ACTOR_FLAG_ENEMY))
            {
                if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS))
                {
                    /* The 0xBB..0xC2 charaId range futtobiChk and
                     * calcPlusAglGet exclude the same way (`slti $3,$3,0xBB`
                     * at 0x00a13490, `slti $3,$3,0xC3` at 0x00a134a8). */
                    if (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN ||
                        calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END)
                    {
                        atk = calcUPGet(unit)->baseAtk;
                    }
                }
            }

            atk += atk * (*(int *)((char *)calcUPGet(unit) + CALC_PARAM_ATK_BOOST_PCT_OFF)) / 100;
            printf(D_00A44478, atk);
        }
    }
    return atk;
}

int calcPhyDefGet(ObjectTask *unit)
{
    int def;
    int i;
    short accessoryId;
    short weaponId;

    def = 0;
    for (i = 0; i < 3; i++)
    {
        accessoryId = calcUPGet(unit)->accessoryId[i];
        if (accessoryId != 0)
        {
            def += dataAccGet(accessoryId)->phyDefense;
        }
    }

    for (i = 0; i < 3; i++)
    {
        weaponId = calcUPGet(unit)->weaponId[i];
        if (weaponId != 0)
        {
            def += dataWpnGet(weaponId)->phyDefense;
        }
    }

    return def;
}

/* dataAccGet (src/ov01/data_unit_org_get.c, still asm), this TU's own
 * private view (see AccessoryData, src/ov01/calc.h). */
extern AccessoryData *dataAccGet(int accessoryId);

int calcMagDefGet(ObjectTask *unit)
{
    int magicDef;
    int i;
    short accessoryId;

    magicDef = 0;
    for (i = 0; i < 3; i++) {
        accessoryId = calcUPGet(unit)->accessoryId[i];
        if (accessoryId != 0) {
            magicDef += dataAccGet(accessoryId)->magicDefense;
        }
    }
    return magicDef;
}

/*
 * dataSpecDataGet's return (src/ov01/data_unit_org_get.c, this TU's own
 * bounded view): calcPowLevGet adds the halfword at +0x04 (`lh $2,0x4($2)`,
 * 0x00a13710) to the technique's power level; calcPlusAglGet adds the
 * halfword at +0x06 (`lh $2,0x6($2)`, 0x00a13828) to the technique's
 * agility. dataSpecDataGet's own TU already names this record
 * (src/ov01/data_unit_org_get.h's `PlSpecialSlot`, an 0xC-byte id/unmodeled
 * pair) but does not yet name these two fields and is not published to
 * calc.c (its header includes this one); see OUTDIR/proposals.md for the
 * same-size split of `PlSpecialSlot.unmodeled_2[10]` that would.
 */
typedef struct CalcSpecSlot {
    short id;                      /* +0x00 */
    unsigned char unmodeled_02[0x04 - 0x02];
    short powLevBonus;              /* +0x04: calcPowLevGet's bonus term */
    short agility;                  /* +0x06: calcPlusAglGet's bonus term */
    unsigned char unmodeled_08[0x0C - 0x08];
} CalcSpecSlot;

extern CalcSpecSlot *dataSpecDataGet(int charaId, int specialId);

/*
 * dataPlChaGet's return (src/ov01/data_unit_org_get.c, this TU's own
 * bounded view): calcPowLevGet reads a short at +0x9A (`lh $4,0x9A($2)`,
 * 0x00a1373c) for tecId 0x262/0x263, divides it by 5 and adds it to the
 * power level. dataPlChaGet's own TU already names the owning record
 * (data_unit_org_get.h's `PlCharacter`, eight 0xC-byte `PlSpecialSlot`
 * entries from +0x48), so +0x9A is that same special[6]'s own +0x0A
 * halfword (0x48 + 6*0xC + 0xA = 0x9A) -- a member of the same 0xC-byte
 * record CalcSpecSlot views above. CalcSpecSlot names only +0x04 and +0x06
 * of it and leaves its +0x0A halfword unmodeled (inside unmodeled_08), so
 * this view is the only place that halfword is named here. Naming it in
 * `PlSpecialSlot` and reading it as `special[6]` there removes this view
 * entirely. This TU
 * cannot include data_unit_org_get.h (that header includes this one), so it
 * keeps the plain offset here and says what it overlaps.
 */
typedef struct CalcPlCharaView {
    /*
     * +0x00/+0x0C/+0x10/+0x14: the four running totals calcResultSub (this
     * TU) adds a battle's share to (`lw`/`sw` at 0x0, 0xC, 0x10, 0x14 of
     * $18, clamped to 9999999 and 9999), inside PlCharacter's unmodeled_0.
     */
    int exp;
    unsigned char unmodeled_04[0x0C - 0x04];
    int techPoint;
    int etherPoint;
    int skillPoint;
    unsigned char unmodeled_18[0x9A - 0x18];
    short special6PowLevTerm; /* +0x9A: PlCharacter.special[6]'s own +0x0A */
} CalcPlCharaView;

extern CalcPlCharaView *dataPlChaGet(int cid);

int calcPowLevGet(ObjectTask *unit, int tecId)
{
    BattleActor *actor;
    CalcSpecSlot *spec;
    int powLev;

    powLev = dataTecGet(tecId)->powLevBase;
    if (dataTecGet(tecId)->kind == 2) {
        actor = (BattleActor *) unit->work;
        if (!(actor->flags & CALC_ACTOR_FLAG_ENEMY)) {
            if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
                /* The same 0xBB..0xC2 charaId exclusion as calcWpnAtkGet
                 * (`slti` against 0xBB at 0x00a136d0, 0xC3 at 0x00a136e8). */
                if (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN ||
                    calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END) {
                    spec = dataSpecDataGet(calcUPGet(unit)->charaId, tecId);
                    if (spec != 0) {
                        powLev += spec->powLevBonus;
                    }
                }
            }
        }
    }
    /* Technique ids 0x262 and 0x263 (`addiu $2,$17,-0x262`/`sltiu $2,$2,0x2`,
     * 0x00a13718..0x00a1371c); what they are is not established. */
    if ((unsigned int) (tecId - 0x262) < 2) {
        powLev += dataPlChaGet(calcUPGet(unit)->charaId)->special6PowLevTerm / 5;
    }
    return powLev;
}

int calcPlusAglGet(ObjectTask *unit, int tecId)
{
    BattleActor *actor;
    CalcSpecSlot *spec;
    int agl;

    agl = dataTecGet(tecId)->agility;
    if (dataTecGet(tecId)->kind == 2) {
        actor = (BattleActor *) unit->work;
        if (!(actor->flags & CALC_ACTOR_FLAG_ENEMY)) {
            if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
                if (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN ||
                    calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END) {
                    spec = dataSpecDataGet(calcUPGet(unit)->charaId, tecId);
                    if (spec != 0) {
                        agl += spec->agility;
                    }
                }
            }
        }
    }
    return agl;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcDmg);

/*
 * futtobiChk: whether a hit with tec knocks unit back ("futtobi") one
 * row. Only a technique carrying TEC_FLAG_KNOCKBACK, an ordinary charaId outside
 * CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN..END on the CALC_ACTOR_FLAG_ENEMY-clear
 * side without CALC_UNIT_FLAG_IS_AGWS, standing in a front position
 * (posId < 4) whose row behind (posId + 3) no unit of the table occupies.
 */
int futtobiChk(ObjectTask *unit, TecKindView *tec)
{
    BattleActor *actor;
    ObjectTask **table;
    int count;
    int posId;
    int i;

    if (!(tec->flags & TEC_FLAG_KNOCKBACK)) {
        return 0;
    }
    actor = (BattleActor *) unit->work;
    if ((actor->flags & CALC_ACTOR_FLAG_ENEMY) || (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
        return 0;
    }
    if (calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN &&
        calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_END) {
        return 0;
    }
    posId = calcUPGet(unit)->posId;
    if (posId >= 4) {
        return 0;
    }
    count = unitTblGet(0, &table);
    posId += 3;
    for (i = 0; i < count; i++) {
        if (table[i] != 0) {
            if (calcUPGet(table[i])->posId == posId) {
                return 0;
            }
        }
    }
    return 1;
}

/*
 * The hit record calcHitDmg (this TU, still asm) fills and hands to
 * calcDmg, which passes it on to calcLinePos; calcLinePos reads only the
 * byte at +0x00 (`lbu $16,0x0($6)`) and goes on when it is 1; calcStatClr
 * records the status category and bits the technique clears.
 */
typedef struct CalcHitInfo {
    /*
     * +0x00: calcRevHit's packed word, the hit mode in the low byte and
     * the side above it. A word: calcHitDmg stores it with `sw $2,0x0($17)`
     * and calcStat hands it whole to calcRevHit (`lw $4,0x0($18)`) and
     * stores the answer back; calcLinePos and calcStat read the mode alone
     * (`lbu`, `& 0xFF`).
     */
    int result;
    unsigned char unmodeled_04[0x0C - 0x04];
    int setCategory;   /* +0x0C: calcHitItem's `sw $3,0xC($16)`, 8 = none */
    int setMask;       /* +0x10: calcHitItem's `sw $0,0x10($16)` */
    /*
     * +0x14/+0x18: calcStatClr's `sw $3,0x14($16)` / `sw $2,0x18($16)`. Read
     * together: category 8 with a zero mask records no clearing; category 8
     * with mask 0xFFFF clears every bit of every category; any other category
     * clears that category's statMask bits.
     */
    int clearCategory;
    int clearMask;
} CalcHitInfo;

/* calcStatReset (this TU, defined below): calcStat's own callee. */
extern void calcStatReset(ObjectTask *unit, int category, int mask, int useAltMask);

/*
 * Applies cmd->target's statuses to a hit of mode hit->result & 0xFF and
 * its *damage: a technique with doubleDamageKind 1 doubles the damage
 * and uses up status (0, 0x20); status (0, 0x10) reverses modes 1 and 2 (calcRevHit);
 * (0, 8) halves mode 2, (1, 0x40) mode 4; (7, 0x40) moves mode 1's damage
 * by 20% per two units of its last decimal digit, down for odd damage, up
 * for even; (3, 2) cancels mode 3. A mode-1 hit also clears
 * CALC_ACTOR_FLAG_BIT4 from a target calcBattleChk refuses, ends statuses
 * (1, 0x2000) and (5, 0x2000), and ends (5, 0x4000) when rnd(100) < 50 (50 of 101 outcomes).
 */
/* calcRevHit (this TU, defined below): calcStat's own callee. */
extern int calcRevHit(int hitMode);

int calcStat(ObjectTask *unit, TgtCmd *cmd, CalcHitInfo *hit, int *damage)
{
    TecKindView *tec;
    int mode;
    int rate;

    tec = dataTecGet(cmd->tecId);
    mode = hit->result & 0xFF;
    if (tec->doubleDamageKind == 1 && calcStatGet(cmd->target, 0, 0x20)) {
        calcStatReset(cmd->target, 0, 0x20, 0);
        *damage *= 2;
    }
    if ((mode == 1 || mode == 2) && calcStatGet(cmd->target, 0, 0x10)) {
        hit->result = calcRevHit(hit->result);
    }
    if (mode == 2 && calcStatGet(cmd->target, 0, 8)) {
        *damage /= 2;
    }
    if (mode == 4 && calcStatGet(cmd->target, 1, 0x40)) {
        *damage /= 2;
    }
    if (mode == 1 && calcStatGet(cmd->target, 7, 0x40)) {
        rate = *damage % 10 / 2 * 20;
        if (*damage & 1) {
            *damage -= *damage * rate / 100;
        } else {
            *damage += *damage * rate / 100;
        }
    }
    if (mode == 3 && calcStatGet(cmd->target, 3, 2)) {
        *damage = 0;
    }
    if (mode == 1) {
        if (!calcBattleChk(cmd->target)) {
            ((CalcActorRecord *) cmd->target->work)->flags &= ~CALC_ACTOR_FLAG_BIT4;
        }
        calcStatReset(cmd->target, 1, 0x2000, 0);
        calcStatReset(cmd->target, 5, 0x2000, 0);
        if (calcStatGet(cmd->target, 5, 0x4000) && rnd(100) < 50) {
            calcStatReset(cmd->target, 5, 0x4000, 0);
        }
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatAtk);

/*
 * A status-clearing technique (effectType 4, `lb $4,0x4($18)`) takes the
 * CALC_ACTOR_FLAG_BIT4 bit off a target calcBattleChk refuses and records
 * the category and bits it clears in the hit record; category 8 records
 * the full mask (`ori $2,$0,0xFFFF`). Any other technique records category 8,
 * no bits, and returns 0.
 */
int calcStatClr(ObjectTask *unit, TgtCmd *cmd, CalcHitInfo *hit)
{
    TecKindView *tec;
    int category;

    tec = dataTecGet(cmd->tecId);
    if (tec->effectType == 4) {
        if (!calcBattleChk(cmd->target)) {
            ((CalcActorRecord *) cmd->target->work)->flags &= ~CALC_ACTOR_FLAG_BIT4;
        }
        category = tec->statCategory;
        if (category == 8) {
            hit->clearCategory = category;
            hit->clearMask = 0xFFFF;
        } else {
            hit->clearCategory = category;
            hit->clearMask = tec->statMask;
        }
        return 1;
    }
    hit->clearMask = 0;
    hit->clearCategory = 8;
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcCrit);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSpec);

/* dataPosLineGet (src/ov01/data_unit_org_get.c, this TU's own private view
 * of its PosTblEntry.line member): the position's battle line index. */
extern short dataPosLineGet(int posId);

extern const char D_00A447A0[]; /* "** LinePos: adjust\n" */

/*
 * A target on battle line 2 (dataPosLineGet of its posId) loses a fifth of
 * the damage when it is hit (result 1), unless it is on side 0x40, carries
 * CalcUnitParam flag 0x40 or is a charaId in 0xBB..0xC2.
 */
int calcLinePos(ObjectTask *unit, TgtCmd *cmd, CalcHitInfo *hit, int *damage)
{
    BattleActor *actor;
    int result;
    int hitResult;
    int line;

    result = 0;
    hitResult = hit->result & 0xFF;
    line = dataPosLineGet(calcUPGet(cmd->target)->posId);
    if (hitResult == 1) {
        actor = (BattleActor *) cmd->target->work;
        if (!(actor->flags & CALC_ACTOR_FLAG_ENEMY)) {
            if (!(calcUPGet(cmd->target)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
                if (calcUPGet(cmd->target)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN ||
                    calcUPGet(cmd->target)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END) {
                    if (line == 2) {
                        *damage -= *damage / 5;
                        result = 1;
                        printf(D_00A447A0);
                    }
                }
            }
        }
    }
    return result;
}

int calcRevHit(int hitMode)
{
    int mode = hitMode & 0xFF;
    int side = (hitMode >> 8) & 0xFF;

    switch (mode)
    {
        case 1:
            mode = 2;
            break;

        case 2:
            mode = 1;
            break;

        case 3:
            mode = 4;
            break;

        case 4:
            mode = 3;
            break;
    }

    return mode | (side << 8);
}

int calcAtrChg(ObjectTask *unit, int attribute) {
    int result;

    result = attribute;
    if (calcStatGet(unit, 3, 0x4000) != 0) {
        result = (result & ~0x1F) | 0x10;
    } else if (calcStatGet(unit, 3, 0x2000) != 0) {
        result = (result & ~0x1F) | 8;
    } else if (calcStatGet(unit, 3, 0x1000) != 0) {
        result = (result & ~0x1F) | 4;
    }
    return result;
}

/* calcSlotAChk (below, this TU): unit and the technique id. */
extern int calcSlotAChk(ObjectTask *unit, int techId);

/* tecId is `int`, not `short`: with `short` the entry block re-sign-extends
 * it (`sll $5,$5,16`/`sra $18,$5,16` replacing the original's plain
 * `daddu $18,$5,$0`), fndiff on form calcTakeEpGet (2026-10-14). */
int calcTakeEpGet(ObjectTask *unit, int tecId)
{
    int ep;

    ep = dataTecGet(tecId)->epCost;
    if (calcStatGet(unit, 1, 0x10)) {
        ep *= 2;
    }
    if (calcStatGet(unit, 3, 0x200)) {
        ep /= 2;
    }
    if (calcStatGet(unit, 3, 0x40)) {
        ep *= 2;
    }
    if (calcStatGet(unit, 7, 0x100) && calcSlotAChk(unit, tecId)) {
        ep = 0;
    }

    return (ep < 100) ? ep : 99;
}

int calcSlotAChk(ObjectTask *unit, int techId)
{
    CalcUnitParam *param;
    int i;

    for (i = 0; i < 2; i++)
    {
        param = calcUPGet(unit);
        if (param->slotTechniqueId[i] == techId)
        {
            return 1;
        }
    }
    return 0;
}

int calcAttWpnChk(int weaponId)
{
    return dataWpnGet(weaponId)->equipped != 0;
}

/*
 * The hit result item cmd->itmId has on cmd->target, by its effect bits;
 * the hit record carries no status to set or clear (category 8, no bits).
 * Effect bit 0's result 2 is reversed (calcRevHit) on a target carrying
 * CALC_UNIT_FLAG_ITEM_REVERSED and again under status (0, 0x10).
 */
int calcHitItem(ObjectTask *unit, TgtCmd *cmd, CalcHitInfo *hit)
{
    ItmTrgView *item;
    int result;

    item = dataItmGet(cmd->itmId);
    hit->setCategory = 8;
    hit->setMask = 0;
    hit->clearCategory = 8;
    hit->clearMask = 0;
    if (item->recoverHp) {
        result = 2;
        if (calcUPGet(cmd->target)->flags & CALC_UNIT_FLAG_ITEM_REVERSED) {
            result = calcRevHit(2);
        }
        if (calcStatGet(cmd->target, 0, 0x10)) {
            result = calcRevHit(result);
        }
    } else if (item->recoverEp) {
        result = 4;
    } else if (item->resultMode6) {
        result = 6;
    } else if (item->resultMode5) {
        result = 5;
    } else if (item->resultMode5Alt) {
        result = 5;
    } else if (item->recoverHpRate) {
        result = 0x802;
    } else if (item->hpToOne) {
        result = (item->power == 1) ? 1 : 6;
    } else {
        result = 1;
    }
    return result;
}

/*
 * The effect amount of item cmd->itmId on cmd->target, by its effect bits:
 * a recovery for recoverHp/recoverEp/recoverHpRate, a damage amount for
 * damageHpRate, and negative when CALC_UNIT_FLAG_ITEM_REVERSED on the target
 * flips a recoverHp item.
 */
int calcDmgItem(ObjectTask *unit, TgtCmd *cmd)
{
    ItmTrgView *item;
    int amount;

    amount = 0;
    item = dataItmGet(cmd->itmId);
    if (item->recoverHp) {
        int base = item->power * 25;

        amount = base * 2;
        if (calcStatGet(cmd->target, 3, 0x80)) {
            amount = base * 4;
            calcStatReset(cmd->target, 3, 0x80, 0);
        }
        if (calcStatGet(cmd->target, 0, 8)) {
            amount /= 2;
        }
        if (calcUPGet(cmd->target)->flags & CALC_UNIT_FLAG_ITEM_REVERSED) {
            amount = -amount;
        }
        if (calcStatGet(cmd->target, 0, 0x10)) {
            amount = -amount;
        }
        if (item->useFlags & 2) {
            if (calcUPGet(cmd->target)->flags & CALC_UNIT_FLAG_BIT12) {
                amount = 0;
            }
        }
    } else if (item->recoverEp) {
        int base = item->power * 5;

        amount = base * 2;
        if (calcStatGet(cmd->target, 3, 0x80)) {
            amount = base * 4;
            calcStatReset(cmd->target, 3, 0x80, 0);
        }
        if (calcStatGet(cmd->target, 1, 0x40)) {
            amount /= 2;
        }
    } else if (item->recoverHpRate) {
        int power = item->power;

        amount = calcUPGet(cmd->target)->maxHp * power / 20;
    } else if (item->damageHpRate) {
        int power = item->power;

        amount = calcUPGet(cmd->target)->maxHp * power / 20;
    } else if (item->hpToOne) {
        if (item->power == 1) {
            amount = (short) calcUPGet(cmd->target)->currentHp - 1;
        }
    }
    return amount;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", tgtFindFirst);

/*
 * calcAtkChk (this TU, below): unit, target and technique id. Its
 * return type is narrower than int: tgtFindChk calls it with jal and returns
 * through its own epilogue, which a same-width int return would turn into a
 * sibling call (`j calcAtkChk`). Every narrow width gives the same bytes here.
 */
extern short calcAtkChk(ObjectTask *unit, ObjectTask *target, int tecId);

int tgtFindChk(ObjectTask *unit, ObjectTask *target, TgtCmd *cmd)
{
    if (cmd->mode != 3) {
        return calcAtkChk(unit, target, cmd->tecId);
    }
    return 1;
}

/*
 * tgtFindFirst's own candidate table (this TU, still asm): tgtNum entries
 * of tgtTbl are filled in target-search order, tgtIdx walks them out one at
 * a time. The gap between tgtTbl and tgtIdx (0x00a57e68..0x00a57e88, 0x20
 * bytes) is this array's only size evidence.
 */
static ObjectTask *tgtTbl[8];
static int tgtIdx;
static int tgtNum;

ObjectTask *tgtFindNext(void)
{
    if (tgtIdx >= tgtNum) {
        return 0;
    }
    return tgtTbl[tgtIdx++];
}

/*
 * moveTbl (ov01/battle_init data, 0x00a3d880, 0x78 bytes): five reachable
 * positions per position, -1 for an empty entry. calcMoveChk indexes it by
 * posId - 1 (`lui/addiu` of 0x00a3d86c, one 0x14-byte row before the
 * table, plus posId * 0x14), so its six rows are posIds 1..6.
 */
extern int moveTbl[6][5];

/*
 * Whether unit may move to toPosId: enemies and AGWS units always may, a
 * player character only to a position its own row of moveTbl lists.
 */
int calcMoveChk(ObjectTask *unit, int toPosId)
{
    int posId;
    int i;

    posId = calcUPGet(unit)->posId;
    if (!(((CalcActorRecord *) unit->work)->flags & CALC_ACTOR_FLAG_ENEMY)) {
        if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)) {
            for (i = 0; i < 5; i++) {
                if (moveTbl[posId - 1][i] != -1 && moveTbl[posId - 1][i] == toPosId) {
                    return 1;
                }
            }
            return 0;
        }
    }
    return 1;
}

/* TecKindView.attr's range class (calcAtkChk below). */
enum { TEC_ATTR_RANGE = 0x03 };

/* calcLineChk (this TU, defined below) and calcA2Chk (this TU, still
 * asm): calcAtkChk's per-unit battle-line tests. */
extern int calcLineChk(ObjectTask *unit);
extern int calcA2Chk(ObjectTask *unit);

short calcAtkChk(ObjectTask *unit, ObjectTask *target, int tecId)
{
    BattleActor *actor;
    int line;
    int range;

    line = dataPosLineGet(calcUPGet(unit)->posId);
    range = dataTecGet(tecId)->attr & TEC_ATTR_RANGE;
    if (range == 0) {
        if (calcUPGet(target)->flags & CALC_UNIT_FLAG_NO_RANGE0_ATTACK) {
            return 0;
        }
    }
    if (calcStatGet(target, 1, 0x200)) {
        return 0;
    }
    actor = (BattleActor *) unit->work;
    if (!(actor->flags & CALC_ACTOR_FLAG_ENEMY)) {
        if (line != 0 && line != 1 && range != 2) {
            return 0;
        }
    } else if (range != 2) {
        if (!calcLineChk(unit)) {
            return 0;
        }
        if (!calcA2Chk(target)) {
            return 0;
        }
    }
    if (!calcLineChk(target)) {
        return range == 2;
    }
    return 1;
}

/*
 * A unit standing at posId 8 passes only when no live unit of its own side
 * stands at a posId in 1..6 (`addiu $3,$3,-0x1`/`sltiu $3,$3,0x6`).
 */
int calcA2Chk(ObjectTask *unit)
{
    ObjectTask **table;
    short posId;
    int side;
    int count;
    int i;

    if (calcUPGet(unit)->posId != 8) {
        return 1;
    }
    if (((CalcActorRecord *) unit->work)->flags & CALC_ACTOR_FLAG_ENEMY) {
        side = 1;
    } else {
        side = 0;
    }
    i = 0;
    count = unitTblGet(side, &table);
    for (; i < count; i++) {
        if (table[i] != 0 && !(((CalcActorRecord *) table[i]->work)->flags & CALC_ACTOR_FLAG_DEAD)) {
            posId = calcUPGet(table[i])->posId;
            if (posId >= 1 && posId <= 6) {
                return 0;
            }
        }
    }
    return 1;
}

int calcLineChk(ObjectTask *unit)
{
    ObjectTask **table;
    short myLine;
    short otherLine;
    int side;
    int count;
    int i;

    myLine = dataPosLineGet(calcUPGet(unit)->posId);
    if (myLine == 0)
    {
        return 1;
    }

    if (((CalcActorRecord *) unit->work)->flags & CALC_ACTOR_FLAG_ENEMY)
    {
        side = 1;
    }
    else
    {
        side = 0;
    }
    i = 0;
    count = unitTblGet(side, &table);
    if (count > 0)
    {
        for (; i < count; i++)
        {
            if (table[i] != 0)
            {
                if ((((CalcActorRecord *) table[i]->work)->flags & 0x80) == 0)
                {
                    otherLine = dataPosLineGet(calcUPGet(table[i])->posId);
                    if (otherLine != 0 && otherLine < myLine)
                    {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

/*
 * calcBackLineChk's extra-blocker selector (calcDmg passes 1, stat2Proc
 * passes 0): selector 0 tests CALC_UNIT_FLAG_BLOCKS_BACK_LINE of a unit standing
 * further back (`lhu $3,0x1C($2)`/`andi $3,$3,0x800`, 0x00a16370);
 * selector 1 tests that unit's charaId against 6 (`lh $3,0x38($2)`/
 * `beq $3,$21`, 0x00a16398; which character 6 is, is not established).
 */
int calcBackLineChk(ObjectTask *unit, int selector)
{
    ObjectTask **table;
    short myLine;
    short otherLine;
    int side;
    int count;
    int i;

    myLine = dataPosLineGet(calcUPGet(unit)->posId);
    if (myLine == 0)
    {
        return 1;
    }

    if (((CalcActorRecord *) unit->work)->flags & CALC_ACTOR_FLAG_ENEMY)
    {
        side = 1;
    }
    else
    {
        side = 0;
    }
    i = 0;
    count = unitTblGet(side, &table);
    if (count > 0)
    {
        for (; i < count; i++)
        {
            if (table[i] != 0)
            {
                if ((((CalcActorRecord *) table[i]->work)->flags & CALC_ACTOR_FLAG_DEAD) == 0)
                {
                    otherLine = dataPosLineGet(calcUPGet(table[i])->posId);
                    if (otherLine != 0 && myLine < otherLine)
                    {
                        switch (selector)
                        {
                        case 0:
                            if (calcUPGet(table[i])->flags & CALC_UNIT_FLAG_BLOCKS_BACK_LINE)
                            {
                                return 0;
                            }
                            break;
                        case 1:
                            if (calcUPGet(table[i])->charaId == 6)
                            {
                                return 0;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    return 1;
}

int calcWpnEnableChk(ObjectTask *unit, int slot)
{
    WpnData *wpn;
    CalcUnitParam *up;
    int weaponId;

    up = calcUPGet(unit);
    weaponId = up->weaponId[slot];
    if (weaponId == 0) {
        return 0;
    }
    wpn = dataWpnGet(weaponId);
    /* The original calls dataTecGet(wpn->tecId) here and uses nothing of
     * its result (`jal dataTecGet` at 0x00a16424, `lh $4,0x14($17)` in its
     * delay slot, $2 overwritten by the next call); the call is kept so the
     * bytes match. */
    dataTecGet(wpn->tecId);
    if (calcAttWpnChk(weaponId) != 0) {
        if (up->attachmentId[slot] == 0) {
            return 0;
        }
        if (up->wpnSlotValue[slot] <= 0) {
            return 0;
        }
    }
    if (wpn->apCost > up->dblActKind) {
        return 0;
    }
    if (calcTakeEpGet(unit, wpn->tecId) > (short)up->currentEp) {
        return 0;
    }
    return 1;
}

int calcWpnUse(ObjectTask *unit, int slot)
{
    CalcUnitParam *up;

    if (slot != -1) {
        if (slot >= 4) {
            return 1;
        }
        up = calcUPGet(unit);
        if (calcAttWpnChk(up->weaponId[slot]) != 0) {
            if (up->wpnSlotValue[slot] != 0xFF) {
                up->wpnSlotValue[slot]--;
            }
            if (up->wpnSlotValue[slot] < 0) {
                up->wpnSlotValue[slot] = 0;
            }
        }
    }
    return 1;
}

int calcWpnEquipOrg(int charaId, int slot, int weaponId, int hand) {
    CalcCharParaData *origin;

    if (slot >= 3 || charaId >= 0x21) {
        return 0;
    }
    origin = dataUnitOrgGet(charaId);
    origin->weaponId[slot] = (short) weaponId;
    origin->hand[slot] = (signed char) hand;
    return 1;
}

int calcAccEquipOrg(int charaId, int slot, int accessoryId) {
    if (slot >= 3 || charaId >= 0x21) {
        return 0;
    }
    dataUnitOrgGet(charaId)->accessoryId[slot] = (short) accessoryId;
    return 1;
}

int calcAttEquipOrg(int charaId, int slot, int attachmentId) {
    if (slot >= 3 || charaId >= 0x21) {
        return 0;
    }
    dataUnitOrgGet(charaId)->attachmentId[slot] = (short) attachmentId;
    return 1;
}

int calcSpecEquipOrg(int charaId, int slot, int specialId) {
    if (slot >= 6 || charaId >= 0x21) {
        return 0;
    }
    dataUnitOrgGet(charaId)->specialId[slot] = (short) specialId;
    return 1;
}

int calcEtherEquipOrg(int charaId, int slot, int etherId) {
    if (slot >= 0xC || charaId >= 0x21) {
        return 0;
    }
    dataUnitOrgGet(charaId)->etherId[slot] = (short) etherId;
    return 1;
}

int calcSkillEquipOrg(int charaId, int slot, int skillId) {
    if (slot >= 3 || charaId >= 0x21) {
        return 0;
    }
    dataUnitOrgGet(charaId)->skillId[slot] = (short) skillId;
    return 1;
}

int calcAgwsEquipOrg(int charaId, int agwsId) {
    if ((unsigned int) (agwsId - 0x11) >= 0x10 || charaId >= 0x11) {
        return 0;
    }
    dataUnitOrgGet(charaId)->agwsId = (short) agwsId;
    return 1;
}

int calcEngineEquipOrg(int charaId, int engineId)
{
    CalcCharParaData *origin = dataUnitOrgGet(charaId);

    if ((origin->flags & 0x40) == 0) {
        return 0;
    }
    origin->engineId = (short) engineId;
    return 1;
}

int calcFrameEquipOrg(int charaId, int frameId)
{
    CalcCharParaData *origin = dataUnitOrgGet(charaId);

    if ((origin->flags & 0x40) == 0) {
        return 0;
    }
    origin->frameId = (short) frameId;
    return 1;
}

/*
 * The CalcUnitParam.dblActKind value calcDblActChk requires (`addiu $4,$0,6`
 * against `lb $2,0x3A($17)`); nothing else in this TU names it.
 */
#define CALC_DBL_ACT_KIND 6

/* calcSameWpnChk (this TU, defined below): a weapon slot of unit, or -1. */
extern int calcSameWpnChk(ObjectTask *unit);

int calcDblActChk(ObjectTask *unit)
{
    CalcUnitParam *up;
    int slot;

    up = calcUPGet(unit);
    slot = calcSameWpnChk(unit);
    if (slot != -1 && up->dblActKind == CALC_DBL_ACT_KIND
        && dataWpnGet(up->weaponId[slot])->dblAct != 0) {
        if (calcAttWpnChk(up->weaponId[slot]) == 0 || up->wpnSlotValue[slot] > 0) {
            return 1;
        }
        return 0;
    }
    return 0;
}

/*
 * The first weapon slot (0 or 1) whose weapon a later slot also holds, or
 * -1. An empty slot (weapon id 0) never counts as a match: slot 0's tests
 * are skipped when it is 0 (`lh $4,0x5E($6)`/`beqz $4`, 0x00a16954..
 * 0x00a16958) and slot 1's likewise (`beqz $3`, 0x00a16a64), so two empty
 * slots give -1. Weapon ids 0x5B and 0x70 count as the same weapon, as do
 * 0x5C and 0x71 (the `addiu` pairs compared at 0x00a16984..0x00a16a48).
 */
int calcSameWpnChk(ObjectTask *unit)
{
    CalcUnitParam *up;

    up = calcUPGet(unit);

    if (up->weaponId[0] != 0) {
        if (up->weaponId[0] == up->weaponId[1]) {
            return 0;
        }
        if (up->weaponId[0] == up->weaponId[2]) {
            return 0;
        }
        if (up->weaponId[0] == 0x5B || up->weaponId[0] == 0x70) {
            if (up->weaponId[1] == 0x5B || up->weaponId[1] == 0x70) {
                return 0;
            }
        }
        if (up->weaponId[0] == 0x5B || up->weaponId[0] == 0x70) {
            if (up->weaponId[2] == 0x5B || up->weaponId[2] == 0x70) {
                return 0;
            }
        }
        if (up->weaponId[0] == 0x5C || up->weaponId[0] == 0x71) {
            if (up->weaponId[1] == 0x5C || up->weaponId[1] == 0x71) {
                return 0;
            }
        }
        if (up->weaponId[0] == 0x5C || up->weaponId[0] == 0x71) {
            if (up->weaponId[2] == 0x5C || up->weaponId[2] == 0x71) {
                return 0;
            }
        }
    }
    if (up->weaponId[1] != 0) {
        if (up->weaponId[1] == up->weaponId[2]) {
            return 1;
        }
        if (up->weaponId[1] == 0x5B || up->weaponId[1] == 0x70) {
            if (up->weaponId[2] == 0x5B || up->weaponId[2] == 0x70) {
                return 1;
            }
        }
        if (up->weaponId[1] == 0x5C || up->weaponId[1] == 0x71) {
            if (up->weaponId[2] == 0x5C || up->weaponId[2] == 0x71) {
                return 1;
            }
        }
    }
    return -1;
}

/* calcEtherDisableGet (this TU, defined below): calcEthEnableChk's own
 * callee, the unit's disable bit for ether slot `bit`. */
extern int calcEtherDisableGet(ObjectTask *unit, int bit);

/*
 * Whether unit can use ether etherId now: its technique must find a
 * target, the unit must have the EP calcTakeEpGet asks, ether 3 is refused
 * while cfEventGet() carries bit 8, status (7, 2) refuses ether 0x2B and
 * its absence 0x2C/0x2D, status (7, 1) refuses 0x2E and its absence
 * 0x2F/0x30, and an equipped slot holding etherId must not be disabled.
 */
int calcEthEnableChk(ObjectTask *unit, int etherId)
{
    int i;

    if (!calcTgtUnitTbl(unit, dataTecGet(dataEthGet(etherId)->tecId)->attr, 0)) {
        return 0;
    }
    if ((short) calcUPGet(unit)->currentEp < calcTakeEpGet(unit, dataEthGet(etherId)->tecId)) {
        return 0;
    }
    if (etherId == 3 && (cfEventGet() & 8)) {
        return 0;
    }
    if ((calcStatGet(unit, 7, 2) && etherId == 0x2B)
        || (!calcStatGet(unit, 7, 2) && (etherId == 0x2C || etherId == 0x2D))
        || (calcStatGet(unit, 7, 1) && etherId == 0x2E)
        || (!calcStatGet(unit, 7, 1) && (etherId == 0x2F || etherId == 0x30))) {
        return 0;
    }
    for (i = 0; i < 12; i++) {
        if (calcUPGet(unit)->etherId[i] == etherId && calcEtherDisableGet(unit, i)) {
            return 0;
        }
    }
    return 1;
}

int calcItmEnableChk(ObjectTask *unit, int itmId)
{
    if (calcTgtUnitTbl(unit, dataItmGet(itmId)->trgFlags, 0) == 0) {
        return 0;
    }
    if (itmId == 0x10 && (cfEventGet() & 8) != 0) {
        return 0;
    }
    return 1;
}

int calcTgtUnitTbl(ObjectTask *unit, int trgFlags, ObjectTask **table)
{
    ObjectTask **srcTable;
    int side;
    int total;
    int count;
    int i;

    count = 0;
    if (trgFlags & CALC_TRG_SELF)
    {
        if (!((trgFlags & CALC_TRG_EXCLUDE_AGWS) && (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS)))
        {
            if (table != 0)
            {
                table[0] = unit;
            }
            count = 1;
        }
    }
    else
    {
        i = 0;
        side = calcTrg2PE(unit, trgFlags);
        total = unitTblGet(side, &srcTable);
        if (total > 0)
        {
            for (; i < total; i++)
            {
                if (srcTable[i] == 0)
                {
                    continue;
                }
                if (!(trgFlags & CALC_TRG_DEAD_UNITS))
                {
                    if (((CalcActorRecord *) srcTable[i]->work)->flags & CALC_ACTOR_FLAG_DEAD)
                    {
                        continue;
                    }
                }
                else
                {
                    if (!(((CalcActorRecord *) srcTable[i]->work)->flags & CALC_ACTOR_FLAG_DEAD))
                    {
                        continue;
                    }
                }
                if (trgFlags & CALC_TRG_EXCLUDE_AGWS)
                {
                    if (calcUPGet(srcTable[i])->flags & CALC_UNIT_FLAG_IS_AGWS)
                    {
                        continue;
                    }
                }
                if (table != 0)
                {
                    table[count] = srcTable[i];
                }
                count++;
            }
        }
    }
    return count;
}

int calcDeadUnitNum(ObjectTask *unit, int flag)
{
    ObjectTask **table;
    int count;
    int i;
    int result;

    count = unitTblGet(calcTrg2PE(unit, flag), &table);
    result = 0;
    if (count > 0)
    {
        for (i = 0; i < count; i++)
        {
            if (table[i] != 0)
            {
                if ((((CalcActorRecord *)table[i]->work)->flags & 0x80) != 0)
                {
                    result++;
                }
            }
        }
    }
    return result;
}

/*
 * 0 for a missing or dead (CALC_ACTOR_FLAG_DEAD) unit or one carrying any
 * of the five statuses tested; otherwise 1.
 */
int calcBattleChk(ObjectTask *unit)
{
    if (unit == 0
        || (((CalcActorRecord *) unit->work)->flags & CALC_ACTOR_FLAG_DEAD)
        || calcStatGet(unit, 0, 0x2000)
        || calcStatGet(unit, 1, 0x2000)
        || calcStatGet(unit, 4, 0x2000)
        || calcStatGet(unit, 5, 0x4000)
        || calcStatGet(unit, 5, 0x2000)) {
        return 0;
    }
    return 1;
}

/*
 * Answers a target-selection flag word (cmdAtktbl, calcTgtUnitTbl and
 * calcDeadUnitNum supply it) against the unit's own side bit, bit 0x40 of
 * CalcActorRecord's flags: a selection of 0x40, and any word carrying 0x08,
 * asks whether the unit is on that side; a selection of 0x80 asks whether it
 * is not; every other selection answers 2.
 */
int calcTrg2PE(ObjectTask *unit, int trgFlags)
{
    if ((trgFlags & 0xC0) == 0x40 || (trgFlags & 8) != 0) {
        if ((((CalcActorRecord *)unit->work)->flags & CALC_ACTOR_FLAG_ENEMY) != 0) {
            return 1;
        }
        return 0;
    }
    if ((trgFlags & 0xC0) == 0x80) {
        if ((((CalcActorRecord *)unit->work)->flags & CALC_ACTOR_FLAG_ENEMY) == 0) {
            return 1;
        }
        return 0;
    }
    return 2;
}

/* calcStatIdx2Bit/calcStatReset: calcStatTurn's own callees, defined later
 * in this TU (calcStatIdx2Bit below) or still asm (calcStatReset). */
extern int calcStatIdx2Bit(int statIdx);
extern void calcStatReset(ObjectTask *unit, int category, int mask, int useAltMask);

void calcStatTurn(ObjectTask *unit)
{
    CalcUnitParam *param;
    int category;
    int statIdx;
    unsigned char remaining;

    param = calcUPGet(unit);
    for (category = 0; category < 8; category++) {
        for (statIdx = 0; statIdx < 16; statIdx++) {
            if (param->statActiveMask[category] & calcStatIdx2Bit(statIdx)) {
                remaining = param->statTurnCount[category][statIdx];
                if (remaining != 0xFF) {
                    remaining--;
                    if (remaining == 0) {
                        calcStatReset(unit, category, calcStatIdx2Bit(statIdx), 0);
                    }
                    param->statTurnCount[category][statIdx] = remaining;
                }
            }
        }
    }
}

int calcStatProtectGet(ObjectTask *unit, int group, int mask)
{
    return calcUPGet(unit)->statProtect[group] & mask;
}

void calcStatProtectSet(ObjectTask *unit, int group, int mask)
{
    calcUPGet(unit)->statProtect[group] |= mask;
}

/*
 * calcStatGetNorm's lookup, widened: outside the two party-wide cases the
 * answer also includes statAltMask[category] & flag of the same unit
 * (`lhu $3,0xAC($2)` and `lhu $2,0xBC($16)`, OR-ed at 0x00a17234).
 */
int calcStatGet(ObjectTask *unit, int category, int flag)
{
    ObjectTask **table;
    int count;
    int result;
    int i;

    if ((category == 3 && (flag & 1)) || (category == 6 && (flag & 0x800))) {
        count = unitTblGet(0, &table);
        result = 0;
        for (i = 0; i < count; i++) {
            if (table[i] != 0) {
                result = calcUPGet(table[i])->statActiveMask[category] & flag;
                if (result != 0) {
                    break;
                }
            }
        }
    } else {
        result = calcUPGet(unit)->statActiveMask[category] & flag;
        result |= calcUPGet(unit)->statAltMask[category] & flag;
    }
    return result;
}

/*
 * calcStatGet's plain lookup: statActiveMask[category] & flag of unit
 * (`lhu $2,0xAC($3)`/`and $6,$2,$17`, 0x00a17328..0x00a1732c). When category
 * is 3 and flag has bit 0x1 (`andi $2,$17,0x1`, 0x00a17288) or category is
 * 6 and flag has bit 0x800 (`andi $2,$17,0x800`, 0x00a1729c), the answer is
 * instead that same statActiveMask[category] & flag, with the whole
 * requested mask (`and $6,$3,$17`, 0x00a17300), of the first unit of
 * unitTblGet(0)'s table for which it is nonzero, or 0 when no unit's is.
 */
int calcStatGetNorm(ObjectTask *unit, int category, int flag)
{
    ObjectTask **table;
    int count;
    int result;
    int i;

    if ((category == 3 && (flag & 1)) || (category == 6 && (flag & 0x800))) {
        count = unitTblGet(0, &table);
        result = 0;
        for (i = 0; i < count; i++) {
            if (table[i] != 0) {
                result = calcUPGet(table[i])->statActiveMask[category] & flag;
                if (result != 0) {
                    break;
                }
            }
        }
    } else {
        result = calcUPGet(unit)->statActiveMask[category] & flag;
    }
    return result;
}

/*
 * Whether status category may be set on unit: with CALC_UNIT_FLAG_BIT12
 * categories 0..3 are refused (`sltiu $3,$16,0x4`) and category 5 too
 * unless CALC_UNIT_FLAG_BIT3 is set; without it categories 4..6 are
 * refused (`addiu $3,$16,-0x4`/`sltiu $3,$3,0x3`). Past that, force
 * accepts outright; otherwise calcStatProtectGet's protection and status
 * 2/0x2000 (which still admits category 7) refuse.
 */
int calcStatSetChk(ObjectTask *unit, int category, int flag, int force)
{
    u16 flags;

    flags = calcUPGet(unit)->flags;
    if (flags & CALC_UNIT_FLAG_BIT12) {
        if (category >= 0 && category <= 3) {
            return 0;
        }
        if (!(flags & CALC_UNIT_FLAG_BIT3) && category == 5) {
            return 0;
        }
    } else if (category >= 4 && category <= 6) {
        return 0;
    }
    if (force != 0) {
        return 1;
    }
    if (calcStatProtectGet(unit, category, flag) != 0) {
        return 0;
    }
    if (calcStatGet(unit, 2, 0x2000) != 0 && category != 7) {
        return 0;
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatSet);

/* calcStatBit2Idx (this TU, defined below): shared with calcStatTurn. */
extern int calcStatBit2Idx(int statBit);

void calcStatReset(ObjectTask *unit, int category, int mask, int useAltMask)
{
    CalcUnitParam *up;
    int idx;

    up = calcUPGet(unit);
    if (useAltMask == 0) {
        up->statActiveMask[category] &= ~mask;
        idx = calcStatBit2Idx(mask);
        up->statTurnCount[category][idx] = 0;
    } else {
        up->statAltMask[category] &= ~mask;
    }
}

/* turn_1 (still asm data, build/ov01/asm/data/ov01/calc.data.s): an 8x16-byte table of
 * per-category/per-status-index wait-count-end turn bonuses. */
extern signed char turn_1[8][16];

/*
 * Base bonus from turn_1[category][statIdx], returned as is unless category
 * is 7 (agility, per calcWct) and statBit has one of its two low bits set
 * (`andi $16,$16,0x3`, 0x00a179e4; `beql $16,$0`, 0x00a17a08). Then one more
 * turn when the unit's current HP (`lh $5,0x34($16)`, signed) is strictly
 * below half of its maxHp (signed halfword, division rounded toward zero:
 * 0x00a17a24..0x00a17a44) and one more again when it is strictly below a
 * quarter of maxHp (0x00a17a5c..0x00a17a74): at most two extra turns, each
 * comparison re-reading calcUPGet(unit) twice rather than caching it.
 */
int calcStatDefTurnGet(ObjectTask *unit, int category, int statBit)
{
    int idx;
    int bonus;
    CalcUnitParam *up;

    idx = calcStatBit2Idx(statBit);
    bonus = turn_1[category][idx];
    if (category == 7 && (statBit & 3) != 0) {
        up = calcUPGet(unit);
        if ((short) up->currentHp < (short) calcUPGet(unit)->maxHp / 2) {
            bonus++;
        }
        up = calcUPGet(unit);
        if ((short) up->currentHp < (short) calcUPGet(unit)->maxHp / 4) {
            bonus++;
        }
    }
    return bonus;
}

/*
 * The inverse of calcStatIdx2Bit below: the index of statBit's lowest set bit
 * among the 16 status bits calcStatSet, calcStatReset and calcStatDefTurnGet
 * index with it, or 16 when statBit holds none of them.
 */
int calcStatBit2Idx(int statBit)
{
    int statIdx;
    int mask;

    mask = 1;
    for (statIdx = 0; statIdx < 16; statIdx++) {
        if (statBit & mask) {
            break;
        }
        mask <<= 1;
    }
    return statIdx;
}

int calcStatIdx2Bit(int statIdx)
{
    return 1 << statIdx;
}

void calcStatReset(ObjectTask *unit, int category, int mask, int useAltMask);

void calcStatResetCha(ObjectTask *unit) {
    calcStatReset(unit, 0, 0x7FFF, 0);
    calcStatReset(unit, 1, 0x7FFF, 0);
    calcStatReset(unit, 2, 0x7FFF, 0);
    calcStatReset(unit, 3, 0x7FFF, 0);
}

void calcStatResetAll(ObjectTask *unit) {
    calcStatReset(unit, 0, 0x7FFF, 0);
    calcStatReset(unit, 1, 0x7FFF, 0);
    calcStatReset(unit, 2, 0x7FFF, 0);
    calcStatReset(unit, 3, 0x7FFF, 0);
    calcStatReset(unit, 4, 0x7FFF, 0);
    calcStatReset(unit, 5, 0x7FFF, 0);
    calcStatReset(unit, 6, 0x7FFF, 0);
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcResult);

/* One battle's four rewards, in calcResultSub's order (+0x0..+0xC of its
 * second argument, +0x4..+0x10 of its fourth). */
typedef struct CalcReward {
    int exp;
    int techPoint;
    int etherPoint;
    int skillPoint;
} CalcReward;

/*
 * The per-character result record calcResult (this TU, still asm) fills
 * and hands to calcResultSub, as far as calcResultSub writes it.
 */
typedef struct CalcResultEntry {
    unsigned char unmodeled_00[0x04];
    CalcReward gained; /* +0x04: calcResultSub's `sw $16,0x4($21)`..0x10 */
} CalcResultEntry;

/*
 * calcSkill's mask bits of the reward skills (kind 0x1000) calcResultSub
 * asks for, each a percentage bonus on one reward.
 */
enum SkillRewardBit {
    SKILL_REWARD_SKILL_POINT = 0x800,
    SKILL_REWARD_ETHER_POINT = 0x1000,
    SKILL_REWARD_TECH_POINT = 0x2000,
    SKILL_REWARD_EXP = 0x4000
};

/*
 * Gives unit's character its share of total (each reward divided by
 * memberCount, then raised by the matching reward skill's percentage) and
 * records the shares in entry.
 */
void calcResultSub(ObjectTask *unit, CalcReward *total, int memberCount, CalcResultEntry *entry)
{
    CalcPlCharaView *pl;
    int gain;
    int bonus;

    pl = dataPlChaGet(calcUPGet(unit)->charaId);

    gain = total->exp / memberCount;
    bonus = calcSkill(unit, 0x1000, SKILL_REWARD_EXP);
    if (bonus > 0) {
        gain += gain * bonus / 100;
    }
    pl->exp += gain;
    if (pl->exp > 9999999) {
        pl->exp = 9999999;
    }
    entry->gained.exp = gain;

    gain = total->techPoint / memberCount;
    bonus = calcSkill(unit, 0x1000, SKILL_REWARD_TECH_POINT);
    if (bonus > 0) {
        gain += gain * bonus / 100;
    }
    pl->techPoint += gain;
    if (pl->techPoint > 9999) {
        pl->techPoint = 9999;
    }
    entry->gained.techPoint = gain;

    gain = total->etherPoint / memberCount;
    bonus = calcSkill(unit, 0x1000, SKILL_REWARD_ETHER_POINT);
    if (bonus > 0) {
        gain += gain * bonus / 100;
    }
    pl->etherPoint += gain;
    if (pl->etherPoint > 9999) {
        pl->etherPoint = 9999;
    }
    entry->gained.etherPoint = gain;

    gain = total->skillPoint / memberCount;
    bonus = calcSkill(unit, 0x1000, SKILL_REWARD_SKILL_POINT);
    if (bonus > 0) {
        gain += gain * bonus / 100;
    }
    pl->skillPoint += gain;
    if (pl->skillPoint > 9999) {
        pl->skillPoint = 9999;
    }
    entry->gained.skillPoint = gain;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcLvUp);

/*
 * entryPhase40 (src/ov01/entry_first_init.c) calls this as a bare statement
 * and never reads a result, but the original ends with a real jal to
 * calcPartyStat plus its own separate epilogue rather than a tail jump into
 * it, which this int return type (left unset) reproduces.
 */
int calcCfEncount(int flags)
{
    if (flags & 1)
    {
        calcPartyStat(0, 0);
        menuTimeSet(1);
    }
    if (flags & 2)
    {
        calcPartyStat(0, 0);
        calcPartyStat(0, 1);
    }
}

void calcPartyStat(int side, int mode)
{
    ObjectTask **table;
    ObjectTask *unit;
    int count;
    int i;

    count = unitTblGet(side, &table);
    for (i = 0; i < count; i++) {
        unit = table[i];
        if (unit != 0) {
            switch (mode) {
            case 0:
                calcUPGet(unit)->waitCount = 0;
                break;
            case 1:
                calcUPGet(unit)->bcValue++;
                break;
            }
        }
    }
}

int calcEtherDisableGet(ObjectTask *unit, int bit)
{
    u16 mask;

    mask = 1 << bit;
    return calcUPGet(unit)->etherDisableFlags & mask;
}

void calcEtherDisableSet(ObjectTask *unit, int bit)
{
    CalcUnitParam *param;
    u16 mask;

    mask = 1 << bit;
    param = calcUPGet(unit);
    param->etherDisableFlags |= mask;
}

int rnd(int max)
{
    return xglSRand() % (max + 1);
}

/* points/count were spelled `int start`/`int end` while calcSpline was asm;
 * the pointer type changes no byte here (two `sw` of the argument). */
void calcSplineInit(CalcSpline *spline, Vector4 *points, int count, int duration)
{
    spline->points = points;
    spline->count = count;
    spline->duration = (float) duration;
    spline->tension = 0.5f;
    spline->frame = 0;
}

/* calcSpline's own callees, all defined below in this TU. */
void calcLinerSub(CalcSpline *spline, Vector4 *out, int idx, float ratio);
void calcBezierSub(CalcSpline *spline, Vector4 *out, int idx, float t);
void calcSplineSub(CalcSpline *spline, Vector4 *out, int idx, float t);
void calcHermiteSub(CalcSpline *spline, Vector4 *out, int idx, float t);

/*
 * Evaluates the curve at the current frame into out with the interpolation
 * mode selects (1 B-spline, 2 Bezier over the whole curve, 3 Hermite,
 * anything else linear). The segmented modes split the duration less one
 * frame evenly over count - 3 (B-spline) or count - 1 segments and pass the
 * segment index and the position inside it. Once the frame reaches the
 * duration out takes the last point and 1 is returned; until then the
 * frame advances and 0 is returned.
 */
int calcSpline(CalcSpline *spline, Vector4 *out, int mode)
{
    float frame;
    float step;
    int idx;

    switch (mode) {
    case 2:
        frame = spline->frame;
        calcBezierSub(spline, out, 0, 1.0f / (spline->duration - 1.0f) * frame);
        break;
    case 1:
        frame = spline->frame;
        step = (spline->duration - 1.0f) / (spline->count - 3);
        idx = frame / step;
        calcSplineSub(spline, out, idx, 1.0f / step * (frame - idx * step));
        break;
    case 3:
        frame = spline->frame;
        step = (spline->duration - 1.0f) / (spline->count - 1);
        idx = frame / step;
        calcHermiteSub(spline, out, idx, 1.0f / step * (frame - idx * step));
        break;
    case 0:
    default:
        frame = spline->frame;
        step = (spline->duration - 1.0f) / (spline->count - 1);
        idx = frame / step;
        calcLinerSub(spline, out, idx, 1.0f / step * (frame - idx * step));
        break;
    }
    if (spline->duration <= frame) {
        idx = spline->count - 1;
        out->x = spline->points[idx].x;
        out->y = spline->points[idx].y;
        out->z = spline->points[idx].z;
        return 1;
    }
    frame += 1.0f;
    spline->frame = frame;
    return 0;
}

/*
 * calcLinerSub reads CalcSpline's point array: Vector4 is the shared
 * four-float record from include/shared.h, used the same way (an indexed
 * array, only x/y/z read) by src/ov01/unit_cmd.c's cmdTarget fields.
 *
 * The original (asm/nonmatchings/ov01/calc/calcLinerSub.s, 0x00a18a98):
 * `sll $6,$6,4` scales idx by 16 before `addu $6,$6,$2` adds it to the base,
 * proving the 16-byte stride per entry; the two `ldl/ldr` pairs at
 * `0x7($6)`/`0x0($6)` and `0xf($6)`/`0x8($6)` copy points[idx] and
 * points[idx+1] (0x10 bytes each) whole, and the `lwc1`/`sub.s`/`mul.s`/
 * `add.s` sequence on $f0-$f5 at offsets 0x0/0x4/0x8 of each copy is the
 * x/y/z lerp this function writes through `out`.
 */
/* The table was a one-member CalcLinerTbl view until calcSpline (which
 * hands it its own CalcSpline) was recovered; no byte changes. */
void calcLinerSub(CalcSpline *spline, Vector4 *out, int idx, float ratio)
{
    Vector4 a, b;

    a = spline->points[idx];
    b = spline->points[idx + 1];
    out->x = a.x + (b.x - a.x) * ratio;
    out->y = a.y + (b.y - a.y) * ratio;
    out->z = a.z + (b.z - a.z) * ratio;
}

/*
 * The cubic Bezier through points[idx..idx+3] at t: weights (1-t)^3,
 * 3t(1-t)^2, 3t^2(1-t) and t^3 (`mul.s` on $f0/$f1/$f12/$f7).
 */
void calcBezierSub(CalcSpline *spline, Vector4 *out, int idx, float t)
{
    Vector4 p0, p1, p2, p3;
    float u;
    float uu;
    float b0, b1, b2, b3;

    u = 1.0f - t;
    p0 = spline->points[idx];
    p1 = spline->points[idx + 1];
    p2 = spline->points[idx + 2];
    p3 = spline->points[idx + 3];
    uu = u * u;
    b0 = uu * u;
    b1 = 3 * t * uu;
    b2 = 3 * t * t * u;
    b3 = t * t * t;
    out->x = b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x;
    out->y = b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y;
    out->z = b0 * p0.z + b1 * p1.z + b2 * p2.z + b3 * p3.z;
}

/*
 * The uniform cubic B-spline segment over points[idx..idx+3] at t; each
 * weight is divided by the double 6.0 (`ori $16,$0,0x8030`/`dsll32
 * $16,$16,15`, then fptodp/dpdiv/dptofp).
 */
void calcSplineSub(CalcSpline *spline, Vector4 *out, int idx, float t)
{
    Vector4 p0, p1, p2, p3;
    float t2;
    float t3;
    float b0, b1, b2, b3;

    t2 = t * t;
    t3 = t2 * t;
    p0 = spline->points[idx];
    p1 = spline->points[idx + 1];
    p2 = spline->points[idx + 2];
    p3 = spline->points[idx + 3];
    b0 = (-t3 + 3 * t2 - 3 * t + 1) / 6.0;
    b1 = (3 * t3 - 6 * t2 + 4) / 6.0;
    b2 = (-3 * t3 + 3 * t2 + 3 * t + 1) / 6.0;
    b3 = t3 / 6.0;
    out->x = b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x;
    out->y = b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y;
    out->z = b0 * p0.z + b1 * p1.z + b2 * p2.z + b3 * p3.z;
}

/*
 * The cubic Hermite segment from points[idx] to points[idx+1] at t; the two
 * tangents are the tension times the difference of the neighbouring points
 * (one-sided at either end of the array: `bnel $6,$0` and `slt $2,$6,$2`
 * against count - 1).
 */
void calcHermiteSub(CalcSpline *spline, Vector4 *out, int idx, float t)
{
    Vector4 p0, p1;
    Vector4 startTangent, endTangent;
    Vector4 *tangent;
    float t2;
    float t3;
    float h00, h01, h10, h11;
    int prev;
    int next;
    int i;

    t2 = t * t;
    t3 = t2 * t;
    h00 = 2 * t3 - 3 * t2 + 1;
    h01 = -2 * t3 + 3 * t2;
    h10 = t3 - 2 * t2 + t;
    h11 = t3 - t2;
    p0 = spline->points[idx];
    p1 = spline->points[idx + 1];
    tangent = &startTangent;
    for (i = 0; i < 2; i++) {
        if (idx == 0) {
            prev = 0;
            next = 1;
        } else if (idx >= spline->count - 1) {
            prev = idx - 1;
            next = idx;
        } else {
            prev = idx - 1;
            next = idx + 1;
        }
        tangent->x = spline->tension * (spline->points[next].x - spline->points[prev].x);
        tangent->y = spline->tension * (spline->points[next].y - spline->points[prev].y);
        tangent->z = spline->tension * (spline->points[next].z - spline->points[prev].z);
        tangent = &endTangent;
        idx++;
    }
    out->x = h00 * p0.x + h01 * p1.x + h10 * startTangent.x + h11 * endTangent.x;
    out->y = h00 * p0.y + h01 * p1.y + h10 * startTangent.y + h11 * endTangent.y;
    out->z = h00 * p0.z + h01 * p1.z + h10 * startTangent.z + h11 * endTangent.z;
}

/* Normalizes vec's x/y/z in place (`lwc1` at 0x0/0x4/0x8). What is tested
 * is the inverse length itself, 1.0f / sqrt(x*x + y*y + z*z) (`div.s $f12,
 * $f0,$f1`, 0x00a1916c), against 0.0f (`c.eq.s $f12,$f2`, 0x00a19170): equal,
 * all three components are stored as 0.0f; otherwise each is multiplied by
 * it. The input length is never compared with zero. */
void calcNormal(Vector4 *vec)
{
    float scale;

    scale = 1.0f / __builtin_sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
    if (scale == 0.0f) {
        vec->x = 0.0f;
        vec->y = 0.0f;
        vec->z = 0.0f;
    } else {
        vec->x *= scale;
        vec->y *= scale;
        vec->z *= scale;
    }
}
