/*
 * OV01 original TU 5: 0x00a191c0..0x00a1d348 (93 functions)
 */
#include "common.h"
#include "data_unit_org_get.h"
#include "ov01/calc.h"
#include "main/xgl_pad.h"

extern const char D_00A457B0[32];

/*
 * orgData is a 33-entry table of 0x180-byte unit-origin records; callers
 * pass a one-based index, so dataUnitOrgGet returns orgData[id - 1].
 */
typedef struct UnitOrgRecord {
    unsigned char unmodeled_0[0x180];
} UnitOrgRecord;

static const char D_00A456A8[32];
extern UnitOrgRecord orgData[];

/* This string precedes dataEtherTecSet's jump table in the original rodata. */
static const char D_00A456A8[32] = "** dataUnitOrgGet: err %d\n";

void *dataUnitOrgGet(int unitOrgId)
{
    if (unitOrgId >= 0x21) {
        printf(D_00A456A8, unitOrgId);
        return 0;
    }
    return &orgData[unitOrgId - 1];
}

extern const char D_00A456C8[];
extern PlCharacter plChaData[];

PlCharacter *dataPlChaGet(int cid)
{
    if (cid >= PL_CHARACTER_ID_END) {
        printf(D_00A456C8, cid);
        return 0;
    }
    return &plChaData[cid - 1];
}

/* Battle-data header offsets are byte offsets from the loaded block at
 * 0x00426F40. Public symbols name the offset words, not the block itself.
 * Other record contents remain opaque until a consumer evidences their fields. */
typedef struct BattleDataHeader {
    unsigned int sectionCount;
    unsigned int byteSize;
    int unitInitOffset;
    int techniqueOffset;
    int normalInitOffset;
    int specialInitOffset;
    int etherInitOffset;
    int weaponOffset;
    int ethOffset;
    int itmOffset;
    int attOffset;
    int accessoryOffset;
    int skillOffset;
    int gainOffset;
    int experienceOffset;
    int parameterOffset;
    int specialTableOffset;
    int defaultEquipmentOffset;
    int frameOffset;
    int engOffset;
} BattleDataHeader;

extern unsigned char batDatBuf[0x10000];

#define BATTLE_DATA_FIELD_OFFSET(member) ((unsigned int)&((BattleDataHeader *)0)->member)

typedef struct NormalInitRecord { unsigned char opaque[0x0C]; } NormalInitRecord;
typedef struct SpecialInitRecord { unsigned char opaque[0x0C]; } SpecialInitRecord;
typedef struct EtherInitRecord { unsigned char opaque[0x18]; } EtherInitRecord;
typedef struct ParameterRecord { unsigned char opaque[0x20]; } ParameterRecord;
typedef struct DefaultEquipmentRecord { unsigned char opaque[0x20]; } DefaultEquipmentRecord;
typedef struct SpecialTableRecord { unsigned char opaque[0x20]; } SpecialTableRecord;

extern int D_426F48;

UnitInitData *dataUnitInitGet(int entry)
{
    unsigned char *block = (unsigned char *)&D_426F48 - BATTLE_DATA_FIELD_OFFSET(unitInitOffset);
    int byteOffset = D_426F48;
    UnitInitData *records = (UnitInitData *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

extern int D_426F50;

NormalInitRecord *dataNormInitGet(int entry)
{
    unsigned char *block = (unsigned char *)&D_426F50 - BATTLE_DATA_FIELD_OFFSET(normalInitOffset);
    int byteOffset = D_426F50;
    NormalInitRecord *records = (NormalInitRecord *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

extern int D_426F54;

SpecialInitRecord *dataSpecInitGet(int entry)
{
    unsigned char *block = (unsigned char *)&D_426F54 - BATTLE_DATA_FIELD_OFFSET(specialInitOffset);
    int byteOffset = D_426F54;
    SpecialInitRecord *records = (SpecialInitRecord *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

extern int D_426F58;

EtherInitRecord *dataEtherInitGet(int entry)
{
    unsigned char *block = (unsigned char *)&D_426F58 - BATTLE_DATA_FIELD_OFFSET(etherInitOffset);
    int byteOffset = D_426F58;
    EtherInitRecord *records = (EtherInitRecord *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

/*
 * D_426F78 is a selector field 0x38 bytes into a loaded battle-data table;
 * dataExpTblGet returns the table entry that selector picks out. Splat has
 * not named the table's own base, only this field.
 */
extern int D_426F78;

unsigned char *dataExpTblGet(void)
{
    return (unsigned char *)&D_426F78 - 0x38 + D_426F78;
}

extern int D_426F7C;

ParameterRecord *dataParaTblGet(int entry)
{
    unsigned char *block = (unsigned char *)&D_426F7C - BATTLE_DATA_FIELD_OFFSET(parameterOffset);
    int byteOffset = D_426F7C;
    ParameterRecord *records = (ParameterRecord *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

extern int D_426F84;

DefaultEquipmentRecord *dataDefEquipGet(int entry)
{
    unsigned char *block = (unsigned char *)&D_426F84 - BATTLE_DATA_FIELD_OFFSET(defaultEquipmentOffset);
    int byteOffset = D_426F84;
    DefaultEquipmentRecord *records = (DefaultEquipmentRecord *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

extern int D_426F80;

SpecialTableRecord *dataSpecTblGet(int entry)
{
    unsigned char *block = (unsigned char *)&D_426F80 - BATTLE_DATA_FIELD_OFFSET(specialTableOffset);
    int byteOffset = D_426F80;
    SpecialTableRecord *records = (SpecialTableRecord *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

extern void dataEtherLearnSet(int etherType, int techniqueId, int selector);
/* The scaffold-owned D_00A456C8 string lies between this and the first
 * definition above; the following switch table is emitted after this string. */
static const char D_00A456E8[40] = "dataEtherTecSet: err %d\n";
extern CalcUnitParam *calcUPGet(ObjectTask *unit);
extern void *dataUnitFileGet(ObjectTask *unit, int charaId);
extern int dataUnitFileLoadMot(ObjectTask *unit, int motionId, int slot, void *fileInfo);

extern void dataSpecLearnSet(int cid, int specialId);

void dataEtherTecSet(int selector)
{
    int etherType = 0;
    int techniqueId = 0;
    int specialId = 0;

    switch (selector) {
    case 1:
        etherType = 3;
        techniqueId = 13;
        break;
    case 2:
        etherType = 3;
        techniqueId = 14;
        break;
    case 3:
        etherType = 3;
        techniqueId = 15;
        break;
    case 4:
        etherType = 3;
        techniqueId = 16;
        break;
    case 5:
        etherType = 6;
        techniqueId = 43;
        specialId = 90;
        break;
    case 6:
        etherType = 6;
        techniqueId = 46;
        specialId = 89;
        break;
    case 7:
        etherType = 7;
        techniqueId = 57;
        specialId = 97;
        break;
    case 8:
        etherType = 6;
        specialId = 88;
        break;
    default:
        printf(D_00A456E8, selector);
        return;
    }

    if (techniqueId != 0) {
        dataEtherLearnSet(etherType, techniqueId, selector);
    }
    if (specialId != 0) {
        dataSpecLearnSet(etherType, specialId);
    }
}

extern const char D_00A45730[];

/* The third argument is supplied by dataEtherTecSet but is not read here. */
void dataEtherLearnSet(int cid, int techniqueId, int selector)
{
    PlCharacter *pl;
    int index;

    if (cid >= PL_LEARNING_CHARACTER_ID_END) {
        printf(D_00A45730, cid);
        return;
    }
    if (techniqueId != 0) {
        pl = dataPlChaGet(cid);
        index = techniqueId - 1;
        pl->learnedEther[index / 8] |= 1 << (index % 8);
    }
}

extern const char D_00A45750[];

int dataEtherLearnGet(int cid, int techniqueId)
{
    PlCharacter *pl;
    int index;

    if (cid >= PL_LEARNING_CHARACTER_ID_END) {
        printf(D_00A45750, cid);
        return 0;
    }
    pl = dataPlChaGet(cid);
    index = techniqueId - 1;
    return pl->learnedEther[index / 8] & (1 << (index % 8));
}

extern const char D_00A45770[];

void dataSkillLearnSet(int cid, int skillId)
{
    PlCharacter *pl;
    int index;

    if (cid >= PL_LEARNING_CHARACTER_ID_END) {
        printf(D_00A45770, cid);
        return;
    }
    if (skillId != 0) {
        pl = dataPlChaGet(cid);
        index = skillId - 1;
        pl->learnedSkill[index / 8] |= 1 << (index % 8);
    }
}

extern const char D_00A45790[];

int dataSkillLearnGet(int cid, int skillId)
{
    PlCharacter *pl;
    int index;

    if (cid >= PL_LEARNING_CHARACTER_ID_END) {
        printf(D_00A45790, cid);
        return 0;
    }
    pl = dataPlChaGet(cid);
    index = skillId - 1;
    return pl->learnedSkill[index / 8] & (1 << (index % 8));
}

#define THINK_MAP_COUNT 100

/* The last map number overlaps specBase[0] at +0x18C. These are two
 * views of the same stored word, not adjacent independent arrays. */
typedef union ThinkMapTable {
    int mapNo[THINK_MAP_COUNT];
    struct {
        int mapPrefix[THINK_MAP_COUNT - 1];
        int specBase[1]; /* +0x18C; the original object ends after this word */
    } special;
} ThinkMapTable;

static ThinkMapTable thinkMapTbl;

int dataSpecBaseGet(int cid)
{
    return thinkMapTbl.special.specBase[cid];
}

/*
 * specTbl's tail is the character-id-indexed backup special-technique base
 * table dataBakpBaseGet reads.
 */
typedef struct SpecTable {
    int specialBase[0x3C / sizeof(int)];
    int bakpBase[1]; /* +0x3C, indexed dynamically */
} SpecTable;

static SpecTable specTbl;

int dataBakpBaseGet(int cid)
{
    return specTbl.bakpBase[cid];
}

void dataSpecLearnSet(int cid, int specialId)
{
    PlCharacter *pl;

    if (cid >= PL_LEARNING_CHARACTER_ID_END) {
        printf(D_00A457B0, cid);
        return;
    }
    if (specialId != 0) {
        pl = dataPlChaGet(cid);
        pl->special[specialId - dataSpecBaseGet(cid)].id = specialId;
    }
}

/*
 * thinkMapTbl's tail is a per-character-id table of special-technique base
 * ids; dataSpecLearnGet subtracts the character's base from specialId
 * before indexing PlCharacter.special, the same subtraction
 * dataSpecLearnSet performs through dataSpecBaseGet.
 */
static const char D_00A457D0[32];

short dataSpecLearnGet(int cid, int specialId)
{
    short learned;

    if (cid >= PL_LEARNING_CHARACTER_ID_END) {
        printf(D_00A457D0, cid);
        return 0;
    }
    learned = 0;
    if (specialId != 0) {
        learned = dataPlChaGet(cid)->special[specialId - thinkMapTbl.special.specBase[cid]].id;
    }
    return learned;
}

PlSpecialSlot *dataSpecDataGet(int cid, int specialId)
{
    PlCharacter *pl;
    int i;

    if (cid >= PL_LEARNING_CHARACTER_ID_END) {
        return 0;
    }
    if (specialId == 0) {
        return 0;
    }
    pl = dataPlChaGet(cid);
    for (i = 0; i < PL_SPECIAL_SLOT_COUNT; i++) {
        if (pl->special[i].id == specialId) {
            return &pl->special[i];
        }
    }
    return 0;
}

int dataNormIdxGet(ObjectTask *unit, int normalId)
{
    int i;
    int result = -1;

    for (i = 0; i < CALC_NORMAL_TECHNIQUE_COUNT; i++) {
        if (calcUPGet(unit)->normalTechniqueId[i] == normalId) {
            result = i;
            break;
        }
    }
    return result;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPlUnitInit);

void dataDummySet(void)
{
}

/*
 * playerData (7 entries) and enemyData (5 entries) are 0x180-byte battle
 * unit records; dataBattleInit is this TU's only writer and only clears the
 * calc-record index at +0x38.
 */
typedef struct BattleUnitInfo {
    unsigned char unmodeled_00[0x38];
    short calcIdx; /* +0x38 */
    unsigned char unmodeled_3A[0x146];
} BattleUnitInfo;

extern void dataFileInfoInit(void);
static BattleUnitInfo playerData[7];
static BattleUnitInfo enemyData[5];
static UnitFileInfo unitFileInfo[6];
static int cdReqNum;
static int leaderCid;
static int padMode;
static short padData;

void dataBattleInit(void)
{
    int i;

    dataFileInfoInit();
    for (i = 6; i >= 0; i--) {
        playerData[i].calcIdx = 0;
    }
    for (i = 4; i >= 0; i--) {
        enemyData[i].calcIdx = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitInitSet);

/*
 * posTbl is a table of 0x20-byte position records; dataPosTblGet and
 * dataPosLineGet both index it with the absolute value of the caller's
 * position id (negative ids alias the same record as their positive
 * counterpart). dataPosLineGet reads the halfword at +0x10 of the record.
 */
typedef struct PosTblEntry {
    float position[4]; /* +0x00, four coordinates used by unit positioning */
    short line; /* +0x10 */
    unsigned char unmodeled_12[0x20 - 0x12];
} PosTblEntry;

static PosTblEntry posTbl[];

void *dataPosTblGet(int id)
{
    if (id < 0) {
        id = -id;
    }
    return &posTbl[id];
}

short dataPosLineGet(int id)
{
    if (id < 0) {
        id = -id;
    }
    return posTbl[id].line;
}

static const unsigned char D_00A45890[24]; /* "** dataCidGet: err %d\n" */
static short cidChgTbl[];

short dataCidGet(unsigned int cid)
{
    if (cid >= 0xC4) {
        printf(D_00A45890, cid);
        return 0;
    }
    return cidChgTbl[cid];
}

static const unsigned char D_00A458A8[24]; /* "** dataWidGet: err %d" */
static short widChgTbl[];

short dataWidGet(unsigned int wid)
{
    if (wid >= 0x75) {
        printf(D_00A458A8, wid);
        return 0;
    }
    return widChgTbl[wid];
}

/*
 * dataMotNameGetMenu reads motionId as a short at +0x14. calcBp2Tid
 * (src/ov01/calc.c, this TU's TecKindView) reads the same +0x11 byte this
 * file's dataFileLoadWepSp/dataFileLoadWepSp2 compare against 6 and names
 * it kind (a BP timeline/special-weapon selector); kept as the same name
 * and signed-byte width here since this file owns the rest of the layout.
 */
typedef struct TecRecord {
    unsigned char unmodeled_00[0x11];
    signed char kind; /* +0x11 */
    unsigned char unmodeled_12[0x14 - 0x12];
    short motionId; /* +0x14 */
    unsigned char unmodeled_16[0x18 - 0x16];
} TecRecord;

extern int D_426F4C;
extern const char D_00A458C0[];

TecRecord *dataTecGet(int entry)
{
    unsigned char *block;
    int byteOffset;
    TecRecord *records;

    if (entry <= 0) {
        printf(D_00A458C0, entry);
        return 0;
    }
    block = (unsigned char *)&D_426F4C - BATTLE_DATA_FIELD_OFFSET(techniqueOffset);
    byteOffset = D_426F4C;
    records = (TecRecord *)(block + byteOffset);
    return &records[entry - 1];
}

/* dataMotNameGetMenu reads the weapon's technique id as a short at +0x14.
 * dataUnitFileLoad loads a weapon slot's own technique motion only when the
 * short at +0xA is 1 (`lh $3,0xA($2)` / `bne $3,$21`, $21 = 1). */
typedef struct WpnRecord {
    unsigned char unmodeled_00[0xA];
    short motionType; /* +0xA */
    unsigned char unmodeled_0c[0x14 - 0xC];
    short tecId; /* +0x14 */
    unsigned char unmodeled_16[0x20 - 0x16];
} WpnRecord;

extern int D_426F5C;
extern const char D_00A458D8[];

WpnRecord *dataWpnGet(int entry)
{
    unsigned char *block;
    int byteOffset;
    WpnRecord *records;

    if (entry <= 0) {
        printf(D_00A458D8, entry);
        return 0;
    }
    block = (unsigned char *)&D_426F5C - BATTLE_DATA_FIELD_OFFSET(weaponOffset);
    byteOffset = D_426F5C;
    records = (WpnRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct AttRecord { unsigned char opaque[0x0E]; } AttRecord;

extern int D_426F68;
extern const char D_00A458F0[];

AttRecord *dataAttGet(int entry)
{
    unsigned char *block;
    int byteOffset;
    AttRecord *records;

    if (entry <= 0) {
        printf(D_00A458F0, entry);
        return 0;
    }
    block = (unsigned char *)&D_426F68 - BATTLE_DATA_FIELD_OFFSET(attOffset);
    byteOffset = D_426F68;
    records = (AttRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct EthRecord { unsigned char opaque[0x0C]; } EthRecord;

extern int D_426F60;
extern const char D_00A45908[];

EthRecord *dataEthGet(int entry)
{
    unsigned char *block;
    int byteOffset;
    EthRecord *records;

    if (entry <= 0) {
        printf(D_00A45908, entry);
        return 0;
    }
    block = (unsigned char *)&D_426F60 - BATTLE_DATA_FIELD_OFFSET(ethOffset);
    byteOffset = D_426F60;
    records = (EthRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct ItmRecord { unsigned char opaque[0x14]; } ItmRecord;

extern int D_426F64;
extern const char D_00A45920[];

ItmRecord *dataItmGet(int entry)
{
    unsigned char *block;
    int byteOffset;
    ItmRecord *records;

    if (entry <= 0) {
        printf(D_00A45920, entry);
        return 0;
    }
    block = (unsigned char *)&D_426F64 - BATTLE_DATA_FIELD_OFFSET(itmOffset);
    byteOffset = D_426F64;
    records = (ItmRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct AccRecord { unsigned char opaque[0x10]; } AccRecord;

extern int D_426F6C;
extern const char D_00A45938[];

AccRecord *dataAccGet(int entry)
{
    unsigned char *block;
    int byteOffset;
    AccRecord *records;

    if (entry <= 0) {
        printf(D_00A45938, entry);
        return 0;
    }
    block = (unsigned char *)&D_426F6C - BATTLE_DATA_FIELD_OFFSET(accessoryOffset);
    byteOffset = D_426F6C;
    records = (AccRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct FrmRecord { unsigned char opaque[0x10]; } FrmRecord;

extern int D_426F88;
extern const char D_00A45950[];

FrmRecord *dataFrmGet(int entry)
{
    unsigned char *block;
    int byteOffset;
    FrmRecord *records;

    if (entry <= 0) {
        printf(D_00A45950, entry);
        return 0;
    }
    block = (unsigned char *)&D_426F88 - BATTLE_DATA_FIELD_OFFSET(frameOffset);
    byteOffset = D_426F88;
    records = (FrmRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct EngRecord { unsigned char opaque[0x0C]; } EngRecord;

extern int D_426F8C;
extern const char D_00A45968[];

EngRecord *dataEngGet(int entry)
{
    unsigned char *block;
    int byteOffset;
    EngRecord *records;

    if (entry <= 0) {
        printf(D_00A45968, entry);
        return 0;
    }
    block = (unsigned char *)&D_426F8C - BATTLE_DATA_FIELD_OFFSET(engOffset);
    byteOffset = D_426F8C;
    records = (EngRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct SklRecord { unsigned char opaque[0x08]; } SklRecord;

extern int D_426F70;
extern const char D_00A45980[];

SklRecord *dataSklGet(int entry)
{
    unsigned char *block;
    int byteOffset;
    SklRecord *records;

    if (entry <= 0) {
        printf(D_00A45980, entry);
        return 0;
    }
    block = (unsigned char *)&D_426F70 - BATTLE_DATA_FIELD_OFFSET(skillOffset);
    byteOffset = D_426F70;
    records = (SklRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct GainRecord { unsigned char opaque[0x14]; } GainRecord;

/*
 * D_426F74 is a selector field 0x34 bytes into a loaded battle-data table;
 * dataGainGet's table starts 0x294 bytes before this field's own byte
 * offset, the same idiom dataExpTblGet uses for its own table. Splat has
 * not named the table's own base, only this field.
 */
extern int D_426F74;
extern const char D_00A45998[];

GainRecord *dataGainGet(int entry)
{
    unsigned char *block;
    int byteOffset;
    GainRecord *records;

    if (entry <= 0) {
        printf(D_00A45998, entry);
        return 0;
    }
    block = (unsigned char *)&D_426F74 - BATTLE_DATA_FIELD_OFFSET(gainOffset);
    byteOffset = D_426F74;
    records = (GainRecord *)(block + byteOffset);
    return (GainRecord *)((unsigned char *)&records[entry] - 0x294);
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFileLoad);

int dataCdSync(void)
{
    return cdReqNum;
}

void dataCdSyncClear(void)
{
    cdReqNum = 0;
}

extern int printf(const char *format, ...);
static const char D_00A459E0[24];
static const char D_00A459F8[24];

void dataNBreadCB(int result) {
    if (result < 0) {
        printf(D_00A459E0, result);
        return;
    }
    if (result == 4) {
        if (--cdReqNum == 0) {
            printf(D_00A459F8, 0);
        }
    }
}

extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);
extern const char D_00A45A10[];
extern const char D_00A45A28[];

int dataFileLoadNB(const char *filename, void *destination)
{
    /* xglCdReadFile's published declaration (include/umbrella/public.h:161,
     * also include/main/xgl_cd.h:6) types the fourth argument `int flags`;
     * this call (like src/main/window_tex_load.c:348 and src/main/srs.c:582,
     * the lane's other published callers passing a callback here) passes
     * dataNBreadCB's completion callback through that int, so the cast
     * stays rather than retyping a shared, out-of-lane header. */
    int result = xglCdReadFile(filename, destination, 1, (int)dataNBreadCB);

    if (result < 0) {
        printf(D_00A45A10, filename);
        return 0;
    }
    cdReqNum++;
    printf(D_00A45A28, cdReqNum, filename, destination);
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFileInfoInit);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFileInfoInitChg);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataEnemyMdlChk);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataLeaderChk);

int dataLeaderCidGet(void) {
    return leaderCid;
}

void dataLeaderCidReset(void) {
    leaderCid = 0;
}

extern unsigned char *PartyDataGet(void);
extern const char D_00A45AB0[];

/* src/main/party.c owns PartyDataPrefix; this TU's own overlay reaches
 * only the leader battle id PartyLeaderSet/PartyLeaderCheck evidence at
 * +0x2C. */
typedef struct PartyLeaderView {
    unsigned char unmodeled_00[0x2C];
    unsigned short leader_battle_id; /* +0x2C */
} PartyLeaderView;

void dataLeaderReload(void)
{
    unsigned short leaderBattleId = ((PartyLeaderView *)PartyDataGet())->leader_battle_id;

    if (leaderBattleId != leaderCid) {
        printf(D_00A45AB0);
    }
}

extern char *strcpy(char *destination, const char *source);
extern char *strcat(char *destination, const char *source);
extern int dataMotNameGetSub(char *name, CalcUnitParam *up, int motionId, int weaponId);
static char *motNameBase;
static char *motNameExt;

/*
 * Menu-side motion name lookup. The unit-origin record is the unit's own
 * CalcUnitParam (0x180 bytes, include/ov01/calc.h), so the weapon slot
 * selects weaponId[]; the weapon record names its technique and the
 * technique record names the motion.
 */
int dataMotNameGetMenu(char *name, int orgId, int weaponSlot)
{
    CalcUnitParam *origin;
    int weaponId;
    int tecId;
    int motionId;
    int found;

    *name = 0;
    origin = (CalcUnitParam *)dataUnitOrgGet(orgId);
    weaponId = origin->weaponId[weaponSlot];
    if (weaponId == 0) {
        return 0;
    }
    tecId = dataWpnGet(weaponId)->tecId;
    motionId = dataTecGet(tecId)->motionId;
    if (motionId == 0) {
        return 0;
    }
    strcpy(name, motNameBase);
    found = dataMotNameGetSub(name, origin, motionId, weaponSlot);
    strcat(name, motNameExt);
    if (found == 0) {
        *name = 0;
    }
    return found;
}

extern char *strcpy(char *destination, const char *source);
extern char *strcat(char *destination, const char *source);
extern int dataMotNameGetSub(char *name, CalcUnitParam *up, int motionId, int weaponId);
static char *motNameBase;
static char *motNameExt;

int dataMotNameGet(char *name, ObjectTask *unit, int motionId, int weaponId)
{
    int found;

    strcpy(name, motNameBase);
    found = dataMotNameGetSub(name, calcUPGet(unit), motionId, weaponId);
    strcat(name, motNameExt);
    if (found == 0) {
        *name = 0;
    }
    return found;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataMotNameGetSub);

extern int unitCidGet(ObjectTask *unit);
extern int dataUnitFileLoadMdl(UnitEquipInfo *unit, int charaId, UnitFileInfo *file);
extern int dataUnitFileLoadWep(ObjectTask *unit, int weaponId, int slot, UnitFileInfo *fileInfo);
/* The leader's character id (`lw $3,0x0($16)`, $16 = &leaderId), compared
 * with charaId; a static of the original TU at 0x00a597b8, still asm-owned. */
extern int leaderId;

/*
 * Load a unit's model, its weapons (except kind-6 techniques' weapons), its
 * base motion and the motions of weapons that carry their own (motion type
 * 1 with a kind-4 technique, into motion slot weapon slot + 1).
 */
int dataUnitFileLoad(ObjectTask *unit, int charaId)
{
    UnitFileInfo *file;
    int weaponId;
    int i;

    file = dataUnitFileGet(unit, charaId);
    if (file == 0) {
        return 0;
    }
    if (leaderId == charaId && leaderId != calcUPGet(unit)->charaId) {
        dataLeaderCidReset();
    }
    charaId = calcUPGet(unit)->charaId;
    dataUnitFileLoadMdl((UnitEquipInfo *)unit, unitCidGet(unit), file);
    for (i = 0; i < 3; i++) {
        weaponId = calcUPGet(unit)->weaponId[i];
        if (weaponId != 0 && dataTecGet(dataWpnGet(weaponId)->tecId)->kind != 6) {
            dataUnitFileLoadWep(unit, weaponId, i, file);
        }
    }
    dataUnitFileLoadMot(unit, charaId, 0, file);
    for (i = 0; i < 3; i++) {
        weaponId = calcUPGet(unit)->weaponId[i];
        if (weaponId != 0 && dataWpnGet(weaponId)->motionType == 1
            && dataTecGet(dataWpnGet(weaponId)->tecId)->kind == 4) {
            dataUnitFileLoadMot(unit, dataTecGet(dataWpnGet(weaponId)->tecId)->motionId, i + 1, file);
        }
    }
    return 1;
}

static const char D_00A45AC8[32];

void *dataUnitFileGet(ObjectTask *unit, int charaId)
{
    UnitFileInfo *slot;
    int lo;
    int hi;
    int i;

    slot = 0;
    for (i = 0; i < 6; i++) {
        if (unitFileInfo[i].charaId == charaId) {
            slot = &unitFileInfo[i];
            slot->charaId = calcUPGet(unit)->charaId;
            break;
        }
    }
    if (slot == 0) {
        if (charaId < 0x21 || charaId >= 0xBB) {
            lo = 0;
            hi = 3;
        } else {
            lo = 3;
            hi = 6;
        }
        for (i = lo; i < hi; i++) {
            if (unitFileInfo[i].charaId == 0) {
                slot = &unitFileInfo[i];
                slot->charaId = calcUPGet(unit)->charaId;
                break;
            }
        }
        if (slot == 0) {
            printf(D_00A45AC8);
        }
    }
    return slot;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPackPcMdlNameGet);

/* pcPack_1 is a 38-entry table (0x130 bytes, 8-byte stride), terminated by
 * a zero weaponId; dataPackWpnMdlNameGet searches it and returns the packed
 * model name for a matching weapon. */
typedef struct PcPackEntry {
    int weaponId;
    char *name;
} PcPackEntry;

extern PcPackEntry pcPack_1[];

char *dataPackWpnMdlNameGet(int weaponId)
{
    char *name;
    int index;

    name = 0;
    for (index = 0; pcPack_1[index].weaponId != 0; index++) {
        if (weaponId == pcPack_1[index].weaponId) {
            name = pcPack_1[index].name;
            break;
        }
    }
    return name;
}

extern void ACT_initMotion(BattleModelActor *actor);

#define CD_SECTOR_SIZE 0x800
/* Leader model data is already resident: these are the animation and
 * texture buffers selected by the leader path, not file identifiers. */
#define LEADER_ANIMATION_ADDRESS ((void *)0x01070800)
#define LEADER_TEXTURE_ADDRESS ((void *)0x010B1000)
/* dataUnitFileLoadFaceMdl's cache-miss buffers: lui $3,(0x10B4000>>16) /
 * ori $3,$3,(0x10B4000&0xFFFF) builds the model address below. */
#define FACE_MODEL_ADDRESS ((void *)0x010B4000)
/* dataUnitFileLoadFaceMdl's cache-miss buffers: lui $4,(0x10E6000>>16) /
 * ori $4,$4,(0x10E6000&0xFFFF) builds the texture address below (stored to
 * actor->textureAdr, not actor->animationAdr). */
#define FACE_TEXTURE_ADDRESS ((void *)0x010E6000)

extern char mdlFileName[];
extern char mdlFileName2[];
extern char *pcNameBase;
extern char D_00A44898[];
extern const char D_00A45DD0[];
extern const char D_00A45DF8[];
extern const char D_00A45E00[];
extern const char D_00A45E08[];
extern const char D_00A45E10[];
extern const char D_00A45E38[];
extern char *dataPackPcMdlNameGet(ObjectTask *unit);
extern void RES_GetMdlFileName(char *name, int charaId);
extern int xglCdGetFileSize(const char *name);
extern int dataUnitFileLoadFaceMdl(ObjectTask *unit, int charaId, UnitFileInfo *file);

/*
 * Reuse cached model data when the character id agrees. Otherwise use
 * the leader's resident data, a packed player model, or separate model,
 * animation and texture files, rounding each loaded file to a CD sector.
 * Repeated calcUPGet calls and signed division preserve the original
 * call sequence and sector-rounding behavior.
 */
int dataUnitFileLoadMdl(UnitEquipInfo *unit, int charaId, UnitFileInfo *file)
{
    BattleModelActor *actor;
    char *packedName;

    actor = unit->motionActor;
    if (charaId != file->modelCharaId) {
        file->modelCharaId = charaId;
        if (((unit->task.battle.work->flags & BATTLE_ACTOR_FLAG_ENEMY) == 0
             && (calcUPGet(&unit->task.object)->flags & CALC_UNIT_FLAG_AGWS) == 0
             && (calcUPGet(&unit->task.object)->charaId < SPECIAL_MODEL_CHARACTER_ID_BEGIN
                 || calcUPGet(&unit->task.object)->charaId >= SPECIAL_MODEL_CHARACTER_ID_END))
            || (calcUPGet(&unit->task.object)->charaId >= SPECIAL_MODEL_CHARACTER_ID_BEGIN
                && calcUPGet(&unit->task.object)->charaId < SPECIAL_MODEL_CHARACTER_ID_END)) {
            if (dataLeaderCidGet() == charaId) {
                file->animationAdr = LEADER_ANIMATION_ADDRESS;
                file->textureAdr = LEADER_TEXTURE_ADDRESS;
                printf(D_00A45DD0, charaId);
            } else {
                packedName = dataPackPcMdlNameGet(&unit->task.object);
                strcpy(mdlFileName2, pcNameBase);
                strcat(mdlFileName2, packedName);
                strcat(mdlFileName2, D_00A44898);
                dataFileLoadNB(mdlFileName2, file->modelAdr);
                file->modelSize = ((xglCdGetFileSize(mdlFileName2) + (CD_SECTOR_SIZE - 1))
                               / CD_SECTOR_SIZE) * CD_SECTOR_SIZE;
                file->animationAdr = 0;
                file->textureAdr = 0;
            }
        } else {
            RES_GetMdlFileName(mdlFileName, charaId);
            strcpy(mdlFileName2, mdlFileName);
            strcat(mdlFileName2, D_00A45DF8);
            dataFileLoadNB(mdlFileName2, file->modelAdr);
            file->modelSize = ((xglCdGetFileSize(mdlFileName2) + (CD_SECTOR_SIZE - 1))
                               / CD_SECTOR_SIZE) * CD_SECTOR_SIZE;
            file->animationAdr = (unsigned char *)file->modelAdr + file->modelSize;
            strcpy(mdlFileName2, mdlFileName);
            strcat(mdlFileName2, D_00A45E00);
            dataFileLoadNB(mdlFileName2, file->animationAdr);
            file->animationSize = ((xglCdGetFileSize(mdlFileName2) + (CD_SECTOR_SIZE - 1))
                               / CD_SECTOR_SIZE) * CD_SECTOR_SIZE;
            file->textureAdr = (unsigned char *)file->animationAdr + file->animationSize;
            strcpy(mdlFileName2, mdlFileName);
            strcat(mdlFileName2, D_00A45E08);
            dataFileLoadNB(mdlFileName2, file->textureAdr);
            file->textureSize = ((xglCdGetFileSize(mdlFileName2) + (CD_SECTOR_SIZE - 1))
                               / CD_SECTOR_SIZE) * CD_SECTOR_SIZE;
        }
    } else {
        printf(D_00A45E10, charaId, file->modelAdr);
    }
    actor->modelAdr = file->modelAdr;
    actor->animationAdr = file->animationAdr;
    actor->textureAdr = file->textureAdr;
    ACT_initMotion(actor);
    printf(D_00A45E38, file->modelSize + file->animationSize + file->textureSize);
    if ((unit->task.battle.work->flags & BATTLE_ACTOR_FLAG_ENEMY) == 0
        && (calcUPGet(&unit->task.object)->flags & CALC_UNIT_FLAG_AGWS) == 0
        && (calcUPGet(&unit->task.object)->charaId < SPECIAL_MODEL_CHARACTER_ID_BEGIN
                 || calcUPGet(&unit->task.object)->charaId >= SPECIAL_MODEL_CHARACTER_ID_END)) {
        dataUnitFileLoadFaceMdl(&unit->task.object, charaId, file);
    } else {
        file->faceCharaId = 0;
    }
    return 1;
}

extern const char D_00A45E50[];
extern void *dataChildActorCreate(int callerId);

int dataUnitFileLoadFaceMdl(ObjectTask *unit, int charaId, UnitFileInfo *file)
{
    /* The caller (dataUnitFileLoadMdl, this file:996) passes
     * &unit->task.object; ObjectTask is the first member of UnitEquipInfo's
     * `task` union (this file's header, data_unit_org_get.h:150-151), so
     * that ObjectTask pointer is the UnitEquipInfo record itself. */
    UnitEquipInfo *equip = (UnitEquipInfo *)unit;
    BattleModelActor *actor;

    if (charaId != file->faceCharaId) {
        file->faceCharaId = charaId;
        if (dataLeaderCidGet() == charaId) {
            file->faceModelAdr = FACE_MODEL_ADDRESS;
            file->faceTextureAdr = FACE_TEXTURE_ADDRESS;
        }
    } else {
        printf(D_00A45E50, charaId, file->faceModelAdr);
    }

    actor = dataChildActorCreate(charaId);
    actor->modelAdr = file->faceModelAdr;
    equip->faceActor = actor;
    actor->animationAdr = file->animationAdr;
    actor->textureAdr = file->faceTextureAdr;
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadMot);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadWep);

int dataWpnLRChk(ObjectTask *unit, int weaponId, int slot, WpnFileInfo *fileInfo)
{
    (void)unit;
    if (slot != 1 || weaponId != fileInfo->weaponId) {
        return 0;
    }
    return 1;
}

extern BattleModelActor *ACT_create(int parent, int callerId);
static const char D_00A45F38[32];

void *dataChildActorCreate(int callerId)
{
    BattleModelActor *actor;

    actor = ACT_create(-1, callerId);
    if (actor == 0) {
        printf(D_00A45F38);
        return 0;
    }
    ACT_initMotion(actor);
    actor->state.update = 0;
    actor->state.draw = 0;
    actor->state.flags = 0x208;
    return actor;
}

int dataUnitFileLoadMotSp(ObjectTask *unit, int motionId, int slot) {
    dataUnitFileLoadMot(unit, motionId, slot, dataUnitFileGet(unit, calcUPGet(unit)->charaId));
    return 1;
}

int dataUnitFileLoadMotSp2(UnitEquipInfo *unit, int motionId, int slot, int p3, int p4)
{
    BattleModelActor *actor;

    dataMotAdrSet(unit->motionActor, slot);
    if (p4 == 5) {
        actor = unit->equipActor[0];
        if (actor != 0) {
            actor->motion.weapon.motionAdr = unit->motionActor->motion.slot[slot];
        }
        actor = unit->equipActor[1];
        if (actor != 0) {
            actor->motion.weapon.motionAdr = unit->motionActor->motion.slot[slot];
        }
    } else if (p3 >= 0) {
        actor = unit->equipActor[p3 & 3];
        if (actor != 0) {
            actor->motion.weapon.motionAdr = unit->motionActor->motion.slot[slot];
        }
    }
    return 1;
}

extern const char D_00A45F58[];
extern void dataUnitFileLoad2Sub(ObjectTask *unit);

/* unitCmdSet's queued-command record: src/ov01/unit_cmd.h:157-162 (CmdEntry)
 * and src/ov01/calc.c:28-34 (CalcCmdEntry) both restate this same layout
 * for their own calls into unitCmdSet (still asm, src/ov01/unit_cmd.c), so
 * this TU forward-declares the identical selector/argument/extra fields
 * rather than inventing its own. This call only writes selector and
 * argument; extra and the trailing 8 bytes are unwritten here, matching
 * calc.c's own "unwritten by the one evidenced call" note for the same
 * tail. */
typedef struct CmdEntry {
    int selector; /* +0x00 */
    int argument; /* +0x04 */
    int extra;    /* +0x08: unwritten by this call */
    unsigned char unmodeled_0c[8]; /* +0x0C: unwritten by this call */
} CmdEntry;
extern void unitCmdSet(ObjectTask *unit, CmdEntry *cmd);
extern int rnd(int max);

void dataUnitFileLoad2(ObjectTask *unit)
{
    CmdEntry cmd;

    printf(D_00A45F58, unit);
    dataUnitFileLoad2Sub(unit);

    cmd.selector = 0x25;
    if (!(((BattleActor *)unit->work)->flags & BATTLE_ACTOR_FLAG_ENEMY)) {
        cmd.argument = 0;
    } else {
        cmd.argument = rnd(0xA);
    }
    unitCmdSet(unit, &cmd);
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoad2Sub);

/*
 * A packed model or animation file: 8 bytes not recovered, then one byte
 * offset per packed entry, counted from the start of the header, the same
 * idiom dataFpkAdrGet below resolves for a single entry.
 */
typedef struct PackedFile {
    unsigned char unmodeled_00[8];
    int entryOffset[3]; /* +0x08 */
} PackedFile;

void dataPackWpnMdl2(BattleModelActor *actor)
{
    PackedFile *pack;
    int offset;

    pack = (PackedFile *)actor->modelAdr;
    offset = pack->entryOffset[0];
    actor->modelAdr = (unsigned char *)pack + offset;
    offset = pack->entryOffset[1];
    actor->animationAdr = (unsigned char *)pack + offset;
    offset = pack->entryOffset[2];
    actor->textureAdr = (unsigned char *)pack + offset;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataMotAdrSet);

/*
 * dataFpkAdrGet resolves a packed file's data pointer: 8 bytes not
 * recovered, then the offset of the packed data from the start of the
 * header.
 */
typedef struct FpkHeader {
    unsigned char unmodeled_00[8];
    int data_offset;
} FpkHeader;

void *dataFpkAdrGet(FpkHeader *fpk)
{
    return (unsigned char *)fpk + fpk->data_offset;
}

/*
 * One 12-byte entry of the special-weapon table, terminated by a zero
 * tecId. dataTid2WepSp only reads tecId; dataFileLoadWepSp* (still asm)
 * read the words at +0x04 and +0x08.
 */
typedef struct WepSpEntry {
    int tecId;                       /* +0x00 */
    unsigned char unmodeled_04[8];
} WepSpEntry;

extern WepSpEntry wepSpTbl[];        /* original storage, still asm-owned */

/* Returns the special-weapon entry of technique `tecId`, or 0. */
WepSpEntry *dataTid2WepSp(int tecId)
{
    int i;

    for (i = 0; wepSpTbl[i].tecId != 0; i++) {
        if (wepSpTbl[i].tecId == tecId)
            return &wepSpTbl[i];
    }
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFileLoadWepSp);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFileLoadWepSp2);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFileLoadWepSpEnd);

/* Callers pass the unit whose default weapon they want; the function
 * always returns 0 and ignores it. */
int dataDefWpnGet(void *unit)
{
    return 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataMtdRead);

typedef struct XtxFileEntry {
    const char *name;
    unsigned int address;
} XtxFileEntry;

static XtxFileEntry xtxFile[];

void *dataXtxFileGet(int index)
{
    return &xtxFile[index];
}

static const char D_00A45FE8[32];
static unsigned short xtxAdr[0x13];

unsigned short dataXtxAdrGet(int index)
{
    unsigned short value;

    value = 0;
    if (index < 0x13) {
        if (index >= 0) {
            value = xtxAdr[index];
        } else {
            goto fail;
        }
    } else {
    fail:
        printf(D_00A45FE8, index);
    }
    return value;
}

/* Starts a non-blocking load of every xtxFile entry up to the null name. */
void dataXtxLoad(void)
{
    int i;

    for (i = 0; xtxFile[i].name != 0; i++)
        dataFileLoadNB(xtxFile[i].name, (void *)xtxFile[i].address);
}

extern int dataFileLoad(const char *name, void *dst);

static const char D_00A46008[32];

void dataBatDatLoad(void)
{
    dataFileLoad(D_00A46008, batDatBuf);
}

extern unsigned char thinkBuf[0x4000];

void *dataThinkAdrGet(void)
{
    return thinkBuf;
}

int thinkMapGet(int mapNo)
{
    int i;
    int *map = thinkMapTbl.mapNo;
    for (i = 0; i < THINK_MAP_COUNT; i++) {
        if (mapNo == map[i]) {
            return i;
        }
    }
    return -1;
}

extern int thinkMapGet(int mapNo);
extern char *strcpy(char *destination, const char *source);
extern char *strcat(char *destination, const char *source);
extern int sprintf(char *buffer, const char *format, ...);
static char *thinkName;
static char *thinkNameBase;
static char *thinkNameExt;
static const char D_00A46028[8];

int dataThinkNameGet(char *name, int thinkNo)
{
    char num[16];
    int mappedNo;

    mappedNo = thinkMapGet(thinkNo);
    if (mappedNo != -1) {
        strcpy(name, thinkNameBase);
        strcat(name, thinkName);
        sprintf(num, D_00A46028, mappedNo);
        strcat(name, num);
        strcat(name, thinkNameExt);
        return 1;
    }
    name[0] = '\0';
    return 0;
}

extern void thinkNoSet(int mappedNo);

/* Install the 16 KiB behavior-data block and select its remapped number. */
int dataThinkDataSet(int thinkNo, const void *data)
{
    int mappedNo;

    mappedNo = thinkMapGet(thinkNo);
    if (mappedNo != -1 && data != 0) {
        __builtin_memcpy(thinkBuf, data, sizeof(thinkBuf));
        thinkNoSet(mappedNo);
        return 1;
    }
    return 0;
}

/* dataThinkLoad: defined below in this TU. */
void dataThinkLoad(int thinkNo);

/* Load the remapped behavior file (dataThinkLoad) and select its number. */
int dataThinkFileLoad(int thinkNo)
{
    int mappedNo;

    mappedNo = thinkMapGet(thinkNo);
    if (mappedNo != -1) {
        dataThinkLoad(mappedNo);
        thinkNoSet(mappedNo);
        return 1;
    }
    return 0;
}

static char *thinkName;
static char *thinkNameBase;
static char *thinkNameExt;
static const char D_00A46028[8];

void dataThinkLoad(int thinkNo)
{
    char path[256];
    char num[16];

    strcpy(path, thinkNameBase);
    strcat(path, thinkName);
    sprintf(num, D_00A46028, thinkNo);
    strcat(path, num);
    strcat(path, thinkNameExt);
    dataFileLoad(path, thinkBuf);
}

/* dataMapLoad passes the map number it is loading; every map loads into
 * the same fixed EE RAM region, so the number is ignored. */
void *dataMapLoadAdrGet(int mapNo)
{
    return (void *)0x01b9e000;
}

typedef struct MapHeader {
    unsigned char unmodeled_00[0x10];
    int cameraOffset; /* +0x10 */
} MapHeader;

void *dataMapCameraAdrGet(int mapNo)
{
    MapHeader *map = dataMapLoadAdrGet(mapNo);
    return (unsigned char *)map + map->cameraOffset;
}

static char *mapNameBase;
static char *mapName;
static char *mapNameExt;
static const char D_00A46028[8];
static const char D_00A46030[32];

void dataMapLoad(int mapNo)
{
    char path[256];
    char num[16];

    if (mapNo < 100) {
        strcpy(path, mapNameBase);
        strcat(path, mapName);
        sprintf(num, D_00A46028, mapNo);
        strcat(path, num);
        strcat(path, mapNameExt);
        dataFileLoadNB(path, dataMapLoadAdrGet(mapNo));
        return;
    }
    printf(D_00A46030, mapNo);
}

#include "data_unit_org_get_data.inc"


void dataVPadModeSet(int mode)
{
    padMode = mode;
    padData = 0;
}

#define VIRTUAL_PAD_BIT_COUNT 14

extern unsigned short padConvTbl[VIRTUAL_PAD_BIT_COUNT];
void dataVPadSet(int virtualPad)
{
    unsigned short result = 0;
    int shift = 0;
    const unsigned short *entry = padConvTbl;
    virtualPad &= 0xFFFF;
    /* Original entry at 0x00A1D2A0 enters the body before testing the
     * loop count; keep the table pointer advance and 16-bit mask. */
    do {
        if ((virtualPad >> shift) & 1) {
            result |= *entry;
        }
        shift++;
        entry++;
    } while (shift < VIRTUAL_PAD_BIT_COUNT);
    padData = result;
}

/* MAIN's two pad records (0x00490d90, 2 * 0x68 bytes). */
extern XglPadRecord PadData[2];

/*
 * Pad 0's direction word: 0 the held directions, 1 the newly pressed ones,
 * 2 the auto-repeated ones. In virtual-pad mode (dataVPadModeSet) the word
 * dataVPadSet built is returned instead.
 */
int dataPadRead(int padNo)
{
    int value = 0;

    if (padMode == 0) {
        switch (padNo) {
        case 0:
            value = PadData[0].directions;
            break;
        case 1:
            value = PadData[0].direction_pressed;
            break;
        case 2:
            value = PadData[0].direction_repeated;
            break;
        }
    } else {
        value = (unsigned short)padData;
    }
    return value;
}



char D_00A44880[24] = "data\\yamamoto\\mot\\";

char D_00A44898[8] = ".bin";

char D_00A448A0[24] = "data\\yamamoto\\map\\";

char D_00A448B8[8] = "map";

char D_00A448C0[24] = "data\\yamamoto\\think\\";

char D_00A448D8[8] = "think";

char D_00A448E0[32] = "data\\yamamoto\\stat\\cur.xtx";

char D_00A44900[32] = "data\\yamamoto\\stat\\manu.xtx";

char D_00A44920[32] = "data\\yamamoto\\stat\\stat.xtx";
