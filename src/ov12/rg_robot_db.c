/*
 * OV12 original TU 9: 0x00a0db58..0x00a0df70 (8 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_simple_db.h"
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

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void RgError(const char *message, const char *source_file, int line,
                    ...);
extern int RgSimpleDBFind(RgSimpleDB *pDB, const char *pszName);
extern void *RgSimpleDBGet(RgSimpleDB *pDB, int nDataID);
extern void RgSimpleDBClear(RgSimpleDB *pDB);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a52280 contains the assertion expression "pDB != NIL".
 * ov12:0x00a52290 contains the source filename "../rg_robot_db.euc.c".
 */
extern const char D_00A52280[];
extern const char D_00A52290[];

void RgRobotDBClear(RgSimpleDB *database)
{
    if (database == 0) {
        assert_prog(D_00A52280, D_00A52290, 49);
    }
    RgSimpleDBClear(database);
}

void *RgRobotDBGet(RgSimpleDB *database, const char *name)
{
    int dataID;

    if (database == 0) {
        assert_prog(D_00A52280, D_00A52290, 64);
    }
    dataID = RgSimpleDBFind(database, name);
    if (dataID >= 0) {
        return RgSimpleDBGet(database, dataID);
    }
    return 0;
}

extern const char *RgActorCharIDToName(int charID);

/*
 * ov12:0x00a522a8 contains "char id (%d) has no char-name".
 */
extern const char D_00A522A8[];

void *RgRobotDBGetByID(RgSimpleDB *database, int charID)
{
    const char *name;

    name = RgActorCharIDToName(charID);
    if (database == 0) {
        assert_prog(D_00A52280, D_00A52290, 76);
    }
    if (name == 0) {
        RgError(D_00A522A8, D_00A52290, 78, charID);
    }
    return RgRobotDBGet(database, name);
}

typedef struct RgRobotSpec RgRobotSpec;

extern void InitRgRobotSpec(RgRobotSpec *pSpec);

/*
 * Scaffold-owned (.bss still owner: asm): the default robot spec instance
 * RgRobotDBGetDefault fills in place and returns.
 */
extern RgRobotSpec D_00A599E0;

RgRobotSpec *RgRobotDBGetDefault(void)
{
    InitRgRobotSpec(&D_00A599E0);
    return &D_00A599E0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_db", RgRobotDBRead);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_db", RgRobotDBDump);
