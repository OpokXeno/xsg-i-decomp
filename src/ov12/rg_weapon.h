/*
 * TU-local declarations of ov12/tu026 (src/ov12/rg_weapon.c).
 */

#ifndef SRC_OV12_RG_WEAPON_H
#define SRC_OV12_RG_WEAPON_H

#include "ov12/rg_weapon.h"
#include "ov12/rg_geom_ball.h"
#include "ov12/rg_matrices_effector.h"
#include "ov12/xrg_sound.h"

/* Shots a weapon keeps attached at once ("pAttach->m_uAttachNum < ATTACH_MAX"). */
#define ATTACH_MAX 8

/* RgWeapon.lockOnStopped. */
enum RgLockOnStop {
    RG_LOCKON_RUNNING = 0,
    RG_LOCKON_STOPPED = 1
};

/* The shots a weapon keeps attached, released when it stops or is freed. */
typedef struct RgWeaponAttach {
    RgShot *shots[ATTACH_MAX];
    unsigned int m_uAttachNum;
} RgWeaponAttach;

/*
 * A live weapon: the common part every weapon type shares. The type-specific
 * callbacks are installed by the type's _InitWeapon* function and run through
 * the RgWeapon* entry points below. _InitWeaponCommon copies the essence
 * members onto it. A shield weapon is a bare RgWeapon; the other types extend
 * it (RgWeaponShotType, RgWeaponAttackType, RgWeaponEnergyType).
 */
struct RgWeapon {
    enum RgWeaponControl controlFlags;                  /* +0x00: RG_WEAPON_CONTROL_* */
    RgStatus *robot;                           /* +0x04: owner */
    int id;                                     /* +0x08: the shot geometry group pointer, int-typed (measured: a pointer member reorders _InitWeaponCommon) */
    void *atkGeoms;                             /* +0x0c: copied from the creation info */
    int shotRequested;                          /* +0x10 */
    void (*dispose)(RgWeapon *weapon);          /* +0x14 */
    void (*disp)(RgWeapon *weapon);             /* +0x18 */
    void (*passTime)(RgWeapon *weapon, float dt); /* +0x1c */
    int (*shoot)(RgWeapon *weapon);             /* +0x20 */
    void (*shotStop)(RgWeapon *weapon);         /* +0x24 */
    void (*hitRobot)(RgWeapon *weapon, RgStatus *robot, RgVector position); /* +0x28 */
    void (*hitBG)(RgWeapon *weapon, RgBgObj *bgObject, RgVector position);     /* +0x2c */
    int (*isBusy)(RgWeapon *weapon);            /* +0x30 */
    void (*setFree)(RgWeapon *weapon);          /* +0x34 */
    struct RgShotEssence *shotEssence;          /* +0x38 */
    unsigned char unmodeled_3c[4];
    RgWeaponAttach shotAttach;                  /* +0x40 */
    int aimJointCursor;                         /* +0x64: next shot point of the essence's shotPnt */
    XrgActor *actor;                            /* +0x68 */
    unsigned char unmodeled_6c[4];
    RgEquip equip;                              /* +0x70 */
    int useMask;                                /* +0xa8 */
    int shotMotionTable[RG_ACTOR_CHAR_ROBNUM][RG_EQUIP_TYPE_NUM];      /* +0xac, indexed [charID][shotMotionOffset] */
    int shotStartMotionTable[RG_ACTOR_CHAR_ROBNUM][RG_EQUIP_TYPE_NUM]; /* +0xf4 */
    int shotEndMotionTable[RG_ACTOR_CHAR_ROBNUM][RG_EQUIP_TYPE_NUM];   /* +0x13c */
    float lockOnAngleMin;                       /* +0x184: _CheckLockOn's inclusive range */
    float lockOnAngleMax;                       /* +0x188 */
    unsigned char unmodeled_18c[4];
    RgVector targetPosition;                    /* +0x190: the locked-on target's last position */
    int targeting;                              /* +0x1a0 */
    int lockedOn;                               /* +0x1a4 */
    int shotMotionOffset;                       /* +0x1a8: the equip type */
    float shotNum;                              /* +0x1ac: rounds left, 1e8 = infinite */
    float weight;                               /* +0x1b0 */
    MatrixEffector *lockOnEffector;             /* +0x1b4 */
    RgWeaponEssence *ess;                       /* +0x1b8 */
    int equipped;                               /* +0x1bc */
    enum RgLockOnStop lockOnStopped;                        /* +0x1c0: RG_LOCKON_* */
    XrgSound *sound;                            /* +0x1c4: sound driver */
    int shotSound;                              /* +0x1c8 */
    int robotHitSound;                          /* +0x1cc */
    int bgHitSound;                             /* +0x1d0 */
    int lifeSound;                              /* +0x1d4 */
    int shotVolume;                             /* +0x1d8 */
    int robotHitVolume;                         /* +0x1dc */
    int bgHitVolume;                            /* +0x1e0 */
    int lifeVolume;                             /* +0x1e4 */
    unsigned char unmodeled_1e8[8];
};

/* An attack weapon, 0x230 bytes: a melee hit that follows an actor joint. */
typedef struct RgWeaponAttackType {
    RgWeapon common;
    int followJoint;                            /* +0x1f0 */
    int followExtra;                            /* +0x1f4 */
    XrgActor *followActor;                      /* +0x1f8 */
    float damage;                               /* +0x1fc */
    RgGeomBall *geom;                           /* +0x200: the collision ball */
    unsigned char unmodeled_204[0x0c];
    RgVector followPosition;                    /* +0x210: last position the ball was moved to */
    int active;                                 /* +0x220 */
    int shotSoundPlaying;                       /* +0x224: the fire sound has rung */
    unsigned char unmodeled_228[8];
} RgWeaponAttackType;

/* A shot weapon, 0x200 bytes. */
typedef struct RgWeaponShotType {
    RgWeapon common;
    float busyTime;                             /* +0x1f0 */
    float cooldown;                             /* +0x1f4: time before it can fire again */
    unsigned char unmodeled_1f8[8];
} RgWeaponShotType;

/* An energy weapon, 0x210 bytes. */
typedef struct RgWeaponEnergyType {
    RgWeapon common;
    int active;                                 /* +0x1f0: overheated */
    int shotPending;                            /* +0x1f4 */
    float energyCharge;                         /* +0x1f8 */
    float shotEnergy;
    float upTemp;
    float limitTemp;
    float downTemp;
    unsigned char unmodeled_20c[4];
} RgWeaponEnergyType;

#endif /* SRC_OV12_RG_WEAPON_H */
