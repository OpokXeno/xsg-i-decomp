/*
 * OV01 original TU 6: 0x00a1d348..0x00a22628 (136 functions)
 */
#include "common.h"
#include "ov01/actor.h"
#include "shared.h"
#include "ov01/calc.h"
#include "ov01/obj.h"
#include "cmd.h"

typedef union {
    unsigned char byteValue;
    short halfwordValue;
} CmdTecparaValue;

extern CmdTecparaValue *cmdTecparaSub(CmdTecRecord *tecRecord, short category, short *valueType);

/*
 * Returns int: with a void callee gcc keeps cmdUnitChange's 0x25 command in
 * $2 across the call, while the original reserves $2 for the return value
 * and uses $3 (addiu $3,$0,0x25 at 0x00a1fd3c).
 */

/* The TU-local statics are declared once in cmd.h; only pCamSetTop has no header copy. */
static short *pCamSetTop;
MessageTask *pMsgObj = 0;

const char cmdThinkProcessExecSubErrText[32] = "thinkProcessExecSub: err -> %d\n";
const char cmdCamInitText[24] = "** camInit ** %X\n";
const char cmdThinkInitText[24] = "** thinkInit ** %X\n";
const char cmdThinkTurnStartText[32] = "** thinkTurnStart ** %X\n";
const char cmdThinkTurnEndText[24] = "** thinkTurnEnd ** %X\n";
const char cmdThinkAtkText[32] = "** think Atk (%d) ** %X\n";
const char cmdThinkDmgText[32] = "** think Dmg (%d) ** %X\n";
const char cmdRegErrText[16] = "** reg err %d\n";
extern const char cmdGOSUBStackErrText[];
extern const char cmdRETURNStackErrText[];
extern const char cmdONGOErrText[]; /* "** ONGO err\n": cmdOngo (printf arg when the jump-table index is out of range) */
extern const char cmdONGOSUBErrText[]; /* cmdOngosub printf arg when count < index (same check as cmdOngo) */
extern const char cmdONGOSUBStackErrText[]; /* cmdOngosub printf arg when the return-address stack (ctx->returnDepth) is full */
extern const char cmdRmath1ErrText[]; /* cmdRmath1 printf arg for an unrecognized opIndex */
extern const char cmdRPUSHStackErrText[];
extern const char cmdRPOPStackErrText[];
extern const char cmdJpText2[];
extern const char cmdJpText[];
extern const char cmdRText[];
extern const char cmdCOUNTERBOOSTCHRText[];
extern const char cmdUnitNoErrText[];
extern const char cmdCamCenterLengthErrText[]; /* "** camCenterLength err **\n": cmdCamCenterLength (printf arg when camMode < 0) */
extern float MCamGetLengthActor(int selector);

void thinkSysInit(void)
{
    memset(processBuf, 0, sizeof(processBuf));
    memset(context, 0, sizeof(context));
}

void thinkTopSet(int dataTop)
{
    pDataTop = dataTop;
}

void thinkContextSet(ThinkContext *context)
{
    pContext = context;
}

int thinkAdrGet(short offset)
{
    /* The offset is rounded down to an even byte count before it is added. */
    return pDataTop + (unsigned int)offset / 2 * 2;
}

extern const char cmdThinkProcessAddOverText[];

/*
 * thinkProcessAdd copies a new process record into the first free slot of
 * processBuf, clears its waitActive flag, and returns its slot number (1-16).
 * It returns 0 after printing an overflow message when every slot is in use.
 */
int thinkProcessAdd(ThinkProcess *proc)
{
    ThinkProcess *slot = processBuf;
    int i;

    for (i = 0; i < THINK_PROCESS_COUNT; i++) {
        if (slot->dataTop == 0) {
            proc->waitActive = 0;
            *slot = *proc;
            return i + 1;
        }
        slot++;
    }
    printf(cmdThinkProcessAddOverText);
    return 0;
}

int thinkProcessDel(int *slot)
{
    if (*slot != 0) {
        *slot = 0;
        return 1;
    }
    return 0;
}

/*
 * thinkProcessChk returns whether AI process slot 1-16 (matching
 * thinkProcessKindChk's sixteen-entry processBuf table) has a live entry
 * (dataTop nonzero); an out-of-range slot returns 0.
 */
int thinkProcessChk(int slot)
{
    ThinkProcess *proc;

    if ((unsigned int)(slot - 1) >= THINK_PROCESS_COUNT) {
        return 0;
    }
    proc = &processBuf[slot - 1];
    return proc->dataTop != 0;
}

int thinkProcessKindChk(int kind)
{
    ThinkProcess *proc = processBuf;
    int count = 0;
    int i;

    for (i = 0; i < THINK_PROCESS_COUNT; i++) {
        if (proc->dataTop != 0 && proc->kind == kind) {
            count++;
        }
        proc++;
    }
    return count;
}

int thinkProcessExecSub(ThinkProcess *proc);

int thinkProcessExec(void)
{
    ThinkProcess *proc;
    int i;

    dataVPadSet(0);
    proc = processBuf;
    for (i = 0; i < THINK_PROCESS_COUNT; i++) {
        if (proc->dataTop != 0) {
            thinkProcessExecSub(proc);
        }
        proc++;
    }
    return 1;
}

extern int thinkExecSub(ThinkProcess *proc);
extern const char cmdThinkProcessExecSubErrText[];

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
        printf(cmdThinkProcessExecSubErrText, status);
    }
    return 1;
}

/*
 * dataThinkAdrGet returns the AI script data base, which this TU keeps as an
 * int address (pThinkTop). The TU-local int view matches the call's bytes.
 */
extern int dataThinkAdrGet(void);
extern void monsSetNoSet(int monsSetNo);
extern void mapNoSet(int mapNo);
static int msgPosX;
static int msgPosY;
extern const char cmdMonsSetOverText[];

/*
 * thinkInit resets the AI work area and the command-output state, then
 * validates the monster-set number and selects the map for the battle.
 */
void thinkInit(void)
{
    int monsSetNo;
    int mapNo;

    thinkSysInit();
    pThinkTop = dataThinkAdrGet();
    thinkTopSet(pThinkTop);
    pMonsSetTop = (MonsTblEntry *)thinkAdrGet(*(short *)pThinkTop);
    cmdPutFlag = 0;
    msgPosX = 100;
    msgPosY = 240;
    pMsgObj = 0;
    camMode = 0;
    persMode = 3;
    bankMode = 2;
    monsSetNo = monsSetNoGet();
    if (monsSetNo >= thinkMonsTblNumGet() || monsSetNo < 0) {
        printf(cmdMonsSetOverText, monsSetNo);
        monsSetNoSet(0);
    }
    mapNo = pMonsSetTop[monsSetNoGet()].mapNo;
    if (mapNo == 0) {
        mapNo = mapNoGet();
    }
    mapNoSet(mapNo);
}

extern int mapNoGet(void);
extern void *dataMapCameraAdrGet(int mapNo);

void camInit(void)
{
    pCamTop = (CamTopEntry *)dataMapCameraAdrGet(mapNoGet());
    thinkTopSet((int)pCamTop);
    pCamSetTop = (short *)thinkAdrGet(pCamTop->camTblOffset);
    camTblIdx = 0;
}

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

ThinkMonsEntry *thinkMonsTblGet(int monsSetNo)
{
    thinkTopSet(pThinkTop);
    return (ThinkMonsEntry *)thinkAdrGet(pMonsSetTop[monsSetNo].script_offset);
}

static short *pCamSetTop;

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
extern const char cmdCamInitText[];

int camInitExec(void)
{
    short script = pCamTop->initScript;

    thinkContextSet(&context[1]);
    if (script != 0) {
        thinkRegSet(THINK_REG_CAMERA_FLAG, cameraFlagGet() & 0xFFFF);
        printf(cmdCamInitText, script);
        return camExec(script);
    }
    return 1;
}

extern const char cmdCamEventExecText[];

/*
 * camEventExec looks up the camera-event table entry for the current
 * camTblIdx and matches the record's type. On a hit it publishes the event's
 * unit operands as script registers, then runs the entry's camera script.
 * It returns 1 when no entry matches.
 */
int camEventExec(ThinkCamEvent *event)
{
    short idx;
    int script;

    thinkTopSet((int)pCamTop);
    idx = pCamSetTop[camTblIdx];
    thinkContextSet(&context[1]);
    if (idx != 0) {
        while (1) {
            short type = *(short *)thinkAdrGet(idx);

            if (type == 0) {
                return 1;
            }
            if (type == event->type) {
                script = *(short *)thinkAdrGet(idx + 2);
                break;
            }
            idx += 4;
        }
        {
            int t = event->type;

            switch (t) {
            case 2: case 3: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 20:
                thinkRegSet(THINK_REG_UNIT, unitNoGet(event->unit) & 0xFFFF);
                thinkRegSet(THINK_REG_TARGET, unitNoGet(event->target) & 0xFFFF);
                break;
            case 1: case 5: case 13: case 14: case 15: case 16: case 17: case 19: case 23: case 24: case 25:
                thinkRegSet(THINK_REG_UNIT, unitNoGet(event->unit) & 0xFFFF);
                break;
            }
        }
        thinkRegSet(THINK_REG_ARG, event->operand.arg);
        thinkRegSet(THINK_REG_LIVE_NUM_SIDE0, unitLiveNumGet(0) & 0xFFFF);
        thinkRegSet(THINK_REG_LIVE_NUM_SIDE1, unitLiveNumGet(1) & 0xFFFF);
        thinkRegSet(THINK_REG_CAMERA_FLAG, cameraFlagGet() & 0xFFFF);
        printf(cmdCamEventExecText, event->type, script);
        return camExec(script);
    }
    return 1;
}

extern const char cmdThinkInitText[];

int thinkInitExec(void)
{
    short script = pMonsSetTop[monsSetNoGet()].initScript;

    thinkContextSet(context);
    if (script != 0) {
        printf(cmdThinkInitText, script);
        return thinkExec(script);
    }
    return 1;
}

int thinkTurnStartExec(BattleUnit *unit)
{
    short script = pMonsSetTop[monsSetNoGet()].turnStartScript;

    thinkContextSet(context);
    if (script != 0) {
        thinkRegSet(THINK_REG_UNIT, unitNoGet(unit) & 0xFFFF);
        thinkRegSet(THINK_REG_LIVE_NUM_SIDE0, unitLiveNumGet(0) & 0xFFFF);
        thinkRegSet(THINK_REG_LIVE_NUM_SIDE1, unitLiveNumGet(1) & 0xFFFF);
        printf(cmdThinkTurnStartText, script);
        return thinkExec(script);
    }
    return 1;
}

int thinkTurnEndExec(BattleUnit *unit)
{
    short script = pMonsSetTop[monsSetNoGet()].turnEndScript;

    thinkContextSet(context);
    if (script != 0) {
        thinkRegSet(THINK_REG_UNIT, unitNoGet(unit) & 0xFFFF);
        thinkRegSet(THINK_REG_LIVE_NUM_SIDE0, unitLiveNumGet(0) & 0xFFFF);
        thinkRegSet(THINK_REG_LIVE_NUM_SIDE1, unitLiveNumGet(1) & 0xFFFF);
        printf(cmdThinkTurnEndText, script);
        return thinkExec(script);
    }
    return 1;
}

int thinkUnitAtkExec(BattleUnit *unit)
{
    AtkTblEntry *atkTbl = *(AtkTblEntry **)((char *)calcUPGet(unit) + CALC_UNIT_PARAM_ATK_TBL_OFF);
    short script = atkTbl->script_offset;

    thinkContextSet(context);
    if (script != 0) {
        pThinkUnit = unit;
        thinkRegSet(THINK_REG_UNIT, unitNoGet(unit) & 0xFFFF);
        thinkRegSet(THINK_REG_COMMAND, 1);
        thinkRegSet(THINK_REG_ARG, 0);
        thinkRegSet(THINK_REG_TARGET, 0);
        thinkRegSet(THINK_REG_LIVE_NUM_SIDE0, unitLiveNumGet(0) & 0xFFFF);
        thinkRegSet(THINK_REG_LIVE_NUM_SIDE1, unitLiveNumGet(1) & 0xFFFF);
        printf(cmdThinkAtkText, unitNoGet(unit), script);
        return thinkExec(script);
    }
    return 1;
}

int thinkUnitDmgExec(BattleUnit *unit, UnitDmgInfo *info)
{
    AtkTblEntry *atkTbl = *(AtkTblEntry **)((char *)calcUPGet(unit) + CALC_UNIT_PARAM_ATK_TBL_OFF);
    short script = atkTbl->dmgScript;

    thinkContextSet(context);
    if (script != 0) {
        pThinkUnit = unit;
        thinkRegSet(THINK_REG_UNIT, unitNoGet(unit) & 0xFFFF);
        thinkRegSet(THINK_REG_ATTACKER, unitNoGet(info->attacker) & 0xFFFF);
        thinkRegSet(0x8015, info->reg8015);
        thinkRegSet(0x8016, info->reg8016);
        thinkRegSet(0x8017, info->reg8017);
        thinkRegSet(THINK_REG_LIVE_NUM_SIDE0, unitLiveNumGet(0) & 0xFFFF);
        thinkRegSet(THINK_REG_LIVE_NUM_SIDE1, unitLiveNumGet(1) & 0xFFFF);
        printf(cmdThinkDmgText, unitNoGet(unit), script);
        return thinkExec(script);
    }
    return 1;
}

extern int thinkProcessAdd(ThinkProcess *proc);

int thinkExec(short scriptOffset)
{
    ThinkProcess proc;

    proc.dataTop = pThinkTop;
    proc.pc = scriptOffset;
    proc.context = pContext;
    proc.kind = 0;
    thinkProcessAdd(&proc);
    return 1;
}

int camExec(short scriptOffset)
{
    ThinkProcess proc;

    if (cameraFlagGet() != 0) {
        proc.dataTop = (int)pCamTop;
        proc.pc = scriptOffset;
        proc.context = pContext;
        proc.kind = 1;
        thinkProcessAdd(&proc);
    }
    return 1;
}

int thinkExecChk(void)
{
    return thinkProcessKindChk(0) == 0;
}

static int (*cmdTbl[THINK_COMMAND_COUNT])(ThinkProcess *proc, int cmd);
extern const char cmdErrText[];
extern void debugThinkPrint(short cmd, short pc, short newPc);

/*
 * thinkExecSub runs one script command of a process. Command numbers from 0
 * to 0x62 dispatch through cmdTbl; the handler returns 2 to wait, which rewinds
 * the program counter and sets waitActive. Any other result clears waitActive
 * and, when cmdPutFlag is 1, prints the executed command.
 */
int thinkExecSub(ThinkProcess *proc)
{
    int command;
    short startPc;
    int result;

    command = *(short *)thinkAdrGet(proc->pc);
    if ((unsigned int)command < THINK_COMMAND_COUNT) {
        startPc = proc->pc;
        proc->pc += 2;
        result = cmdTbl[command](proc, command);
        if (result == 2) {
            proc->pc = startPc;
            proc->waitActive = 1;
        } else {
            if (cmdPutFlag == 1) {
                debugThinkPrint(command, startPc, proc->pc);
            }
            proc->waitActive = 0;
        }
    } else {
        printf(cmdErrText);
        result = 1;
    }
    return result;
}

/* pc is the script position the register operand was read from; regChk does not use it. */
int regChk(int operand, int pc)
{
    unsigned short reg = operand;

    if (!(reg & THINK_OPERAND_IS_REG)) {
        printf(cmdRegErrText, reg);
        return 0;
    }
    return 1;
}

short thinkRegNo(short pc)
{
    short *reg_address = (short *)thinkAdrGet(pc);
    short reg = *reg_address;

    regChk(reg & 0xFFFF, pc);
    return reg;
}

/*
 * Script VM register operand, as thinkRegNo reads it from the script and
 * thinkRegGet/thinkRegSet decode it:
 *   bit 15 (0x8000, tested by cmdNum)  the script operand names a register
 *                                      rather than a literal
 *   bit 14  THINK_REG_INDIRECT      the register holds a script offset; the
 *                                   halfword at that offset is the real operand
 *   bits 0-13 THINK_REG_INDEX_MASK  index into the context's register file
 * The register file is the array of halfwords at the start of the think
 * context (pContext); each register holds a 15-bit value (THINK_VALUE_MASK)
 * that thinkValSet sign-extends from bit 14.
 */
#define THINK_REG_INDIRECT 0x4000
#define THINK_REG_INDEX_MASK 0x3FFF
#define THINK_VALUE_MASK 0x7FFF
/* Unit-number sentinel in a script register: the acting unit (pThinkUnit). */
#define THINK_UNIT_SELF 0x7FFF

int thinkValSet(int value)
{
    value &= THINK_VALUE_MASK;

    if (value & 0x4000) {
        value |= ~THINK_VALUE_MASK;
    }
    return value;
}

short thinkRegGet(int regIndex)
{
    unsigned short reg = regIndex;
    short *value_address = (short *)pContext + (reg & THINK_REG_INDEX_MASK);
    unsigned int value = (unsigned int)(int)*value_address;

    if (reg & THINK_REG_INDIRECT) {
        value_address = (short *)thinkAdrGet(value);
        value = (unsigned int)(int)*value_address;
    }
    value = (unsigned int)thinkValSet((short)value);
    return (short)value;
}

void thinkRegSet(int regOperand, int value)
{
    unsigned short regIndex = regOperand;
    unsigned short operand = regIndex;
    unsigned short regValue = value & THINK_VALUE_MASK;
    short *slot;

    regIndex &= THINK_REG_INDEX_MASK;

    if (operand & THINK_REG_INDIRECT) {
        short *indirect = (short *)pContext + regIndex;

        slot = (short *)thinkAdrGet(*indirect);
        *slot = regValue;
    } else {
        slot = (short *)pContext + regIndex;
        *slot = regValue;
    }
}

short cmdReg(short pc)
{
    unsigned short *reg_address = (unsigned short *)thinkAdrGet(pc);
    unsigned short reg = *reg_address;

    regChk(reg, pc);
    return thinkRegGet(reg);
}

short cmdNum(short pc)
{
    short *value_address = (short *)thinkAdrGet(pc);
    short value = *value_address;

    /* Bit 0x8000 marks a register operand; anything else is a literal. */
    if (value & THINK_OPERAND_IS_REG) {
        return cmdReg(pc);
    }
    /*
     * thinkValSet is defined returning int: a short definition adds sll/sra
     * after its `or` (0x00a1e064) in every spelling, while cmdNum passes the
     * value on unextended (0x00a1e1c0..0x00a1e1d0), so the result is taken as
     * the short the callers store (same idiom as regChk in thinkRegNo).
     */
    return ((short (*)(int))thinkValSet)(value);
}

int cmdExit(void)
{
    return 1;
}

int cmdGo(ThinkProcess *proc)
{
    proc->pc = cmdNum(proc->pc);
    return 0;
}

int cmdGosub(ThinkProcess *proc)
{
    ThinkContext *ctx = pContext;

    if (ctx->returnDepth < THINK_RETURN_STACK_SIZE) {
        ctx->returnStack[ctx->returnDepth++] = proc->pc + 2;
    } else {
        printf(cmdGOSUBStackErrText);
        return 1;
    }
    proc->pc = cmdNum(proc->pc);
    return 0;
}

int cmdReturn(ThinkProcess *proc)
{
    ThinkContext *ctx = pContext;

    if (ctx->returnDepth > 0) {
        proc->pc = ctx->returnStack[--ctx->returnDepth];
    } else {
        printf(cmdRETURNStackErrText);
        return 1;
    }
    return 0;
}

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

const char cmdJpText[8] = "@";
const char cmdRText[16] = "R%02d = %d\n";
const char cmdCOUNTERBOOSTCHRText[32] = "** COUNTER BOOST CHR=%d\n";
const char cmdUnitNoErrText[24] = "** unit No err -> %d\n";

int cmdOngo(ThinkProcess *proc)
{
    int index = cmdNum(proc->pc);
    int count;

    proc->pc += 2;
    count = cmdNum(proc->pc);
    proc->pc += 2;

    if (count < index) {
        printf(cmdONGOErrText);
        return 1;
    }
    proc->pc = cmdNum(proc->pc + index * 2);
    return 0;
}

int cmdOngosub(ThinkProcess *proc)
{
    int index = cmdNum(proc->pc);
    int count;
    ThinkContext *ctx;

    proc->pc += 2;
    count = cmdNum(proc->pc);
    proc->pc += 2;

    if (count < index) {
        printf(cmdONGOSUBErrText);
        return 1;
    }

    ctx = pContext;
    if (ctx->returnDepth < THINK_RETURN_STACK_SIZE) {
        ctx->returnStack[ctx->returnDepth++] = proc->pc + count * 2;
    } else {
        printf(cmdONGOSUBStackErrText);
        return 1;
    }

    proc->pc = cmdNum(proc->pc + index * 2);
    return 0;
}

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

/*
 * cmdRmath1's opIndex values (the halfword before the operand) and what the
 * asm does for each: the beq tests at 0x00a1e670/0x00a1e688/0x00a1e698/
 * 0x00a1e6a4 select the case; the arithmetic is the instruction named.
 */
enum Rmath1Op {
    RMATH1_INCREMENT = 8,   /* addiu $2,$3,0x1 at 0x00a1e6bc */
    RMATH1_DECREMENT = 9,   /* addiu $2,$3,-0x1 at 0x00a1e674 */
    RMATH1_COMPLEMENT = 18, /* nor $2,$0,$2 at 0x00a1e6c4 */
    RMATH1_NEGATE = 19      /* nor $2,$0,$2 at 0x00a1e6cc, addiu $2,$2,0x1 at 0x00a1e6d0 */
};

int cmdRmath1(ThinkProcess *proc)
{
    int opIndex = *(short *)thinkAdrGet(proc->pc - 2);
    int regNo = thinkRegNo(proc->pc) & 0xFFFF;
    int regValue;
    unsigned short masked;
    short result;

    proc->pc += 2;
    regValue = thinkRegGet(regNo);

    switch (opIndex) {
    case RMATH1_INCREMENT:
        result = regValue + 1;
        break;
    case RMATH1_DECREMENT:
        result = regValue - 1;
        break;
    case RMATH1_COMPLEMENT:
        masked = regValue & 0xFFFF;
        result = ~masked;
        break;
    case RMATH1_NEGATE:
        masked = regValue & 0xFFFF;
        result = ~masked + 1;
        break;
    default:
        printf(cmdRmath1ErrText);
        return 1;
    }

    thinkRegSet(regNo, result & 0xFFFF);
    return 0;
}

extern int rnd(int max);
extern const char cmdRmath2ErrText[];

/*
 * cmdRmath2 applies a two-operand arithmetic opcode to a script register: the
 * register's value and the following numeric operand are combined, truncated
 * to a halfword, and written back. Opcode 20 takes a random value below the
 * operand. Other opcodes print cmdRmath2ErrText and return 1.
 */
int cmdRmath2(ThinkProcess *proc)
{
    short op = *(short *)thinkAdrGet(proc->pc - 2);
    int regNo = thinkRegNo(proc->pc) & 0xFFFF;
    int n;
    int v;

    proc->pc += 2;
    n = cmdNum(proc->pc);
    proc->pc += 2;
    v = thinkRegGet(regNo);
    switch (op) {
    case 10:
        n = (short)(v + n);
        break;
    case 11:
        n = (short)(v - n);
        break;
    case 12:
        n = (short)(v * n);
        break;
    case 13:
        n = (short)(v / n);
        break;
    case 14:
        n = (short)(v % n);
        break;
    case 15:
        n = (short)(v & n);
        break;
    case 16:
        n = (short)(v | n);
        break;
    case 17:
        n = (short)(v ^ n);
        break;
    case 20:
        n = (short)(rnd(n));
        break;
    default:
        printf(cmdRmath2ErrText);
        return 1;
    }
    thinkRegSet(regNo, n & 0xFFFF);
    return 0;
}

int cmdRpush(ThinkProcess *proc)
{
    int value = cmdNum(proc->pc);
    ThinkContext *ctx = pContext;

    proc->pc += 2;
    if (ctx->valueDepth < THINK_VALUE_STACK_SIZE) {
        ctx->valueStack[ctx->valueDepth++] = value;
    } else {
        printf(cmdRPUSHStackErrText);
        return 1;
    }
    return 0;
}

int cmdRpop(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    ThinkContext *ctx = pContext;
    short value;

    proc->pc += 2;
    if (ctx->valueDepth > 0) {
        value = ctx->valueStack[--ctx->valueDepth];
    } else {
        printf(cmdRPOPStackErrText);
        return 1;
    }
    thinkRegSet(regNo & 0xFFFF, value & 0xFFFF);
    return 0;
}

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

/*
 * TBLSET <register> <table> <entry>: store the register's value into one
 * halfword of a script table. regOperand keeps the register-addressing flags
 * thinkRegGet decodes; tableOffset is the table's script byte offset and
 * entry its decoded halfword index.
 */
int cmdTblset(ThinkProcess *proc)
{
    int regOperand = thinkRegNo(proc->pc);
    int tableOffset;
    int entry;
    int value;
    short *slot;

    proc->pc += 2;
    tableOffset = cmdNum(proc->pc);
    proc->pc += 2;
    entry = cmdNum(proc->pc);
    proc->pc += 2;
    value = thinkRegGet(regOperand & 0xFFFF);
    slot = (short *)thinkAdrGet(tableOffset + entry * 2);
    *slot = value;
    return 0;
}

int cmdRput(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    int reg;

    proc->pc += 2;
    if (regNo & THINK_REG_INDIRECT) {
        printf(cmdJpText);
    }
    /* regNo's low 16 bits, kept in its own register separately from the raw
     * value the indirect-addressing test above reads. */
    reg = 0xFFFF;
    reg = regNo & reg;
    printf(cmdRText, reg & THINK_REG_INDEX_MASK, thinkRegGet(reg));
    return 0;
}

int cmdPrint(ThinkProcess *proc)
{
    short len = 0;
    char *text = (char *)thinkAdrGet(proc->pc);

    printf(cmdJpText2, text);
    while (text[len++] != 0) {
    }
    if (len & 1) {
        len++;
    }
    proc->pc += len;
    return 0;
}

extern const char cmdMemDumpText[];
extern const char cmdJpText3[];
extern const char cmdJpText4[];
extern const char cmdJpText5[];
extern const char cmdJpText6[];
extern const char cmdJpText7[];

/*
 * cmdMemdump prints count halfwords of script memory starting at addr, eight
 * per line, framed by a header and a footer.
 */
int cmdMemdump(ThinkProcess *proc)
{
    int i;
    int count;
    int addr;

    i = 0;
    addr = cmdNum(proc->pc);
    proc->pc += 2;
    count = cmdNum(proc->pc);
    proc->pc += 2;
    printf(cmdMemDumpText);
    printf(cmdJpText3, addr);
    for (; i < count; i++) {
        printf(cmdJpText4, *(short *)thinkAdrGet(addr + i * 2));
        if (i % 8 == 7) {
            printf(cmdJpText5, addr + i * 2 + 2);
        }
    }
    printf(cmdJpText6);
    printf(cmdJpText7);
    return 0;
}

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

/* Reads the next script number into *dst and advances the program counter past it. */
static inline void cmdNumStore(ThinkProcess *proc, int *dst)
{
    *dst = cmdNum(proc->pc);
    proc->pc += 2;
}

/* cmdMsgPos sets the message window position from two script numbers. */
int cmdMsgPos(ThinkProcess *proc)
{
    cmdNumStore(proc, &msgPosX);
    cmdNumStore(proc, &msgPosY);
    return 0;
}

void battleMsgPut(int textId, int speaker, int target, int frames);
void battleMsgPut2(int textId, int speaker, int target);

/*
 * cmdMsg displays the string at the program counter at the current message
 * position. A nonzero script number selects the timed battleMsgPut form.
 * The string is padded to an even length before the number that follows.
 */
int cmdMsg(ThinkProcess *proc)
{
    int len = 0;
    char *str = (char *)thinkAdrGet(proc->pc);
    int n;

    do {
    } while (str[len++] != 0);
    len += len & 1;
    proc->pc = (unsigned short)proc->pc + len;
    n = cmdNum(proc->pc);
    proc->pc = (unsigned short)proc->pc + 2;
    if (n != 0) {
        battleMsgPut(msgPosX, msgPosY, (int)str, n);
    } else {
        battleMsgPut2(msgPosX, msgPosY, (int)str);
    }
    return 0;
}

int battleMsgEndChk(void)
{
    return pMsgObj == 0;
}

/*
 * eBattleWinOpen2's own argument words (src/main/e_battle_win_open.c, no
 * published parameter type). The header shape (textId/
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
        objRemovePure(&task->task);
        pMsgObj = 0;
    }
}

/*
 * eBattleWinOpen4's own argument words (src/main/e_battle_win_open.c, no
 * published parameter type). battleMsgPut2 fills textId,
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
        objRemovePure(&task->task);
        pMsgObj = 0;
    } else {
        eBattleWinMain4();
    }
}

int cmdThinkset(ThinkProcess *proc)
{
    int result;
    int commandList[8];

    result = cmdThinksetSub(proc);
    commandList[0] = 8;
    unitCmdListSet(pThinkUnit, commandList);
    return result;
}

void cmdThinkset2(ThinkProcess *proc)
{
    cmdThinksetSub(proc);
}

extern const char cmdTHINKSETErrTgtText[];
extern const char cmdThinkSetErrNormText[];
extern const char cmdThinkSetEtherLOSTText[];
extern const char cmdThinkSetErrEtherText[];
extern const char cmdTHINKSETErrCmdText[];

/* The technique id of an ether record, at +6, as cmdThinksetSub reads it. */
typedef struct EtherRecord {
    unsigned char unmodeled_00[6];
    short tecId;
} EtherRecord;

extern EtherRecord *dataEthGet(int id);

/*
 * cmdThinksetSub applies one script-selected unit setting from the three
 * script registers THINK_REG_COMMAND, THINK_REG_ARG and THINK_REG_TARGET
 * (the target unit). It builds the command list for unitCmdListSet on the acting unit.
 * An unknown command prints cmdTHINKSETErrCmdText and does nothing.
 */
int cmdThinksetSub(ThinkProcess *proc)
{
    int list[8];
    int cmd = thinkRegGet(THINK_REG_COMMAND);
    int arg = thinkRegGet(THINK_REG_ARG);
    int tgt = thinkRegGet(THINK_REG_TARGET);
    BattleUnit *unit = unitPtrGet(tgt);
    short id;

    if (unit == 0) {
        printf(cmdTHINKSETErrTgtText, tgt);
        unit = unitPtrGet(0);
    }
    switch (cmd) {
    case 1:
        {
            short tec = calcUPGet(pThinkUnit)->normalTechniqueId[arg];

            if (tec != 0) {
                list[0] = 1;
                list[1] = tec;
                list[2] = (int)unit;
                list[3] = -1;
                unitCmdListSet(pThinkUnit, list);
            } else {
                printf(cmdThinkSetErrNormText, arg);
            }
        }
        break;
    case 2:
        if (calcStatGet(pThinkUnit, 1, 0x1000) != 0) {
            printf(cmdThinkSetEtherLOSTText, arg);
        } else {
            id = ((short *)((char *)calcUPGet(pThinkUnit) + 0x8E))[arg];
            if (id != 0) {
                int tec = dataEthGet(id)->tecId;

                list[0] = 2;
                list[1] = tec;
                list[2] = (int)unit;
                list[3] = id;
                unitCmdListSet(pThinkUnit, list);
            } else {
                printf(cmdThinkSetErrEtherText, arg);
            }
        }
        break;
    case 3:
        list[0] = 3;
        list[1] = 0;
        list[2] = (int)unit;
        list[3] = arg;
        unitCmdListSet(pThinkUnit, list);
        break;
    case 4:
        list[0] = 4;
        unitCmdListSet(pThinkUnit, list);
        break;
    case 6:
        if ((unsigned short)(arg - 10) < 0x1A) {
            list[0] = 6;
            list[1] = -arg;
            unitCmdListSet(pThinkUnit, list);
        }
        break;
    case 7:
        list[0] = 0xD;
        list[1] = arg;
        list[2] = 0;
        unitCmdListSet(pThinkUnit, list);
        break;
    case 8:
        list[0] = 0x2E;
        unitCmdListSet(pThinkUnit, list);
        break;
    default:
        printf(cmdTHINKSETErrCmdText, cmd);
    case 0:
        break;
    }
    return 0;
}

BattleUnit *thinkUnitPtrGetReg(int reg)
{
    int unitNo = thinkRegGet(reg & 0xFFFF);

    if (unitNo == THINK_UNIT_SELF) {
        unitNo = unitNoGet(pThinkUnit);
    }
    return unitPtrGet(unitNo);
}

extern int calcBoostChk(BattleUnit *unit, int mode);


int cmdCounterBoost(void)
{
    BattleUnit *unit = thinkUnitPtrGetReg(THINK_REG_UNIT);

    if (calcBoostChk(unit, 0) != 0) {
        calcBoost(unit, 0);
        printf(cmdCOUNTERBOOSTCHRText, calcUPGet(unit)->charaId);
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
    thinkRegSet(regNo & 0xFFFF, (unitPtrGet(unitNo)->work->flags & mask) != 0);
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
    BattleUnit *unit;

    proc->pc += 2;
    category = cmdNum(proc->pc);
    proc->pc += 2;
    unitNo = cmdNum(proc->pc);
    proc->pc += 2;
    unit = unitPtrGet(unitNo);
    cmdUnitpara(unit, category, thinkRegGet(regNo & 0xFFFF) & THINK_VALUE_MASK, 1);
    return 0;
}

typedef union {
    signed char byteValue;
    short halfwordValue;
} CmdUnitParaValue;

extern CmdUnitParaValue *cmdUnitparaSub(BattleUnit *unit, short category, short *valueType);

int cmdUnitpara(BattleUnit *unit, int category, int value, int mode)
{
    short truncValue = value;
    short valueType;
    CmdUnitParaValue *unitParaValue = cmdUnitparaSub(unit, category, &valueType);
    int result;

    if (valueType == 1) {
        result = unitParaValue->byteValue;
        if (mode == 1) {
            unitParaValue->byteValue = truncValue;
        }
    } else {
        result = unitParaValue->halfwordValue;
        if (mode == 1) {
            unitParaValue->halfwordValue = truncValue;
        }
    }
    return result;
}

extern const char cmdUNITPARAErrText[];

/*
 * cmdUnitparaSub locates one unit parameter inside the calcUPGet record of a
 * unit. It returns the field's address and stores its width in *size (1 or
 * 2 bytes) for the caller. Category 0 to 26 selects the field; any other
 * category prints cmdUNITPARAErrText and returns 0.
 */
CmdUnitParaValue *cmdUnitparaSub(BattleUnit *unit, short category, short *size)
{
    short c = category;
    char *base = (char *)calcUPGet(unit);
    char *p;

    switch (c) {
    case 0:
        p = base + 0x0;
        *size = 2;
        break;
    case 1:
        p = base + 0x2;
        *size = 2;
        break;
    case 2:
        p = base + 0x4;
        *size = 2;
        break;
    case 3:
        p = base + 0x6;
        *size = 2;
        break;
    case 4:
        p = base + 0x8;
        *size = 2;
        break;
    case 5:
        p = base + 0xa;
        *size = 2;
        break;
    case 6:
        p = base + 0xc;
        *size = 1;
        break;
    case 7:
        p = base + 0xd;
        *size = 1;
        break;
    case 8:
        p = base + 0xe;
        *size = 1;
        break;
    case 9:
        p = base + 0xf;
        *size = 1;
        break;
    case 10:
        p = base + 0x10;
        *size = 2;
        break;
    case 11:
        p = base + 0x12;
        *size = 2;
        break;
    case 12:
        p = base + 0x14;
        *size = 1;
        break;
    case 13:
        p = base + 0x15;
        *size = 1;
        break;
    case 14:
        p = base + 0x16;
        *size = 1;
        break;
    case 15:
        p = base + 0x17;
        *size = 1;
        break;
    case 16:
        p = base + 0x18;
        *size = 2;
        break;
    case 17:
        p = base + 0x1a;
        *size = 2;
        break;
    case 18:
        p = base + 0x1c;
        *size = 2;
        break;
    case 19:
        p = base + 0x1e;
        *size = 2;
        break;
    case 20:
        p = base + 0x34;
        *size = 2;
        break;
    case 21:
        p = base + 0x36;
        *size = 2;
        break;
    case 22:
        p = base + 0x38;
        *size = 2;
        break;
    case 23:
        p = base + 0x44;
        *size = 2;
        break;
    case 24:
        p = base + 0x3a;
        *size = 1;
        break;
    case 25:
        p = base + 0x20;
        *size = 1;
        break;
    case 26:
        p = base + 0x3c;
        *size = 1;
        break;
    default:
        printf(cmdUNITPARAErrText, c);
        return 0;
    }
    return (CmdUnitParaValue *)p;
}

/* unitTblGet's table of battle units (src/ov01/unit_cmd.c, BattleUnit ***). */
extern int unitTblGet(int side, BattleUnit ***table);

int cmdUnittbl(ThinkProcess *proc)
{
    int regNo;
    int outStart;
    int adr;
    int side;
    BattleUnit **table;
    int i = 0;
    int count;
    short successCount = 0;

    regNo = thinkRegNo(proc->pc);
    proc->pc += 2;
    outStart = cmdNum(proc->pc);
    proc->pc += 2;
    side = cmdNum(proc->pc);
    proc->pc += 2;

    count = unitTblGet(side, &table);

    if (count > 0) {
        adr = outStart;
        for (; i < count; i++) {
            if (table[i] != 0 && !(table[i]->work->flags & ACTOR_FLAG_DEAD)) {
                *(short *)thinkAdrGet(adr) = unitNoGet(table[i]);
                adr += 2;
                successCount++;
            }
        }
    }

    thinkRegSet(regNo & 0xFFFF, successCount & 0xFFFF);
    return 0;
}

static short posTbl[36];
extern BattleUnit *unitEnCreate(short *pl);

/*
 * cmdUnitCreate creates a unit from the script's record at adr. It gathers
 * the ids in [start, hi] that no live unit occupies (posId, sign ignored),
 * picks one at random for the new unit, and queues two commands on it: 0x2A
 * with the sync value and a random unitCmdCode step. The result is the new
 * unit's slot number, or -1 when no id is free or the unit cannot be created.
 */
int cmdUnitCreate(ThinkProcess *proc)
{
    int list[8];
    BattleUnit **tbl;
    int regNo;
    short adr;
    int lo;
    int start;
    int unitCmdCode;
    int hi;
    int n;
    int i;
    int found;
    int cnt;
    BattleUnit *unit;
    int val;
    int idx;
    short *wp;

    cnt = 0;
    regNo = thinkRegNo(proc->pc);
    proc->pc += 2;
    adr = cmdNum(proc->pc);
    proc->pc += 2;
    start = cmdNum(proc->pc);
    unitCmdCode = 0x25;
    proc->pc += 2;
    hi = cmdNum(proc->pc);
    proc->pc += 2;
    n = unitTblGet(1, &tbl);
    lo = start;
    if (lo <= hi) {
        wp = posTbl;
        do {
            found = 0;
            for (i = 0; i < n; i++) {
                if (tbl[i] != 0) {
                    if ((calcUPGet(tbl[i])->posId < 0) ? (-calcUPGet(tbl[i])->posId == lo)
                                                       : (calcUPGet(tbl[i])->posId == lo)) {
                        found = 1;
                        break;
                    }
                }
            }
            if (!found) {
                *wp++ = lo;
                cnt++;
            }
            lo++;
        } while (lo <= hi);
    }
    if (cnt > 0) {
        short *pl = (short *)thinkAdrGet(adr);

        idx = rnd(cnt - 1);
        pl[1] = posTbl[idx];
        lo = dataCdSync();
        unit = unitEnCreate(pl);
        if (unit != 0) {
            list[0] = 0x2A;
            list[1] = lo;
            unitCmdListSet(unit, list);
            list[0] = unitCmdCode;
            list[1] = rnd(8);
            unitCmdListSet(unit, list);
            val = (short)unitNoGet(unit);
        } else {
            val = -1;
        }
    } else {
        val = -1;
    }
    thinkRegSet(regNo & 0xFFFF, val & 0xFFFF);
    return 0;
}

int cmdUnitChange(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    int targetNo;
    BattleUnit *unit;
    int changed;
    int distance;
    int commandList[8];

    proc->pc += 2;
    targetNo = cmdNum(proc->pc);
    proc->pc += 2;
    unit = thinkUnitPtrGetReg(THINK_REG_UNIT);
    changed = 0;
    if (unit != 0) {
        commandList[0] = 0x2B;
        if (calcUPGet(unit)->posId < 0) {
            distance = -calcUPGet(unit)->posId;
        } else {
            distance = calcUPGet(unit)->posId;
        }
        commandList[1] = distance;
        commandList[2] = targetNo;
        unitCmdListSet(unit, commandList);
        commandList[0] = 0x25;
        commandList[1] = 0;
        unitCmdListSet(unit, commandList);
        changed = 1;
    }
    thinkRegSet(regNo & 0xFFFF, changed);
    return 0;
}

/*
 * Builds the attack table at script offset outStart: for every one of the
 * unit's normal techniques, every eligible live target adds a (technique
 * slot, unit number) pair after entry 0, which finally receives the pair
 * count (also stored in the script register regNo). Eligible means: on the side
 * calcTrg2PE selects from the technique's target flags (unitTblGet's table),
 * not flagged dead (andi $2,$2,0x80 at 0x00a1fe9c) and accepted by calcAtkChk.
 */
int cmdAtktbl(ThinkProcess *proc)
{
    int regNo;
    short outStart;
    int tecCount;
    BattleUnit *unit;
    BattleUnit **table;
    short entry = 1;
    int slot;
    short tec;
    int targetCount;
    int i;

    regNo = thinkRegNo(proc->pc);
    proc->pc += 2;
    outStart = cmdNum(proc->pc);
    proc->pc += 2;
    tecCount = cmdNum(proc->pc);
    proc->pc += 2;
    unit = thinkUnitPtrGetReg(THINK_REG_UNIT);
    for (slot = 0; slot < tecCount; slot++) {
        tec = calcUPGet(unit)->normalTechniqueId[slot];
        if (tec != 0) {
            targetCount = unitTblGet(calcTrg2PE(unit, dataTecGet(tec)->attr), &table);
            for (i = 0; i < targetCount; i++) {
                if (table[i] != 0 && !(table[i]->work->flags & ACTOR_FLAG_DEAD)) {
                    if (calcAtkChk(unit, table[i], tec)) {
                        *(short *)thinkAdrGet(outStart + entry * 2) = slot;
                        entry++;
                        *(short *)thinkAdrGet(outStart + entry * 2) = unitNoGet(table[i]);
                        entry++;
                    }
                }
            }
        }
    }
    /* The table's entry count becomes its pair count, stored in entry 0. */
    entry = (entry - 1) / 2;
    thinkRegSet(regNo & 0xFFFF, entry & 0xFFFF);
    *(short *)thinkAdrGet(outStart) = entry;
    return 0;
}

extern const char cmdATKSETOVERText[];

/*
 * cmdAtkset copies two halfwords of an attack-table entry into script
 * registers THINK_REG_ARG and THINK_REG_TARGET. The table starts with a count halfword at base;
 * entry idx is at base + idx * 4 (its index is sign-extended from 15 bits
 * before the scaling). An index at or past the count prints cmdATKSETOVERText and
 * uses entry 0.
 */
int cmdAtkset(ThinkProcess *proc)
{
    short base;
    int idx;
    short max;
    int off;

    base = cmdNum(proc->pc);
    proc->pc += 2;
    idx = cmdNum(proc->pc);
    proc->pc += 2;
    max = *(short *)thinkAdrGet(base);
    if (idx >= max) {
        printf(cmdATKSETOVERText, idx, max);
        idx = 0;
    }
    off = base + ((idx << 17) >> 15);
    thinkRegSet(THINK_REG_ARG, *(unsigned short *)thinkAdrGet(off + 2));
    thinkRegSet(THINK_REG_TARGET, *(unsigned short *)thinkAdrGet(off + 4));
    return 0;
}

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

/*
 * Sorts the (technique slot, unit number) pairs of the attack table at the
 * script offset of the first operand by the unit parameter the second operand
 * selects: descending (largest first) when cmdAtktblSort called (sortMode 0,
 * a larger value moves into the slot), ascending from cmdAtktblSortRev
 * (sortMode 1, a smaller one does). A sort that swaps on every improvement, over the table's own
 * script memory, every access going through thinkAdrGet.
 */
int cmdAtktblSortSub(ThinkProcess *proc)
{
    short valueType;
    short tableStart;
    short category;
    int count;
    int best = 0;    /* read by the movz before any write (sw $0,0x14($29) at 0x00a200d8) */
    int i;
    int j;
    int value;
    short jSlot;
    short iSlot;
    short jUnit;
    short iUnit;
    CmdUnitParaValue *unitParaValue;

    tableStart = cmdNum(proc->pc);
    proc->pc += 2;
    category = cmdNum(proc->pc);
    proc->pc += 2;
    count = *(short *)thinkAdrGet(tableStart);
    tableStart += 2;
    for (i = 0; i < count - 1; i++) {
        for (j = i; j < count; j++) {
            unitParaValue = cmdUnitparaSub(unitPtrGet(*(short *)thinkAdrGet(tableStart + j * 4 + 2)),
                                           category, &valueType);
            value = (valueType == 1) ? unitParaValue->byteValue : unitParaValue->halfwordValue;
            if (i == j) {
                best = value;
            }
            if ((proc->sortMode == 0 && best < value) || (proc->sortMode == 1 && value < best)) {
                /* Swap pair j into slot i, slot halves first. */
                jSlot = *(short *)thinkAdrGet(tableStart + j * 4);
                iSlot = *(short *)thinkAdrGet(tableStart + i * 4);
                *(short *)thinkAdrGet(tableStart + j * 4) = iSlot;
                *(short *)thinkAdrGet(tableStart + i * 4) = jSlot;
                jUnit = *(short *)thinkAdrGet(tableStart + j * 4 + 2);
                iUnit = *(short *)thinkAdrGet(tableStart + i * 4 + 2);
                *(short *)thinkAdrGet(tableStart + j * 4 + 2) = iUnit;
                *(short *)thinkAdrGet(tableStart + i * 4 + 2) = jUnit;
                best = value;
            }
        }
    }
    return 0;
}

/*
 * Runs the script at the third operand once per (technique slot, unit number)
 * pair of the attack table at the first operand, with the unit number in
 * register THINK_REG_UNIT and the technique slot in THINK_REG_ARG, and copies
 * every pair the script accepts (register THINK_REG_ACCEPT nonzero) into the
 * table at the second operand, whose entry 0 receives the
 * accepted pair count (also stored in the script register regNo). The process's
 * pc and both registers are restored afterwards.
 */
int cmdAtktblSearch(ThinkProcess *proc)
{
    int regNo;
    short srcTable;
    short outStart;
    short script;
    int savedUnitReg;
    int savedSlotReg;
    int count;
    short savedPc;
    short slot;
    short unitNo;
    int entry = 1;
    int i;

    regNo = thinkRegNo(proc->pc);
    proc->pc += 2;
    srcTable = cmdNum(proc->pc);
    proc->pc += 2;
    outStart = cmdNum(proc->pc);
    proc->pc += 2;
    script = cmdNum(proc->pc);
    proc->pc += 2;
    savedUnitReg = thinkRegGet(THINK_REG_UNIT);
    savedSlotReg = thinkRegGet(THINK_REG_ARG);
    count = *(short *)thinkAdrGet(srcTable);
    srcTable += 2;
    savedPc = proc->pc;
    for (i = 0; i < count; i++) {
        slot = *(short *)thinkAdrGet(srcTable + i * 4);
        unitNo = *(short *)thinkAdrGet(srcTable + i * 4 + 2);
        thinkRegSet(THINK_REG_UNIT, unitNo & 0xFFFF);
        thinkRegSet(THINK_REG_ARG, slot & 0xFFFF);
        proc->pc = script;
        while (thinkExecSub(proc) == 0) {
        }
        if (thinkRegGet(THINK_REG_ACCEPT) != 0) {
            *(short *)thinkAdrGet(outStart + entry * 2) = slot;
            entry++;
            *(short *)thinkAdrGet(outStart + entry * 2) = unitNo;
            entry++;
        }
    }
    proc->pc = savedPc;
    thinkRegSet(THINK_REG_ARG, savedSlotReg & 0xFFFF);
    thinkRegSet(THINK_REG_UNIT, savedUnitReg & 0xFFFF);
    /* Entries after the count word come in pairs. */
    entry = (entry - 1) / 2;
    thinkRegSet(regNo & 0xFFFF, entry & 0xFFFF);
    *(short *)thinkAdrGet(outStart) = entry;
    return 0;
}

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

int cmdTecpara(CmdTecRecord *tecRecord, int category)
{
    short valueType;
    CmdTecparaValue *valueAddress = cmdTecparaSub(tecRecord, category, &valueType);

    if (valueType == 1) {
        return valueAddress->byteValue;
    }
    return valueAddress->halfwordValue;
}

extern const char cmdTECPARAErrText[];

/*
 * cmdTecparaSub returns the address of one technique parameter inside a
 * technique record and stores its width in *valueType (1 byte or 2 bytes).
 * Categories 0 to 9 select the field; any other category prints cmdTECPARAErrText
 * and returns 0.
 */
CmdTecparaValue *cmdTecparaSub(CmdTecRecord *tecRecord, short category, short *valueType)
{
    switch (category) {
    case 0: *valueType = 1; break;
    case 1: tecRecord += 2; *valueType = 2; break;
    case 2: tecRecord += 4; *valueType = 1; break;
    case 3: tecRecord += 5; *valueType = 1; break;
    case 4: tecRecord += 12; *valueType = 1; break;
    case 5: tecRecord += 13; *valueType = 1; break;
    case 6: tecRecord += 14; *valueType = 2; break;
    case 7: tecRecord += 16; *valueType = 1; break;
    case 8: tecRecord += 17; *valueType = 1; break;
    case 9: tecRecord += 18; *valueType = 2; break;
    default:
        printf(cmdTECPARAErrText, category);
        return 0;
    }
    return (CmdTecparaValue *)tecRecord;
}

int cmdMonsSetNoGet(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);

    proc->pc += 2;
    thinkRegSet(regNo & 0xFFFF, monsSetNoGet() & 0xFFFF);
    return 0;
}

int cmdPartySizeGet(ThinkProcess *proc)
{
    int regNo;
    int side;
    BattleUnit **table;
    int i = 0;
    int count;
    int maxSize = 0;

    regNo = thinkRegNo(proc->pc);
    proc->pc += 2;
    side = cmdNum(proc->pc);
    proc->pc += 2;

    count = unitTblGet(side, &table);
    for (; i < count; i++) {
        if (table[i] != 0 && maxSize < calcUPGet(table[i])->sizeClass) {
            maxSize = calcUPGet(table[i])->sizeClass;
        }
    }

    thinkRegSet(regNo & 0xFFFF, maxSize & 0xFFFF);
    return 0;
}

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

static McamParams mcamView;
static McamParams mcamLookAt;
static McamParams mcamViewMove;
static McamParams mcamLookAtMove;

/* The camera parameter block the current camMode edits (see
 * cmdCamCamMode/cmdCamRefMode/cmdCamCamMoveMode/cmdCamRefMoveMode). */
McamParams *mcamPtrGet(void)
{
    McamParams *cam;

    switch (camMode) {
    case -1:
        cam = &mcamViewMove;
        break;
    case -2:
        cam = &mcamLookAtMove;
        break;
    case 0:
        cam = &mcamView;
        break;
    case 1:
        cam = &mcamLookAt;
        break;
    default:
        cam = 0;
        break;
    }
    return cam;
}

struct MpersParams;
static struct MpersParams mcamPers;
static struct MpersParams mcamPersMove;

struct MpersParams *mpersPtrGet(void)
{
    return persMode == -3 ? &mcamPersMove : persMode == 3 ? &mcamPers : 0;
}

struct MbankParams;
static struct MbankParams mcamBank;
static struct MbankParams mcamBankMove;

struct MbankParams *mbankPtrGet(void)
{
    return bankMode == -4 ? &mcamBankMove : bankMode == 2 ? &mcamBank : 0;
}

void myMCamSet(int mode, void *params)
{
    if (mode >= 0) {
        MCamSet(mode, params);
    }
}

int unitNoChk(int unitNo)
{
    if (unitNo >= 8) {
        printf(cmdUnitNoErrText, unitNo);
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

extern const char cmdCamLockErrText[];

/*
 * cmdCamLock locks the camera on the current camera point: it copies the
 * point's coordinate into the lock position, clears the unit-track mode and
 * the offset flag, and applies the result with myMCamSet. camMode below zero
 * prints cmdCamLockErrText and does nothing.
 */
/*
 * Partial view of McamParams (measured: the zero fields are stored through
 * pointer-typed members, which fixes the store and load order of the emitted
 * code; the int fields of McamParams give a different order). Used only by
 * cmdCamLock.
 */
typedef struct {
    void *unitTrackMode;   /* +0x00: mode selector, cleared by this function */
    int pad4[3];
    Vector4 lockPosition;  /* +0x10 */
    unsigned char pad20[0x70 - 0x20];
    void *offsEnabled;     /* +0x70: cleared by this function */
} McamLockView;

int cmdCamLock(void)
{
    Vector4 v;
    McamLockView *cam;

    if (camMode < 0) {
        printf(cmdCamLockErrText, camMode);
        return 0;
    }
    /*
     * MCamGetCurrentCoord is called with only the output pointer set; the
     * second argument register is not loaded, so the callee is called
     * through the one-argument type.
     */
    ((Vector4 *(*)(Vector4 *))MCamGetCurrentCoord)(&v);
    cam = (McamLockView *)mcamPtrGet();
    cam->lockPosition.x = v.x;
    cam->unitTrackMode = 0;
    cam->offsEnabled = 0;
    cam->lockPosition.y = v.y;
    cam->lockPosition.z = v.z;
    cam->lockPosition.w = 1.0f;
    myMCamSet(camMode, cam);
    return 0;
}

/* The w store last gives the original's order of the four swc1 stores. */
int cmdCamCopy(void)
{
    Vector4 coord;
    McamParams *cam;
    int point;

    if (camMode == 1) {
        point = 0;
    } else if (camMode == 0) {
        point = 1;
    } else {
        printf(cmdCamCopyErrText);
        return 0;
    }
    MCamGetCurrentCoord(&coord, point);
    cam = mcamPtrGet();
    cam->unitTrackMode = 0;
    cam->offsEnabled = 0;
    cam->lockPosition.x = coord.x;
    cam->lockPosition.y = coord.y;
    cam->lockPosition.z = coord.z;
    cam->lockPosition.w = 1.0f;
    myMCamSet(camMode, cam);
    return 0;
}

int cmdCamSetPos(ThinkProcess *proc)
{
    McamParams *cam = mcamPtrGet();
    float posZ;

    cam->unitTrackMode = 0;
    cam->lockPosition.x = (float)cmdNum(proc->pc) * 0.01f;
    proc->pc += 2;
    cam->lockPosition.y = (float)cmdNum(proc->pc) * 0.01f;
    proc->pc += 2;
    posZ = (float)cmdNum(proc->pc);
    proc->pc += 2;
    cam->lockPosition.w = 1.0f;
    cam->offsEnabled = 0;
    cam->lockPosition.z = posZ * 0.01f;
    myMCamSet(camMode, cam);
    return 0;
}

int cmdCamSetAct(ThinkProcess *proc)
{
    int unitNo;
    McamParams *cam = mcamPtrGet();
    BattleUnit *unit;
    int actId;
    float offsX;
    float offsY;
    float offsZ;

    unitNo = unitNoChk(cmdNum(proc->pc));

    cam->unitTrackMode = 1;

    unit = unitPtrGet(unitNo);
    cam->targetBlock = unit->actor;
    unit = unitPtrGet(unitNo);
    cam->charaId = calcUPGet(unit)->charaId;

    proc->pc += 2;
    actId = cmdNum(proc->pc);
    proc->pc += 2;
    cam->offsEnabled = 1;
    cam->actId = actId;

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

int cmdCamSetAct2(ThinkProcess *proc)
{
    McamParams *cam = mcamPtrGet();
    int firstUnitNo;
    int secondUnitNo;
    BattleUnit *unit;
    float offsX;
    float offsY;
    float offsZ;
    float ratio;

    firstUnitNo = unitNoChk(cmdNum(proc->pc));
    cam->unitTrackMode = 2;
    unit = unitPtrGet(firstUnitNo);
    cam->pairTarget[0].targetBlock = unit->actor;
    unit = unitPtrGet(firstUnitNo);
    cam->pairTarget[0].charaId = calcUPGet(unit)->charaId;
    proc->pc += 2;
    cam->pairTarget[0].actId = cmdNum(proc->pc);
    proc->pc += 2;

    secondUnitNo = unitNoChk(cmdNum(proc->pc));
    unit = unitPtrGet(secondUnitNo);
    cam->pairTarget[1].targetBlock = unit->actor;
    unit = unitPtrGet(secondUnitNo);
    cam->pairTarget[1].charaId = calcUPGet(unit)->charaId;
    proc->pc += 2;
    cam->pairTarget[1].actId = cmdNum(proc->pc);
    proc->pc += 2;

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

    ratio = (float)cmdNum(proc->pc);
    proc->pc += 2;
    cam->ratio = ratio * 0.01f;

    myMCamSet(camMode, cam);
    return 0;
}

/*
 * Two-unit form of cmdCamSetAct. Each unit number has its own variable
 * (firstUnitNo, secondUnitNo): with a single shared unit-number variable the
 * allocator ranks it ahead of proc and cam for the callee-saved registers
 * $16..$18, which permutes all three registers and changes the bytes.
 */
int cmdCamSetAct3(ThinkProcess *proc)
{
    McamParams *cam = mcamPtrGet();
    int firstUnitNo;
    int secondUnitNo;
    BattleUnit *unit;
    float offsX;
    float offsY;
    float offsZ;

    firstUnitNo = unitNoChk(cmdNum(proc->pc));
    cam->unitTrackMode = 3;
    unit = unitPtrGet(firstUnitNo);
    cam->pairTarget[0].targetBlock = unit->actor;
    unit = unitPtrGet(firstUnitNo);
    cam->pairTarget[0].charaId = calcUPGet(unit)->charaId;
    proc->pc += 2;
    cam->pairTarget[0].actId = cmdNum(proc->pc);
    proc->pc += 2;

    secondUnitNo = unitNoChk(cmdNum(proc->pc));
    unit = unitPtrGet(secondUnitNo);
    cam->pairTarget[1].targetBlock = unit->actor;
    unit = unitPtrGet(secondUnitNo);
    cam->pairTarget[1].charaId = calcUPGet(unit)->charaId;
    proc->pc += 2;
    cam->pairTarget[1].actId = cmdNum(proc->pc);
    proc->pc += 2;

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

/* Camera move record: a raw copy of one camera parameter block, followed by the script operands. */
typedef struct CamMoveBlk {
    long long raw[0x16];
} CamMoveBlk;

typedef struct CamMoveArg {
    CamMoveBlk blk;
    int a;
    int b;
    int c;
    short d;
} CamMoveArg;

void MCamMove(int mode, CamMoveArg *arg);

int cmdCamMove(ThinkProcess *proc)
{
    CamMoveArg arg;
    int kind;
    int mode;
    void *src;

    cmdNumStore(proc, &arg.a);
    cmdNumStore(proc, &arg.b);
    cmdNumStore(proc, &arg.c);
    arg.d = cmdNum(proc->pc);
    proc->pc += 2;
    kind = cmdNum(proc->pc);
    proc->pc += 2;
    if (kind == 0) {
        if ((unsigned int)(camMode + 1) < 2) {
            mode = 0;
            src = &mcamViewMove;
        } else {
            mode = 1;
            src = &mcamLookAtMove;
        }
    } else if (kind == 1) {
        mode = 3;
        src = &mcamPersMove;
    } else if (kind == 2) {
        mode = 2;
        src = &mcamBankMove;
    } else {
        return 0;
    }
    arg.blk = *(CamMoveBlk *)src;
    MCamMove(mode, &arg);
    return 0;
}

/*
 * MCamShake (src/ov01/m_cam.c; no C definition or other declaration exists
 * in this tree). The five unsigned int arguments are read off its code: $a0
 * is stored into the shake record (sw $4,0x0($9), 0x00a31ccc), $a3 is stored
 * raw (sw $7,0x18($9), 0x00a31d94), and $a1, $a2 and $a4 are converted from
 * unsigned to float (0x00a31cd0, 0x00a31d40, 0x00a31da8); cmdCamShake passes
 * it five script-decoded values. The parameter names below are guesses.
 */
extern void MCamShake(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

int cmdCamShake(ThinkProcess *proc)
{
    int mode;
    int amplitude;
    int phaseStep;
    int holdFrames;
    int decayFrames;

    mode = cmdNum(proc->pc);
    proc->pc += 2;
    amplitude = cmdNum(proc->pc);
    proc->pc += 2;
    phaseStep = cmdNum(proc->pc);
    proc->pc += 2;
    holdFrames = cmdNum(proc->pc);
    proc->pc += 2;
    decayFrames = cmdNum(proc->pc);
    proc->pc += 2;

    MCamShake(mode, amplitude, phaseStep, holdFrames, decayFrames);
    return 0;
}

int cmdCamCenterLength(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);

    proc->pc += 2;
    if (camMode < 0) {
        printf(cmdCamCenterLengthErrText);
        return 0;
    }
    thinkRegSet(regNo & 0xFFFF, (int)(MCamGetLengthActor(camMode) * 100.0f) & 0xFFFF);
    return 0;
}

int cmdCamUnitHeight(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    short unitNo;
    BattleUnit *unit;
    Vector4 pos;

    proc->pc += 2;
    unitNo = unitNoChk(cmdNum(proc->pc));
    proc->pc += 2;
    unit = unitPtrGet(unitNo);
    sefGetNullPosition(&pos, unit->actor, 13, 0);
    thinkRegSet(regNo & 0xFFFF, (int)(pos.y * 100.0f) & 0xFFFF);
    return 0;
}

int cmdCamCoordGet(ThinkProcess *proc)
{
    McamParams *cam = mcamPtrGet();
    int regX = thinkRegNo(proc->pc);
    int regY;
    int regZ;
    Vector4 coord;

    proc->pc += 2;
    regY = thinkRegNo(proc->pc);
    proc->pc += 2;
    regZ = thinkRegNo(proc->pc);
    proc->pc += 2;
    if (camMode >= 0) {
        MCamGetCurrentCoord(&coord, camMode);
    } else {
        coord.x = cam->lockPosition.x;
        coord.y = cam->lockPosition.y;
        coord.z = cam->lockPosition.z;
    }
    thinkRegSet(regX & 0xFFFF, (int)(coord.x * 100.0f) & 0xFFFF);
    thinkRegSet(regY & 0xFFFF, (int)(coord.y * 100.0f) & 0xFFFF);
    thinkRegSet(regZ & 0xFFFF, (int)(coord.z * 100.0f) & 0xFFFF);
    return 0;
}

/*
 * mbankPtrGet (defined above in this TU) returns the bank parameter block
 * cmdCamBank fills in; only the member that function touches is modeled.
 */
typedef struct MbankParams {
    unsigned char unmodeled_0[0xA0];
    float angle;                      /* +0xA0: bank angle in radians */
    unsigned char unmodeled_a4[0x0C];
} MbankParams;

static MbankParams mcamBank;
static MbankParams mcamBankMove;

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
 * mpersPtrGet (defined above in this TU) returns the perspective parameter block
 * cmdCamPers fills in; only the member that function touches is modeled.
 */
typedef struct MpersParams {
    unsigned char unmodeled_0[0xA4];
    float value;                      /* +0xA4: tenths-scaled perspective value */
    unsigned char unmodeled_a8[0x08];
} MpersParams;

static MpersParams mcamPers;
static MpersParams mcamPersMove;

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

int cmdEvent(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    ThinkProcess newProc;
    int scriptOffset;
    int handle;

    proc->pc += 2;
    scriptOffset = cmdNum(proc->pc);
    proc->pc += 2;
    newProc.dataTop = pDataTop;
    newProc.pc = scriptOffset;
    newProc.context = pContext;
    handle = thinkProcessAdd(&newProc);
    thinkRegSet(regNo & 0xFFFF, handle & 0xFFFF);
    return 0;
}

int cmdWaitEvent(ThinkProcess *proc)
{
    int result;

    if (proc->waitActive == 0) {
        proc->sortMode = cmdNum(proc->pc);
        result = 2;
    } else if (thinkProcessChk(proc->sortMode) != 0) {
        result = 2;
    } else {
        proc->pc += 2;
        result = 0;
    }
    return result;
}

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
    BattleUnit *unit;
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

int cmdThinkCamEvent(ThinkProcess *proc)
{
    ThinkCamEvent event;
    ThinkProcess saved;
    short param;

    param = cmdNum(proc->pc);
    proc->pc += 2;
    /* A block copy, not struct assignment: an assignment lets `sw $5,8($29)` float above it. */
    __builtin_memcpy(&saved, proc, sizeof(ThinkProcess));
    event.type = CAM_EVENT_THINK;
    event.unit = pThinkUnit;
    event.operand.param = param;
    event.target = 0;
    camEventExec(&event);
    __builtin_memcpy(proc, &saved, sizeof(ThinkProcess));
    thinkTopSet(pThinkTop);
    thinkContextSet(context);
    return 0;
}

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

int cmdStatGet(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    int category;
    int flag;
    int unitNo;
    BattleUnit *unit;
    int value;

    proc->pc += 2;
    category = cmdNum(proc->pc);
    proc->pc += 2;
    flag = cmdNum(proc->pc);
    proc->pc += 2;
    unitNo = cmdNum(proc->pc);
    proc->pc += 2;
    unit = unitPtrGet(unitNo);
    value = calcStatGet(unit, category, flag);
    thinkRegSet(regNo & 0xFFFF, value & 0xFFFF);
    return 0;
}

int cmdStatSet(ThinkProcess *proc)
{
    int regNo = thinkRegNo(proc->pc);
    int category;
    int flag;
    int unitNo;
    BattleUnit *unit;
    int value;

    proc->pc += 2;
    category = cmdNum(proc->pc);
    proc->pc += 2;
    flag = cmdNum(proc->pc);
    proc->pc += 2;
    unitNo = cmdNum(proc->pc);
    proc->pc += 2;
    unit = unitPtrGet(unitNo);
    value = calcStatSet(unit, category, flag, 0);
    thinkRegSet(regNo & 0xFFFF, value & 0xFFFF);
    return 0;
}

/*
 * calcStatReset (src/ov01/calc.c) resets one status
 * category's clamp for a unit; calcStatResetCha (src/ov01/calc.c) calls it
 * the same way, with the same always-zero fourth word, and names the
 * parameters this declaration reuses.
 */

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

int cmdDeadTime(ThinkProcess *proc)
{
    int unitNo;
    int duration;

    unitNo = cmdNum(proc->pc);
    proc->pc += 2;
    duration = cmdNum(proc->pc);
    proc->pc += 2;
    ((UnitDeadTimeBlock *)unitPtrGet(unitNo)->actor)->value = 1.0f / (float)duration;
    return 0;
}



static int (*cmdTbl[THINK_COMMAND_COUNT])(ThinkProcess *proc, int cmd) = {
    (int (*)(ThinkProcess *, int))cmdExit, (int (*)(ThinkProcess *, int))cmdGo, (int (*)(ThinkProcess *, int))cmdGosub, (int (*)(ThinkProcess *, int))cmdReturn,
    (int (*)(ThinkProcess *, int))cmdBra, (int (*)(ThinkProcess *, int))cmdOngo, (int (*)(ThinkProcess *, int))cmdOngosub, (int (*)(ThinkProcess *, int))cmdRset,
    (int (*)(ThinkProcess *, int))cmdRmath1, (int (*)(ThinkProcess *, int))cmdRmath1, (int (*)(ThinkProcess *, int))cmdRmath2, (int (*)(ThinkProcess *, int))cmdRmath2,
    (int (*)(ThinkProcess *, int))cmdRmath2, (int (*)(ThinkProcess *, int))cmdRmath2, (int (*)(ThinkProcess *, int))cmdRmath2, (int (*)(ThinkProcess *, int))cmdRmath2,
    (int (*)(ThinkProcess *, int))cmdRmath2, (int (*)(ThinkProcess *, int))cmdRmath2, (int (*)(ThinkProcess *, int))cmdRmath1, (int (*)(ThinkProcess *, int))cmdRmath1,
    (int (*)(ThinkProcess *, int))cmdRmath2, (int (*)(ThinkProcess *, int))cmdRpush, (int (*)(ThinkProcess *, int))cmdRpop, (int (*)(ThinkProcess *, int))cmdTblget,
    (int (*)(ThinkProcess *, int))cmdTblset, (int (*)(ThinkProcess *, int))cmdRput, (int (*)(ThinkProcess *, int))cmdPrint, (int (*)(ThinkProcess *, int))cmdMemdump,
    (int (*)(ThinkProcess *, int))cmdCmdputon, (int (*)(ThinkProcess *, int))cmdCmdputoff, 0, 0,
    (int (*)(ThinkProcess *, int))cmdMsgPos, (int (*)(ThinkProcess *, int))cmdMsg, (int (*)(ThinkProcess *, int))cmdThinkset, (int (*)(ThinkProcess *, int))cmdThinkset2,
    (int (*)(ThinkProcess *, int))cmdCounterBoost, (int (*)(ThinkProcess *, int))cmdStatChk, (int (*)(ThinkProcess *, int))cmdLineChk, (int (*)(ThinkProcess *, int))cmdUnitparaGet,
    (int (*)(ThinkProcess *, int))cmdUnitparaSet, (int (*)(ThinkProcess *, int))cmdUnittbl, (int (*)(ThinkProcess *, int))cmdUnitCreate, (int (*)(ThinkProcess *, int))cmdUnitChange,
    (int (*)(ThinkProcess *, int))cmdAtktbl, (int (*)(ThinkProcess *, int))cmdAtkset, (int (*)(ThinkProcess *, int))cmdAtktblSort, (int (*)(ThinkProcess *, int))cmdAtktblSortRev,
    (int (*)(ThinkProcess *, int))cmdAtktblSearch, (int (*)(ThinkProcess *, int))cmdTecparaGet, (int (*)(ThinkProcess *, int))cmdMonsSetNoGet, (int (*)(ThinkProcess *, int))cmdPartySizeGet,
    (int (*)(ThinkProcess *, int))cmdLightAmb, (int (*)(ThinkProcess *, int))cmdLightCol, (int (*)(ThinkProcess *, int))cmdLightDir, (int (*)(ThinkProcess *, int))cmdCamRefMode,
    (int (*)(ThinkProcess *, int))cmdCamCamMode, (int (*)(ThinkProcess *, int))cmdCamPersMode, (int (*)(ThinkProcess *, int))cmdCamBankMode, (int (*)(ThinkProcess *, int))cmdCamRefMoveMode,
    (int (*)(ThinkProcess *, int))cmdCamCamMoveMode, (int (*)(ThinkProcess *, int))cmdCamPersMoveMode, (int (*)(ThinkProcess *, int))cmdCamBankMoveMode, (int (*)(ThinkProcess *, int))cmdCamLock,
    (int (*)(ThinkProcess *, int))cmdCamCopy, (int (*)(ThinkProcess *, int))cmdCamSetPos, (int (*)(ThinkProcess *, int))cmdCamSetAct, (int (*)(ThinkProcess *, int))cmdCamSetAct2,
    (int (*)(ThinkProcess *, int))cmdCamSetAct3, (int (*)(ThinkProcess *, int))cmdCamOffsPos, (int (*)(ThinkProcess *, int))cmdCamOffsAng, (int (*)(ThinkProcess *, int))cmdCamMove,
    (int (*)(ThinkProcess *, int))cmdCamShake, (int (*)(ThinkProcess *, int))cmdCamCenterLength, (int (*)(ThinkProcess *, int))cmdCamUnitHeight, (int (*)(ThinkProcess *, int))cmdCamCoordGet,
    (int (*)(ThinkProcess *, int))cmdCamBank, (int (*)(ThinkProcess *, int))cmdCamPers, (int (*)(ThinkProcess *, int))cmdCamTblSet, (int (*)(ThinkProcess *, int))cmdCamMoveStop,
    (int (*)(ThinkProcess *, int))cmdEvent, (int (*)(ThinkProcess *, int))cmdWaitEvent, (int (*)(ThinkProcess *, int))cmdWaitCnt, (int (*)(ThinkProcess *, int))cmdWaitCamMove,
    (int (*)(ThinkProcess *, int))cmdWaitMsg, (int (*)(ThinkProcess *, int))cmdVPadEneble, (int (*)(ThinkProcess *, int))cmdVPadDisable, (int (*)(ThinkProcess *, int))cmdVPadSet,
    (int (*)(ThinkProcess *, int))cmdMapMulSet, (int (*)(ThinkProcess *, int))cmdHpPerGet, (int (*)(ThinkProcess *, int))cmdEventSlotGet, (int (*)(ThinkProcess *, int))cmdThinkCamEvent,
    (int (*)(ThinkProcess *, int))cmdCfEncountGet, (int (*)(ThinkProcess *, int))cmdCfEventGet, (int (*)(ThinkProcess *, int))cmdThinkNoGet, (int (*)(ThinkProcess *, int))cmdStatGet,
    (int (*)(ThinkProcess *, int))cmdStatSet, (int (*)(ThinkProcess *, int))cmdStatReset, (int (*)(ThinkProcess *, int))cmdDeadTime
};

const char cmdThinkProcessAddOverText[24] = "thinkProcessAdd: over\n";

const char cmdMonsSetOverText[24] = "** monsSet: over -> %d\n";

const char cmdCamEventExecText[40] = "** camEventExec %d ** %X\n";

const char cmdErrText[16] = "** cmd err %d\n";

const char cmdGOSUBStackErrText[24] = "** GOSUB stack err\n";

const char cmdRETURNStackErrText[32] = "** RETURN stack err\n";

const char cmdONGOErrText[16] = "** ONGO err\n";

const char cmdONGOSUBErrText[16] = "** ONGOSUB err\n";

const char cmdONGOSUBStackErrText[24] = "** ONGOSUB stack err\n";

const char cmdRmath1ErrText[16] = "** Rmath1 err\n";

const char cmdRmath2ErrText[16] = "** Rmath2 err\n";

const char cmdRPUSHStackErrText[24] = "** RPUSH stack err\n";

const char cmdRPOPStackErrText[24] = "** RPOP stack err\n";

const char cmdJpText2[8] = "%s\n";

const char cmdMemDumpText[48] = "------------------ Mem Dump -----------------\n";

const char cmdJpText3[8] = "%04X: ";

const char cmdJpText4[8] = "%04X ";

const char cmdJpText5[8] = "\n%04X: ";

const char cmdJpText6[8] = "\n";

const char cmdJpText7[48] = "---------------------------------------------\n";

const char cmdTHINKSETErrTgtText[32] = "** THINKSET: err -> tgt %d\n";

const char cmdThinkSetErrNormText[32] = "** ThinkSet: err -> norm %d\n";

const char cmdThinkSetEtherLOSTText[32] = "** ThinkSet: ether LOST %d\n";

const char cmdThinkSetErrEtherText[32] = "** ThinkSet: err -> ether %d\n";

const char cmdTHINKSETErrCmdText[32] = "** THINKSET: err -> cmd %d\n";

const char cmdUNITPARAErrText[24] = "** UNIT PARA err -> %d\n";

const char cmdATKSETOVERText[32] = "** ATKSET OVER => %d/%d\n";

const char cmdTECPARAErrText[32] = "** TEC PARA err -> %d\n";

const char cmdCamLockErrText[24] = "** CamLock err **\n";

const char cmdCamCopyErrText[24] = "** CamCopy err **\n";

const char cmdCamCenterLengthErrText[32] = "** camCenterLength err **\n";

static int msgPosX;

static int msgPosY;

static McamParams mcamView;

static McamParams mcamLookAt;

static McamParams mcamViewMove;

static McamParams mcamLookAtMove;

static short posTbl[36];
