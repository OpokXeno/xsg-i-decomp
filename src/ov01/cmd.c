/*
 * OV01 original TU 6: 0x00a1d348..0x00a22628 (136 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov01/calc.h"
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

int thinkProcessExecSub(ThinkProcess *proc);

int thinkProcessExec(void)
{
    ThinkProcess *proc;
    int i;

    dataVPadSet(0);
    proc = (ThinkProcess *)processBuf;
    for (i = 0; i < 16; i++) {
        if (proc->dataTop != 0) {
            thinkProcessExecSub(proc);
        }
        proc = (ThinkProcess *)((unsigned char *)proc + 0x24);
    }
    return 1;
}

extern int thinkExecSub(ThinkProcess *proc);
extern const char D_00A46068[];

int thinkProcessExecSub(ThinkProcess *proc)
{
    int status;

    thinkTopSet(proc->dataTop);
    thinkContextSet(proc->context);
    do {
        status = thinkExecSub(proc);
    } while (status == 0);
    if (status == 1) {
        thinkProcessDel(&proc->dataTop);
    } else if (status != 2) {
        printf(D_00A46068, status);
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", thinkInit);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", camInit);

int thinkMonsTblNumGet(void)
{
    MonsTblEntry *entry = pMonsSetTop;
    int count = 0;

    if (entry->script_offset != 0) {
        do {
            entry++;
            count++;
        } while (entry->script_offset != 0);
    }
    return count;
}

int thinkMonsTblGet(int monsSetNo)
{
    thinkTopSet(pThinkTop);
    return thinkAdrGet(pMonsSetTop[monsSetNo].script_offset);
}

extern short *pCamSetTop;

int thinkCamTblNumGet(void)
{
    short *entry = pCamSetTop;
    int count = 0;

    if (*entry != 0) {
        do {
            entry++;
            count++;
        } while (*entry != 0);
    }
    return count;
}

extern int camExec(short scriptOffset);
extern int cameraFlagGet(void);
extern const char D_00A460A0[];
extern unsigned char D_00A59AF8;

int camInitExec(void)
{
    short script = pCamTop->initScript;

    thinkContextSet((int)&D_00A59AF8);
    if (script != 0) {
        thinkRegSet(0x801B, cameraFlagGet() & 0xFFFF);
        printf(D_00A460A0, script);
        return camExec(script);
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", camEventExec);

extern const char D_00A46148[];

int thinkInitExec(void)
{
    short script = pMonsSetTop[monsSetNoGet()].initScript;

    thinkContextSet((int)context);
    if (script != 0) {
        printf(D_00A46148, script);
        return thinkExec(script);
    }
    return 1;
}

int thinkTurnStartExec(ObjectTask *unit)
{
    short script = pMonsSetTop[monsSetNoGet()].turnStartScript;

    thinkContextSet((int)context);
    if (script != 0) {
        thinkRegSet(0x8010, unitNoGet(unit) & 0xFFFF);
        thinkRegSet(0x8019, unitLiveNumGet(0) & 0xFFFF);
        thinkRegSet(0x801A, unitLiveNumGet(1) & 0xFFFF);
        printf(D_00A46160, script);
        return thinkExec(script);
    }
    return 1;
}

int thinkTurnEndExec(ObjectTask *unit)
{
    short script = pMonsSetTop[monsSetNoGet()].turnEndScript;

    thinkContextSet((int)context);
    if (script != 0) {
        thinkRegSet(0x8010, unitNoGet(unit) & 0xFFFF);
        thinkRegSet(0x8019, unitLiveNumGet(0) & 0xFFFF);
        thinkRegSet(0x801A, unitLiveNumGet(1) & 0xFFFF);
        printf(D_00A46180, script);
        return thinkExec(script);
    }
    return 1;
}

int thinkUnitAtkExec(ObjectTask *unit)
{
    AtkTblEntry *atkTbl = *(AtkTblEntry **)((char *)calcUPGet(unit) + CALC_UNIT_PARAM_ATK_TBL_OFF);
    short script = atkTbl->script_offset;

    thinkContextSet((int)context);
    if (script != 0) {
        pThinkUnit = unit;
        thinkRegSet(0x8010, unitNoGet(unit) & 0xFFFF);
        thinkRegSet(0x8011, 1);
        thinkRegSet(0x8012, 0);
        thinkRegSet(0x8013, 0);
        thinkRegSet(0x8019, unitLiveNumGet(0) & 0xFFFF);
        thinkRegSet(0x801A, unitLiveNumGet(1) & 0xFFFF);
        printf(D_00A46198, unitNoGet(unit), script);
        return thinkExec(script);
    }
    return 1;
}

int thinkUnitDmgExec(ObjectTask *unit, UnitDmgInfo *info)
{
    AtkTblEntry *atkTbl = *(AtkTblEntry **)((char *)calcUPGet(unit) + CALC_UNIT_PARAM_ATK_TBL_OFF);
    short script = atkTbl->dmgScript;

    thinkContextSet((int)context);
    if (script != 0) {
        pThinkUnit = unit;
        thinkRegSet(0x8010, unitNoGet(unit) & 0xFFFF);
        thinkRegSet(0x8014, unitNoGet(info->attacker) & 0xFFFF);
        thinkRegSet(0x8015, info->reg8015);
        thinkRegSet(0x8016, info->reg8016);
        thinkRegSet(0x8017, info->reg8017);
        thinkRegSet(0x8019, unitLiveNumGet(0) & 0xFFFF);
        thinkRegSet(0x801A, unitLiveNumGet(1) & 0xFFFF);
        printf(D_00A461B8, unitNoGet(unit), script);
        return thinkExec(script);
    }
    return 1;
}

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

int cmdBra(ThinkProcess *proc)
{
    int left;
    int relation;
    int right;
    int comparison;
    int branch;

    left = cmdNum(proc->pc);
    proc->pc += 2;

    relation = cmdNum(proc->pc);
    proc->pc += 2;

    right = cmdNum(proc->pc);
    proc->pc += 2;

    branch = 0;

    switch (relation) {
    case 0:
        if (left == right) {
            branch = 1;
        }
        break;
    case 1:
        if (left != right) {
            branch = 1;
        }
        break;
    case 2:
        comparison = right < left;
        if (comparison) {
            branch = 1;
        }
        break;
    case 3:
        if (left >= right) {
            branch = 1;
        }
        break;
    case 4:
        comparison = left < right;
        if (comparison) {
            branch = 1;
        }
        break;
    case 5:
        comparison = right < left;
        branch = !comparison;
        break;
    }

    if (branch == 1) {
        proc->pc = cmdNum(proc->pc);
    } else {
        proc->pc += 2;
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdOngo);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdOngosub);

int cmdRset(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    int value;

    proc->pc += 2;
    value = cmdNum(proc->pc) & 0xFFFF;
    proc->pc += 2;
    thinkRegSet(regNo & 0xFFFF, value);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdRmath1);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdRmath2);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdRpush);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdRpop);

int cmdTblget(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    int base;
    int index;
    unsigned short value;

    proc->pc += 2;
    base = cmdNum(proc->pc);
    proc->pc += 2;
    index = base + cmdNum(proc->pc) * 2;
    proc->pc += 2;
    value = *(unsigned short *)thinkAdrGet(index);
    thinkRegSet(regNo & 0xFFFF, (int)value);
    return 0;
}

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

/*
 * eBattleWinOpen2's own argument words (src/main/e_battle_win_open.c, still
 * asm there, no published parameter type). The header shape (textId/
 * speaker/flags/priority/target/iconGraphic) and the flags constant
 * 0x00FFFFFD match src/ov01/menu.c's StatNameOpenParams, the same window
 * family's argument record for this same callee; battleMsgPut passes no
 * icon graphic (iconGraphic is NULL) and a fixed priority of 0x138.
 */
struct BattleMsgOpen2Params {
    int textId;              /* +0x00 */
    int speaker;              /* +0x04 */
    int flags;                  /* +0x08 */
    int priority;                 /* +0x0C */
    int target;                     /* +0x10 */
    unsigned char *iconGraphic;      /* +0x14 */
};
extern void eBattleWinOpen2(struct BattleMsgOpen2Params *params);
extern MessageTask *objEntryPure(void (*callback)(MessageTask *));

void battleMsgPut(int textId, int speaker, int target, int frames)
{
    struct BattleMsgOpen2Params params;
    MessageTask *task;

    task = objEntryPure(msgObj);
    task->remaining_frames = frames;
    params.textId = textId;
    params.speaker = speaker;
    params.flags = 0x00FFFFFD;
    params.target = target;
    params.priority = 0x138;
    params.iconGraphic = 0;
    eBattleWinOpen2(&params);
    pMsgObj = task;
}

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

/*
 * eBattleWinOpen4's own argument words (src/main/e_battle_win_open.c, still
 * asm there, no published parameter type). battleMsgPut2 fills textId,
 * speaker and target from its own three parameters (in that order, textId
 * and speaker before the call to objEntryPure returns, target and flags
 * after); flags is always 0x00FFFFFD and priority is always zero at this
 * call site.
 */
struct BattleMsgOpenParams {
    int textId;
    int speaker;
    int flags;
    int priority;
    int target;
};
extern void eBattleWinOpen4(struct BattleMsgOpenParams *params);
extern MessageTask *objEntryPure(void (*callback)(MessageTask *));

void battleMsgPut2(int textId, int speaker, int target)
{
    struct BattleMsgOpenParams params;
    MessageTask *task;

    task = objEntryPure(msgObj2);
    params.textId = textId;
    params.speaker = speaker;
    params.flags = 0x00FFFFFD;
    params.target = target;
    params.priority = 0;
    eBattleWinOpen4(&params);
    pMsgObj = task;
}

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

ObjectTask *thinkUnitPtrGetReg(int reg)
{
    int unitNo = thinkRegGet(reg & 0xFFFF);

    if (unitNo == 0x7FFF) {
        unitNo = unitNoGet(pThinkUnit);
    }
    return unitPtrGet(unitNo);
}

extern int calcBoost(ObjectTask *unit, int mode);
extern int calcBoostChk(ObjectTask *unit, int mode);
extern CalcUnitParam *calcUPGet(ObjectTask *unit);
extern const char D_00A46468[];

int cmdCounterBoost(void)
{
    ObjectTask *unit = thinkUnitPtrGetReg(0x8010);

    if (calcBoostChk(unit, 0) != 0) {
        calcBoost(unit, 0);
        printf(D_00A46468, calcUPGet(unit)->charaId);
    }
    return 0;
}

int cmdStatChk(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    int mask;
    int unitNo;

    proc->pc += 2;
    mask = cmdNum(proc->pc);
    proc->pc += 2;
    unitNo = cmdNum(proc->pc);
    proc->pc += 2;
    thinkRegSet(regNo & 0xFFFF, (((CmdActorFlags *)unitPtrGet(unitNo)->work)->flags & mask) != 0);
    return 0;
}

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

int cmdUnitparaGet(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    int category;
    int unitNo;

    proc->pc += 2;
    category = cmdNum(proc->pc);
    proc->pc += 2;
    unitNo = cmdNum(proc->pc);
    proc->pc += 2;
    thinkRegSet(regNo & 0xFFFF, cmdUnitpara(unitPtrGet(unitNo), category, 0, 0) & 0xFFFF);
    return 0;
}

int cmdUnitparaSet(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    int category;
    int unitNo;
    ObjectTask *unit;

    proc->pc += 2;
    category = cmdNum(proc->pc);
    proc->pc += 2;
    unitNo = cmdNum(proc->pc);
    proc->pc += 2;
    unit = unitPtrGet(unitNo);
    cmdUnitpara(unit, category, thinkRegGet(regNo & 0xFFFF) & 0x7FFF, 1);
    return 0;
}

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

int cmdTecparaGet(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    int category;
    int tecId;

    proc->pc += 2;
    category = cmdNum(proc->pc);
    proc->pc += 2;
    tecId = cmdNum(proc->pc);
    proc->pc += 2;
    thinkRegSet(regNo & 0xFFFF, cmdTecpara(dataTecGet(tecId), category) & 0xFFFF);
    return 0;
}

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

int cmdLightAmb(ThinkProcess *proc)
{
    Vector4 ambient;
    StudioLight *light;
    float red;
    float green;
    float blue;

    red = (float)cmdNum(proc->pc);
    proc->pc += 2;
    ambient.x = red * 0.01f;
    green = (float)cmdNum(proc->pc);
    proc->pc += 2;
    ambient.y = green * 0.01f;
    blue = (float)cmdNum(proc->pc);
    proc->pc += 2;
    ambient.w = 1.0f;
    ambient.z = blue * 0.01f;
    xglStudioGetLight(&light);
    xglLightIntensityAmbient(light, &ambient);
    return 0;
}

int cmdLightCol(ThinkProcess *proc)
{
    Vector4 color;
    StudioLight *light;
    int index;
    float red;
    float green;
    float blue;

    index = cmdNum(proc->pc);
    proc->pc += 2;
    red = (float)cmdNum(proc->pc);
    proc->pc += 2;
    color.x = red * 0.01f;
    green = (float)cmdNum(proc->pc);
    proc->pc += 2;
    color.y = green * 0.01f;
    blue = (float)cmdNum(proc->pc);
    proc->pc += 2;
    color.w = 1.0f;
    color.z = blue * 0.01f;
    xglStudioGetLight(&light);
    xglLightIntensityParallel(light, index, &color);
    return 0;
}

int cmdLightDir(ThinkProcess *proc)
{
    Vector4 direction;
    StudioLight *light;
    int index;
    float x;
    float y;
    float z;

    index = cmdNum(proc->pc);
    proc->pc += 2;
    x = (float)cmdNum(proc->pc);
    proc->pc += 2;
    direction.x = x * 0.01f;
    y = (float)cmdNum(proc->pc);
    proc->pc += 2;
    direction.y = y * 0.01f;
    z = (float)cmdNum(proc->pc);
    proc->pc += 2;
    direction.w = 1.0f;
    direction.z = z * 0.01f;
    xglStudioGetLight(&light);
    xglLightDirection(light, index, &direction);
    return 0;
}

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

int cmdCamOffsPos(ThinkProcess *proc)
{
    McamParams *cam = mcamPtrGet();
    float offsX;
    float offsY;
    float offsZ;

    cam->offsEnabled = 1;
    offsX = (float)cmdNum(proc->pc);
    proc->pc += 2;
    cam->offset.x = offsX * 0.01f;
    offsY = (float)cmdNum(proc->pc);
    proc->pc += 2;
    cam->offset.y = offsY * 0.01f;
    offsZ = (float)cmdNum(proc->pc);
    proc->pc += 2;
    cam->offset.w = 1.0f;
    cam->offset.z = offsZ * 0.01f;
    myMCamSet(camMode, cam);
    return 0;
}

int cmdCamOffsAng(ThinkProcess *proc)
{
    McamParams *cam = mcamPtrGet();
    float offsX;
    float offsY;
    float offsZ;

    cam->offsEnabled = 2;
    offsX = (float)cmdNum(proc->pc);
    cam->angleOffset.x = offsX * 0.01f;
    proc->pc += 2;
    offsY = (float)cmdNum(proc->pc);
    proc->pc += 2;
    cam->angleOffset.y = (offsY / 180.0f) * 3.1415927f;
    offsZ = (float)cmdNum(proc->pc);
    proc->pc += 2;
    cam->angleOffset.z = (offsZ / 180.0f) * 3.1415927f;
    myMCamSet(camMode, cam);
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamMove);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamShake);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamCenterLength);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamUnitHeight);

INCLUDE_ASM("asm/nonmatchings/ov01/cmd", cmdCamCoordGet);

/*
 * mbankPtrGet (this TU, still asm) returns the bank parameter block
 * cmdCamBank fills in; only the member that function touches is modeled.
 */
typedef struct MbankParams {
    unsigned char unmodeled_0[0xA0];
    float angle;                      /* +0xA0: bank angle in radians */
} MbankParams;

extern MbankParams *mbankPtrGet(void);

int cmdCamBank(ThinkProcess *proc)
{
    MbankParams *bank = mbankPtrGet();
    float raw;
    float angle;

    raw = (float)cmdNum(proc->pc);
    angle = ((raw * 0.1f) / 180.0f) * 3.1415927f;
    proc->pc += 2;
    bank->angle = angle;
    myMCamSet(bankMode, bank);
    return 0;
}

/*
 * mpersPtrGet (this TU, still asm) returns the perspective parameter block
 * cmdCamPers fills in; only the member that function touches is modeled.
 */
typedef struct MpersParams {
    unsigned char unmodeled_0[0xA4];
    float value;                      /* +0xA4: tenths-scaled perspective value */
} MpersParams;

extern MpersParams *mpersPtrGet(void);

int cmdCamPers(ThinkProcess *proc)
{
    MpersParams *pers;
    float value;

    pers = mpersPtrGet();
    value = (float)cmdNum(proc->pc) * 0.1f;
    proc->pc += 2;
    pers->value = value;
    myMCamSet(persMode, pers);
    return 0;
}

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

int cmdWaitCnt(ThinkProcess *proc)
{
    int count;
    int result;

    if (!proc->waitActive) {
        proc->sortMode = cmdNum(proc->pc);
        result = 2;
    } else {
        count = proc->sortMode - 1;
        proc->sortMode = count;
        if (count <= 0) {
            proc->pc += 2;
            result = 0;
        } else {
            result = 2;
        }
    }
    return result;
}

extern int MCamIsMoving(int selector);

int cmdWaitCamMove(ThinkProcess *proc)
{
    if (!proc->waitActive) {
        if ((unsigned int)(camMode + 1) < 2) {
            proc->sortMode = 0;
        } else {
            proc->sortMode = 1;
        }
        return 2;
    }
    return MCamIsMoving(proc->sortMode) == 0 ? 0 : 2;
}

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

int cmdMapMulSet(ThinkProcess *proc)
{
    Vector4 mul;
    float rawX;
    float rawY;
    float rawZ;
    float rawW;

    rawX = (float)cmdNum(proc->pc);
    proc->pc += 2;
    mul.x = rawX * 0.01f;
    rawY = (float)cmdNum(proc->pc);
    proc->pc += 2;
    mul.y = rawY * 0.01f;
    rawZ = (float)cmdNum(proc->pc);
    proc->pc += 2;
    mul.z = rawZ * 0.01f;
    rawW = (float)cmdNum(proc->pc);
    proc->pc += 2;
    mul.w = rawW * 0.01f;
    mapMulSet(&mul);
    return 0;
}

int cmdHpPerGet(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    int unitNo;
    ObjectTask *unit;
    CalcUnitParam *up;

    proc->pc += 2;
    unitNo = cmdNum(proc->pc);
    proc->pc += 2;
    unit = unitPtrGet(unitNo);
    up = calcUPGet(unit);
    thinkRegSet(regNo & 0xFFFF,
                ((int)(*(short *)((char *)up + CALC_UNIT_PARAM_HP_OFF) * 100)
                 / *(short *)((char *)calcUPGet(unit) + CALC_UNIT_PARAM_MAX_HP_OFF)) & 0xFFFF);
    return 0;
}

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

/*
 * calcStatReset (src/ov01/calc.c, still asm there) resets one status
 * category's clamp for a unit; calcStatResetCha (src/ov01/calc.c) calls it
 * the same way, with the same always-zero fourth word, and names the
 * parameters this declaration reuses.
 */
extern void calcStatReset(ObjectTask *unit, int category, int mask, int unused);

int cmdStatReset(ThinkProcess *proc)
{
    int category;
    int mask;
    int unitNo;

    category = cmdNum(proc->pc);
    proc->pc += 2;
    mask = cmdNum(proc->pc);
    proc->pc += 2;
    unitNo = cmdNum(proc->pc);
    proc->pc += 2;
    calcStatReset(unitPtrGet(unitNo), category, mask, 0);
    return 0;
}

/*
 * The block cmdDeadTime reaches through the pointer stored just past
 * ObjectTask's modeled task/work prefix; only the field this function
 * writes is modeled.
 */
typedef struct UnitDeadTimeBlock {
    unsigned char unmodeled_0[0xD0];
    float value;    /* +0xD0: reciprocal-scaled duration */
} UnitDeadTimeBlock;

/*
 * Offset 0x14 of the record unitPtrGet returns lands just past ObjectTask's
 * modeled task/work prefix (shared.h); the real per-unit allocation is
 * longer than that partial view (shared.h's own XglTaskPrefix comment
 * documents the same gap for the task-specific payload). cmdDeadTime is the
 * only function in this allocation that reads the pointer stored there.
 */
typedef struct UnitDeadTimeRecord {
    unsigned char unmodeled_0[0x14];
    UnitDeadTimeBlock *target;   /* +0x14 */
} UnitDeadTimeRecord;

int cmdDeadTime(ThinkProcess *proc)
{
    int unitNo;
    int duration;

    unitNo = cmdNum(proc->pc);
    proc->pc += 2;
    duration = cmdNum(proc->pc);
    proc->pc += 2;
    ((UnitDeadTimeRecord *)unitPtrGet(unitNo))->target->value = 1.0f / (float)duration;
    return 0;
}
