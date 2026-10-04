/*
 * OV12 original TU 30: 0x00a1eea0..0x00a1f7e0 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_equip_type.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

static int s_aePosToJntID[8] = {0, 2, 4, 1, 3, 5, 6, 7};
const char D_00A53E10[] = "(RG_EQUIP_POS_LEFT_HAND <= (ePos) && (ePos) <= RG_EQUIP_POS_BACK_RIGHT) || (ePos) == RG_EQUIP_POS_INVALID";
const char D_00A53E80[] = "../rg_equip_type.euc.c";
const char D_00A53E98[] = "pEquip != NIL";
const char D_00A53EC8[] = "RG_EQUIP_TYPE_MIN <= (eType) && (eType) < RG_EQUIP_TYPE_NUM";
const char D_00A53F90[] = "left";
const char D_00A53F98[] = "right";
const char D_00A53FA0[] = "back";
const char D_00A53FA8[] = "???";

#define RG_EQUIP_TYPE_COUNT 3

int RgEquipPosToJntID(int ePos)
{
    if ((unsigned int)ePos + 1u >= 9u) {
        assert_prog(D_00A53E10, D_00A53E80, 50);
    }
    if (ePos == -1) {
        return -1;
    }
    return s_aePosToJntID[ePos];
}

void InitRgEquip(RgEquipRecord *pEquip)
{
    if (pEquip == 0) {
        assert_prog(D_00A53E98, D_00A53E80, 0x3D);
    }
    pEquip->mountCount = 0;
    pEquip->activeMount = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", CopyRgEquip);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", RgEquipAddMount);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", RgEquipSetDefault);

int RgEquipIsEquipable(RgEquipRecord *pEquip, int eType)
{
    int mountIndex;

    if (pEquip == 0) {
        assert_prog(D_00A53E98, D_00A53E80, 131);
    }
    for (mountIndex = 0; mountIndex < pEquip->mountCount; mountIndex++) {
        if (pEquip->mounts[mountIndex].type == eType) {
            return 1;
        }
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", RgEquipCheckConfrict);

const char *RgEquipTypeString(int eType)
{
    if ((unsigned int)eType >= RG_EQUIP_TYPE_COUNT) {
        assert_prog(D_00A53EC8, D_00A53E80, 276);
    }
    switch (eType) {
    case 0:
        return D_00A53F90;
    case 1:
        return D_00A53F98;
    case 2:
        return D_00A53FA0;
    default:
        return D_00A53FA8;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", _GetMountData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", RgEquipMountPos);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", RgEquipMountPosString);
