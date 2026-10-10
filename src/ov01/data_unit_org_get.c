/*
 * OV01 original TU 5: 0x00a191c0..0x00a1d348 (93 functions)
 */
#include "common.h"
#include "data_unit_org_get.h"
#include "ov01/calc.h"

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
#define BATTLE_DATA_FIELD_OFFSET(member) ((unsigned int)&((BattleDataHeader *)0)->member)

typedef struct BattleDataHeader {
    unsigned char unmodeled_00[8];
    int unitInitOffset; /* +0x08 */
    unsigned char unmodeled_0c[4];
    int normalInitOffset; /* +0x10 */
    int specialInitOffset; /* +0x14 */
    int etherInitOffset; /* +0x18 */
    unsigned char unmodeled_1c[0x38 - 0x1C];
    int experienceOffset; /* +0x38 */
    int parameterOffset; /* +0x3C */
    int specialTableOffset; /* +0x40 */
    int defaultEquipmentOffset; /* +0x44 */
} BattleDataHeader;

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataTecGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataWpnGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataAttGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataEthGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataItmGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataAccGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFrmGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataEngGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSklGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataGainGet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFileLoadNB);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataLeaderReload);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataMotNameGetMenu);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoad);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPackWpnMdlNameGet);

extern void ACT_initMotion(BattleModelActor *actor);

#define CD_SECTOR_SIZE 0x800
/* Leader model data is already resident: these are the animation and
 * texture buffers selected by the leader path, not file identifiers. */
#define LEADER_ANIMATION_ADDRESS ((void *)0x01070800)
#define LEADER_TEXTURE_ADDRESS ((void *)0x010B1000)

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadFaceMdl);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoad2);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataTid2WepSp);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataXtxLoad);

extern int dataFileLoad(const char *name, void *dst);

static const char D_00A46008[32];
extern unsigned char batDatBuf[0x10000];

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataThinkFileLoad);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPadRead);



char D_00A44880[24] = "data\\yamamoto\\mot\\";

char D_00A44898[8] = ".bin";

char D_00A448A0[24] = "data\\yamamoto\\map\\";

char D_00A448B8[8] = "map";

char D_00A448C0[24] = "data\\yamamoto\\think\\";

char D_00A448D8[8] = "think";

char D_00A448E0[32] = "data\\yamamoto\\stat\\cur.xtx";

char D_00A44900[32] = "data\\yamamoto\\stat\\manu.xtx";

char D_00A44920[32] = "data\\yamamoto\\stat\\stat.xtx";
