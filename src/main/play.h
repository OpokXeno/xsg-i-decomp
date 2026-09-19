/*
 * TU-local declarations of main/tu150 (src/main/play.c).
 */

#ifndef SRC_MAIN_PLAY_H
#define SRC_MAIN_PLAY_H

#include "shared.h"

/*
 * The engine's playback controller, recovered head.
 *
 * `PLAY_setTimeChart` (main:0x0026a770) is the only function in this TU that
 * touches the object so far: it stores its second argument at +0x08. Its
 * caller Java_xeno_PlayControl_loadTimeChart (main:0x002fa4c0, not in this
 * TU) reads the pointer at +0x00 of the Java peer it is handed as this
 * object's address, and the value at +0x04 of that same peer as the value
 * stored here, matching both the annotation ("Assigns a time-chart resource
 * to the supplied playback controller") and the Java method name
 * (`loadTimeChart`). PLAY_ctrl passes this member of `playControl`
 * (the object PLAY_getCurrent returns) as the first argument of TCH_getInfo,
 * which dereferences it, so the member is a pointer to a time-chart resource
 * whose layout belongs to tch.c.
 *
 * Nothing between +0x00 and +0x08 is recovered yet: the other PLAY_*
 * functions of this TU (`PLAY_getCurrent`, `PLAY_setupDefault`, `PLAY_setup`,
 * `PLAY_setObserver`, `PLAY_ctrl`, `PLAY_getCallBackParams`,
 * `PLAY_checkTCH`) are still assembly and are the evidence for any further
 * member.
 */

/*
 * +0x04 of the span above is `state`: PLAY_setupDefault clears it
 * (sw $0,4 at 0x0026a650). Nothing between +0x00 and +0x04 is recovered.
 */

/*
 * The pointer PLAY_setupDefault clears (sw $0,12 at 0x0026a674). Its
 * pointee is read by PLAY_setup, still assembly in this TU, so it stays
 * opaque here.
 */
typedef struct PlaySource PlaySource;

/*
 * One registered playback observer (Play.observers, 32 entries of 12 bytes
 * each starting at +0x4c): PLAY_setupDefault clears the callback argument
 * and method at +0x00/+0x04 for every entry (sw $0 at 0x0026a690/
 * 0x0026a698 through the 32-entry loop at 0x0026a680). The chart/key
 * selectors PLAY_setObserver stores at +0x08/+0x0a, still assembly in this
 * TU, are not modeled here.
 */
typedef struct PlayObserver {
    SceneObject argument;          /* +0x00 */
    SceneMethod *method;           /* +0x04 */
    unsigned char unmodeled_08[4]; /* +0x08 */
} PlayObserver;

/*
 * The playback controller's embedded callback-parameter object
 * (PLAY_getCallBackParams, VA 0x0026ab90): +0x00 is set from
 * classJava_xeno_util_TCHParams->instance_class_ref (lw 24 at 0x0026ab9c),
 * the same class-ref slot SceneObjectHeader keeps at its own +0x00, so a
 * Java peer can be read back through this object. The rest of the object
 * is touched only by PLAY_checkTCH, still assembly in this TU.
 */
typedef struct TCHParams {
    SceneObjectClassRef *class_ref; /* +0x00 */
    unsigned char unmodeled_04[12]; /* +0x04 */
} TCHParams;

extern SceneClass *classJava_xeno_util_TCHParams;

/*
 * The per-function .lit4 float constants of this TU (still scaffold-owned):
 * D_004D7D14/D_004D7D18 (10/3 and 1/30) are PLAY_setupDefault's defaults.
 */
extern const float D_004D7D14;
extern const float D_004D7D18;

/*
 * The remaining members PLAY_setupDefault evidences (PLAY_setup,
 * PLAY_setObserver and PLAY_ctrl, still assembly in this TU, evidence
 * `source`'s pointee, the observer chart/key selectors, `flags`,
 * `cameraIndex`, `startTime`/`endTime`/`frameStep` and `currentTime`'s
 * further use): `source` is cleared at 0x0026a674; `flags` is cleared at
 * 0x0026a668; `cameraIndex` is set to -1 (sh $2,48 at 0x0026a648);
 * `startTime`/`endTime`/`frameStep` and `currentTime` are defaulted at
 * 0x0026a660/0x0026a658/0x0026a654/0x0026a65c; `observers` is the 32-slot
 * table the loop at 0x0026a680 clears; `callbackParams` is the object
 * PLAY_getCallBackParams returns, ending exactly at the object's
 * 0x1dc-byte size (config/symbols/main.txt).
 */
typedef struct Play {
    unsigned char unmodeled_00[4];
    unsigned int state;              /* +0x04 */
    void *timeChart;
    PlaySource *source;              /* +0x0c */
    unsigned char unmodeled_10[28];  /* +0x10 */
    unsigned int flags;              /* +0x2c */
    short cameraIndex;                /* +0x30 */
    unsigned char unmodeled_32[2];   /* +0x32 */
    float startTime;                 /* +0x34 */
    float endTime;                   /* +0x38 */
    float frameStep;                 /* +0x3c */
    unsigned char unmodeled_40[4];   /* +0x40 */
    float currentTime;               /* +0x44 */
    unsigned char unmodeled_48[4];   /* +0x48 */
    PlayObserver observers[32];      /* +0x4c */
    TCHParams callbackParams;        /* +0x1cc */
} Play;

extern Play playControl;

#endif
