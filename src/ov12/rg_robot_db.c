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

const char D_00A52280[] = "pDB != NIL";
const char D_00A52290[] = "../rg_robot_db.euc.c";

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

/* Assertion, parse, and diagnostic strings used by this translation unit. */
const char D_00A522A8[] = "char id (%d) has no char-name";
const char D_00A522C8[] = "pReader != NIL";
const char D_00A522D8[] = "character";
const char D_00A522E8[] = "character '%s' is defined more than twice";
const char D_00A52318[] = "character name '%s' cannot be used";
const char D_00A52340[] = "NAME [%s] -------\n";

typedef struct RgRobotSpec {
    int charID;
    int type;
    unsigned char unmodeled_08[4];
    float baseSpeed;
    unsigned char unmodeled_10[4];
    float turnRate;
    unsigned char unmodeled_18[4];
    float speedRating;
    float accelRate;
    float rotateForce;
    float moveResist;
    float rotResist;
    unsigned char unmodeled_30[4];
    float attackAdvanceRate;
    unsigned char unmodeled_38[8];
    int unmodeled_40;
    float unmodeled_44;
    int unmodeled_48;
    float unmodeled_4c;
    float unmodeled_50;
    float unmodeled_54;
    unsigned char unmodeled_58;
    unsigned char unmodeled_59[0x1f];
    unsigned char unmodeled_78;
    unsigned char unmodeled_79[0x1f];
    unsigned char unmodeled_98[8];
    unsigned char unmodeled_a0[0x20];
} RgRobotSpec;

extern void InitRgRobotSpec(RgRobotSpec *pSpec);

RgRobotSpec *RgRobotDBGetDefault(void)
{
    static RgRobotSpec inSpec;

    InitRgRobotSpec(&inSpec);
    return &inSpec;
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

extern int RgSimpleDBSize(RgSimpleDB *pDB);
extern char *RgSimpleDBGetName(RgSimpleDB *pDB, int nDataID);
extern void XrgLog(const char *format, const char *source_file, int line,
                   ...);
extern void RgRobotSpecDump(RgRobotSpec *pSpec);

void RgRobotDBDump(RgSimpleDB *database)
{
    unsigned int count;
    unsigned int dataID;
    char *name;

    if (database == 0) {
        assert_prog(D_00A52280, D_00A52290, 144);
    }
    count = RgSimpleDBSize(database);
    for (dataID = 0; dataID < count; dataID++) {
        name = RgSimpleDBGetName(database, dataID);
        XrgLog(D_00A52340, D_00A52290, 147, name);
        RgRobotSpecDump(RgSimpleDBGet(database, dataID));
    }
}
