/*
 * TU-local declarations of ov12/tu023 (src/ov12/rg_shot.c).
 */

#ifndef SRC_OV12_RG_SHOT_H
#define SRC_OV12_RG_SHOT_H

#include "shared.h"

typedef struct BeamLifetimeField {
    float lifetime;
} BeamLifetimeField;

/*
 * Opaque handles this TU only ever passes through by pointer: RgShotEffect
 * is fully defined in the prelude of a different translation unit
 * (src/ov12/rg_shot_effect.c, ov12/tu024); RgChar is fully defined in
 * src/ov12/rg_char.c (ov12/tu002). _ShotCommonHit passes the shot pointer
 * to RgCharFree(RgChar *) with a base-class cast, the same relationship
 * CreateRgBgObj already uses in the other direction for RgBgObj
 * (src/ov12/rg_bgobj.c).
 */
typedef struct RgShotEffect RgShotEffect;
typedef struct RgChar RgChar;

/*
 * Partial view of a shot essence, modelled only at the byte offset this
 * allocation reads: _ShotCommonHitBg passes essence->hitEffectFile as the
 * hit-effect particle file name to CreateRgHitEffectPos.  _InitRgShotEssence
 * (ov12:0x00a15c9c) empties it (sb zero at +0xD0) and the next string slot
 * at +0xF0, which bounds it to 0x20 bytes; the rest of the essence layout
 * (also cleared at +0x0C/+0x4C/+0x8C) is outside this allocation.
 */
typedef struct RgShotEssence RgShotEssence;

struct RgShotEssence {
    unsigned char unmodeled_000[0xD0];  /* +0x00..+0xCF */
    char hitEffectFile[0x20];           /* +0xD0: a string slot;
                                          * _InitRgShotEssence empties it
                                          * and the next one at +0xF0 */
};

/*
 * Partial view of a normal-shot essence, modelled only at the byte offset
 * _CreateRgNormalShot reads beyond the common RgShotEssence prefix (opaque
 * here): the float at +0x120 that it forwards to _InitShotCommon, which
 * uses it (as $f12/$f20) to scale the aim direction returned by
 * _GetShotPosDir into the shot's initial velocity before RgGeomPointSetVel.
 */
typedef struct RgNormalShotEssence RgNormalShotEssence;

struct RgNormalShotEssence {
    unsigned char unmodeled_000[0x120]; /* +0x00..+0x11f: the RgShotEssence
                                          * prefix, opaque here */
    float speed;                        /* +0x120 */
};

/*
 * Partial view of a grenade essence, modelled only at the byte offsets
 * InitRgGrenadeEssence (ov12:0x00a16b48) writes: a create-method slot at
 * +0x00 that it points at _CreateGrenade, and four floats at +0x130..+0x13c
 * that _InitGrenade (ov12:0x00a169c8) later copies into the live grenade
 * shot object (+0x130 and +0x134 verbatim to +0x90 and +0x94, +0x138
 * verbatim to +0x9c, +0x13c through a float-to-int conversion to +0xc8).
 * No other byte is claimed.
 */
typedef struct RgGrenadeEssence RgGrenadeEssence;

struct RgGrenadeEssence {
    void *(*createFunc)(void *essence, void *info);  /* +0x00 */
    unsigned char unmodeled_004[0x12c];               /* +0x04..+0x12f */
    float explosionRadius;                            /* +0x130 */
    float duration;                                   /* +0x134 */
    float proximity;                                  /* +0x138 */
    float repeatCount;                                /* +0x13c */
};

/*
 * Partial view of a shot object, modelled only at the byte offsets the
 * functions in this allocation read or write.  Bytes 0x00-0x23, 0x34-0x3b
 * and 0x40-0x43 belong to fields other functions of this translation unit
 * use (the create callback, geometry/effect handles, ...) and stay
 * unmodelled here.
 */
typedef struct RgShot RgShot;

/*
 * This allocation also models +0x00..+0x07: _GetShotPos2 reads the
 * position-owner pointer at +0x00 and, when it and the position callback
 * at +0x04 are both set, calls that callback with the owner as its first
 * argument and the caller's two output vectors as the rest, returning
 * whatever the callback returns (0 when either field is unset).
 *
 * This allocation also models +0x34..+0x3b: _ShotCommonDestruct frees the
 * geometry tray and disposes the effect object there, and _ShotCommonDisp
 * displays that same effect object.
 */

struct RgShot {
    void *posOwner;                    /* +0x00: first argument to getPos */
    int (*getPos)(void *posOwner, RgVector position, RgVector aimPoint);
                                        /* +0x04 */
    unsigned char unmodeled_008[0x1C];        /* +0x08..+0x23 */
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
    unsigned char unmodeled_040[0x04];        /* +0x40..+0x43 */
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

/*
 * Partial view of a fire projectile beyond the common RgShot prefix
 * (+0x00..+0x6b): _InitFire (ov12:0x00a16f08) clears a status word at +0x70
 * that _FireRelease tests before killing the fire, and the diagnostic
 * "unknown fire status %d" (D_00A533B0) reports an unhandled value of the
 * same word.  _InitFire also copies a robot reference and clears a
 * bookkeeping flag at +0xb4/+0xb8 that _FireHitRobot reads and updates.
 */
typedef struct RgFireShot RgFireShot;

struct RgFireShot {
    unsigned char unmodeled_000[0x70]; /* +0x00..+0x6f: the RgShot prefix */
    int status;                        /* +0x70 */
    unsigned char unmodeled_074[0x40]; /* +0x74..+0xb3 */
    int robot;                         /* +0xb4: nonzero once a target robot
                                         * is recorded */
    int alreadyHitRobot;               /* +0xb8: set once _FireHitRobot has
                                         * run the common hit-robot handling */
};

/*
 * Partial view of a beam beyond the common RgShot prefix (+0x00..+0x6b):
 * _BeamRelease already treats the +0x98 float as a standalone lifetime slot
 * (BeamLifetimeField above); _BeamIsReleased/_BeamHit additionally need the
 * hit flag _InitBeam clears at +0x9c right next to it.
 */
typedef struct RgBeamShot RgBeamShot;

struct RgBeamShot {
    unsigned char unmodeled_000[0x98]; /* +0x00..+0x97: the RgShot prefix */
    float lifetime;                    /* +0x98 */
    int hit;                           /* +0x9c */
};

#endif /* SRC_OV12_RG_SHOT_H */
