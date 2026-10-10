/*
 * OV12 original TU 25: 0x00a18668..0x00a19388 (13 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "ov12/rg_shot.h"
#include "rg_shot_db.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
struct RgReadText;
extern void RgReadTextGetString(struct RgReadText *pReader, char *pszOut);
extern float RgReadTextGetFloat(struct RgReadText *pReader);
extern int RgReadTextGetInt(struct RgReadText *pReader);
extern void RgWarn(const char *format, const char *source_file, int line, ...);
extern char *strcpy(char *destination, const char *source);
extern int strcmp(const char *s1, const char *s2);
extern const char D_00A53450[];
extern const char D_00A53458[];
extern const char D_00A53470[];
extern const char D_00A53488[];
extern const char D_00A53490[];
extern const char D_00A53498[];
extern const char D_00A534A0[];
extern const char D_00A534B0[];
extern const char D_00A534C0[];
extern const char D_00A534D0[];
extern const char D_00A534D8[];
extern const char D_00A534E0[];
extern const char D_00A534F0[];
extern const char D_00A53500[];
extern const char D_00A53510[];
extern const char D_00A53538[];
extern const char D_00A53548[];
extern const char D_00A53550[];
extern const char D_00A53560[];

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

/* Common settings are addressed through every shot-essence allocation. */
typedef struct RgShotDbCommonFields RgShotDbCommonFields;
struct RgShotDbCommonFields {
    void *(*createFunc)(void *essence, void *info);
    float damage;
    float life;
    char modelVariant[0x40];
    char particleFile[0x40];
    char texLineFile[0x20];
    float texLineWidth;
    float texLineHeight;
    unsigned char unmodeled_0b4[0x0c];
    int texLineColor[3];
    unsigned char unmodeled_0cc[4];
    char hitEffectFile[0x20];
    char damageParticleFile[0x20];
    float notDamLimit;
    float notDamTime;
    float slowTime;
    float slowRate;
};

static int _ReadCommon(struct RgReadText *pReader, void *essence,
                       char *pszToken)
{
    float defaultNoDamageTime;
    char value[0x80];
    RgShotDbCommonFields *shotEssence;

    shotEssence = essence;
    if (strcmp(pszToken, D_00A53450) == 0) {
        RgReadTextGetString(pReader, value);
        RgWarn(D_00A53458, D_00A53470, 87, value);
    } else if (strcmp(pszToken, D_00A53488) == 0) {
        RgReadTextGetString(pReader, value);
        strcpy(shotEssence->modelVariant, value);
    } else if (strcmp(pszToken, D_00A53490) == 0) {
        RgReadTextGetString(pReader, value);
        strcpy(shotEssence->particleFile, value);
    } else if (strcmp(pszToken, D_00A53498) == 0) {
        RgReadTextGetString(pReader, value);
        strcpy(shotEssence->texLineFile, value);
    } else if (strcmp(pszToken, D_00A534A0) == 0) {
        RgReadTextGetString(pReader, value);
        strcpy(shotEssence->hitEffectFile, value);
    } else if (strcmp(pszToken, D_00A534B0) == 0) {
        RgReadTextGetString(pReader, value);
        strcpy(shotEssence->damageParticleFile, value);
    } else if (strcmp(pszToken, D_00A534C0) == 0) {
        RgReadTextGetString(pReader, value);
        strcpy(shotEssence->texLineFile, value);
        shotEssence->texLineWidth = RgReadTextGetFloat(pReader);
        shotEssence->texLineHeight = RgReadTextGetFloat(pReader);
        shotEssence->texLineColor[0] = RgReadTextGetInt(pReader);
        shotEssence->texLineColor[1] = RgReadTextGetInt(pReader);
        shotEssence->texLineColor[2] = RgReadTextGetInt(pReader);
    } else if (strcmp(pszToken, D_00A534D0) == 0) {
        shotEssence->damage = RgReadTextGetFloat(pReader);
    } else if (strcmp(pszToken, D_00A534D8) == 0) {
        shotEssence->life = RgReadTextGetFloat(pReader);
    } else if (strcmp(pszToken, D_00A534E0) == 0) {
        shotEssence->notDamLimit = RgReadTextGetFloat(pReader);
        defaultNoDamageTime = 1.0f;
        shotEssence->slowTime = 0.5f;
        shotEssence->notDamTime = defaultNoDamageTime;
        shotEssence->slowRate = 0.5f;
    } else if (strcmp(pszToken, D_00A534F0) == 0) {
        shotEssence->notDamTime = RgReadTextGetFloat(pReader);
    } else if (strcmp(pszToken, D_00A53500) == 0) {
        shotEssence->slowTime = RgReadTextGetFloat(pReader);
    } else if (strcmp(pszToken, D_00A53510) == 0) {
        shotEssence->slowRate = RgReadTextGetFloat(pReader);
    } else {
        return 0;
    }
    return 1;
}

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


/*
 * Scaffold-owned data (splat names, no config/symbols/ov12.txt entry).
 * ov12:0x00a53470 holds "../rg_shot_db.euc.c", the source file name
 * assert_prog/RgHeapAlloc report below. ov12:0x00a53520 holds
 * "pReader != NIL", the argument check every Read*Type function below
 * reports. ov12:0x00a53530 holds "spd", the text key _ReadNormal and
 * _ReadBeam both recognize for the shot speed.
 */

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

static int _ReadHomingMain(RgReadText *pReader,
                           struct RgHomingShotEssence *essence,
                           char *pszToken)
{
    if (essence == 0) {
        assert_prog(D_00A53538, D_00A53470, 169);
    }
    if (pReader == 0) {
        assert_prog(D_00A53520, D_00A53470, 170);
    }

    if (_ReadCommon(pReader, essence, pszToken) != 0) {
        return 1;
    }
    if (strcmp(pszToken, D_00A53530) == 0) {
        essence->speed = RgReadTextGetFloat(pReader);
    } else if (strcmp(pszToken, D_00A53548) == 0) {
        essence->turnRate = 3.1415927f / RgReadTextGetFloat(pReader);
    } else if (strcmp(pszToken, D_00A53550) == 0) {
        essence->cosThreshold = RgReadTextGetFloat(pReader) * 3.1415927f / 180.0f;
    } else if (strcmp(pszToken, D_00A53560) == 0) {
        essence->delay = RgReadTextGetFloat(pReader);
    } else {
        return 0;
    }
    return 1;
}

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

/* The grenade initializer is defined in ov12/tu023; its essence is opaque here. */
struct RgGrenadeEssence;
extern void InitRgGrenadeEssence(struct RgGrenadeEssence *essence);
extern int strcasecmp(const char *s1, const char *s2);
extern const char D_00A53568[];
extern const char D_00A53578[];
extern const char D_00A53588[];
extern const char D_00A53598[];

typedef struct RgShotDbGrenadeEssence RgShotDbGrenadeEssence;
struct RgShotDbGrenadeEssence {
    RgHomingShotEssence homing;
    float explosionRadius; /* +0x130, the "bom-size" key */
    float duration;        /* +0x134, the "bom-time" key */
    float proximity;       /* +0x138, the "bom-dist" key */
    float repeatCount;     /* +0x13c, the "chaff" key */
};

static struct RgGrenadeEssence *_ReadGrenade(RgReadText *pReader)
{
    char szToken[0x80];
    void *essenceStorage;
    struct RgGrenadeEssence *grenade;
    RgHomingShotEssence *homingEssence;
    RgShotDbGrenadeEssence *parsedEssence;

    essenceStorage = RgHeapAlloc(InstanceOfRgHeap(), 0x140, D_00A53470, 212);
    grenade = essenceStorage;
    homingEssence = essenceStorage;
    parsedEssence = essenceStorage;
    if (pReader == 0) {
        assert_prog(D_00A53520, D_00A53470, 214);
    }
    InitRgGrenadeEssence(grenade);

    while (RgReadTextIsEOF(pReader) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (_ReadHomingMain(pReader, homingEssence, szToken) != 0) {
            continue;
        }
        if (strcasecmp(szToken, D_00A53568) == 0) {
            parsedEssence->explosionRadius = RgReadTextGetFloat(pReader);
            continue;
        }
        if (strcasecmp(szToken, D_00A53578) == 0) {
            parsedEssence->duration = RgReadTextGetFloat(pReader);
            continue;
        }
        if (strcasecmp(szToken, D_00A53588) == 0) {
            parsedEssence->proximity = RgReadTextGetFloat(pReader);
            continue;
        }
        if (strcasecmp(szToken, D_00A53598) == 0) {
            parsedEssence->repeatCount = RgReadTextGetFloat(pReader);
            continue;
        }
        RgReadTextUnget(pReader, szToken);
        break;
    }
    return grenade;
}

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

/* The keys read by the fire essence parser are scaffold-owned string data. */
extern const char D_00A535B0[];
extern const char D_00A535B8[];
extern const char D_00A535C8[];

typedef struct RgShotDbFireEssence RgShotDbFireEssence;
struct RgShotDbFireEssence {
    unsigned char unmodeled_000[0x120];
    float length;       /* +0x120, the "length" key */
    float attachTime;   /* +0x124, the "attach-time" key */
    int immDead;        /* +0x128, set by the "imm-dead" key */
    int onceHit;        /* +0x12c, set by the "once-hit" key */
};

extern void InitRgFireEssence(void *essence);

static RgShotDbFireEssence *_ReadFire(RgReadText *pReader)
{
    char szToken[0x80];
    RgShotDbFireEssence *essence;

    essence = RgHeapAlloc(InstanceOfRgHeap(), 0x130, D_00A53470, 265);
    if (pReader == 0) {
        assert_prog(D_00A53520, D_00A53470, 267);
    }
    InitRgFireEssence(essence);

    while (RgReadTextIsEOF(pReader) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (_ReadCommon(pReader, essence, szToken) != 0) {
            continue;
        }
        if (strcmp(szToken, D_00A535B0) == 0) {
            essence->length = RgReadTextGetFloat(pReader);
            continue;
        }
        if (strcmp(szToken, D_00A535A0) == 0) {
            essence->attachTime = RgReadTextGetFloat(pReader);
            continue;
        }
        if (strcmp(szToken, D_00A535B8) == 0) {
            essence->immDead = 1;
            continue;
        }
        if (strcmp(szToken, D_00A535C8) == 0) {
            essence->onceHit = 1;
            continue;
        }
        RgReadTextUnget(pReader, szToken);
        break;
    }
    return essence;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_db", RgShotDBRead);

extern const char D_00A535D8[];


void RgShotDBClear(RgSimpleDB *database)
{
    if (database == 0) {
        assert_prog(D_00A535D8, D_00A53470, 346);
    }
    RgSimpleDBClear(database);
    _EntryTemporariesShotDB(database);
}

const char D_00A53450[8] = "shot";
const char D_00A53458[24] = "ignored 'shot %s'";
const char D_00A53470[24] = "../rg_shot_db.euc.c";
const char D_00A53488[8] = "model";
const char D_00A53490[8] = "ptcl";
const char D_00A53498[8] = "puttex";
const char D_00A534A0[16] = "break-ptcl";
const char D_00A534B0[16] = "damage-ptcl";
const char D_00A534C0[16] = "puttex-all";
const char D_00A534D0[8] = "damage";
const char D_00A534D8[8] = "life";
const char D_00A534E0[16] = "not-dam-limit";
const char D_00A534F0[16] = "not-dam-time";
const char D_00A53500[16] = "slow-time";
const char D_00A53510[16] = "slow-rate";
const char D_00A53520[16] = "pReader != NIL";
const char D_00A53530[8] = "spd";
const char D_00A53538[16] = "pEss != NIL";
const char D_00A53548[8] = "aim";
const char D_00A53550[16] = "accuracy";
const char D_00A53560[8] = "wait";
const char D_00A53568[16] = "bom-size";
const char D_00A53578[16] = "bom-time";
const char D_00A53588[16] = "bom-dist";
const char D_00A53598[8] = "chaff";
const char D_00A535A0[16] = "attach-time";
const char D_00A535B0[8] = "length";
const char D_00A535B8[16] = "imm-dead";
const char D_00A535C8[16] = "once-hit";
const char D_00A535D8[16] = "pDB != NIL";
