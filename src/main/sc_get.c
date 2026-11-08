#include "common.h"
#include "shared.h"
#include "sc_get.h"
#include "main/sc_get.h"
#include "main/sdv.h"
#include "main/sef.h"
#include "main/srs.h"

#define SCRIPT_COMMAND_COUNT 73       /* opcodes in _scFuncHandler */
#define SCRIPT_REG_INDIRECT 0x8000    /* register selector names another register */
#define SCRIPT_OPERAND_IMMEDIATE 0x8000 /* numeric operand is a literal, not a register */
#define SCRIPT_OPERAND_SIGNED 0x4000    /* literal keeps its sign bit */
#define SCRIPT_OPERAND_VALUE_MASK 0x3FFF
#define SCRIPT_ADDRESS_NONE 0xFFFF    /* absent table entry or address */
#define SCRIPT_BRANCH_DEPTH_LIMIT 3   /* deepest GOSUB level */
#define SCRIPT_RUN_STEP_LIMIT 2000    /* commands one task runs per frame */
#define SCRIPT_DUMP_ROWS 2
#define SCRIPT_DUMP_COLUMNS 8
#define SCRIPT_DUMP_LINE_SIZE 128

/* Effect number ranges with their own handling. */
#define SCRIPT_CAMERA_EFFECT_FIRST 2000
#define SCRIPT_CAMERA_EFFECT_COUNT 100
#define SCRIPT_CAMERA_PARAM_WORDS 44
#define SCRIPT_SINGLE_VARIANT_EFFECT_FIRST 2900
#define SCRIPT_SINGLE_VARIANT_EFFECT_COUNT 99

/* Header kinds of an effect script table (table[8]). */
#define SCRIPT_TABLE_KIND_FIRST_ENTRY 60
#define SCRIPT_TABLE_KIND_INDEXED 61
#define SCRIPT_TABLE_KIND_SECOND_ENTRY 62

/* Comparison selectors of scBRAScript. */
enum ScriptCondition {
    SCRIPT_COND_EQ = 0,
    SCRIPT_COND_NE = 1,
    SCRIPT_COND_GT = 2,
    SCRIPT_COND_GE = 3,
    SCRIPT_COND_LT = 4,
    SCRIPT_COND_LE = 5
};

/* Sixteen zero-filled script records (SCRIPT_COUNT). */
ScriptRecord _scriptWork[];

/* EUC-JP: "clear effect outside the valid range %d %d". */
static const char outOfRangeEffectDeleteFormat[];

static const char registerOverflowFormat[];
static const char effectStackUnderflowFormat[];
extern int _nowDataIdx;
const char registerDumpFormat[];

static const char noLoadEffectDataFormat[];

static const char taskOutOfRangeFormat[];
static const char taskWorkEmptyFormat[];
static const char effectStackOverflowFormat[];
static const char badExpressionFormat[];
static const char simajiriMoviePath[];

extern void sefDestroyScriptScheduler2(int script_index, int task_index);

static void scDeleteTask(int script_index, int task_index);

extern void scDestroyScript(int script_index);

/* scExecScript is defined below in this file; declare it so its caller does
 * not see an implicit declaration. */
static void scExecScript(void);

static int scDispatchScript(ScriptTask *task);

/* Runs the effect scheduler's tick (defined in sef.c). scExecEffect calls scExecScript, then tail-calls (`j`, not `jal`) into it. */
extern void sefExecScheduler(void);

extern int tracePrint();

extern int scGetCmdScript(ScriptTask *task);

extern int scGetReg(int reg);
extern int scGetAdrScript(ScriptTask *task);
static int scCreateTask(int script_index, int task_index, int address, ScriptTaskParameters *data);
static void scDeleteTaskAll(int event, int task_index);

extern int scGetNumScript(ScriptTask *task);

extern unsigned int strlen(const char *s);

/* Same indirect-selector and bound-check logic as scGetReg above, writing
 * `value` into the resolved register instead of reading it. */
extern void scSetReg(int reg, int value);

extern int scGetAdrImmScript(ScriptTask *task);

/*
 * Creates a scheduler entry for an effect started by a script task. It receives
 * the task's parameters and initial position, the script's data table, and -1,
 * which selects an automatic task index.
 */
extern int sefCreateScheduler(int address, void *taskField0x30, void *taskField0x20, int table, int taskIndex);

extern void sefCreateBattleActorTbl(short effect_no);

static void scFreezeCamera(int effect_no);

static int scERRORScript(ScriptTask *task);
static int scGOScript(ScriptTask *task);
static int scGOSUBScript(ScriptTask *task);
static int scBRAScript(ScriptTask *task);
static int scEXITScript(ScriptTask *task);
static int scRETURNScript(ScriptTask *task);
static int scONGOScript(ScriptTask *task);
static int scONGOSUBScript(ScriptTask *task);
static int scPRINTScript(ScriptTask *task);
static int scRPUTScript(ScriptTask *task);
static int scRDUMPScript(ScriptTask *task);
static int scCMDPUTONScript(ScriptTask *task);
static int scCMDPUTOFFScript(ScriptTask *task);
static int scFADEONScript(ScriptTask *task);
static int scRSETScript(ScriptTask *task);
static int scRINCScript(ScriptTask *task);
static int scRDECScript(ScriptTask *task);
static int scRADDScript(ScriptTask *task);
static int scRSUBScript(ScriptTask *task);
static int scRMULScript(ScriptTask *task);
static int scRDIVScript(ScriptTask *task);
static int scRMODScript(ScriptTask *task);
static int scR_ANDScript(ScriptTask *task);
static int scR_ORScript(ScriptTask *task);
static int scR_XORScript(ScriptTask *task);
static int scR_NOTScript(ScriptTask *task);
static int scR_NEGScript(ScriptTask *task);
static int scRRNDScript(ScriptTask *task);
static int scREVEScript(ScriptTask *task);
static int scOBJEVEScript(ScriptTask *task);
static int scABORTScript(ScriptTask *task);
static int scPAUSEScript(ScriptTask *task);
static int scWAITCNTScript(ScriptTask *task);
static int scWAITEVEScript(ScriptTask *task);
static int scWAITEFTScript(ScriptTask *task);
static int scWAITMOVIEScript(ScriptTask *task);
static int scWAITMISSILEScript(ScriptTask *task);
static int scEFFECTScript(ScriptTask *task);
static int scEFFECT2Script(ScriptTask *task);
static int scEFFECT3Script(ScriptTask *task);
static int scMOVIEScript(ScriptTask *task);
static int scMISSILEScript(ScriptTask *task);
static int scMISSILE3Script(ScriptTask *task);
extern void **svGetScript(int effectNo);
extern void sefDestroyScriptScheduler(int script_index);
extern int func_A32FA8(const char *filename);
extern int rand(void);
extern void func_A31500(int *position, int mode);
extern void func_A31920(int mode, int *position);

static short _cmdPut;

int _scMslCate;

static int (*_scFuncHandler[SCRIPT_COMMAND_COUNT])(ScriptTask *task);

static DoublewordAlignedVector4 _zeroPos_0041E290;

static short _cmdPut = 0;

int _scMslCate = 0;

const char registerDumpFormat[8] = " %04x";

ScriptRecord _scriptWork[SCRIPT_COUNT] = {{0}};

/* The constant position scCreateTask seeds every task with: (0, 0, 0, 1). */
static DoublewordAlignedVector4 _zeroPos_0041E290 = { 0.0f, 0.0f, 0.0f, 1.0f };

static int (*_scFuncHandler[SCRIPT_COMMAND_COUNT])(ScriptTask *task) = {
    scERRORScript, scGOScript, scGOSUBScript, scBRAScript,
    scEXITScript, scRETURNScript, scONGOScript, scONGOSUBScript,
    scERRORScript, scERRORScript, scPRINTScript, scRPUTScript,
    scRDUMPScript, scCMDPUTONScript, scCMDPUTOFFScript, scFADEONScript,
    scERRORScript, scERRORScript, scERRORScript, scERRORScript,
    scRSETScript, scRINCScript, scRDECScript, scRADDScript,
    scRSUBScript, scRMULScript, scRDIVScript, scRMODScript,
    scR_ANDScript, scR_ORScript, scR_XORScript, scR_NOTScript,
    scR_NEGScript, scRRNDScript, scREVEScript, scERRORScript,
    scERRORScript, scERRORScript, scERRORScript, scERRORScript,
    scOBJEVEScript, scABORTScript, scPAUSEScript, scERRORScript,
    scERRORScript, scERRORScript, scERRORScript, scERRORScript,
    scERRORScript, scERRORScript, scWAITCNTScript, scWAITEVEScript,
    scWAITEFTScript, scWAITMOVIEScript, scWAITMISSILEScript, scERRORScript,
    scERRORScript, scERRORScript, scERRORScript, scERRORScript,
    scEFFECTScript, scEFFECT2Script, scEFFECT3Script, scERRORScript,
    scERRORScript, scERRORScript, scERRORScript, scERRORScript,
    scERRORScript, scERRORScript, scMOVIEScript, scMISSILEScript,
    scMISSILE3Script
};

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
int _nowScript;

int _nowEvent;

/* Clears every script record and task slot, and stores each slot's task and script numbers. */
void scInitScript(void)
{
    int script;
    int task;

    _cmdPut = 0;
    for (script = 0; script < SCRIPT_COUNT; script++) {
        _scriptWork[script].dataTable = 0;
        _scriptWork[script].dataIndex = -1;
        _scriptWork[script].activeTasks = 0;
        _scriptWork[script].eventValue = 0;
        _scriptWork[script].event = 0;
        for (task = 0; task < SCRIPT_TASK_COUNT; task++) {
            ScriptTask *slot = &_scriptWork[script].tasks[task];

            memset(slot, 0, SCRIPT_TASK_SIZE);
            slot->task_no = task;
            slot->script_no = script;
        }
    }
}

static int scFindScriptData(int eftNo)
{
    return sefSearchMapperIndex(eftNo);
}

/*
 * Starts a script for an effect. A running script with the same effect number is
 * reused first, otherwise a free script record is taken. Returns the task index
 * with the script number in bits 8 and up, or a negative value on failure.
 */
int scCreateScript(ScriptTaskParameters *info)
{
    int effectNo;
    int dataIndex;
    int script;
    int task;
    void **entry;

    effectNo = info->effect_no;
    dataIndex = scFindScriptData(effectNo);
    if (dataIndex < 0) {
        tracePrint(noLoadEffectDataFormat, effectNo);
        return dataIndex;
    }
    for (script = 0; script < SCRIPT_COUNT; script++) {
        if (_scriptWork[script].dataTable != 0 && _scriptWork[script].event == effectNo) {
            task = scCreateTask(script, -1, _scriptWork[script].dataTable[_scriptWork[script].dataTable[0]], info);
            if (task >= 0) {
                return task | (script << 8);
            }
        }
    }
    for (script = 0; script < SCRIPT_COUNT; script++) {
        if (_scriptWork[script].dataTable == 0) {
            entry = svGetScript(effectNo);
            if (entry == 0) {
                return -1;
            }
            _scriptWork[script].event = effectNo;
            _scriptWork[script].dataIndex = dataIndex;
            _scriptWork[script].dataTable = *entry;
            task = scCreateTask(script, 0, _scriptWork[script].dataTable[_scriptWork[script].dataTable[0]], info);
            if (task >= 0) {
                task |= script << 8;
            }
            return task;
        }
    }
    return -1;
}

/* Removes the script's scheduler entry; when the script has data, clears the record and its tasks. */
void scDestroyScript(int script_index)
{
    int task;

    if (script_index >= 0) {
        sefDestroyScriptScheduler(script_index);
        if (_scriptWork[script_index].dataTable != 0) {
            _scriptWork[script_index].dataTable = 0;
            _scriptWork[script_index].dataIndex = -1;
            _scriptWork[script_index].activeTasks = 0;
            _scriptWork[script_index].eventValue = 0;
            _scriptWork[script_index].event = 0;
            for (task = 0; task < SCRIPT_TASK_COUNT; task++) {
                ScriptTask *slot = &_scriptWork[script_index].tasks[task];

                memset(slot, 0, SCRIPT_TASK_SIZE);
                slot->task_no = task;
                slot->script_no = script_index;
            }
        }
    }
}

/*
 * Destroys one task of a script. An out-of-range script index is reported and
 * ignored. Otherwise both indexes go to the scheduler, and the task is deleted
 * when the script has data.
 */
void scDestroyScript2(int script_index, int task_index)
{
    /* tracePrint's other callers in this TU (below) declare it with fewer
     * trailing arguments; this call needs both script_index and task_index. */
    extern int tracePrint(const char *fmt, int script_index, int task_index);

    if ((unsigned int)script_index >= SCRIPT_COUNT) {
        tracePrint(outOfRangeEffectDeleteFormat, script_index, task_index);
        return;
    }
    sefDestroyScriptScheduler2(script_index, task_index);
    if (_scriptWork[script_index].dataTable != 0) {
        scDeleteTask(script_index, task_index);
        return;
    }
}

void scDestroyScriptAll(void)
{
    int script_index;

    for (script_index = 0; script_index < SCRIPT_COUNT; script_index++)
        scDestroyScript(script_index);
}

static void scExecScript(void)
{
    int script;
    int task;
    ScriptRecord *scripts = 0;

    for (script = 0; script < SCRIPT_COUNT; script++) {
        _nowScript = script;
        if (_scriptWork[script].dataTable != 0) {
            _nowDataIdx = _scriptWork[script].dataIndex;
            if (_scriptWork[script].activeTasks == 0) {
                scDestroyScript(script);
            } else {
                for (task = 0; task < SCRIPT_TASK_COUNT; task++) {
                    _nowEvent = task;
                    scripts = _scriptWork;
                    scDispatchScript(&scripts[script].tasks[task]);
                }
            }
        }
    }
}

void scExecEffect(void)
{
    scExecScript();
    sefExecScheduler();
}

static int scCreateTask(int script_index, int task_index, int address, ScriptTaskParameters *data)
{
    int slot;
    ScriptTask *task;

    if (task_index >= SCRIPT_TASK_COUNT) {
        tracePrint(taskOutOfRangeFormat, task_index);
        return -1;
    }
    if (task_index < 0) {
        for (slot = 0; slot < SCRIPT_TASK_COUNT; slot++) {
            task = &_scriptWork[script_index].tasks[slot];
            if ((task->flags & SCRIPT_TASK_ACTIVE) == 0) {
                task_index = slot;
                break;
            }
        }
    }
    if (task_index >= 0) {
        task = &_scriptWork[script_index].tasks[task_index];
        task->flags = SCRIPT_TASK_ACTIVE;
        task->script_pc[task->branch_depth] = address;
        task->parameters = *data;
        task->effect_scheduler = -1;
        task->initial_position = _zeroPos_0041E290;
        task->slot_no = task_index;
        _scriptWork[script_index].eventValue = task_index;
        _scriptWork[script_index].activeTasks++;
    } else {
        tracePrint(taskWorkEmptyFormat, script_index);
    }
    return task_index;
}

/* Clears one task slot and decrements its script's active-task count when the slot was active. */
static void scDeleteTask(int script_index, int task_index)
{
    ScriptTask *task;
    int remaining_tasks;

    if ((unsigned int)task_index < SCRIPT_TASK_COUNT + 1u) {
        if (task_index < 0) {
            task = (ScriptTask *)0;
        } else {
            task = &_scriptWork[script_index].tasks[task_index];
        }

        if (task->flags != 0) {
            remaining_tasks = (ScSchedulerWord)_scriptWork[script_index].activeTasks;
            remaining_tasks -= 1;
            _scriptWork[script_index].activeTasks = (ScSchedulerWord)remaining_tasks;
        }

        task->flags = 0;
        task->branch_depth = 0;
        task->script_pc[0] = -1;
    }
}

static void scDeleteTaskAll(int selected_script, int unused_task_index)
{
    int script_index;
    int task_index;

    for (script_index = 0; script_index < SCRIPT_COUNT; script_index++) {
        if (selected_script < 0 || script_index == selected_script) {
            for (task_index = 0; task_index < SCRIPT_TASK_COUNT; task_index++)
                scDeleteTask(script_index, task_index);
        }
    }
}

/*
 * Reads a script register. A selector with bit 0x8000 set names another register,
 * whose value is used as the selector. An out-of-range selector is reported
 * through tracePrint and reads as 0.
 */
int scGetReg(int reg)
{
    int *address;
    int value;

    if (reg & SCRIPT_REG_INDIRECT) {
        reg = scGetReg(reg & ~SCRIPT_REG_INDIRECT);
    }
    if ((unsigned int)reg >= SCRIPT_REGISTER_COUNT) {
        tracePrint(registerOverflowFormat, reg);
        address = (int *)0;
    } else {
        address = &_scriptWork[_nowScript].registers[reg];
    }
    value = 0;
    if (address != (int *)0) {
        value = *address;
    }
    return value;
}

void scSetReg(int reg, int value)
{
    int *address;

    if (reg & SCRIPT_REG_INDIRECT) {
        reg = scGetReg(reg & ~SCRIPT_REG_INDIRECT);
    }
    if ((unsigned int)reg >= SCRIPT_REGISTER_COUNT) {
        tracePrint(registerOverflowFormat, reg);
        address = (int *)0;
    } else {
        address = &_scriptWork[_nowScript].registers[reg];
    }
    if (address != (int *)0) {
        *address = value;
    }
}

int scGetCmdScript(ScriptTask *task)
{
    short *command;
    int offset;

    unsigned short depth;

    command = (short *)0;
    offset = task->script_pc[task->branch_depth];
    depth = task->branch_depth;
    if (offset != 0) {
        command = _scriptWork[_nowScript].dataTable + offset;
    }
    task->script_pc[(short)depth]++;
    return *command;
}

/* Decodes the current command's next numeric operand: a literal, or the value of the register it names. */
int scGetNumScript(ScriptTask *task)
{
    int value;
    short reg;

    value = scGetCmdScript(task);
    if (value & SCRIPT_OPERAND_IMMEDIATE) {
        if (!(value & SCRIPT_OPERAND_SIGNED)) {
            value &= SCRIPT_OPERAND_VALUE_MASK;
        }
    } else {
        reg = scGetReg(value);
        value = reg;
    }
    return value;
}

int scGetRegScript(ScriptTask *task)
{
    int operand;
    int resolved_operand;

    operand = scGetCmdScript(task);
    if (operand & SCRIPT_OPERAND_IMMEDIATE) {
        resolved_operand = (short)scGetReg(operand & 0xFFFF7FFF);
    } else {
        resolved_operand = operand;
    }

    return resolved_operand;
}

int scGetAdrScript(ScriptTask *task)
{
    int address;

    address = scGetCmdScript(task) & 0xFFFF;
    if (address == SCRIPT_ADDRESS_NONE ||address == -1) {
        address = 0;
    }
    return address;
}

unsigned short scGetAdrIdx(ScriptTask *task, int index)
{
    int operand;
    unsigned short *table;
    int value;

    table = (unsigned short *)0;
    operand = task->script_pc[task->branch_depth];
    if (operand != 0) {
        table = (unsigned short *)((int)_scriptWork[_nowScript].dataTable + operand * 2);
    }
    value = table[index];
    if (value == SCRIPT_ADDRESS_NONE ||value == -1) {
        value = 0;
    }
    return value;
}

unsigned short scGetTableAdrIdx(int base, int index)
{
    unsigned short *table;
    int value;

    table = 0;
    if (base != 0) {
        table = (unsigned short *)_scriptWork[_nowScript].dataTable + base;
    }
    value = table[index];
    if (value == SCRIPT_ADDRESS_NONE ||value == -1) {
        value = 0;
    }
    return value;
}

int scGetTableAdrImmIdx(int base, int index)
{
    unsigned short entry;
    int address;

    entry = scGetTableAdrIdx(base, index);
    address = 0;
    if (entry != 0) {
        address = (int)_scriptWork[_nowScript].dataTable + entry * 2;
    }
    return address;
}

int scGetImmAdrImmIdx(const unsigned short *table, int index)
{
    int entry;
    int address;

    entry = table[index];
    if (entry == SCRIPT_ADDRESS_NONE ||entry == -1) {
        entry = 0;
    }
    address = 0;
    if (entry != 0) {
        address = (int)_scriptWork[_nowScript].dataTable + entry * 2;
    }
    return address;
}

int scGetImmAdrImmIdx2(int base, unsigned short *table, int index)
{
    int offset;
    int address;

    offset = table[index];
    if (offset == SCRIPT_ADDRESS_NONE ||offset == -1) {
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
    short *table;

    table = 0;
    if (base != 0) {
        table = _scriptWork[_nowScript].dataTable + base;
    }
    return table[index];
}

short scGetImmNumIdx(short *table, int index)
{
    return table[index];
}

int scGetAdrImmScript(ScriptTask *task)
{
    int table_index;
    ScriptRecord *script_record;
    unsigned short *table;

    table_index = scGetAdrScript(task);
    if (table_index == 0)
        return 0;

    script_record = &_scriptWork[_nowScript];
    table = (unsigned short *)script_record->dataTable;
    return (int)&table[table_index];
}

static int scERRORScript(ScriptTask *task)
{
    return 0;
}

static int scGOScript(ScriptTask *task)
{
    task->script_pc[task->branch_depth] = scGetAdrScript(task);
    return 1;
}

static int scGOSUBScript(ScriptTask *task)
{
    int destination = scGetAdrScript(task);
    short branch_depth = task->branch_depth;
    unsigned int next_depth = (unsigned short)task->branch_depth + 1;

    if (branch_depth < SCRIPT_BRANCH_DEPTH_LIMIT) {
        unsigned int shifted_depth = next_depth << 16;
        task->branch_depth = next_depth;
        task->script_pc[(int)shifted_depth >> 16] = destination;
    } else {
        tracePrint(effectStackOverflowFormat);
    }
    return 1;
}

static int scRETURNScript(ScriptTask *task)
{
    short depth = task->branch_depth;
    int depthU = (unsigned short)task->branch_depth;

    if (depth > 0) {
        task->branch_depth = depthU - 1;
        task->script_pc[depth] = 0;
    } else {
        tracePrint(effectStackUnderflowFormat, depthU);
    }
    return 1;
}

static int scEXITScript(ScriptTask *task)
{
    return 0;
}

static int scBRAScript(ScriptTask *task)
{
    int take_branch = 0;
    int left = scGetNumScript(task);
    int comparison = scGetCmdScript(task);
    int right = scGetNumScript(task);
    int destination = scGetAdrScript(task);

    switch (comparison) {
    case SCRIPT_COND_EQ: if (left == right) take_branch = 1; break;
    case SCRIPT_COND_NE: if (left != right) take_branch = 1; break;
    case SCRIPT_COND_GT: if (left > right) take_branch = 1; break;
    case SCRIPT_COND_GE: if (left >= right) take_branch = 1; break;
    case SCRIPT_COND_LT: if (left < right) take_branch = 1; break;
    case SCRIPT_COND_LE: if (left <= right) take_branch = 1; break;
    default:
        tracePrint(badExpressionFormat, comparison);
        break;
    }
    if (take_branch)
        task->script_pc[task->branch_depth] = destination;
    return 1;
}

static int scONGOSUB(ScriptTask *task)
{
    int index;
    int count;
    int next;
    int target;

    index = scGetNumScript(task);
    count = scGetCmdScript(task);
    next = task->script_pc[task->branch_depth] + count;
    if (index >= 0 && index < count) {
        target = scGetAdrIdx(task, index);
    } else {
        target = next;
    }
    task->script_pc[task->branch_depth] = next;
    return target;
}

static int scONGOScript(ScriptTask *task)
{
    int destination = scONGOSUB(task);
    short branch_depth = task->branch_depth;

    task->script_pc[branch_depth] = destination;
    return 1;
}

static int scONGOSUBScript(ScriptTask *task)
{
    int destination = scONGOSUB(task);

    if (task->branch_depth < SCRIPT_BRANCH_DEPTH_LIMIT) {
        task->branch_depth++;
        task->script_pc[task->branch_depth] = destination;
    }
    return 1;
}

static int scOBJEVEScript(ScriptTask *task)
{
    int number;
    int newTaskIndex;
    ScriptTask *newTask;

    number = scGetNumScript(task);
    newTaskIndex = scCreateTask(_nowScript, -1, scGetAdrScript(task), &task->parameters);
    if (newTaskIndex >= 0) {
        newTask = &_scriptWork[_nowScript].tasks[newTaskIndex];
        newTask->effect_scheduler = number;
    }
    return 1;
}

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

static int scPRINTScript(ScriptTask *task)
{
    char *string;
    int length;

    string = (char *)0;
    if (task->script_pc[task->branch_depth] != 0) {
        string = (char *)((int)_scriptWork[_nowScript].dataTable
            + task->script_pc[task->branch_depth] * 2);
    }
    length = strlen(string);
    length = (length & 1) ? length + 1 : length + 2;
    task->script_pc[task->branch_depth] += length / 2;
    return 1;
}

static int scRPUTScript(ScriptTask *task)
{
    scGetReg(scGetCmdScript(task));
    return 1;
}

static int scRDUMPScript(ScriptTask *task)
{
    char line[SCRIPT_DUMP_LINE_SIZE];
    int length;
    int i;
    int j;

    length = 0;
    for (i = 0; i < SCRIPT_DUMP_ROWS; i++) {
        for (j = 0; j < SCRIPT_DUMP_COLUMNS; j++) {
            length += sprintf(line + length, registerDumpFormat, scGetReg(i * SCRIPT_DUMP_COLUMNS + j));
        }
        line[0] = 0;
    }
    return 1;
}

static int scCMDPUTONScript(ScriptTask *task)
{
    _cmdPut = 1;
    return 1;
}

static int scCMDPUTOFFScript(ScriptTask *task)
{
    _cmdPut = 0;
    return 1;
}

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

static int scRRNDScript(ScriptTask *task)
{
    int reg;
    int low;
    int span;
    int value;

    reg = scGetCmdScript(task);
    low = scGetNumScript(task);
    span = scGetNumScript(task);
    if (span < low) {
        value = span;
        span = low;
        low = value;
    }
    span = (short)(span - low);
    if (span != 0) {
        value = (short)(rand() % span);
    } else {
        value = 0;
    }
    scSetReg(reg, value + low);
    return 1;
}

static int scREVEScript(ScriptTask *task)
{
    int reg;
    short value;

    reg = scGetCmdScript(task);
    value = _scriptWork[_nowScript].eventValue;
    scSetReg(reg, value);
    return 1;
}

static int scWAITCNTScript(ScriptTask *task)
{
    int wait_count;

    wait_count = scGetNumScript(task);
    if (wait_count <= 0)
        return 1;

    task->wait_frames = wait_count;
    /* Reuse the local after the count is stored to hold the yielded mode. */
    wait_count = SCRIPT_WAIT_FRAMES;
    task->wait_mode = wait_count;
    return 2;
}

static int scWAITEVEScript(ScriptTask *task)
{
    task->wait_operand = scGetNumScript(task);
    task->wait_mode = SCRIPT_WAIT_EVENT;
    return 2;
}

static int scWAITEFTScript(ScriptTask *task)
{
    scGetNumScript(task);
    task->wait_mode = SCRIPT_WAIT_EFFECT;
    task->wait_operand = task->effect_scheduler;
    return 2;
}

static int scWAITMOVIEScript(ScriptTask *task)
{
    task->wait_mode = SCRIPT_WAIT_MOVIE;
    return 2;
}

static int scWAITMISSILEScript(ScriptTask *task)
{
    task->wait_mode = SCRIPT_WAIT_MISSILE;
    return 2;
}

static int scFADEONScript(ScriptTask *task)
{
    task->flags |= SCRIPT_TASK_FADE_ON;
    return 1;
}

/* For a camera effect number in range, runs the camera setup once for each of modes 0 and 1. */
static void scFreezeCamera(int effect_no)
{
    int i;

    if ((unsigned int)(effect_no - SCRIPT_CAMERA_EFFECT_FIRST) < SCRIPT_CAMERA_EFFECT_COUNT) {
        int modes[2] = {0, 1};
        int params[SCRIPT_CAMERA_PARAM_WORDS];

        for (i = 0; i < 2; i++) {
            memset(params, 0, sizeof(params));
            func_A31500(&params[4], modes[i]);
            func_A31920(modes[i], (params[0] = 0, params[28] = 0, params));
        }
    }
}

/* Starts the ambient sound of the task's effect, using the fade variant while the task is fading. Only task 0 plays it. */
static void scSetAmbient(ScriptTask *task)
{
    short effectNo;

    if (_nowEvent != 0) {
        return;
    }
    effectNo = task->parameters.effect_no;
    if (task->flags & SCRIPT_TASK_FADE_ON) {
        sdvSetAmbState2(1, effectNo);
        return;
    }
    sdvSetAmbState(1, effectNo);
}

static int scEFFECTScript(ScriptTask *task)
{
    int address;
    int schedulerId;

    address = scGetAdrImmScript(task);
    if (address != 0) {
        schedulerId = sefCreateScheduler(address, &task->parameters, &task->initial_position,
            (int)_scriptWork[_nowScript].dataTable, -1);
        task->effect_scheduler = schedulerId;
        sefCreateBattleActorTbl(task->parameters.effect_no);
        if (schedulerId >= 0) {
            scSetAmbient(task);
            scFreezeCamera(task->parameters.effect_no);
        }
    }
    return 1;
}

static int scEFFECT2Script(ScriptTask *task)
{
    int address;
    int command;
    int selector;
    int next;
    int entry;

    address = 0;
    command = scGetCmdScript(task);
    selector = srsGetEffect2Idx(task->parameters.effect_no);
    if ((unsigned int)((unsigned short)task->parameters.effect_no - SCRIPT_SINGLE_VARIANT_EFFECT_FIRST) < SCRIPT_SINGLE_VARIANT_EFFECT_COUNT) {
        selector = 1;
    }
    next = task->script_pc[task->branch_depth] + command;
    if (selector < command) {
        entry = scGetAdrIdx(task, selector);
        if (entry == 0) {
            address = 0;
        } else {
            address = (int)_scriptWork[_nowScript].dataTable + entry * 2;
        }
    }
    task->script_pc[task->branch_depth] = next;
    if (address != 0) {
        task->effect_scheduler = sefCreateScheduler(address, &task->parameters, &task->initial_position,
            (int)_scriptWork[_nowScript].dataTable, -1);
        sefCreateBattleActorTbl(task->parameters.effect_no);
    }
    return 1;
}

static int scEFFECT3Script(ScriptTask *task)
{
    int count;
    int firstAddress;
    int secondAddress;
    int savedWord;
    short savedHalf;
    int schedulerId;

    count = scGetNumScript(task);
    firstAddress = scGetAdrImmScript(task);
    secondAddress = scGetAdrImmScript(task);
    if (_nowEvent == 0) {
        savedWord = task->parameters.scheduler_word;
        savedHalf = task->parameters.scheduler_halfword;
        task->parameters.scheduler_word = 0;
        task->parameters.scheduler_halfword = 0;
        schedulerId = sefCreateScheduler(firstAddress, &task->parameters, &task->initial_position,
            (int)_scriptWork[_nowScript].dataTable, -1);
        task->parameters.scheduler_word = savedWord;
        task->parameters.scheduler_halfword = savedHalf;
        if (schedulerId >= 0) {
            scSetAmbient(task);
            scFreezeCamera(task->parameters.effect_no);
        }
    }
    task->effect_scheduler = sefCreateScheduler2(secondAddress, &task->parameters, &task->initial_position,
        (int)_scriptWork[_nowScript].dataTable, count * _nowEvent, -1);
    sefCreateBattleActorTbl(task->parameters.effect_no);
    return 1;
}

static int scMOVIEScript(ScriptTask *task)
{
    char *name;
    int length;
    char path[256];

    name = (char *)0;
    if (task->script_pc[task->branch_depth] != 0) {
        name = (char *)_scriptWork[_nowScript].dataTable
            + task->script_pc[task->branch_depth] * 2;
    }
    length = strlen(name);
    length = (length & 1) ? length + 1 : length + 2;
    task->script_pc[task->branch_depth] += length / 2;
    if (_nowEvent == 0) {
        sprintf(path, "%s%s", simajiriMoviePath, name);
        func_A32FA8(path);
    }
    return 1;
}

static int scMISSILEScript(ScriptTask *task)
{
    int charId;
    int address;

    address = scGetAdrImmScript(task);
    if (address != 0 && _nowEvent == 0) {
        task->missile_adr = address;
        task->missile_active = 1;
        task->flags |= SCRIPT_TASK_MISSILE;
        task->missile_wait_frames = 0;
        task->missile_record_index = 0;
        srsAnalyzeEftNo(task->parameters.effect_no, &charId, &_scMslCate);
    }
    return 1;
}

static int scMISSILE3Script(ScriptTask *task)
{
    int charId;
    int address;
    int count;

    count = scGetNumScript(task);
    address = scGetAdrImmScript(task);
    if (address != 0) {
        task->missile_active = 1;
        task->missile_adr = address;
        task->flags |= SCRIPT_TASK_MISSILE;
        task->missile_record_index = 0;
        task->missile_wait_frames = -count * _nowEvent;
        srsAnalyzeEftNo(task->parameters.effect_no, &charId, &_scMslCate);
    }
    return 1;
}

/* Advances one task: resumes it while it waits, otherwise parses its script, then moves its missile if it has one. */
static int scDispatchScript(ScriptTask *task)
{
    unsigned short flags = task->flags;

    if (flags == 0)
        return 0;
    if (flags & SCRIPT_TASK_PAUSED)
        return 4;

    if (flags & SCRIPT_TASK_WAITING) {
        scWaitParseScript(task);
        flags = task->flags;
    }

    if (!(flags & SCRIPT_TASK_WAITING)) {
        scParseScript(task);
        flags = task->flags;
    }
    if (flags & SCRIPT_TASK_MISSILE)
        scMoveParseScript(task);
    return 1;
}

/* Runs script commands until a handler asks to stop. A handler that returns 0
 * ends the script, so its task is deleted; every other exit leaves it alone. */
static inline void scRunScript(ScriptTask *task)
{
    int count;
    int result;
    int command;
    unsigned short flags;

    count = 0;
    while (command = scGetCmdScript(task), (result = _scFuncHandler[command](task)) != 0) {
        flags = task->flags;
        if (flags == 0 || (flags & SCRIPT_TASK_PAUSED)) {
            return;
        }
        if (result == 2) {
            task->flags = flags | SCRIPT_TASK_WAITING;
            return;
        }
        count++;
        if (count > SCRIPT_RUN_STEP_LIMIT) {
            return;
        }
    }
    scDeleteTask(_nowScript, _nowEvent);
}

static int scParseScript(ScriptTask *task)
{
    scRunScript(task);
    return 1;
}

/* A table offset of 0xFFFF (or -1) marks an absent entry. */
static inline int scOffsetOrNone(int offset)
{
    int result = offset;

    if (offset == SCRIPT_ADDRESS_NONE ||offset == -1) {
        result = 0;
    }
    return result;
}

/*
 * Returns the script table of an effect and writes the offset of its entry to
 * *offset. Returns NULL when the effect has no entry.
 */
short *scAnalyzeScriptCf(int effectNo, int *offset)
{
    short *table;
    void **entry;
    int index;
    int value;
    int tableOffset;

    index = scFindScriptData(effectNo);
    *offset = 0;
    if (index < 0) {
        tracePrint(noLoadEffectDataFormat, effectNo);
        return (short *)0;
    }
    entry = svGetScript(effectNo);
    if (entry != 0) {
        table = *entry;
        if (table != 0) {
            value = 0;
            if (table[8] == SCRIPT_TABLE_KIND_FIRST_ENTRY) {
                tableOffset = (unsigned short)table[9];
                value = scOffsetOrNone(tableOffset);
            } else if (table[8] == SCRIPT_TABLE_KIND_INDEXED) {
                index = srsGetEffect2Idx(effectNo);
                if (index < table[9]) {
                    tableOffset = (unsigned short)table[10 + index];
                    value = scOffsetOrNone(tableOffset);
                }
            } else if (table[8] == SCRIPT_TABLE_KIND_SECOND_ENTRY) {
                tableOffset = (unsigned short)table[10];
                value = scOffsetOrNone(tableOffset);
            }
            if (value == 0) {
                return (short *)0;
            }
            *offset = value;
            return table;
        }
    }
    return (short *)0;
}

ScriptTask *scGetTaskAdr(int script_index, int task_index)
{
    if (task_index < 0)
        return 0;

    return &_scriptWork[script_index].tasks[task_index];
}

/* Returns the address of a register of the current script, or NULL when the number is out of range. */
void *scGetRegAdr(unsigned int reg)
{
    if (reg >= SCRIPT_REGISTER_COUNT) {
        tracePrint(registerOverflowFormat, reg);
        return (void *)0;
    }
    return &_scriptWork[_nowScript].registers[reg];
}

int scOfsToAdr(int offset)
{
    if (offset == SCRIPT_ADDRESS_NONE ||offset == -1) {
        offset = 0;
    }
    return offset;
}

int scAdrToImm(int index)
{
    int address;

    address = 0;
    if (index != 0) {
        address = (int)_scriptWork[_nowScript].dataTable + index * 2;
    }
    return address;
}

int scGetCmdAdrScript(ScriptTask *task)
{
    int operand;
    int address;

    address = 0;
    operand = task->script_pc[task->branch_depth];
    if (operand != 0) {
        address = (int)_scriptWork[_nowScript].dataTable + operand * 2;
    }
    return address;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
static const char noLoadEffectDataFormat[40] = "--- error no load effect data %d";

/* JP: 範囲外のエフェクトを消去しようとしてます %d %d
 * EN: Attempting to delete an out-of-range effect %d %d */
static const char outOfRangeEffectDeleteFormat[48] =
    "\xC8\xCF\xB0\xCF\xB3\xB0\xA4\xCE\xA5\xA8\xA5\xD5\xA5\xA7\xA5\xAF\xA5\xC8\xA4\xF2\xBE\xC3\xB5\xEE\xA4\xB7\xA4\xE8\xA4\xA6\xA4\xC8\xA4\xB7\xA4\xC6\xA4\xDE\xA4\xB9\x20\x25\x64\x20\x25\x64";

/* JP: 範囲外のTask(%d)です
 * EN: Task(%d) is out of range */
static const char taskOutOfRangeFormat[24] = "\xC8\xCF\xB0\xCF\xB3\xB0\xA4\xCE" "Task(%d)" "\xA4\xC7\xA4\xB9";

static const char taskWorkEmptyFormat[24] = "task work is empty : %d";

static const char registerOverflowFormat[24] = "register overflow : %d";

static const char effectStackOverflowFormat[32] = "--- effect stack over flow";

static const char effectStackUnderflowFormat[32] = "--- effect stack under flow";

static const char badExpressionFormat[16] = "--- bad exp(%d)";

static const char simajiriMoviePath[24] = "data\\simajiri\\movie\\";

int _nowScript = 0;

int _nowEvent = 0;

int _nowDataIdx = 0;
