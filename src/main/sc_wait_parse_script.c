#include "common.h"
#include "shared.h"
#include "sc_wait_parse_script.h"

INCLUDE_ASM("asm/main/nonmatchings/sc_wait_parse_script", scWaitParseNopScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_wait_parse_script", scWaitParseCntScript);

/*
 * WAITEVE: the wait is over once the event task the command names has gone
 * away or stopped running.  `wait_operand` is the task index scWAITEVEScript
 * stored when it parsed the command, and scGetTaskAdr returns 0 for a negative
 * one (bltz at 0x002ebc58), which counts as finished.
 */
static int scWaitParseEveScript(ScriptObject *script)
{
    EventTask *event_task = scGetTaskAdr(_nowScript, script->wait_operand);

    return event_task == 0 || event_task->active == 0;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_wait_parse_script", scWaitParseEftScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_wait_parse_script", scWaitParseMovieScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_wait_parse_script", scWaitParseMovScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_wait_parse_script", scWaitParseScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_wait_parse_script", scWaitMissileScript);

INCLUDE_ASM("asm/main/nonmatchings/sc_wait_parse_script", scMoveParseScript);
