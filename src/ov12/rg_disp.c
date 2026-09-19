/*
 * OV12 original TU 44: 0x00a26a00..0x00a26c78 (3 functions)
 */
#include "common.h"
#include "rg_disp.h"

extern RgWeaponEssence *RgWeaponGetEss(RgWeapon *weapon);
extern unsigned int RgWeaponGetRestUnit(RgWeapon *weapon);
extern float RgWeaponGetShotNum(RgWeapon *weapon);

/* ov12:0x00a54970 "%d/%d" */
extern const char D_00A54970[];
/* ov12:0x00a54978 "%d%%" */
extern const char D_00A54978[];

int RgDispWpnDat_CreateRestNumStr(RgWeapon *weapon, char *buffer)
{
    RgWeaponEssence *ess;
    unsigned int restUnit;
    float shotNum;
    float capacity;

    ess = RgWeaponGetEss(weapon);
    restUnit = RgWeaponGetRestUnit(weapon);
    shotNum = RgWeaponGetShotNum(weapon);
    capacity = ess->capacity;
    switch (restUnit) {
    case 0:
        return sprintf(buffer, D_00A54970, (int) shotNum, (int) capacity);
    case 1:
        return sprintf(buffer, D_00A54978, (int) (shotNum / capacity * 100.0f));
    case 2:
        *buffer = 0;
        break;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp", RgDispWpnDat_GetNameUVWH);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp", RgDispWpnDat_CreateSpeedStr);
