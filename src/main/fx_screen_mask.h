/*
 * TU-local declarations of main/tu151 (src/main/fx_screen_mask.c).
 */

#ifndef SRC_MAIN_FX_SCREEN_MASK_H
#define SRC_MAIN_FX_SCREEN_MASK_H

#include "shared.h"

/*
 * The task body of this unit's fade tasks, behind the scheduler's 0x80-byte
 * pool node.  The first 16 bytes are the XglTaskPrefix xglTaskEntryNext
 * maintains (src/main/xgl_task.c); the words after them belong to the entry
 * point that allocated the node.  FX_ScreenMask (0x0026adb8) and fxAdapter
 * (0x0026ace0) both initialise the same three of them with the same stores
 * (sw 0x1c/0x10/0x14 at 0x0026ae14..0x0026ae1c and 0x0026ad50..0x0026ad58).
 *
 * `flags` bit 0 is the latch that makes the first visit of a task different
 * from the rest: screenMask tests and sets it at 0x0026ac78..0x0026ac8c and
 * fxAdapter at 0x0026acf4..0x0026ad08, both then seeding `frame` with -1.
 * `next_callback` is the callback fxAdapter chains the successor task with
 * (lw $5,0x14 at 0x0026ad24, the callback argument of the xglTaskEntryNext
 * call at 0x0026ad3c); it is cleared by both entry points.  `state` is the
 * GameLoopState record, stored by both entry points and not read by this
 * unit's C.  The word at +0x18 is touched by nothing here.
 *
 * `frame` is the visit counter: seeded with -1 on the latched first visit and
 * incremented before it is compared, so the first compared value is 0
 * (screenMask 0x0026ac9c..0x0026acbc, fxAdapter 0x0026ad0c..0x0026ad1c).  It is
 * written as a halfword and read back unsigned, and screenMask compares it
 * against `duration` sign-extended from 16 bits (sll/sra 16 at 0x0026acac).
 * `duration` and the two words either side of it are the fade parameters
 * FX_ScreenMask takes from its caller (sw 0x28/0x20/0x24 at
 * 0x0026ae20..0x0026ae28); only `duration` has an evidenced role in this unit's
 * C, and fxAdapter copies all four words 0x20..0x2c into the successor task
 * (the loop at 0x0026ad68).  Between +0x2c and +0x40 nothing here reads or
 * writes the node.
 */
typedef struct {
    XglTaskPrefix entry;                        /* +0x00 */
    unsigned int flags;                         /* +0x10 */
    int (*next_callback)(XglTaskPrefix *task);  /* +0x14 */
    unsigned char unmodeled_18[4];              /* +0x18 */
    void *state;                                /* +0x1c */
    int color;                                  /* +0x20 */
    int duration;                               /* +0x24 */
    int mode;                                   /* +0x28 */
    unsigned char unmodeled_2c[20];             /* +0x2c */
    short frame;                                /* +0x40 */
} ScreenMaskTask;

/*
 * FX_ScreenMask (0x0026adb8) stores its own three fade arguments directly
 * into `color`, `duration` and `mode` (sw 0x20/0x24/0x28 at
 * 0x0026ae20..0x0026ae28).
 */

int xglTaskRemove(XglTaskPrefix *task);

extern void nmlModelSetFadeDoit(void);

/*
 * This common address view is used only for the proved pointer-sized access
 * at byte offset +8 (the task scheduler, read at 0x0026adf0).  It is not a
 * complete GameLoopState layout or a claim about elements at offsets +0/+4.
 */
typedef void *GameLoopStateAddressView[];

extern GameLoopStateAddressView GameLoopState;

/* canon: config/header-canon.json chose src/core/main-0021c5c0/xglTaskEntryPrev.c over 0 other accepted spellings */
XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                int (*callback)(XglTaskPrefix *task),
                                XglTaskPrefix *entry);

#endif /* SRC_MAIN_FX_SCREEN_MASK_H */
