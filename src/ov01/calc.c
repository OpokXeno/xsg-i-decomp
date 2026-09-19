/*
 * OV01 original TU 4: 0x00a10bb8..0x00a191c0 (116 functions)
 */
#include "common.h"
#include "main/xgl_2.h"
#include "calc.h"

/* calcCfEncount's own callees; both are still asm in this TU. */
extern void calcPartyStat(int, int);
extern void menuTimeSet(int);

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

extern WpnData *dataWpnGet(void);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcPara2OrgSub);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcDeadReset);

CalcUnitParam *calcUPGet(ObjectTask *unit)
{
    return ((CalcActorRecord *)unit->work)->up;
}

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHpDmg);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcTotalParaMenu);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcTotalParaMenuSub);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcTakeEpGetMenu);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcUseEtherMenu);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcUseItemMenu);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcUseItem);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSkillInit);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSkillInitSub);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSkill);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSkillSub);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcAccEquChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSkillEquChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSkillEquChkAll);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcAp);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHp);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcEp);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcBp);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcBp2Tid);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcBc);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcCounter);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcBoostChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcBoost);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcIdol2Chk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcTuigekiChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSpecChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcDItemChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcTurnStart);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcTurnEnd);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcWct);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcWctDec);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHitMotChk);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHitDmg);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHitRate);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHitParaGet);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcHit);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcWpnAtkGet);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcPhyDefGet);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcMagDefGet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcAtrChg);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcTakeEpGet);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSlotAChk);

int calcAttWpnChk(void)
{
    return dataWpnGet()->equipped != 0;
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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcWpnEquipOrg);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcAccEquipOrg);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcAttEquipOrg);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSpecEquipOrg);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcEtherEquipOrg);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcSkillEquipOrg);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcAgwsEquipOrg);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatTurn);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatProtectGet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatResetCha);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcStatResetAll);

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

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcEtherDisableGet);

INCLUDE_ASM("asm/nonmatchings/ov01/calc", calcEtherDisableSet);

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
