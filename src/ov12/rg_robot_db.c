/*
 * OV12 original TU 9: 0x00a0db58..0x00a0df70 (8 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_simple_db.h"
#include "ov12/rg_singleton_id.h"
#include "rg_robot_db.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_actor_charid.h"
#include "ov12/rg_error.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_read_text.h"
#include "ov12/xrg_v_log.h"

#define RG_ROBOT_DB_CAPACITY 0x20
#define RG_ROBOT_DB_ENTRY_SIZE 0x10
/* Bytes RgRobotDBRead allocates for each spec. */
#define RG_ROBOT_SPEC_ALLOC_SIZE 0xB8

extern const char robotDbCharIdHasNoNameFormat[]; /* "char id (%d) has no char-name" */

static void _WrapperDestruct(void *database)
{
    DisposeRgSimpleDB(database);
}

/* Returns the robot database singleton, creating and clearing it on first use. */
RgSimpleDB *InstanceOfRgRobotDB(void)
{
    RgSimpleDB *database;

    database = RgSingletonIDGet(RG_SINGLETON_ROBOT_DB);
    if (database == 0) {
        database = CreateRgSimpleDB(RG_ROBOT_DB_CAPACITY, RG_ROBOT_DB_ENTRY_SIZE);
        RgSingletonIDEntry(RG_SINGLETON_ROBOT_DB, database, _WrapperDestruct);
        RgRobotDBClear(database);
    }
    return database;
}

const char robotDbAssertDbNotNil[] = "pDB != NIL";
const char robotDbSourcePath[] = "../rg_robot_db.euc.c";

void RgRobotDBClear(RgSimpleDB *database)
{
    if (database == 0) {
        assert_prog(robotDbAssertDbNotNil, robotDbSourcePath, 49);
    }
    RgSimpleDBClear(database);
}

/* Returns the spec stored under name, or 0 when the database has no such entry. */
void *RgRobotDBGet(RgSimpleDB *database, const char *name)
{
    int dataID;

    if (database == 0) {
        assert_prog(robotDbAssertDbNotNil, robotDbSourcePath, 64);
    }
    dataID = RgSimpleDBFind(database, name);
    if (dataID >= 0) {
        return RgSimpleDBGet(database, dataID);
    }
    return 0;
}

/* Returns the spec of the character with charID, looked up through its name. Raises an error when the ID has no name. */
void *RgRobotDBGetByID(RgSimpleDB *database, int charID)
{
    const char *name;

    name = RgActorCharIDToName(charID);
    if (database == 0) {
        assert_prog(robotDbAssertDbNotNil, robotDbSourcePath, 76);
    }
    if (name == 0) {
        RgError(robotDbCharIdHasNoNameFormat, robotDbSourcePath, 78, charID);
    }
    return RgRobotDBGet(database, name);
}

/* Assertion, parse, and diagnostic strings used by this translation unit. */
const char robotDbCharIdHasNoNameFormat[] = "char id (%d) has no char-name";
const char robotDbAssertReaderNotNil[] = "pReader != NIL";
const char robotDbCharacterParagraphName[] = "character";
const char robotDbCharacterDefinedTwiceFormat[] = "character '%s' is defined more than twice";
const char robotDbCharacterNameUnusableFormat[] = "character name '%s' cannot be used";
const char robotDbDumpNameFormat[] = "NAME [%s] -------\n";

/* Returns a shared default robot spec, reset to its initial values on each call. */
RgRobotSpec *RgRobotDBGetDefault(void)
{
    static RgRobotSpec inSpec;

    InitRgRobotSpec(&inSpec);
    return &inSpec;
}

/* Reads every "character" paragraph from pReader, allocating a spec for each and adding it to the database. */
void RgRobotDBRead(RgSimpleDB *database, RgReadText *pReader)
{
    char szToken[0x80];
    RgRobotSpec *pSpec;
    int charID;

    if (database == 0) {
        assert_prog(robotDbAssertDbNotNil, robotDbSourcePath, 104);
    }
    if (pReader == 0) {
        assert_prog(robotDbAssertReaderNotNil, robotDbSourcePath, 105);
    }
    RgReadTextRewind(pReader);
    while (RgReadTextFindParagraph(pReader, robotDbCharacterParagraphName, 0) != 0) {
        RgReadTextGetString(pReader, szToken);
        if (RgRobotDBGet(database, szToken) != 0) {
            RgError(robotDbCharacterDefinedTwiceFormat, robotDbSourcePath, 119, szToken);
        }
        charID = RgActorNameToCharID(szToken);
        if (charID == -1) {
            RgError(robotDbCharacterNameUnusableFormat, robotDbSourcePath, 124, szToken);
        }
        pSpec = RgHeapAlloc(InstanceOfRgHeap(), RG_ROBOT_SPEC_ALLOC_SIZE, robotDbSourcePath, 127);
        InitRgRobotSpec(pSpec);
        RgRobotSpecReadFromText(pSpec, pReader);
        pSpec->charID = charID;
        RgSimpleDBEntry(database, pSpec, szToken);
    }
}

/* Logs each entry's name followed by its robot spec. */
void RgRobotDBDump(RgSimpleDB *database)
{
    unsigned int count;
    unsigned int dataID;
    char *name;

    if (database == 0) {
        assert_prog(robotDbAssertDbNotNil, robotDbSourcePath, 144);
    }
    count = RgSimpleDBSize(database);
    for (dataID = 0; dataID < count; dataID++) {
        name = RgSimpleDBGetName(database, dataID);
        XrgLog(robotDbDumpNameFormat, robotDbSourcePath, 147, name);
        RgRobotSpecDump(RgSimpleDBGet(database, dataID));
    }
}
