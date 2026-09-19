/*
 * TU-local declarations of ov12/tu011 (src/ov12/rg_shot_thread.c).
 *
 * RgShotThread is the per-weapon shot-attack state CreateRgShotThread
 * allocates (RgHeapAlloc size 0x24, ov12:0x00a0ee68..0x00a0ee78) and
 * _InitShotThread (ov12:0x00a0edf0) resets. This allocation only ever
 * reaches the type through a pointer, so no total object size is claimed;
 * field offsets below are attested by this TU's own loads/stores:
 *   weapon 0x00      - RgShotThreadSleep's callees RgWeaponIsAttachedShot/
 *                       RgWeaponShotStop/RgWeaponSetdown all take this value
 *                       unchanged (ov12:0x00a0efd0..0x00a0eff8).
 *   motCont 0x04      - DisposeRgShotMotCont's argument in RgShotThreadMotOff
 *                       (ov12:0x00a0f048), RgShotMotContStop's argument in
 *                       RgShotThreadSleep (ov12:0x00a0efc8), RgShotMotContPlay's
 *                       argument in RgShotThreadStart (ov12:0x00a0f100).
 *   motionId 0x08     - compared and replaced by RgShotThreadStart
 *                       (ov12:0x00a0f0b4/0x00a0f0d0), reset to -1 by
 *                       _InitShotThread and RgShotThreadSleep.
 *   status 0x0c       - RgShotThreadGetStatus's return value; 5 disables the
 *                       thread (RgShotThreadSleep/RgShotThreadStart both
 *                       no-op when it holds 5), 0 is the sleeping state
 *                       RgShotThreadSleep restores, 1 is the playing state
 *                       RgShotThreadStart sets.
 *   elapsedTime 0x10  - cleared by _InitShotThread and by RgShotThreadStart
 *                       when it begins a new motion.
 *   active 0x14       - cleared by _InitShotThread, set by RgShotThreadStart
 *                       whenever the thread is not disabled.
 *   frame 0x18, duration 0x1c - copied from the RgShotThreadStart parameter
 *                       block (ov12:0x00a0f0c4/0x00a0f0e0); RgShotThreadPassTime
 *                       (ov12:0x00a0f128, not part of this allocation) compares
 *                       elapsedTime against duration and then feeds frame to
 *                       RgShotMotContSetFrame, which is this allocation's only
 *                       evidence for those two members' role.
 *
 * RgWeapon (defined by src/ov12/rg_weapon.c) and RgShotMotCont (its own
 * defining TU is outside this allocation) are only ever reached by pointer
 * from this allocation, so they stay opaque forward declarations here, the
 * same way src/ov12/rg_weapon.h forward-declares XrgActor for a type its
 * own TU does not define either.
 */

#ifndef SRC_OV12_RG_SHOT_THREAD_H
#define SRC_OV12_RG_SHOT_THREAD_H

typedef struct RgWeapon RgWeapon;
typedef struct RgShotMotCont RgShotMotCont;

typedef struct RgShotThread {
    RgWeapon *weapon;
    RgShotMotCont *motCont;
    int motionId;
    int status;
    float elapsedTime;
    int active;
    float frame;
    float duration;
} RgShotThread;

#endif /* SRC_OV12_RG_SHOT_THREAD_H */
