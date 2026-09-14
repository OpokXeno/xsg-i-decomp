#include "common.h"
#include "shared.h"
#include "sc_get.h"

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scInitScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scFindScriptData);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scCreateScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scDestroyScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scDestroyScript2);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scDestroyScriptAll);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scExecScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scExecEffect);

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

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetAdrScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetAdrIdx);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetTableAdrIdx);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetTableAdrImmIdx);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetImmAdrImmIdx);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetImmAdrImmIdx2);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetTableNumIdx);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetImmNumIdx);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetAdrImmScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scERRORScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGOScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGOSUBScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRETURNScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scEXITScript);

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

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scABORTScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scPAUSEScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scPRINTScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRPUTScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRDUMPScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scCMDPUTONScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scCMDPUTOFFScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRSETScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRINCScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRDECScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRADDScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRSUBScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRMULScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRDIVScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRMODScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scR_ANDScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scR_ORScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scR_XORScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scR_NOTScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scR_NEGScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scRRNDScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scREVEScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scWAITCNTScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scWAITEVEScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scWAITEFTScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scWAITMOVIEScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scWAITMISSILEScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scFADEONScript);

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

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scOfsToAdr);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scAdrToImm);

INCLUDE_ASM("asm/main/nonmatchings/sc_get", scGetCmdAdrScript);
