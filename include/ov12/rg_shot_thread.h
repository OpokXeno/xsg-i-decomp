#ifndef INCLUDE_OV12_RG_SHOT_THREAD_H
#define INCLUDE_OV12_RG_SHOT_THREAD_H

#include "ov12/rg_weapon.h"
#include "ov12/rg_shotmot_control.h"

/* RgShotThread.status values. */
#define RG_SHOT_THREAD_IDLE 0
#define RG_SHOT_THREAD_SETUP 1
#define RG_SHOT_THREAD_SHOOT 2
#define RG_SHOT_THREAD_STOP 4
#define RG_SHOT_THREAD_DISABLED 5

/* RgShotThread.motionId when no shot motion is playing. */
#define RG_SHOT_THREAD_NO_MOTION (-1)

/*
 * The per-weapon shot-attack state machine CreateRgShotThread allocates
 * (0x24 bytes): it sets the weapon up, fires it once the setup frame has
 * passed and keeps the shot motion controller in step.
 */
typedef struct RgShotThread {
    RgWeapon *weapon;
    RgShotMotCont *motCont;
    int motionId;
    int status;           /* RG_SHOT_THREAD_* */
    float elapsedTime;
    int active;
    float frame;          /* the time at which the weapon fires */
    float duration;       /* the time at which the motion loops back to frame */
    unsigned int shotCount;
} RgShotThread;

/*
 * The parameter block RgShotThreadStart receives: the motion id to play plus
 * the frame/duration values copied into the thread.
 */
typedef struct RgShotMotStartInfo {
    int motion;
    float frame;
    float duration;
} RgShotMotStartInfo;

/* Functions and data used by other translation units. */
RgShotThread *CreateRgShotThread(void);
void DisposeRgShotThread(RgShotThread *thread);
int RgShotThreadGetStatus(RgShotThread *thread);
void RgShotThreadInit(RgShotThread *thread, RgWeapon *weapon, int armType,
                      struct XrgActor *parentActor);
void RgShotThreadSleep(RgShotThread *thread);
void RgShotThreadMotOff(RgShotThread *thread);
void RgShotThreadStart(RgShotThread *thread, RgShotMotStartInfo *info);
void RgShotThreadPassTime(RgShotThread *thread, float deltaTime);

#endif /* INCLUDE_OV12_RG_SHOT_THREAD_H */
