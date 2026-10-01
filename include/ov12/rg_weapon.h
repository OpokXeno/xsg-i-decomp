#ifndef INCLUDE_OV12_RG_WEAPON_H
#define INCLUDE_OV12_RG_WEAPON_H

/*
 * The weapon handle owned by rg_weapon.c, opaque here.
 */
typedef struct RgWeapon RgWeapon;

/*
 * Opaque per-type essence and creation-info handles _CreateWeaponAttackType,
 * _CreateWeaponUnArmedType, _CreateWeaponShieldType and _CreateWeaponEnergyType
 * only null-check ("pEss != NIL", "pInfo != NIL", ov12:0x00a53710/0x00a53720)
 * and forward to their still-unrecovered _InitWeaponAttack/_InitWeaponUnArmedType/
 * _InitWeaponShield/_InitEnergyType; no interior is evidenced here.
 */
typedef struct RgWeaponCreateInfo RgWeaponCreateInfo;

typedef struct RgWeaponUnArmedEssence RgWeaponUnArmedEssence;

/*
 * CreateRgWeaponFromEssence (ov12:0x00a1b420) is the shared essence -> weapon
 * front end every concrete essence type's create-info pair goes through: it
 * only ever reads the create-method slot every essence type places at +0x00
 * (RgWeaponShieldEssence.m_pCreateMethod below, RgWeaponUnArmedEssenceInit's
 * own m_pCreateMethod further below), so only that one member is claimed
 * through this generic view.
 */
typedef struct RgWeaponEssenceCommon {
    RgWeapon *(*m_pCreateMethod)(void *essence, RgWeaponCreateInfo *info);
} RgWeaponEssenceCommon;

RgWeapon *CreateRgWeaponFromEssence(RgWeaponEssenceCommon *pEss, RgWeaponCreateInfo *pInfo);

RgWeaponUnArmedEssence *RgWeaponEssCastToUnArmed(RgWeaponEssenceCommon *ess);

#endif /* INCLUDE_OV12_RG_WEAPON_H */
