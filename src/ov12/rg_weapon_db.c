/*
 * OV12 original TU 27: 0x00a1c7d8..0x00a1e618 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_simple_db.h"
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

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern unsigned int strlen(const char *string);

/*
 * Scaffold-owned (.rodata still owner: asm, config/tu-build.json
 * data_ownership window 0x00a53816..0x00a53cf0). ov12:0x00a53818 holds
 * "pDB != NIL", ov12:0x00a53828 holds "../rg_weapon_db.euc.c".
 */
extern const char D_00A53818[];
extern const char D_00A53828[];

extern void RgSimpleDBClear(RgSimpleDB *pDB);
extern int RgSimpleDBFind(RgSimpleDB *pDB, const char *pszName);
extern int RgSimpleDBSize(RgSimpleDB *pDB);
extern void *RgSimpleDBGet(RgSimpleDB *pDB, int nDataID);
extern void RgSimpleDBDump(void);

void RgWeaponDBClear(RgSimpleDB *pDB)
{
    if (pDB == 0) {
        assert_prog(D_00A53818, D_00A53828, 85);
    }
    RgSimpleDBClear(pDB);
    _EntryTemporaries(pDB);
}

void *RgWeaponDBGetEssence(RgSimpleDB *pDB, const char *pszName)
{
    int nDataID;

    if ((pszName == 0) || (strlen(pszName) == 0)) {
        return 0;
    }
    nDataID = RgSimpleDBFind(pDB, pszName);
    if (nDataID >= 0) {
        return RgSimpleDBGet(pDB, nDataID);
    }
    return 0;
}

int RgWeaponDBGetSize(RgSimpleDB *pDB)
{
    if (pDB == 0) {
        assert_prog(D_00A53818, D_00A53828, 113);
    }
    return RgSimpleDBSize(pDB);
}

void *RgWeaponDBGetIndex(RgSimpleDB *pDB, int nDataID)
{
    if (pDB == 0) {
        assert_prog(D_00A53818, D_00A53828, 121);
    }
    return RgSimpleDBGet(pDB, nDataID);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadEquipType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _IsOnChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadEquipBitMask);

/*
 * RG_ACTOR_CHAR_ROBNUM (6): the same evidenced constant as ov12/tu026's
 * RgWeapon.shotMotionTable[RG_ACTOR_CHAR_ROBNUM][3] (src/ov12/rg_weapon.h),
 * the number of party characters who can pilot an AGWS robot. The loop below
 * fills the equivalent per-character motion table one row at a time.
 */
#define RG_ACTOR_CHAR_ROBNUM 6

static void _SetAllCharMotion(int motionTable[][3], int motion0, int motion1,
                               int motion2)
{
    unsigned int i;

    for (i = 0; i < RG_ACTOR_CHAR_ROBNUM; i++) {
        motionTable[i][0] = motion0;
        motionTable[i][1] = motion1;
        motionTable[i][2] = motion2;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadCommon_00A1CD08);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadShotType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadAttackType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadUnArmedType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadShieldType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", _ReadEnergyType);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_weapon_db", RgWeaponDBRead);

void RgWeaponDBDump(void)
{
    RgSimpleDBDump();
}
