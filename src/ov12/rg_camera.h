/*
 * TU-local declarations of ov12/tu016 (src/ov12/rg_camera.c).
 */

#ifndef SRC_OV12_RG_CAMERA_H
#define SRC_OV12_RG_CAMERA_H

#include "shared.h"
#include "ov12/rg_draw.h"

/*
 * The robot-game abstract camera record. Its layout is not recovered here:
 * _IsActionRoll/_IsActionAdvance/_IsActionAttack/_IsActionSeeTarget/
 * _IsActionLockAndAttack/_IsActionLockOn/_IsActionDash reach only the
 * actionFlags word at +0x44, through the named-offset fallback of
 * docs/style.md rule 2 (the same one rg_matrices_effector.h's
 * MATRIX_CONSTRAINT_POINT_TO uses).
 */
typedef struct RgCamera RgCamera;

#define RG_CAMERA_ACTION_FLAGS(camera) (*(int *)((unsigned char *)(camera) + 0x44))

/*
 * _InitAbstructCamera (ov12:0x00a10f78) attests the leading span: the draw
 * studio pointer at +0x00 (its own "pStudio != NIL" assert), the eye and
 * target vectors it clears at +0x10/+0x20, the up vector it copies from
 * XrgVectorY() at +0x30, and the seven-word action-state block it resets at
 * +0x40..+0x58 (all zero except index 1, +0x44, the actionFlags word above,
 * which it also clears; the other six indices' individual roles are not
 * evidenced yet). +0x04..+0x0f is untouched by any claimed function.
 */
struct RgCamera {
    RgDrawStudio *studio;              /* +0x00 */
    unsigned char unmodeled_04[0x0c];  /* +0x04 */
    RgVector eye;                      /* +0x10 */
    RgVector target;                   /* +0x20 */
    RgVector up;                       /* +0x30 */
    int actionState[7];                /* +0x40..+0x58 */
    unsigned char unmodeled_5c[0x14];  /* +0x5c */
    float farDistance;                 /* +0x70 */
};

/*
 * RgCameraSetFarMode (ov12:0x00a124c8) is the only claimed function that
 * reaches past the action-state block; it writes farDistance to 14.0 or 8.0
 * according to its enable argument. The bytes between the action-state
 * block (+0x5c) and this field are not recovered yet.
 */

/* The predicate-gated countdown timer used by the Version3 camera mode. */
typedef struct RgVer3Timer {
    int (*predicate)(int value); /* +0x00: consulted by _PreJobVer3Timer */
    float duration;              /* +0x04: reload value copied into "remaining" */
    float remaining;             /* +0x08: counted down by _PastVer3Timer, tested by _IsVer3TimerActive */
} RgVer3Timer;

/*
 * RgCameraGetTarget reads actionState[2] (+0x48) as a RgGeomPoint pointer
 * (the "me" pointer RgCameraDump logs with "%p"), RgCameraDump also logs
 * actionState[3] (+0x4c) as "enemy", and RgCameraLoadText reads actionState[6]
 * (+0x58) as a per-mode text-load callback. Each of those functions casts the
 * element at its own call site; the array's remaining indices are not
 * evidenced.
 */

/*
 * RgCameraSelectableFar (ov12:0x00a12540) is the only claimed function that
 * reaches this far into RgCamera; the bytes between the action-state block
 * (+0x58) and this field are not recovered yet.
 */
#define RG_CAMERA_SELECTABLE_FAR(camera) (*(int *)((unsigned char *)(camera) + 0x14C))

#endif /* SRC_OV12_RG_CAMERA_H */
