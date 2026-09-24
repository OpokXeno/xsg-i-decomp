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
 * (src/ov12/rg_bgobj.c). RgParticleEffect is fully defined in
 * src/ov12/rg_particle_effect.c; _GrenadeSetBom only forwards the handle
 * RgShotEffectGetPtclEffect returns to RgParticleEffectStopShoot.
 * RgGeomBall is fully defined in src/ov12/rg_geom_ball.c; _GrenadeSetBom only
 * forwards the shot's geometry tray, already RgGeom *, to
 * RgGeomBallSetRadius.
 */
typedef struct RgShotEffect RgShotEffect;
typedef struct RgChar RgChar;
typedef struct RgParticleEffect RgParticleEffect;
typedef struct RgGeomBall RgGeomBall;

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

struct RgShotEssence {
    void *(*createFunc)(void *essence, void *info);  /* +0x00 */
    float damage;                        /* +0x04: copied onto the shot by
                                           * _InitShotCommon */
    float life;                          /* +0x08: copied onto the shot by
                                           * _InitShotCommon */
    char modelVariant[0x40];             /* +0x0c: RgShotEffectSetModel's
                                           * variant name */
    char particleFile[0x40];             /* +0x4c: RgShotEffectSetParticle's
                                           * file name */
    char texLineFile[0x20];              /* +0x8c: RgShotEffectSetTexLine's
                                           * file name */
    float texLineWidth;                  /* +0xac */
    float texLineHeight;                 /* +0xb0 */
    unsigned char unmodeled_0b4[0x0C];   /* +0xb4..+0xbf */
    unsigned char texLineColor[0x10];    /* +0xc0: quadword copied by
                                           * RgShotEffectSetTexLineModelColor */
    char hitEffectFile[0x20];            /* +0xd0: a string slot;
                                           * _InitRgShotEssence empties it
                                           * and the next one at +0xF0 */
};

/*
 * Partial view of a normal-shot essence, modelled only at the byte offset
 * _CreateRgNormalShot reads beyond the common RgShotEssence prefix: the
 * float at +0x120 that it forwards to _InitShotCommon, which uses it (as
 * $f12/$f20) to scale the aim direction returned by _GetShotPosDir into the
 * shot's initial velocity before RgGeomPointSetVel.
 *
 * InitRgNormalShotEssence (ov12:0x00a15d28) also writes the create-method
 * slot at +0x00, the same slot the common RgShotEssence prefix names
 * createFunc; the gap at +0x04..+0x11f beyond it is not evidenced by this
 * allocation.
 */
typedef struct RgNormalShotEssence RgNormalShotEssence;

struct RgNormalShotEssence {
    RgShotEssence common;                /* +0x00..+0xef */
    unsigned char unmodeled_0f0[0x30];   /* +0xf0..+0x11f */
    float speed;                        /* +0x120 */
};

/*
 * Partial view of a homing-shot essence beyond the common RgShotEssence
 * prefix (+0x00..+0xef): InitRgHomingShotEssence writes the speed, turn
 * rate, cosine threshold and delay _InitHoming copies onto the new shot;
 * the gap at +0xf0..+0x11f is not evidenced by this allocation.
 */
typedef struct RgHomingShotEssence RgHomingShotEssence;

struct RgHomingShotEssence {
    RgShotEssence common;                /* +0x00..+0xef */
    unsigned char unmodeled_0f0[0x30];   /* +0xf0..+0x11f */
    float speed;                         /* +0x120 */
    float turnRate;                      /* +0x124 */
    float cosThreshold;                  /* +0x128 */
    float delay;                         /* +0x12c */
};

/*
 * Partial view of a grenade essence, modelled at the byte offsets
 * InitRgGrenadeEssence (ov12:0x00a16b48) writes: a create-method slot at
 * +0x00 that it points at _CreateGrenade, and four floats at +0x130..+0x13c
 * that _InitGrenade (ov12:0x00a169c8) later copies into the live grenade
 * shot object (+0x130 and +0x134 verbatim to +0x90 and +0x94, +0x138
 * verbatim to +0x9c, +0x13c through a float-to-int conversion to +0xc8).
 * No other byte beyond the common RgHomingShotEssence prefix is claimed.
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
 * Partial view of a fire essence beyond the common RgShotEssence prefix
 * (+0x00..+0xef): InitRgFireEssence writes the speed and duration
 * _CreateFire's sibling _InitFire (still assembler) presumably copies onto
 * the new fire, and clears a trailing word at +0x128 whose only evidenced
 * behaviour here is being zeroed; the gap at +0xf0..+0x11f is not
 * evidenced by this allocation.
 */
typedef struct RgFireEssence RgFireEssence;

struct RgFireEssence {
    RgShotEssence common;                /* +0x00..+0xef */
    unsigned char unmodeled_0f0[0x30];   /* +0xf0..+0x11f */
    float speed;                         /* +0x120 */
    float duration;                      /* +0x124 */
    int trailingCount;                   /* +0x128: cleared by
                                           * InitRgFireEssence; no reader is
                                           * evidenced by this allocation */
};

/*
 * Partial view of a beam essence beyond the common RgShotEssence prefix
 * (+0x00..+0xef): InitRgBeamEssence writes the speed and duration
 * _CreateBeam's sibling _InitBeam (still assembler) presumably copies onto
 * the new beam; the gap at +0xf0..+0x11f is not evidenced by this
 * allocation.
 */
typedef struct RgBeamEssence RgBeamEssence;

struct RgBeamEssence {
    RgShotEssence common;                /* +0x00..+0xef */
    unsigned char unmodeled_0f0[0x30];   /* +0xf0..+0x11f */
    float speed;                         /* +0x120 */
    float duration;                      /* +0x124 */
};

/*
 * Partial view of the position/creation request _InitShotCommon and
 * _InitHoming receive as their extInfo/info argument.  The first two words
 * alias RgShot's own posOwner/getPos pair: _InitShotCommon forwards the
 * whole pointer to _GetShotPosDir, which only ever reads those two fields.
 * _InitShotCommonSub records the id at +0x0c, and _InitHoming copies the
 * target geometry at +0x08 and the targeting-state flag at +0x10 onto the
 * new homing shot.
 */
typedef struct RgShotRequest RgShotRequest;

struct RgShotRequest {
    unsigned char unmodeled_00[0x08]; /* +0x00..+0x07: posOwner/getPos,
                                        * aliased with RgShot for
                                        * _GetShotPosDir */
    RgGeom *target;                   /* +0x08 */
    int id;                           /* +0x0c */
    int targetConfused;               /* +0x10 */
};

/*
 * Partial view of a shot object, modelled only at the byte offsets the
 * functions in this allocation read or write.  Bytes 0x08-0x1f, 0x34-0x3b
 * belong to fields other functions of this translation unit use (geometry/
 * effect handles, ...) and stay unmodelled here.
 */
typedef struct RgShot RgShot;

/*
 * This allocation also models +0x00..+0x07: _GetShotPos2 reads the
 * position-owner pointer at +0x00 and, when it and the position callback
 * at +0x04 are both set, calls that callback with the owner as its first
 * argument and the caller's two output vectors as the rest, returning
 * whatever the callback returns (0 when either field is unset).
 *
 * This allocation also models +0x20: _GrenadeSetBom clears this word when
 * the grenade switches to bomb behaviour; no function of this allocation
 * reads it back, and no other claimed function of this TU writes it.
 *
 * This allocation also models +0x34..+0x3b: _ShotCommonDestruct frees the
 * geometry tray and disposes the effect object there, and _ShotCommonDisp
 * displays that same effect object.
 *
 * This allocation also models +0x40: _InitShotCommon copies the essence's
 * life span here, and _GrenadeSetBom sets it to a effectively-infinite
 * value once the grenade has switched to bomb behaviour.
 */

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

/*
 * Partial view of a homing shot beyond the common RgShot prefix
 * (+0x00..+0x6b): _InitHoming (ov12:0x00a15f60) copies the turn rate, cosine
 * threshold, speed and delay from the essence, and the target geometry and
 * random-targeting flag from the creation request; _HomingPassTime reads all
 * six once the delay has expired. The gap at +0x6c..+0x6f is not evidenced
 * by this allocation.
 */
typedef struct RgHomingShot RgHomingShot;

struct RgHomingShot {
    RgShot common;                     /* +0x00..+0x6b */
    unsigned char unmodeled_06c[4];    /* +0x6c..+0x6f */
    float turnRate;                    /* +0x70 */
    float cosThreshold;                /* +0x74 */
    float speed;                       /* +0x78 */
    float delay;                       /* +0x7c: counts down toward zero */
    RgGeom *target;                    /* +0x80 */
    int randomTargeting;               /* +0x84 */
};

/*
 * Partial view of a grenade shot beyond the common RgHomingShot prefix
 * (+0x00..+0x87): _GrenadeSetBom and _GrenadeHomingPassTime read the
 * explosion radius, duration and proximity _InitGrenade (still assembler)
 * copies verbatim from the essence, and the repeat count it copies through
 * a float-to-int conversion. The gaps at +0x88..+0x8f, +0x98..+0x9b and
 * +0xa0..+0xc7 are not evidenced by this allocation.
 */
typedef struct RgGrenadeShot RgGrenadeShot;

struct RgGrenadeShot {
    RgHomingShot common;                /* +0x00..+0x87 */
    unsigned char unmodeled_088[8];     /* +0x88..+0x8f */
    float explosionRadius;              /* +0x90 */
    float duration;                     /* +0x94 */
    unsigned char unmodeled_098[4];     /* +0x98..+0x9b */
    float proximity;                    /* +0x9c */
    unsigned char unmodeled_0a0[0x28];  /* +0xa0..+0xc7 */
    unsigned int repeatCount;           /* +0xc8 */
};

/*
 * Partial view of a fire projectile beyond the common RgShot prefix
 * (+0x00..+0x6b): _InitFire (ov12:0x00a16f08) clears a status word at +0x70
 * that _FireRelease tests before killing the fire, and the diagnostic
 * "unknown fire status %d" (D_00A533B0) reports an unhandled value of the
 * same word. _FireKill reads a flag at +0xb0 to pick the fade time it
 * stores at +0xac before stopping the effect. _InitFire also copies a
 * robot reference and clears a bookkeeping flag at +0xb4/+0xb8 that
 * _FireHitRobot reads and updates. The gap at +0x74..+0xab is not
 * evidenced by this allocation.
 */
typedef struct RgFireShot RgFireShot;

struct RgFireShot {
    unsigned char unmodeled_000[0x70]; /* +0x00..+0x6f: the RgShot prefix */
    int status;                        /* +0x70 */
    unsigned char unmodeled_074[0x38]; /* +0x74..+0xab */
    float fadeTime;                    /* +0xac: the time _FireKill passes
                                         * to RgShotEffectStopAlive */
    int quickFade;                     /* +0xb0: nonzero selects a fade
                                         * time of 0.0f instead of 1.0f */
    int robot;                         /* +0xb4: nonzero once a target robot
                                         * is recorded */
    int alreadyHitRobot;               /* +0xb8: set once _FireHitRobot has
                                         * run the common hit-robot handling */
};

/*
 * Partial view of a beam beyond the common RgShot prefix (+0x00..+0x6b):
 * _BeamPassTime evidences the origin/direction pair _GetShotPosDir fills in
 * once released, the per-tick speed and accumulated length that gate its
 * movement and its collapse to zero length, and the per-tick damage rate it
 * applies while attached. _BeamRelease already treats the +0x98 float as a
 * standalone lifetime slot (BeamLifetimeField above); _BeamIsReleased/
 * _BeamHit additionally need the hit flag _InitBeam clears at +0x9c right
 * next to it. The embedded request at +0xb0 is the pointer/callback pair
 * _GetShotPosDir reads to recompute the origin/direction pair while the
 * beam's source is still attached; the gap at +0xa4..+0xaf is not evidenced
 * by this allocation.
 */
typedef struct RgBeamShot RgBeamShot;

struct RgBeamShot {
    RgShot common;                     /* +0x00..+0x6b */
    unsigned char unmodeled_06c[4];    /* +0x6c..+0x6f */
    RgVector origin;                   /* +0x70 */
    RgVector direction;                /* +0x80 */
    float speed;                       /* +0x90 */
    float length;                      /* +0x94: collapses the beam to
                                         * nothing at zero */
    float lifetime;                    /* +0x98 */
    int hit;                           /* +0x9c */
    float damageRate;                  /* +0xa0 */
    unsigned char unmodeled_0a4[0x0C]; /* +0xa4..+0xaf */
    RgShotRequest request;             /* +0xb0 */
};

#endif /* SRC_OV12_RG_SHOT_H */
