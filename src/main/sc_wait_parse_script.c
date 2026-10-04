#include "common.h"
#include "shared.h"
#include "sc_wait_parse_script.h"
#include "main/sef.h"

static int (*moveHandlerTbl_1[])(ScriptObject *);

/* WAITNOP: no condition to wait on; the command finishes on its first visit. */
static int scWaitParseNopScript(void)
{
    return 1;
}

/*
 * WAITCNT: the wait is over once `wait_frames`, decremented once per visit,
 * has run out.
 */
static int scWaitParseCntScript(ScriptObject *script)
{
    int remaining_frames = script->wait_frames - 1;

    script->wait_frames = remaining_frames;
    return remaining_frames < 1;
}

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

/*
 * WAITEFT: the wait is over once the effect scheduler scWAITEFTScript named
 * (`wait_operand`) has died.  Once it has, this also restores the ambient
 * state the effect's own SETAMB command replaced: bit 8 of `flags` picks
 * sdvSetAmbState2 over sdvSetAmbState, both given `amb_effect_no` as the
 * effect index.
 */
static short scWaitParseEftScript(ScriptObject *script)
{
    int scheduler_dead = sefIsDeadSchduler(script->wait_operand);

    if (scheduler_dead)
    {
        short amb_effect_no = script->amb_effect_no;

        if (script->flags & 0x100)
            sdvSetAmbState2(2, amb_effect_no);
        else
            sdvSetAmbState(2, amb_effect_no);
    }

    return scheduler_dead;
}

/* WAITMOVIE: the wait is over once the movie player reports it stopped. */
static int scWaitParseMovieScript(void)
{
    return func_A33248() == 0;
}

/* WAITMOV: the wait is over while `flags` bit 5 is clear. */
static int scWaitParseMovScript(ScriptObject *script)
{
    return ((script->flags >> 5) ^ 1) & 1;
}

extern int scWaitMissileScript(ScriptObject *);

static int (*waitHandlerTbl_0[6])(ScriptObject *) = {
    (int (*)(ScriptObject *))scWaitParseNopScript,
    scWaitParseCntScript,
    scWaitParseEveScript,
    (int (*)(ScriptObject *))scWaitParseEftScript,
    (int (*)(ScriptObject *))scWaitParseMovieScript,
    scWaitParseMovScript
};


int scWaitParseScript(ScriptObject *script)
{
    unsigned short wait_kind = script->wait_kind;
    int result;

    if (wait_kind >= 6) {
        script->wait_kind = 0;
        return 1;
    }

    result = waitHandlerTbl_0[(short)wait_kind](script);
    if (result != 0)
        script->flags &= ~0x10;

    return (short)result;
}

INCLUDE_ASM("asm/main/nonmatchings/sc_wait_parse_script", scWaitMissileScript);

int scMoveParseScript(ScriptObject *script)
{
    unsigned short move_kind = script->move_kind;
    int result;

    if (move_kind >= 2) {
        script->move_kind = 0;
        return 1;
    }

    result = moveHandlerTbl_1[(short)move_kind](script);
    if (result != 0)
        script->flags &= ~0x20;

    return (short)result;
}

static int (*moveHandlerTbl_1[])(ScriptObject *) = {
    scWaitParseNopScript, scWaitMissileScript
};
