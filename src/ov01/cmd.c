/*
 * OV01 original TU 6: 0x00a1d348..0x00a22628 (136 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov01/obj.h"
#include "cmd.h"

void thinkSysInit(void)
{
    memset(processBuf, 0, sizeof(processBuf));
    memset(context, 0, sizeof(context));
}

void thinkTopSet(int dataTop)
{
    pDataTop = dataTop;
}

void thinkContextSet(int context)
{
    pContext = context;
}

int thinkAdrGet(short offset)
{
    /* The offset is rounded down to an even byte count before it is added. */
    return pDataTop + (unsigned int)offset / 2 * 2;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkProcessAdd);

int thinkProcessDel(int *slot)
{
    if (*slot != 0) {
        *slot = 0;
        return 1;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkProcessChk);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkProcessKindChk);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkProcessExec);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkProcessExecSub);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkInit);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", camInit);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkMonsTblNumGet);

int thinkMonsTblGet(int monsSetNo)
{
    thinkTopSet(pThinkTop);
    return thinkAdrGet(pMonsSetTop[monsSetNo].script_offset);
}

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

int thinkExecChk(void)
{
    return thinkProcessKindChk(0) == 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkExecSub);

int regChk(int operand)
{
    unsigned short reg = operand;

    if (!(reg & 0x8000)) {
        printf(D_00A461E8, reg);
        return 0;
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkRegNo);

int thinkValSet(int value)
{
    value &= 0x7FFF;

    if (value & 0x4000) {
        value |= ~0x7FFF;
    }
    return value;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkRegGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkRegSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdReg);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdNum);

int cmdExit(void)
{
    return 1;
}

int cmdGo(ThinkProcess *proc)
{
    proc->pc = cmdNum(proc->pc);
    return 0;
}

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

int cmdRput(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    int reg;

    proc->pc += 2;
    if (regNo & 0x4000) {
        printf(D_00A46300);
    }
    /* regNo's low 16 bits, kept in its own register separately from the raw
     * value the indirect-addressing test above reads. */
    reg = 0xFFFF;
    reg = regNo & reg;
    printf(D_00A46308, reg & 0x3FFF, thinkRegGet(reg));
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdPrint);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdMemdump);

int cmdCmdputon(void)
{
    cmdPutFlag = 1;
    return 0;
}

int cmdCmdputoff(void)
{
    cmdPutFlag = 0;
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdMsgPos);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdMsg);

int battleMsgEndChk(void)
{
    return pMsgObj == 0;
}

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

void cmdThinkset2(ThinkProcess *proc)
{
    cmdThinksetSub(proc);
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdThinksetSub);

void thinkUnitPtrGetReg(int reg)
{
    int unitNo = thinkRegGet(reg & 0xFFFF);

    if (unitNo == 0x7FFF) {
        unitNo = unitNoGet(pThinkUnit);
    }
    unitPtrGet(unitNo);
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCounterBoost);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdStatChk);

int cmdLineChk(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    int unitNo;

    proc->pc += 2;
    unitNo = cmdNum(proc->pc);

    proc->pc += 2;
    thinkRegSet(regNo & 0xFFFF, calcLineChk(unitPtrGet(unitNo)) != 0);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnitparaGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnitparaSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnitpara);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnitparaSub);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnittbl);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnitCreate);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdUnitChange);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdAtktbl);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdAtkset);

void cmdAtktblSort(ThinkProcess *proc)
{
    proc->sortMode = 0;
    cmdAtktblSortSub(proc);
}

void cmdAtktblSortRev(ThinkProcess *proc)
{
    proc->sortMode = 1;
    cmdAtktblSortSub(proc);
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdAtktblSortSub);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdAtktblSearch);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdTecparaGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdTecpara);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdTecparaSub);

int cmdMonsSetNoGet(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);

    proc->pc += 2;
    thinkRegSet(regNo & 0xFFFF, monsSetNoGet() & 0xFFFF);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdPartySizeGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdLightAmb);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdLightCol);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdLightDir);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", mcamPtrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", mpersPtrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", mbankPtrGet);

void myMCamSet(int mode, void *params)
{
    if (mode >= 0) {
        MCamSet(mode, params);
    }
}

int unitNoChk(int unitNo)
{
    if (unitNo >= 8) {
        printf(D_00A46578, unitNo);
        unitNo = 0;
    }
    return unitNo;
}

int cmdCamRefMode(void)
{
    camMode = 1;
    return 0;
}

int cmdCamCamMode(void)
{
    camMode = 0;
    return 0;
}

int cmdCamRefMoveMode(void)
{
    camMode = -2;
    return 0;
}

int cmdCamCamMoveMode(void)
{
    camMode = -1;
    return 0;
}

int cmdCamPersMode(void)
{
    persMode = 3;
    return 0;
}

int cmdCamBankMode(void)
{
    bankMode = 2;
    return 0;
}

int cmdCamPersMoveMode(void)
{
    persMode = -3;
    return 0;
}

int cmdCamBankMoveMode(void)
{
    bankMode = -4;
    return 0;
}

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

int cmdCamTblSet(ThinkProcess *proc)
{
    int tblNo = cmdNum(proc->pc);

    proc->pc += 2;
    if (tblNo < thinkCamTblNumGet()) {
        camTblIdx = tblNo;
    }
    return 0;
}

int cmdCamMoveStop(void)
{
    MCamStopMove((unsigned int)(camMode + 1) >= 2);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdEvent);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdWaitEvent);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdWaitCnt);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdWaitCamMove);

int cmdWaitMsg(ThinkProcess *proc)
{
    int result = 2;

    if (proc->waitActive) {
        result = pMsgObj == 0;
        result = result ? 0 : 2;
    }
    return result;
}

int cmdVPadEneble(void)
{
    dataVPadModeSet(1);
    return 0;
}

int cmdVPadDisable(void)
{
    dataVPadModeSet(0);
    return 0;
}

int cmdVPadSet(ThinkProcess *proc)
{
    int padMask = cmdNum(proc->pc) & 0xFFFF;

    proc->pc += 2;
    dataVPadSet(padMask);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdMapMulSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdHpPerGet);

int cmdEventSlotGet(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);

    proc->pc += 2;
    thinkRegSet(regNo & 0xFFFF, menuTimeGet() & 0xFFFF);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdThinkCamEvent);

int cmdCfEncountGet(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);

    proc->pc += 2;
    thinkRegSet(regNo & 0xFFFF, cfEncountGet() & 0xFFFF);
    return 0;
}

int cmdCfEventGet(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);

    proc->pc += 2;
    thinkRegSet(regNo & 0xFFFF, cfEventGet() & 0xFFFF);
    return 0;
}

int cmdThinkNoGet(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);

    proc->pc += 2;
    thinkRegSet(regNo & 0xFFFF, thinkNoGet() & 0xFFFF);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdStatGet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdStatSet);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdStatReset);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdDeadTime);
