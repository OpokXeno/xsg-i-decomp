/*
 * OV12 original TU 27: 0x00a1c7d8..0x00a1e618 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_simple_db.h"
#include "ov12/rg_singleton_id.h"
#include "rg_weapon_db.h"

static void _EntryTemporaries(RgSimpleDB *database) {

}

extern void *memcpy(void *destination, const void *source, unsigned int count);

/*
 * Copies at most nSize bytes from pszSrc into pszDest, then always writes a
 * NUL at pszDest[nSize - 1] (memcpy, main:0x0032eccc, followed by a single
 * sb $0,-1(dest)): the destination is terminated even when the source is
 * not, unlike a plain memcpy.
 */
static void _copy_str_n(char *pszDest, const char *pszSrc, u32 nSize)
{
    memcpy(pszDest, pszSrc, nSize);
    pszDest[nSize - 1] = 0;
}

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
const char D_00A53818[] = "pDB != NIL";
const char D_00A53828[] = "../rg_weapon_db.euc.c";
const char D_00A53840[] = "left";
const char D_00A53848[] = "right";
const char D_00A53850[] = "back";
const char D_00A53858[] = "unknown equip type '%s'";
const char D_00A53870[] = "'%c' is not 'o' or 'x'";
const char D_00A53888[] = "'%s' is not '???-??-???'";
const char D_00A538A8[] = "'%s' is not '???-??-???' format";

extern void RgSimpleDBClear(RgSimpleDB *pDB);
extern int RgSimpleDBFind(RgSimpleDB *pDB, const char *pszName);
extern int RgSimpleDBSize(RgSimpleDB *pDB);
extern void *RgSimpleDBGet(RgSimpleDB *pDB, int nDataID);
extern void RgSimpleDBDump(RgSimpleDB *pDB);

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

/*
 * Opaque handle owned by ov12/tu036 (src/ov12/rg_read_text.c,
 * src/ov12/rg_read_text.h): this TU only forwards the pointer between
 * RgReadTextGetString/-GetFloat/-IsEOF/-Unget and RgError, it never reads or
 * writes a member of it.
 */
typedef struct RgReadText RgReadText;

extern void RgError(const char *message, const char *source_file, int line,
                    ...);
extern void RgReadTextGetString(RgReadText *pReader, char *pszOut);
extern int strcasecmp(const char *s1, const char *s2);

/*
 * Scaffold-owned data (same window as above). ov12:0x00a53840 holds "left",
 * 0x00a53848 "right", 0x00a53850 "back": the three equip-slot keywords
 * _ReadEquipType recognizes. 0x00a53858 holds "unknown equip type '%s'", the
 * RgError message it reports for anything else.
 */
/*
 * The equip-slot indices the strings above are keyed to; "left" (index 0) is
 * the value _ReadEquipType falls through to below without a named constant.
 */
#define RG_EQUIP_RIGHT 1
#define RG_EQUIP_BACK  2

static int _ReadEquipType(RgReadText *pReader)
{
    char szToken[0x80];
    int result;

    RgReadTextGetString(pReader, szToken);
    result = strcasecmp(szToken, D_00A53840);
    if (result != 0) {
        if (strcasecmp(szToken, D_00A53848) == 0) {
            return RG_EQUIP_RIGHT;
        }
        if (strcasecmp(szToken, D_00A53850) == 0) {
            return RG_EQUIP_BACK;
        }
        RgError(D_00A53858, D_00A53828, 0x8C, szToken);
        result = -1;
        return result;
    }
    return result;
}

static int _IsOnChar(char marker)
{
    if (marker == 'o') {
        return 1;
    }
    if (marker == 'm') {
        return 2;
    }
    if (marker == 'x') {
        return 0;
    }
    RgError(D_00A53870, D_00A53828, 157, marker);
    return 0;
}

static int _ReadEquipBitMask(RgReadText *pReader, unsigned int *pMask)
{
    char szToken[0x80];
    unsigned int mask;
    int equipmentType = -1;

    RgReadTextGetString(pReader, szToken);
    if (strlen(szToken) != 10) {
        RgError(D_00A53888, D_00A53828, 169, szToken);
    }
    if (szToken[3] != '-') {
        RgError(D_00A538A8, D_00A53828, 170, szToken);
    }
    if (szToken[6] != '-') {
        RgError(D_00A538A8, D_00A53828, 171, szToken);
    }

    mask = 0;
    if (_IsOnChar(szToken[0]) != 0) mask |= 0x01;
    if (_IsOnChar(szToken[1]) != 0) mask |= 0x02;
    if (_IsOnChar(szToken[2]) != 0) mask |= 0x04;
    if (_IsOnChar(szToken[9]) != 0) mask |= 0x08;
    if (_IsOnChar(szToken[8]) != 0) mask |= 0x10;
    if (_IsOnChar(szToken[7]) != 0) mask |= 0x20;
    if (_IsOnChar(szToken[4]) != 0) mask |= 0x40;
    if (_IsOnChar(szToken[5]) != 0) mask |= 0x80;

    if (_IsOnChar(szToken[0]) == 2) equipmentType = 0;
    if (_IsOnChar(szToken[1]) == 2) equipmentType = 1;
    if (_IsOnChar(szToken[2]) == 2) equipmentType = 2;
    if (_IsOnChar(szToken[9]) == 2) equipmentType = 3;
    if (_IsOnChar(szToken[8]) == 2) equipmentType = 4;
    if (_IsOnChar(szToken[7]) == 2) equipmentType = 5;
    if (_IsOnChar(szToken[4]) == 2) equipmentType = 6;
    if (_IsOnChar(szToken[5]) == 2) equipmentType = 7;

    if (equipmentType != -1) {
        mask |= 1u << equipmentType;
    }
    *pMask = mask;
    return equipmentType;
}

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

extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);
extern RgHeap *InstanceOfRgHeap(void);
extern float RgReadTextGetFloat(RgReadText *pReader);
extern int RgReadTextIsEOF(RgReadText *pReader);
extern void RgReadTextUnget(RgReadText *pReader, char *pszToken);

/*
 * _ReadCommon is one of two file-local statics of that name in this overlay
 * (the other, ov12:0x00a18730, belongs to a different translation unit).
 * This one, ov12:0x00a1cd08, is still INCLUDE_ASM above and shared by every
 * Read*Type function below: it consumes the token they already read and
 * reports (zero) whether it recognized a field common to every essence type.
 */
static int _ReadCommon(void *pEss, RgReadText *pReader, char *pszToken);

/*
 * The prefix every weapon essence the weapon database builds shares: each
 * _Read*Type function of rg_weapon_db.c hands its own essence to _ReadCommon
 * (ov12:0x00a1cd08), which fills these members from the common keys.
 *
 * shotEssence (+0x10): the "shot" key (ov12:0x00a53a48) names an entry of
 * the shot database. _ReadCommon stores RgShotDBGetEssence's result here
 * (sw $2,0x10 at ov12:0x00a1d130) and reports "unknown shot '%s'" when there
 * is none. That entry is whichever shot kind RgShotDBRead built (normal,
 * homing, grenade, beam or fire), so it is typed by the prefix all of them
 * share. rg_weapon.c's _InitWeaponCommon copies it onto the weapon
 * (ov12:0x00a198ec/0x00a19904), and _ShotTypeShot and _EnergyPassTime pass
 * that copy to CreateRgShotFromEssence (ov12:0x00a1a0ec/0x00a1b0dc).
 *
 * capacity (+0x14): the "num", "energy" and "hardness" keys all store one
 * float here, and "inf" stores 1e8 (swc1 0x14 at ov12:0x00a1d02c/0x00a1d048).
 * _InitWeaponCommon copies it to the weapon's shot count (ov12:0x00a198c0/
 * 0x00a198d0).
 *
 * _ReadCommon also writes parts of the span before +0x10 (the "lock" key
 * stores two angles at +0x08/+0x0c); that span stays unmodeled here.
 *
 * shotEssence is spelled with the struct tag so that this type needs no
 * typedef from rg_shot.c's header; RgShotEssence (src/ov12/rg_shot.h)
 * completes the same tag.
 */
typedef struct RgWeaponEssence RgWeaponEssence;
struct RgWeaponEssence {
    unsigned char unmodeled_00[0x10];
    struct RgShotEssence *shotEssence; /* +0x10 */
    float capacity;                    /* +0x14 */
};

/*
 * Scaffold-owned data. ov12:0x00a53c20 holds "pReader != NIL", the argument
 * check every Read*Type function below reports; 0x00a53c30 holds "busy", the
 * key _ReadShotType recognizes for a shot type's cooldown.
 */
const char D_00A53C20[] = "pReader != NIL";
const char D_00A53C30[] = "busy";

/*
 * The per-instance data _ReadShotType allocates. ov12/tu026's rg_weapon.h
 * only forward-declares this tag (its own evidence, _CreateWeaponShotType's
 * null checks, does not reach the interior); this allocation's own field
 * write below completes it.
 */
typedef struct RgWeaponShotEssence RgWeaponShotEssence;
struct RgWeaponShotEssence {
    unsigned char unmodeled_000[0x3b0];
    float busyTime;             /* +0x3b0, the "busy" key above */
    unsigned char unmodeled_3b4[0x0c];
};

extern void InitRgWeaponShotEssence(RgWeaponShotEssence *pEss);

static RgWeaponShotEssence *_ReadShotType(RgReadText *pReader)
{
    char szToken[0x80];
    RgWeaponShotEssence *pEss;

    if (pReader == 0) {
        assert_prog(D_00A53C20, D_00A53828, 0x238);
    }
    pEss = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeaponShotEssence),
                       D_00A53828, 0x23A);
    InitRgWeaponShotEssence(pEss);
    for (;;) {
        if (RgReadTextIsEOF(pReader) == 0) {
            RgReadTextGetString(pReader, szToken);
            if (_ReadCommon(pEss, pReader, szToken) == 0) {
                if (strcasecmp(szToken, D_00A53C30) == 0) {
                    pEss->busyTime = RgReadTextGetFloat(pReader);
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

/*
 * Scaffold-owned data. ov12:0x00a53c38 holds "damage", 0x00a53c40 "hiteff":
 * the two keys _ReadAttackType and _ReadUnArmedType both recognize.
 */
const char D_00A53C38[] = "damage";
const char D_00A53C40[] = "hiteff";

/*
 * The per-instance data _ReadAttackType allocates. ov12/tu026's rg_weapon.h
 * only forward-declares this tag; this allocation's own field writes below
 * complete it.
 */
typedef struct RgWeaponAttackEssence RgWeaponAttackEssence;
struct RgWeaponAttackEssence {
    unsigned char unmodeled_000[0x3b8];
    float damage;                /* +0x3b8, the "damage" key above */
    char hitEffectName[0x24];    /* +0x3bc, the "hiteff" key above */
};

extern void InitRgWeaponAttackEssence(RgWeaponAttackEssence *pEss);

static RgWeaponAttackEssence *_ReadAttackType(RgReadText *pReader)
{
    char szToken[0x80];
    RgWeaponAttackEssence *pEss;

    if (pReader == 0) {
        assert_prog(D_00A53C20, D_00A53828, 0x252);
    }
    pEss = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeaponAttackEssence),
                       D_00A53828, 0x254);
    InitRgWeaponAttackEssence(pEss);
    while (RgReadTextIsEOF(pReader) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (_ReadCommon(pEss, pReader, szToken) == 0) {
            if (strcasecmp(szToken, D_00A53C38) == 0) {
                pEss->damage = RgReadTextGetFloat(pReader);
                continue;
            } else {
                if (strcasecmp(szToken, D_00A53C40) == 0) {
                    RgReadTextGetString(pReader, pEss->hitEffectName);
                    continue;
                }
                RgReadTextUnget(pReader, szToken);
            }
            break;
        }
    }

    return pEss;
}

/*
 * The per-instance data _ReadUnArmedType allocates. ov12/tu026's rg_weapon.h
 * only forward-declares this tag. Its layout matches RgWeaponAttackEssence:
 * the same "damage"/"hiteff" keys at the same offsets.
 */
typedef struct RgWeaponUnArmedEssence RgWeaponUnArmedEssence;
struct RgWeaponUnArmedEssence {
    unsigned char unmodeled_000[0x3b8];
    float damage;                /* +0x3b8, the "damage" key above */
    char hitEffectName[0x24];    /* +0x3bc, the "hiteff" key above */
};

extern void InitRgWeaponUnArmedEssence(RgWeaponUnArmedEssence *pEss);

static RgWeaponUnArmedEssence *_ReadUnArmedType(RgReadText *pReader)
{
    char szToken[0x80];
    RgWeaponUnArmedEssence *pEss;

    if (pReader == 0) {
        assert_prog(D_00A53C20, D_00A53828, 0x26F);
    }
    pEss = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeaponUnArmedEssence),
                       D_00A53828, 0x271);
    InitRgWeaponUnArmedEssence(pEss);
    while (RgReadTextIsEOF(pReader) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (_ReadCommon(pEss, pReader, szToken) == 0) {
            if (strcasecmp(szToken, D_00A53C38) == 0) {
                pEss->damage = RgReadTextGetFloat(pReader);
                continue;
            } else {
                if (strcasecmp(szToken, D_00A53C40) == 0) {
                    RgReadTextGetString(pReader, pEss->hitEffectName);
                    continue;
                }
                RgReadTextUnget(pReader, szToken);
            }
            break;
        }
    }

    return pEss;
}

/*
 * RgWeaponShieldEssence is fully evidenced and completed by ov12/tu026
 * (src/ov12/rg_weapon.h, InitRgWeaponShieldEssence's own field writes); this
 * TU only allocates and forwards the pointer, so it stays opaque here.
 */
typedef struct RgWeaponShieldEssence RgWeaponShieldEssence;

extern void InitRgWeaponShieldEssence(RgWeaponShieldEssence *pEss);

static RgWeaponShieldEssence *_ReadShieldType(RgReadText *pReader)
{
    char szToken[0x80];
    RgWeaponShieldEssence *pEss;

    if (pReader == 0) {
        assert_prog(D_00A53C20, D_00A53828, 0x28C);
    }
    pEss = RgHeapAlloc(InstanceOfRgHeap(), 0x3B0, D_00A53828, 0x28E);
    InitRgWeaponShieldEssence(pEss);
    for (;;) {
        if (RgReadTextIsEOF(pReader) == 0) {
            RgReadTextGetString(pReader, szToken);
            if (_ReadCommon(pEss, pReader, szToken) == 0) {
                RgReadTextUnget(pReader, szToken);
            } else {
                continue;
            }
        }
        break;
    }

    return pEss;
}

typedef struct RgWeaponEnergyEssence RgWeaponEnergyEssence;
struct RgWeaponEnergyEssence {
    unsigned char unmodeled_000[0x3b0];
    float shotEnergy;
    float upTemp;
    float limitTemp;
    float downTemp;
};

extern void InitRgWeaponEnergyEssence(RgWeaponEnergyEssence *pEss);

static RgWeaponEnergyEssence *_ReadEnergyType(RgReadText *pReader)
{
    char szToken[0x80];
    RgWeaponEnergyEssence *pEss;

    if (pReader == 0) {
        assert_prog(D_00A53C20, D_00A53828, 676);
    }
    pEss = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeaponEnergyEssence),
                       D_00A53828, 678);
    InitRgWeaponEnergyEssence(pEss);
    while (RgReadTextIsEOF(pReader) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (_ReadCommon(pEss, pReader, szToken) == 0) {
            if (strcasecmp(szToken, "shot-energy") == 0) {
                pEss->shotEnergy = RgReadTextGetFloat(pReader);
                continue;
            } else {
                if (strcasecmp(szToken, "up-temp") == 0) {
                    pEss->upTemp = RgReadTextGetFloat(pReader);
                    continue;
                } else {
                    if (strcasecmp(szToken, "down-temp") == 0) {
                        pEss->downTemp = RgReadTextGetFloat(pReader);
                        continue;
                    } else {
                        if (strcasecmp(szToken, "limit-temp") == 0) {
                            pEss->limitTemp = RgReadTextGetFloat(pReader);
                            continue;
                        }
                        RgReadTextUnget(pReader, szToken);
                        break;
                    }
                }
            }
        }
    }

    return pEss;
}

/*
 * Database names occupy two fields shared by every type-specific essence.
 * The parser stores the record key at +0x330 and uses +0x350 as its optional
 * display name, defaulting it to the key when _ReadCommon left it empty.
 */
typedef struct RgWeaponDBRecord RgWeaponDBRecord;
struct RgWeaponDBRecord {
    RgWeaponEssence common;
    unsigned char unmodeled_18[0x318];
    char databaseName[0x20];
    char displayName[0x30];
};

const char D_00A53C80[] = "pszDataBase != NIL";
const char D_00A53C98[] = "shottype";
const char D_00A53CA8[] = "atktype";
const char D_00A53CB0[] = "sldtype";
const char D_00A53CB8[] = "enetype";
const char D_00A53CC0[] = "sudetype";
const char D_00A53CD0[] = "%s : unknown keyword : %s";

extern char *strcpy(char *destination, const char *source);
extern void RgSimpleDBEntry(RgSimpleDB *pDB, void *pDat,
                            const char *pszName);
extern RgReadText *CreateRgReadText(const char *pszDataBase);
extern void DisposeRgReadText(RgReadText *pReader);

void RgWeaponDBRead(RgSimpleDB *pDB, const char *pszDataBase)
{
    char szToken[0x100];
    RgReadText *pReader;
    RgWeaponDBRecord *pEntry;

    if (pDB == 0) {
        assert_prog(D_00A53818, D_00A53828, 709);
    }
    if (pszDataBase == 0) {
        assert_prog(D_00A53C80, D_00A53828, 710);
    }

    pReader = CreateRgReadText(pszDataBase);
    if (pReader != 0) {
        while (RgReadTextIsEOF(pReader) == 0) {
            pEntry = 0;
            RgReadTextGetString(pReader, szToken);

            if (strcasecmp(szToken, D_00A53C98) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = (RgWeaponDBRecord *)_ReadShotType(pReader);
            } else if (strcasecmp(szToken, D_00A53CA8) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = (RgWeaponDBRecord *)_ReadAttackType(pReader);
            } else if (strcasecmp(szToken, D_00A53CB0) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = (RgWeaponDBRecord *)_ReadShieldType(pReader);
            } else if (strcasecmp(szToken, D_00A53CB8) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = (RgWeaponDBRecord *)_ReadEnergyType(pReader);
            } else if (strcasecmp(szToken, D_00A53CC0) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = (RgWeaponDBRecord *)_ReadUnArmedType(pReader);
            } else {
                RgError(D_00A53CD0, D_00A53828, 746, pszDataBase, szToken);
            }

            if (pEntry != 0) {
                RgSimpleDBEntry(pDB, pEntry, szToken);
                strcpy(pEntry->databaseName, szToken);
                if (strlen(pEntry->displayName) == 0) {
                    _copy_str_n(pEntry->displayName, szToken, 48);
                }
            }
        }
        DisposeRgReadText(pReader);
    }
}

void RgWeaponDBDump(RgSimpleDB *pDB)
{
    RgSimpleDBDump(pDB);
}
