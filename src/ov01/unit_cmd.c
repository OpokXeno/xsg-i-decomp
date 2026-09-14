/*
 * OV01 original TU 2: 0x00a00d60..0x00a07770 (119 functions)
 */
#include "common.h"

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitInit);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTblGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTblSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTblRemove);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTblChg);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitNumGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitLiveNumGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitLiveNumGet2);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCidGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitPlCreate);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitEnCreate);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitAgwsPilotGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCreate);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitRemoveActor);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitRemove);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitLoad);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitActupdate);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitActdraw);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitParaInit);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitPlInit);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitEnInit);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitClipSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitPlFunc);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitEnFunc);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdClear);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdTailGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdPtrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdNext);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdExec);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdFree);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdAtk);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdAtkSub);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitJobTypeAtk);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdAtkEnd);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDeadSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdEther);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdItem);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdItemSub);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMove);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdGuard);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdAgws);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdUnitChange);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdStand);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdEnd);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDmg);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdEva);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDef);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDead);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitDeadSpecChk);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMot);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdTgtSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdTurn);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdTurnSub);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdTurnSubInit);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveCha);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveNext);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaNorm);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaRun);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaJump);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaHover);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaWarp);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaJump2);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaHover2);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaFloat);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveChaHover3);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePos);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosNorm);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosRun);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosJump);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosHover);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosWarp);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosHover2);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosFloat);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMovePosHover3);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDirAdj);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdPosAdj);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdRun);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdJump);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdHover);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdFloat);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdWarp);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDstSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDirSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdLenGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdDivSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdVASet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdMoveCalc);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTelIn);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitTelOut);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdBtst);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdBted);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitMtdProc);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdRec);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdWake);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdStat);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdReadWait);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdAgws2);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdEscape);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdFadeIn);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitCmdFadeOut);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitMotSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitMotSetEx);

/*
 * unitSetInterp enables interpolation on the actor object owned by a unit.
 * The surrounding Unit layout is not established here; the two scalar
 * offsets below are the members directly demonstrated by this function and
 * its callers.
 */
void unitSetInterp(void *unit)
{
    void *object = *(void **)((unsigned char *)unit + 0x10);
    *(int *)((unsigned char *)object + 0x100) = 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitSetInterpTime);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitWpnMotSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitFaceMotSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitMotGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitMotStandSet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitNoGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitPtrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitDispOn);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitDispOff);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitColiGet);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitAgwsChk);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitIdChk);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitDispOnAll);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitDispOffAll);

INCLUDE_ASM("asm/nonmatchings/ov01/unit_cmd", unitDispOnOff);
