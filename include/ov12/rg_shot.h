#ifndef INCLUDE_OV12_RG_SHOT_H
#define INCLUDE_OV12_RG_SHOT_H

#include "shared.h"

/*
 * Opaque handles this TU only ever passes through by pointer: RgShotEffect
 * is fully defined in the prelude of a different translation unit
 * (src/ov12/rg_shot_effect.c, ov12/tu024); RgChar is fully defined in
 * src/ov12/rg_char.c (ov12/tu002). _ShotCommonHit passes the shot pointer
 * to RgCharFree(RgChar *) with a base-class cast, the same relationship
 * CreateRgBgObj already uses in the other direction for RgBgObj
 * (src/ov12/rg_bgobj.c). RgParticleEffect is fully defined in
 * src/ov12/rg_particle_effect.c; _GrenadeSetBom only forwards the handle
 * RgShotEffectGetPtclEffect returns to RgParticleEffectStopShoot.
 * RgGeomBall is fully defined in src/ov12/rg_geom_ball.c; _GrenadeSetBom only
 * forwards the shot's geometry tray, already RgGeom *, to
 * RgGeomBallSetRadius.
 */
typedef struct RgShotEffect RgShotEffect;

/*
 * Partial view of a shot essence, modelled at the byte offsets
 * _InitShotCommon (the create-method slot at +0x00, the damage at +0x04 and
 * the life span at +0x08 it copies onto the new shot, and the model/particle/
 * texline descriptors at +0x0c/+0x4c/+0x8c/+0xac/+0xb0/+0xc0 it forwards to
 * the shot's effect) and _ShotCommonHitBg (essence->hitEffectFile as the
 * hit-effect particle file name passed to CreateRgHitEffectPos) read.
 * _InitRgShotEssence (ov12:0x00a15c9c) empties the two string slots at
 * +0xd0 and +0xf0 (sb zero), which bounds hitEffectFile to 0x20 bytes; the
 * gap at +0xb4..+0xbf is not evidenced by this allocation.
 */
typedef struct RgShotEssence RgShotEssence;

struct RgHomingShotEssence {
    RgShotEssence common;                /* +0x00..+0xef */
    unsigned char unmodeled_0f0[0x30];   /* +0xf0..+0x11f */
    float speed;                         /* +0x120 */
    float turnRate;                      /* +0x124 */
    float cosThreshold;                  /* +0x128 */
    float delay;                         /* +0x12c */
};

/*
 * Partial view of a shot object, modelled only at the byte offsets the
 * functions in this allocation read or write.  Bytes 0x08-0x1f, 0x34-0x3b
 * belong to fields other functions of this translation unit use (geometry/
 * effect handles, ...) and stay unmodelled here.
 */
typedef struct RgShot RgShot;

struct RgShot {
    void *posOwner;                    /* +0x00: first argument to getPos */
    int (*getPos)(void *posOwner, RgVector position, RgVector aimPoint);
                                        /* +0x04 */
    unsigned char unmodeled_008[0x18];        /* +0x08..+0x1f */
    int hitHistory;                           /* +0x20 */
    void (*release)(RgShot *shot);            /* +0x24 */
    void (*hitRobot)(RgShot *shot, int robot, RgVector position);  /* +0x28 */
    void (*hitBg)(RgShot *shot, int bgObject, RgVector position);  /* +0x2c */
    void (*notifyHit)(RgShot *shot, RgVector position);            /* +0x30 */
    RgGeom *geomTray;                         /* +0x34: freed by
                                                * _ShotCommonDestruct and
                                                * used by RgGeomPassTime/
                                                * RgGeomResetStatus */
    RgShotEffect *effect;                     /* +0x38: disposed by
                                                * _ShotCommonDestruct and
                                                * displayed by
                                                * _ShotCommonDisp */
    RgShotEssence *essence;                   /* +0x3c: stored by
                                                * _InitShotCommon
                                                * (ov12:0x00a15b80) */
    float life;                               /* +0x40 */
    float damage;                             /* +0x44 */
    int hitRobotCount;                        /* +0x48 */
    int soundDriver;                          /* +0x4c */
    int hitSoundId;                           /* +0x50 */
    int bgHitSoundId;                         /* +0x54 */
    int noLifeSoundId;                        /* +0x58 */
    int hitSoundVolume;                       /* +0x5c */
    int bgHitSoundVolume;                     /* +0x60 */
    int noLifeSoundVolume;                    /* +0x64 */
    int hideSoundId;                          /* +0x68 */
};

#endif /* INCLUDE_OV12_RG_SHOT_H */
