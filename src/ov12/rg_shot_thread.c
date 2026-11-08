#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_equip_type.h"
#include "ov12/rg_shot_thread.h"
#include "ov12/rg_error.h"
#include "ov12/rg_weapon.h"

const char shotThreadAssertThreadNotNil[] = "pThread != NIL";
const char shotThreadSourcePath[] = "../rg_shot_thread.euc.c";
const char shotThreadUnknownStatusFormat[] = "unknown shot thread status %d";



/*
 * Attaches weapon to the thread and creates its shot motion controller, unless the
 * arm type is the back arm. Without a weapon or parent actor the thread is disabled.
 */
static void _SetShotThread(RgShotThread *thread, RgWeapon *weapon,
                           int armType, XrgActor *parentActor)
{
    RgShotMotCont *motCont;

    if (thread == 0) {
        assert_prog(shotThreadAssertThreadNotNil, shotThreadSourcePath, 0x32);
    }
    motCont = thread->motCont;
    if (motCont != 0) {
        DisposeRgShotMotCont(motCont);
        thread->motCont = 0;
    }
    if (weapon != 0 && parentActor != 0 && RgWeaponGetActor(weapon) != 0) {
        thread->weapon = weapon;
        thread->status = RG_SHOT_THREAD_IDLE;
        if (armType != RG_EQUIP_TYPE_BACK &&
            !(RgWeaponGetControlFlag(weapon) & 0x20)) {
            thread->motCont = CreateRgShotMotCont(parentActor,
                                                  RgWeaponGetActor(weapon),
                                                  armType);
        }
    } else {
        motCont = thread->motCont;
        thread->status = RG_SHOT_THREAD_DISABLED;
        thread->weapon = 0;
        if (motCont != 0) {
            DisposeRgShotMotCont(motCont);
            thread->motCont = 0;
        }
    }
    thread->elapsedTime = 0.0f;
    thread->motionId = RG_SHOT_THREAD_NO_MOTION;
}

/* Clears a shot thread to the disabled state with no weapon or motion. */
static void _InitShotThread(RgShotThread *thread)
{
    if (thread == 0) {
        assert_prog(shotThreadAssertThreadNotNil, shotThreadSourcePath, 0x59);
    }
    thread->weapon = 0;
    thread->motCont = 0;
    thread->motionId = RG_SHOT_THREAD_NO_MOTION;
    thread->status = RG_SHOT_THREAD_DISABLED;
    thread->elapsedTime = 0.0f;
    thread->active = 0;
}

/* Allocates a shot thread and clears it. */
RgShotThread *CreateRgShotThread(void)
{
    RgShotThread *thread;

    thread = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgShotThread), shotThreadSourcePath, 0x69);
    _InitShotThread(thread);
    return thread;
}

/* Detaches the weapon and frees the thread. */
void DisposeRgShotThread(RgShotThread *thread)
{
    if (thread == 0) {
        assert_prog(shotThreadAssertThreadNotNil, shotThreadSourcePath, 0x71);
    }
    _SetShotThread(thread, 0, -1, 0);
    RgHeapFree(InstanceOfRgHeap(), thread, shotThreadSourcePath, 0x73);
}

int RgShotThreadGetStatus(RgShotThread *thread)
{
    if (thread == 0) {
        assert_prog(shotThreadAssertThreadNotNil, shotThreadSourcePath, 0x7D);
    }
    return thread->status;
}

/* Attaches weapon to the thread for the given arm type and parent actor. */
void RgShotThreadInit(RgShotThread *thread, RgWeapon *weapon, int armType,
                      XrgActor *parentActor)
{
    _SetShotThread(thread, weapon, armType, parentActor);
}

/* Stops the motion and any attached shot and returns an enabled thread to idle. */
void RgShotThreadSleep(RgShotThread *thread)
{
    if (thread == 0) {
        assert_prog(shotThreadAssertThreadNotNil, shotThreadSourcePath, 0x90);
    }
    if (thread->status != RG_SHOT_THREAD_DISABLED) {
        thread->status = RG_SHOT_THREAD_IDLE;
        thread->motionId = RG_SHOT_THREAD_NO_MOTION;
        if (thread->motCont != 0) {
            RgShotMotContStop(thread->motCont);
        }
        if (RgWeaponIsAttachedShot(thread->weapon) != 0) {
            RgWeaponShotStop(thread->weapon);
        }
        RgWeaponSetdown(thread->weapon);
    }
}

void RgShotThreadMotOff(RgShotThread *thread)
{
    if (thread == 0) {
        assert_prog(shotThreadAssertThreadNotNil, shotThreadSourcePath, 0xA7);
    }
    if (thread->motCont != 0) {
        DisposeRgShotMotCont(thread->motCont);
        thread->motCont = 0;
    }
}

/*
 * Starts the motion described by info on an enabled thread and marks it active.
 * The motion restarts only when its id changes or the thread is idle.
 */
void RgShotThreadStart(RgShotThread *thread, RgShotMotStartInfo *info)
{
    int motion;

    if (thread == 0) {
        assert_prog(shotThreadAssertThreadNotNil, shotThreadSourcePath, 0xB6);
    }
    if (thread->status != RG_SHOT_THREAD_DISABLED) {
        if (thread->status == RG_SHOT_THREAD_IDLE ||
            thread->motionId != info->motion) {
            thread->motionId = info->motion;
            thread->frame = info->frame;
            motion = info->motion;
            thread->status = RG_SHOT_THREAD_SETUP;
            thread->duration = info->duration;
            thread->elapsedTime = 0.0f;
            if (motion != RG_SHOT_THREAD_NO_MOTION && thread->motCont != 0) {
                RgShotMotContPlay(thread->motCont, motion);
            }
        }
        thread->active = 1;
    }
}

/* Advances the thread's state machine (setup, shoot, stop) and its motion controller by deltaTime. */
void RgShotThreadPassTime(RgShotThread *thread, float deltaTime)
{
    int status;
    int hasAttachedShot;

    if (thread == 0) {
        assert_prog(shotThreadAssertThreadNotNil, shotThreadSourcePath, 218);
    }
    if (thread->status == RG_SHOT_THREAD_DISABLED) {
        return;
    }

    thread->elapsedTime += deltaTime;
    if (RgWeaponGetShotNum(thread->weapon) <= 0.0f) {
        thread->active = 0;
    }

    /* Runs the state machine until a pass leaves the state unchanged. */
    status = thread->status;
    do {
        thread->status = status;
        switch ((unsigned int)status) {
        case RG_SHOT_THREAD_IDLE:
            break;
        case RG_SHOT_THREAD_SETUP:
            RgWeaponSetup(thread->weapon);
            if (thread->elapsedTime >= thread->frame) {
                thread->shotCount = 0;
                status = RG_SHOT_THREAD_SHOOT;
            }
            break;
        case RG_SHOT_THREAD_SHOOT:
            RgWeaponShot(thread->weapon);
            thread->shotCount++;
            hasAttachedShot = RgWeaponIsAttachedShot(thread->weapon);
            if (thread->active == 0 &&
                hasAttachedShot == 0 &&
                thread->shotCount >= 2U) {
                status = RG_SHOT_THREAD_STOP;
                if (thread->motCont != 0) {
                    RgShotMotContStop(thread->motCont);
                }
            } else if (thread->elapsedTime >= thread->duration) {
                thread->elapsedTime = thread->frame;
                if (thread->motionId != RG_SHOT_THREAD_NO_MOTION &&
                    thread->motCont != 0) {
                    RgShotMotContSetFrame(thread->motCont, thread->frame);
                }
            }
            break;
        case RG_SHOT_THREAD_STOP:
            RgWeaponShotStop(thread->weapon);
            status = RG_SHOT_THREAD_IDLE;
            RgWeaponSetdown(thread->weapon);
            break;
        default:
            RgError(shotThreadUnknownStatusFormat, shotThreadSourcePath, 296, status);
            break;
        }
    } while (status != thread->status);

    /* Advances the motion controller once the state machine has settled. */
    if (thread->motionId != RG_SHOT_THREAD_NO_MOTION && thread->motCont != 0) {
        RgShotMotContPassTime(thread->motCont, deltaTime);
    }
    thread->active = 0;
}
