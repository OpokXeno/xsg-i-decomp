/*
 * OV12 original TU 9: 0x00a0db58..0x00a0df70 (8 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "rg_robot_db.h"

static void _WrapperDestruct(RgSimpleDB *database)
{
    DisposeRgSimpleDB(database);
}

RgSimpleDB *InstanceOfRgRobotDB(void)
{
    RgSimpleDB *database;

    database = RgSingletonIDGet(3);
    if (database == 0) {
        database = CreateRgSimpleDB(0x20, 0x10);
        RgSingletonIDEntry(3, database, _WrapperDestruct);
        RgRobotDBClear(database);
    }
    return database;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_db", RgRobotDBClear);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_db", RgRobotDBGet);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_db", RgRobotDBGetByID);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_db", RgRobotDBGetDefault);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_db", RgRobotDBRead);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_db", RgRobotDBDump);
