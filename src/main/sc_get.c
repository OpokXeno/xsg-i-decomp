#include "common.h"

#include "shared.h"

#include "sc_get.h"

/* Sixteen zero-filled script records, each one 0x450 bytes from the observed index stride. */

ScriptRecord _scriptWork[];

#define SCRIPT_WORK_BYTES ((unsigned char *)(void *)_scriptWork)

/* EUC-JP: "clear effect outside the valid range %d %d". */

static const char D_004CC7B8[];

static const char D_004CC818[];

static const char D_004CC850[];

static short _cmdPut;

int _scMslCate;

int _nowScript;

int _nowEvent;

/*
 * scFindScriptData is a genuine tail call (`j sefSearchMapperIndex`, not
 * `jal`) into the mapper-index search main/tu211 (src/main/sef.c) defines;
 * that function is still INCLUDE_ASM there.
 */

extern int sefSearchMapperIndex(int eftNo);

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

extern void scDestroyScript(int script_index);

/* scExecScript (above) is still INCLUDE_ASM; declare it so its caller does
 * not see an implicit declaration. */

static void scExecScript(void);

/*
 * sefExecScheduler runs the effect scheduler's tick; it is defined in
 * main/tu211 (src/main/sef.c), still INCLUDE_ASM there. scExecEffect calls
 * scExecScript, then genuinely tail-calls (`j`, not `jal`) into it.
 */

extern void sefExecScheduler(void);

/*
 * Invalidate one task slot and decrement its owning script's active count.
 * The original storage declarations are unavailable; the slot's recovered
 * members are in sc_get.h, and the script header at +0x440 of each script
 * record keeps its measured offsets, because only its active-task counter has
 * an evidenced role here.
 */

extern int tracePrint();

/*
 * scGetReg resolves a script-register selector the same way scGetRegAdr
 * below does (bit 0x8000 set means "read the register that selector names,
 * then use its value as the real selector"; an out-of-range selector reports
 * the error through tracePrint and reads as 0), but keeps its own copy of the
 * address computation and bound check instead of calling scGetRegAdr.
 */

/* Same indirect-selector and bound-check logic as scGetReg above, writing
 * `value` into the resolved register instead of reading it. */

extern int scGetCmdScript(ScriptTask *task);

extern int scGetReg(int reg);

extern int scGetAdrScript(ScriptTask *task);

static int scCreateTask(int script_index, int task_index, int address, ScriptTaskParameters *data);

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

extern unsigned int strlen(const char *s);

const char D_004DBB38[];

/* Preserve the earlier external reference above while owning its bytes here. */

/* Keep the active script scalars after scRDUMPScript's small-data format. */

extern void scSetReg(int reg, int value);

extern void sdvSetAmbState(int state, int effect_no);

extern void sdvSetAmbState2(int state, int effect_no);

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

static void scFreezeCamera(int effect_no);

extern char *srsAnalyzeEftNo(int effectId, int *charId, int *effectCategory);

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

static int (*_scFuncHandler[73])(ScriptTask *task) = {
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

/* Sixteen zero-filled script records, each one 0x450 bytes from the observed index stride. */

static const char D_004CC790[];

static const char D_004CC7E8[];

static const char D_004CC800[];

static const char D_004CC830[];

static const char D_004CC870[];

static const char D_004CC898[];

static ScriptZeroPosition _zeroPos_0041E290;

extern void **svGetScript(int effectNo);

extern void sefDestroyScriptScheduler(int script_index);

extern int srsGetEffect2Idx(int effectNo);

extern int sefCreateScheduler2(int address, void *taskData, void *positionData, int table, int flags, int taskIndex);

extern int func_A32FA8(const char *filename);

extern int rand(void);

extern void func_A31500(int *position, int mode);

extern void func_A31920(int mode, int *position);

void scInitScript(void)
{
    int script;
    int task;

    _cmdPut = 0;
    for (script = 0; script < 16; script++) {
        _scriptWork[script].dataTable = 0;
        _scriptWork[script].dataIndex = -1;
        _scriptWork[script].activeTasks = 0;
        _scriptWork[script].eventValue = 0;
        _scriptWork[script].event = 0;
        for (task = 0; task < 8; task++) {
            ScriptTask *slot = &_scriptWork[script].tasks[task];

            memset(slot, 0, 128);
            slot->task_no = task;
            slot->script_no = script;
        }
    }
}

static int scFindScriptData(int eftNo)
{
    return sefSearchMapperIndex(eftNo);
}

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
        tracePrint((unsigned char *)D_004CC790, effectNo);
        return dataIndex;
    }
    for (script = 0; script < 16; script++) {
        if (_scriptWork[script].dataTable != 0 && _scriptWork[script].event == effectNo) {
            task = scCreateTask(script, -1, ((short *)_scriptWork[script].dataTable)[((short *)_scriptWork[script].dataTable)[0]], info);
            if (task >= 0) {
                return task | (script << 8);
            }
        }
    }
    for (script = 0; script < 16; script++) {
        if (_scriptWork[script].dataTable == 0) {
            entry = svGetScript(effectNo);
            if (entry == 0) {
                return -1;
            }
            _scriptWork[script].event = effectNo;
            _scriptWork[script].dataIndex = dataIndex;
            _scriptWork[script].dataTable = *entry;
            task = scCreateTask(script, 0, ((short *)_scriptWork[script].dataTable)[((short *)_scriptWork[script].dataTable)[0]], info);
            if (task >= 0) {
                task |= script << 8;
            }
            return task;
        }
    }
    return -1;
}

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
            for (task = 0; task < 8; task++) {
                ScriptTask *slot = &_scriptWork[script_index].tasks[task];

                memset(slot, 0, 128);
                slot->task_no = task;
                slot->script_no = script_index;
            }
        }
    }
}

void scDestroyScript2(int script_index, int task_index)
{
    /* tracePrint's other callers in this TU (below) declare it with fewer
     * trailing arguments; this call needs both script_index and task_index. */
    extern int tracePrint(unsigned char *fmt, int script_index, int task_index);

    if ((unsigned int)script_index >= 16u) {
        tracePrint((unsigned char *)D_004CC7B8, script_index, task_index);
        return;
    }
    sefDestroyScriptScheduler2(script_index, task_index);
    if (((ScriptRecord *)(SCRIPT_WORK_BYTES + script_index * 1104))->dataTable != (int *)0) {
        scDeleteTask(script_index, task_index);
        return;
    }
}

void scDestroyScriptAll(void)
{
    int script_index;

    for (script_index = 0; script_index < 16; script_index++)
        scDestroyScript(script_index);
}

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scExecScript);

void scExecEffect(void)
{
    scExecScript();
    sefExecScheduler();
}

static int scCreateTask(int script_index, int task_index, int address, ScriptTaskParameters *data)
{
    int slot;
    ScriptTask *task;

    if (task_index >= 8) {
        tracePrint((unsigned char *)D_004CC7E8, task_index);
        return -1;
    }
    if (task_index < 0) {
        for (slot = 0; slot < 8; slot++) {
            task = &_scriptWork[script_index].tasks[slot];
            if ((task->flags & 1) == 0) {
                task_index = slot;
                break;
            }
        }
    }
    if (task_index >= 0) {
        task = &_scriptWork[script_index].tasks[task_index];
        task->flags = 1;
        task->script_pc[task->branch_depth] = address;
        task->parameters = *data;
        task->effect_scheduler = -1;
        task->initial_position.words[0] = _zeroPos_0041E290.words[0];
        task->slot_no = task_index;
        _scriptWork[script_index].eventValue = task_index;
        _scriptWork[script_index].activeTasks++;
        task->initial_position.words[1] = _zeroPos_0041E290.words[1];
    } else {
        tracePrint((unsigned char *)D_004CC800, script_index);
    }
    return task_index;
}

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
            task_scripts = SCRIPT_WORK_BYTES;
            task = (ScriptTask *)(task_scripts + task_offset);
        }

        if (task->flags != 0) {
            count_scripts = SCRIPT_WORK_BYTES;
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

static void scDeleteTaskAll(int selected_script, int unused_task_index)
{
    int script_index;
    int task_index;

    for (script_index = 0; script_index < 16; script_index++) {
        if (selected_script < 0 || script_index == selected_script) {
            for (task_index = 0; task_index < 8; task_index++)
                scDeleteTask(script_index, task_index);
        }
    }
}

int scGetReg(int reg)
{
    int *address;
    int value;

    if (reg & 0x8000) {
        reg = scGetReg(reg & ~0x8000);
    }
    if ((unsigned int)reg >= 16u) {
        tracePrint((unsigned char *)D_004CC818, reg);
        address = (int *)0;
    } else {
        address = (int *)(SCRIPT_WORK_BYTES + _nowScript * 1104 + 0x404 + reg * 4);
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

    if (reg & 0x8000) {
        reg = scGetReg(reg & ~0x8000);
    }
    if ((unsigned int)reg >= 16u) {
        tracePrint((unsigned char *)D_004CC818, reg);
        address = (int *)0;
    } else {
        address = (int *)(SCRIPT_WORK_BYTES + _nowScript * 1104 + 0x404 + reg * 4);
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
        command = (short *)_scriptWork[_nowScript].dataTable + offset;
    }
    task->script_pc[(short)depth]++;
    return *command;
}

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

int scGetRegScript(ScriptTask *task)
{
    int operand;
    int resolved_operand;

    operand = scGetCmdScript(task);
    if (operand & 0x8000) {
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
    if (address == 0xFFFF || address == -1) {
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
        table = (unsigned short *)((int)((ScriptRecord *)(SCRIPT_WORK_BYTES + _nowScript * 1104))->dataTable + operand * 2);
    }
    value = table[index];
    if (value == 0xFFFF || value == -1) {
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
    if (value == 0xFFFF || value == -1) {
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
    if (entry == 0xFFFF || entry == -1) {
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
    short *table;

    table = 0;
    if (base != 0) {
        table = (short *)_scriptWork[_nowScript].dataTable + base;
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

    script_record = (ScriptRecord *)(SCRIPT_WORK_BYTES + _nowScript * 1104);
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

    if (branch_depth < 3) {
        unsigned int shifted_depth = next_depth << 16;
        task->branch_depth = next_depth;
        task->script_pc[(int)shifted_depth >> 16] = destination;
    } else {
        ((int (*)(unsigned char *))tracePrint)((unsigned char *)D_004CC830);
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
        tracePrint((unsigned char *)D_004CC850, depthU);
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
    case 0: if (left == right) take_branch = 1; break;
    case 1: if (left != right) take_branch = 1; break;
    case 2: if (left > right) take_branch = 1; break;
    case 3: if (left >= right) take_branch = 1; break;
    case 4: if (left < right) take_branch = 1; break;
    case 5: if (left <= right) take_branch = 1; break;
    default:
        tracePrint((unsigned char *)D_004CC870, comparison);
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

    if (task->branch_depth < 3) {
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
        newTask = (ScriptTask *)(SCRIPT_WORK_BYTES + _nowScript * 1104 + newTaskIndex * 128);
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
        string = (char *)((int)((ScriptRecord *)(SCRIPT_WORK_BYTES + _nowScript * 1104))->dataTable
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
    value = ((ScriptRecord *)(SCRIPT_WORK_BYTES + _nowScript * 1104))->eventValue;
    scSetReg(reg, value);
    return 1;
}

static int scWAITCNTScript(ScriptTask *task)
{
    int wait_count;

    wait_count = scGetNumScript(task);
    if (wait_count <= 0)
        return 1;

    task->wait_count = wait_count;
    /* Reuse the local after the count is stored to hold the yielded mode. */
    wait_count = 1;
    task->wait_mode = wait_count;
    return 2;
}

static int scWAITEVEScript(ScriptTask *task)
{
    task->wait_value = scGetNumScript(task);
    task->wait_mode = 2;
    return 2;
}

static int scWAITEFTScript(ScriptTask *task)
{
    scGetNumScript(task);
    task->wait_mode = 3;
    task->wait_value = task->effect_scheduler;
    return 2;
}

static int scWAITMOVIEScript(ScriptTask *task)
{
    task->wait_mode = 4;
    return 2;
}

static int scWAITMISSILEScript(ScriptTask *task)
{
    task->wait_mode = 5;
    return 2;
}

static int scFADEONScript(ScriptTask *task)
{
    task->flags |= 0x100;
    return 1;
}

static void scFreezeCamera(int effect_no)
{
    int i;

    if ((unsigned int)(effect_no - 2000) < 100u) {
        int modes[2] = {0, 1};
        int params[44];

        for (i = 0; i < 2; i++) {
            memset(params, 0, 176);
            func_A31500(&params[4], modes[i]);
            func_A31920(modes[i], (params[0] = 0, params[28] = 0, params));
        }
    }
}

static void scSetAmbient(ScriptTask *task)
{
    short effectNo;

    if (_nowEvent != 0) {
        return;
    }
    effectNo = task->parameters.effect_no;
    if (task->flags & 0x100) {
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
            (int)((ScriptRecord *)(SCRIPT_WORK_BYTES + _nowScript * 1104))->dataTable, -1);
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
    if ((unsigned int)((unsigned short)task->parameters.effect_no - 2900) < 99u) {
        selector = 1;
    }
    next = task->script_pc[task->branch_depth] + command;
    if (selector < command) {
        entry = scGetAdrIdx(task, selector);
        if (entry == 0) {
            address = 0;
        } else {
            address = (int)((unsigned char *)_scriptWork[_nowScript].dataTable + entry * 2);
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
        sprintf(path, "%s%s", D_004CC898, name);
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
        task->flags |= 0x20;
        task->missile_reset0 = 0;
        task->missile_reset1 = 0;
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
        task->flags |= 0x20;
        task->missile_reset1 = 0;
        task->missile_reset0 = -count * _nowEvent;
        srsAnalyzeEftNo(task->parameters.effect_no, &charId, &_scMslCate);
    }
    return 1;
}

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
    }

    if (!(flags & 0x10)) {
        scParseScript(task);
        flags = task->flags;
    }
    if (flags & 0x20)
        scMoveParseScript(task);
    return 1;
}

static int scParseScript(ScriptTask *task)
{
    int count;
    int result;
    int command;
    unsigned short flags;

    count = 0;
    while (command = scGetCmdScript(task), (result = _scFuncHandler[command](task)) != 0) {
        flags = task->flags;
        if (flags == 0 || (flags & 4)) {
            goto done;
        }
        if (result == 2) {
            task->flags = flags | 0x10;
            goto done;
        }
        count++;
        if (count > 2000) {
            goto done;
        }
    }
    scDeleteTask(_nowScript, _nowEvent);
done:
    return 1;
}

/* A table offset of 0xFFFF (or -1) marks an absent entry. */
static inline int scOffsetOrNone(int offset)
{
    int result = offset;

    if (offset == 0xFFFF || offset == -1) {
        result = 0;
    }
    return result;
}

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
        tracePrint((unsigned char *)D_004CC790, effectNo);
        return (short *)0;
    }
    entry = svGetScript(effectNo);
    if (entry != 0) {
        table = *entry;
        if (table != 0) {
            value = 0;
            if (table[8] == 60) {
                tableOffset = (unsigned short)table[9];
                value = scOffsetOrNone(tableOffset);
            } else if (table[8] == 61) {
                index = srsGetEffect2Idx(effectNo);
                if (index < table[9]) {
                    tableOffset = (unsigned short)table[10 + index];
                    value = scOffsetOrNone(tableOffset);
                }
            } else if (table[8] == 62) {
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

struct EventTask *scGetTaskAdr(int script_index, int task_index)
{
    if (task_index < 0)
        return 0;

    return (struct EventTask *)(SCRIPT_WORK_BYTES + script_index * 1104 + task_index * 128);
}

void *scGetRegAdr(unsigned int reg)
{
    if (reg >= 16u) {
        tracePrint((unsigned char *)D_004CC818, reg);
        return (void *)0;
    }
    return SCRIPT_WORK_BYTES + _nowScript * 1104 + 0x404 + reg * 4;
}

int scOfsToAdr(int offset)
{
    if (offset == 0xFFFF || offset == -1) {
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

ScriptRecord _scriptWork[16] = {{0}};
static const char D_004CC7B8[48] =
    "\xC8\xCF\xB0\xCF\xB3\xB0\xA4\xCE\xA5\xA8\xA5\xD5\xA5\xA7\xA5\xAF\xA5\xC8\xA4\xF2\xBE\xC3\xB5\xEE\xA4\xB7\xA4\xE8\xA4\xA6\xA4\xC8\xA4\xB7\xA4\xC6\xA4\xDE\xA4\xB9\x20\x25\x64\x20\x25\x64";
static const char D_004CC818[24] = "register overflow : %d";
static const char D_004CC850[32] = "--- effect stack under flow";
static short _cmdPut = 0;
int _scMslCate = 0;
int _nowScript = 0;
int _nowEvent = 0;
const char D_004DBB38[8] = " %04x";
static const char D_004CC790[40] = "--- error no load effect data %d";
static const char D_004CC7E8[24] = "\xC8\xCF\xB0\xCF\xB3\xB0\xA4\xCE" "Task(%d)" "\xA4\xC7\xA4\xB9";
static const char D_004CC800[24] = "task work is empty : %d";
static const char D_004CC830[32] = "--- effect stack over flow";
static const char D_004CC870[16] = "--- bad exp(%d)";
static const char D_004CC898[24] = "data\\simajiri\\movie\\";
static ScriptZeroPosition _zeroPos_0041E290 = {{0, 0x3f80000000000000ULL}};


