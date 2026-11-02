#ifndef INCLUDE_MAIN_SC_GET_H
#define INCLUDE_MAIN_SC_GET_H

#include "shared.h"

typedef struct ScriptTaskParameters {
    unsigned char unmodeled_00[4];
    int scheduler_word;
    unsigned char unmodeled_08[4];
    short missile_subtype;               /* +0x0C: selects the 527/528 command kind */
    unsigned char unmodeled_0e[6];
    short effect_no;                     /* +0x14: ambient effect index (ScriptTask +0x44) */
    short scheduler_halfword;
    unsigned char unmodeled_18[0x0C];
} ScriptTaskParameters;

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
 * 4, 0x10 and 0x20 of it.  The wait handlers (src/main/sc_wait_parse_script.c)
 * clear bit 4 once the wait is over, test bit 5 (scWaitParseMovScript) and bit
 * 8 (scWaitParseEftScript) of the same halfword.
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
 * scDeleteTask (sh $0 at 0x002ea268).  scCreateTask copies 32 bytes from its
 * caller into +0x30 (`parameters`) and writes halfwords at +0x56 and +0x58.
 * +0x56 is the scheduler handle `effect_scheduler` (scEFFECTScript and
 * siblings store it, scWAITEFTScript reads it); +0x58 is `slot_no`.
 *
 * `wait_mode` at +0x5A records why the task yielded: scWAITCNTScript sets it
 * to 1 for a frame-count wait (sh at 0x002eb310), and scWAITEVEScript,
 * scWAITEFTScript, scWAITMOVIEScript and scWAITMISSILEScript set it to 2, 3, 4
 * and 5 (sh at 0x002eb344, 0x002eb37c, 0x002eb3a4, 0x002eb3b4).  The wait
 * handler selector scWaitParseScript reads it from the same halfword: it
 * refuses a value of 6 or more and zeroes it, and otherwise calls
 * waitHandlerTbl[wait_mode] (0x002ebeb0..0x002ebedc).
 *
 * `wait_frames` at +0x5E is the mode-1 frame count: scWAITCNTScript stores it
 * (sh at 0x002eb308) and scWaitParseCntScript counts it down once per visit,
 * reporting the wait finished at zero (lh/addiu -1/sh/slti 1 at
 * 0x002ebd98..0x002ebda8).
 *
 * `wait_operand` at +0x60 holds the operand for modes 2 and 3: the numeric
 * operand scGetNumScript() returns (scWAITEVEScript, sh at 0x002eb340; also the
 * event-task index scWaitParseEveScript passes to scGetTaskAdr, lh 0x60 at
 * 0x002ebdb8) or the copied `effect_scheduler` (scWAITEFTScript, sh at
 * 0x002eb384; scWaitParseEftScript passes it to sefIsDeadSchduler, lh 0x60 at
 * 0x002ebe08).  Modes 4 and
 * 5 need none.
 *
 * `parameters.effect_no` at +0x44 (within the +0x20..+0x54 span above), a signed
 * halfword both scSetAmbient (lh 0x44 at 0x002eb48c) and scWaitParseEftScript
 * (lh 0x44 at 0x002ebe1c) read; it is handed to sdvSetAmbState/sdvSetAmbState2
 * or srsAnalyzeEftNo as an effect number.
 *
 * Missile state: scMISSILEScript sets `missile_active` at +0x5C (sh 1 at
 * 0x002eb91c) and writes `missile_adr` at +0x70 (sw at 0x002eb910, the address
 * scGetAdrImmScript() resolved).  The wait handlers view the same word as
 * `missile_data`, the serialized command block scWaitMissileScript reads, and
 * copy `missile_spawn_context` (+0x30) into the missile parameters.
 * `missile_wait_frames` (+0x66) and `missile_record_index` (+0x68) are halfwords
 * scMISSILEScript clears (sh $0 at 0x002eb92c and 0x002eb930) with no read
 * evidenced in the script task allocation; the wait handlers read them as the
 * missile wait count (scWaitMissileScript) and the record index.
 *
 * `move_kind` at +0x5C is the scMoveParseScript handler selector: the same
 * halfword as `missile_active`, read as unsigned.
 *
 * The record ends at +0x74: scGetTaskAdr strides the per-script task array at
 * 128 bytes, so the slot is that long.
 */
typedef struct ScriptTask ScriptTask;

struct ScriptTask {
    unsigned short flags;             /* +0x00 */
    unsigned char unmodeled_02[2];    /* +0x02 */
    short task_no;                    /* +0x04 */
    short script_no;                  /* +0x06 */
    int script_pc[6];                 /* +0x08 */
    DoublewordAlignedVector4 initial_position; /* +0x20..+0x2F */
    union {
        ScriptTaskParameters parameters;  /* +0x30..+0x53, copied as one call block */
        int missile_spawn_context[3];     /* +0x30, copied into missile parameters */
    };
    short branch_depth;               /* +0x54 */
    unsigned short effect_scheduler;  /* +0x56 */
    short slot_no;                    /* +0x58, scCreateTask stores its slot index */
    unsigned short wait_mode;         /* +0x5A */
    union {
        short missile_active;         /* +0x5C */
        unsigned short move_kind;     /* +0x5C */
    };
    short wait_frames;                /* +0x5E */
    short wait_operand;               /* +0x60 */
    unsigned char unmodeled_62[4];    /* +0x62 */
    short missile_wait_frames;        /* +0x66 */
    short missile_record_index;       /* +0x68 */
    unsigned char unmodeled_6A[6];    /* +0x6A */
    union {
        int missile_adr;              /* +0x70 */
        struct MissileCommandData *missile_data;  /* +0x70 */
    };
    unsigned char unmodeled_74[0x0C]; /* +0x74..+0x7F */
};

int scGetImmAdrImmIdx2(int base, unsigned short *table, int index);
ScriptTask *scGetTaskAdr(int script_index, int task_index);

/* Functions and data used by other translation units. */
extern int _nowScript;

#endif /* INCLUDE_MAIN_SC_GET_H */
