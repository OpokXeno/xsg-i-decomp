#include "common.h"
#include "shared.h"
#include "sc_get.h"

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scInitScript);

/*
 * scFindScriptData is a genuine tail call (`j sefSearchMapperIndex`, not
 * `jal`) into the mapper-index search main/tu211 (src/main/sef.c) defines;
 * that function is still INCLUDE_ASM there.
 */
extern int sefSearchMapperIndex(int eftNo);

static int scFindScriptData(int eftNo)
{
    return sefSearchMapperIndex(eftNo);
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scCreateScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scDestroyScript);

extern unsigned char D_004CC7B8[];
extern int _nowScript;

/*
 * sefDestroyScriptScheduler2 (main:0x002e5320, defined in main/tu211/sef.c)
 * takes the same script_index/task_index pair scDeleteTask below does; its
 * own accepted definition (src/main/sef.c) documents the match.
 */
extern void sefDestroyScriptScheduler2(int script_index, int task_index);
static void scDeleteTask(int script_index, int task_index);

/*
 * scDestroyScript2 forwards both parameters unchanged to
 * sefDestroyScriptScheduler2, then, when the target script's data table (the
 * same +0x400 pointer scGetTableAdrIdx and its siblings below read) is
 * loaded, also deletes the one task scDeleteTask selects.
 */
void scDestroyScript2(int script_index, int task_index)
{
    /* tracePrint's other callers in this TU (below) declare it with fewer
     * trailing arguments; this call needs both script_index and task_index. */
    extern int tracePrint(unsigned char *fmt, int script_index, int task_index);

    if ((unsigned int)script_index >= 16u) {
        tracePrint(D_004CC7B8, script_index, task_index);
        return;
    }
    sefDestroyScriptScheduler2(script_index, task_index);
    if (((ScriptRecord *)(_scriptWork + script_index * 1104))->dataTable != (int *)0) {
        scDeleteTask(script_index, task_index);
        return;
    }
}

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

extern int tracePrint(unsigned char *fmt, unsigned int value);
extern unsigned char D_004CC818[];

/*
 * scGetReg resolves a script-register selector the same way scGetRegAdr
 * below does (bit 0x8000 set means "read the register that selector names,
 * then use its value as the real selector"; an out-of-range selector reports
 * the error through tracePrint and reads as 0), but keeps its own copy of the
 * address computation and bound check instead of calling scGetRegAdr.
 */
int scGetReg(int reg)
{
    int *address;
    int value;

    if (reg & 0x8000) {
        reg = scGetReg(reg & ~0x8000);
    }
    if ((unsigned int)reg >= 16u) {
        tracePrint(D_004CC818, reg);
        address = (int *)0;
    } else {
        address = (int *)(_scriptWork + _nowScript * 1104 + 0x404 + reg * 4);
    }
    value = 0;
    if (address != (int *)0) {
        value = *address;
    }
    return value;
}

/* Same indirect-selector and bound-check logic as scGetReg above, writing
 * `value` into the resolved register instead of reading it. */
void scSetReg(int reg, int value)
{
    int *address;

    if (reg & 0x8000) {
        reg = scGetReg(reg & ~0x8000);
    }
    if ((unsigned int)reg >= 16u) {
        tracePrint(D_004CC818, reg);
        address = (int *)0;
    } else {
        address = (int *)(_scriptWork + _nowScript * 1104 + 0x404 + reg * 4);
    }
    if (address != (int *)0) {
        *address = value;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetCmdScript);

extern int scGetCmdScript(ScriptTask *task);
extern int scGetReg(int reg);

int scGetNumScript(ScriptTask *task)
{
    int value;
    short reg;

    value = scGetCmdScript(task);
    if (value & 0x8000) {
        if (!(value & 0x4000)) {
            value &= 0x3FFF;
        }
    } else {
        reg = scGetReg(value);
        value = reg;
    }
    return value;
}

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

/*
 * scGetAdrIdx reads the same task->script_pc[task->branch_depth] operand
 * scGetCmdAdrScript below does and resolves it to a data-table base exactly
 * the way scGetTableAdrIdx does, then indexes that base by `index` instead of
 * returning it directly.
 */
unsigned short scGetAdrIdx(ScriptTask *task, int index)
{
    int operand;
    unsigned short *table;
    int value;

    table = (unsigned short *)0;
    operand = task->script_pc[task->branch_depth];
    if (operand != 0) {
        table = (unsigned short *)((int)((ScriptRecord *)(_scriptWork + _nowScript * 1104))->dataTable + operand * 2);
    }
    value = table[index];
    if (value == 0xFFFF || value == -1) {
        value = 0;
    }
    return value;
}

extern int _nowScript;

/*
 * Reads a table-relative entry from the current script's data table (the
 * pointer stored at the script record's +0x400, the same field
 * scGetTableAdrImmIdx, scGetImmAdrImmIdx, scGetTableNumIdx, scAdrToImm and
 * scGetCmdAdrScript below read): `base` selects an optional sub-table before
 * `index` is applied (`base == 0` reads directly at `index * 2`). Same
 * 0xFFFF/-1 "no address" sentinels as scGetAdrScript, scGetImmAdrImmIdx2 and
 * scOfsToAdr collapse to 0.
 */
unsigned short scGetTableAdrIdx(int base, int index)
{
    int table;
    int value;

    table = 0;
    if (base != 0) {
        table = *(int *)(_scriptWork + _nowScript * 1104 + 0x400) + base * 2;
    }
    value = *(unsigned short *)(table + index * 2);
    if (value == 0xFFFF || value == -1) {
        value = 0;
    }
    return value;
}

/*
 * Forwards `base`/`index` to scGetTableAdrIdx and turns the entry it returns
 * into an absolute address in the current script's data table: entry 0
 * stays 0, any other entry is doubled and added to the table pointer.
 */
int scGetTableAdrImmIdx(int base, int index)
{
    unsigned short entry;
    int address;

    entry = scGetTableAdrIdx(base, index);
    address = 0;
    if (entry != 0) {
        address = *(int *)(_scriptWork + _nowScript * 1104 + 0x400) + entry * 2;
    }
    return address;
}

/*
 * Reads a 16-bit entry directly out of `table[index]` (an immediate table
 * the caller passed, not the current script's), normalizes the same
 * 0xFFFF/-1 sentinels scGetTableAdrIdx above collapses, and turns a nonzero
 * entry into an absolute address in the current script's data table.
 */
int scGetImmAdrImmIdx(const unsigned short *table, int index)
{
    int entry;
    int address;

    entry = table[index];
    if (entry == 0xFFFF || entry == -1) {
        entry = 0;
    }
    address = 0;
    if (entry != 0) {
        address = *(int *)(_scriptWork + _nowScript * 1104 + 0x400) + entry * 2;
    }
    return address;
}

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

short scGetTableNumIdx(int base, int index)
{
    int table;

    table = 0;
    if (base != 0) {
        table = *(int *)(_scriptWork + _nowScript * 1104 + 0x400) + base * 2;
    }
    return *(short *)(table + index * 2);
}

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

extern int tracePrint(unsigned char *fmt, unsigned int value);
extern unsigned char D_004CC850[];

/*
 * scRETURNScript is the script VM's RETURN opcode handler: it pops the
 * branch-stack entry the task's current depth points to (clearing it) and
 * decrements the depth; with nothing left to return to (branch_depth <= 0)
 * it reports the underflow through tracePrint instead.
 */
static int scRETURNScript(ScriptTask *task)
{
    short depth = task->branch_depth;
    int depthU = (unsigned short)task->branch_depth;

    if (depth > 0) {
        task->branch_depth = depthU - 1;
        task->script_pc[depth] = 0;
    } else {
        tracePrint(D_004CC850, depthU);
    }
    return 1;
}

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

/*
 * scCreateTask (main:0x002ea028, still assembly in this TU) takes the
 * script to create the task under, a task-index hint (-1 auto-assigns a free
 * slot, the same sentinel scOBJEVEScript passes below), the branch address to
 * seed script_pc[0] with (sc_get.h's ScriptTask comment) and the 16-byte
 * caller span it copies into the new task's +0x30 (the same comment).
 */
extern int scCreateTask(int script_index, int task_index, int address, unsigned char *data);

/*
 * scOBJEVEScript is the script VM's OBJEVE opcode handler: it decodes a
 * numeric operand and an address operand, creates a task at that address
 * (auto-assigning its slot), and stores the decoded number into the new
 * task's effect_scheduler halfword.
 */
static int scOBJEVEScript(ScriptTask *task)
{
    int number;
    int newTaskIndex;
    ScriptTask *newTask;

    number = scGetNumScript(task);
    newTaskIndex = scCreateTask(_nowScript, -1, scGetAdrScript(task), &task->unmodeled_20[0x10]);
    if (newTaskIndex >= 0) {
        newTask = (ScriptTask *)(_scriptWork + _nowScript * 1104 + newTaskIndex * 128);
        newTask->effect_scheduler = number;
    }
    return 1;
}

extern int _nowEvent;
extern int _nowScript;
static void scDeleteTaskAll(int event, int task_index);
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

extern unsigned int strlen(const char *s);

/*
 * scPRINTScript is the script VM's PRINT opcode handler: the current command
 * operand (task->script_pc[task->branch_depth], the same value
 * scGetCmdAdrScript below decodes) is the address, in the script's data
 * table, of an inline null-terminated string; this handler does not print it
 * (that trace call is compiled out of this build), but still has to skip the
 * script reader past the string's bytes, so it advances the same script_pc
 * entry by the string's length, padded to an even count of bytes plus its
 * terminator, in halfword units.
 */
static int scPRINTScript(ScriptTask *task)
{
    char *string;
    int length;

    string = (char *)0;
    if (task->script_pc[task->branch_depth] != 0) {
        string = (char *)((int)((ScriptRecord *)(_scriptWork + _nowScript * 1104))->dataTable
            + task->script_pc[task->branch_depth] * 2);
    }
    length = strlen(string);
    length = (length & 1) ? length + 1 : length + 2;
    task->script_pc[task->branch_depth] += length / 2;
    return 1;
}

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

extern const char D_004DBB38[];

/*
 * scRDUMPScript dumps the 16 script registers through the trace logger, 8 per
 * line, formatting each as " %04x" and re-terminating the line buffer between
 * lines.
 */
static int scRDUMPScript(void)
{
    char line[128];
    int length;
    int i;
    int j;

    length = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 8; j++) {
            length += sprintf(line + length, D_004DBB38, scGetReg(i * 8 + j));
        }
        line[0] = 0;
    }
    return 1;
}

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

/*
 * scREVEScript is the script VM's REVE opcode handler: it stores the
 * current script's header halfword at +0x446 (the header's other evidenced
 * entry is scDeleteTask's active-task counter at +0x448) into the operand
 * register scGetCmdScript decodes.
 */
static int scREVEScript(ScriptTask *task)
{
    int reg;
    short value;

    reg = scGetCmdScript(task);
    value = *(short *)(_scriptWork + _nowScript * 1104 + 0x446);
    scSetReg(reg, value);
    return 1;
}

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

extern void sdvSetAmbState(int state, int effect_no);
extern void sdvSetAmbState2(int state, int effect_no);

/*
 * scSetAmbient is the script VM's ambient-state opcode handler: outside an
 * event it starts (or overrides, when `flags & 0x100`) the ambient state for
 * `effect_no`.
 */
static void scSetAmbient(ScriptTask *task)
{
    short effectNo;

    if (_nowEvent != 0) {
        return;
    }
    effectNo = task->effect_no;
    if (task->flags & 0x100) {
        sdvSetAmbState2(1, effectNo);
        return;
    }
    sdvSetAmbState(1, effectNo);
}

extern int scGetAdrImmScript(ScriptTask *task);

/*
 * sefCreateScheduler (main:0x002e5900) and sefCreateBattleActorTbl
 * (main:0x002e63f0) are still assembly in main/tu211/sef.c. sefCreateScheduler
 * is called here with the task's own two 16-byte spans at +0x20/+0x30 (the
 * same +0x30 span scCreateTask's own comment above documents) and the current
 * script's data table pointer; -1 is the same "auto-assign" sentinel
 * scOBJEVEScript's scCreateTask call above uses for its task-index argument.
 */
extern int sefCreateScheduler(int address, void *taskField0x30, void *taskField0x20, int table, int taskIndex);
extern void sefCreateBattleActorTbl(short effect_no);

/* scFreezeCamera (still assembly in this TU) takes the task's effect_no. */
extern void scFreezeCamera(short effect_no);

/*
 * scEFFECTScript is the script VM's EFFECT opcode handler: a resolved address
 * creates an effect scheduler seeded from the task's own state, records the
 * scheduler handle in the task's effect_scheduler field (the same field the
 * WAIT-for-effect handler above reads back), and, once the scheduler starts
 * successfully, applies the task's ambient state and freezes the camera for
 * its effect.
 */
static int scEFFECTScript(ScriptTask *task)
{
    int address;
    int schedulerId;

    address = scGetAdrImmScript(task);
    if (address != 0) {
        schedulerId = sefCreateScheduler(address, &task->unmodeled_20[0x10], &task->unmodeled_20[0],
            (int)((ScriptRecord *)(_scriptWork + _nowScript * 1104))->dataTable, -1);
        task->effect_scheduler = schedulerId;
        sefCreateBattleActorTbl(task->effect_no);
        if (schedulerId >= 0) {
            scSetAmbient(task);
            scFreezeCamera(task->effect_no);
        }
    }
    return 1;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scEFFECT2Script);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scEFFECT3Script);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scMOVIEScript);

extern int scGetAdrImmScript(ScriptTask *task);
extern int srsAnalyzeEftNo(short effectId, unsigned char *charId, int *effectCategory);
extern int _scMslCate;

/*
 * scMISSILEScript is the script VM's MISSILE opcode handler: outside an
 * event, a nonzero resolved address stores itself as `missile_adr`, marks
 * the task missile-active and sets flags bit 0x20, clears the two halfwords
 * at +0x66/+0x68 (no read of either is evidenced in this allocation), and
 * hands the task's `effect_no` to srsAnalyzeEftNo for category analysis.
 */
static int scMISSILEScript(ScriptTask *task)
{
    unsigned char charId;
    int address;

    address = scGetAdrImmScript(task);
    if (address != 0 && _nowEvent == 0) {
        task->missile_adr = address;
        task->missile_active = 1;
        task->flags |= 0x20;
        task->missile_reset0 = 0;
        task->missile_reset1 = 0;
        srsAnalyzeEftNo(task->effect_no, &charId, &_scMslCate);
    }
    return 1;
}

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

extern unsigned char D_004CC818[];

/*
 * Resolves a script-register selector (0-15) to the address of its backing
 * storage in the current script's record (+0x404, 16 4-byte slots); an
 * out-of-range selector reports the error through tracePrint and returns
 * NULL instead.
 */
void *scGetRegAdr(unsigned int reg)
{
    if (reg >= 16u) {
        tracePrint(D_004CC818, reg);
        return (void *)0;
    }
    return _scriptWork + _nowScript * 1104 + 0x404 + reg * 4;
}

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

/*
 * Converts a halfword-unit immediate index into an absolute address in the
 * current script's data table: 0 stays 0, any other index is doubled and
 * added to the table pointer, the same as scGetTableAdrImmIdx above.
 */
int scAdrToImm(int index)
{
    int address;

    address = 0;
    if (index != 0) {
        address = *(int *)(_scriptWork + _nowScript * 1104 + 0x400) + index * 2;
    }
    return address;
}

/*
 * Reads the running task's next command operand (script_pc[branch_depth],
 * still decoded the same way scGetCmdScript is) and, when nonzero, turns it
 * into an absolute address in the current script's data table the same way
 * scAdrToImm above does.
 */
int scGetCmdAdrScript(ScriptTask *task)
{
    int operand;
    int address;

    address = 0;
    operand = task->script_pc[task->branch_depth];
    if (operand != 0) {
        address = *(int *)(_scriptWork + _nowScript * 1104 + 0x400) + operand * 2;
    }
    return address;
}
