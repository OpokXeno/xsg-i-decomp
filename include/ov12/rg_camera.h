#ifndef INCLUDE_OV12_RG_CAMERA_H
#define INCLUDE_OV12_RG_CAMERA_H

#include "shared.h"

#include "ov12/rg_draw.h"

struct RgReadText;

/* The camera version CreateRgCamera builds (the only one that exists). */
#define RG_CAMERA_VERSION3 2

/* Number of conditions (see s_aszVer3TimerName in rg_camera.c) the Version3 camera tracks. */
#define RG_CAMERA_TIMER_COUNT 10
/* Conditions combined into the two "turn" decisions: two per decision. */
#define RG_CAMERA_CONDITION_COUNT 4
#define RG_CAMERA_DECISION_COUNT 2

/* Bits of RgCamera.actionFlags, set by the RgCameraSetAction* functions. */
#define RG_CAMERA_ACTION_LOCKED_ON 0
#define RG_CAMERA_ACTION_ADVANCE 1
#define RG_CAMERA_ACTION_ROLL 2
#define RG_CAMERA_ACTION_DASH 3
#define RG_CAMERA_ACTION_ATTACK 4
#define RG_CAMERA_ACTION_SEE_TARGET 5

/* The robot-game camera, 0x150 bytes (CreateRgCamera). */
typedef struct RgCamera RgCamera;

/* The predicate-gated countdown timer used by the Version3 camera mode. */
typedef struct RgVer3Timer {
    int (*predicate)(RgCamera *camera); /* +0x00: consulted by _PreJobVer3Timer */
    float duration;              /* +0x04: reload value copied into "remaining" */
    float remaining;             /* +0x08: counted down by _PastVer3Timer, tested by _IsVer3TimerActive */
} RgVer3Timer;

struct RgCamera {
    RgDrawStudio *studio;              /* +0x00 */
    unsigned char unmodeled_04[0x0c];  /* +0x04 */
    RgVector eye;                      /* +0x10 */
    RgVector target;                   /* +0x20 */
    RgVector up;                       /* +0x30 */
    float turnRate;                    /* +0x40: smoothed sideways drift of the camera yaw */
    int actionFlags;                   /* +0x44: bit RG_CAMERA_ACTION_*, reset every frame */
    RgGeomPoint *me;                   /* +0x48: the followed robot (RgCameraDump "me") */
    RgGeomPoint *enemy;                /* +0x4c: its opponent ("enemy"), may be null */
    int snapToTarget;                  /* +0x50: nonzero until the first frame has placed the camera */
    void (*setCamera)(RgCamera *camera, RgGeomPoint **points); /* +0x54: per-mode setup */
    void (*loadText)(RgCamera *camera, struct RgReadText *reader); /* +0x58: per-mode text load */
    unsigned char unmodeled_5c[0x04];  /* +0x5c */
    RgVector extent;                   /* +0x60 */
    float farDistance;                 /* +0x70 */
    float lookDown;                    /* +0x74: "look-down" */
    float upperOffset;                 /* +0x78: "upper-offset" */
    float rollCoef;                    /* +0x7c: "roll-coef" */
    unsigned char unmodeled_80[0x0c];  /* +0x80..+0x8b */
    float enemyRange;                  /* +0x8c: enemy distance below which the camera pulls back */
    float lockAngle;                   /* +0x90: view angle limit, degrees in the text, radians after load */
    float pullBack;                    /* +0x94: extra distance at full enemyRange pressure */
    float distance;                    /* +0x98: current smoothed follow distance */
    float distanceSmoothing;           /* +0x9c */
    int useMoveDir;                    /* +0xa0: "use-move-dir" */
    unsigned char unmodeled_a4[0x0c];  /* +0xa4..+0xaf */
    RgVer3Timer timers[RG_CAMERA_TIMER_COUNT]; /* +0xb0..+0x127 */
    unsigned char unmodeled_128[0x08]; /* +0x128..+0x12f */
    int conditionTimer[RG_CAMERA_CONDITION_COUNT]; /* +0x130: timer each condition reads */
    int conditionInvertMask;           /* +0x140: bit i inverts condition i */
    int decisionIsAnd[RG_CAMERA_DECISION_COUNT];   /* +0x144: pair combined with AND (1) or OR (0) */
    int selectableFar;                 /* +0x14c: pad whose SELECT toggles the distance, or -1 */
};

/* Functions and data used by other translation units. */
RgCamera *CreateRgCamera(int cameraVersion, RgDrawStudio *studio);
void DisposeRgCamera(RgCamera *camera);
void RgCameraSetting(RgCamera *camera, RgGeomPoint **points);
void RgCameraLoadText(RgCamera *camera, struct RgReadText *reader);
void RgCameraSetFarMode(RgCamera *camera, int enable);
void RgCameraSelectableFar(RgCamera *camera, int level);
void RgCameraLocal(RgCamera *camera, RgMatrix matrix);
RgDrawStudio *RgCameraGetStudio(RgCamera *camera);
void RgCameraGetTarget(RgCamera *camera, RgVector position);
void RgCameraResetAction(RgCamera *camera);
void RgCameraSetActionLockedOn(RgCamera *camera, int enable);
void RgCameraSetActionAdvance(RgCamera *camera, int enable);
void RgCameraSetActionRoll(RgCamera *camera, int enable);
void RgCameraSetActionDash(RgCamera *camera, int enable);
void RgCameraSetActionAttack(RgCamera *camera, int enable);
void RgCameraSetActionSeeTarget(RgCamera *camera, int enable);
void RgCameraControl(RgCamera *camera, float elapsed);
void RgCameraDump(RgCamera *camera);

#endif /* INCLUDE_OV12_RG_CAMERA_H */
