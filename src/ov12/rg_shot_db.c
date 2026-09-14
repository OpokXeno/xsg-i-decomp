/*
 * OV12 original TU 25: 0x00a18668..0x00a19388 (13 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "rg_shot_db.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", _EntryTemporariesShotDB);

/*
 * The destructor callback is local to this translation unit: the original
 * symbol at 0x00a18670 has LOCAL binding.  CreateRgSimpleDB's first argument
 * is its initial capacity and its second is the byte size of each entry;
 * those roles are established by the callee's stores and allocation loop.
 */

static void _WrapperDestruct(RgSimpleDB *database)
{
    DisposeRgSimpleDB(database);
}

RgSimpleDB *InstanceOfRgShotDB(void)
{
    RgSimpleDB *shot_database;

    shot_database = RgSingletonIDGet(1);
    if (shot_database == 0) {
        shot_database = CreateRgSimpleDB(32, 16);
        _EntryTemporariesShotDB(shot_database);
        RgSingletonIDEntry(1, shot_database, _WrapperDestruct);
    }
    return shot_database;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", RgShotDBGetEssence);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", _ReadCommon_00A18730);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", _ReadNormal);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", _ReadHomingMain);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", _ReadHoming);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", _ReadGrenade);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", _ReadBeam);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", _ReadFire);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", RgShotDBRead);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", RgShotDBClear);
