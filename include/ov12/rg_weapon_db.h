#ifndef INCLUDE_OV12_RG_WEAPON_DB_H
#define INCLUDE_OV12_RG_WEAPON_DB_H

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
