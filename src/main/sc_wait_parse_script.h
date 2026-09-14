/*
 * TU-local declarations of main/tu214 (src/main/sc_wait_parse_script.c).
 */

#ifndef SRC_MAIN_SC_WAIT_PARSE_SCRIPT_H
#define SRC_MAIN_SC_WAIT_PARSE_SCRIPT_H

#include "shared.h"

typedef struct EventTask {
    unsigned short active;
} EventTask;

extern EventTask *scGetTaskAdr(int script_index, int task_index);

typedef struct ScriptObject ScriptObject;

/*
 * The script command record the wait handlers of this unit are dispatched on.
 *
 * The members below are the ones this original object establishes for itself;
 * everything between them is an explicit byte range, not an invented span.
 *
 *   +0x00 flags         scWaitParseScript clears bit 4 of it once the handler
 *                       it dispatched reports the wait finished (lhu/andi
 *                       0xffef/sh at 0x002ebeec..0x002ebef4), and
 *                       scWaitParseEftScript tests bit 8 of the same halfword
 *                       to pick which ambient-state call it makes
 *                       (0x002ebe18..0x002ebe24).
 *   +0x5a wait_kind     the handler selector: scWaitParseScript reads it,
 *                       refuses a value of 6 or more and zeroes it, and
 *                       otherwise calls waitHandlerTbl[wait_kind]
 *                       (0x002ebeb0..0x002ebedc).  scWAITEVEScript
 *                       (src/main/sc_get.c, 0x002eb344) writes 2 here and
 *                       scWAITEFTScript (0x002eb37c) writes its own kind.
 *   +0x5e wait_frames   scWaitParseCntScript counts it down once per visit and
 *                       reports the wait finished when it reaches zero
 *                       (lh/addiu -1/sh/slti 1 at 0x002ebd98..0x002ebda8).
 *   +0x60 wait_operand  the numeric operand of the WAIT command, taken from
 *                       scGetNumScript by both producers (sh 0x60 at
 *                       0x002eb340 and 0x002eb384).  Each handler reads it in
 *                       its own terms: scWaitParseEveScript passes it to
 *                       scGetTaskAdr as the event-task index (lh 0x60 at
 *                       0x002ebdb8) and scWaitParseEftScript passes it to
 *                       sefIsDeadSchduler (lh 0x60 at 0x002ebe08).
 *
 * This is not the complete record.  scGetTaskAdr strides the per-script task
 * array at 128 bytes (sll 7 at 0x002ebc4c), so the object is at least that
 * long, and nothing else of it is recovered here.
 */
struct ScriptObject {
    unsigned short flags;               /* +0x00 */
    unsigned char unmodeled_02[0x58];   /* +0x02 */
    unsigned short wait_kind;           /* +0x5a */
    unsigned char unmodeled_5c[2];      /* +0x5c */
    short wait_frames;                  /* +0x5e */
    short wait_operand;                 /* +0x60 */
};

extern int _nowScript;

#endif /* SRC_MAIN_SC_WAIT_PARSE_SCRIPT_H */
