#ifndef INCLUDE_OV12_RG_WEAPON_DB_H
#define INCLUDE_OV12_RG_WEAPON_DB_H

/*
 * _CreateWeaponShotType (ov12:0x00a1a410) only null-checks its essence and
 * creation-info arguments ("pEss != NIL", "pInfo != NIL",
 * ov12:0x00a53710/0x00a53720) and forwards them to the still-unrecovered
 * _InitWeaponShot; no interior of RgWeaponShotEssence is evidenced here.
 */
typedef struct RgWeaponShotEssence RgWeaponShotEssence;

/*
 * The prefix every weapon essence the weapon database builds shares: each
 * _Read*Type function of rg_weapon_db.c hands its own essence to _ReadCommon
 * (ov12:0x00a1cd08), which fills these members from the common keys.
 *
 * shotEssence (+0x10): the "shot" key (ov12:0x00a53a48) names an entry of
 * the shot database. _ReadCommon stores RgShotDBGetEssence's result here
 * (sw $2,0x10 at ov12:0x00a1d130) and reports "unknown shot '%s'" when there
 * is none. That entry is whichever shot kind RgShotDBRead built (normal,
 * homing, grenade, beam or fire), so it is typed by the prefix all of them
 * share. rg_weapon.c's _InitWeaponCommon copies it onto the weapon
 * (ov12:0x00a198ec/0x00a19904), and _ShotTypeShot and _EnergyPassTime pass
 * that copy to CreateRgShotFromEssence (ov12:0x00a1a0ec/0x00a1b0dc).
 *
 * capacity (+0x14): the "num", "energy" and "hardness" keys all store one
 * float here, and "inf" stores 1e8 (swc1 0x14 at ov12:0x00a1d02c/0x00a1d048).
 * _InitWeaponCommon copies it to the weapon's shot count (ov12:0x00a198c0/
 * 0x00a198d0).
 *
 * _ReadCommon also writes parts of the span before +0x10 (the "lock" key
 * stores two angles at +0x08/+0x0c); that span stays unmodeled here.
 *
 * shotEssence is spelled with the struct tag so that this type needs no
 * typedef from rg_shot.c's header; RgShotEssence (src/ov12/rg_shot.h)
 * completes the same tag.
 */
typedef struct RgWeaponEssence RgWeaponEssence;

struct RgWeaponEssence {
    unsigned char unmodeled_00[0x10];
    struct RgShotEssence *shotEssence; /* +0x10 */
    float capacity;                    /* +0x14 */
};

struct RgWeaponShotEssence {
    unsigned char unmodeled_000[0x3b0];
    float busyTime;             /* +0x3b0, the "busy" key above */
    unsigned char unmodeled_3b4[0x0c];
};

struct RgWeaponAttackEssence {
    unsigned char unmodeled_000[0x3b8];
    float damage;                /* +0x3b8, the "damage" key above */
    char hitEffectName[0x24];    /* +0x3bc, the "hiteff" key above */
};

struct RgWeaponUnArmedEssence {
    unsigned char unmodeled_000[0x3b8];
    float damage;                /* +0x3b8, the "damage" key above */
    char hitEffectName[0x24];    /* +0x3bc, the "hiteff" key above */
};

#endif /* INCLUDE_OV12_RG_WEAPON_DB_H */
