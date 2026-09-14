/*
 * OV01 original TU 3: 0x00a07770..0x00a10bb8 (150 functions)
 * Name: battle_init (first global function)
 */
#include "common.h"
#include "shared.h"
#include "battle_init.h"

#define CURSOR_STATE_OFF 0x10
#define STATE_FLAGS_OFF 0x00
#define STATE_UNK70_OFF 0x70
#define STATE_UNK74_OFF 0x74

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleMain);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleAfter);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrl);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlConfusion);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlJunk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockMaxGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockMake);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockPick);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockRemove);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actStockGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlHelp);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlHelpGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlBoostPl);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlBoostEn);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlMan);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", manInput);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", camEventTurnStartSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cmdListPtrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cmdListSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cmdListChange);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", convKey2unit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", unitCmdListSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cmdListCansel);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleCtrlThink);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleExec);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleExecNorm);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleExecDead);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", stat1Proc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", stat2Proc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyCmdGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyCmdApGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyCmdChk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyCmd2tid);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyCmd2wno);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyBuffClear);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyBuffIdxGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyBuffSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", keyBuffChk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", busyUnitChk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actUnitTblMake);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", tgtUnitPick);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleEndChk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleRetCodeGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleRetCodeSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", thinkNoGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", thinkNoSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", plUnitTblGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", plUnitTblSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", monsSetNoGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", monsSetNoSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", mapNoGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", mapNoSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cfEncountGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cfEncountSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cfEventGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", cfEventSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", actUnitGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", lastUnitGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", tgtUnitAdrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", tgtUnitGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", tgtUnitSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", manWorkGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", menuChk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", batCtrlGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioPtrFuncSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioSrcSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioDstSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioSrcGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioDstGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatNext);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioCam);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExec);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatDataSync);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", etehrCVGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", hairWindOn);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", hairWindOff);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase10);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase20);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase25);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase30);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase40);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecPhase50);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecMain);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", nusumeSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", nusumeGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", flowSpecProcInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", flowSpecProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", syoukanFadeOut);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", syoukanFadeIn);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", syoukan1Sub);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", syoukan2Sub);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", specSyoukanProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", specHensinProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", specAnalyzeProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", specEscapeProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", specInoriProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", specNusumuProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", itemNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", battleMsg);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatExecDst);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", decoEffCall);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", decoEffCallSub);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioEffCall);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioMtd);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", spWepDispOn);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", spWepDispOnSub);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", spWepDispOff);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", spWepDispOffSub);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", transWepIn);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", transWepInDraw);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", transWepOut);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", transWepOutDraw);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioSrcEndChk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", scenarioBatEnd);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", counterProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", dmgValPut);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", monsGainPtrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", monsGainInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", monsGainSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", itemGrpChk);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", statEffProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", statEffOn);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", statEffOff);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objEnCurCreate);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objCurRemove);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objCur);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objCurDraw);

/*
 * Evidenced word offsets on the opaque state/cursor handles. Each offset is
 * observed directly in the original body; the labels stay neutral because
 * the historical member names are unresolved:
 *   cursor + 0x10 : state pointer (lw s1,16(a0) in the callee delay slot);
 *   state  + 0x00 : flags word (read, OR mask 0x2, written back);
 *   state  + 0x70 : word zeroed on entry (sw zero,112(s1));
 *   state  + 0x74 : word zeroed on entry (sw zero,116(s1)).
 * Accesses are written inline (no helper calls) so the candidate emits only
 * the single curCmdSet extent; the macros keep the offsets reviewable.
 */

void curCmdSet(CursorObject *cursor, const CursorCommand *command)
{
    CursorCommandState *state;
    CursorCommand *tail;
    unsigned int flags;

    state = *(CursorCommandState **)((char *)cursor + CURSOR_STATE_OFF);
    tail = objCmdTailGet(cursor);
    *(unsigned int *)((char *)state + STATE_UNK70_OFF) = 0;
    *(unsigned int *)((char *)state + STATE_UNK74_OFF) = 0;
    *tail = *command;
    if (tail->selector != 0) {
        flags = *(unsigned int *)((char *)state + STATE_FLAGS_OFF);
        *(unsigned int *)((char *)state + STATE_FLAGS_OFF) = flags | 2;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curSel);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curGrpSel);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curMove2Pos);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curEnProcInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curEnProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curPosProcInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curPosProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curTgtProcInit);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curTgtProc);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curSelGrpSet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curSelGrpGet);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objDispPlCur);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objDispEnCur);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objDispCur);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objDispCurSubPl);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", objDispCurSubEn);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curPutSub);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curTexTrans);

INCLUDE_ASM("asm/nonmatchings/ov01/battle_init", curEnvMake);
