/*
 * TU-local declarations of main/tu140 (src/main/disp_on.c).
 */

#ifndef SRC_MAIN_DISP_ON_H
#define SRC_MAIN_DISP_ON_H

#include "shared.h"

/*
 * The game loop's own record, as far as this unit reads it.  Three spans are
 * evidenced, and nothing between them is modelled: the task scheduler every
 * entry point of this unit allocates from (lw 0x8 at 0x0025abc0, 0x0025ac58,
 * 0x0025ace0 and 0x0026adf0), the runtime flags word disptest and loader test
 * and update (lw 0x10 at 0x0025a8d0 and 0x0025aa4c, sw at 0x0025a928), and the
 * halfword loader copies the map's event index into (sh 0x52 at 0x0025aae8).
 * This is not a complete GameLoopState layout: the object is still owned by the
 * generated .sdata scaffolding and the rest of it is unrecovered.
 */
typedef struct {
    unsigned char unmodeled_00[8];      /* +0x00 */
    XglTaskScheduler *task_scheduler;   /* +0x08 */
    unsigned char unmodeled_0c[4];      /* +0x0c */
    unsigned int runtime_flags;         /* +0x10 */
    unsigned char unmodeled_14[62];     /* +0x14 */
    unsigned short map_event_index;     /* +0x52 */
} GameLoopStateRecord;

extern GameLoopStateRecord GameLoopState;

/*
 * The task body behind the scheduler's 0x80-byte pool node.
 *
 * xglTaskEntryNext hands out a node whose first 16 bytes are the XglTaskPrefix
 * it maintains itself (src/main/xgl_task.c); everything after them belongs to
 * the entry point that allocated the node.  All six entry points of this unit
 * initialise the same three words with the same three stores, in this order:
 * DISP_on (0x0025a998..0x0025a9a4), DISP_off (0x0025aa08..0x0025aa14),
 * LoadMap (0x0025abec..0x0025abf4), LoadMap2 (0x0025ac84..0x0025ac8c),
 * LoadMapOnly (0x0025ad04..0x0025ad0c) and setEventTimerTaskEntry
 * (0x0025adc8..0x0025add4).
 *
 * `flags` is the word loader latches bit 0 of, so that the body below it runs
 * once per task (lw/andi/ori 0x10 at 0x0025aa78..0x0025aa98); screenMask
 * (0x0026ac78) and fxAdapter (0x0026acf4) latch the same bit of the same word
 * on their own nodes.  `next_callback` is cleared by every entry point here and
 * never read in this unit; the role in the name is fxAdapter's, which loads
 * this word (lw $5,0x14 at 0x0026ad24) as the callback argument of the
 * xglTaskEntryNext call that chains the successor task.  `state` is the
 * GameLoopState record the callback works on: disptest reads it at 0x0025a8cc
 * and loader at 0x0025aa48, both to reach its flags word.  The word at +0x18 is
 * touched by nothing in this unit and stays unmodelled.
 *
 * From +0x20 on the node is the callback's own working area, and the three
 * types below model what each entry-point/callback pair of this unit actually
 * writes and reads there.  That the original declared three record types over
 * one header rather than one record with a union is not established; the
 * offsets, the widths and the roles are.
 */
typedef struct {
    XglTaskPrefix entry;                            /* +0x00 */
    unsigned int flags;                             /* +0x10 */
    int (*next_callback)(XglTaskPrefix *task);      /* +0x14 */
    unsigned char unmodeled_18[4];                  /* +0x18 */
    GameLoopStateRecord *state;                     /* +0x1c */
} GameTaskHeader;

/*
 * DISP_on/DISP_off (0x0025a950, 0x0025a9c0) and their callback disptest
 * (0x0025a8c0).  disptest counts `delay` down once per visit (lw/addiu/sw 0x24
 * at 0x0025a8ec..0x0025a8f8) and, when it goes negative, applies `request` to
 * bit 1 of the state's flags word: 0 sets it (ori 0x2 at 0x0025a924), 1 clears
 * it (and -3 at 0x0025a914..0x0025a91c), and any other value only removes the
 * task.
 */
typedef struct {
    GameTaskHeader header;  /* +0x00 */
    int request;            /* +0x20 */
    int delay;              /* +0x24 */
} DispSwitchTask;

#define DISP_REQUEST_ON  0
#define DISP_REQUEST_OFF 1

/*
 * LoadMap (0x0025ab90), LoadMap2 (0x0025ac20) and LoadMapOnly (0x0025acb8), and
 * their callback loader (0x0025aa38).  `request` selects what loader does with
 * `map_id`: LoadMapOnly writes 0x100 (change the map only), LoadMap/LoadMap2
 * write 0x102 (change the map, then run the map's event script), and 0x101,
 * which no entry point of this unit writes, runs the script without changing
 * the map.  `event_index` is written as a word (-1 by LoadMap at 0x0025ac10,
 * the caller's value by LoadMap2 at 0x0025ac90) and read back as a halfword
 * (lhu 0x28 at 0x0025aadc) into the state record's own halfword at +0x52.
 */
typedef struct {
    GameTaskHeader header;  /* +0x00 */
    int request;            /* +0x20 */
    int map_id;             /* +0x24 */
    int event_index;        /* +0x28 */
} MapLoadTask;

#define MAP_LOAD_MAP_ONLY      0x100
#define MAP_LOAD_SCRIPT_ONLY   0x101
#define MAP_LOAD_MAP_AND_EVENT 0x102

/*
 * setEventTimerTaskEntry (0x0025ada0) and its callback EventTimerTask
 * (0x0025ad30), which counts `countdown` down once per visit (lw/addiu/sw 0x20
 * at 0x0025ad70..0x0025ad7c) and passes `method_reference` to CallMethod when
 * it reaches zero (lw 0x30 at 0x0025ad84).
 */
typedef struct {
    GameTaskHeader header;      /* +0x00 */
    int countdown;              /* +0x20 */
    unsigned char unmodeled_24[12]; /* +0x24 */
    const char *method_reference;   /* +0x30 */
} EventTimerWork;

/* canon: config/header-canon.json chose src/core/main-0021c5c0/xglTaskEntryPrev.c over 0 other accepted spellings */
XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                int (*callback)(XglTaskPrefix *task),
                                XglTaskPrefix *entry);

/* canon: config/header-canon.json chose src/core/main-0021c6b0/xglTaskRemove.c over 0 other accepted spellings */
int xglTaskRemove(XglTaskPrefix *task);

void DISP_on(int countdown);

static int disptest(XglTaskPrefix *task);

void DISP_off(int countdown);

void setEventTimerTaskEntry(const char *method_reference, int countdown);

extern int EventTimerTask(XglTaskPrefix *task);

#endif /* SRC_MAIN_DISP_ON_H */
