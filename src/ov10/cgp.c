/*
 * OV10 original TU 8: 0x00a21768..0x00a324b8 (68 functions)
 */
#include "common.h"
#include "cgp.h"

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPShuntPosSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPRecalcPosSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CCO143ExecSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPPrintYama);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPPrintJunk);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPPrintSute);

void CGPSetMessage(CardGameWork *work, s32 index)
{
    work->mess = OLMessTbl[index];
}

void CGPSetInterruptMess(CardGameWork *work, s32 index)
{
    work->interruptMess = CardPlayTMessList[index];
}

void CGPSetPermanentMess(CardGameWork *work, s32 index)
{
    if (index == 0) {
        work->permanentMess = 0;
        return;
    }
    work->permanentMess = PermMessTbl[index];
}

void CGPSetErrorMess(s8 code, CardGameWork *work, u8 reason)
{
    work->errorMessCode = code;
    work->errorMessKind = 1;
    work->errorMessReason = reason;
    work->errorMessValue = -1;
}

void CGPSetErrorMessPlus(s8 code, CardGameWork *work, u8 reason, s16 value)
{
    work->errorMessCode = code;
    work->flags |= CGP_FLAG_ERROR_PLUS;
    work->errorMessKind = 3;
    work->errorMessReason = reason;
    work->errorMessValue = value;
}

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDispErrorMessCore);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDispErrorMess3);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDispErrorMessPlus);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDispErrorMess);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDispErrorMess2);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardPlayDispInfo);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPChkFaseLock);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardSetEffect);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CDEMatrixSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardDispEffect);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCheckTeMax);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCommFinishSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardGameInit);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPNextTurnSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPMoveDSBATTLEwork);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPequipSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPWeaponEquipChk);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDisposeWeaponSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCursor2Area);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCursor2Pos);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCMSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCursorMove2);

/* CGPCursorMove2, which does the actual cursor movement, is still
 * assembler-scaffolded in this TU. */
extern void CGPCursorMove2(s32 flag, CardGameWork *work);

void CGPCursorMove(s32 flag, CardGameWork *work)
{
    CGPCursorMove2(flag, work);
}

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPSetCursor);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPChkPlayable);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPChkOpePlayable);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardPlayClearEndflg);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPChkCardPlayCost);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardPlayDisployment);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDispoSub0);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDisposeSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPMoveSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleBeforeEffect);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleAttackEffect);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleDiffenceEffect);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBAEsub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleAfterEffect);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleCalcuration);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCalcPower);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCalcPowerPlus);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCalcPowerEnv);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPHPCheckSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPRefreshFieldHP);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleSub);

void CGPRotStageSub(CardGameWork *work, u32 stage)
{
    if ((*work->objFlags & 0x10) && work->fase == 9 &&
        work->rotStageId != stage && work->rotStageCooldown == 0)
    {
        work->rotStageId = stage;
        work->rotStageCooldown = 20;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPDrawFaseProc);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPMoveFaseProc);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPSetFaseProc);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCFPlayCommOperation);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCFSDispSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPAnswerSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCommFaseEndSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPCommFaseSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPEndFaseSub);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPBattleFaseProc);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CGPEndFaseProc);

INCLUDE_ASM("asm/nonmatchings/ov10/cgp", CardGameProc);
