#include "common.h"
#include "shared.h"
#include "sc_get.h"

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scInitScript);

/*
 * scFindScriptData is a genuine tail call (`j sefSearchMapperIndex`, not
 * `jal`) into the mapper-index search main/tu211 (src/main/sef.c) defines;
 * that function is still INCLUDE_ASM there.
 */
extern void sefSearchMapperIndex(void);

static void scFindScriptData(void)
{
    sefSearchMapperIndex();
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scCreateScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scDestroyScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scDestroyScript2);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scDestroyScriptAll);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scExecScript);

/* scExecScript (above) is still INCLUDE_ASM; declare it so its caller does
 * not see an implicit declaration. */
static void scExecScript(void);

/*
 * sefExecScheduler runs the effect scheduler's tick; it is defined in
 * main/tu211 (src/main/sef.c), still INCLUDE_ASM there. scExecEffect calls
 * scExecScript, then genuinely tail-calls (`j`, not `jal`) into it.
 */
extern void sefExecScheduler(void);

void scExecEffect(void)
{
    scExecScript();
    sefExecScheduler();
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scCreateTask);

/*
 * Invalidate one task slot and decrement its owning script's active count.
 * The original storage declarations are unavailable; the slot's recovered
 * members are in sc_get.h, and the script header at +0x440 of each script
 * record keeps its measured offsets, because only its active-task counter has
 * an evidenced role here.
 */
static void scDeleteTask(int script_index, int task_index)
{
    ScriptTask *task;
    unsigned char *task_scripts;
    unsigned char *count_scripts;
    ScSchedulerWord *active_task_count;
    int task_offset;
    int remaining_tasks;

    if ((unsigned int)task_index < 9u) {
        if (task_index < 0) {
            task = (ScriptTask *)0;
        } else {
            task_offset =
                script_index * 1104
                + task_index * 128;
            task_scripts = _scriptWork;
            task = (ScriptTask *)(task_scripts + task_offset);
        }

        if (task->flags != 0) {
            count_scripts = _scriptWork;
            active_task_count = (ScSchedulerWord *)(count_scripts
                + script_index * 1104 + 1088);
            remaining_tasks = active_task_count[4];
            remaining_tasks -= 1;
            active_task_count[4] = (ScSchedulerWord)remaining_tasks;
        }

        task->flags = 0;
        task->branch_depth = 0;
        task->script_pc[0] = -1;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scDeleteTaskAll);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetReg);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scSetReg);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetCmdScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetNumScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetRegScript);

/*
 * scGetAdrScript forwards the running task it was given straight through to
 * scGetCmdScript (register evidence: it sets up no argument of its own before
 * the `jal scGetCmdScript`), and collapses the same 0xFFFF/-1 "no address"
 * sentinels scGetImmAdrImmIdx2 above documents.
 */
extern int scGetCmdScript(ScriptTask *task);

int scGetAdrScript(ScriptTask *task)
{
    int address;

    address = scGetCmdScript(task) & 0xFFFF;
    if (address == 0xFFFF || address == -1) {
        address = 0;
    }
    return address;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetAdrIdx);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetTableAdrIdx);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetTableAdrImmIdx);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetImmAdrImmIdx);

/*
 * Resolves a table-indexed operand to an address relative to `base`: the
 * halfword at `table[index]` is a count of halfword units (multiplied by 2
 * below), with the same 0xFFFF/-1 "no address" sentinels as scGetAdrScript
 * collapsing to 0.
 */
int scGetImmAdrImmIdx2(int base, unsigned short *table, int index)
{
    int offset;
    int address;

    offset = table[index];
    if (offset == 0xFFFF || offset == -1) {
        offset = 0;
    }
    address = 0;
    if (offset != 0) {
        address = base + offset * 2;
    }
    return address;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetTableNumIdx);

/* Reads a signed 16-bit table entry: `table[index]`. */
short scGetImmNumIdx(short *table, int index)
{
    return table[index];
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetAdrImmScript);

static int scERRORScript(void)
{
    return 0;
}

extern int scGetAdrScript(ScriptTask *task);

/*
 * scGOScript is the script VM's GO opcode wrapper: it stores the resolved
 * address at the branch-stack entry `branch_depth` selects, the same slot
 * scONGOScript above writes.
 */
static int scGOScript(ScriptTask *task)
{
    task->script_pc[task->branch_depth] = scGetAdrScript(task);
    return 1;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGOSUBScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRETURNScript);

static int scEXITScript(void)
{
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scBRAScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scONGOSUB);

/*
 * scONGOScript is the script VM's ONGO opcode wrapper.  scONGOSUB performs the
 * operand decoding and branch selection; this wrapper stores the result into
 * the branch-stack entry the task's current depth selects.
 */
static int scONGOScript(ScriptTask *task)
{
    int destination = scONGOSUB(task);
    short branch_depth = task->branch_depth;

    task->script_pc[branch_depth] = destination;
    return 1;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scONGOSUBScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scOBJEVEScript);

extern int _nowEvent;
extern int _nowScript;
extern void scDeleteTaskAll(int event, int task_index);
/*
 * scGetNumScript decodes the current script command's next numeric operand.
 * Every opcode handler is called with the running ScriptTask in $a0 (as
 * scONGOScript and scPAUSEScript use it), and the handlers below hand that
 * task on to scGetNumScript: the R*Script handlers save $a0 across
 * scGetCmdScript and reload it before the call (daddu a0,s0 at 0x002eae90),
 * and scABORTScript, whose first call it is, leaves $a0 untouched
 * (0x002eabc4..0x002eabc8). scGetNumScript's own body does not read it.
 */
extern int scGetNumScript(ScriptTask *task);

/*
 * Deletes the task the decoded index selects.  A negative index deletes every
 * task of the given event instead (gp-relative _nowEvent/_nowScript at
 * 0x002eabdc/0x002eabf0).
 *
 * `event` first receives the scGetNumScript(task) result (discarded once
 * `taskIndex` is initialised from the same expression) and only later the
 * _nowEvent load: forms 01-03 (build/form-0{1,2,3}/functions/scABORTScript.json)
 * show that giving `taskIndex` and `event` independent initialisations, in
 * either declaration order, costs the compiler the shared epilogue between
 * the two branches (+8 bytes over the original); the chained assignment
 * below (form04) is what keeps it.
 */
static int scABORTScript(ScriptTask *task)
{
    int taskIndex;
    int event;

    taskIndex = (event = scGetNumScript(task));
    if (taskIndex < 0) {
        event = _nowEvent;
        scDeleteTaskAll(event, taskIndex);
    } else {
        scDeleteTask(_nowScript, taskIndex);
    }
    return 1;
}

static int scPAUSEScript(ScriptTask *task)
{
    task->flags |= 4; /* pause bit; scDispatchScript's `flags & 4` reads it */
    return 1;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scPRINTScript);

/*
 * scGetCmdScript (still asm) decodes the running task's next command
 * operand: it reads the task the handler was given in $a0
 * (`lh $3,0x54($4)` = task->branch_depth at 0x002ea418,
 * `addiu $5,$4,0x8` = task->script_pc at 0x002ea41c).
 */
extern int scGetCmdScript(ScriptTask *task);
extern int scGetReg(int reg);

static int scRPUTScript(ScriptTask *task)
{
    scGetReg(scGetCmdScript(task));
    return 1;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRDUMPScript);

extern short _cmdPut;

static int scCMDPUTONScript(void)
{
    _cmdPut = 1;
    return 1;
}

static int scCMDPUTOFFScript(void)
{
    _cmdPut = 0;
    return 1;
}

extern void scSetReg(int reg, int value);

static int scRSETScript(ScriptTask *task)
{
    int reg = scGetCmdScript(task);

    scSetReg(reg, scGetNumScript(task));
    return 1;
}

static int scRINCScript(ScriptTask *task)
{
    int reg = scGetCmdScript(task);

    scSetReg(reg, scGetReg(reg) + 1);
    return 1;
}

static int scRDECScript(ScriptTask *task)
{
    int reg = scGetCmdScript(task);

    scSetReg(reg, scGetReg(reg) - 1);
    return 1;
}

static int scRADDScript(ScriptTask *task)
{
    int reg = scGetCmdScript(task);
    int operand = scGetNumScript(task);

    scSetReg(reg, scGetReg(reg) + operand);
    return 1;
}

static int scRSUBScript(ScriptTask *task)
{
    int reg = scGetCmdScript(task);
    int operand = scGetNumScript(task);

    scSetReg(reg, scGetReg(reg) - operand);
    return 1;
}

static int scRMULScript(ScriptTask *task)
{
    int reg = scGetCmdScript(task);
    int operand = scGetNumScript(task);

    scSetReg(reg, scGetReg(reg) * operand);
    return 1;
}

/*
 * `divisor` duplicates `operand` into its own local: form01
 * (build/form-01/functions/scRDIVScript.json) shows the original keeps a
 * third callee-saved register (s2) holding this value only for the `beql
 * s2,zero` zero-divide guard, separate from the register the `div`
 * instruction itself reads, and drops to two saved registers without it.
 */
static int scRDIVScript(ScriptTask *task)
{
    int reg = scGetCmdScript(task);
    int operand = scGetNumScript(task);
    int divisor = operand;

    if (divisor != 0) {
        scSetReg(reg, scGetReg(reg) / divisor);
    }
    return 1;
}

/* Same compiler-forced extra register as scRDIVScript; see its comment. */
static int scRMODScript(ScriptTask *task)
{
    int reg = scGetCmdScript(task);
    int operand = scGetNumScript(task);
    int divisor = operand;

    if (divisor != 0) {
        scSetReg(reg, scGetReg(reg) % divisor);
    }
    return 1;
}

static int scR_ANDScript(ScriptTask *task)
{
    int reg = scGetCmdScript(task);
    int operand = scGetNumScript(task);

    scSetReg(reg, scGetReg(reg) & operand);
    return 1;
}

static int scR_ORScript(ScriptTask *task)
{
    int reg = scGetCmdScript(task);
    int operand = scGetNumScript(task);

    scSetReg(reg, scGetReg(reg) | operand);
    return 1;
}

static int scR_XORScript(ScriptTask *task)
{
    int reg = scGetCmdScript(task);
    int operand = scGetNumScript(task);

    scSetReg(reg, scGetReg(reg) ^ operand);
    return 1;
}

static int scR_NOTScript(ScriptTask *task)
{
    int reg = scGetCmdScript(task);

    scSetReg(reg, ~scGetReg(reg));
    return 1;
}

static int scR_NEGScript(ScriptTask *task)
{
    int reg = scGetCmdScript(task);

    scSetReg(reg, -scGetReg(reg));
    return 1;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRRNDScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scREVEScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scWAITCNTScript);

/*
 * scWAITEVEScript is the WAIT-for-event opcode handler: it reads the event
 * number operand, records it as the task's wait_value and sets wait_mode to
 * 2, then returns the parser-yield status 2.
 */
static int scWAITEVEScript(ScriptTask *task)
{
    task->wait_value = scGetNumScript(task);
    task->wait_mode = 2;
    return 2;
}

/*
 * scWAITEFTScript is the WAIT-for-effect opcode handler. It consumes an
 * operand from the script stream (the scGetNumScript(task) call), but the
 * value it waits on is the effect_scheduler handle scEFFECTScript (still
 * asm) already stored in the task: it copies that into wait_value and sets
 * wait_mode to 3.
 */
static int scWAITEFTScript(ScriptTask *task)
{
    scGetNumScript(task);
    task->wait_mode = 3;
    task->wait_value = task->effect_scheduler;
    return 2;
}

/* scWAITMOVIEScript is the WAIT-for-movie opcode handler: no operand of its
 * own, it just sets wait_mode to 4 and yields. */
static int scWAITMOVIEScript(ScriptTask *task)
{
    task->wait_mode = 4;
    return 2;
}

/* scWAITMISSILEScript is the WAIT-for-missile opcode handler: no operand of
 * its own, it just sets wait_mode to 5 and yields. */
static int scWAITMISSILEScript(ScriptTask *task)
{
    task->wait_mode = 5;
    return 2;
}

/* scFADEONScript sets the fade-on bit of the task's flags word and returns
 * the running status 1. */
static int scFADEONScript(ScriptTask *task)
{
    task->flags |= 0x100;
    return 1;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scFreezeCamera);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scSetAmbient);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scEFFECTScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scEFFECT2Script);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scEFFECT3Script);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scMOVIEScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scMISSILEScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scMISSILE3Script);

static int scDispatchScript(ScriptTask *task)
{
    unsigned short flags = task->flags;

    if (flags == 0)
        return 0;
    if (flags & 4)
        return 4;

    if (flags & 0x10) {
        scWaitParseScript(task);
        flags = task->flags;
        if (flags & 0x10)
            goto check_move;
    }

    scParseScript(task);
    flags = task->flags;

check_move:
    if (flags & 0x20)
        scMoveParseScript(task);
    return 1;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scParseScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scAnalyzeScriptCf);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetTaskAdr);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetRegAdr);

/*
 * Collapses the same 0xFFFF/-1 "no address" sentinels scGetAdrScript and
 * scGetImmAdrImmIdx2 above document to 0; any other offset is returned as-is.
 */
int scOfsToAdr(int offset)
{
    if (offset == 0xFFFF || offset == -1) {
        offset = 0;
    }
    return offset;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scAdrToImm);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetCmdAdrScript);
