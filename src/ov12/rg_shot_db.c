/*
 * OV12 original TU 25: 0x00a18668..0x00a19388 (13 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_read_text.h"
#include "ov12/rg_shot_db.h"
#include "ov12/rg_simple_db.h"
#include "ov12/rg_singleton_id.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/rg_error.h"

/* The shot database's initial capacity and the size of one entry. */
#define RG_SHOT_DB_CAPACITY 32
#define RG_SHOT_DB_ENTRY_SIZE 16


/* The text keys and messages of the shot database file, defined below. */
extern const char shotDbShotToken[];
extern const char shotDbIgnoredShotFormat[];
extern const char shotDbSourcePath[];
extern const char shotDbModelToken[];
extern const char shotDbParticleToken[];
extern const char shotDbPutTextureToken[];
extern const char shotDbBreakParticleKeyword[];
extern const char shotDbDamageParticleKeyword[];
extern const char shotDbPutTextureAllKeyword[];
extern const char shotDbDamageKeyword[];
extern const char shotDbLifeKeyword[];
extern const char shotDbNotDamageLimitKeyword[];
extern const char shotDbNotDamageTimeKeyword[];
extern const char shotDbSlowTimeKeyword[];
extern const char shotDbSlowRateKeyword[];
extern const char shotDbAssertNameNotNil[];
extern const char shotDbSpeedKeyword[];
extern const char shotDbAssertEssenceNotNil[];
extern const char shotDbAimKeyword[];
extern const char shotDbAccuracyKeyword[];
extern const char shotDbWaitKeyword[];
extern const char shotDbBombSizeKeyword[];
extern const char shotDbBombTimeKeyword[];
extern const char shotDbBombDistanceKeyword[];
extern const char shotDbChaffKeyword[];
extern const char shotDbAttachTimeKeyword[];
extern const char shotDbLengthKeyword[];
extern const char shotDbImmediateDeadKeyword[];
extern const char shotDbOnceHitKeyword[];
extern const char shotDbAssertDbNotNil[];
extern const char shotDbAssertDataBaseNameNotNil[];
extern const char shotDbNormalTypeKeyword[];
extern const char shotDbHomingTypeKeyword[];
extern const char shotDbGrenadeTypeKeyword[];
extern const char shotDbBeamTypeKeyword[];
extern const char shotDbFireTypeKeyword[];
extern const char shotDbUnknownKeywordFormat[];

static void _EntryTemporariesShotDB(RgSimpleDB *database) {

}

static void _WrapperDestruct(void *database)
{
    DisposeRgSimpleDB(database);
}

RgSimpleDB *InstanceOfRgShotDB(void)
{
    RgSimpleDB *shot_database;

    shot_database = RgSingletonIDGet(RG_SINGLETON_SHOT_DB);
    if (shot_database == 0) {
        shot_database = CreateRgSimpleDB(RG_SHOT_DB_CAPACITY,
                                         RG_SHOT_DB_ENTRY_SIZE);
        _EntryTemporariesShotDB(shot_database);
        RgSingletonIDEntry(RG_SINGLETON_SHOT_DB, shot_database, _WrapperDestruct);
    }
    return shot_database;
}

RgShotEssence *RgShotDBGetEssence(RgSimpleDB *database, const char *name)
{
    int dataID;

    dataID = RgSimpleDBFind(database, name);
    if (dataID >= 0) {
        return RgSimpleDBGet(database, dataID);
    }
    return 0;
}

/* Reads the keys every essence type shares; nonzero when the token was one. */
static int _ReadCommon(RgReadText *pReader, RgShotEssence *shotEssence,
                       char *pszToken)
{
    float defaultNoDamageTime;
    char value[0x80];

    if (strcmp(pszToken, shotDbShotToken) == 0) {
        RgReadTextGetString(pReader, value);
        RgWarn(shotDbIgnoredShotFormat, shotDbSourcePath, 87, value);
    } else if (strcmp(pszToken, shotDbModelToken) == 0) {
        RgReadTextGetString(pReader, value);
        strcpy(shotEssence->modelVariant, value);
    } else if (strcmp(pszToken, shotDbParticleToken) == 0) {
        RgReadTextGetString(pReader, value);
        strcpy(shotEssence->particleFile, value);
    } else if (strcmp(pszToken, shotDbPutTextureToken) == 0) {
        RgReadTextGetString(pReader, value);
        strcpy(shotEssence->texLineFile, value);
    } else if (strcmp(pszToken, shotDbBreakParticleKeyword) == 0) {
        RgReadTextGetString(pReader, value);
        strcpy(shotEssence->hitEffectFile, value);
    } else if (strcmp(pszToken, shotDbDamageParticleKeyword) == 0) {
        RgReadTextGetString(pReader, value);
        strcpy(shotEssence->damageParticleFile, value);
    } else if (strcmp(pszToken, shotDbPutTextureAllKeyword) == 0) {
        RgReadTextGetString(pReader, value);
        strcpy(shotEssence->texLineFile, value);
        shotEssence->texLineWidth = RgReadTextGetFloat(pReader);
        shotEssence->texLineHeight = RgReadTextGetFloat(pReader);
        shotEssence->texLineColor.r = RgReadTextGetInt(pReader);
        shotEssence->texLineColor.g = RgReadTextGetInt(pReader);
        shotEssence->texLineColor.b = RgReadTextGetInt(pReader);
    } else if (strcmp(pszToken, shotDbDamageKeyword) == 0) {
        shotEssence->damage = RgReadTextGetFloat(pReader);
    } else if (strcmp(pszToken, shotDbLifeKeyword) == 0) {
        shotEssence->life = RgReadTextGetFloat(pReader);
    } else if (strcmp(pszToken, shotDbNotDamageLimitKeyword) == 0) {
        shotEssence->notDamLimit = RgReadTextGetFloat(pReader);
        defaultNoDamageTime = 1.0f;
        shotEssence->slowTime = 0.5f;
        shotEssence->notDamTime = defaultNoDamageTime;
        shotEssence->slowRate = 0.5f;
    } else if (strcmp(pszToken, shotDbNotDamageTimeKeyword) == 0) {
        shotEssence->notDamTime = RgReadTextGetFloat(pReader);
    } else if (strcmp(pszToken, shotDbSlowTimeKeyword) == 0) {
        shotEssence->slowTime = RgReadTextGetFloat(pReader);
    } else if (strcmp(pszToken, shotDbSlowRateKeyword) == 0) {
        shotEssence->slowRate = RgReadTextGetFloat(pReader);
    } else {
        return 0;
    }
    return 1;
}

static RgNormalShotEssence *_ReadNormal(RgReadText *pReader)
{
    char szToken[0x80];
    RgNormalShotEssence *pEss;

    pEss = RgHeapAlloc(InstanceOfRgHeap(), RG_SHOT_ESSENCE_SIZE, shotDbSourcePath,
                       147);
    if (pReader == 0) {
        assert_prog(shotDbAssertNameNotNil, shotDbSourcePath, 149);
    }
    InitRgNormalShotEssence(pEss);
    for (;;) {
        if (RgReadTextIsEOF(pReader) == 0) {
            RgReadTextGetString(pReader, szToken);
            if (_ReadCommon(pReader, &pEss->common, szToken) == 0) {
                if (strcmp(szToken, shotDbSpeedKeyword) == 0) {
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

/* Reads the keys of the homing essences; nonzero when the token was one. */
static int _ReadHomingMain(RgReadText *pReader, RgHomingShotEssence *essence,
                           char *pszToken)
{
    if (essence == 0) {
        assert_prog(shotDbAssertEssenceNotNil, shotDbSourcePath, 169);
    }
    if (pReader == 0) {
        assert_prog(shotDbAssertNameNotNil, shotDbSourcePath, 170);
    }

    if (_ReadCommon(pReader, &essence->common, pszToken) != 0) {
        return 1;
    }
    if (strcmp(pszToken, shotDbSpeedKeyword) == 0) {
        essence->speed = RgReadTextGetFloat(pReader);
    } else if (strcmp(pszToken, shotDbAimKeyword) == 0) {
        essence->turnRate = XRG_PI / RgReadTextGetFloat(pReader);
    } else if (strcmp(pszToken, shotDbAccuracyKeyword) == 0) {
        essence->cosThreshold = RgReadTextGetFloat(pReader) * XRG_PI / 180.0f;
    } else if (strcmp(pszToken, shotDbWaitKeyword) == 0) {
        essence->delay = RgReadTextGetFloat(pReader);
    } else {
        return 0;
    }
    return 1;
}

static RgHomingShotEssence *_ReadHoming(RgReadText *pReader)
{
    char szToken[0x80];
    RgHomingShotEssence *pEss;

    pEss = RgHeapAlloc(InstanceOfRgHeap(), RG_SHOT_ESSENCE_SIZE, shotDbSourcePath,
                       192);
    if (pReader == 0) {
        assert_prog(shotDbAssertNameNotNil, shotDbSourcePath, 194);
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

static RgGrenadeEssence *_ReadGrenade(RgReadText *pReader)
{
    char szToken[0x80];
    RgGrenadeEssence *grenade;

    grenade = RgHeapAlloc(InstanceOfRgHeap(), RG_GRENADE_ESSENCE_SIZE,
                          shotDbSourcePath, 212);
    if (pReader == 0) {
        assert_prog(shotDbAssertNameNotNil, shotDbSourcePath, 214);
    }
    InitRgGrenadeEssence(grenade);

    while (RgReadTextIsEOF(pReader) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (_ReadHomingMain(pReader, &grenade->homing, szToken) != 0) {
            continue;
        }
        if (strcasecmp(szToken, shotDbBombSizeKeyword) == 0) {
            grenade->explosionRadius = RgReadTextGetFloat(pReader);
            continue;
        }
        if (strcasecmp(szToken, shotDbBombTimeKeyword) == 0) {
            grenade->duration = RgReadTextGetFloat(pReader);
            continue;
        }
        if (strcasecmp(szToken, shotDbBombDistanceKeyword) == 0) {
            grenade->proximity = RgReadTextGetFloat(pReader);
            continue;
        }
        if (strcasecmp(szToken, shotDbChaffKeyword) == 0) {
            grenade->confuseTime = RgReadTextGetFloat(pReader);
            continue;
        }
        RgReadTextUnget(pReader, szToken);
        break;
    }
    return grenade;
}

static RgBeamEssence *_ReadBeam(RgReadText *pReader)
{
    char szToken[0x80];
    RgBeamEssence *pEss;

    pEss = RgHeapAlloc(InstanceOfRgHeap(), RG_SHOT_ESSENCE_SIZE, shotDbSourcePath,
                       241);
    if (pReader == 0) {
        assert_prog(shotDbAssertNameNotNil, shotDbSourcePath, 243);
    }
    InitRgBeamEssence(pEss);
    while (RgReadTextIsEOF(pReader) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (_ReadCommon(pReader, &pEss->common, szToken) == 0) {
            if (strcmp(szToken, shotDbSpeedKeyword) == 0) {
                pEss->speed = RgReadTextGetFloat(pReader);
                continue;
            } else {
                if (strcmp(szToken, shotDbAttachTimeKeyword) == 0) {
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

static RgFireEssence *_ReadFire(RgReadText *pReader)
{
    char szToken[0x80];
    RgFireEssence *essence;

    essence = RgHeapAlloc(InstanceOfRgHeap(), RG_SHOT_ESSENCE_SIZE,
                          shotDbSourcePath, 265);
    if (pReader == 0) {
        assert_prog(shotDbAssertNameNotNil, shotDbSourcePath, 267);
    }
    InitRgFireEssence(essence);

    while (RgReadTextIsEOF(pReader) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (_ReadCommon(pReader, &essence->common, szToken) != 0) {
            continue;
        }
        if (strcmp(szToken, shotDbLengthKeyword) == 0) {
            essence->length = RgReadTextGetFloat(pReader);
            continue;
        }
        if (strcmp(szToken, shotDbAttachTimeKeyword) == 0) {
            essence->attachTime = RgReadTextGetFloat(pReader);
            continue;
        }
        if (strcmp(szToken, shotDbImmediateDeadKeyword) == 0) {
            essence->immDead = 1;
            continue;
        }
        if (strcmp(szToken, shotDbOnceHitKeyword) == 0) {
            essence->onceHit = 1;
            continue;
        }
        RgReadTextUnget(pReader, szToken);
        break;
    }
    return essence;
}

void RgShotDBRead(RgSimpleDB *pDB, const char *pszDataBase)
{
    char szToken[0x100];
    RgReadText *pReader;
    void *pEntry;

    if (pDB == 0) {
        assert_prog(shotDbAssertDbNotNil, shotDbSourcePath, 297);
    }
    if (pszDataBase == 0) {
        assert_prog(shotDbAssertDataBaseNameNotNil, shotDbSourcePath, 298);
    }

    pReader = CreateRgReadText(pszDataBase);
    if (pReader != 0) {
        while (RgReadTextIsEOF(pReader) == 0) {
            pEntry = 0;
            RgReadTextGetString(pReader, szToken);

            if (strcmp(szToken, shotDbNormalTypeKeyword) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = _ReadNormal(pReader);
            } else if (strcmp(szToken, shotDbHomingTypeKeyword) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = _ReadHoming(pReader);
            } else if (strcmp(szToken, shotDbGrenadeTypeKeyword) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = _ReadGrenade(pReader);
            } else if (strcmp(szToken, shotDbBeamTypeKeyword) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = _ReadBeam(pReader);
            } else if (strcmp(szToken, shotDbFireTypeKeyword) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = _ReadFire(pReader);
            } else {
                RgError(shotDbUnknownKeywordFormat, shotDbSourcePath, 334, pszDataBase, szToken);
            }

            if (pEntry != 0) {
                RgSimpleDBEntry(pDB, pEntry, szToken);
            }
        }
        DisposeRgReadText(pReader);
    }
}

void RgShotDBClear(RgSimpleDB *database)
{
    if (database == 0) {
        assert_prog(shotDbAssertDbNotNil, shotDbSourcePath, 346);
    }
    RgSimpleDBClear(database);
    _EntryTemporariesShotDB(database);
}

const char shotDbShotToken[8] = "shot";

const char shotDbIgnoredShotFormat[24] = "ignored 'shot %s'";

const char shotDbSourcePath[24] = "../rg_shot_db.euc.c";

const char shotDbModelToken[8] = "model";

const char shotDbParticleToken[8] = "ptcl";

const char shotDbPutTextureToken[8] = "puttex";

const char shotDbBreakParticleKeyword[16] = "break-ptcl";

const char shotDbDamageParticleKeyword[16] = "damage-ptcl";

const char shotDbPutTextureAllKeyword[16] = "puttex-all";

const char shotDbDamageKeyword[8] = "damage";

const char shotDbLifeKeyword[8] = "life";

const char shotDbNotDamageLimitKeyword[16] = "not-dam-limit";

const char shotDbNotDamageTimeKeyword[16] = "not-dam-time";

const char shotDbSlowTimeKeyword[16] = "slow-time";

const char shotDbSlowRateKeyword[16] = "slow-rate";

const char shotDbAssertNameNotNil[16] = "pReader != NIL";

const char shotDbSpeedKeyword[8] = "spd";

const char shotDbAssertEssenceNotNil[16] = "pEss != NIL";

const char shotDbAimKeyword[8] = "aim";

const char shotDbAccuracyKeyword[16] = "accuracy";

const char shotDbWaitKeyword[8] = "wait";

const char shotDbBombSizeKeyword[16] = "bom-size";

const char shotDbBombTimeKeyword[16] = "bom-time";

const char shotDbBombDistanceKeyword[16] = "bom-dist";

const char shotDbChaffKeyword[8] = "chaff";

const char shotDbAttachTimeKeyword[16] = "attach-time";

const char shotDbLengthKeyword[8] = "length";

const char shotDbImmediateDeadKeyword[16] = "imm-dead";

const char shotDbOnceHitKeyword[16] = "once-hit";

const char shotDbAssertDbNotNil[16] = "pDB != NIL";



const char shotDbAssertDataBaseNameNotNil[24] = "pszDataBase != NIL";

const char shotDbNormalTypeKeyword[8] = "normal";

const char shotDbHomingTypeKeyword[8] = "homing";

const char shotDbGrenadeTypeKeyword[8] = "gre";

const char shotDbBeamTypeKeyword[8] = "beam";

const char shotDbFireTypeKeyword[8] = "fire";

const char shotDbUnknownKeywordFormat[32] = "%s : unknown keyword : %s";
