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

/*
 * The embedded equip record of a weapon essence is owned by ov12/tu030
 * (RgEquipRecord, 0x38 bytes); this file sets only its activeMount member
 * (+0x34) and passes its address on, so the rest is left unmodeled.
 */
typedef struct RgWeaponEquip {
    unsigned char unmodeled_00[0x34];
    int activeMount;                   /* +0x34 */
} RgWeaponEquip;

extern const char D_00A538C8[];
extern const char D_00A538D0[];
extern const char D_00A538D8[];
extern const char D_00A538E0[];
extern const char D_00A538F0[];
extern const char D_00A53918[];
extern const char D_00A53940[];
extern const char D_00A53968[];
extern const char D_00A53970[];
extern const char D_00A53978[];
extern const char D_00A53988[];
extern const char D_00A53990[];
extern const char D_00A53998[];
extern const char D_00A539A0[];
extern const char D_00A539A8[];
extern const char D_00A539D8[];
extern const char D_00A539F0[];
extern const char D_00A539F8[];
extern const char D_00A53A00[];
extern const char D_00A53A10[];
extern const char D_00A53A18[];
extern const char D_00A53A28[];
extern const char D_00A53A40[];
extern const char D_00A53A48[];
extern const char D_00A53A50[];
extern const char D_00A53A68[];
extern const char D_00A53A70[];
extern const char D_00A53A78[];
extern const char D_00A53A88[];
extern const char D_00A53A90[];
extern const char D_00A53A98[];
extern const char D_00A53AA8[];
extern const char D_00A53AB0[];
extern const char D_00A53AC0[];
extern const short D_00A53AC8[];
extern const char D_00A53AE0[];
extern const char D_00A53AF0[];
extern const char D_00A53AF8[];
extern const char D_00A53B10[];
extern const char D_00A53B18[];
extern const char D_00A53B20[];
extern const char D_00A53B28[];
extern const char D_00A53B30[];
extern const char D_00A53B38[];
extern const char D_00A53B40[];
extern const char D_00A53B48[];
extern const char D_00A53B50[];
extern const char D_00A53B58[];
extern const char D_00A53B60[];
extern const char D_00A53B68[];
extern const char D_00A53B70[];
extern const char D_00A53B78[];
extern const char D_00A53B88[];
extern const char D_00A53B90[];
extern const char D_00A53BA0[];
extern const char D_00A53BA8[];
extern const char D_00A53BB0[];
extern const char D_00A53BB8[];
extern const char D_00A53BC0[];
extern const char D_00A53BD0[];
extern const char D_00A53C00[];
extern int s_aeJntTbl_0[8];
extern RgReadText *CreateRgReadText(const char *pszDataBase);
extern double atof(const char *pszText);
extern float dptofp(double value);
extern float RgReadTextGetFloat(RgReadText *pReader);
extern int RgReadTextGetInt(RgReadText *pReader);
extern void RgWarn(const char *format, const char *file, int line, ...);
extern RgSimpleDB *InstanceOfRgShotDB(void);
extern struct RgShotEssence *RgShotDBGetEssence(RgSimpleDB *pDB, char *pszName);
extern int RgActorMotIDGetFromName(char *pszName);
extern int RgActorNameToCharID(char *pszName);
extern void RgEquipSetDefault(RgWeaponEquip *pEquip, unsigned int mask);
extern void RgEquipAddMount(RgWeaponEquip *pEquip, int type, int equipmentType,
                            unsigned int mask);

/*
 * The prefix every weapon essence the weapon database builds shares: each
 * _Read*Type function of rg_weapon_db.c hands its own essence to _ReadCommon
 * (ov12:0x00a1cd08), which fills these members from the common keys. The
 * database record (RgWeaponDBRecord) keeps its key and display name at the
 * same offsets (+0x330, +0x350).
 */
typedef struct RgWeaponModelEntry RgWeaponModelEntry;

struct RgWeaponModelEntry {
    int robotCount;                    /* +0x00: RG_ACTOR_CHAR_ROBNUM */
    char path[0x84];                   /* +0x04: "data\weapon\robo\" + name */
};

typedef struct RgWeaponEssence RgWeaponEssence;

struct RgWeaponEssence {
    unsigned char unmodeled_00[0x04];
    unsigned int flags;                /* +0x04: notlock/full-mot/adv/lock bits */
    float lockOnAngleMin;              /* +0x08: "lock" (radians) */
    float lockOnAngleMax;              /* +0x0c */
    struct RgShotEssence *shotEssence; /* +0x10: "shot" */
    float capacity;                    /* +0x14: "num", "energy", "hardness" */
    float vDir;                        /* +0x18: "v-dir" */
    float hDir;                        /* +0x1c: "h-dir" */
    unsigned char unmodeled_20[0x04];
    int shotPntNum;                    /* +0x24: "shot-pnt" count */
    int shotPnt[16]; /* +0x28: 16 joint indices */
    unsigned char unmodeled_68[0x08];
    RgWeaponEquip equip;               /* +0x70: "equip", "occupy", "fake" */
    int useMask;                       /* +0xa8: "use" */
    float mass;                        /* +0xac: "mass", "weight" */
    int shotMotionTable[RG_ACTOR_CHAR_ROBNUM][3];  /* +0xb0: "mot" */
    int selfMotionTable[RG_ACTOR_CHAR_ROBNUM][3];  /* +0xf8: "selfmot" */
    int baseMotionTable[RG_ACTOR_CHAR_ROBNUM][3];  /* +0x140: "basemot" */
    unsigned char unmodeled_188[0x08];
    RgWeaponModelEntry models[3];      /* +0x190: "model", "modelall" */
    unsigned char unmodeled_328[0x08];
    char databaseName[0x20];           /* +0x330: record key */
    char displayName[0x30];            /* +0x350: "name" */
    int shotSound;                     /* +0x380: "shotsnd" */
    int robotHitSound;                 /* +0x384: "hitsnd" */
    int bgHitSound;                    /* +0x388: "bgsnd" */
    int lifeSound;                     /* +0x38c: "lifesnd" */
    int shotVolume;                    /* +0x390: "vshotsnd" */
    int robotHitVolume;                /* +0x394: "vhitsnd" */
    int bgHitVolume;                   /* +0x398: "vbgsnd" */
    int lifeVolume;                    /* +0x39c: "vlifesnd" */
    int attackSlot;                    /* +0x3a0: "1wa", "2wa", "3wa" */
    int isDefault;                     /* +0x3a4: "default" */
};

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

    if (strcasecmp(pszToken, D_00A538C8) == 0) {
        RgReadTextGetString(pReader, szToken);
        while (strcasecmp(szToken, D_00A538D0) != 0) {
            if (strcasecmp(szToken, D_00A538D8) == 0) {
                pWeapon->flags &= ~0x20;
                pWeapon->flags &= ~0x18;
                pWeapon->flags |= 0x08;
            } else if (strcasecmp(szToken, D_00A538E0) == 0) {
                if ((pWeapon->flags & 7) == 2) {
                    RgError(D_00A538F0, D_00A53828, 224);
                } else if ((pWeapon->flags & 7) == 3) {
                    RgError(D_00A53918, D_00A53828, 226);
                } else if ((pWeapon->flags & 7) == 4) {
                    RgError(D_00A53940, D_00A53828, 228);
                } else {
                    pWeapon->flags = (pWeapon->flags & ~7) | 1;
                }
            } else if (strcasecmp(szToken, D_00A53968) == 0) {
                pWeapon->flags &= ~0x40;
            } else if (strcasecmp(szToken, D_00A53970) == 0) {
                pWeapon->flags |= 0x40;
            } else if (strcasecmp(szToken, D_00A53978) == 0) {
                pWeapon->flags |= 0x80;
            } else if (strcasecmp(szToken + 4, D_00A53988) == 0) {
                pWeapon->flags &= ~0x18;
                szToken[3] = 0;
                if (strcasecmp(szToken, D_00A53990) != 0) {
                    if (strcasecmp(szToken, D_00A53998) == 0) {
                        pWeapon->flags |= 0x10;
                    } else if (strcasecmp(szToken, D_00A539A0) == 0) {
                        pWeapon->flags |= 0x08;
                    } else {
                        RgError(D_00A539A8, D_00A53828, 256);
                    }
                }
            } else {
                RgError(D_00A539D8, D_00A53828, 260, szToken);
            }
            RgReadTextGetString(pReader, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, D_00A539F0) == 0 || strcasecmp(pszToken, D_00A539F8) == 0 ||
        strcasecmp(pszToken, D_00A53A00) == 0) {
        RgReadTextGetString(pReader, szName);
        if (strcasecmp(szName, D_00A53A10) == 0) {
            pWeapon->capacity = 1.0e8f;
        } else {
            pWeapon->capacity = dptofp(atof(szName));
        }
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53A18) == 0) {
        RgReadTextGetFloat(pReader);
        RgWarn(D_00A53A28, D_00A53828, 282);
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53A40) == 0) {
        pWeapon->lockOnAngleMin = RgReadTextGetFloat(pReader) * 3.1415927f / 180.0f;
        pWeapon->lockOnAngleMax = RgReadTextGetFloat(pReader) * 3.1415927f / 180.0f;
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53A48) == 0) {
        RgReadTextGetString(pReader, szToken);
        pWeapon->shotEssence = RgShotDBGetEssence(InstanceOfRgShotDB(), szToken);
        if (pWeapon->shotEssence == 0) {
            RgError(D_00A53A50, D_00A53828, 294, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53A68) == 0) {
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

    if (strcasecmp(pszToken, D_00A53A70) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (szToken[0] != '(') {
            assert_prog(D_00A53A78, D_00A53828, 307);
        }
        RgReadTextGetString(pReader, szToken);
        while (szToken[0] != ')') {
            int motion0;
            int motion1;
            int motion2;
            int charID;

            if (strcasecmp(szToken, D_00A53A88) == 0) {
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

    if (strcasecmp(pszToken, D_00A53A90) == 0) {
        int motion0;
        int motion1;
        int motion2;

        RgReadTextGetString(pReader, szToken);
        motion0 = RgActorMotIDGetFromName(szToken);
        RgReadTextGetString(pReader, szToken);
        motion1 = RgActorMotIDGetFromName(szToken);
        RgReadTextGetString(pReader, szToken);
        motion2 = RgActorMotIDGetFromName(szToken);
        _SetAllCharMotion(pWeapon->selfMotionTable, motion0, motion1, motion2);
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53A98) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (szToken[0] != '(') {
            assert_prog(D_00A53A78, D_00A53828, 339);
        }
        RgReadTextGetString(pReader, szToken);
        while (szToken[0] != ')') {
            int motion0;
            int motion1;
            int motion2;
            int charID;

            if (strcasecmp(szToken, D_00A53A88) == 0) {
                RgReadTextGetString(pReader, szToken);
                motion0 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion1 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion2 = RgActorMotIDGetFromName(szToken);
                _SetAllCharMotion(pWeapon->selfMotionTable, motion0, motion1, motion2);
            } else {
                charID = RgActorNameToCharID(szToken);
                RgReadTextGetString(pReader, szToken);
                motion0 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion1 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion2 = RgActorMotIDGetFromName(szToken);
                pWeapon->selfMotionTable[charID][0] = motion0;
                pWeapon->selfMotionTable[charID][1] = motion1;
                pWeapon->selfMotionTable[charID][2] = motion2;
            }
            RgReadTextGetString(pReader, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53AA8) == 0) {
        int motion0;
        int motion1;
        int motion2;

        RgReadTextGetString(pReader, szToken);
        motion0 = RgActorMotIDGetFromName(szToken);
        RgReadTextGetString(pReader, szToken);
        motion1 = RgActorMotIDGetFromName(szToken);
        RgReadTextGetString(pReader, szToken);
        motion2 = RgActorMotIDGetFromName(szToken);
        _SetAllCharMotion(pWeapon->baseMotionTable, motion0, motion1, motion2);
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53AB0) == 0) {
        RgReadTextGetString(pReader, szToken);
        if (szToken[0] != '(') {
            assert_prog(D_00A53A78, D_00A53828, 371);
        }
        RgReadTextGetString(pReader, szToken);
        while (szToken[0] != ')') {
            int motion0;
            int motion1;
            int motion2;
            int charID;

            if (strcasecmp(szToken, D_00A53A88) == 0) {
                RgReadTextGetString(pReader, szToken);
                motion0 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion1 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion2 = RgActorMotIDGetFromName(szToken);
                _SetAllCharMotion(pWeapon->baseMotionTable, motion0, motion1, motion2);
            } else {
                charID = RgActorNameToCharID(szToken);
                RgReadTextGetString(pReader, szToken);
                motion0 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion1 = RgActorMotIDGetFromName(szToken);
                RgReadTextGetString(pReader, szToken);
                motion2 = RgActorMotIDGetFromName(szToken);
                pWeapon->baseMotionTable[charID][0] = motion0;
                pWeapon->baseMotionTable[charID][1] = motion1;
                pWeapon->baseMotionTable[charID][2] = motion2;
            }
            RgReadTextGetString(pReader, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53AC0) == 0) {
        RgReadTextGetString(pReader, szToken);
        for (i = 0; i < 3; i++) {
            pWeapon->models[i].robotCount = RG_ACTOR_CHAR_ROBNUM;
            __builtin_memcpy(pWeapon->models[i].path, D_00A53AC8, 18);
            strcat(pWeapon->models[i].path, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53AE0) == 0) {
        for (i = 0; i < 3; i++) {
            RgReadTextGetString(pReader, szToken);
            pWeapon->models[i].robotCount = RG_ACTOR_CHAR_ROBNUM;
            __builtin_memcpy(pWeapon->models[i].path, D_00A53AC8, 18);
            strcat(pWeapon->models[i].path, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53AF0) == 0) {
        int charID;

        pWeapon->useMask = 0;
        RgReadTextGetString(pReader, szToken);
        while (strcasecmp(szToken, D_00A538D0) != 0) {
            charID = RgActorNameToCharID(szToken);
            if (charID == -1) {
                RgError(D_00A53AF8, D_00A53828, 426, szToken);
            }
            pWeapon->useMask |= 1 << charID;
            RgReadTextGetString(pReader, szToken);
        }
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B10) == 0) {
        _ReadEquipBitMask(pReader, &equipMask);
        RgEquipSetDefault(&pWeapon->equip, equipMask);
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B18) == 0) {
        equipmentType = _ReadEquipType(pReader);
        occupyMask = 0;
        mountType = _ReadEquipBitMask(pReader, &occupyMask);
        RgEquipAddMount(&pWeapon->equip, equipmentType, mountType, occupyMask);
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B20) == 0) {
        pWeapon->equip.activeMount = 1;
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B28) == 0 || strcasecmp(pszToken, D_00A53B30) == 0) {
        pWeapon->mass = RgReadTextGetFloat(pReader);
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B38) == 0) {
        RgReadTextGetString(pReader, szToken);
        _copy_str_n(pWeapon->displayName, szToken, 48);
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B40) == 0) {
        pWeapon->vDir = -RgReadTextGetFloat(pReader) * 3.1415927f / 180.0f;
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B48) == 0) {
        pWeapon->hDir = -RgReadTextGetFloat(pReader) * 3.1415927f / 180.0f;
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B50) == 0) {
        pWeapon->robotHitSound = RgReadTextGetInt(pReader);
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B58) == 0) {
        pWeapon->shotSound = RgReadTextGetInt(pReader);
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B60) == 0) {
        pWeapon->bgHitSound = RgReadTextGetInt(pReader);
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B68) == 0) {
        pWeapon->lifeSound = RgReadTextGetInt(pReader);
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B70) == 0) {
        pWeapon->robotHitSound = RgReadTextGetInt(pReader);
        volume = (int)(RgReadTextGetFloat(pReader) * 128.0f);
        level = _Clamp(volume, 0, 128);
        pWeapon->robotHitVolume = level;
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B78) == 0) {
        pWeapon->shotSound = RgReadTextGetInt(pReader);
        volume = (int)(RgReadTextGetFloat(pReader) * 128.0f);
        level = _Clamp(volume, 0, 128);
        pWeapon->shotVolume = level;
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B88) == 0) {
        pWeapon->bgHitSound = RgReadTextGetInt(pReader);
        volume = (int)(RgReadTextGetFloat(pReader) * 128.0f);
        level = _Clamp(volume, 0, 128);
        pWeapon->bgHitVolume = level;
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53B90) == 0) {
        pWeapon->lifeSound = RgReadTextGetInt(pReader);
        volume = (int)(RgReadTextGetFloat(pReader) * 128.0f);
        level = _Clamp(volume, 0, 128);
        pWeapon->lifeVolume = level;
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53BA0) == 0) {
        pWeapon->attackSlot = 0;
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53BA8) == 0) {
        pWeapon->attackSlot = 1;
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53BB0) == 0) {
        pWeapon->attackSlot = 2;
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53BB8) == 0) {
        pWeapon->isDefault = 1;
        return 1;
    }

    if (strcasecmp(pszToken, D_00A53BC0) == 0) {
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
                        assert_prog(D_00A53BD0, D_00A53828, 552);
                    }
                    if (!((unsigned int)tableIndex < 8)) {
                        assert_prog(D_00A53C00, D_00A53828, 553);
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

extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);
extern RgHeap *InstanceOfRgHeap(void);
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
typedef struct RgWeaponEssence RgWeaponDBRecord;

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
