#ifndef INCLUDE_OV12_RG_SHOT_H
#define INCLUDE_OV12_RG_SHOT_H

#include "shared.h"
#include "ov12/rg_char.h"
#include "ov12/rg_geom_ball.h"
#include "ov12/rg_shot_effect.h"
#include "ov12/xrg_sound.h"

/* The shot database allocates every essence but the grenade's at this size. */
#define RG_SHOT_ESSENCE_SIZE 0x130
#define RG_GRENADE_ESSENCE_SIZE 0x140

/* Sound volume a shot starts with, and the volume of a full-scale sound. */
#define RG_SHOT_SOUND_VOLUME_DEFAULT 128

/* RgShot.life of a shot that never expires. */
#define RG_SHOT_LIFE_INFINITE 1e8f

typedef struct RgShot RgShot;
typedef struct RgShotEssence RgShotEssence;
typedef struct RgShotRequest RgShotRequest;
struct RgGeomGroup;
struct RgBgObj;

/*
 * The creation request a weapon hands to an essence's create method. The
 * shot reads the muzzle position and aim point through getPos.
 */
struct RgShotRequest {
    void *posOwner;                   /* +0x00: first argument to getPos */
    int (*getPos)(void *posOwner, RgVector position, RgVector aimPoint);
                                      /* +0x04 */
    RgGeomPoint *target;              /* +0x08: the geometry a homing shot chases */
    struct RgGeomGroup *shotGroup;    /* +0x0c: the group the shot's ball joins */
    int targetConfused;               /* +0x10 */
};

/*
 * The shot parameters the shot database reads from a text file. Every
 * essence type starts with this record, and its create method makes the
 * matching shot.
 */
struct RgShotEssence {
    RgShot *(*createFunc)(RgShotEssence *essence, RgShotRequest *info);
                                         /* +0x00 */
    float damage;                        /* +0x04: copied onto the shot */
    float life;                          /* +0x08: copied onto the shot */
    char modelVariant[0x40];             /* +0x0c */
    char particleFile[0x40];             /* +0x4c */
    char texLineFile[0x20];              /* +0x8c */
    float texLineWidth;                  /* +0xac */
    float texLineHeight;                 /* +0xb0 */
    unsigned char unmodeled_0b4[0x0C];   /* +0xb4 */
    XrgPaint2DRGBA texLineColor;         /* +0xc0 */
    char hitEffectFile[0x20];            /* +0xd0 */
    char damageParticleFile[0x20];       /* +0xf0 */
    float notDamLimit;                   /* +0x110 */
    float notDamTime;                    /* +0x114 */
    float slowTime;                      /* +0x118 */
    float slowRate;                      /* +0x11c */
};

typedef struct RgNormalShotEssence {
    RgShotEssence common;                /* +0x00 */
    float speed;                         /* +0x120 */
} RgNormalShotEssence;

typedef struct RgHomingShotEssence {
    RgShotEssence common;                /* +0x00 */
    float speed;                         /* +0x120: "spd" */
    float turnRate;                      /* +0x124: "aim" */
    float cosThreshold;                  /* +0x128: "accuracy" */
    float delay;                         /* +0x12c: "wait" */
} RgHomingShotEssence;

typedef struct RgGrenadeEssence {
    RgHomingShotEssence homing;          /* +0x00 */
    float explosionRadius;               /* +0x130: "bom-size" */
    float duration;                      /* +0x134: "bom-time" */
    float proximity;                     /* +0x138: "bom-dist" */
    float confuseTime;                   /* +0x13c: "chaff" */
} RgGrenadeEssence;

typedef struct RgFireEssence {
    RgShotEssence common;                /* +0x00 */
    float length;                        /* +0x120: "length" */
    float attachTime;                    /* +0x124: "attach-time" */
    int immDead;                         /* +0x128: "imm-dead" */
    int onceHit;                         /* +0x12c: "once-hit" */
} RgFireEssence;

typedef struct RgBeamEssence {
    RgShotEssence common;                /* +0x00 */
    float speed;                         /* +0x120: "spd" */
    float attachTime;                    /* +0x124: "attach-time" */
} RgBeamEssence;

/* The shot every kind of projectile starts with. */
struct RgShot {
    RgChar base;                              /* +0x00 */
    int (*isReleased)(RgShot *shot);          /* +0x1c */
    void (*expire)(RgShot *shot);             /* +0x20: called when life runs out */
    void (*release)(RgShot *shot);            /* +0x24 */
    void (*hitRobot)(RgShot *shot, RgStatus *robot, RgVector position);
                                              /* +0x28 */
    void (*hitBg)(RgShot *shot, struct RgBgObj *bgObject, RgVector position);
                                              /* +0x2c */
    void (*notifyHit)(RgShot *shot, RgVector position);
                                              /* +0x30 */
    RgGeomBall *geomTray;                     /* +0x34 */
    RgShotEffect *effect;                     /* +0x38 */
    RgShotEssence *essence;                   /* +0x3c */
    float life;                               /* +0x40 */
    float damage;                             /* +0x44 */
    int hitRobotCount;                        /* +0x48 */
    XrgSound *soundDriver;                    /* +0x4c */
    int hitSoundId;                           /* +0x50 */
    int bgHitSoundId;                         /* +0x54 */
    int noLifeSoundId;                        /* +0x58 */
    int hitSoundVolume;                       /* +0x5c */
    int bgHitSoundVolume;                     /* +0x60 */
    int noLifeSoundVolume;                    /* +0x64 */
    int hideSoundId;                          /* +0x68 */
};

typedef struct RgHomingShot {
    RgShot common;                     /* +0x00 */
    unsigned char unmodeled_06c[4];    /* +0x6c */
    float turnRate;                    /* +0x70 */
    float cosThreshold;                /* +0x74 */
    float speed;                       /* +0x78 */
    float delay;                       /* +0x7c: counts down toward zero */
    RgGeomPoint *target;               /* +0x80 */
    int randomTargeting;               /* +0x84 */
} RgHomingShot;

#define RG_GRENADE_HISTORY_MAX 8

typedef struct RgGrenadeShot {
    RgHomingShot common;                /* +0x00 */
    unsigned char unmodeled_088[8];     /* +0x88 */
    float explosionRadius;              /* +0x90 */
    float duration;                     /* +0x94 */
    float elapsedTime;                  /* +0x98: the bomb is released once it exceeds duration */
    float proximity;                    /* +0x9c */
    float spinAngle;                    /* +0xa0 */
    void *hitHistory[RG_GRENADE_HISTORY_MAX];
                                        /* +0xa4: the robots and background objects already hit */
    unsigned int historyCount;          /* +0xc4 */
    unsigned int confuseTime;          /* +0xc8: robots it hits are confused this long instead of damaged */
} RgGrenadeShot;

/* RgFireShot.status values. */
#define RG_FIRE_STATUS_START 0
#define RG_FIRE_STATUS_BURN 1
#define RG_FIRE_STATUS_FADE 2

typedef struct RgFireShot {
    RgShot common;                     /* +0x00 */
    unsigned char unmodeled_06c[4];    /* +0x6c */
    int status;                        /* +0x70: RG_FIRE_STATUS_* */
    unsigned char unmodeled_074[0x0C]; /* +0x74 */
    RgVector origin;                   /* +0x80 */
    RgVector tipPosition;              /* +0x90 */
    float speed;                       /* +0xa0 */
    float remainingTime;               /* +0xa4 */
    float damageRate;                  /* +0xa8 */
    float fadeTime;                    /* +0xac */
    int quickFade;                     /* +0xb0: nonzero fades out at once */
    int onceHit;                       /* +0xb4 */
    int alreadyHitRobot;               /* +0xb8 */
    unsigned char unmodeled_0bc[0x04]; /* +0xbc */
    RgShotRequest request;             /* +0xc0 */
} RgFireShot;

typedef struct RgBeamShot {
    RgShot common;                     /* +0x00 */
    unsigned char unmodeled_06c[4];    /* +0x6c */
    RgVector origin;                   /* +0x70 */
    RgVector direction;                /* +0x80 */
    float speed;                       /* +0x90 */
    float length;                      /* +0x94 */
    float lifetime;                    /* +0x98 */
    int hit;                           /* +0x9c */
    float damageRate;                  /* +0xa0 */
    unsigned char unmodeled_0a4[0x0C]; /* +0xa4 */
    RgShotRequest request;             /* +0xb0 */
} RgBeamShot;

/* Functions and data used by other translation units. */
void InitRgNormalShotEssence(RgNormalShotEssence *essence);
void InitRgHomingShotEssence(RgHomingShotEssence *essence);
void InitRgGrenadeEssence(RgGrenadeEssence *essence);
void InitRgFireEssence(RgFireEssence *essence);
void InitRgBeamEssence(RgBeamEssence *essence);
RgShot *CreateRgShotFromEssence(RgShotEssence *essence, RgShotRequest *info);
void RgShotSetSoundDriver(RgShot *shot, XrgSound *driver);
void RgShotSetHitSound(RgShot *shot, int soundId, int volume);
void RgShotSetBgHitSound(RgShot *shot, int soundId, int volume);
void RgShotSetNoLifeSound(RgShot *shot, int soundId, int volume);
void RgShotSetHideSound(RgShot *shot, int soundId);
float RgShotGetDamage(RgShot *shot);
int RgShotIsReleased(RgShot *shot);
int RgShotGetHitRobNum(RgShot *shot);
void RgShotRelease(RgShot *shot);
void RgShotHitRobot(RgShot *shot, RgStatus *robot, RgVector position);
void RgShotHitBg(RgShot *shot, struct RgBgObj *bgObject, RgVector position);

#endif /* INCLUDE_OV12_RG_SHOT_H */
