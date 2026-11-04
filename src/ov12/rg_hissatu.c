/*
 * OV12 original TU 14: 0x00a10318..0x00a10618 (3 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_hissatu.h"
#include "ov12/rg_weapon_db.h"


/* Declared here: the assert text of _IsSpecWep, in the scaffold-owned data window. */
extern const char hissatuAssertNameNotNil[];

/* Defined here: the assert strings and the weapon database names a special move needs. */
const char hissatuSourcePath[24] = "../rg_hissatu.euc.c";
const char hissatuAssertWeaponNotNil[16] = "apWep != NIL";
const char hissatuWeaponNameLm11vx[8] = "LM11VX";
const char hissatuWeaponNameShd02ag[8] = "SHD02AG";
const char hissatuWeaponNameShd12vx[8] = "SHD12VX";
const char hissatuWeaponNameHg75vx[8] = "HG75VX";
const char hissatuWeaponNameHg45vx[8] = "HG45VX";
const char hissatuWeaponNameErVx[8] = "ER-VX";
const char hissatuWeaponNameBsw13ag[8] = "BSW13AG";
const char hissatuWeaponNameGrd20ag[8] = "GRD20AG";
const char hissatuWeaponNameHmr55ag[8] = "HMR55AG";

static int _IsSpecWep(RgWeapon *weapon, const char *weaponName);

static int _IsSpecWep(RgWeapon *weapon, const char *weaponName)
{
    RgWeaponEssence *essence;

    if (weaponName == 0) {
        assert_prog(hissatuAssertNameNotNil, hissatuSourcePath, 21);
    }
    if (weapon == 0) {
        return 0;
    }
    essence = RgWeaponGetEss(weapon);
    if (essence == 0 || strcasecmp(essence->databaseName, weaponName) != 0) {
        return 0;
    }
    if (RgWeaponGetShotNum(weapon) <= 0.0f || RgWeaponIsNotBusy(weapon) == 0) {
        return 0;
    }
    return 1;
}

static int _IsUnarmed(RgWeapon *weapon)
{
    int isUnarmed;

    isUnarmed = 1;
    if (weapon != 0) {
        isUnarmed = (RgWeaponGetControlFlag(weapon) & RG_WEAPON_CONTROL_KIND_MASK) == RG_WEAPON_CONTROL_KIND_UNARMED;
    }
    return isUnarmed;
}

int RgHissatuCheck(unsigned int attackType, RgWeapon *weapons[RG_EQUIP_TYPE_NUM])
{
    RgWeapon *weapon0;
    RgWeapon *weapon1;
    RgWeapon *weapon2;

    if (weapons == 0) {
        assert_prog(hissatuAssertWeaponNotNil, hissatuSourcePath, 48);
    }
    weapon0 = weapons[RG_EQUIP_TYPE_LEFT];
    weapon1 = weapons[RG_EQUIP_TYPE_RIGHT];
    weapon2 = weapons[RG_EQUIP_TYPE_BACK];
    if (weapon2 != 0 &&
        (RgWeaponGetControlFlag(weapon2) & RG_WEAPON_CONTROL_KIND_MASK) != RG_WEAPON_CONTROL_KIND_UNARMED) {
        return 0;
    }
    if (weapon0 == 0) {
        return 0;
    }
    if (weapon1 == 0) {
        return 0;
    }
    if (RgWeaponIsNotBusy(weapon0) == 0) {
        return 0;
    }
    if (RgWeaponIsNotBusy(weapon1) == 0) {
        return 0;
    }
    switch (attackType) {
    case 0:
        if (_IsSpecWep(weapon1, hissatuWeaponNameLm11vx) != 0) {
            if (_IsSpecWep(weapon0, hissatuWeaponNameShd02ag) != 0) {
                return 1;
            }
            if (_IsSpecWep(weapon0, hissatuWeaponNameShd12vx) != 0) {
                return 1;
            }
        }
        break;
    case 2:
        if (_IsSpecWep(weapon1, hissatuWeaponNameHg75vx) != 0 ||
            _IsSpecWep(weapon1, hissatuWeaponNameHg45vx) != 0) {
            if (_IsSpecWep(weapon0, hissatuWeaponNameHg75vx) != 0) {
                return 1;
            }
            if (_IsSpecWep(weapon0, hissatuWeaponNameHg45vx) != 0) {
                return 1;
            }
        }
        break;
    case 4:
        if (_IsSpecWep(weapon0, hissatuWeaponNameErVx) != 0) {
            if (_IsSpecWep(weapon1, hissatuWeaponNameHg75vx) != 0) {
                return 1;
            }
            if (_IsSpecWep(weapon1, hissatuWeaponNameHg45vx) != 0) {
                return 1;
            }
        }
        break;
    case 5:
        if (_IsSpecWep(weapon1, hissatuWeaponNameBsw13ag) != 0 &&
            _IsSpecWep(weapon0, hissatuWeaponNameGrd20ag) != 0) {
            return 1;
        }
        break;
    case 3:
        if (_IsUnarmed(weapon1) != 0) {
            if (_IsUnarmed(weapon0) != 0) {
                return 1;
            }
        }
        break;
    case 1:
        if (_IsSpecWep(weapon1, hissatuWeaponNameHmr55ag) != 0 &&
            _IsSpecWep(weapon0, hissatuWeaponNameHmr55ag) != 0) {
            return 1;
        }
        break;
    default:
        break;
    }
    return 0;
}
