/*
 * OV12 original TU 30: 0x00a1eea0..0x00a1f7e0 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_equip_type.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern const char D_00A53E98[]; /* "pEquip != NIL" */
extern const char D_00A53E80[]; /* "../rg_equip_type.euc.c" */

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", RgEquipPosToJntID);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", RgEquipIsEquipable);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", RgEquipCheckConfrict);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", RgEquipTypeString);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", _GetMountData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", RgEquipMountPos);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_equip_type", RgEquipMountPosString);
