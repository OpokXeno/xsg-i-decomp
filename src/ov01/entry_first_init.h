/*
 * TU-local declarations of ov01/tu007 (src/ov01/entry_first_init.c).
 */

#ifndef SRC_OV01_ENTRY_FIRST_INIT_H
#define SRC_OV01_ENTRY_FIRST_INIT_H

#include "shared.h"

int fadeEndChk(void);

extern int fadeFlag;

extern int cameraFlag;

/* The battle entry sequencer's current phase, one of the entryPhaseNN
   functions of this TU, all shaped int (void). */
typedef int (*EntryPhaseFunc)(void);
extern EntryPhaseFunc battleSeq;

/*
 * vramCopyObj's task-specific payload past the shared ObjectTask head
 * (include/shared.h): the source/destination frame-buffer addresses it
 * hands to grVramCopyFBtoFB before releasing the one-shot task.
 */
typedef struct VramCopyObjTask {
    ObjectTask task;               /* +0x00 */
    unsigned char unmodeled_14[0x1C - 0x14];
    int srcAddr;                   /* +0x1C */
    int dstAddr;                   /* +0x20 */
} VramCopyObjTask;

/*
 * fadeOutObj's task-specific payload past the shared ObjectTask head: a
 * countdown (`life`) that fadeOutObj decrements by `speed` each tick, and a
 * `delay` tick count fadeOutObj counts down to zero first, before it starts
 * touching `life` at all.  When `life` reaches zero, fadeOutObj draws the
 * final frame, sets fadeFlag and releases the task.
 */
typedef struct FadeObjTask {
    ObjectTask task;                /* +0x00 */
    unsigned char unmodeled_14[0x1C - 0x14];
    int life;                       /* +0x1C */
    int speed;                      /* +0x20 */
    int delay;                      /* +0x24 */
} FadeObjTask;

#endif /* SRC_OV01_ENTRY_FIRST_INIT_H */
