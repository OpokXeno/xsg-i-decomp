/*
 * TU-local declarations of main/tu213 (src/main/sc_get.c).
 */

#ifndef SRC_MAIN_SC_GET_H
#define SRC_MAIN_SC_GET_H

typedef struct ScriptTask ScriptTask;

/*
 * One slot of the script VM's task table.  The table is `_scriptWork`, indexed
 * as `script * 1104 + task * 128`, with at most 8 task slots per script
 * (scCreateTask 0x002ea028: slot stride sll 7 at 0x002ea110, script stride
 * 1104 at 0x002ea10c..0x002ea11c, bound slti 0x8 at 0x002ea0ac) and a 16-byte
 * script header at +0x440 after them.
 *
 * `flags` is the halfword at +0: scCreateTask sets bit 0 when it takes a slot
 * (sh 1 at 0x002ea13c) and tests it when it looks for a free one (lhu/andi 1 at
 * 0x002ea08c); scDeleteTask clears the whole halfword (sh $0 at 0x002ea264) and
 * treats a nonzero value as "this slot was in use"; scDispatchScript tests bits
 * 4, 0x10 and 0x20 of it.  scDeleteTask's own accepted record called this
 * halfword `active` and modelled it as its own type ScTaskSlot; the bit tests
 * above show the two are one record and one flags word, so they are spelled
 * once here.
 *
 * `script_pc` is the branch stack: scCreateTask seeds entry 0 with the script
 * address it was given (lh 0x54 / sll 2 / sw 0x8 at 0x002ea134..0x002ea14c),
 * scONGOScript writes the branch target to the entry `branch_depth` selects
 * (the same three instructions at 0x002eaabc..0x002eaacc), and scDeleteTask
 * invalidates entry 0 with -1 (sw at 0x002ea26c).  Its six entries are bounded
 * by the 16 bytes scCreateTask zeroes from _zeroPos at +0x20 (sd at
 * 0x002ea1a4/0x002ea1c0), which is where the stack stops.
 *
 * `branch_depth` is the signed halfword at +0x54 that indexes it, cleared by
 * scDeleteTask (sh $0 at 0x002ea268).  Between +0x02 and +0x08, and from +0x20
 * to +0x54, this unit's C reads nothing: scCreateTask copies 32 bytes from its
 * caller into +0x30 and writes halfwords at +0x56 and +0x58, but what they mean
 * is not established here, so those spans stay byte ranges.
 */
/*
 * This allocation's own WAIT opcode handlers evidence three more slots past
 * `branch_depth`:
 *
 * `effect_scheduler` is the sefCreateScheduler() handle scEFFECTScript,
 * scEFFECT2Script and scEFFECT3Script store at +0x56 (still asm; sh at
 * 0x002eb534, 0x002eb670, 0x002eb7c4); scWAITEFTScript reads it (lhu at
 * 0x002eb374) and copies it into `wait_value` below.
 *
 * `wait_mode` at +0x5A records why the task yielded: scWAITCNTScript (still
 * asm) sets it to 1 for an elapsed frame count (sh at 0x002eb310), and
 * scWAITEVEScript, scWAITEFTScript, scWAITMOVIEScript and scWAITMISSILEScript
 * set it to 2, 3, 4 and 5 (sh at 0x002eb344, 0x002eb37c, 0x002eb3a4,
 * 0x002eb3b4).
 *
 * `wait_value` at +0x60 holds the operand for modes 2 and 3: the event
 * number scGetNumScript() returns (scWAITEVEScript, sh at 0x002eb340) or the
 * copied `effect_scheduler` (scWAITEFTScript, sh at 0x002eb384). Modes 4 and
 * 5 need none; mode 1's own count lives at +0x5E (scWAITCNTScript, still
 * asm, sh at 0x002eb308), outside this allocation.
 *
 * +0x58 and +0x5C..+0x60 stay unmodeled: nothing this allocation reads or
 * writes touches them.
 */
struct ScriptTask {
    unsigned short flags;             /* +0x00 */
    unsigned char unmodeled_02[6];    /* +0x02 */
    int script_pc[6];                 /* +0x08 */
    unsigned char unmodeled_20[52];   /* +0x20 */
    short branch_depth;               /* +0x54 */
    unsigned short effect_scheduler;  /* +0x56 */
    unsigned char unmodeled_58[2];    /* +0x58 */
    unsigned short wait_mode;         /* +0x5A */
    unsigned char unmodeled_5C[4];    /* +0x5C */
    short wait_value;                 /* +0x60 */
};

extern int scWaitParseScript(ScriptTask *task);

extern int scMoveParseScript(ScriptTask *task);

#include "shared.h"

typedef unsigned short ScSchedulerWord;

typedef unsigned int ScSchedulerWord32;

extern unsigned char _scriptWork[];

extern int scONGOSUB(ScriptTask *task);

extern int scParseScript(ScriptTask *task);

#endif /* SRC_MAIN_SC_GET_H */
