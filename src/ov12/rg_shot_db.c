/*
 * OV12 original TU 25: 0x00a18668..0x00a19388 (13 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "rg_shot_db.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

static void _EntryTemporariesShotDB(RgSimpleDB *database) {

}

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

extern int RgSimpleDBFind(RgSimpleDB *pDB, const char *pszName);
extern void *RgSimpleDBGet(RgSimpleDB *pDB, int nDataID);
extern void RgSimpleDBClear(RgSimpleDB *pDB);

void *RgShotDBGetEssence(RgSimpleDB *database, const char *name)
{
    int dataID;

    dataID = RgSimpleDBFind(database, name);
    if (dataID >= 0) {
        return RgSimpleDBGet(database, dataID);
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", _ReadCommon_00A18730);

/*
 * Opaque handle owned by ov12/tu036 (src/ov12/rg_read_text.c,
 * src/ov12/rg_read_text.h): this TU only forwards the pointer between
 * RgReadTextGetString/-GetFloat/-IsEOF/-Unget and its own _ReadCommon/
 * _ReadHomingMain helpers below, it never reads or writes a member of it.
 */
typedef struct RgReadText RgReadText;

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);
extern void RgReadTextGetString(RgReadText *pReader, char *pszOut);
extern float RgReadTextGetFloat(RgReadText *pReader);
extern int RgReadTextIsEOF(RgReadText *pReader);
extern void RgReadTextUnget(RgReadText *pReader, char *pszToken);
extern int strcmp(const char *s1, const char *s2);

/*
 * Scaffold-owned data (splat names, no config/symbols/ov12.txt entry).
 * ov12:0x00a53470 holds "../rg_shot_db.euc.c", the source file name
 * assert_prog/RgHeapAlloc report below. ov12:0x00a53520 holds
 * "pReader != NIL", the argument check every Read*Type function below
 * reports. ov12:0x00a53530 holds "spd", the text key _ReadNormal and
 * _ReadBeam both recognize for the shot speed.
 */
extern const char D_00A53470[];
extern const char D_00A53520[];
extern const char D_00A53530[];

/*
 * _ReadCommon is one of two file-local statics of that name in this
 * overlay (the other, ov12:0x00a1cd08, belongs to a different translation
 * unit). This one, ov12:0x00a18730, is still INCLUDE_ASM above and shared
 * by _ReadNormal and _ReadBeam below: it consumes the token they already
 * read and reports (nonzero) whether it recognized a field common to every
 * essence type.
 */
static int _ReadCommon(RgReadText *pReader, void *essence, char *pszToken);

/*
 * Partial view of a normal-shot essence beyond RgHeapAlloc's own 0x130-byte
 * allocation: only the float the "spd" key above writes below, at the same
 * +0x120 offset ov12/tu023 (src/ov12/rg_shot.c) independently completes as
 * RgNormalShotEssence.speed.
 */
typedef struct RgShotDbNormalEssence RgShotDbNormalEssence;
struct RgShotDbNormalEssence {
    unsigned char unmodeled_000[0x120];
    float speed;                 /* +0x120, the "spd" key above */
};

extern void InitRgNormalShotEssence(RgShotDbNormalEssence *essence);

static RgShotDbNormalEssence *_ReadNormal(RgReadText *pReader)
{
    char szToken[0x80];
    RgShotDbNormalEssence *pEss;

    pEss = RgHeapAlloc(InstanceOfRgHeap(), 0x130, D_00A53470, 147);
    if (pReader == 0) {
        assert_prog(D_00A53520, D_00A53470, 149);
    }
    InitRgNormalShotEssence(pEss);
    for (;;) {
        if (RgReadTextIsEOF(pReader) == 0) {
            RgReadTextGetString(pReader, szToken);
            if (_ReadCommon(pReader, pEss, szToken) == 0) {
                if (strcmp(szToken, D_00A53530) == 0) {
                    pEss->speed = RgReadTextGetFloat(pReader);
                    continue;
                } else {
                    RgReadTextUnget(pReader, szToken);
                }
            } else {
                continue;
            }
        }
        break;
    }

    return pEss;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", _ReadHomingMain);

/*
 * RgHomingShotEssence is fully evidenced and completed by ov12/tu023
 * (src/ov12/rg_shot.c, InitRgHomingShotEssence's own field writes); this
 * TU only allocates and forwards the pointer, so it stays opaque here.
 */
typedef struct RgHomingShotEssence RgHomingShotEssence;

extern void InitRgHomingShotEssence(RgHomingShotEssence *essence);

/*
 * _ReadHomingMain (ov12:0x00a18c50's sibling, still INCLUDE_ASM above)
 * consumes the token _ReadHoming already read and reports (nonzero) whether
 * it recognized a homing-specific field.
 */
static int _ReadHomingMain(RgReadText *pReader, RgHomingShotEssence *essence,
                           char *pszToken);

static RgHomingShotEssence *_ReadHoming(RgReadText *pReader)
{
    char szToken[0x80];
    RgHomingShotEssence *pEss;

    pEss = RgHeapAlloc(InstanceOfRgHeap(), 0x130, D_00A53470, 192);
    if (pReader == 0) {
        assert_prog(D_00A53520, D_00A53470, 194);
    }
    InitRgHomingShotEssence(pEss);
    for (;;) {
        if (RgReadTextIsEOF(pReader) == 0) {
            RgReadTextGetString(pReader, szToken);
            if (_ReadHomingMain(pReader, pEss, szToken) == 0) {
                RgReadTextUnget(pReader, szToken);
            } else {
                continue;
            }
        }
        break;
    }

    return pEss;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", _ReadGrenade);

/*
 * ov12:0x00a535a0 holds "attach-time", the text key _ReadBeam recognizes
 * below for the beam's attach delay.
 */
extern const char D_00A535A0[];

/*
 * Partial view of a beam essence beyond RgHeapAlloc's own 0x130-byte
 * allocation: the two floats the "spd"/"attach-time" keys above write
 * below, at the same +0x120/+0x124 offsets ov12/tu023 (src/ov12/rg_shot.c)
 * independently completes as RgBeamEssence.speed/.duration.
 */
typedef struct RgShotDbBeamEssence RgShotDbBeamEssence;
struct RgShotDbBeamEssence {
    unsigned char unmodeled_000[0x120];
    float speed;                 /* +0x120, the "spd" key above */
    float attachTime;            /* +0x124, the "attach-time" key above */
};

extern void InitRgBeamEssence(RgShotDbBeamEssence *essence);

static RgShotDbBeamEssence *_ReadBeam(RgReadText *pReader)
{
    char szToken[0x80];
    RgShotDbBeamEssence *pEss;

    pEss = RgHeapAlloc(InstanceOfRgHeap(), 0x130, D_00A53470, 241);
    if (pReader == 0) {
        assert_prog(D_00A53520, D_00A53470, 243);
    }
    InitRgBeamEssence(pEss);
    while (RgReadTextIsEOF(pReader) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (_ReadCommon(pReader, pEss, szToken) == 0) {
            if (strcmp(szToken, D_00A53530) == 0) {
                pEss->speed = RgReadTextGetFloat(pReader);
                continue;
            } else {
                if (strcmp(szToken, D_00A535A0) == 0) {
                    pEss->attachTime = RgReadTextGetFloat(pReader);
                    continue;
                }
                RgReadTextUnget(pReader, szToken);
            }
            break;
        }
    }

    return pEss;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", _ReadFire);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", RgShotDBRead);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a535d8 contains the assertion expression "pDB != NIL".
 * ov12:0x00a53470 contains the source filename "../rg_shot_db.euc.c".
 */
extern const char D_00A535D8[];
extern const char D_00A53470[];

void RgShotDBClear(RgSimpleDB *database)
{
    if (database == 0) {
        assert_prog(D_00A535D8, D_00A53470, 346);
    }
    RgSimpleDBClear(database);
    _EntryTemporariesShotDB(database);
}
