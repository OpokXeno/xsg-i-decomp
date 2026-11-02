/*
 * TU-local declarations of main/tu213 (src/main/sc_get.c).
 */

#ifndef SRC_MAIN_SC_GET_H
#define SRC_MAIN_SC_GET_H

#include "shared.h"
#include "main/sc_get.h"

typedef unsigned short ScSchedulerWord;

typedef unsigned int ScSchedulerWord32;

typedef struct ScriptRecord ScriptRecord;

/*
 * ScriptRecord is the 1104-byte (0x450) per-script record _scriptWork indexes
 * (the same stride ScriptTask's own comment documents). scGetTableAdrIdx,
 * scGetTableAdrImmIdx, scGetImmAdrImmIdx, scGetTableNumIdx, scAdrToImm,
 * scGetCmdAdrScript and scGetRegAdr all read the 4-byte pointer at +0x400;
 * only that field is evidenced here.
 */
struct ScriptRecord {
    ScriptTask tasks[SCRIPT_TASK_COUNT]; /* eight 0x80-byte task slots */
    short *dataTable; /* +0x400 */
    int registers[SCRIPT_REGISTER_COUNT]; /* +0x404, register selectors 0..15 */
    short dataIndex; /* +0x444: scCreateScript's mapper result */
    short eventValue; /* +0x446: returned by scREVEScript */
    short activeTasks; /* +0x448: number of active task slots */
    short event; /* +0x44A: event number matched by scCreateScript */
    unsigned char unmodeled_44C[4];
};

extern ScriptRecord _scriptWork[SCRIPT_COUNT];

extern int scWaitParseScript(ScriptTask *task);
extern int scMoveParseScript(ScriptTask *task);
static int scONGOSUB(ScriptTask *task);
static int scParseScript(ScriptTask *task);

#endif /* SRC_MAIN_SC_GET_H */
