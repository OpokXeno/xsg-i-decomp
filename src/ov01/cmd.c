/*
 * OV01 original TU 6: 0x00a1d348..0x00a22628 (136 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov01/obj.h"
#include "cmd.h"

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkSysInit);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkTopSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkContextSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkAdrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkProcessAdd);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkProcessDel);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkProcessChk);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkProcessKindChk);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkProcessExec);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkProcessExecSub);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkInit);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", camInit);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkMonsTblNumGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkMonsTblGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkCamTblNumGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", camInitExec);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", camEventExec);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkInitExec);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkTurnStartExec);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkTurnEndExec);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkUnitAtkExec);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkUnitDmgExec);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkExec);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", camExec);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkExecChk);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkExecSub);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", regChk);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkRegNo);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkValSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkRegGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkRegSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdReg);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdNum);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdExit);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdGo);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdGosub);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdReturn);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdBra);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdOngo);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdOngosub);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdRset);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdRmath1);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdRmath2);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdRpush);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdRpop);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdTblget);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdTblset);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdRput);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdPrint);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdMemdump);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCmdputon);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCmdputoff);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdMsgPos);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdMsg);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", battleMsgEndChk);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", battleMsgPut);

void msgObj(MessageTask *task)
{
    task->remaining_frames--;
    if (task->remaining_frames != 0) {
        eBattleWinMain2();
    } else {
        eBattleWinClose2();
        objRemovePure((ObjectTask *)task);
        pMsgObj = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", battleMsgPut2);

void msgObj2(MessageTask *task)
{
    if (eBattleWinPageCheck4() != 0) {
        eBattleWinClose4();
        objRemovePure((ObjectTask *)task);
        pMsgObj = 0;
    } else {
        eBattleWinMain4();
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdThinkset);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdThinkset2);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdThinksetSub);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkUnitPtrGetReg);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCounterBoost);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdStatChk);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdLineChk);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnitparaGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnitparaSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnitpara);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnitparaSub);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnittbl);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnitCreate);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnitChange);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdAtktbl);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdAtkset);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdAtktblSort);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdAtktblSortRev);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdAtktblSortSub);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdAtktblSearch);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdTecparaGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdTecpara);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdTecparaSub);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdMonsSetNoGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdPartySizeGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdLightAmb);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdLightCol);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdLightDir);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", mcamPtrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", mpersPtrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", mbankPtrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", myMCamSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", unitNoChk);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamRefMode);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamCamMode);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamRefMoveMode);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamCamMoveMode);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamPersMode);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamBankMode);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamPersMoveMode);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamBankMoveMode);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamLock);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamCopy);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamSetPos);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamSetAct);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamSetAct2);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamSetAct3);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamOffsPos);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamOffsAng);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamMove);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamShake);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamCenterLength);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamUnitHeight);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamCoordGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamBank);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamPers);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamTblSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamMoveStop);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdEvent);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdWaitEvent);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdWaitCnt);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdWaitCamMove);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdWaitMsg);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdVPadEneble);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdVPadDisable);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdVPadSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdMapMulSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdHpPerGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdEventSlotGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdThinkCamEvent);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCfEncountGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCfEventGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdThinkNoGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdStatGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdStatSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdStatReset);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdDeadTime);
