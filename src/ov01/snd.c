/*
 * OV01 original TU 13: 0x00a2c9f8..0x00a2e4b0 (34 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/act_1.h"
#include "ov01/data_unit_org_get.h"
#include "snd.h"

extern int dataFileLoadNB(const char *filename, void *destination);

#define SND_MU_VOLUME_MAX 0x7F
#define SND_MU_FADE_TIME  2000
#define SND_MU_SET_COUNT  3

/* CD reads come in whole 2048-byte sectors. */
#define SND_SECTOR_SIZE         0x800
#define SND_SECTOR_SHIFT        11
#define SND_SECTOR_ROUND_BIAS   (SND_SECTOR_SIZE - 1)
#define SND_SECTOR_ROUND_NEG_BIAS (SND_SECTOR_SIZE * 2 - 2)

/* The tags a sound-effect bank's "sed" and "swd" blocks start with (the
 * little-endian words of the ASCII tags) and the largest "sed" block. */
#define SND_SED_TAG      0x73646573
#define SND_SWD_TAG      0x6D647773
#define SND_SED_MAX_SIZE 0x10000

/* Sound-effect id of the damage sound, which plays once per damage bank. */
#define SND_SE_ID_DAMAGE 2
#define SND_DMG_BANK_COUNT 3
#define SND_DMG_BANK_FIRST 4
#define SND_SYS_SE_COUNT 0x26
#define SND_MONS_ID_RANGE_LEN 0x24U
#define SND_SE_AGWS_BANK_COUNT 3
/* Sound ids sndConvSe and sndConvHitSe return for the two heavier reaction
 * categories (normal indices 3 and 5). */
#define SND_SE_REACT_HEAVY 0x21
#define SND_SE_HIT_HEAVY   0x22

/* Character id ranges: the boss range is CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN
 * up to CALC_SPECIAL_MODEL_CHARACTER_ID_END (exclusive, include/ov01/calc.h);
 * enemy ids start at SND_CHARA_ID_ENEMY_FIRST. */
#define SND_CHARA_ID_ENEMY_FIRST 0x97

/* The flags word at the head of a battle actor (unit->work, Actor.flags). */
/* The same bit value on CalcUnitParam.flags means AGWS, not enemy. */
#define SND_SE_UNIT_FLAG_AGWS 0x40u

/* Unprototyped on purpose: sndSeStop calls it with no arguments (jal with no
 * argument setup, ov01:0x00a2db90) while sndSePlay passes (unit, id); the
 * definition below takes (BattleUnit *unit, int id). */
extern int sndBankGet();

/* ov01:0x00a2d1f8. The base of loaded sound-effect bank metadata: one record
 * shaped like sndSeRegDat's element (ov01:0x00a42d18, size 0x68). */
extern SndSeRegDat sndSeDat;
extern SndSeRegDat sndSeRegDat[SND_SE_REG_COUNT];

void sndInit(void)
{
    int i;

    for (i = 0; i < SND_SE_REG_COUNT; i++) {
        memset(&sndSeRegDat[i], 0, sizeof(SndSeRegDat));
        if (i < SND_SE_REG_HALF) {
            sndSeRegDat[i].bank[0].work.dest = (void *)(0x1914000 + i * 0x50000);
        } else {
            sndSeRegDat[i].bank[0].work.dest = (void *)(0x1905000 + i * 0x55000);
        }
    }
    memset(&sndSeDat, 0, sizeof(SndSeRegDat));
    sndSeDat.bank[0].work.dest = (void *)0x1B4E000;
    memset(&sndMuDat, 0, sizeof(SndMuData));
    sndMuDat.smdNormal = (void *)0x1904000;
}

extern int dmgBankOffs[SND_DMG_BANK_COUNT];
extern int dmgBankStat[SND_DMG_BANK_COUNT];
extern int ctrlId;

void sndInit2(void)
{
    int i;

    for (i = 0; i < SND_DMG_BANK_COUNT; i++) {
        dmgBankOffs[i] = 0;
        dmgBankStat[i] = 0;
    }
    ctrlId = 0;
}

/* Station-edit condition codes: the words of a btst/bted record's condition
 * list, evaluated by dataSndStEdNumSub. */
enum SndStEdCond {
    SND_STED_COND_NEVER = 0,
    SND_STED_COND_ALWAYS = 1,
    SND_STED_COND_UNIT_ID_2 = 2,
    SND_STED_COND_UNIT_ID_3 = 3,
    SND_STED_COND_STAT = 4,
    SND_STED_COND_ETHER = 5,
    SND_STED_COND_MONS_FLAG = 6,
    SND_STED_COND_IDOL2 = 7,
    SND_STED_COND_UNIT_TBL_FLAG = 8,
    SND_STED_COND_MONS_RANGE = 9,
    SND_STED_COND_MONS_A2 = 10,
    SND_STED_COND_MONS_AA = 11,
    SND_STED_COND_MONS_AC = 12,
    /* Codes from here up take the whole pick (dataSndStEdNum). */
    SND_STED_COND_OVERRIDE_FIRST = 7,
    SND_STED_COND_COUNT = 8
};

typedef struct SndStEdData {
    int unitId;
    const char *name;
    int condition[SND_STED_COND_COUNT];
} SndStEdData;

extern int dataSndStEdNumSub(BattleUnit *unit, int condition);
extern int rnd(int maximum);

/* The caller selects btst/bted records; every live record starts with the
 * unconditional condition 1, so at least one candidate is always written.
 * dataSndStEdNameGet passes its type argument as a third word (daddu $6 before
 * jal dataSndStEdNum, ov01:0x00a2cf40); this body never reads it. */
int dataSndStEdNum(SndStEdData *data, BattleUnit *unit, int type)
{
    int candidate[8];
    int index;
    int count;
    int *conditions;
    int *condition;

    conditions = data->condition;
    count = 0;
    for (index = 0; index < SND_STED_COND_COUNT; index++) {
        condition = &conditions[index];
        if (dataSndStEdNumSub(unit, *condition)) {
            if (*condition >= SND_STED_COND_OVERRIDE_FIRST) {
                candidate[0] = index + 1;
                count = 1;
                break;
            }
            candidate[count] = index + 1;
            count++;
        }
    }
    return candidate[rnd(count - 1)];
}

extern int monsSetNoGet(void);
extern void *thinkMonsTblGet(int monsSetNo);
extern int unitIdChk(int side, int id);
extern int dataEtherLearnGet(int cid, int id);
extern int MenuScenarioNoGet(void);
extern int calcIdol2Chk(BattleUnit *unit);
extern int unitTblGet(int side, BattleUnit ***table);

/* One record of the monster-set table: the monster id, then three halfwords
 * the code never reads. */
typedef struct {
    short id;
    short unmodeled_02[3];
} SndMonsEnt;

/*
 * Evaluates one condition code of a station-edit record for unit; the return
 * value is nonzero when the condition holds. Code 0 never holds and code 1
 * always does. Codes 2, 3 and 7 test the unit (unitIdChk, calcIdol2Chk) and
 * codes 4 and 5 test its status or the ether learn table (calcStatGet,
 * dataEtherLearnGet), with an early hold where the first test already passes.
 * Code 6 needs scenario number 10 or more and a monster-set entry whose init
 * record has flag 0x2000; code 8 tests flag 0x80 on the unit table of side 0;
 * codes 9 (ids 0x97..0xBA), 10 (0xA2), 11 (0xAA) and 12 (0xAC) scan the
 * monster set for their ids.
 */
int dataSndStEdNumSub(BattleUnit *unit, int condition)
{
    int ret;
    SndMonsEnt *m;
    BattleUnit **tbl;
    BattleUnit **t;
    int n;
    int i;
    int v;
    int x;
    int one;
    UnitInitData *d;

    ret = 0;
    m = thinkMonsTblGet(monsSetNoGet());
    switch (condition) {
    case SND_STED_COND_NEVER:
        break;
    case SND_STED_COND_UNIT_ID_2:
        x = unitIdChk(0, 2);
        goto chk;
    case SND_STED_COND_UNIT_ID_3:
        x = unitIdChk(0, 3);
        goto chk;
    case SND_STED_COND_ETHER:
        if (dataEtherLearnGet(6, 0x2B)) {
            ret = 1;
            break;
        }
        x = dataEtherLearnGet(6, 0x2E);
        goto chk;
    case SND_STED_COND_STAT:
        if (calcStatGet(unit, 7, 2)) {
            ret = 1;
            break;
        }
        x = calcStatGet(unit, 7, 1);
        goto chk;
    case SND_STED_COND_MONS_FLAG:
        if (!unitIdChk(0, 2)) break;
        if (MenuScenarioNoGet() < 10) break;
        for (i = 0; m[i].id != 0; i++) {
            d = dataUnitInitGet(m[i].id);
            if (d->flags & 0x2000) ret = 1;
        }
        return ret;
    case SND_STED_COND_IDOL2:
        x = calcIdol2Chk(unit);
    chk:
        if (!x) break;
    case SND_STED_COND_ALWAYS:
        ret = 1;
        break;
    case SND_STED_COND_UNIT_TBL_FLAG:
        n = unitTblGet(0, &tbl);
        i = 0;
        if (n > 0) {
            t = tbl;
            one = 0;
            do {
                if (*t != 0) {
                    one = 1;
                    if ((*t)->work->flags & 0x80) ret = one;
                }
                i++;
                t++;
            } while (i < n);
            return ret;
        }
        break;
    case SND_STED_COND_MONS_RANGE:
        for (i = 0; m[i].id != 0; i++) {
            if ((unsigned short)m[i].id - SND_CHARA_ID_ENEMY_FIRST < SND_MONS_ID_RANGE_LEN) ret = 1;
        }
        return ret;
    case SND_STED_COND_MONS_A2:
        for (i = 0; m[i].id != 0; i++) {
            if (m[i].id == 0xA2) ret = 1;
        }
        return ret;
    case SND_STED_COND_MONS_AA:
        for (i = 0; m[i].id != 0; i++) {
            if (m[i].id == 0xAA) ret = 1;
        }
        return ret;
    case SND_STED_COND_MONS_AC:
        for (i = 0; m[i].id != 0; i++) {
            if (m[i].id == 0xAC) ret = 1;
        }
        return ret;
    default:
        break;
    }
    return ret;
}

static SndStEdData btst[7];
static SndStEdData bted[7];
extern char *strcpy(char *destination, const char *source);
extern char *strcat(char *destination, const char *source);

/*
 * Builds the station-edit sound name for unit from the btst table (type 0x18)
 * or the bted table (any other type). Each record whose unitId is the unit's
 * charaId copies its name into name and appends a digit: '0' plus the running
 * sum of the values dataSndStEdNum picks for the matching records so far.
 */
char *dataSndStEdNameGet(char *name, BattleUnit *unit, int type)
{
    SndStEdData *data;
    SndStEdData *table;
    char num[2];
    SndStEdData **pdata;
    short charaId;
    int n;

    charaId = calcUPGet(unit)->charaId;
    num[0] = '0';
    num[1] = '\0';
    if (type == SND_SE_TYPE_STED_BTST) {
        table = btst;
    } else {
        table = bted;
    }
    if (table->unitId != 0) {
        for (data = table; data->unitId != 0; data++) {
            /* Read through a pointer to the record, as the original does. */
            pdata = &data;
            if ((*pdata)->unitId == charaId) {
                n = dataSndStEdNum(*pdata, unit, type);
                num[0] += n;
                strcpy(name, (*pdata)->name);
                strcat(name, num);
            }
        }
    }
    return name;
}

typedef struct {
    unsigned char unmodeled_00[0x11]; /* +0x00 */
    signed char category;             /* +0x11 */
    unsigned char unmodeled_12[4];    /* +0x12 */
    short spid;                       /* +0x16 */
} TecData;
extern TecData *dataTecGet(int tecId);
extern unsigned char *dataWpnGet(int index);
extern char *sefGetEffectName(int effectNo);
extern char *strlwr(char *s);
extern char *sndUnitSeBaseName[];
/* Defined below with the name tables (keeps the data order). */
static const char *plChaMidSeName[24];
static const char *plAgwsMidSeName[3];
static const char *enemyMidSeName[3];

/*
 * Writes the sound-effect name of seType for unit into name (emptied first)
 * and lowercases it. A player-side unit (not enemy, not AGWS, charaId outside
 * the boss range 0xBB..0xC2) takes the station-edit names for seType 0x18 and
 * 0x19 and otherwise its base name plus plChaMidSeName[seType]. An AGWS unit
 * takes its base name plus plAgwsMidSeName[seType] below 3, else the effect of
 * weaponId[seType - 3] when that is nonzero. Any other unit takes its base name
 * plus enemyMidSeName[seType] below 3, else the effect of
 * normalTechniqueId[seType - 3] when that is nonzero. The base name is
 * sndUnitSeBaseName[charaId] and a null base writes nothing in the table forms; types
 * from 0x1A up name the effect seType - 0x1A directly.
 */
char *dataSndSeNameGet(char *name, BattleUnit *unit, int seType)
{
    short charaId;
    char *base;
    short id;
    short wpn;

    charaId = calcUPGet(unit)->charaId;
    *name = 0;
    if (seType < SND_SE_TYPE_EFFECT_FIRST) {
        if (!(unit->work->flags & ACTOR_FLAG_ENEMY)
            && !(calcUPGet(unit)->flags & SND_SE_UNIT_FLAG_AGWS)
            && (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN
                || calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END)) {
            if ((unsigned int)(seType - SND_SE_TYPE_STED_BTST) < 2) {
                dataSndStEdNameGet(name, unit, seType);
            } else {
                base = sndUnitSeBaseName[charaId];
                if (base != 0) {
                    strcpy(name, base);
                    strcat(name, plChaMidSeName[seType]);
                }
            }
        } else if (calcUPGet(unit)->flags & SND_SE_UNIT_FLAG_AGWS) {
            base = sndUnitSeBaseName[charaId];
            if (base != 0) {
                if (seType < SND_SE_TYPE_SLOT_FIRST) {
                    strcpy(name, base);
                    strcat(name, plAgwsMidSeName[seType]);
                } else {
                    wpn = calcUPGet(unit)->weaponId[seType - SND_SE_TYPE_SLOT_FIRST];
                    if (wpn != 0) {
                        strcpy(name, sefGetEffectName(dataTecGet(*(short *)(dataWpnGet(wpn) + 0x14))->spid));
                    }
                }
            }
        } else {
            base = sndUnitSeBaseName[charaId];
            if (base != 0) {
                if (seType < SND_SE_TYPE_SLOT_FIRST) {
                    strcpy(name, base);
                    strcat(name, enemyMidSeName[seType]);
                } else {
                    id = calcUPGet(unit)->normalTechniqueId[seType - SND_SE_TYPE_SLOT_FIRST];
                    if (id != 0) {
                        strcpy(name, base);
                        strcpy(name, sefGetEffectName(dataTecGet(id)->spid));
                    }
                }
            }
        }
    } else {
        strcpy(name, sefGetEffectName(seType - SND_SE_TYPE_EFFECT_FIRST));
    }
    strlwr(name);
    return name;
}

/*
 * Declared unprototyped in this TU: dataSndSeLoad2 (ov01:0x00a2d654/
 * 0x00a2d658) and dataSndSeLoad evaluate and pass the owning unit's charaId
 * here even though the body below ignores every argument.
 */
SndSeRegDat *sndSeAdrGet()
{
    return &sndSeDat;
}

SndSeRegDat *sndSeRegAdrChk(int id)
{
    int i;

    for (i = 0; i < SND_SE_REG_COUNT; i++) {
        if (sndSeRegDat[i].id == id) {
            return &sndSeRegDat[i];
        }
    }
    return 0;
}

/*
 * ov01:0x00a2d248. Reuses an existing sound registration if one already
 * matches id; otherwise, for a character id below 0xBB, claims the first
 * free slot in the id's half of sndSeRegDat (0..2 below 0x21, 3..5
 * otherwise) and returns its address, or 0 if none is free or id is 0xBB or
 * above.
 */
SndSeRegDat *sndSeRegAdrGet(int id)
{
    SndSeRegDat *found;
    int i;
    int start;
    int end;

    found = sndSeRegAdrChk(id);
    if (found != 0) {
        return found;
    }
    if (id >= CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN) {
        return 0;
    }
    if (id < SND_SE_REG_SPLIT_ID) {
        start = 0;
        end = SND_SE_REG_HALF;
    } else {
        start = SND_SE_REG_HALF;
        end = SND_SE_REG_COUNT;
    }
    for (i = start; i < end; i++) {
        if (sndSeRegDat[i].id == 0) {
            sndSeRegDat[i].id = id;
            return &sndSeRegDat[i];
        }
    }
    return 0;
}

int sndSeRegRemove(int id)
{
    int i;
    int j;

    for (i = 0; i < SND_SE_REG_COUNT; i++) {
        if (sndSeRegDat[i].id == id) {
            sndSeRegDat[i].id = 0;
            for (j = SND_SE_BANK_COUNT - 1; j >= 0; j--) {
                sndSeRegDat[i].bank[j].work.seType = SND_SE_TYPE_NONE;
            }
            return 1;
        }
    }
    return 0;
}

/*
 * ov01:0x00a2d388. Reports whether the sound-effect index seType has a
 * registered bank for this unit: a player-side unit outside the boss charaId
 * range (0xBB..0xC2) registers 1..4 and 23, a boss or enemy unit in the
 * 0x97..0xBA charaId range registers 1 and 2, and every other enemy
 * registers 1..5.
 */
int sndSeRegChk(BattleUnit *unit, int seType)
{
    int registered = 0;

    if (!(unit->work->flags & ACTOR_FLAG_ENEMY)
        && !(calcUPGet(unit)->flags & SND_SE_UNIT_FLAG_AGWS)
        && (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN
            || calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END)) {
        if (seType > SND_SE_TYPE_NONE && (seType <= SND_SE_TYPE_AT2 || seType == SND_SE_TYPE_CV)) {
            registered = 1;
        }
    } else if (calcUPGet(unit)->charaId >= SND_CHARA_ID_ENEMY_FIRST
               && calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN) {
        if (seType < SND_SE_TYPE_SLOT_FIRST) {
            if (seType > SND_SE_TYPE_NONE) {
                registered = 1;
            }
        }
    } else if (seType <= SND_SE_TYPE_SLOT_LAST) {
        registered = seType > SND_SE_TYPE_NONE;
    }
    return registered;
}

extern int xglCdGetFileSize(const char *name);
extern int printf(const char *format, ...);
#define SND_FILE_NAME_SIZE 0x100
static char fileName[SND_FILE_NAME_SIZE];
static char *sndSeNameBase = "data\\yamamoto\\snd\\sed\\";
static char *sndSeNameExt = ".bin";
static char *sndMuNameBase = "data\\yamamoto\\snd\\smd\\";
static char *sndMuNameExt1 = ".SMD";
static char *sndMuNameExt2 = ".SWD";
static char *muName[3] = { "BATTLE1", "BATTLE2", "BATTLE3" };
static char *mu2Name[3] = { "JINGLE1", "JINGLE2", 0 };

/* Readable C spellings for literal data used by the functions below. */
#define SND_SE_LOAD_SUB_ERROR_FORMAT "** dataSndSeLoadSub: err -> (%d) %d\n"
#define SND_SE_LOAD_SUB_SIZE_FORMAT "** SND read Size = %d\n"
#define SND_SE_REG_LOAD_SKIP_FORMAT "** dataSndSeRegLoad: skip %d\n"
#define SND_SE_PLAY_FORMAT "<< SE Play bank=%d id=%d no=%d %d >>\n"
#define SND_SE_STOP_FORMAT "<< SE Stop pack=%d >>\n"

typedef struct {
    int key;
    int value;
} SndConvRegPair;
typedef struct {
    SndConvRegPair pair[7];
} SndConvRegTable;
#define SND_MU_PLAY_FORMAT "<< MU Play no=%d >>\n"
#define SND_CONV_REG_TABLE \
    ((SndConvRegTable){{{1, 0}, {2, 1}, {3, 2}, {4, 3}, {5, 4}, {0x17, 4}, {0, 0}}})

#define SND_SE_LOAD_SKIP_FORMAT "** dataSndSeLoad: skip %d %d\n"

int dataSndSeLoadSub(BattleUnit *unit, int seType, SndSeLoadWork *work)
{
    char name[16];
    int fileSize;
    int withSlack;
    int clamped;
    int roundedSize;

    dataSndSeNameGet(name, unit, seType);
    if (name[0] == 0) {
        printf(SND_SE_LOAD_SUB_ERROR_FORMAT, calcUPGet(unit)->charaId, seType);
        work->seType = SND_SE_TYPE_NONE;
        return 0;
    }

    strcpy(fileName, sndSeNameBase);
    strcat(fileName, name);
    strcat(fileName, sndSeNameExt);
    dataFileLoadNB(fileName, work->dest);

    fileSize = xglCdGetFileSize(fileName);
    withSlack = fileSize + SND_SECTOR_ROUND_BIAS;
    clamped = (withSlack < 0) ? (fileSize + SND_SECTOR_ROUND_NEG_BIAS) : withSlack;
    roundedSize = (clamped >> SND_SECTOR_SHIFT) << SND_SECTOR_SHIFT;
    work->size = roundedSize;
    printf(SND_SE_LOAD_SUB_SIZE_FORMAT, roundedSize);
    work->seType = seType;
    return 1;
}

/*
 * Loads the sound-effect bank seType of unit into the record of its character
 * (sndSeAdrGet). A record already holding this character and bank is skipped
 * with a message; otherwise the record's sed and swd addresses are cleared
 * before the file is loaded, and the character id is stored only on success.
 */
int dataSndSeLoad(BattleUnit *unit, int seType)
{
    short charaId;
    SndSeRegDat *dat;
    SndSeRegBank *bank;

    charaId = calcUPGet(unit)->charaId;
    dat = sndSeAdrGet(charaId);
    bank = &dat->bank[0];
    if (dat->id == charaId && bank->work.seType == seType) {
        printf(SND_SE_LOAD_SKIP_FORMAT, charaId, seType);
        return 1;
    }
    bank->swd = 0;
    bank->sed = 0;
    if (dataSndSeLoadSub(unit, seType, &bank->work) == 0) {
        return 0;
    }
    dat->id = charaId;
    return 1;
}

/*
 * ov01:0x00a2d648. Derives the two tagged-block pointers of the current
 * unit's base sound-effect bank from its loaded file header, unless either
 * is already set.
 */
void dataSndSeLoad2(BattleUnit *unit)
{
    SndSeRegDat *record;
    SndSeRegBank *bank;
    SndSeBankHeader *header;

    /* sndSeAdrGet ignores its argument (ov01:0x00a42d18 is a fixed global),
     * but the caller still evaluates and passes the owning unit's charaId
     * here (ov01:0x00a2d654/0x00a2d658), matching every other lookup in this
     * TU that keys off calcUPGet(unit)->charaId. */
    record = sndSeAdrGet(calcUPGet(unit)->charaId);
    bank = &record->bank[0];
    if (bank->sed != 0) {
        return;
    }
    if (bank->swd != 0) {
        return;
    }
    header = bank->work.dest;
    bank->sed = (unsigned char *)header + header->offset.sed;
    bank->swd = (unsigned char *)header + header->offset.swd;
}

/*
 * ov01:0x00a2d6a0. Claims a unit sound registration, loads its base bank,
 * then loads three AGWS-specific banks when applicable.
 */
int dataSndSeRegLoad(BattleUnit *unit)
{
    short charaId;
    SndSeRegDat *record;
    SndSeRegBank *bank;
    void *end;
    int seType;

    charaId = calcUPGet(unit)->charaId;
    if (sndSeRegAdrChk(charaId) != 0) {
        printf(SND_SE_REG_LOAD_SKIP_FORMAT, charaId);
        return 1;
    }
    record = sndSeRegAdrGet(charaId);
    if (record == 0) {
        return 0;
    }
    bank = &record->bank[0];
    if (dataSndSeLoadSub(unit, SND_SE_TYPE_BASE, &bank->work) == 0) {
        return 0;
    }
    if (calcUPGet(unit)->flags & SND_SE_UNIT_FLAG_AGWS) {
        end = (unsigned char *)bank->work.dest + bank->work.size;
        for (seType = SND_SE_TYPE_SLOT_FIRST; seType <= SND_SE_TYPE_SLOT_LAST; seType++) {
            bank = &record->bank[seType - 1];
            bank->work.dest = end;
            if (dataSndSeLoadSub(unit, seType, &bank->work) == 0) {
                bank->work.size = 0;
                bank->work.seType = SND_SE_TYPE_NONE;
            }
            end = (unsigned char *)bank->work.dest + bank->work.size;
        }
    }
    record->id = charaId;
    return 1;
}

/*
 * ov01:0x00a2d7b0. Once the unit's base sound-effect bank file is loaded,
 * resolves every bank's "sed"/"swd" block address from the file's offset
 * table (bank i gets index i + 1), then, for an AGWS unit, relocates the
 * three bank files dataSndSeRegLoad loaded after it from their own headers.
 * The offset table is read as 8-byte records from the file start: record 0
 * is the count, so bank i's offsets are the record after record i. The
 * cursor starts at the file start, not at SndSeBankHeader.offset: the loop
 * head's `lw $2,8($7)` addresses from it.
 */
void dataSndSeRegLoad2(BattleUnit *unit)
{
    char name[16];
    SndSeRegDat *record;
    SndSeRegBank *bank;
    SndSeBankHeader *header;
    SndSeBankOffset *entry;
    int count;
    int i;

    dataSndSeNameGet(name, unit, SND_SE_TYPE_BASE);
    if (name[0] == 0) {
        return;
    }
    record = sndSeRegAdrGet(calcUPGet(unit)->charaId);
    if (record == 0) {
        return;
    }
    bank = &record->bank[0];
    header = bank->work.dest;
    count = header->count / 2;
    for (i = 0; i < count; i++) {
        entry = (SndSeBankOffset *)header + i;
        bank->sed = (unsigned char *)header + entry[1].sed;
        bank->swd = (unsigned char *)header + entry[1].swd;
        bank->work.seType = i + 1;
        bank++;
    }
    if (calcUPGet(unit)->flags & SND_SE_UNIT_FLAG_AGWS) {
        for (i = SND_SE_AGWS_BANK_COUNT - 1; i >= 0; i--) {
            if (bank->work.seType != SND_SE_TYPE_NONE) {
                header = bank->work.dest;
                bank->sed = (unsigned char *)header + header->offset.sed;
                bank->swd = (unsigned char *)header + header->offset.swd;
            }
            bank++;
        }
    }
}

extern int sndConvRegNo(int id);
extern int xglSoundSendSed(void *sed, int bank);
extern int SsdCheckSpuMemory(int verbose);

#define SND_SE_TRANS_SET_ERROR_FORMAT ">>>> SetTrans err\n"
#define SND_SE_TRANS_SED_TYPE_FORMAT "** sndSeTrans: err! sedType %X %X\n"
#define SND_SE_TRANS_SED_SIZE_FORMAT "** sndSeTrans: err! sedSize %d\n"
#define SND_SE_TRANS_SWD_TYPE_FORMAT "** sndSeTrans: err! swdType %X %X\n"
#define SND_SE_TRANS_RESULT_FORMAT "** sndSeTrans: %d %d\n"
#define SND_SE_TRANS_BANK_ERROR_FORMAT "** sndSeTrans: err -> (%d) %d\n"

/*
 * Sends the loaded sound-effect bank for seId of unit to the SPU. A
 * registered unit (sndSeRegChk) uses the entry sndConvRegNo(seId) of its
 * registration; any other sound uses the first entry of the shared record at
 * sndSeDat. An entry with no loaded file (seType 0) reports and returns 0, as
 * does a damage sound (seId 2) whose bank dmgBankOffs already marks, one
 * whose "sed" block lacks the tag 0x73646573 or is larger than 0x10000 bytes,
 * and one whose "swd" block lacks the tag 0x6D647773. Otherwise both blocks
 * are sent and the result is 1.
 */
int sndSeTrans(BattleUnit *unit, int seId)
{
    short charaId;
    SndSeRegBank *bank;
    int *sed;
    int *swd;
    int b;
    int r1;
    int r2;

    if (seId == 0) {
        return 0;
    }
    charaId = calcUPGet(unit)->charaId;
    if (sndSeRegChk(unit, seId) != 0) {
        bank = &sndSeRegAdrGet(charaId)->bank[sndConvRegNo(seId)];
    } else {
        bank = &sndSeDat.bank[0];
    }
    if (bank->work.seType != 0) {
        b = sndBankGet(unit, seId);
        if (seId == SND_SE_ID_DAMAGE && dmgBankOffs[b] != 0) {
            printf(SND_SE_TRANS_SET_ERROR_FORMAT);
            return 0;
        }
        sed = bank->sed;
        if (sed[0] != SND_SED_TAG) {
            printf(SND_SE_TRANS_SED_TYPE_FORMAT, sed[0], sed);
            return 0;
        }
        if (sed[2] > SND_SED_MAX_SIZE) {
            printf(SND_SE_TRANS_SED_SIZE_FORMAT, sed[2]);
            return 0;
        }
        swd = bank->swd;
        if (swd[0] != SND_SWD_TAG) {
            printf(SND_SE_TRANS_SWD_TYPE_FORMAT, swd[0], swd);
            return 0;
        }
        r1 = xglSoundSendSed(sed, b);
        r2 = xglSoundSendSwd(bank->swd, b);
        printf(SND_SE_TRANS_RESULT_FORMAT, r1, r2);
        if (r1 < 0 || r2 < 0) {
            SsdCheckSpuMemory(1);
        }
        return 1;
    }
    printf(SND_SE_TRANS_BANK_ERROR_FORMAT, charaId, seId);
    return 0;
}

extern void xglSoundEffectNormalID(int sound_id, int variant);

/* ov01:0x00a2da80. Plays system sound identifiers below 0x26 on variant 0
 * and reports whether the identifier was valid. */
int sndSysSePlay(int soundId)
{
    if (soundId >= SND_SYS_SE_COUNT) {
        return 0;
    }
    xglSoundEffectNormalID(soundId, 0);
    return 1;
}

/*
 * ov01:0x00a2dab0. Starts sound-effect id on the unit's bank. sndBankGet
 * resolves that bank from the unit the sound belongs to, which reaches
 * sndSePlay as the word sndSeTransPlay stores in the deferred task
 * (lw a0,28(s0) ov01:0x00a2e128) and is handed straight on (no argument setup
 * at the jal, ov01:0x00a2dac8). A damage sound
 * (id 2) plays once per bank: the first one marks the bank in dmgBankOffs and
 * every later one is dropped, and it does not advance the control id the other
 * sounds count with.
 */
int sndSePlay(BattleUnit *unit, int id, int no)
{
    int bank;

    if (id == 0) {
        return 0;
    }
    bank = sndBankGet(unit, id);
    if (id == SND_SE_ID_DAMAGE) {
        if (dmgBankOffs[bank] != 0) {
            return 0;
        }
        dmgBankOffs[bank] = 1;
    }
    xglSoundEffectNormalID((bank << 16) | no, ctrlId);
    printf(SND_SE_PLAY_FORMAT, bank, id, no, ctrlId);
    if (id != SND_SE_ID_DAMAGE) {
        ctrlId++;
    }
    return 1;
}

extern void xglSoundEffectStopID(int sound_id, int flags);

int sndSeStop(int unused, int pack)
{
    int stopped = 0;

    if (pack != 0) {
        xglSoundEffectStopID(sndBankGet() << 0x10, 0);
        printf(SND_SE_STOP_FORMAT, pack);
        stopped = 1;
    }
    return stopped;
}

/*
 * ov01:0x00a2dbc8. Searches a sentinel-terminated sound conversion pair
 * table and returns its mapped register number, or minus one.
 */
int sndConvRegNo(int id)
{
    SndConvRegTable table;
    int i;

    table = SND_CONV_REG_TABLE;
    for (i = 0; table.pair[i].key != 0; i++) {
        if (table.pair[i].key == id) {
            return table.pair[i].value;
        }
    }
    return -1;
}

/*
 * ov01:0x00a2dca0. Picks the sound-effect bank a sound id plays from. Ids
 * 5..22 of a player-side unit (no actor enemy flag, no CalcUnitParam AGWS flag)
 * use bank 2 unless the unit is a boss (charaId 0xBB..0xC2); ids 24, 25 and
 * 26 up use bank 2, ids 3..7 of an enemy actor too; id 1 uses bank 3, every
 * other id but 2 bank 1. The damage sound (id 2) gets one of the banks 4..6:
 * the one dmgBankOffs already keys to this charaId, else the first free one,
 * claimed here; bank 4 when none is free.
 */
int sndBankGet(BattleUnit *unit, int id)
{
    int bank;
    int i;

    if (id >= 5 && id < 23
        && !(unit->work->flags & ACTOR_FLAG_ENEMY)
        && !(calcUPGet(unit)->flags & SND_SE_UNIT_FLAG_AGWS)
        && (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN
            || calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END)) {
        bank = 2;
    } else if (id >= 26 || id == 24 || id == 25
               || (id >= 3 && id < 8
                   && (unit->work->flags & ACTOR_FLAG_ENEMY))) {
        bank = 2;
    } else if (id == 1) {
        bank = 3;
    } else if (id == SND_SE_ID_DAMAGE) {
        bank = SND_DMG_BANK_FIRST;
        for (i = 0; i < SND_DMG_BANK_COUNT; i++) {
            if (dmgBankOffs[i] == calcUPGet(unit)->charaId) {
                bank += i;
                break;
            }
        }
        if (i >= SND_DMG_BANK_COUNT) {
            for (i = 0; i < SND_DMG_BANK_COUNT; i++) {
                if (dmgBankOffs[i] == 0) {
                    bank += i;
                    dmgBankOffs[i] = calcUPGet(unit)->charaId;
                    break;
                }
            }
        }
    } else {
        bank = 1;
    }
    return bank;
}

typedef struct {
    unsigned char unmodeled_00[0x10]; /* +0x00 */
    short spid;                       /* +0x10 */
} ItmData;
extern ItmData *dataItmGet(int itmId);
extern int dataSpecBaseGet(int charaId);
extern int dataBakpBaseGet(int charaId);
#define SND_SPID_NORMAL  3
#define SND_SPID_SPECIAL 9
#define SND_SPID_BREAK   17
#define SND_SPID_EVENT   26
/* sndConvSpid modes: a technique, a technique's own index, an item's own index,
 * and the fixed sound. */
#define SND_SPID_MODE_TECHNIQUE 1
#define SND_SPID_MODE_TECH_EVENT 2
#define SND_SPID_MODE_ITEM_EVENT 3
#define SND_SPID_MODE_FIXED 9

/*
 * ov01:0x00a2de30. Resolves the sound-effect index a battle command plays:
 * a technique (mode 1) by its category, the technique's or the item's own
 * recorded index (modes 2 and 3) and a fixed one for mode 9; every other mode
 * has none.
 */
int sndConvSpid(BattleUnit *unit, int mode, int tecId, int itmId)
{
    short charaId;
    int spid = 0;
    int category;

    charaId = calcUPGet(unit)->charaId;
    switch (mode) {
    case SND_SPID_MODE_TECHNIQUE:
        category = dataTecGet(tecId)->category;
        switch (category) {
        case 6:
            spid = (tecId - dataBakpBaseGet(charaId)) + SND_SPID_BREAK;
            break;
        case 2:
            spid = (tecId - dataSpecBaseGet(charaId)) + SND_SPID_SPECIAL;
            break;
        case 4:
        case 5:
            spid = itmId + SND_SPID_NORMAL;
            break;
        default:
            spid = dataNormIdxGet(unit, tecId) + SND_SPID_NORMAL;
            break;
        }
        break;
    case SND_SPID_MODE_FIXED:
        spid = 2;
        break;
    case SND_SPID_MODE_TECH_EVENT:
        spid = dataTecGet(tecId)->spid + SND_SPID_EVENT;
        break;
    case SND_SPID_MODE_ITEM_EVENT:
        spid = dataItmGet(itmId)->spid + SND_SPID_EVENT;
        break;
    }
    return spid;
}

/* ov01:0x00a2df80. Picks the sound-effect normal-hit-reaction index for a
 * non-enemy unit outside the boss charaId range (0xBB..0xC2), returning
 * 0x21 when that index selects one of the two heavier reaction categories
 * (3 or 5); every other case passes seType through unchanged (result 1). */
int sndConvSe(BattleUnit *unit, int mode, int seType)
{
    int idx;
    int result = 1;

    if (mode == 1
        && !(unit->work->flags & ACTOR_FLAG_ENEMY)
        && !(calcUPGet(unit)->flags & SND_SE_UNIT_FLAG_AGWS)
        && (calcUPGet(unit)->charaId < CALC_SPECIAL_MODEL_CHARACTER_ID_BEGIN
            || calcUPGet(unit)->charaId >= CALC_SPECIAL_MODEL_CHARACTER_ID_END)) {
        idx = dataNormIdxGet(unit, seType);
        if (idx != -1) {
            if (idx == 3 || idx == 5) {
                result = SND_SE_REACT_HEAVY;
            }
        }
    }
    return result;
}

/* ov01:0x00a2e050. Returns hit sound 0x22 for normal indices three or five
 * under mode one; otherwise returns two. */
int sndConvHitSe(BattleUnit *unit, int mode, int seType)
{
    int idx;
    int result = 2;

    if (mode == 1) {
        idx = dataNormIdxGet(unit, seType);
        if (idx != -1) {
            if (idx == 3 || idx == 5) {
                result = SND_SE_HIT_HEAVY;
            }
        }
    }
    return result;
}

extern void *objEntryPure();
void sndSeTransPlayObj(SndSePlayTask *task);

void sndSeTransPlay(BattleUnit *unit, int seId, int pan)
{
    SndSePlayTask *task;

    if (sndSeTrans(unit, seId) != 0) {
        task = objEntryPure(sndSeTransPlayObj);
        task->unit = unit;
        task->seId = seId;
        task->pan = pan;
    }
}

void sndSeTransPlayObj(SndSePlayTask *task)
{
    if (SsdSpuDmaCompleted(0) != 0) {
        return;
    }
    sndSePlay(task->unit, task->seId, task->pan);
    objRemovePure(&task->base);
}

/*
 * ov01:0x00a2e160. Loads the three sound files of music set mode into
 * sndMuDat: the normal sequence, the alternate one the set may not have, and
 * the wave table, each laid out right behind the previous one and its size
 * rounded up to a 2048-byte CD sector. Returns whether the set exists.
 */
int dataSndMuLoad(int mode)
{
    SndMuData *mu;
    int smdFileSize;
    int altFileSize;
    int swdFileSize;
    int smdSectorSize;
    int altSectorSize;
    int swdSectorSize;

    if (mode >= SND_MU_SET_COUNT) {
        return 0;
    }
    mu = &sndMuDat;

    strcpy(fileName, sndMuNameBase);
    strcat(fileName, muName[mode]);
    strcat(fileName, sndMuNameExt1);
    dataFileLoadNB(fileName, mu->smdNormal);
    smdFileSize = xglCdGetFileSize(fileName);
    smdSectorSize = smdFileSize + SND_SECTOR_ROUND_BIAS;
    if (smdSectorSize < 0) {
        smdSectorSize = smdFileSize + SND_SECTOR_ROUND_NEG_BIAS;
    }
    smdSectorSize >>= SND_SECTOR_SHIFT;
    smdSectorSize <<= SND_SECTOR_SHIFT;
    mu->smdSize = smdSectorSize;
    mu->smdAlt = (unsigned char *)mu->smdNormal + smdSectorSize;

    if (mu2Name[mode] != 0) {
        strcpy(fileName, sndMuNameBase);
        strcat(fileName, mu2Name[mode]);
        strcat(fileName, sndMuNameExt1);
        dataFileLoadNB(fileName, mu->smdAlt);
        altFileSize = xglCdGetFileSize(fileName);
        altSectorSize = altFileSize + SND_SECTOR_ROUND_BIAS;
        if (altSectorSize < 0) {
            altSectorSize = altFileSize + SND_SECTOR_ROUND_NEG_BIAS;
        }
        altSectorSize >>= SND_SECTOR_SHIFT;
        altSectorSize <<= SND_SECTOR_SHIFT;
        mu->smdAltSize = altSectorSize;
    } else {
        mu->smdAltSize = 0;
    }
    mu->swd = (unsigned char *)mu->smdAlt + mu->smdAltSize;

    strcpy(fileName, sndMuNameBase);
    strcat(fileName, muName[mode]);
    strcat(fileName, sndMuNameExt2);
    dataFileLoadNB(fileName, mu->swd);
    swdFileSize = xglCdGetFileSize(fileName);
    mu->mode = mode;
    swdSectorSize = swdFileSize + SND_SECTOR_ROUND_BIAS;
    if (swdSectorSize < 0) {
        swdSectorSize = swdFileSize + SND_SECTOR_ROUND_NEG_BIAS;
    }
    swdSectorSize >>= SND_SECTOR_SHIFT;
    swdSectorSize <<= SND_SECTOR_SHIFT;
    mu->swdSize = swdSectorSize;
    return 1;
}

/* These initialized data objects occupy .data in the overlay even though
 * their original file bytes are all zero. Their types and extents follow the
 * existing field accesses and exact original symbol sizes. */
SndSeRegDat sndSeRegDat[SND_SE_REG_COUNT] = { 0 };
SndSeRegDat sndSeDat = { 0 };
SndMuData sndMuDat = { 0 };
int ctrlId = 0;
int dmgBankOffs[SND_DMG_BANK_COUNT] = { 0 };
int dmgBankStat[SND_DMG_BANK_COUNT] = { 0 };


int sndMuTrans(int mode)
{
    if (mode == 0) {
        xglSoundSendSmd(sndMuDat.smdNormal);
        xglSoundSendSwd(sndMuDat.swd, -1);
    } else {
        xglSoundSendSmd(sndMuDat.smdAlt);
    }
    return 1;
}

int sndMuPlay(void)
{
    xglSoundSequenceNormal(SND_MU_VOLUME_MAX);
    return 1;
}

int sndMuStop(void)
{
    xglSoundSequenceFadeOut(SND_MU_FADE_TIME);
    return 1;
}

void sndMuFadeIn(void)
{
    xglSoundSequenceNormal3(0, SND_MU_VOLUME_MAX, SND_MU_FADE_TIME);
}

void sndMuTransPlayObj(ObjectTask *task);

void sndMuTransPlay(int mode)
{
    if (sndMuTrans(mode) != 0) {
        printf(SND_MU_PLAY_FORMAT, mode);
        objEntryPure(sndMuTransPlayObj);
    }
}

void sndMuTransPlayObj(ObjectTask *task)
{
    if (SsdSpuDmaCompleted(0) != 0) {
        return;
    }
    sndMuPlay();
    objRemovePure(task);
}

extern const char sndCSText[8];
extern const char sndKMText[8];
extern const char sndSOText[8];
extern const char sndZ8Text[8];
extern const char sndMOText[8];
extern const char sndJRText[8];
extern const char sndBGText[8];
extern const char sndVX01Text[8];
extern const char sndVX02Text[8];
extern const char sndVX03Text[8];
extern const char sndVX04Text[8];
extern const char sndVX06Text[8];
extern const char sndAG02Text[8];
extern const char sndAG04Text[8];
extern const char sndAG05Text[8];
extern const char sndGUNO001Text[8];
extern const char sndGUNO002Text[8];
extern const char sndGUNO003Text[8];
extern const char sndGUNO004Text[8];
extern const char sndGUNO005Text[8];
extern const char sndGUNO006Text[8];
extern const char sndGUNO007Text[8];
extern const char sndGUNO008Text[8];
extern const char sndGUNO009Text[8];
extern const char sndGUNO010Text[8];
extern const char sndGUNO011Text[8];
extern const char sndGUNO012Text[8];
extern const char sndGUNO013Text[8];
extern const char sndGUNO014Text[8];
extern const char sndGUNO015Text[8];
extern const char sndGUNO016Text[8];
extern const char sndGUNO017Text[8];
extern const char sndGUNO018Text[8];
extern const char sndGUNO019Text[8];
extern const char sndGUNO020Text[8];
extern const char sndGUNO021Text[8];
extern const char sndGUNO022Text[8];
extern const char sndGUNO023Text[8];
extern const char sndGUNO024Text[8];
extern const char sndGUNO025Text[8];
extern const char sndGUNO101Text[8];
extern const char sndGUNO102Text[8];
extern const char sndGUNO103Text[8];
extern const char sndGUNO114Text[8];
extern const char sndFEMA001Text[8];
extern const char sndFEMA002Text[8];
extern const char sndFEMA005Text[8];
extern const char sndFERE001Text[8];
extern const char sndFERE002Text[8];
extern const char sndFERO001Text[8];
extern const char sndFESO001Text[8];
extern const char sndFESO002Text[8];
extern const char sndFESO003Text[8];
extern const char sndFESO004Text[8];
extern const char sndUTMA001Text[8];
extern const char sndUTMA002Text[8];
extern const char sndUTMA003Text[8];
extern const char sndUTMA004Text[8];
extern const char sndUTMA005Text[8];
extern const char sndUTMA006Text[8];
extern const char sndUTMA007Text[8];
extern const char sndUTMA008Text[8];
extern const char sndUTMA009Text[8];
extern const char sndUTMA011Text[8];
extern const char sndUTMA012Text[8];
extern const char sndUTMA013Text[8];
extern const char sndUTRE002Text[8];
extern const char sndUTRE003Text[8];
extern const char sndUTRE004Text[8];
extern const char sndUTRE005Text[8];
extern const char sndUTRO001Text[8];
extern const char sndUTRO002Text[8];
extern const char sndUTRO003Text[8];
extern const char sndUTRO004Text[8];
extern const char sndUTRO005Text[8];
extern const char sndUTRO006Text[8];
extern const char sndUTRO007Text[8];
extern const char sndUTSO001Text[8];
extern const char sndUTSO002Text[8];
extern const char sndUTSO003Text[8];
extern const char sndUTSO004Text[8];
extern const char sndUTSO005Text[8];
extern const char sndUTSO006Text[8];
extern const char sndUTSO007Text[8];
extern const char sndMONS001Text[8];
extern const char sndMONS002Text[8];
extern const char sndMONS003Text[8];
extern const char sndMONS004Text[8];
extern const char sndMONS005Text[8];
extern const char sndMONS006Text[8];
extern const char sndMONS007Text[8];
extern const char sndBOSS001Text[8];
extern const char sndBOSS002Text[8];
extern const char sndBOSS003Text[8];
extern const char sndBOSS004Text[8];
extern const char sndBOSS005Text[8];
extern const char sndBOSS007Text[8];
extern const char sndBOSS009Text[8];
extern const char sndBOSS010Text[8];
extern const char sndBOSS011Text[8];
extern const char sndBOSS012Text[8];
extern const char sndBOSS013Text[8];
extern const char sndBOSS014Text[8];
extern const char sndBOSS016Text[8];
extern const char sndBOSS017Text[8];
extern const char sndBOSS018Text[8];
extern const char sndBOSS019Text[8];
extern const char sndBOSS020Text[8];
extern const char sndBOSS021Text[8];
extern const char sndBOSS024Text[8];
extern const char sndBOSS025Text[8];
extern const char sndBOSS026Text[8];
extern const char sndBOSS029Text[8];
extern const char sndBOSS030Text[8];
extern const char sndBOSS033Text[8];
extern const char sndBOSS103Text[8];
extern const char sndBOSS104Text[8];
extern const char sndWP2Text[8];
extern const char sndDMText[8];
extern const char sndSP7Text[8];
extern const char sndSP4Text[8];
extern const char sndJpText[8];
extern const char sndAT1Text[8];
extern const char sndCVText[8];
extern const char sndWP3Text[8];
extern const char sndWP4Text[8];
extern const char sndWP1Text[8];
extern const char sndSP6Text[8];
extern const char sndSP8Text[8];
extern const char sndSP3Text[8];
extern const char sndAT2Text[8];
extern const char sndSP2Text[8];
extern const char sndMVText[8];
extern const char sndWP5Text[8];
extern const char sndWP6Text[8];
extern const char sndSP5Text[8];
extern const char sndAT56Text[8];
extern const char sndAT34Text[8];
extern const char sndSP1Text[8];
extern const char sndDText[8];
extern const char sndMText[8];
extern const char sndCsBstText[];
extern const char sndKmBstText[];
extern const char sndSoBstText[];
extern const char sndZ8BstText[];
extern const char sndMoBstText[];
extern const char sndJrBstText[];
extern const char sndCsBedText[];
extern const char sndKmBedText[];
extern const char sndSoBedText[];
extern const char sndZ8BedText[];
extern const char sndMoBedText[];
extern const char sndJrBedText[];

char * sndUnitSeBaseName[1] = { 0 };

static const char *plChaBaseSeName[16] = {
    sndCSText, sndKMText, sndSOText, 0,
    sndZ8Text, sndMOText, sndJRText, 0,
    0, sndBGText, 0, 0,
    0, 0, 0, 0
};

static const char *plAgwsBaseSeName[16] = {
    sndVX01Text, sndVX02Text, sndVX03Text, sndVX04Text,
    0, sndVX06Text, 0, sndAG02Text,
    0, sndAG04Text, sndAG05Text, 0,
    0, 0, 0, 0
};

static const char *enemyBaseSeName[161] = {
    0, sndGUNO001Text, sndGUNO002Text, sndGUNO003Text,
    sndGUNO004Text, sndGUNO005Text, sndGUNO006Text, sndGUNO007Text,
    sndGUNO008Text, sndGUNO009Text, sndGUNO010Text, sndGUNO011Text,
    sndGUNO012Text, sndGUNO013Text, sndGUNO014Text, sndGUNO015Text,
    sndGUNO016Text, sndGUNO017Text, sndGUNO018Text, sndGUNO019Text,
    sndGUNO020Text, sndGUNO021Text, sndGUNO022Text, sndGUNO023Text,
    sndGUNO024Text, sndGUNO025Text, sndGUNO101Text, sndGUNO102Text,
    sndGUNO103Text, sndGUNO114Text, sndFEMA001Text, sndFEMA002Text,
    0, 0, sndFEMA005Text, 0,
    sndFERE001Text, sndFERE002Text, 0, sndFERO001Text,
    sndFESO001Text, sndFESO002Text, sndFESO003Text, sndFESO004Text,
    sndUTMA001Text, sndUTMA002Text, sndUTMA003Text, sndUTMA004Text,
    sndUTMA005Text, sndUTMA006Text, sndUTMA007Text, sndUTMA008Text,
    sndUTMA009Text, 0, sndUTMA011Text, sndUTMA012Text,
    sndUTMA013Text, 0, 0, sndUTRE002Text,
    sndUTRE003Text, sndUTRE004Text, sndUTRE005Text, 0,
    sndUTRO001Text, sndUTRO002Text, sndUTRO003Text, sndUTRO004Text,
    sndUTRO005Text, sndUTRO006Text, sndUTRO007Text, sndUTSO001Text,
    sndUTSO002Text, sndUTSO003Text, sndUTSO004Text, sndUTSO005Text,
    sndUTSO006Text, sndUTSO007Text, sndMONS001Text, sndMONS002Text,
    sndMONS003Text, sndMONS004Text, sndMONS005Text, sndMONS006Text,
    sndMONS007Text, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, sndBOSS001Text, sndBOSS002Text,
    sndBOSS003Text, sndBOSS004Text, sndBOSS005Text, 0,
    sndBOSS007Text, 0, sndBOSS009Text, sndBOSS010Text,
    sndBOSS011Text, sndBOSS012Text, sndBOSS013Text, sndBOSS014Text,
    0, sndBOSS016Text, sndBOSS017Text, sndBOSS018Text,
    sndBOSS019Text, sndBOSS020Text, sndBOSS021Text, 0,
    0, sndBOSS024Text, sndBOSS025Text, sndBOSS026Text,
    0, 0, sndBOSS029Text, sndBOSS030Text,
    0, 0, sndBOSS033Text, sndBOSS103Text,
    sndBOSS104Text, 0, 0, 0,
    0, 0, 0, 0,
    0
};

static const char *plChaMidSeName[24] = {
    sndJpText, sndMVText, sndDMText, sndAT1Text, sndAT2Text, sndAT34Text,
    sndAT34Text, sndAT56Text, sndAT56Text, sndSP1Text, sndSP2Text, sndSP3Text,
    sndSP4Text, sndSP5Text, sndSP6Text, sndSP7Text, sndSP8Text, sndWP1Text,
    sndWP2Text, sndWP3Text, sndWP4Text, sndWP5Text, sndWP6Text, sndCVText
};

static const char *plAgwsMidSeName[3] = { sndJpText, sndMText, sndDText };

static const char *enemyMidSeName[3] = { sndJpText, sndMText, sndDText };

static SndStEdData btst[7] = {
    { .unitId = 1, .name = sndCsBstText, .condition = { 1, 1, 1, 1, 9, 0, 0, 0 } },
    { .unitId = 2, .name = sndKmBstText, .condition = { 1, 1, 3, 6, 9, 0, 0, 0 } },
    { .unitId = 3, .name = sndSoBstText, .condition = { 1, 1, 1, 2, 6, 10, 12, 9 } },
    { .unitId = 5, .name = sndZ8BstText, .condition = { 1, 1, 1, 11, 9, 0, 0, 0 } },
    { .unitId = 6, .name = sndMoBstText, .condition = { 1, 1, 1, 5, 11, 9, 0, 0 } },
    { .unitId = 7, .name = sndJrBstText, .condition = { 1, 1, 1, 12, 10, 9, 0, 0 } },
    { .unitId = 0, .name = 0, .condition = { 0, 0, 0, 0, 0, 0, 0, 0 } }
};

static SndStEdData bted[7] = {
    { .unitId = 1, .name = sndCsBedText, .condition = { 1, 1, 1, 7, 0, 0, 0, 0 } },
    { .unitId = 2, .name = sndKmBedText, .condition = { 1, 1, 3, 7, 8, 0, 0, 0 } },
    { .unitId = 3, .name = sndSoBedText, .condition = { 1, 1, 1, 7, 8, 0, 0, 0 } },
    { .unitId = 5, .name = sndZ8BedText, .condition = { 1, 1, 1, 7, 0, 0, 0, 0 } },
    { .unitId = 6, .name = sndMoBedText, .condition = { 1, 1, 1, 4, 7, 0, 0, 0 } },
    { .unitId = 7, .name = sndJrBedText, .condition = { 1, 1, 1, 7, 0, 0, 0, 0 } },
    { .unitId = 0, .name = 0, .condition = { 0, 0, 0, 0, 0, 0, 0, 0 } }
};

const char sndBGText[8] = "BG";

const char sndJRText[8] = "JR";

const char sndMOText[8] = "MO";

const char sndZ8Text[8] = "Z8";

const char sndSOText[8] = "SO";

const char sndKMText[8] = "KM";

const char sndCSText[8] = "CS";

const char sndAG05Text[8] = "AG_05";

const char sndAG04Text[8] = "AG_04";

const char sndAG02Text[8] = "AG_02";

const char sndVX06Text[8] = "VX_06";

const char sndVX04Text[8] = "VX_04";

const char sndVX03Text[8] = "VX_03";

const char sndVX02Text[8] = "VX_02";

const char sndVX01Text[8] = "VX_01";

const char sndBOSS104Text[8] = "BOSS104";

const char sndBOSS103Text[8] = "BOSS103";

const char sndBOSS033Text[8] = "BOSS033";

const char sndBOSS030Text[8] = "BOSS030";

const char sndBOSS029Text[8] = "BOSS029";

const char sndBOSS026Text[8] = "BOSS026";

const char sndBOSS025Text[8] = "BOSS025";

const char sndBOSS024Text[8] = "BOSS024";

const char sndBOSS021Text[8] = "BOSS021";

const char sndBOSS020Text[8] = "BOSS020";

const char sndBOSS019Text[8] = "BOSS019";

const char sndBOSS018Text[8] = "BOSS018";

const char sndBOSS017Text[8] = "BOSS017";

const char sndBOSS016Text[8] = "BOSS016";

const char sndBOSS014Text[8] = "BOSS014";

const char sndBOSS013Text[8] = "BOSS013";

const char sndBOSS012Text[8] = "BOSS012";

const char sndBOSS011Text[8] = "BOSS011";

const char sndBOSS010Text[8] = "BOSS010";

const char sndBOSS009Text[8] = "BOSS009";

const char sndBOSS007Text[8] = "BOSS007";

const char sndBOSS005Text[8] = "BOSS005";

const char sndBOSS004Text[8] = "BOSS004";

const char sndBOSS003Text[8] = "BOSS003";

const char sndBOSS002Text[8] = "BOSS002";

const char sndBOSS001Text[8] = "BOSS001";

const char sndMONS007Text[8] = "MONS007";

const char sndMONS006Text[8] = "MONS006";

const char sndMONS005Text[8] = "MONS005";

const char sndMONS004Text[8] = "MONS004";

const char sndMONS003Text[8] = "MONS003";

const char sndMONS002Text[8] = "MONS002";

const char sndMONS001Text[8] = "MONS001";

const char sndUTSO007Text[8] = "UTSO007";

const char sndUTSO006Text[8] = "UTSO006";

const char sndUTSO005Text[8] = "UTSO005";

const char sndUTSO004Text[8] = "UTSO004";

const char sndUTSO003Text[8] = "UTSO003";

const char sndUTSO002Text[8] = "UTSO002";

const char sndUTSO001Text[8] = "UTSO001";

const char sndUTRO007Text[8] = "UTRO007";

const char sndUTRO006Text[8] = "UTRO006";

const char sndUTRO005Text[8] = "UTRO005";

const char sndUTRO004Text[8] = "UTRO004";

const char sndUTRO003Text[8] = "UTRO003";

const char sndUTRO002Text[8] = "UTRO002";

const char sndUTRO001Text[8] = "UTRO001";

const char sndUTRE005Text[8] = "UTRE005";

const char sndUTRE004Text[8] = "UTRE004";

const char sndUTRE003Text[8] = "UTRE003";

const char sndUTRE002Text[8] = "UTRE002";

const char sndUTMA013Text[8] = "UTMA013";

const char sndUTMA012Text[8] = "UTMA012";

const char sndUTMA011Text[8] = "UTMA011";

const char sndUTMA009Text[8] = "UTMA009";

const char sndUTMA008Text[8] = "UTMA008";

const char sndUTMA007Text[8] = "UTMA007";

const char sndUTMA006Text[8] = "UTMA006";

const char sndUTMA005Text[8] = "UTMA005";

const char sndUTMA004Text[8] = "UTMA004";

const char sndUTMA003Text[8] = "UTMA003";

const char sndUTMA002Text[8] = "UTMA002";

const char sndUTMA001Text[8] = "UTMA001";

const char sndFESO004Text[8] = "FESO004";

const char sndFESO003Text[8] = "FESO003";

const char sndFESO002Text[8] = "FESO002";

const char sndFESO001Text[8] = "FESO001";

const char sndFERO001Text[8] = "FERO001";

const char sndFERE002Text[8] = "FERE002";

const char sndFERE001Text[8] = "FERE001";

const char sndFEMA005Text[8] = "FEMA005";

const char sndFEMA002Text[8] = "FEMA002";

const char sndFEMA001Text[8] = "FEMA001";

const char sndGUNO114Text[8] = "GUNO114";

const char sndGUNO103Text[8] = "GUNO103";

const char sndGUNO102Text[8] = "GUNO102";

const char sndGUNO101Text[8] = "GUNO101";

const char sndGUNO025Text[8] = "GUNO025";

const char sndGUNO024Text[8] = "GUNO024";

const char sndGUNO023Text[8] = "GUNO023";

const char sndGUNO022Text[8] = "GUNO022";

const char sndGUNO021Text[8] = "GUNO021";

const char sndGUNO020Text[8] = "GUNO020";

const char sndGUNO019Text[8] = "GUNO019";

const char sndGUNO018Text[8] = "GUNO018";

const char sndGUNO017Text[8] = "GUNO017";

const char sndGUNO016Text[8] = "GUNO016";

const char sndGUNO015Text[8] = "GUNO015";

const char sndGUNO014Text[8] = "GUNO014";

const char sndGUNO013Text[8] = "GUNO013";

const char sndGUNO012Text[8] = "GUNO012";

const char sndGUNO011Text[8] = "GUNO011";

const char sndGUNO010Text[8] = "GUNO010";

const char sndGUNO009Text[8] = "GUNO009";

const char sndGUNO008Text[8] = "GUNO008";

const char sndGUNO007Text[8] = "GUNO007";

const char sndGUNO006Text[8] = "GUNO006";

const char sndGUNO005Text[8] = "GUNO005";

const char sndGUNO004Text[8] = "GUNO004";

const char sndGUNO003Text[8] = "GUNO003";

const char sndGUNO002Text[8] = "GUNO002";

const char sndGUNO001Text[8] = "GUNO001";

const char sndCVText[8] = "_CV";

const char sndWP6Text[8] = "_WP6";

const char sndWP5Text[8] = "_WP5";

const char sndWP4Text[8] = "_WP4";

const char sndWP3Text[8] = "_WP3";

const char sndWP2Text[8] = "_WP2";

const char sndWP1Text[8] = "_WP1";

const char sndSP8Text[8] = "_SP8";

const char sndSP7Text[8] = "_SP7";

const char sndSP6Text[8] = "_SP6";

const char sndSP5Text[8] = "_SP5";

const char sndSP4Text[8] = "_SP4";

const char sndSP3Text[8] = "_SP3";

const char sndSP2Text[8] = "_SP2";

const char sndSP1Text[8] = "_SP1";

const char sndAT56Text[8] = "_AT5_6";

const char sndAT34Text[8] = "_AT3_4";

const char sndAT2Text[8] = "_AT2";

const char sndAT1Text[8] = "_AT1";

const char sndDMText[8] = "_DM";

const char sndMVText[8] = "_MV";

const char sndJpText[8] = "";

const char sndDText[8] = "D";

const char sndMText[8] = "M";

const char sndJrBstText[8] = "jr_bst";

const char sndMoBstText[8] = "mo_bst";

const char sndZ8BstText[8] = "z8_bst";

const char sndSoBstText[8] = "so_bst";

const char sndKmBstText[8] = "km_bst";

const char sndCsBstText[8] = "cs_bst";

const char sndJrBedText[8] = "jr_bed";

const char sndMoBedText[8] = "mo_bed";

const char sndZ8BedText[8] = "z8_bed";

const char sndSoBedText[8] = "so_bed";

const char sndKmBedText[8] = "km_bed";

const char sndCsBedText[16] = "cs_bed";
