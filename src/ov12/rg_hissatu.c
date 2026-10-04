/*
 * OV12 original TU 14: 0x00a10318..0x00a10618 (3 functions)
 */
#include "common.h"
#include "rg_hissatu.h"

const char D_00A52670[24] = "../rg_hissatu.euc.c";
const char D_00A52688[16] = "apWep != NIL";
const char D_00A52698[8] = "LM11VX";
const char D_00A526A0[8] = "SHD02AG";
const char D_00A526A8[8] = "SHD12VX";
const char D_00A526B0[8] = "HG75VX";
const char D_00A526B8[8] = "HG45VX";
const char D_00A526C0[8] = "ER-VX";
const char D_00A526C8[8] = "BSW13AG";
const char D_00A526D0[8] = "GRD20AG";
const char D_00A526D8[8] = "HMR55AG";


extern void assert_prog(const char *expression, const char *sourceFile,
                        int sourceLine);
extern int RgWeaponIsNotBusy(RgWeapon *weapon);

static int _IsSpecWep(RgWeapon *weapon, const char *weaponName);


INCLUDE_ASM("asm/nonmatchings/ov12/rg_hissatu", _IsSpecWep);

extern int RgWeaponGetControlFlag(RgWeapon *weapon);

static int _IsUnarmed(RgWeapon *weapon)
{
    int isUnarmed;

    isUnarmed = 1;
    if (weapon != 0) {
        isUnarmed = (RgWeaponGetControlFlag(weapon) & 7) == 4;
    }
    return isUnarmed;
}

int RgHissatuCheck(unsigned int attackType, RgWeapon *weapons[3])
{
    RgWeapon *weapon0;
    RgWeapon *weapon1;
    RgWeapon *weapon2;

    if (weapons == 0) {
        assert_prog(D_00A52688, D_00A52670, 48);
    }
    weapon0 = weapons[0];
    weapon1 = weapons[1];
    weapon2 = weapons[2];
    if (weapon2 != 0 &&
        (RgWeaponGetControlFlag(weapon2) & 7) != 4) {
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
        if (_IsSpecWep(weapon1, D_00A52698) != 0) {
            if (_IsSpecWep(weapon0, D_00A526A0) != 0) {
                return 1;
            }
            if (_IsSpecWep(weapon0, D_00A526A8) != 0) {
                return 1;
            }
        }
        break;
    case 2:
        if (_IsSpecWep(weapon1, D_00A526B0) != 0 ||
            _IsSpecWep(weapon1, D_00A526B8) != 0) {
            if (_IsSpecWep(weapon0, D_00A526B0) != 0) {
                return 1;
            }
            if (_IsSpecWep(weapon0, D_00A526B8) != 0) {
                return 1;
            }
        }
        break;
    case 4:
        if (_IsSpecWep(weapon0, D_00A526C0) != 0) {
            if (_IsSpecWep(weapon1, D_00A526B0) != 0) {
                return 1;
            }
            if (_IsSpecWep(weapon1, D_00A526B8) != 0) {
                return 1;
            }
        }
        break;
    case 5:
        if (_IsSpecWep(weapon1, D_00A526C8) != 0 &&
            _IsSpecWep(weapon0, D_00A526D0) != 0) {
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
        if (_IsSpecWep(weapon1, D_00A526D8) != 0 &&
            _IsSpecWep(weapon0, D_00A526D8) != 0) {
            return 1;
        }
        break;
    default:
        break;
    }
    return 0;
}
