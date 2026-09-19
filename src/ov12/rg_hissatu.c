/*
 * OV12 original TU 14: 0x00a10318..0x00a10618 (3 functions)
 */
#include "common.h"
#include "rg_hissatu.h"

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_hissatu", RgHissatuCheck);
