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
 * calcStatGet (this TU, still asm) as every caller in this file invokes it:
 * a status/flag lookup keyed by category and bit.
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

extern void calcAp(ObjectTask *unit, int ap);
extern CalcCmdEntry *unitCmdPtrGet(ObjectTask *unit);
extern void unitCmdSet(ObjectTask *unit, CalcCmdEntry *cmd);

/* calcDeadUnitNum: calcSpecChk's own callee, still asm in this TU. */
extern int calcDeadUnitNum(ObjectTask *unit, int flag);

/* calcBattleChk: calcWctDec's own callee, still asm in this TU. */
extern int calcBattleChk(ObjectTask *unit);

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
    unsigned char unmodeled_00[0x10]; /* +0x00 */
    u8 equipped;                      /* +0x10: calcAttWpnChk's own check */
};

extern WpnData *dataWpnGet(int weaponId);

/*
 * calcSpline/calcSplineSub (both still asm in this TU) advance this
 * descriptor every frame; calcSplineInit sets it up with the value range,
 * the duration in frames as a float and a fixed 0.5 midpoint, the frame
 * counter starting at zero.
 */
typedef struct CalcSpline {
    int start;
    int end;
    float duration;
    float midpoint;
    int frame;
} CalcSpline;

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcCopyParaMake);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcPara2Org);

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

/* The per-charaId table calcTotalParaMenu hands calcTotalParaMenuSub. */
extern unsigned char D_00A57CE8[];

CalcCharParaData *calcTotalParaMenu(int charaId, int *attack, int *defense)
{
    return calcTotalParaMenuSub(charaId, attack, defense, D_00A57CE8);
}

/*
 * calcTotalParaMenuSub/calcSkillEquChkAll's (this TU) own fake unit: a
 * stack-local ObjectTask/CalcActorRecord pair whose `work`/`up` chain
 * (calcUPGet's own traversal, 0x00a11010..0x00a1101c) is pointed at a
 * temporary CalcUnitParam-compatible record instead of a live battle unit,
 * so the two functions can reuse calcSkillInit/calcWpnAtkGet/calcPhyDefGet/
 * calcMagDefGet/calcSkillEquChk for a unit that has no ObjectTask.
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

/* calcCopyParaMake/calcSkillInit/calcWpnAtkGet/calcPhyDefGet/calcMagDefGet:
 * calcTotalParaMenuSub's own callees, still asm in this TU except
 * calcMagDefGet (this TU, below); the latter four each read their argument
 * through calcUPGet, so calcTotalParaMenuSub hands them its own fake unit
 * above. */
extern void calcCopyParaMake(CalcCharParaData *origin, unsigned char *table);
extern void calcSkillInit(ObjectTask *unit);
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
 * calcTakeEpGet (this TU, still asm, 0x00a15670); nothing else in this TU
 * fills the rest of it.
 */
typedef struct CalcTakeEpArg {
    unsigned char unmodeled_00[0x10];
    CalcTakeEpPara *totalPara; /* +0x10: the record above */
    unsigned char unmodeled_14[0x80 - 0x14];
} CalcTakeEpArg;

extern int calcTakeEpGet(CalcTakeEpArg *arg, short tecId);

/* dataEthGet (src/ov01/data_unit_org_get.c, still asm), this TU's own
 * private view (see CalcEtherStatus, src/ov01/calc.h). */
extern CalcEtherStatus *dataEthGet(int etherId);

int calcTakeEpGetMenu(int charaId, int etherId) {
    CalcTakeEpArg arg;
    CalcTakeEpPara para;

    arg.totalPara = &para;
    para.totalPara = calcTotalParaMenu(charaId, 0, 0);
    return calcTakeEpGet(&arg, dataEthGet(etherId)->tecId);
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcUseEtherMenu);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcUseItemMenu);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcUseItem);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSkillInit);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSkillInitSub);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSkill);

/*
 * dataSklGet's return record (src/ov01/data_unit_org_get.c, still asm), as
 * far as calcSkillSub reads it: two halfword bit sets.
 */
typedef struct SkillData SkillData;

struct SkillData {
    u16 typeFlags;   /* +0x00: calcSkillSub's own mask test */
    u16 levelFlags;  /* +0x02: calcSkillSub returns bits 0-6 of this field */
};

extern SkillData *dataSklGet(int skillId);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcAccEquChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSkillEquChk);

/* calcSkillEquChk/unitTblGet: calcSkillEquChkAll's own callees, still asm
 * in this TU. calcSkillEquChk's second call below hands it the same fake
 * unit calcMagDefGet (this TU) takes; unitTblGet writes the active unit
 * table's base address through its second argument and returns its count. */
extern int calcSkillEquChk(ObjectTask *unit, int skillId);
extern int unitTblGet(int type, ObjectTask ***table);

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
                    if (calcUPGet(table[i])->flags & 0x40) {
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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcAp);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcBp);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcBp2Tid);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcBc);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcCounter);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcBoostChk);

extern int *actStockGet(void);
void calcBc(BattleUnit *unit, int delta);
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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcIdol2Chk);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcDItemChk);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHitRate);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcWpnAtkGet);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcPhyDefGet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcPowLevGet);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcPlusAglGet);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcDmg);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", futtobiChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStat);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatAtk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatClr);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcCrit);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSpec);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcLinePos);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcTakeEpGet);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSlotAChk);

int calcAttWpnChk(int weaponId)
{
    return dataWpnGet(weaponId)->equipped != 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHitItem);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcDmgItem);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", tgtFindFirst);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", tgtFindChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", tgtFindNext);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcMoveChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcAtkChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcA2Chk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcLineChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcBackLineChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcWpnEnableChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcWpnUse);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcEngineEquipOrg);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcFrameEquipOrg);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcDblActChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSameWpnChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcEthEnableChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcItmEnableChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcTgtUnitTbl);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcDeadUnitNum);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcBattleChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcTrg2PE);

/* calcStatIdx2Bit/calcStatReset: calcStatTurn's own callees, defined later
 * in this TU (calcStatIdx2Bit below) or still asm (calcStatReset). */
extern int calcStatIdx2Bit(int statIdx);
extern void calcStatReset(ObjectTask *unit, int category, int mask, int unused);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatProtectSet);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatGet);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatGetNorm);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatSetChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatSet);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatReset);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatDefTurnGet);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatBit2Idx);

int calcStatIdx2Bit(int statIdx)
{
    return 1 << statIdx;
}

void calcStatReset(ObjectTask *unit, int category, int mask, int unused);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcResultSub);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcPartyStat);

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

void calcSplineInit(CalcSpline *spline, int start, int end, int duration)
{
    spline->start = start;
    spline->end = end;
    spline->duration = (float) duration;
    spline->midpoint = 0.5f;
    spline->frame = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSpline);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcLinerSub);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcBezierSub);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSplineSub);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHermiteSub);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcNormal);
