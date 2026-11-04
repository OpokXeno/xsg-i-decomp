#ifndef INCLUDE_OV12_RG_WEAPON_DB_H
#define INCLUDE_OV12_RG_WEAPON_DB_H

#include "shared.h"
#include "ov12/rg_equip_type.h"
#include "ov12/rg_simple_db.h"
#include "ov12/xrg_actor.h"

/* Party characters who can pilot an AGWS robot: rows of the per-character motion tables. */
#define RG_ACTOR_CHAR_ROBNUM 6

/* Joint indices a weapon essence lists for its shot points ("shot-pnt"). */
#define RG_WEAPON_SHOTPNT_MAX 16

/*
 * RgWeaponEssence.controlFlags, as the "cont" key of the weapon database sets
 * it. The low three bits are the weapon kind; bits 3..4 the target mode
 * ("not-target", "wth-target", "aft-target").
 */
enum RgWeaponControl { RG_WEAPON_CONTROL_NONE = 0 };
enum RgActorFlag { RG_ACTOR_FLAG_NONE = 0 };

#define RG_WEAPON_CONTROL_KIND_MASK 0x07
#define RG_WEAPON_CONTROL_KIND_FULL_MOT 1  /* "full-mot", shot types only */
#define RG_WEAPON_CONTROL_KIND_ATTACK 2
#define RG_WEAPON_CONTROL_KIND_SHIELD 3
#define RG_WEAPON_CONTROL_KIND_UNARMED 4
#define RG_WEAPON_CONTROL_TARGET_MASK 0x18
#define RG_WEAPON_CONTROL_TARGET_WTH 0x08
#define RG_WEAPON_CONTROL_TARGET_AFT 0x10
#define RG_WEAPON_CONTROL_LOCKON 0x20      /* cleared by "notlock" */
#define RG_WEAPON_CONTROL_ADV 0x40         /* "adv" / "not-adv" */
#define RG_WEAPON_CONTROL_NO_ROBMOT 0x80   /* "no-robmot" */

/* RgWeaponEssence.aimAxis: no joint matrix row to aim along. */
#define RG_WEAPON_AIM_AXIS_NONE 3

/* RgWeaponEssence.capacity and RgWeapon.shotNum of a weapon with no ammunition limit ("inf"). */
/* Motion ids the essence initialisers put in shotMotionTable, per equip type. */
#define RG_WEAPON_MOTION_SHOT_LEFT 39
#define RG_WEAPON_MOTION_SHOT_RIGHT 40
#define RG_WEAPON_MOTION_ATTACK_LEFT 0x2B   /* attack and shield weapons */
#define RG_WEAPON_MOTION_ATTACK_RIGHT 0x2C
#define RG_WEAPON_MOTION_UNARMED_LEFT 0x15
#define RG_WEAPON_MOTION_UNARMED_RIGHT 0x16
/* Defaults of _InitEssenceCommon. */
#define RG_WEAPON_DEFAULT_CAPACITY 50.0f
#define RG_WEAPON_SOUND_SHOT 21
#define RG_WEAPON_SOUND_HIT 22

#define RG_WEAPON_SHOT_INFINITE 1.0e8f

/* Weapon database sound volumes run from 0 to this (the "v*snd" keys scale by it). */
#define RG_WEAPON_VOLUME_MAX 128

/* The weapon handle owned by rg_weapon.c, opaque outside it. */
typedef struct RgWeapon RgWeapon;

/* The creation info CreateRgWeaponFromEssence takes (include/ov12/rg_weapon.h). */
typedef struct RgWeaponCreateInfo RgWeaponCreateInfo;

/*
 * Every essence type's m_pCreateMethod: builds the weapon its type needs from
 * the essence it sits in.
 */
typedef RgWeapon *(*RgWeaponCreateFunc)(void *essence, RgWeaponCreateInfo *info);

typedef struct RgWeaponEssence RgWeaponEssence;
typedef struct RgWeaponShotEssence RgWeaponShotEssence;
typedef struct RgWeaponAttackEssence RgWeaponAttackEssence;
typedef struct RgWeaponAttackEssence RgWeaponUnArmedEssence;
typedef struct RgWeaponShieldEssence RgWeaponShieldEssence;
typedef struct RgWeaponEnergyEssence RgWeaponEnergyEssence;

/*
 * The prefix every weapon essence shares: _InitEssenceCommon (rg_weapon.c)
 * fills the defaults, each _Read*Type of rg_weapon_db.c parses the common
 * keys into it through _ReadCommon, and _InitWeaponCommon copies it onto the
 * weapon. The key it was read from is named beside each member.
 */
struct RgWeaponEssence {
    RgWeaponCreateFunc m_pCreateMethod;   /* +0x00 */
    enum RgWeaponControl controlFlags;            /* +0x04: "cont", RG_WEAPON_CONTROL_* */
    float lockOnAngleMin;                 /* +0x08: "lock" (radians) */
    float lockOnAngleMax;                 /* +0x0c */
    struct RgShotEssence *shotEssence;    /* +0x10: "shot", an entry of the shot database */
    float capacity;                       /* +0x14: "num", "energy", "hardness"; "inf" is 1e8 */
    float shotPitch;                      /* +0x18: "v-dir" */
    float shotYaw;                        /* +0x1c: "h-dir" */
    int aimAxis;                          /* +0x20: joint matrix row the shot follows; 3 = none */
    int shotPntNum;                       /* +0x24: "shot-pnt" count */
    int shotPnt[RG_WEAPON_SHOTPNT_MAX];   /* +0x28: joint indices */
    int unmodeled_68;                     /* +0x68 */
    unsigned char unmodeled_6c[4];
    RgEquip equip;                        /* +0x70: "equip", "occupy", "fake" */
    int useMask;                          /* +0xa8: "use", bit per character */
    float mass;                           /* +0xac: "mass", "weight" */
    int shotMotionTable[RG_ACTOR_CHAR_ROBNUM][RG_EQUIP_TYPE_NUM];      /* +0xb0: "mot" */
    int shotStartMotionTable[RG_ACTOR_CHAR_ROBNUM][RG_EQUIP_TYPE_NUM]; /* +0xf8: "selfmot" */
    int shotEndMotionTable[RG_ACTOR_CHAR_ROBNUM][RG_EQUIP_TYPE_NUM];   /* +0x140: "basemot" */
    enum RgActorFlag actorFlag;                        /* +0x188: nonzero when the weapon has an actor model */
    unsigned char unmodeled_18c[4];
    XrgActorEssence actorSpec[RG_EQUIP_TYPE_NUM]; /* +0x190: "model", "modelall" */
    unsigned char unmodeled_328[8];
    char databaseName[0x20];              /* +0x330: record key */
    char displayName[0x30];               /* +0x350: "name" */
    int shotSound;                        /* +0x380: "shotsnd" */
    int robotHitSound;                    /* +0x384: "hitsnd" */
    int bgHitSound;                       /* +0x388: "bgsnd" */
    int lifeSound;                        /* +0x38c: "lifesnd" */
    int shotVolume;                       /* +0x390: "vshotsnd" */
    int robotHitVolume;                   /* +0x394: "vhitsnd" */
    int bgHitVolume;                      /* +0x398: "vbgsnd" */
    int lifeVolume;                       /* +0x39c: "vlifesnd" */
    int eventLevel;                       /* +0x3a0: "1wa", "2wa", "3wa": event level at which the weapon becomes selectable */
    int isDefault;                        /* +0x3a4: "default" */
    unsigned char unmodeled_3a8[8];
};

/* "shottype": a weapon that fires a shot from the shot database. */
struct RgWeaponShotEssence {
    RgWeaponEssence common;
    float busyTime;                       /* +0x3b0: "busy" */
    unsigned char unmodeled_3b4[0x0c];
};

/* "atktype" and "sudetype" (unarmed): a melee attack. */
struct RgWeaponAttackEssence {
    RgWeaponEssence common;
    int followJoint;                      /* +0x3b0 */
    int followExtra;                      /* +0x3b4 */
    float damage;                         /* +0x3b8: "damage" */
    char hitEffectName[0x24];             /* +0x3bc: "hiteff" */
};

/* "sldtype": a shield. */
struct RgWeaponShieldEssence {
    RgWeaponEssence common;
};

/* "enetype": a weapon that charges and overheats. */
struct RgWeaponEnergyEssence {
    RgWeaponEssence common;
    float shotEnergy;                     /* +0x3b0: "shot-energy" */
    float upTemp;                         /* +0x3b4: "up-temp" */
    float limitTemp;                      /* +0x3b8: "limit-temp" */
    float downTemp;                       /* +0x3bc: "down-temp" */
};

/* Functions and data used by other translation units. */
RgSimpleDB *InstanceOfRgWeaponDB(void);
void RgWeaponDBClear(RgSimpleDB *pDB);
void *RgWeaponDBGetEssence(RgSimpleDB *pDB, const char *pszName);
int RgWeaponDBGetSize(RgSimpleDB *pDB);
void *RgWeaponDBGetIndex(RgSimpleDB *pDB, int nDataID);
void RgWeaponDBRead(RgSimpleDB *pDB, const char *pszDataBase);
void RgWeaponDBDump(RgSimpleDB *pDB);

#endif /* INCLUDE_OV12_RG_WEAPON_DB_H */
