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

/* The predicate-gated countdown timer used by the Version3 camera mode. */
typedef struct RgVer3Timer {
    int (*predicate)(int value); /* +0x00: consulted by _PreJobVer3Timer */
    float duration;              /* +0x04: reload value copied into "remaining" */
    float remaining;             /* +0x08: counted down by _PastVer3Timer, tested by _IsVer3TimerActive */
} RgVer3Timer;

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
    unsigned char unmodeled_5c[0x04];  /* +0x5c */
    RgVector extent;                   /* +0x60 */
    float farDistance;                 /* +0x70 */
    float cam74;                       /* +0x74 */
    float cam78;                       /* +0x78 */
    float cam7c;                       /* +0x7c */
    unsigned char unmodeled_80[0x0c];  /* +0x80..+0x8b */
    float cam8c;                       /* +0x8c */
    float cam90;                       /* +0x90 */
    float cam94;                       /* +0x94 */
    int cam98;                         /* +0x98 */
    float cam9c;                       /* +0x9c */
    int cam_a0;                        /* +0xa0 */
    unsigned char unmodeled_a4[0x0c];  /* +0xa4..+0xaf */
    RgVer3Timer timers[10];            /* +0xb0..+0x127 */
    unsigned char unmodeled_128[0x08]; /* +0x128..+0x12f */
    int cam130;                        /* +0x130 */
    int cam134;                        /* +0x134 */
    int cam138;                        /* +0x138 */
    int cam13c;                        /* +0x13c */
    int cam140;                        /* +0x140 */
    int cam144;                        /* +0x144 */
    int cam148;                        /* +0x148 */
};

/*
 * RgCameraSetFarMode (ov12:0x00a124c8) is the only claimed function that
 * reaches past the action-state block; it writes farDistance to 14.0 or 8.0
 * according to its enable argument. The bytes between the action-state
 * block (+0x5c) and this field are not recovered yet.
 */

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

/*
 * Version3 extent defaults written by _SetVersion3Extent (ov12:0x00a11428).
 * Names come from the parameter keys _LoadVersion3 maps to these offsets
 * ("look-down", "upper-offset", "roll-coef", "use-move-dir"); the other offsets
 * are the named-offset fallback of docs/style.md rule 2 until their roles are
 * recovered.
 */
#define RG_CAMERA_VEC_60(camera) ((float *)((unsigned char *)(camera) + 0x60))
#define RG_CAMERA_LOOK_DOWN(camera) (*(float *)((unsigned char *)(camera) + 0x74))
#define RG_CAMERA_UPPER_OFFSET(camera) (*(float *)((unsigned char *)(camera) + 0x78))
#define RG_CAMERA_ROLL_COEF(camera) (*(float *)((unsigned char *)(camera) + 0x7C))
#define RG_CAMERA_FLOAT_8C(camera) (*(float *)((unsigned char *)(camera) + 0x8C))
#define RG_CAMERA_FLOAT_90(camera) (*(float *)((unsigned char *)(camera) + 0x90))
#define RG_CAMERA_FLOAT_94(camera) (*(float *)((unsigned char *)(camera) + 0x94))
#define RG_CAMERA_INT_98(camera) (*(int *)((unsigned char *)(camera) + 0x98))
#define RG_CAMERA_FLOAT_9C(camera) (*(float *)((unsigned char *)(camera) + 0x9C))
#define RG_CAMERA_USE_MOVE_DIR(camera) (*(int *)((unsigned char *)(camera) + 0xA0))
#define RG_CAMERA_VER3_TIMERS(camera) ((RgVer3Timer *)((unsigned char *)(camera) + 0xB0))
#define RG_CAMERA_INT_130(camera) (*(int *)((unsigned char *)(camera) + 0x130))
#define RG_CAMERA_INT_134(camera) (*(int *)((unsigned char *)(camera) + 0x134))
#define RG_CAMERA_INT_138(camera) (*(int *)((unsigned char *)(camera) + 0x138))
#define RG_CAMERA_INT_13C(camera) (*(int *)((unsigned char *)(camera) + 0x13C))
#define RG_CAMERA_INT_140(camera) (*(int *)((unsigned char *)(camera) + 0x140))
#define RG_CAMERA_INT_144(camera) (*(int *)((unsigned char *)(camera) + 0x144))
#define RG_CAMERA_INT_148(camera) (*(int *)((unsigned char *)(camera) + 0x148))
#define RG_CAMERA_TIMER_IDS(camera) ((int *)((unsigned char *)(camera) + 0x130))
#define RG_CAMERA_FLAGS_144(camera) ((int *)((unsigned char *)(camera) + 0x144))
#define RG_CAMERA_FLOAT_40(camera) (*(float *)((unsigned char *)(camera) + 0x40))
#define RG_CAMERA_INT_50(camera) (*(int *)((unsigned char *)(camera) + 0x50))
#define RG_CAMERA_FLOAT_98(camera) (*(float *)((unsigned char *)(camera) + 0x98))

#endif /* SRC_OV12_RG_CAMERA_H */
