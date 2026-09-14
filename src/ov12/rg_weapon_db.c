/*
 * OV12 original TU 27: 0x00a1c7d8..0x00a1e618 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "rg_weapon_db.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _EntryTemporaries);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _copy_str_n);

static void _WrapperDestruct(RgSimpleDB *database)
{
    DisposeRgSimpleDB(database);
}

RgSimpleDB *InstanceOfRgWeaponDB(void)
{
    RgSimpleDB *database = RgSingletonIDGet(2);

    if (database == 0) {
        database = CreateRgSimpleDB(48, 16);
        _EntryTemporaries(database);
        RgSingletonIDEntry(2, database, _WrapperDestruct);
    }
    return database;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", RgWeaponDBClear);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", RgWeaponDBGetEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", RgWeaponDBGetSize);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", RgWeaponDBGetIndex);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadEquipType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _IsOnChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadEquipBitMask);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _SetAllCharMotion);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadCommon_00A1CD08);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadShotType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadAttackType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadUnArmedType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadShieldType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadEnergyType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", RgWeaponDBRead);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", RgWeaponDBDump);
