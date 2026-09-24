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

/* ov12/tu067 (src/ov12/rg_actor_charid.c), already accepted there. */
extern int RgActorNameToCharID(const char *pszName);

/*
 * ov12/tu036 (src/ov12/rg_read_text.h). RgReadTextRewind and
 * RgReadTextGetString are already accepted there; RgReadTextFindParagraph
 * is still INCLUDE_ASM.
 */
typedef struct RgReadText RgReadText;

extern void RgReadTextRewind(RgReadText *pReader);
extern void RgReadTextGetString(RgReadText *pReader, char *pszOut);
extern int RgReadTextFindParagraph(RgReadText *pReader, const char *pszTag,
                                   const char *pszSubTag);
/* ov12/tu019 (src/ov12/rg_heap.h), still INCLUDE_ASM there. */
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);
/* ov12/tu008 (src/ov12/rg_robot_spec.c), still INCLUDE_ASM there. */
extern void RgRobotSpecReadFromText(RgRobotSpec *pSpec, RgReadText *pReader);
/* ov12/tu022 (src/ov12/rg_simple_db.c), already accepted there. */
extern void RgSimpleDBEntry(RgSimpleDB *pDB, void *pDat, const char *pszName);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a522c8 contains the assertion expression "pReader != NIL".
 * ov12:0x00a522d8 contains the paragraph tag "character".
 * ov12:0x00a522e8 contains the format string
 *   "character '%s' is defined more than twice".
 * ov12:0x00a52318 contains the format string
 *   "character name '%s' cannot be used".
 */
extern const char D_00A522C8[];
extern const char D_00A522D8[];
extern const char D_00A522E8[];
extern const char D_00A52318[];

void RgRobotDBRead(RgSimpleDB *database, RgReadText *pReader)
{
    char szToken[0x80];
    RgRobotSpec *pSpec;
    int charID;

    if (database == 0) {
        assert_prog(D_00A52280, D_00A52290, 104);
    }
    if (pReader == 0) {
        assert_prog(D_00A522C8, D_00A52290, 105);
    }
    RgReadTextRewind(pReader);
    while (RgReadTextFindParagraph(pReader, D_00A522D8, 0) != 0) {
        RgReadTextGetString(pReader, szToken);
        if (RgRobotDBGet(database, szToken) != 0) {
            RgError(D_00A522E8, D_00A52290, 119, szToken);
        }
        charID = RgActorNameToCharID(szToken);
        if (charID == -1) {
            RgError(D_00A52318, D_00A52290, 124, szToken);
        }
        pSpec = RgHeapAlloc(InstanceOfRgHeap(), 0xB8, D_00A52290, 127);
        InitRgRobotSpec(pSpec);
        RgRobotSpecReadFromText(pSpec, pReader);
        /*
         * RgRobotSpec's own character id (RgActorNameToCharID's result):
         * sw s1,0(s0) at ov12:0x00a0de5c, right after RgRobotSpecReadFromText
         * and before RgSimpleDBEntry. RgRobotSpec is owned by ov12/tu004
         * (src/ov12/rg_robot.h), which has not named this field yet
         * (proposed: charID at +0x00, tools/header_types.py propose).
         */
        *(int *) pSpec = charID;
        RgSimpleDBEntry(database, pSpec, szToken);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_robot_db", RgRobotDBDump);
