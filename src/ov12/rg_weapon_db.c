/*
 * OV12 original TU 27: 0x00a1c7d8..0x00a1e618 (20 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_read_text.h"
#include "ov12/rg_simple_db.h"
#include "ov12/rg_singleton_id.h"
#include "ov12/rg_weapon_db.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/rg_actor_charid.h"
#include "ov12/rg_actor_motid.h"
#include "ov12/rg_error.h"
#include "ov12/rg_shot_db.h"
#include "ov12/rg_weapon.h"

/* Lengths of the "???-??-???" equip mark string _ReadEquipBitMask parses. */
#define RG_EQUIP_MARK_LEN 10

/* Weapon database sound volumes run from 0 to RG_WEAPON_VOLUME_MAX; the file stores them as 0..1. */
#define RG_WEAPON_VOLUME_SCALE 128.0f

/* Weapon database capacity: entries, and bytes per entry. */
#define RG_WEAPON_DB_ENTRIES 48
#define RG_WEAPON_DB_ENTRY_SIZE 16

extern float dptofp(double value);


/* Declared here: the read-only strings defined between the functions below. */
extern const char weaponDbAssertDbNotNil[];
extern const char weaponDbSourcePath[];
extern const char weaponDbLeftKeyword[];
extern const char weaponDbRightKeyword[];
extern const char weaponDbBackKeyword[];
extern const char weaponDbUnknownEquipTypeFormat[];
extern const char weaponDbNotOrXMarkerFormat[];
extern const char weaponDbBadEquipMaskFormat[];
extern const char weaponDbBadEquipMaskLayoutFormat[];
extern const char weaponDbAssertReaderNotNil[];
extern const char weaponDbBusyKeyword[];
extern const char weaponDbDamageKeyword[];
extern const char weaponDbHitEffectKeyword[];
extern const char weaponDbAssertDataBaseNameNotNil[];
extern const char weaponDbShotTypeKeyword[];
extern const char weaponDbAttackTypeKeyword[];
extern const char weaponDbShieldTypeKeyword[];
extern const char weaponDbEnergyTypeKeyword[];
extern const char weaponDbUnarmedTypeKeyword[];
extern const char weaponDbUnknownKeywordFormat[];
/* Declared here: strings and tables of the scaffold-owned data window (ov12:0x00a538c8..0x00a53c1c). */
extern const char weaponDbContinueKeyword[];  /* "cont" */
extern const char weaponDbEndMarkerKeyword[];  /* "." */
extern const char weaponDbNotLockKeyword[];  /* "notlock" */
extern const char weaponDbFullMotionKeyword[];  /* "full-mot" */
extern const char weaponDbFullMotionAttackErrorText[];  /* "'full-mot' cannot spec to AttackWeapon" */
extern const char weaponDbFullMotionShieldErrorText[];  /* "'full-mot' cannot spec to ShieldWeapon" */
extern const char weaponDbFullMotionUnarmedErrorText[];  /* "'full-mot' cannot spec to UnArmedWeapon" */
extern const char weaponDbNotAdvanceKeyword[];  /* "not-adv" */
extern const char weaponDbAdvanceKeyword[];  /* "adv" */
extern const char weaponDbNoRobotMotionKeyword[];  /* "no-robmot" */
extern const char weaponDbTargetSuffixKeyword[];  /* "target" */
extern const char weaponDbNotPrefixKeyword[];  /* "not" */
extern const char weaponDbAfterKeyword[];  /* "aft" */
extern const char weaponDbWithKeyword[];  /* "wth" */
extern const char weaponDbUnknownTargetKeywordFormat[];  /* "unknown keyword '%s-target'..." */
extern const char weaponDbContinueErrorFormat[];  /* "'%s' is error in 'cont'" */
extern const char weaponDbNumKeyword[];  /* "num" */
extern const char weaponDbEnergyKeyword[];  /* "energy" */
extern const char weaponDbHardnessKeyword[];  /* "hardness" */
extern const char weaponDbInfiniteKeyword[];  /* "inf" */
extern const char weaponDbLockRangeKeyword[];  /* "lock-range" */
extern const char weaponDbIgnoredLockRangeText[];  /* "ignored 'lock-range'" */
extern const char weaponDbLockKeyword[];  /* "lock" */
extern const char weaponDbShotKeyword[];  /* "shot" */
extern const char weaponDbUnknownShotFormat[];  /* "unknown shot '%s'" */
extern const char weaponDbMotionKeyword[];  /* "mot" */
extern const char weaponDbMotionAllKeyword[];  /* "mot-all" */
extern const char weaponDbAssertOpenParen[];  /* "szStr[0] == '('" */
extern const char weaponDbDefaultKeyword[];  /* "def" */
extern const char weaponDbSelfMotionKeyword[];  /* "selfmot" */
extern const char weaponDbSelfMotionAllKeyword[];  /* "selfmot-all" */
extern const char weaponDbBaseMotionKeyword[];  /* "basemot" */
extern const char weaponDbBaseMotionAllKeyword[];  /* "basemot-all" */
extern const char weaponDbModelKeyword[];  /* "model" */
extern const short weaponDbRobotModelDirectory[]; /* "data\weapon\robo\" */
extern const char weaponDbModelAllKeyword[];  /* "modelall" */
extern const char weaponDbUseKeyword[];  /* "use" */
extern const char weaponDbUnknownCharNameFormat[];  /* "unknown char name '%s'" */
extern const char weaponDbEquipKeyword[];  /* "equip" */
extern const char weaponDbOccupyKeyword[];  /* "occupy" */
extern const char weaponDbFakeKeyword[];  /* "fake" */
extern const char weaponDbMassKeyword[];  /* "mass" */
extern const char weaponDbWeightKeyword[];  /* "weight" */
extern const char weaponDbNameKeyword[];  /* "name" */
extern const char weaponDbVerticalDirectionKeyword[];  /* "v-dir" */
extern const char weaponDbHorizontalDirectionKeyword[];  /* "h-dir" */
extern const char weaponDbHitSoundKeyword[];  /* "hitsnd" */
extern const char weaponDbShotSoundKeyword[];  /* "shotsnd" */
extern const char weaponDbBackgroundSoundKeyword[];  /* "bgsnd" */
extern const char weaponDbLifeSoundKeyword[];  /* "lifesnd" */
extern const char weaponDbVoiceHitSoundKeyword[];  /* "vhitsnd" */
extern const char weaponDbVoiceShotSoundKeyword[];  /* "vshotsnd" */
extern const char weaponDbVoiceBackgroundSoundKeyword[];  /* "vbgsnd" */
extern const char weaponDbVoiceLifeSoundKeyword[];  /* "vlifesnd" */
extern const char weaponDbFirstWeaponArmKeyword[];  /* "1wa" */
extern const char weaponDbSecondWeaponArmKeyword[];  /* "2wa" */
extern const char weaponDbThirdWeaponArmKeyword[];  /* "3wa" */
extern const char weaponDbDefaultArmKeyword[];  /* "default" */
extern const char weaponDbShotPointKeyword[];  /* "shot-pnt" */
extern const char weaponDbAssertShotPointCountInRange[];  /* "pEss->m_uShotPntNum <= RG_WEAPON_SHOTPNT_MAX" */
extern const char weaponDbAssertDataIndexInRange[];  /* "0 <= uDatIdx && uDatIdx < 8" */
static int s_aeJntTbl_0[8];


static void _EntryTemporaries(RgSimpleDB *database);
static int _ReadCommon(void *pEss, RgReadText *pReader, char *pszToken);

static void _EntryTemporaries(RgSimpleDB *database) {

}

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

static void _WrapperDestruct(void *database)
{
    DisposeRgSimpleDB(database);
}

RgSimpleDB *InstanceOfRgWeaponDB(void)
{
    RgSimpleDB *database = RgSingletonIDGet(2);

    if (database == 0) {
        database = CreateRgSimpleDB(RG_WEAPON_DB_ENTRIES, RG_WEAPON_DB_ENTRY_SIZE);
        _EntryTemporaries(database);
        RgSingletonIDEntry(2, database, _WrapperDestruct);
    }
    return database;
}

const char weaponDbAssertDbNotNil[] = "pDB != NIL";

const char weaponDbSourcePath[] = "../rg_weapon_db.euc.c";

const char weaponDbLeftKeyword[] = "left";

const char weaponDbRightKeyword[] = "right";

const char weaponDbBackKeyword[] = "back";

const char weaponDbUnknownEquipTypeFormat[] = "unknown equip type '%s'";

const char weaponDbNotOrXMarkerFormat[] = "'%c' is not 'o' or 'x'";

const char weaponDbBadEquipMaskFormat[] = "'%s' is not '???-??-???'";

const char weaponDbBadEquipMaskLayoutFormat[] = "'%s' is not '???-??-???' format";

void RgWeaponDBClear(RgSimpleDB *pDB)
{
    if (pDB == 0) {
        assert_prog(weaponDbAssertDbNotNil, weaponDbSourcePath, 85);
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
        assert_prog(weaponDbAssertDbNotNil, weaponDbSourcePath, 113);
    }
    return RgSimpleDBSize(pDB);
}

void *RgWeaponDBGetIndex(RgSimpleDB *pDB, int nDataID)
{
    if (pDB == 0) {
        assert_prog(weaponDbAssertDbNotNil, weaponDbSourcePath, 121);
    }
    return RgSimpleDBGet(pDB, nDataID);
}

static int _ReadEquipType(RgReadText *pReader)
{
    char szToken[0x80];
    int result;

    RgReadTextGetString(pReader, szToken);
    result = strcasecmp(szToken, weaponDbLeftKeyword);
    if (result != 0) {
        if (strcasecmp(szToken, weaponDbRightKeyword) == 0) {
            return RG_EQUIP_TYPE_RIGHT;
        }
        if (strcasecmp(szToken, weaponDbBackKeyword) == 0) {
            return RG_EQUIP_TYPE_BACK;
        }
        RgError(weaponDbUnknownEquipTypeFormat, weaponDbSourcePath, 0x8C, szToken);
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
    RgError(weaponDbNotOrXMarkerFormat, weaponDbSourcePath, 157, marker);
    return 0;
}

static int _ReadEquipBitMask(RgReadText *pReader, unsigned int *pMask)
{
    char szToken[0x80];
    unsigned int mask;
    int equipmentType = -1;

    RgReadTextGetString(pReader, szToken);
    if (strlen(szToken) != RG_EQUIP_MARK_LEN) {
        RgError(weaponDbBadEquipMaskFormat, weaponDbSourcePath, 169, szToken);
    }
    if (szToken[3] != '-') {
        RgError(weaponDbBadEquipMaskLayoutFormat, weaponDbSourcePath, 170, szToken);
    }
    if (szToken[6] != '-') {
        RgError(weaponDbBadEquipMaskLayoutFormat, weaponDbSourcePath, 171, szToken);
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

static void _SetAllCharMotion(int motionTable[][RG_EQUIP_TYPE_NUM], int motion0, int motion1,
                               int motion2)
{
    unsigned int i;

    for (i = 0; i < RG_ACTOR_CHAR_ROBNUM; i++) {
        motionTable[i][0] = motion0;
        motionTable[i][1] = motion1;
        motionTable[i][2] = motion2;
    }
}

static inline int _Clamp(int value, int low, int high)
{
    if (value < low) {
        return low;
    }
    return high < value ? high : value;
}

static int _ReadCommon(void *pEss, RgReadText *pReader, char *pszToken)
{
    RgWeaponEssence *pWeapon;
    char szToken[0x80];
    char szName[0x40];
    unsigned int equipMask;
    unsigned int occupyMask;
    int equipmentType;
    int mountType;
    unsigned int i;
    int volume;
    char firstChar;
    int level;
    pWeapon = (RgWeaponEssence *)pEss;

    if (strcasecmp(pszToken, weaponDbContinueKeyword) == 0) {
        RgReadTextGetString(pReader, szToken);
        while (strcasecmp(szToken, weaponDbEndMarkerKeyword) != 0) {
            if (strcasecmp(szToken, weaponDbNotLockKeyword) == 0) {
                pWeapon->controlFlags &= ~RG_WEAPON_CONTROL_LOCKON;
                pWeapon->controlFlags &= ~RG_WEAPON_CONTROL_TARGET_MASK;
                pWeapon->controlFlags |= RG_WEAPON_CONTROL_TARGET_WTH;
            } else if (strcasecmp(szToken, weaponDbFullMotionKeyword) == 0) {
                if ((pWeapon->controlFlags & RG_WEAPON_CONTROL_KIND_MASK) == RG_WEAPON_CONTROL_KIND_ATTACK) {
                    RgError(weaponDbFullMotionAttackErrorText, weaponDbSourcePath, 224);
                } else if ((pWeapon->controlFlags & RG_WEAPON_CONTROL_KIND_MASK) == RG_WEAPON_CONTROL_KIND_SHIELD) {
                    RgError(weaponDbFullMotionShieldErrorText, weaponDbSourcePath, 226);
                } else if ((pWeapon->controlFlags & RG_WEAPON_CONTROL_KIND_MASK) == RG_WEAPON_CONTROL_KIND_UNARMED) {
                    RgError(weaponDbFullMotionUnarmedErrorText, weaponDbSourcePath, 228);
                } else {
                    pWeapon->controlFlags = (pWeapon->controlFlags & ~RG_WEAPON_CONTROL_KIND_MASK) | RG_WEAPON_CONTROL_KIND_FULL_MOT;
                }
            } else if (strcasecmp(szToken, weaponDbNotAdvanceKeyword) == 0) {
                pWeapon->controlFlags &= ~RG_WEAPON_CONTROL_ADV;
            } else if (strcasecmp(szToken, weaponDbAdvanceKeyword) == 0) {
                pWeapon->controlFlags |= RG_WEAPON_CONTROL_ADV;
            } else if (strcasecmp(szToken, weaponDbNoRobotMotionKeyword) == 0) {
                pWeapon->controlFlags |= RG_WEAPON_CONTROL_NO_ROBMOT;
            } else if (strcasecmp(szToken + 4, weaponDbTargetSuffixKeyword) == 0) {
                pWeapon->controlFlags &= ~RG_WEAPON_CONTROL_TARGET_MASK;
                szToken[3] = 0;
                if (strcasecmp(szToken, weaponDbNotPrefixKeyword) != 0) {
                    if (strcasecmp(szToken, weaponDbAfterKeyword) == 0) {
                        pWeapon->controlFlags |= RG_WEAPON_CONTROL_TARGET_AFT;
                    } else if (strcasecmp(szToken, weaponDbWithKeyword) == 0) {
                        pWeapon->controlFlags |= RG_WEAPON_CONTROL_TARGET_WTH;
                    } else {
                        RgError(weaponDbUnknownTargetKeywordFormat, weaponDbSourcePath, 256);
                    }
                }
            } else {
                RgError(weaponDbContinueErrorFormat, weaponDbSourcePath, 260, szToken);
            }
            RgReadTextGetString(pReader, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbNumKeyword) == 0 || strcasecmp(pszToken, weaponDbEnergyKeyword) == 0 ||
        strcasecmp(pszToken, weaponDbHardnessKeyword) == 0) {
        RgReadTextGetString(pReader, szName);
        if (strcasecmp(szName, weaponDbInfiniteKeyword) == 0) {
            pWeapon->capacity = 1.0e8f;
        } else {
            pWeapon->capacity = dptofp(atof(szName));
        }
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbLockRangeKeyword) == 0) {
        RgReadTextGetFloat(pReader);
        RgWarn(weaponDbIgnoredLockRangeText, weaponDbSourcePath, 282);
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbLockKeyword) == 0) {
        pWeapon->lockOnAngleMin = RgReadTextGetFloat(pReader) * XRG_PI / 180.0f;
        pWeapon->lockOnAngleMax = RgReadTextGetFloat(pReader) * XRG_PI / 180.0f;
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbShotKeyword) == 0) {
        RgReadTextGetString(pReader, szToken);
        pWeapon->shotEssence = RgShotDBGetEssence(InstanceOfRgShotDB(), szToken);
        if (pWeapon->shotEssence == 0) {
            RgError(weaponDbUnknownShotFormat, weaponDbSourcePath, 294, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbMotionKeyword) == 0) {
        int motion0;
        int motion1;
        int motion2;

        RgReadTextGetString(pReader, szToken);
        motion0 = RgActorMotIDGetFromName(szToken);
        RgReadTextGetString(pReader, szToken);
        motion1 = RgActorMotIDGetFromName(szToken);
        RgReadTextGetString(pReader, szToken);
        motion2 = RgActorMotIDGetFromName(szToken);
        _SetAllCharMotion(pWeapon->shotMotionTable, motion0, motion1, motion2);
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbMotionAllKeyword) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (szToken[0] != '(') {
            assert_prog(weaponDbAssertOpenParen, weaponDbSourcePath, 307);
        }
        RgReadTextGetString(pReader, szToken);
        while (szToken[0] != ')') {
            int motion0;
            int motion1;
            int motion2;
            int charID;

            if (strcasecmp(szToken, weaponDbDefaultKeyword) == 0) {
                RgReadTextGetString(pReader, szToken);
                motion0 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion1 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion2 = RgActorMotIDGetFromName(szToken);
                _SetAllCharMotion(pWeapon->shotMotionTable, motion0, motion1, motion2);
            } else {
                charID = RgActorNameToCharID(szToken);
                RgReadTextGetString(pReader, szToken);
                motion0 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion1 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion2 = RgActorMotIDGetFromName(szToken);
                pWeapon->shotMotionTable[charID][0] = motion0;
                pWeapon->shotMotionTable[charID][1] = motion1;
                pWeapon->shotMotionTable[charID][2] = motion2;
            }
            RgReadTextGetString(pReader, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbSelfMotionKeyword) == 0) {
        int motion0;
        int motion1;
        int motion2;

        RgReadTextGetString(pReader, szToken);
        motion0 = RgActorMotIDGetFromName(szToken);
        RgReadTextGetString(pReader, szToken);
        motion1 = RgActorMotIDGetFromName(szToken);
        RgReadTextGetString(pReader, szToken);
        motion2 = RgActorMotIDGetFromName(szToken);
        _SetAllCharMotion(pWeapon->shotStartMotionTable, motion0, motion1, motion2);
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbSelfMotionAllKeyword) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (szToken[0] != '(') {
            assert_prog(weaponDbAssertOpenParen, weaponDbSourcePath, 339);
        }
        RgReadTextGetString(pReader, szToken);
        while (szToken[0] != ')') {
            int motion0;
            int motion1;
            int motion2;
            int charID;

            if (strcasecmp(szToken, weaponDbDefaultKeyword) == 0) {
                RgReadTextGetString(pReader, szToken);
                motion0 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion1 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion2 = RgActorMotIDGetFromName(szToken);
                _SetAllCharMotion(pWeapon->shotStartMotionTable, motion0, motion1, motion2);
            } else {
                charID = RgActorNameToCharID(szToken);
                RgReadTextGetString(pReader, szToken);
                motion0 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion1 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion2 = RgActorMotIDGetFromName(szToken);
                pWeapon->shotStartMotionTable[charID][0] = motion0;
                pWeapon->shotStartMotionTable[charID][1] = motion1;
                pWeapon->shotStartMotionTable[charID][2] = motion2;
            }
            RgReadTextGetString(pReader, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbBaseMotionKeyword) == 0) {
        int motion0;
        int motion1;
        int motion2;

        RgReadTextGetString(pReader, szToken);
        motion0 = RgActorMotIDGetFromName(szToken);
        RgReadTextGetString(pReader, szToken);
        motion1 = RgActorMotIDGetFromName(szToken);
        RgReadTextGetString(pReader, szToken);
        motion2 = RgActorMotIDGetFromName(szToken);
        _SetAllCharMotion(pWeapon->shotEndMotionTable, motion0, motion1, motion2);
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbBaseMotionAllKeyword) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (szToken[0] != '(') {
            assert_prog(weaponDbAssertOpenParen, weaponDbSourcePath, 371);
        }
        RgReadTextGetString(pReader, szToken);
        while (szToken[0] != ')') {
            int motion0;
            int motion1;
            int motion2;
            int charID;

            if (strcasecmp(szToken, weaponDbDefaultKeyword) == 0) {
                RgReadTextGetString(pReader, szToken);
                motion0 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion1 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion2 = RgActorMotIDGetFromName(szToken);
                _SetAllCharMotion(pWeapon->shotEndMotionTable, motion0, motion1, motion2);
            } else {
                charID = RgActorNameToCharID(szToken);
                RgReadTextGetString(pReader, szToken);
                motion0 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion1 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion2 = RgActorMotIDGetFromName(szToken);
                pWeapon->shotEndMotionTable[charID][0] = motion0;
                pWeapon->shotEndMotionTable[charID][1] = motion1;
                pWeapon->shotEndMotionTable[charID][2] = motion2;
            }
            RgReadTextGetString(pReader, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbModelKeyword) == 0) {
        RgReadTextGetString(pReader, szToken);
        for (i = 0; i < 3; i++) {
            pWeapon->actorSpec[i].actorID = RG_ACTOR_CHAR_ROBNUM;
            __builtin_memcpy(pWeapon->actorSpec[i].fileName, weaponDbRobotModelDirectory, 18);
            strcat(pWeapon->actorSpec[i].fileName, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbModelAllKeyword) == 0) {
        for (i = 0; i < 3; i++) {
            RgReadTextGetString(pReader, szToken);
            pWeapon->actorSpec[i].actorID = RG_ACTOR_CHAR_ROBNUM;
            __builtin_memcpy(pWeapon->actorSpec[i].fileName, weaponDbRobotModelDirectory, 18);
            strcat(pWeapon->actorSpec[i].fileName, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbUseKeyword) == 0) {
        int charID;

        pWeapon->useMask = 0;
        RgReadTextGetString(pReader, szToken);
        while (strcasecmp(szToken, weaponDbEndMarkerKeyword) != 0) {
            charID = RgActorNameToCharID(szToken);
            if (charID == -1) {
                RgError(weaponDbUnknownCharNameFormat, weaponDbSourcePath, 426, szToken);
            }
            pWeapon->useMask |= 1 << charID;
            RgReadTextGetString(pReader, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbEquipKeyword) == 0) {
        _ReadEquipBitMask(pReader, &equipMask);
        RgEquipSetDefault(&pWeapon->equip, equipMask);
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbOccupyKeyword) == 0) {
        equipmentType = _ReadEquipType(pReader);
        occupyMask = 0;
        mountType = _ReadEquipBitMask(pReader, &occupyMask);
        RgEquipAddMount(&pWeapon->equip, equipmentType, mountType, occupyMask);
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbFakeKeyword) == 0) {
        pWeapon->equip.activeMount = 1;
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbMassKeyword) == 0 || strcasecmp(pszToken, weaponDbWeightKeyword) == 0) {
        pWeapon->mass = RgReadTextGetFloat(pReader);
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbNameKeyword) == 0) {
        RgReadTextGetString(pReader, szToken);
        _copy_str_n(pWeapon->displayName, szToken, 48);
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbVerticalDirectionKeyword) == 0) {
        pWeapon->shotPitch = -RgReadTextGetFloat(pReader) * XRG_PI / 180.0f;
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbHorizontalDirectionKeyword) == 0) {
        pWeapon->shotYaw = -RgReadTextGetFloat(pReader) * XRG_PI / 180.0f;
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbHitSoundKeyword) == 0) {
        pWeapon->robotHitSound = RgReadTextGetInt(pReader);
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbShotSoundKeyword) == 0) {
        pWeapon->shotSound = RgReadTextGetInt(pReader);
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbBackgroundSoundKeyword) == 0) {
        pWeapon->bgHitSound = RgReadTextGetInt(pReader);
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbLifeSoundKeyword) == 0) {
        pWeapon->lifeSound = RgReadTextGetInt(pReader);
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbVoiceHitSoundKeyword) == 0) {
        pWeapon->robotHitSound = RgReadTextGetInt(pReader);
        volume = (int)(RgReadTextGetFloat(pReader) * RG_WEAPON_VOLUME_SCALE);
        level = _Clamp(volume, 0, RG_WEAPON_VOLUME_MAX);
        pWeapon->robotHitVolume = level;
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbVoiceShotSoundKeyword) == 0) {
        pWeapon->shotSound = RgReadTextGetInt(pReader);
        volume = (int)(RgReadTextGetFloat(pReader) * RG_WEAPON_VOLUME_SCALE);
        level = _Clamp(volume, 0, RG_WEAPON_VOLUME_MAX);
        pWeapon->shotVolume = level;
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbVoiceBackgroundSoundKeyword) == 0) {
        pWeapon->bgHitSound = RgReadTextGetInt(pReader);
        volume = (int)(RgReadTextGetFloat(pReader) * RG_WEAPON_VOLUME_SCALE);
        level = _Clamp(volume, 0, RG_WEAPON_VOLUME_MAX);
        pWeapon->bgHitVolume = level;
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbVoiceLifeSoundKeyword) == 0) {
        pWeapon->lifeSound = RgReadTextGetInt(pReader);
        volume = (int)(RgReadTextGetFloat(pReader) * RG_WEAPON_VOLUME_SCALE);
        level = _Clamp(volume, 0, RG_WEAPON_VOLUME_MAX);
        pWeapon->lifeVolume = level;
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbFirstWeaponArmKeyword) == 0) {
        pWeapon->eventLevel = 0;
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbSecondWeaponArmKeyword) == 0) {
        pWeapon->eventLevel = 1;
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbThirdWeaponArmKeyword) == 0) {
        pWeapon->eventLevel = 2;
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbDefaultArmKeyword) == 0) {
        pWeapon->isDefault = 1;
        return 1;
    }

    if (strcasecmp(pszToken, weaponDbShotPointKeyword) == 0) {
        unsigned char *pChar;
        unsigned int nextChar;
        unsigned int shotChar;
        int *pShotPntNum;
        unsigned int shotPnt;
        int tableIndex;

        pWeapon->shotPntNum = 0;
        RgReadTextGetString(pReader, szToken);
        firstChar = szToken[0];
        pChar = (unsigned char *)szToken;
        shotChar = *pChar;
        if (firstChar != 0) {
            do {
                pShotPntNum = &pWeapon->shotPntNum;
                if ((unsigned int)(shotChar - '1') < 8) {
                    shotPnt = *pShotPntNum;
                    *pShotPntNum = shotPnt + 1;
                    tableIndex = (char)shotChar - '1';
                    if (!(shotPnt + 1 < 17)) {
                        assert_prog(weaponDbAssertShotPointCountInRange, weaponDbSourcePath, 552);
                    }
                    if (!((unsigned int)tableIndex < 8)) {
                        assert_prog(weaponDbAssertDataIndexInRange, weaponDbSourcePath, 553);
                    }
                    pWeapon->shotPnt[shotPnt] = s_aeJntTbl_0[tableIndex];
                }
                pChar++;
                nextChar = *pChar;
                shotChar = nextChar;
            } while (nextChar != 0);
        }
        return 1;
    }

    return 0;
}

const char weaponDbAssertReaderNotNil[] = "pReader != NIL";

const char weaponDbBusyKeyword[] = "busy";

static RgWeaponShotEssence *_ReadShotType(RgReadText *pReader)
{
    char szToken[0x80];
    RgWeaponShotEssence *pEss;

    if (pReader == 0) {
        assert_prog(weaponDbAssertReaderNotNil, weaponDbSourcePath, 0x238);
    }
    pEss = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeaponShotEssence),
                       weaponDbSourcePath, 0x23A);
    InitRgWeaponShotEssence(pEss);
    for (;;) {
        if (RgReadTextIsEOF(pReader) == 0) {
            RgReadTextGetString(pReader, szToken);
            if (_ReadCommon(pEss, pReader, szToken) == 0) {
                if (strcasecmp(szToken, weaponDbBusyKeyword) == 0) {
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

const char weaponDbDamageKeyword[] = "damage";

const char weaponDbHitEffectKeyword[] = "hiteff";

static RgWeaponAttackEssence *_ReadAttackType(RgReadText *pReader)
{
    char szToken[0x80];
    RgWeaponAttackEssence *pEss;

    if (pReader == 0) {
        assert_prog(weaponDbAssertReaderNotNil, weaponDbSourcePath, 0x252);
    }
    pEss = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeaponAttackEssence),
                       weaponDbSourcePath, 0x254);
    InitRgWeaponAttackEssence(pEss);
    while (RgReadTextIsEOF(pReader) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (_ReadCommon(pEss, pReader, szToken) == 0) {
            if (strcasecmp(szToken, weaponDbDamageKeyword) == 0) {
                pEss->damage = RgReadTextGetFloat(pReader);
                continue;
            } else {
                if (strcasecmp(szToken, weaponDbHitEffectKeyword) == 0) {
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

static RgWeaponUnArmedEssence *_ReadUnArmedType(RgReadText *pReader)
{
    char szToken[0x80];
    RgWeaponUnArmedEssence *pEss;

    if (pReader == 0) {
        assert_prog(weaponDbAssertReaderNotNil, weaponDbSourcePath, 0x26F);
    }
    pEss = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeaponUnArmedEssence),
                       weaponDbSourcePath, 0x271);
    InitRgWeaponUnArmedEssence(pEss);
    while (RgReadTextIsEOF(pReader) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (_ReadCommon(pEss, pReader, szToken) == 0) {
            if (strcasecmp(szToken, weaponDbDamageKeyword) == 0) {
                pEss->damage = RgReadTextGetFloat(pReader);
                continue;
            } else {
                if (strcasecmp(szToken, weaponDbHitEffectKeyword) == 0) {
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

static RgWeaponShieldEssence *_ReadShieldType(RgReadText *pReader)
{
    char szToken[0x80];
    RgWeaponShieldEssence *pEss;

    if (pReader == 0) {
        assert_prog(weaponDbAssertReaderNotNil, weaponDbSourcePath, 0x28C);
    }
    pEss = RgHeapAlloc(InstanceOfRgHeap(), 0x3B0, weaponDbSourcePath, 0x28E);
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

static RgWeaponEnergyEssence *_ReadEnergyType(RgReadText *pReader)
{
    char szToken[0x80];
    RgWeaponEnergyEssence *pEss;

    if (pReader == 0) {
        assert_prog(weaponDbAssertReaderNotNil, weaponDbSourcePath, 676);
    }
    pEss = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgWeaponEnergyEssence),
                       weaponDbSourcePath, 678);
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

const char weaponDbAssertDataBaseNameNotNil[] = "pszDataBase != NIL";

const char weaponDbShotTypeKeyword[] = "shottype";

const char weaponDbAttackTypeKeyword[] = "atktype";

const char weaponDbShieldTypeKeyword[] = "sldtype";

const char weaponDbEnergyTypeKeyword[] = "enetype";

const char weaponDbUnarmedTypeKeyword[] = "sudetype";

const char weaponDbUnknownKeywordFormat[32] = "%s : unknown keyword : %s";

void RgWeaponDBRead(RgSimpleDB *pDB, const char *pszDataBase)
{
    char szToken[0x100];
    RgReadText *pReader;
    RgWeaponEssence *pEntry;

    if (pDB == 0) {
        assert_prog(weaponDbAssertDbNotNil, weaponDbSourcePath, 709);
    }
    if (pszDataBase == 0) {
        assert_prog(weaponDbAssertDataBaseNameNotNil, weaponDbSourcePath, 710);
    }

    pReader = CreateRgReadText(pszDataBase);
    if (pReader != 0) {
        while (RgReadTextIsEOF(pReader) == 0) {
            pEntry = 0;
            RgReadTextGetString(pReader, szToken);

            if (strcasecmp(szToken, weaponDbShotTypeKeyword) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = &_ReadShotType(pReader)->common;
            } else if (strcasecmp(szToken, weaponDbAttackTypeKeyword) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = &_ReadAttackType(pReader)->common;
            } else if (strcasecmp(szToken, weaponDbShieldTypeKeyword) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = &_ReadShieldType(pReader)->common;
            } else if (strcasecmp(szToken, weaponDbEnergyTypeKeyword) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = &_ReadEnergyType(pReader)->common;
            } else if (strcasecmp(szToken, weaponDbUnarmedTypeKeyword) == 0) {
                RgReadTextGetString(pReader, szToken);
                pEntry = &_ReadUnArmedType(pReader)->common;
            } else {
                RgError(weaponDbUnknownKeywordFormat, weaponDbSourcePath, 746, pszDataBase, szToken);
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



static int s_aeJntTbl_0[8] = { 8, 9, 10, 11, 12, 13, 14, 15 };

const char weaponDbContinueKeyword[8] = "cont";

const char weaponDbEndMarkerKeyword[8] = ".";

const char weaponDbNotLockKeyword[8] = "notlock";

const char weaponDbFullMotionKeyword[16] = "full-mot";

const char weaponDbFullMotionAttackErrorText[40] = "'full-mot' cannot spec to AttackWeapon";

const char weaponDbFullMotionShieldErrorText[40] = "'full-mot' cannot spec to ShieldWeapon";

const char weaponDbFullMotionUnarmedErrorText[40] = "'full-mot' cannot spec to UnArmedWeapon";

const char weaponDbNotAdvanceKeyword[8] = "not-adv";

const char weaponDbAdvanceKeyword[8] = "adv";

const char weaponDbNoRobotMotionKeyword[16] = "no-robmot";

const char weaponDbTargetSuffixKeyword[8] = "target";

const char weaponDbNotPrefixKeyword[8] = "not";

const char weaponDbAfterKeyword[8] = "aft";

const char weaponDbWithKeyword[8] = "wth";

const char weaponDbUnknownTargetKeywordFormat[48] = "unknown keyword '%s-target'\ncollect not,aft,wth";

const char weaponDbContinueErrorFormat[24] = "'%s' is error in 'cont'";

const char weaponDbNumKeyword[8] = "num";

const char weaponDbEnergyKeyword[8] = "energy";

const char weaponDbHardnessKeyword[16] = "hardness";

const char weaponDbInfiniteKeyword[8] = "inf";

const char weaponDbLockRangeKeyword[16] = "lock-range";

const char weaponDbIgnoredLockRangeText[24] = "ignored 'lock-range'";

const char weaponDbLockKeyword[8] = "lock";

const char weaponDbShotKeyword[8] = "shot";

const char weaponDbUnknownShotFormat[24] = "unknown shot '%s'";

const char weaponDbMotionKeyword[8] = "mot";

const char weaponDbMotionAllKeyword[8] = "mot-all";

const char weaponDbAssertOpenParen[16] = "szStr[0] == '('";

const char weaponDbDefaultKeyword[8] = "def";

const char weaponDbSelfMotionKeyword[8] = "selfmot";

const char weaponDbSelfMotionAllKeyword[16] = "selfmot-all";

const char weaponDbBaseMotionKeyword[8] = "basemot";

const char weaponDbBaseMotionAllKeyword[16] = "basemot-all";

const char weaponDbModelKeyword[8] = "model";

const short weaponDbRobotModelDirectory[12] = { 24932, 24948, 30556, 24933, 28528, 23662, 28530, 28514, 92, 0, 0, 0 };

const char weaponDbModelAllKeyword[16] = "modelall";

const char weaponDbUseKeyword[8] = "use";

const char weaponDbUnknownCharNameFormat[24] = "unknown char name '%s'";

const char weaponDbEquipKeyword[8] = "equip";

const char weaponDbOccupyKeyword[8] = "occupy";

const char weaponDbFakeKeyword[8] = "fake";

const char weaponDbMassKeyword[8] = "mass";

const char weaponDbWeightKeyword[8] = "weight";

const char weaponDbNameKeyword[8] = "name";

const char weaponDbVerticalDirectionKeyword[8] = "v-dir";

const char weaponDbHorizontalDirectionKeyword[8] = "h-dir";

const char weaponDbHitSoundKeyword[8] = "hitsnd";

const char weaponDbShotSoundKeyword[8] = "shotsnd";

const char weaponDbBackgroundSoundKeyword[8] = "bgsnd";

const char weaponDbLifeSoundKeyword[8] = "lifesnd";

const char weaponDbVoiceHitSoundKeyword[8] = "vhitsnd";

const char weaponDbVoiceShotSoundKeyword[16] = "vshotsnd";

const char weaponDbVoiceBackgroundSoundKeyword[8] = "vbgsnd";

const char weaponDbVoiceLifeSoundKeyword[16] = "vlifesnd";

const char weaponDbFirstWeaponArmKeyword[8] = "1wa";

const char weaponDbSecondWeaponArmKeyword[8] = "2wa";

const char weaponDbThirdWeaponArmKeyword[8] = "3wa";

const char weaponDbDefaultArmKeyword[8] = "default";

const char weaponDbShotPointKeyword[16] = "shot-pnt";

const char weaponDbAssertShotPointCountInRange[48] = "pEss->m_uShotPntNum <= RG_WEAPON_SHOTPNT_MAX";

const char weaponDbAssertDataIndexInRange[32] = "0 <= uDatIdx && uDatIdx < 8";
