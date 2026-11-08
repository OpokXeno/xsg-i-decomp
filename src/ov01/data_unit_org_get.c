/*
 * OV01 original TU 5: 0x00a191c0..0x00a1d348 (93 functions)
 */
#include "common.h"
#include "ov01/actor.h"
#include "data_unit_org_get.h"
#include "ov01/calc.h"
#include "main/xgl_pad.h"
#include "main/party.h"
#include "main/sdv.h"
#include "quadword.h"

extern const char unitOrgDataSpecLearnSetErrText[32];

/*
 * orgData is a 33-entry table of 0x180-byte unit record table entries; callers
 * pass a one-based index, so dataUnitOrgGet returns orgData[id - 1].
 */
#define UNIT_ORG_RECORD_SIZE 0x180
#define UNIT_ORG_COUNT 0x21

typedef struct UnitOrgRecord {
    unsigned char unmodeled_0[0x180];
} UnitOrgRecord;

static const char unitOrgDataUnitOrgGetErrText[32];
extern UnitOrgRecord orgData[];

static const char unitOrgDataUnitOrgGetErrText[32] = "** dataUnitOrgGet: err %d\n";

/* Returns the org record for a one-based unit org id, or 0 after printing an error when the id is out of range. */
void *dataUnitOrgGet(int unitOrgId)
{
    if (unitOrgId >= UNIT_ORG_COUNT) {
        printf(unitOrgDataUnitOrgGetErrText, unitOrgId);
        return 0;
    }
    return &orgData[unitOrgId - 1];
}

extern const char unitOrgDataPlChaGetErrText[];
extern PlCharacter plChaData[];

/* Returns the character record for a one-based character id, or 0 after printing an error when the id is out of range. */
PlCharacter *dataPlChaGet(int cid)
{
    if (cid >= PL_CHARACTER_ID_END) {
        printf(unitOrgDataPlChaGetErrText, cid);
        return 0;
    }
    return &plChaData[cid - 1];
}

/* Battle-data header offsets are byte offsets from the loaded block batDatBuf.
 * Other record contents remain opaque until a consumer evidences their fields. */
#define BATTLE_DATA_FIELD_OFFSET(member) ((unsigned int)&((BattleDataHeader *)0)->member)


typedef struct NormalInitRecord { unsigned char opaque[0x0C]; } NormalInitRecord;
typedef struct SpecialInitRecord { unsigned char opaque[0x0C]; } SpecialInitRecord;
typedef struct EtherInitRecord { unsigned char opaque[0x18]; } EtherInitRecord;
typedef struct ParameterRecord { unsigned char opaque[0x20]; } ParameterRecord;
typedef struct DefaultEquipmentRecord { unsigned char opaque[0x20]; } DefaultEquipmentRecord;
typedef struct SpecialTableRecord { unsigned char opaque[0x20]; } SpecialTableRecord;

UnitInitData *dataUnitInitGet(int entry)
{
    int *field = &batDatBuf.header.unitInitOffset;
    unsigned char *block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(unitInitOffset);
    int byteOffset = *field;
    UnitInitData *records = (UnitInitData *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

NormalInitRecord *dataNormInitGet(int entry)
{
    int *field = &batDatBuf.header.normalInitOffset;
    unsigned char *block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(normalInitOffset);
    int byteOffset = *field;
    NormalInitRecord *records = (NormalInitRecord *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

SpecialInitRecord *dataSpecInitGet(int entry)
{
    int *field = &batDatBuf.header.specialInitOffset;
    unsigned char *block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(specialInitOffset);
    int byteOffset = *field;
    SpecialInitRecord *records = (SpecialInitRecord *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

EtherInitRecord *dataEtherInitGet(int entry)
{
    int *field = &batDatBuf.header.etherInitOffset;
    unsigned char *block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(etherInitOffset);
    int byteOffset = *field;
    EtherInitRecord *records = (EtherInitRecord *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

/*
 * batDatBuf.header.experienceOffset is the experience table's byte offset from
 * the loaded block; dataExpTblGet returns the table.
 */
unsigned char *dataExpTblGet(void)
{
    int *field = &batDatBuf.header.experienceOffset;
    unsigned char *block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(experienceOffset);

    return *field + block;
}

ParameterRecord *dataParaTblGet(int entry)
{
    int *field = &batDatBuf.header.parameterOffset;
    unsigned char *block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(parameterOffset);
    int byteOffset = *field;
    ParameterRecord *records = (ParameterRecord *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

DefaultEquipmentRecord *dataDefEquipGet(int entry)
{
    int *field = &batDatBuf.header.defaultEquipmentOffset;
    unsigned char *block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(defaultEquipmentOffset);
    int byteOffset = *field;
    DefaultEquipmentRecord *records = (DefaultEquipmentRecord *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

SpecialTableRecord *dataSpecTblGet(int entry)
{
    int *field = &batDatBuf.header.specialTableOffset;
    unsigned char *block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(specialTableOffset);
    int byteOffset = *field;
    SpecialTableRecord *records = (SpecialTableRecord *)(block + byteOffset);

    /* Caller indices are one-based; retain the original lack of bounds checks. */
    return &records[entry - 1];
}

extern void dataEtherLearnSet(int etherType, int techniqueId, int selector);
static const char unitOrgDataEtherTecSetErrText[40] = "dataEtherTecSet: err %d\n";
extern void *dataUnitFileGet(BattleUnit *unit, int charaId);
extern int dataUnitFileLoadMot(BattleUnit *unit, int motionId, int slot, UnitFileInfo *file);

extern void dataSpecLearnSet(int cid, int specialId);

/*
 * Teaches the ether technique and special technique mapped to selector 1 to 8.
 * Prints an error and returns for any other selector.
 */
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
        printf(unitOrgDataEtherTecSetErrText, selector);
        return;
    }

    if (techniqueId != 0) {
        dataEtherLearnSet(etherType, techniqueId, selector);
    }
    if (specialId != 0) {
        dataSpecLearnSet(etherType, specialId);
    }
}

extern const char unitOrgDataEtherLearnSetErrText[];

/* Sets the learned-ether bit for techniqueId of character cid, when techniqueId is not 0. */
/* The third argument is supplied by dataEtherTecSet but is not read here. */
void dataEtherLearnSet(int cid, int techniqueId, int selector)
{
    PlCharacter *pl;
    int index;

    if (cid >= PL_LEARNING_CHARACTER_ID_END) {
        printf(unitOrgDataEtherLearnSetErrText, cid);
        return;
    }
    if (techniqueId != 0) {
        pl = dataPlChaGet(cid);
        index = techniqueId - 1;
        pl->learnedEther[index / 8] |= 1 << (index % 8);
    }
}

extern const char unitOrgDataEtherLearnGetErrText[];

int dataEtherLearnGet(int cid, int techniqueId)
{
    PlCharacter *pl;
    int index;

    if (cid >= PL_LEARNING_CHARACTER_ID_END) {
        printf(unitOrgDataEtherLearnGetErrText, cid);
        return 0;
    }
    pl = dataPlChaGet(cid);
    index = techniqueId - 1;
    return pl->learnedEther[index / 8] & (1 << (index % 8));
}

extern const char unitOrgDataSkillLearnSetErrText[];

void dataSkillLearnSet(int cid, int skillId)
{
    PlCharacter *pl;
    int index;

    if (cid >= PL_LEARNING_CHARACTER_ID_END) {
        printf(unitOrgDataSkillLearnSetErrText, cid);
        return;
    }
    if (skillId != 0) {
        pl = dataPlChaGet(cid);
        index = skillId - 1;
        pl->learnedSkill[index / 8] |= 1 << (index % 8);
    }
}

extern const char unitOrgDataSkillLearnGetErrText[];

int dataSkillLearnGet(int cid, int skillId)
{
    PlCharacter *pl;
    int index;

    if (cid >= PL_LEARNING_CHARACTER_ID_END) {
        printf(unitOrgDataSkillLearnGetErrText, cid);
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
        printf(unitOrgDataSpecLearnSetErrText, cid);
        return;
    }
    if (specialId != 0) {
        pl = dataPlChaGet(cid);
        pl->special[specialId - dataSpecBaseGet(cid)].id = specialId;
    }
}

static const char unitOrgDataSpecLearnGetErrText[32];
/*
 * Returns the special id stored in the character's slot for specialId, or 0 when
 * specialId is 0 or cid is out of range. The slot index is specialId minus the
 * character's base id from thinkMapTbl.
 */
short dataSpecLearnGet(int cid, int specialId)
{
    short learned;

    if (cid >= PL_LEARNING_CHARACTER_ID_END) {
        printf(unitOrgDataSpecLearnGetErrText, cid);
        return 0;
    }
    learned = 0;
    if (specialId != 0) {
        learned = dataPlChaGet(cid)->special[specialId - thinkMapTbl.special.specBase[cid]].id;
    }
    return learned;
}

/* Returns the special slot of character cid whose id is specialId, or 0 when none matches. */
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

/* Returns the index of normalId in the unit's normal technique list, or -1 when it is absent. */
int dataNormIdxGet(BattleUnit *unit, int normalId)
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

typedef struct PlInitCopy12 { unsigned char b[0xC]; } PlInitCopy12;
typedef struct PlInitCopy24 { unsigned char b[0x18]; } PlInitCopy24;
typedef struct PlInitCopy34 { unsigned char b[0x34]; } PlInitCopy34;
typedef struct PlUnitSub {
    unsigned char pad0[4];
    unsigned short f4;
    unsigned short f6;
    unsigned short f8;
    unsigned char b[3];
    unsigned char padD;
    unsigned short s[3][3];
} PlUnitSub;
typedef struct PlUnitInit {
    unsigned short w0;
    unsigned short w2;
    unsigned char pad4[0x10];
    signed char level;
    unsigned char pad15[0x34 - 0x15];
} PlUnitInit;
typedef struct PlUnitRec {
    unsigned short w0;
    unsigned short w2;
    unsigned char pad4[0x10];
    signed char level;
    unsigned char pad15[0x30 - 0x15];
    PlUnitSub a;
    PlUnitSub b;
    unsigned char pad70[0x180 - 0x70];
} PlUnitRec;
typedef struct PlEquipSlot {
    unsigned short s0;
    unsigned char b;
    unsigned char pad;
    unsigned short s1;
} PlEquipSlot;
typedef struct PlChaSpec {
    short id;
    short pad2[2];
    short lv;
    unsigned char pad8[4];
} PlChaSpec;
typedef struct PlChaRec {
    unsigned char pad0[4];
    int nextExp;
    unsigned char pad8[0x10];
    unsigned short growth[8];
    unsigned char pad28[0x20];
    PlChaSpec spec[8];
} PlChaRec;
typedef struct PlShortTbl {
    short v[12];
} PlShortTbl;

/*
 * The init tables inside orgData[0] (+0x76, +0x82, +0x8E). dataPlUnitInit
 * addresses them as symbols of their own: orgData-relative forms make CSE
 * derive them from one base and change the original's instructions.
 */
typedef struct PlNormSlot { PlInitCopy12 norm; unsigned char pad[0x180 - 0xC]; } PlNormSlot;
extern PlNormSlot orgNormalInit[];
extern PlInitCopy12 orgSpecialInit;
extern PlInitCopy24 orgEtherInit;

/* Fills the unit org records of ids 1 to 32 from the battle data tables. */
void dataPlUnitInit(void)
{
  PlInitCopy12 *nb;
  unsigned char *cha;
  int i;
  int id;
  int k;
  int ofs;
  int ofs2;
  int j;
  PlUnitRec *rec;
  PlUnitSub *p;
  PlUnitSub *p2;
  unsigned char *q4;
  PlEquipSlot *eq;
  unsigned char *q;
  unsigned char *exp;
  unsigned short *para;
  PlInitCopy12 *sb;
  PlInitCopy24 *new_var;
  unsigned char *new_var2;
  unsigned short *d2;
  PlChaRec *chaRec;
  int nextExp;
  int expEntry;
  new_var = &orgEtherInit;
  cha = (unsigned char *) plChaData;
  /* Unit org records for ids 1 to 32. */
  for (i = 0; i < 0x20; i++)
  {
    rec = &((PlUnitRec *) orgData)[i];
    nb = &orgNormalInit[i].norm;
    memset(rec, 0, UNIT_ORG_RECORD_SIZE);
    id = i + 1;
    *((PlUnitInit *) rec) = *((PlUnitInit *) dataUnitInitGet(id));
    ofs = (i * UNIT_ORG_RECORD_SIZE) + 0x30;
    p = (PlUnitSub *) (((unsigned char *) orgData) + ofs);
    p->f4 = ((PlUnitInit *) rec)->w0;
    p->f6 = ((PlUnitInit *) rec)->w2;
    p->f8 = id;
    *nb = *((PlInitCopy12 *) dataNormInitGet(id));
    eq = (PlEquipSlot *) dataDefEquipGet(id);
    d2 = rec->b.s[2];
    for (k = 0; k < 3; k++)
    {
      j = k * 6;
      q = ((unsigned char *) eq) + 2;
      q4 = ((unsigned char *) eq) + 4;
      rec->b.b[k] = q[j];
      d2[-6] = *((unsigned short *) (((unsigned char *) eq) + j));
      *d2 = *((unsigned short *) (q4 + j));
      d2++;
    }

    for (k = 0; k < 3; k++)
    {
      ((PlUnitRec *) (((unsigned char *) orgData) + (UNIT_ORG_RECORD_SIZE * i)))->b.s[1][k] = ((unsigned short *) (q + 0x10))[k];
    }

    new_var2 = (unsigned char *) new_var;
    ofs2 = (i * UNIT_ORG_RECORD_SIZE) + 0x50;
    p2 = (PlUnitSub *) (((unsigned char *) orgData) + ofs2);
    p2->f4 = *((unsigned short *) (((unsigned char *) eq) + 0x18));
    p2->f8 = *((unsigned short *) (((unsigned char *) eq) + 0x1A));
    p2->f6 = *((unsigned short *) (((unsigned char *) eq) + 0x1C));
    /* Ids 1 to 16 also fill their character records. */
    if (i < 0x10)
    {
      memset((void *) ((i * 0xA8) + ((int) cha)), 0, 0xA8);
      for (k = 7; k >= 0; k--)
      {
        ((short *) (cha + (i * 0xA8)))[(k * 6) + 0x27] = 2;
      }

      exp = ((unsigned char *(*)(int)) dataExpTblGet)(id);
      chaRec = (PlChaRec *) dataPlChaGet(id);
      expEntry = ((PlUnitRec *) (((unsigned char *) orgData) + (i * UNIT_ORG_RECORD_SIZE)))->level * 4;
      expEntry += (int) exp;
      LoadWordUnaligned(nextExp, expEntry);
      chaRec->nextExp = nextExp;
      para = (unsigned short *) dataParaTblGet(id);
      for (k = 0; k < 8; k++)
      {
        ((PlChaRec *) dataPlChaGet(id))->growth[k] = ((PlShortTbl *) para)->v[k];
      }

      for (k = 0; k < 6; k++)
      {
        dataSpecLearnSet(i + 1, ((PlShortTbl *) dataSpecInitGet(i + 1))->v[k]);
      }

      for (k = 0; k < 12; k++)
      {
        ((void (*)(int, int)) dataEtherLearnSet)(i + 1, ((PlShortTbl *) dataEtherInitGet(i + 1))->v[k]);
      }

      sb = (PlInitCopy12 *) (((unsigned char *) (&orgSpecialInit)) + (i * UNIT_ORG_RECORD_SIZE));
      *sb = *((PlInitCopy12 *) dataSpecInitGet(id));
      *((PlInitCopy24 *) (new_var2 + (i * UNIT_ORG_RECORD_SIZE))) = *((PlInitCopy24 *) dataEtherInitGet(id));
      nb = &orgNormalInit[i].norm;
    }
  }

}

void dataDummySet(void)
{
}

#define BATTLE_UNIT_STAT_OFFSET 0x30

/* The part of a unit record dataUnitInitSet's slot searches read: the
 * character id sits 8 bytes into the 0x20-byte block at record offset 0x30. */
typedef struct BattleUnitStat {
    unsigned char unmodeled_00[8];
    short charaId; /* +0x08 */
    unsigned char unmodeled_0A[0x20 - 0x0A];
} BattleUnitStat;

/*
 * playerData (7 entries) and enemyData (5 entries) are 0x180-byte battle
 * unit records. dataBattleInit clears the character id at +0x38 of each entry.
 */
typedef struct BattleUnitInfo {
    UnitInitData init; /* +0x00: copy of dataUnitInitGet's record */
    unsigned short currentHp; /* +0x34: init.maxHp */
    unsigned short currentEp; /* +0x36: init.maxEp */
    short charaId; /* +0x38 */
    unsigned char unmodeled_3A[0x5E - 0x3A];
    short weaponId[3]; /* +0x5E */
    unsigned char unmodeled_64[0x76 - 0x64];
    NormalInitRecord normalInit; /* +0x76: dataNormInitGet's 0xC-byte record */
    unsigned char unmodeled_82[0x8E - 0x82];
    EtherInitRecord etherInit; /* +0x8E: dataEtherInitGet's 0x18-byte record */
    unsigned char unmodeled_A6[0x180 - 0xA6];
} BattleUnitInfo;

extern void dataFileInfoInit(void);
static BattleUnitInfo playerData[7];
static BattleUnitInfo enemyData[5];
static UnitFileInfo unitFileInfo[6];
static int cdReqNum;
static int leaderCid;
static int padMode;
static short padData;

/* Clears the character id of every player and enemy battle slot, after initializing the file info. */
void dataBattleInit(void)
{
    int i;

    dataFileInfoInit();
    for (i = 6; i >= 0; i--) {
        playerData[i].charaId = 0;
    }
    for (i = 4; i >= 0; i--) {
        enemyData[i].charaId = 0;
    }
}

/*
 * Binds the battle unit record for charaId to unit->work: ids 1 to 32 use or fill a
 * player slot, ids 33 to 186 fill a free enemy slot, and ids 187 to 194 copy a player
 * slot and clear its weapon ids. Prints an error and returns when no slot is free.
 */
extern const char unitOrgDataUnitInitSetErrText[32];
extern const char unitOrgDataUnitInitSetErrPLText[32];
extern const char unitOrgDataUnitInitSetErrENText[32];
extern const char unitOrgDataUnitInitSetErrSKJText[32];

void dataUnitInitSet(BattleUnit *unit, int charaId)
{
    BattleUnitInfo *slot;
    Actor *work;
    BattleUnitStat *stat;
    int i;
    int sourceSlotId;
    int srcSlotOffset;
    int srcStatBlockOffset;
    BattleUnitStat *srcStat;
    int statBlockOffset;
    int slotOffset;

    slot = 0;
    if (charaId < 1) {
        printf(unitOrgDataUnitInitSetErrText);
        return;
    }
    work = unit->work;
    if (charaId <= 32) {
        for (i = 0; i < 7; i++) {
            slotOffset = i * sizeof(BattleUnitInfo);
            statBlockOffset = slotOffset + BATTLE_UNIT_STAT_OFFSET;
            stat = (BattleUnitStat *)((unsigned char *)playerData + statBlockOffset);
            if (stat->charaId == charaId) {
                work->up = (CalcUnitParam *)((unsigned char *)playerData + slotOffset);
                return;
            }
        }
        for (i = 0; i < 7; i++) {
            if (playerData[i].charaId == 0) {
                slot = &playerData[i];
                break;
            }
        }
        if (!slot) {
            printf(unitOrgDataUnitInitSetErrPLText);
            return;
        }
        work->up = (CalcUnitParam *)slot;
        calcCopyParaMake(dataUnitOrgGet(charaId), (unsigned char *)work->up);
        calcSkillInit(unit);
    } else if (charaId <= 186) {
        for (i = 0; i < 5; i++) {
            if (enemyData[i].charaId == 0) {
                slot = &enemyData[i];
                break;
            }
        }
        if (!slot) {
            printf(unitOrgDataUnitInitSetErrENText);
            return;
        }
        work->up = (CalcUnitParam *)slot;
        memset(slot, 0, sizeof(*slot));
        slot->init = *dataUnitInitGet(charaId);
        slot->currentHp = slot->init.maxHp;
        slot->currentEp = slot->init.maxEp;
        slot->normalInit = *dataNormInitGet(charaId);
        slot->etherInit = *dataEtherInitGet(charaId);
    } else {
        /* Find a free player slot for ids 187 to 194. */
        for (i = 0; i < 7; i++) {
            if (playerData[i].charaId == 0) {
                slot = &playerData[i];
                break;
            }
        }
        if (!slot) {
            printf(unitOrgDataUnitInitSetErrSKJText);
            return;
        }
        switch (charaId) {
        case 188:
        case 187:
            sourceSlotId = 7;
            break;
        case 189:
            sourceSlotId = 6;
            break;
        case 193:
        case 192:
        case 191:
        case 190:
            sourceSlotId = 3;
            break;
        case 194:
            sourceSlotId = 2;
            break;
        default:
            printf(unitOrgDataUnitInitSetErrSKJText);
            return;
        }
        work->up = (CalcUnitParam *)slot;
        /* Copy the player slot whose charaId is the source id. */
        for (i = 0; i < 7; i++) {
            srcSlotOffset = i * sizeof(BattleUnitInfo);
            srcStatBlockOffset = srcSlotOffset + BATTLE_UNIT_STAT_OFFSET;
            srcStat = (BattleUnitStat *)((unsigned char *)playerData + srcStatBlockOffset);
            if (srcStat->charaId == sourceSlotId) {
                *slot = *(BattleUnitInfo *)((unsigned char *)playerData + srcSlotOffset);
            }
        }
        for (i = 0; i < 3; i++) {
            slot->weaponId[i] = 0;
        }
    }
}

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

static const unsigned char unitOrgDataCidGetErrText[24]; /* "** dataCidGet: err %d\n" */
static short cidChgTbl[];

short dataCidGet(unsigned int cid)
{
    if (cid >= 0xC4) {
        printf(unitOrgDataCidGetErrText, cid);
        return 0;
    }
    return cidChgTbl[cid];
}

static const unsigned char unitOrgDataWidGetErrText[24]; /* "** dataWidGet: err %d" */
static short widChgTbl[];

short dataWidGet(unsigned int wid)
{
    if (wid >= 0x75) {
        printf(unitOrgDataWidGetErrText, wid);
        return 0;
    }
    return widChgTbl[wid];
}

/*
 * dataMotNameGetMenu reads motionId as a short at +0x14. The byte at +0x11 is kind,
 * a BP timeline/special-weapon selector that calcBp2Tid also reads.
 */
typedef struct TecRecord {
    unsigned char unmodeled_00[0x11];
    signed char kind; /* +0x11 */
    unsigned char unmodeled_12[0x14 - 0x12];
    short motionId; /* +0x14 */
    unsigned char unmodeled_16[0x18 - 0x16];
} TecRecord;

extern const char unitOrgDataTecGetErrText[];

TecRecord *dataTecGet(int entry)
{
    int *field;
    unsigned char *block;
    int byteOffset;
    TecRecord *records;

    if (entry <= 0) {
        printf(unitOrgDataTecGetErrText, entry);
        return 0;
    }
    field = &batDatBuf.header.techniqueOffset;
    block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(techniqueOffset);
    byteOffset = *field;
    records = (TecRecord *)(block + byteOffset);
    return &records[entry - 1];
}

/* dataMotNameGetMenu reads the weapon's technique id as a short at +0x14.
 * dataUnitFileLoad loads a weapon slot's own technique motion only when the
 * short at +0xA is 1. */
typedef struct WpnRecord {
    unsigned char unmodeled_00[0xA];
    short motionType; /* +0xA */
    unsigned char unmodeled_0c[0x14 - 0xC];
    short tecId; /* +0x14 */
    unsigned char unmodeled_16[0x20 - 0x16];
} WpnRecord;

extern const char unitOrgDataWpnGetErrText[];

WpnRecord *dataWpnGet(int entry)
{
    int *field;
    unsigned char *block;
    int byteOffset;
    WpnRecord *records;

    if (entry <= 0) {
        printf(unitOrgDataWpnGetErrText, entry);
        return 0;
    }
    field = &batDatBuf.header.weaponOffset;
    block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(weaponOffset);
    byteOffset = *field;
    records = (WpnRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct AttRecord { unsigned char opaque[0x0E]; } AttRecord;

extern const char unitOrgDataAttGetErrText[];

AttRecord *dataAttGet(int entry)
{
    int *field;
    unsigned char *block;
    int byteOffset;
    AttRecord *records;

    if (entry <= 0) {
        printf(unitOrgDataAttGetErrText, entry);
        return 0;
    }
    field = &batDatBuf.header.attOffset;
    block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(attOffset);
    byteOffset = *field;
    records = (AttRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct EthRecord { unsigned char opaque[0x0C]; } EthRecord;

extern const char unitOrgDataEthGetErrText[];

EthRecord *dataEthGet(int entry)
{
    int *field;
    unsigned char *block;
    int byteOffset;
    EthRecord *records;

    if (entry <= 0) {
        printf(unitOrgDataEthGetErrText, entry);
        return 0;
    }
    field = &batDatBuf.header.ethOffset;
    block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(ethOffset);
    byteOffset = *field;
    records = (EthRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct ItmRecord { unsigned char opaque[0x14]; } ItmRecord;

extern const char unitOrgDataItmGetErrText[];

ItmRecord *dataItmGet(int entry)
{
    int *field;
    unsigned char *block;
    int byteOffset;
    ItmRecord *records;

    if (entry <= 0) {
        printf(unitOrgDataItmGetErrText, entry);
        return 0;
    }
    field = &batDatBuf.header.itmOffset;
    block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(itmOffset);
    byteOffset = *field;
    records = (ItmRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct AccRecord { unsigned char opaque[0x10]; } AccRecord;

extern const char unitOrgDataAccGetErrText[];

AccRecord *dataAccGet(int entry)
{
    int *field;
    unsigned char *block;
    int byteOffset;
    AccRecord *records;

    if (entry <= 0) {
        printf(unitOrgDataAccGetErrText, entry);
        return 0;
    }
    field = &batDatBuf.header.accessoryOffset;
    block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(accessoryOffset);
    byteOffset = *field;
    records = (AccRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct FrmRecord { unsigned char opaque[0x10]; } FrmRecord;

extern const char unitOrgDataFrmGetErrText[];

FrmRecord *dataFrmGet(int entry)
{
    int *field;
    unsigned char *block;
    int byteOffset;
    FrmRecord *records;

    if (entry <= 0) {
        printf(unitOrgDataFrmGetErrText, entry);
        return 0;
    }
    field = &batDatBuf.header.frameOffset;
    block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(frameOffset);
    byteOffset = *field;
    records = (FrmRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct EngRecord { unsigned char opaque[0x0C]; } EngRecord;

extern const char unitOrgDataEngGetErrText[];

EngRecord *dataEngGet(int entry)
{
    int *field;
    unsigned char *block;
    int byteOffset;
    EngRecord *records;

    if (entry <= 0) {
        printf(unitOrgDataEngGetErrText, entry);
        return 0;
    }
    field = &batDatBuf.header.engOffset;
    block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(engOffset);
    byteOffset = *field;
    records = (EngRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct SklRecord { unsigned char opaque[0x08]; } SklRecord;

extern const char unitOrgDataSklGetErrText[];

SklRecord *dataSklGet(int entry)
{
    int *field;
    unsigned char *block;
    int byteOffset;
    SklRecord *records;

    if (entry <= 0) {
        printf(unitOrgDataSklGetErrText, entry);
        return 0;
    }
    field = &batDatBuf.header.skillOffset;
    block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(skillOffset);
    byteOffset = *field;
    records = (SklRecord *)(block + byteOffset);
    return &records[entry - 1];
}

typedef struct GainRecord { unsigned char opaque[0x14]; } GainRecord;

/*
 * batDatBuf.header.gainOffset is the gain table's byte offset from the loaded block;
 * dataGainGet's table starts 0x294 bytes before that offset.
 */
extern const char unitOrgDataGainGetErrText[];

GainRecord *dataGainGet(int entry)
{
    int *field;
    unsigned char *block;
    int byteOffset;
    GainRecord *records;

    if (entry <= 0) {
        printf(unitOrgDataGainGetErrText, entry);
        return 0;
    }
    field = &batDatBuf.header.gainOffset;
    block = (unsigned char *)field - BATTLE_DATA_FIELD_OFFSET(gainOffset);
    byteOffset = *field;
    records = (GainRecord *)(block + byteOffset);
    return (GainRecord *)((unsigned char *)&records[entry] - 0x294);
}

extern int xglCdReadFile(const char *name, void *buffer, int mode, int flags);
extern int printf(const char *format, ...);
extern const char unitOrgReadErrText[];
extern const char unitOrgReadText[];

/* Reads a whole file into dst; returns 1 on success and 0 when the read fails. */
int dataFileLoad(const char *name, void *dst)
{
    long size = xglCdReadFile(name, dst, 0, 0);

    if (size < 0) {
        printf(unitOrgReadErrText, name);
        return 0;
    }
    printf(unitOrgReadText, name, (int) size);
    return 1;
}

int dataCdSync(void)
{
    return cdReqNum;
}

void dataCdSyncClear(void)
{
    cdReqNum = 0;
}

static const char unitOrgNBreadErrText[24];
static const char unitOrgNBreadEndText[24];

/*
 * CD read callback for dataFileLoadNB: prints read errors and, on result 4,
 * counts down cdReqNum and prints the end text when it reaches zero.
 */
void dataNBreadCB(int result) {
    if (result < 0) {
        printf(unitOrgNBreadErrText, result);
        return;
    }
    if (result == 4) {
        if (--cdReqNum == 0) {
            printf(unitOrgNBreadEndText, 0);
        }
    }
}

extern const char unitOrgNBreadErrText2[];
extern const char unitOrgNBreadText[];

/*
 * Starts an asynchronous CD read of filename into destination and counts it in
 * cdReqNum. Returns 1 when the request is accepted and 0 when it fails.
 */
int dataFileLoadNB(const char *filename, void *destination)
{
    /* xglCdReadFile declares its fourth argument as int, so the completion callback
     * is passed through an int cast. */
    int result = xglCdReadFile(filename, destination, 1, (int)dataNBreadCB);

    if (result < 0) {
        printf(unitOrgNBreadErrText2, filename);
        return 0;
    }
    cdReqNum++;
    printf(unitOrgNBreadText, cdReqNum, filename, destination);
    return 1;
}

extern void *motAdrTbl[];
extern int mdlAdrTbl[];

/* Resets the unit file table and fills its motion and model addresses from motAdrTbl and mdlAdrTbl. */
void dataFileInfoInit(void)
{
    int i;
    int j;
    int k;
    UnitFileInfo *info = unitFileInfo;
    int *tail;
    int *slot;

    i = 0;
    tail = (int *) ((unsigned char *) info + 16);
    for (; i < 6; i++) {
        unitFileInfo[i].charaId = 0;
        for (j = 0; j < 5; j++) {
            unitFileInfo[i].motion[j].motionId = 0;
            unitFileInfo[i].motion[j].adr = 0;
            unitFileInfo[i].motion[j].size = 0;
        }
        unitFileInfo[i].motion[0].adr = motAdrTbl[i];
        unitFileInfo[i].motion[4].adr = motAdrTbl[6];
        for (j = 0; j < 3; j++) {
            UnitFileInfo *file = &unitFileInfo[i];
            int *mdl = &file->modelCharaId;
            int *face = (int *) file->unmodeled_114;

            mdl[j * 3 + 1] = mdlAdrTbl[i];
            mdl[j * 3] = 0;
            mdl[j * 3 + 2] = 0;
            slot = &tail[i * 76 + 64 + j * 3];
            face[j * 3 - 2] = 0;
            *slot = 0;
            k = (int) slot;
            face[j * 3] = 0;
            for (k = 0; k < 3; k++) {
                unitFileInfo[i].wpn[k].part[j].id = 0;
                unitFileInfo[i].wpn[k].part[j].adr = 0;
                unitFileInfo[i].wpn[k].part[j].size = 0;
            }
        }
    }
}

extern const char unitOrgDataFileInfoInitChgErrText[];

/*
 * Clears the unit file's modelCharaId, three 0x18-byte records at +0x38 and,
 * inside each record, three 0xC-byte groups from +0xA0 (see dataFileInfoInit).
 */
void dataFileInfoInitChg(BattleUnit *unit, int charaId)
{
    unsigned char *file = dataUnitFileGet(unit, charaId);
    unsigned char *p4;
    unsigned char *p8;
    unsigned char *pc;
    unsigned char *p10;
    int i;
    int j;

    if (file == 0) {
        printf(unitOrgDataFileInfoInitChgErrText);
        return;
    }
    i = 0;
    p8 = file + 8;
    pc = file + 12;
    p10 = file + 16;
    *(int *) (file + 4) = 0;
    p4 = file + 4;
    for (; i < 3; i++) {
        int off = 0x38 + i * 24;

        *(int *) (p8 + off) = 0;
        *(float *) (pc + off) = 0.0f;
        *(int *) (p10 + off) = 0;
        for (j = 0; j < 3; j++) {
            int k = 0xa0 + i * 36 + j * 12;

            *(int *) (file + k) = 0;
            *(int *) (p4 + k) = 0;
            *(int *) (p8 + k) = 0;
        }
    }
}

/* A GameResource block of an enemy model: top, then the animation part at mid
 * and the texture part at end (offsets from top). */
typedef struct EnemyMdlHdr {
    unsigned int top;
    unsigned char *mid;
    unsigned char *end;
} EnemyMdlHdr;

typedef struct EnemyMdlRes {
    EnemyMdlHdr *address;
    int size;
    int pad[2];
} EnemyMdlRes;

typedef struct EnemyMdlEnt {
    short id;
    short pad[3];
} EnemyMdlEnt;

extern EnemyMdlRes GameResource[];
extern const char unitOrgSrcText[];
extern const char unitOrgDstText[];
extern const char unitOrgEnemyMdlFoundText[];
extern int GameResourceSearch(int type, int id);
extern void xglMemCopy16(unsigned int dst, unsigned int src, int n);
extern void xglMemCopy16b(unsigned int dst, unsigned int src, int n);

typedef struct EnemyMdlFile {
    int charaId;
    int cid;
    unsigned char *base;
    int pad0c[2];
    unsigned char *mid;
    int pad18[2];
    unsigned char *end;
    unsigned char pad24[0x130 - 0x24];
} EnemyMdlFile;

/*
 * Finds the GameResource block of the first entry of tbl (at most three, ended by
 * id 0) whose parts are in order and copies it to unit file slot 3 + i, rebasing its
 * part pointers to the slot's model address.
 */
void dataEnemyMdlChk(EnemyMdlEnt *tbl)
{
    EnemyMdlFile *dst;
    EnemyMdlHdr *h;
    int id;
    int cid;
    int idx;
    int sz;
    int i;
    unsigned char *n;
    unsigned char **p;

    for (i = 0; (tbl[i].id != 0) && (i < 3); i++) {
        id = dataCidGet(tbl[i].id);
        cid = id;
        idx = GameResourceSearch(0, cid);
        if (idx != -1) {
            h = GameResource[idx].address;
            sz = GameResource[idx].size;
            printf(unitOrgSrcText, h->top, h->mid, h->end, sz);
            if (h->top != 0 && h->mid != 0 && h->end != 0 && h->top < (unsigned int) h->mid && h->mid < h->end) {
                id = tbl[i].id;
                dst = (EnemyMdlFile *) &unitFileInfo[3 + i];
                do {
                    n = dst->base;
                } while (0);
                dst->charaId = id;
                do {
                    p = &h->mid;
                    n += ((unsigned int) (*p)) - h->top;
                    dst->cid = cid;
                    dst->mid = n;
                    p = &h->end;
                    n += (*p) - h->mid;
                    dst->end = n;
                } while (0);
                if (((unsigned int) dst->base) < h->top) {
                    xglMemCopy16((unsigned int) dst->base, h->top, sz / 16);
                } else {
                    xglMemCopy16b((((unsigned int) dst->base) + sz) - 0x10, (h->top + sz) - 0x10, sz / 16);
                }
                printf(unitOrgDstText, dst->base, dst->mid, dst->end);
                printf(unitOrgEnemyMdlFoundText, tbl[i].id);
                return;
            }
        }
    }
}

typedef struct LeaderChkWork {
    unsigned char unmodeled_0[0x84];
    void *org; /* +0x84: dataUnitOrgGet record of the candidate */
    unsigned char unmodeled_88[0x88];
} LeaderChkWork;

extern int calcAccEquChk(BattleUnit *unit, int accId);
/* Id of the party leader, set by dataLeaderChk and compared in dataUnitFileLoad. */
static int leaderId;

typedef struct LeaderUnitFile {
    int charaId;
    unsigned char unmodeled_4[0x12c];
} LeaderUnitFile;

/*
 * Scans the party candidates ids[0], ids[4] and ids[8]. The first one whose cid matches
 * the party leader's battle id and who has no accessory equipped becomes the leader: the
 * battle id is stored in leaderCid, its id in leaderId, and a pointer field of its file slot is rewritten.
 */
void dataLeaderChk(short *ids)
{
    union {
        BattleUnit unit;
        unsigned char pad[0x80];
    } t;
    LeaderChkWork work;
    PartyDataPrefix *party;
    LeaderUnitFile *base;
    int *p0;
    int i;

    party = PartyDataGet();
    dataLeaderCidReset();
    base = (LeaderUnitFile *) unitFileInfo;
    for (i = 0; i < 3; i++) {
        unsigned short cid;

        p0 = (int *) (base[0].unmodeled_4 + 4);
        p0 = (int *) ((*((LeaderUnitFile *) unitFileInfo + 0)).unmodeled_4 + 4);
        t.unit.work = (Actor *) &work;
        work.org = dataUnitOrgGet(ids[i * 4]);
        cid = party->leader_battle_id;
        if ((cid == dataCidGet(ids[i * 4])) && (calcAccEquChk(&t.unit, 0x2D) == 0)) {
            int tmp = *((int *) (base[i].unmodeled_4 + 4));
            int tmp0 = ((int *) base)[2];

            *((void **) p0) = (void *) tmp;
            *((unsigned int *) (&leaderCid)) = party->leader_battle_id;
            *((int *) (base[i].unmodeled_4 + 4)) = tmp0;
            leaderId = ids[(i * 2) * 2];
            *((int *) (base[i].unmodeled_4 + 4)) = tmp0;
            return;
        }
    }
}

int dataLeaderCidGet(void) {
    return leaderCid;
}

void dataLeaderCidReset(void) {
    leaderCid = 0;
}

extern const char unitOrgDataLeaderReloadText[];

void dataLeaderReload(void)
{
    unsigned short leaderBattleId = PartyDataGet()->leader_battle_id;

    if (leaderBattleId != leaderCid) {
        printf(unitOrgDataLeaderReloadText);
    }
}

extern char *strcpy(char *destination, const char *source);
extern char *strcat(char *destination, const char *source);
extern int dataMotNameGetSub(char *name, CalcUnitParam *up, int motionId, int weaponId);
static const char *motNameBase;
static const char *motNameExt;

/*
 * Menu-side motion name lookup for one weapon slot of a unit origin record. The weapon
 * id comes from weaponId[], the weapon record names its technique and the technique
 * record names the motion.
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

/* Builds the motion name of motionId for weaponId into name, and clears name when no suffix matches. */
int dataMotNameGet(char *name, BattleUnit *unit, int motionId, int weaponId)
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

static const char *motFile[303];
static const char *agwsName[13];
static const char *agwsLRName[3];

/* Motion ids at or above MOTION_NAME_ID_LIMIT have no suffix. From
 * MOTION_NAME_REMAP_ID on, the suffix index is the id minus
 * MOTION_NAME_REMAP_OFFSET; from MOTION_NAME_AGWS_ID on, a unit with flag 0x40
 * takes its AGWS name. The names are guesses from this use: the meaning of the
 * id ranges is not established. */
#define MOTION_NAME_ID_LIMIT 0x157
#define MOTION_NAME_REMAP_ID 0x12E
#define MOTION_NAME_REMAP_OFFSET 0x2A
#define MOTION_NAME_AGWS_ID 0x103

/* dataWidGet returns this id for a weapon that has no model (widChgTbl). */
#define WEAPON_MODEL_ID_NONE 0x3000

/* Appends the motion-name suffix of motionId to name, with the AGWS name and
 * left/right part for flag-0x40 units from MOTION_NAME_AGWS_ID on. */
int dataMotNameGetSub(char *name, CalcUnitParam *up, int motionId, int weaponId)
{
    const char *suffix;
    int lrIdx;
    int tableIdx;
    int agwsIdx;

    if (motionId < MOTION_NAME_ID_LIMIT) {
        tableIdx = motionId;
        if (motionId >= MOTION_NAME_REMAP_ID) {
            tableIdx = motionId - MOTION_NAME_REMAP_OFFSET;
        }
        suffix = motFile[tableIdx - 1];
        if (suffix != 0) {
            if (motionId >= MOTION_NAME_AGWS_ID && (up->flags & 0x40)) {
                agwsIdx = up->charaId - 0x11;
                if (motionId >= MOTION_NAME_REMAP_ID) {
                    lrIdx = 2;
                } else {
                    lrIdx = *(signed char *) (weaponId + (int) up + 0x5A) < 0x20;
                }
                strcat(name, agwsName[agwsIdx]);
                strcat(name, suffix);
                strcat(name, agwsLRName[lrIdx]);
            } else {
                strcat(name, suffix);
            }
            return 1;
        }
    }
    return 0;
}

extern int unitCidGet(BattleUnit *unit);
extern int dataUnitFileLoadMdl(BattleUnit *unit, int charaId, UnitFileInfo *file);
extern int dataUnitFileLoadWep(BattleUnit *unit, int weaponId, int slot, UnitFileInfo *fileInfo);

/*
 * Load a unit's model, its weapons (except kind-6 techniques' weapons), its
 * base motion and the motions of weapons that carry their own (motion type
 * 1 with a kind-4 technique, into motion slot weapon slot + 1).
 */
int dataUnitFileLoad(BattleUnit *unit, int charaId)
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
    dataUnitFileLoadMdl(unit, unitCidGet(unit), file);
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

static const char unitOrgDataUnitFileLoadErrText[32];

/*
 * Returns the file slot holding charaId, or else claims a free slot for it: ids 0x21
 * to 0xBA use slots 3 to 5 and other ids use slots 0 to 2. Returns 0 and prints an
 * error when no slot is free.
 */
void *dataUnitFileGet(BattleUnit *unit, int charaId)
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
            printf(unitOrgDataUnitFileLoadErrText);
        }
    }
    return slot;
}

typedef struct PackMdlEntry {
    int charaId;
    const char *mdlName;
} PackMdlEntry;

static PackMdlEntry pcPack_0[15];
extern const char unitOrgShionmText[];
extern const char unitOrgMomomText[];
extern const char unitOrgChaosmText[];
extern const char unitOrgJrmText[];
extern const char unitOrgKosmos1Text[];
extern const char unitOrgMomoaText[];
extern const char unitOrgMomobText[];
extern int MenuScenarioNoGet(void);

/*
 * Picks the packed model name for a party member: pcPack_0 maps the character id to a name,
 * and outside the special character range accessory and status checks may replace it.
 * AGWS-flagged units outside that range get no name.
 */
const char *dataPackPcMdlNameGet(BattleUnit *unit)
{
    const char *name;
    short charaId;
    int i;

    if ((unit->work->flags & CALC_UNIT_FLAG_IS_AGWS) != 0
        || (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) != 0
        || (calcUPGet(unit)->charaId >= SPECIAL_MODEL_CHARACTER_ID_BEGIN
            && calcUPGet(unit)->charaId < SPECIAL_MODEL_CHARACTER_ID_END)) {
        if (calcUPGet(unit)->charaId < SPECIAL_MODEL_CHARACTER_ID_BEGIN
            || calcUPGet(unit)->charaId >= SPECIAL_MODEL_CHARACTER_ID_END) {
            return 0;
        }
    }

    name = 0;
    charaId = calcUPGet(unit)->charaId;
    for (i = 0; pcPack_0[i].charaId != 0; i++) {
        if (charaId == pcPack_0[i].charaId) {
            name = pcPack_0[i].mdlName;
            break;
        }
    }
    if (calcUPGet(unit)->charaId < SPECIAL_MODEL_CHARACTER_ID_BEGIN
        || calcUPGet(unit)->charaId >= SPECIAL_MODEL_CHARACTER_ID_END) {
        if (calcAccEquChk(unit, 0x2D) != 0) {
            if (charaId == 3) {
                name = unitOrgShionmText;
            } else if (charaId == 6) {
                name = unitOrgMomomText;
            } else if (charaId == 1) {
                name = unitOrgChaosmText;
            } else if (charaId == 7) {
                name = unitOrgJrmText;
            }
        }
        if (charaId == 2 && MenuScenarioNoGet() < 0xA) {
            name = unitOrgKosmos1Text;
        }
        if (calcStatGet(unit, 7, 2) != 0) {
            name = unitOrgMomoaText;
        } else if (calcStatGet(unit, 7, 1) != 0) {
            name = unitOrgMomobText;
        }
    }
    return name;
}

/* pcPack_1 is a 38-entry table (0x130 bytes, 8-byte stride), terminated by
 * a zero weaponId; dataPackWpnMdlNameGet searches it and returns the packed
 * model name for a matching weapon. */
typedef struct PcPackEntry {
    int weaponId;
    const char *name;
} PcPackEntry;

static PcPackEntry pcPack_1[38];

const char *dataPackWpnMdlNameGet(int weaponId)
{
    const char *name;
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

extern void ACT_initMotion(Actor *actor);

#define CD_SECTOR_SIZE 0x800
/* Leader model data is already resident: these are the animation and
 * texture buffers selected by the leader path, not file identifiers. */
#define LEADER_ANIMATION_ADDRESS ((void *)0x01070800)
#define LEADER_TEXTURE_ADDRESS ((void *)0x010B1000)
/* Face model buffer that dataUnitFileLoadFaceMdl sets for the leader. */
#define FACE_MODEL_ADDRESS ((void *)0x010B4000)
/* Face texture buffer that dataUnitFileLoadFaceMdl sets for the leader; it goes to the MOVE resource slot. */
#define FACE_TEXTURE_ADDRESS ((void *)0x010E6000)

static char mdlFileName[256];
static char mdlFileName2[256];
static const char *pcNameBase;
extern char unitOrgBinText[];
extern const char unitOrgDataUnitFileLoadMdlLeaderText[];
extern const char unitOrgLexText[];
extern const char unitOrgXtxText[];
extern const char unitOrgJntText[];
extern const char unitOrgDataUnitFileLoadMdlHitText[];
extern const char unitOrgMDLReadSizeText[];
extern const char *dataPackPcMdlNameGet(BattleUnit *unit);
extern void RES_GetMdlFileName(char *name, int charaId);
extern int xglCdGetFileSize(const char *name);
extern int dataUnitFileLoadFaceMdl(BattleUnit *unit, int charaId, UnitFileInfo *file);

/*
 * Loads the model, animation and texture data of a unit, or reuses it when the character
 * id matches. The leader's resident data, a packed player model or separate files are used,
 * each rounded to a CD sector. Repeated calcUPGet calls and signed division keep the original
 * call sequence and sector rounding.
 */
int dataUnitFileLoadMdl(BattleUnit *unit, int charaId, UnitFileInfo *file)
{
    Actor *actor;
    const char *packedName;

    actor = unit->actor;
    if (charaId != file->modelCharaId) {
        file->modelCharaId = charaId;
        if (((unit->work->flags & ACTOR_FLAG_ENEMY) == 0
             && (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) == 0
             && (calcUPGet(unit)->charaId < SPECIAL_MODEL_CHARACTER_ID_BEGIN
                 || calcUPGet(unit)->charaId >= SPECIAL_MODEL_CHARACTER_ID_END))
            || (calcUPGet(unit)->charaId >= SPECIAL_MODEL_CHARACTER_ID_BEGIN
                && calcUPGet(unit)->charaId < SPECIAL_MODEL_CHARACTER_ID_END)) {
            if (dataLeaderCidGet() == charaId) {
                file->animationAdr = LEADER_ANIMATION_ADDRESS;
                file->textureAdr = LEADER_TEXTURE_ADDRESS;
                printf(unitOrgDataUnitFileLoadMdlLeaderText, charaId);
            } else {
                packedName = dataPackPcMdlNameGet(unit);
                strcpy(mdlFileName2, pcNameBase);
                strcat(mdlFileName2, packedName);
                strcat(mdlFileName2, unitOrgBinText);
                dataFileLoadNB(mdlFileName2, file->modelAdr);
                file->modelSize = ((xglCdGetFileSize(mdlFileName2) + (CD_SECTOR_SIZE - 1))
                               / CD_SECTOR_SIZE) * CD_SECTOR_SIZE;
                file->animationAdr = 0;
                file->textureAdr = 0;
            }
        } else {
            /* Separate model, animation and texture files. */
            RES_GetMdlFileName(mdlFileName, charaId);
            strcpy(mdlFileName2, mdlFileName);
            strcat(mdlFileName2, unitOrgLexText);
            dataFileLoadNB(mdlFileName2, file->modelAdr);
            file->modelSize = ((xglCdGetFileSize(mdlFileName2) + (CD_SECTOR_SIZE - 1))
                               / CD_SECTOR_SIZE) * CD_SECTOR_SIZE;
            file->animationAdr = (unsigned char *)file->modelAdr + file->modelSize;
            strcpy(mdlFileName2, mdlFileName);
            strcat(mdlFileName2, unitOrgXtxText);
            dataFileLoadNB(mdlFileName2, file->animationAdr);
            file->animationSize = ((xglCdGetFileSize(mdlFileName2) + (CD_SECTOR_SIZE - 1))
                               / CD_SECTOR_SIZE) * CD_SECTOR_SIZE;
            file->textureAdr = (unsigned char *)file->animationAdr + file->animationSize;
            strcpy(mdlFileName2, mdlFileName);
            strcat(mdlFileName2, unitOrgJntText);
            dataFileLoadNB(mdlFileName2, file->textureAdr);
            file->textureSize = ((xglCdGetFileSize(mdlFileName2) + (CD_SECTOR_SIZE - 1))
                               / CD_SECTOR_SIZE) * CD_SECTOR_SIZE;
        }
    } else {
        printf(unitOrgDataUnitFileLoadMdlHitText, charaId, file->modelAdr);
    }
    actor->resource[BATTLE_ACTOR_RESOURCE_MODEL] = file->modelAdr;
    actor->resource[BATTLE_ACTOR_RESOURCE_MATRIX] = file->animationAdr;
    actor->resource[BATTLE_ACTOR_RESOURCE_MOVE] = file->textureAdr;
    ACT_initMotion(actor);
    printf(unitOrgMDLReadSizeText, file->modelSize + file->animationSize + file->textureSize);
    if ((unit->work->flags & ACTOR_FLAG_ENEMY) == 0
        && (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) == 0
        && (calcUPGet(unit)->charaId < SPECIAL_MODEL_CHARACTER_ID_BEGIN
                 || calcUPGet(unit)->charaId >= SPECIAL_MODEL_CHARACTER_ID_END)) {
        dataUnitFileLoadFaceMdl(unit, charaId, file);
    } else {
        file->faceCharaId = 0;
    }
    return 1;
}

extern const char unitOrgDataUnitFileLoadFaceMdlHitText[];
extern void *dataChildActorCreate(int callerId);

/* Creates the face child actor of a unit. The leader's fixed face buffers are set when its id changes. */
int dataUnitFileLoadFaceMdl(BattleUnit *unit, int charaId, UnitFileInfo *file)
{
    Actor *actor;

    if (charaId != file->faceCharaId) {
        file->faceCharaId = charaId;
        if (dataLeaderCidGet() == charaId) {
            file->faceModelAdr = FACE_MODEL_ADDRESS;
            file->faceTextureAdr = FACE_TEXTURE_ADDRESS;
        }
    } else {
        printf(unitOrgDataUnitFileLoadFaceMdlHitText, charaId, file->faceModelAdr);
    }

    actor = dataChildActorCreate(charaId);
    actor->resource[BATTLE_ACTOR_RESOURCE_MODEL] = file->faceModelAdr;
    unit->subActor = (Actor *) actor;
    actor->resource[BATTLE_ACTOR_RESOURCE_MATRIX] = file->animationAdr;
    actor->resource[BATTLE_ACTOR_RESOURCE_MOVE] = file->faceTextureAdr;
    return 1;
}

/* Word view of an Actor's pack slots. Its int lvalues keep the
 * original alias set: stores through the void * fields of Actor
 * move the size load in dataUnitFileLoadMot. */
typedef struct ActorWords {
    unsigned char unmodeled_0[0x100];
    int packEntry[4]; /* +0x100 */
    unsigned char unmodeled_110[0x8D0 - 0x110];
    int resource[11]; /* +0x8D0 */
} ActorWords;

static char motFileName[256];
extern const char unitOrgFPKDataNothingText[];
extern const char unitOrgDataUnitFileLoadMotHitText[];
extern const char unitOrgMOTReadSizeText[];

/* Loads the motion pack for motionId into slot of the unit file and points the actor's pack table entry at it. */
int dataUnitFileLoadMot(BattleUnit *unit, int motionId, int slot, UnitFileInfo *file)
{
    Actor *actor;
    char *pack;
    int index;
    int i;

    actor = unit->actor;
    if (motionId != file->motion[slot].motionId) {
        if (dataMotNameGet(motFileName, unit, motionId, slot - 1) == 0) {
            printf(unitOrgFPKDataNothingText);
            return 0;
        }
        file->motion[slot].size = ((xglCdGetFileSize(motFileName) + (CD_SECTOR_SIZE - 1))
                                   / CD_SECTOR_SIZE) * CD_SECTOR_SIZE;
        if (slot == 0 || slot == 4) {
            pack = file->motion[slot].adr;
        } else {
            pack = file->motion[0].adr;
            pack += (unit->work->flags & ACTOR_FLAG_ENEMY) == 0 ? 0x40000 : 0x32000;
            for (i = 1; i < 4; i++) {
                if (file->motion[i].motionId != 0 && file->motion[i].adr != 0
                    && (unsigned)file->motion[i].adr < (unsigned)pack) {
                    pack = file->motion[i].adr;
                }
            }
            pack -= file->motion[slot].size;
        }
        file->motion[slot].adr = pack;
        dataFileLoadNB(motFileName, pack);
        if (slot != 4) {
            file->motion[slot].motionId = motionId;
        }
    } else {
        printf(unitOrgDataUnitFileLoadMotHitText, motionId, file->motion[slot].adr);
    }
    pack = file->motion[slot].adr;
    index = BATTLE_ACTOR_RESOURCE_ANIM_PACK_TABLE + slot;
    ((ActorWords *)actor)->resource[index] = (int)pack;
    ((ActorWords *)actor)->packEntry[slot] = (int)pack;
    printf(unitOrgMOTReadSizeText, file->motion[slot].size, slot);
    return 1;
}

static const char *wpnNameBase;
extern const char unitOrgDataUnitFileLoadWepHitText[];
extern const char unitOrgWEPReadSizeText[];
extern const char unitOrgWpnActotText[];

#define WPN_FILE_SECTORS(name) \
    (((xglCdGetFileSize(name) + (CD_SECTOR_SIZE - 1)) / CD_SECTOR_SIZE) * CD_SECTOR_SIZE)

/* Loads a weapon model into the unit's weapon slot and creates its child actor. Returns 0 when the weapon has no model. */
int dataUnitFileLoadWep(BattleUnit *unit, int weaponId, int slot, UnitFileInfo *file)
{
    UnitWeaponActors *equip = (UnitWeaponActors *)unit;
    Actor *actor;
    const char *packedName;
    int packed;
    unsigned char *address;
    int i;

    if (dataWidGet(weaponId) == WEAPON_MODEL_ID_NONE) {
        return 0;
    }
    if (weaponId != file->wpn[slot].part[0].id) {
        packedName = dataPackWpnMdlNameGet(weaponId);
        if (packedName != 0) {
            packed = 1;
            strcpy(mdlFileName2, wpnNameBase);
            strcat(mdlFileName2, packedName);
            strcat(mdlFileName2, unitOrgBinText);
            file->wpn[slot].part[0].size = WPN_FILE_SECTORS(mdlFileName2);
            file->wpn[slot].part[1].size = 0;
            file->wpn[slot].part[2].size = 0;
        } else {
            packed = 0;
            RES_GetMdlFileName(mdlFileName, dataWidGet(weaponId));
            strcpy(mdlFileName2, mdlFileName);
            strcat(mdlFileName2, unitOrgLexText);
            file->wpn[slot].part[0].size = WPN_FILE_SECTORS(mdlFileName2);
            strcpy(mdlFileName2, mdlFileName);
            strcat(mdlFileName2, unitOrgXtxText);
            file->wpn[slot].part[1].size = WPN_FILE_SECTORS(mdlFileName2);
            strcpy(mdlFileName2, mdlFileName);
            strcat(mdlFileName2, unitOrgJntText);
            file->wpn[slot].part[2].size = WPN_FILE_SECTORS(mdlFileName2);
        }
        /* Slot 2 of a unit without the AGWS flag uses the mdlAdrTbl[6] address. */
        if (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) && slot == 2) {
            address = (unsigned char *)mdlAdrTbl[6];
            file->wpn[2].part[0].id = 0x3000;
        } else {
            address = file->modelAdr;
            if (!(equip->work->flags & ACTOR_FLAG_ENEMY)) {
                address += 0x180000;
            } else {
                address += 0x100000;
            }
            for (i = 0; i < 3; i++) {
                if (file->wpn[i].part[0].id != 0 && file->wpn[i].part[0].adr != 0
                    && (unsigned char *)file->wpn[i].part[0].adr < address) {
                    address = file->wpn[i].part[0].adr;
                }
            }
            address -= file->wpn[slot].part[0].size + file->wpn[slot].part[1].size
                       + file->wpn[slot].part[2].size;
            file->wpn[slot].part[0].id = weaponId;
        }
        /* Packed weapon models load as one file; other models load their parts in sequence. */
        if (packed == 1) {
            if (dataWpnLRChk(unit, weaponId, slot, file)) {
                file->wpn[slot].part[0].adr = file->wpn[0].part[0].adr;
            } else {
                file->wpn[slot].part[0].adr = address;
                dataFileLoadNB(mdlFileName2, address);
            }
            file->wpn[slot].part[1].adr = 0;
            file->wpn[slot].part[2].adr = 0;
        } else if (dataWpnLRChk(unit, weaponId, slot, file)) {
            file->wpn[slot].part[0].adr = file->wpn[0].part[0].adr;
            file->wpn[slot].part[1].adr = file->wpn[0].part[1].adr;
            file->wpn[slot].part[2].adr = file->wpn[0].part[2].adr;
        } else {
            file->wpn[slot].part[0].adr = address;
            strcpy(mdlFileName2, mdlFileName);
            strcat(mdlFileName2, unitOrgLexText);
            dataFileLoadNB(mdlFileName2, file->wpn[slot].part[0].adr);
            file->wpn[slot].part[1].adr =
                (unsigned char *)file->wpn[slot].part[0].adr + file->wpn[slot].part[0].size;
            strcpy(mdlFileName2, mdlFileName);
            strcat(mdlFileName2, unitOrgXtxText);
            dataFileLoadNB(mdlFileName2, file->wpn[slot].part[1].adr);
            file->wpn[slot].part[2].adr =
                (unsigned char *)file->wpn[slot].part[1].adr + file->wpn[slot].part[1].size;
            strcpy(mdlFileName2, mdlFileName);
            strcat(mdlFileName2, unitOrgJntText);
            dataFileLoadNB(mdlFileName2, file->wpn[slot].part[2].adr);
        }
    } else {
        printf(unitOrgDataUnitFileLoadWepHitText, weaponId, file->wpn[slot].part[0].adr);
    }
    printf(unitOrgWEPReadSizeText, file->wpn[slot].part[0].size + file->wpn[slot].part[1].size
                           + file->wpn[slot].part[2].size);
    actor = dataChildActorCreate(dataWidGet(weaponId));
    actor->resource[BATTLE_ACTOR_RESOURCE_MODEL] = file->wpn[slot].part[0].adr;
    actor->drawEnvFlags |= 0x200;
    actor->resource[BATTLE_ACTOR_RESOURCE_MATRIX] = file->wpn[slot].part[1].adr;
    equip->equipActor[slot & 3] = (int) actor;
    actor->resource[BATTLE_ACTOR_RESOURCE_MOVE] = file->wpn[slot].part[2].adr;
    printf(unitOrgWpnActotText, actor, ((Actor *) equip->motionActor)->weaponLoadInfo);
    return 1;
}

/* Returns 1 when slot 1 holds the same weapon as slot 0, so slot 1 can reuse slot 0's parts. */
int dataWpnLRChk(BattleUnit *unit, int weaponId, int slot, UnitFileInfo *fileInfo)
{
    (void)unit;
    if (slot != 1 || weaponId != fileInfo->wpn[0].part[0].id) {
        return 0;
    }
    return 1;
}

extern Actor *ACT_create(int parent, int callerId);
static const char unitOrgDataChildActorCreateErrText[32];

/* Creates a child actor for callerId and initializes its motion. Returns 0 when creation fails. */
void *dataChildActorCreate(int callerId)
{
    Actor *actor;

    actor = ACT_create(-1, callerId);
    if (actor == 0) {
        printf(unitOrgDataChildActorCreateErrText);
        return 0;
    }
    ACT_initMotion(actor);
    actor->update = 0;
    actor->draw = 0;
    actor->flags = 0x208;
    return actor;
}

int dataUnitFileLoadMotSp(BattleUnit *unit, int motionId, int slot) {
    dataUnitFileLoadMot(unit, motionId, slot, dataUnitFileGet(unit, calcUPGet(unit)->charaId));
    return 1;
}

/*
 * Sets the slot's motion address and copies its pack entry into the weapon actors' motion
 * slots: both weapons when p4 is 5, else weapon p3 when p3 is not negative.
 */
int dataUnitFileLoadMotSp2(BattleUnit *unit, int motionId, int slot, int p3, int p4)
{
    Actor *actor;

    dataMotAdrSet(unit->actor, slot);
    if (p4 == 5) {
        actor = unit->weapon[0];
        if (actor != 0) {
            actor->resource[BATTLE_ACTOR_RESOURCE_WEAPON_MOTION] =
                unit->actor->resource[BATTLE_ACTOR_RESOURCE_ANIM_PACK_TABLE + slot];
        }
        actor = unit->weapon[1];
        if (actor != 0) {
            actor->resource[BATTLE_ACTOR_RESOURCE_WEAPON_MOTION] =
                unit->actor->resource[BATTLE_ACTOR_RESOURCE_ANIM_PACK_TABLE + slot];
        }
    } else if (p3 >= 0) {
        actor = unit->weapon[p3 & 3];
        if (actor != 0) {
            actor->resource[BATTLE_ACTOR_RESOURCE_WEAPON_MOTION] =
                unit->actor->resource[BATTLE_ACTOR_RESOURCE_ANIM_PACK_TABLE + slot];
        }
    }
    return 1;
}

extern const char unitOrgDataUnitFileLoad2Text[];
extern void dataUnitFileLoad2Sub(BattleUnit *unit);

/* Queued-command record for unitCmdSet. dataUnitFileLoad2 writes only selector and argument;
 * extra and the trailing 8 bytes are left unwritten. */
typedef struct CmdEntry {
    int selector; /* +0x00 */
    int argument; /* +0x04 */
    int extra;    /* +0x08: unwritten by this call */
    unsigned char unmodeled_0c[8]; /* +0x0C: unwritten by this call */
} CmdEntry;
extern int rnd(int max);

/* Runs dataUnitFileLoad2Sub and queues command 0x25 for the unit; enemies get a random argument, others 0. */
void dataUnitFileLoad2(BattleUnit *unit)
{
    CmdEntry cmd;

    printf(unitOrgDataUnitFileLoad2Text, unit);
    dataUnitFileLoad2Sub(unit);

    cmd.selector = 0x25;
    if (!(unit->work->flags & ACTOR_FLAG_ENEMY)) {
        cmd.argument = 0;
    } else {
        cmd.argument = rnd(0xA);
    }
    unitCmdSet(unit, &cmd);
}

typedef struct EquipActor {
    int flags; /* +0x00: Actor.flags */
    void (*update)(Actor *self); /* +0x04: Actor.update */
    unsigned char unmodeled_08[0x86 - 0x08];
    short handType; /* +0x86: compared with 4, 0x14 and 0x15 to select ACT_setHand's hand */
    unsigned char unmodeled_88[0x8d0 - 0x88];
    unsigned char *wpnMdl[3]; /* +0x8d0 */
    int baseMotionAdr; /* +0x8dc: the motion pack's slot[0] address (MotionAdrTable) */
    int wpnMotionAdr[3]; /* +0x8e0: one motion address per weapon slot */
} EquipActor;

extern void ACT_setHand(void *actor, int hand);
extern void ACT_setFace(void *actor, void *other, int flags);
extern void unitFaceMotSet(BattleUnit *unit, int motionId);
extern void *sefCreateEffect2(void *param);
extern void dataSndSeRegLoad2(BattleUnit *unit);
extern void dataPackWpnMdl2(Actor *actor);
extern void ACT_setArms(Actor *actor, Actor *parent, int accId, int flags);
extern void ACT_allocMatrix(Actor *actor, int count);
extern void ACT_setModelWrapper(Actor *actor, int flags);
extern void ACT_updateDefault(Actor *self);
extern const char unitOrgDataUnitFileLoad2SubMdlPackText[];
extern const char unitOrgDataUnitFileLoad2SubWpnPackText[];

/*
 * Finishes the actors of a unit once its files are loaded: unpacks the model pack pointers
 * on first use, sets the hand, face and weapon actors and their motions, and creates the
 * unit effect when the stat checks allow it.
 */
void dataUnitFileLoad2Sub(BattleUnit *unit)
{
    unsigned char effectParam[0x24];
    EquipActor *mot;
    EquipActor *face;
    EquipActor *actor;
    unsigned char *pack;
    UnitWork *effect;
    int i;
    int off;
    int tecKind;
    short wpnId;

    mot = (EquipActor *) unit->actor;
    face = (EquipActor *) unit->subActor;
    if (mot->wpnMdl[1] == 0) {
        int *wpnHdr;
        int t;

        pack = mot->wpnMdl[0];
        wpnHdr = (int *) pack;
        t = wpnHdr[2];
        mot->wpnMdl[0] = pack + t;
        t = wpnHdr[3];
        mot->wpnMdl[1] = pack + t;
        t = wpnHdr[4];
        mot->wpnMdl[2] = pack + t;
        if (face != 0) {
            face->wpnMdl[1] = pack + wpnHdr[3];
            t = wpnHdr[5];
            face->wpnMdl[0] = pack + t;
            t = wpnHdr[6];
            do {
                face->wpnMdl[2] = pack + t;
            } while (0);
        }
        printf(unitOrgDataUnitFileLoad2SubMdlPackText);
    }
    /* Pack the models of weapons that have one (id other than 0x3000). */
    i = 0;
    off = 0x50;
    do {
        if (effectParam) {
        }
        wpnId = *((short *) ((((char *) calcUPGet(unit)) + (off + (i * 2))) + 0xE));
        actor = ((EquipActor **) unit->weapon)[i & 3];
        i += 1;
        if (wpnId != 0x3000) {
            if (actor != 0) {
                if (actor->wpnMdl[1] == 0) {
                    dataPackWpnMdl2((Actor *) actor);
                    printf(unitOrgDataUnitFileLoad2SubWpnPackText);
                }
            }
        }
    } while (i < 3);
    ACT_allocMatrix((Actor *) mot, -1);
    ACT_setModelWrapper((Actor *) mot, 0);
    if (((!(unit->work->flags & ACTOR_FLAG_ENEMY)) && (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS))) && ((calcUPGet(unit)->charaId < SPECIAL_MODEL_CHARACTER_ID_BEGIN) || (calcUPGet(unit)->charaId >= SPECIAL_MODEL_CHARACTER_ID_END))) {
        short hand;
        int handSel;

        hand = mot->handType;
        if (((hand == 4) || (hand == 0x14)) || ((handSel = 0, hand == 0x15))) {
            handSel = 1;
        }
        ACT_setHand(mot, handSel);
    }
    dataMotAdrSet((Actor *) mot, 0);
    if (face != 0) {
        ACT_setFace(face, mot, 0);
        ACT_allocMatrix((Actor *) face, -1);
        ACT_setModelWrapper((Actor *) face, 0);
        face->update = ACT_updateDefault;
        face->baseMotionAdr = mot->baseMotionAdr;
        face->flags = face->flags & (~8);
        unitFaceMotSet(unit, 0x35);
    }
    /* Attach each weapon to its arm slot and set its motion. */
    for (i = 0; i < 3; i++) {
        wpnId = calcUPGet(unit)->weaponId[i];
        actor = ((EquipActor **) unit->weapon)[i & 3];
        if (wpnId != 0) {
            tecKind = ((signed char *) dataTecGet(*((short *) ((unsigned char *) dataWpnGet(wpnId) + 0x14))))[0x11];
            if ((*((short *) ((unsigned char *) dataWpnGet(wpnId) + 0xA))) == 1) {
                if (tecKind == 4) {
                    dataMotAdrSet((Actor *) mot, i + 1);
                }
            }
            off = 0x5A;
            if ((dataWidGet(wpnId) != WEAPON_MODEL_ID_NONE) && (0 != actor)) {
                ACT_setArms((Actor *) actor, (Actor *) mot, ((signed char *) calcUPGet(unit))[off + i], 0);
                ACT_allocMatrix((Actor *) actor, -1);
                ACT_setModelWrapper((Actor *) actor, 0);
                actor->update = ACT_updateDefault;
                if ((tecKind != 6) && (wpnId != 0xC)) {
                    actor->flags = actor->flags & (~8);
                }
                if (!(unit->work->flags & ACTOR_FLAG_ENEMY)) {
                    if (calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS) {
                        actor->baseMotionAdr = mot->wpnMotionAdr[i];
                        unitWpnMotSet(unit, i, 0, 5);
                    } else {
                        actor->baseMotionAdr = mot->baseMotionAdr;
                        unitWpnMotSet(unit, i, 0, 0xFF);
                    }
                }
            }
        }
    }

    if ((((!(unit->work->flags & ACTOR_FLAG_ENEMY)) && (!(calcUPGet(unit)->flags & CALC_UNIT_FLAG_IS_AGWS))) && ((calcUPGet(unit)->charaId < SPECIAL_MODEL_CHARACTER_ID_BEGIN) || (calcUPGet(unit)->charaId >= SPECIAL_MODEL_CHARACTER_ID_END))) || ((calcUPGet(unit)->charaId >= 0x97) && (calcUPGet(unit)->charaId < 0xBB))) {
        if ((calcStatGet(unit, 7, 2) == 0) && (calcStatGet(unit, 7, 1) == 0)) {
            memset(effectParam, 0, 0x24);
            *((short *) (effectParam + 0x14)) = 0x4001;
            *((void **) effectParam) = unit->actor;
            *((short *) (effectParam + 0x10)) = calcUPGet(unit)->charaId;
            effect = sefCreateEffect2(effectParam);
            calcUPGet(unit)->effectUnit = effect;
            if (effect != 0) {
                effect->flags = effect->flags | 0x100;
            }
        }
    } else {
        calcUPGet(unit)->effectUnit = 0;
    }
    dataSndSeRegLoad2(unit);
}

/*
 * A packed model or animation file: 8 bytes not recovered, then one byte
 * offset per packed entry, counted from the start of the header, the same
 * idiom dataFpkAdrGet below resolves for a single entry.
 */
typedef struct PackedFile {
    unsigned char unmodeled_00[8];
    int entryOffset[3]; /* +0x08 */
} PackedFile;

void dataPackWpnMdl2(Actor *actor)
{
    PackedFile *pack;
    int offset;

    pack = (PackedFile *)actor->resource[BATTLE_ACTOR_RESOURCE_MODEL];
    offset = pack->entryOffset[0];
    actor->resource[BATTLE_ACTOR_RESOURCE_MODEL] = (unsigned char *)pack + offset;
    offset = pack->entryOffset[1];
    actor->resource[BATTLE_ACTOR_RESOURCE_MATRIX] = (unsigned char *)pack + offset;
    offset = pack->entryOffset[2];
    actor->resource[BATTLE_ACTOR_RESOURCE_MOVE] = (unsigned char *)pack + offset;
}

/*
 * Resolves a loaded motion pack in place: the anim pack table slot gets the address of the
 * pack's first entry and the motionPackEntry slot that of its second, both as offsets from
 * the pack header (the PackedFile idiom above). The slot is computed in a local, and both
 * header offsets are read before the stores, to keep the original index copy.
 */
void dataMotAdrSet(Actor *actor, int motionId)
{
    PackedFile *pack;
    unsigned char *motion;
    unsigned char *packEntry;
    int slot;

    slot = BATTLE_ACTOR_RESOURCE_ANIM_PACK_TABLE + motionId;
    pack = (PackedFile *)actor->resource[slot];
    motion = (unsigned char *)pack + pack->entryOffset[0];
    packEntry = (unsigned char *)pack + pack->entryOffset[1];
    actor->resource[slot] = motion;
    actor->motionPackEntry[motionId] = packEntry;
}

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

extern WepSpEntry wepSpTbl[];        /* original storage, defined below */

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

extern void ACT_dispose(Actor *actor);

/*
 * Loads the special weapon of technique tecId into slot 2, or into slot 1 for
 * a kind-6 technique without a table entry. Returns 0 when neither applies.
 */
int dataFileLoadWepSp(BattleUnit *unit, int tecId)
{
    WepSpEntry *ent;
    UnitFileInfo *fileInfo;
    int slot;

    fileInfo = dataUnitFileGet(unit, calcUPGet(unit)->charaId);
    ent = dataTid2WepSp(tecId);
    slot = 2;
    if (ent == 0) {
        if (dataTecGet(tecId)->kind == 6) {
            slot = 1;
        } else {
            return 0;
        }
    }
    if (slot == 2) {
        calcUPGet(unit)->weaponId[2] = ent->weaponId;
        *(unsigned char *) ((unsigned char *) calcUPGet(unit) + 0x5C) = ent->unk8;
        dataUnitFileLoadWep(unit, ent->weaponId, 2, fileInfo);
        return 1;
    } else if (slot == 1) {
        dataUnitFileLoadWep(unit, calcUPGet(unit)->weaponId[1], 1, fileInfo);
        return 1;
    }
    return 0;
}

/* Attaches the already loaded special-weapon model of slot 1 or 2 to its
 * equipment actor and starts its update. */
int dataFileLoadWepSp2(BattleUnit *unit, int tecId)
{
    int slot;
    Actor *parent;
    short wid;
    Actor *actor;

    slot = 2;
    if (dataTid2WepSp(tecId) == 0) {
        slot = 1;
        if (dataTecGet(tecId)->kind != 6) {
            return 0;
        }
    }
    parent = unit->actor;
    wid = calcUPGet(unit)->weaponId[slot];
    actor = unit->weapon[slot];
    if (wid != 0) {
        if (dataWidGet(wid) == WEAPON_MODEL_ID_NONE) {
            return 1;
        }
        if (actor == 0) {
            return 1;
        }
        if (actor->resource[1] == 0) {
            dataPackWpnMdl2(actor);
        }
        if ((unsigned int) (wid - 0xD) >= 4) {
            ACT_setArms(actor, parent, *(signed char *) (slot + (int) calcUPGet(unit) + 0x5A), 0);
        }
        ACT_allocMatrix(actor, -1);
        ACT_setModelWrapper(actor, 0);
        actor->update = ACT_updateDefault;
    }
    return 1;
}

/* Releases the equipment actor of the special-weapon slot of tecId and clears
 * its weapon id when that slot is slot 2. */
int dataFileLoadWepSpEnd(BattleUnit *unit, int tecId)
{
    int slot;
    int off;
    Actor **equipSlot;

    slot = 2;
    if (dataTid2WepSp(tecId) == 0) {
        slot = 1;
        if (dataTecGet(tecId)->kind != 6) {
            return 0;
        }
    }
    off = slot * 4 + 0x1C;
    equipSlot = (Actor **) ((unsigned char *) unit + off);
    if (*equipSlot != 0) {
        ACT_dispose(*equipSlot);
        *equipSlot = 0;
    }
    if (slot == 2) {
        calcUPGet(unit)->weaponId[2] = 0;
        *(unsigned char *) ((unsigned char *) calcUPGet(unit) + 0x5C) = 0;
    }
    return 1;
}

/* Callers pass the unit whose default weapon they want; the function
 * always returns 0 and ignores it. */
int dataDefWpnGet(BattleUnit *unit)
{
    return 0;
}

extern const char unitOrgMTDIdFrameText[];

/*
 * Finds the motion record for the actor's current motion id, then walks its
 * trigger records for one keyed by trigger. Returns 1 (after printing the key)
 * when that record lists the current frame, 2 when the trigger is present
 * without that frame, else 0.
 */
int dataMtdRead(BattleUnit *unit, short trigger)
{
    unsigned char *act;
    unsigned char *base;
    unsigned char *rec;
    unsigned char *r;
    short *p;
    unsigned int off;
    unsigned int v;
    int idx;
    int id;
    int i;
    int sk;
    int frame;
    unsigned short key;
    unsigned short k;
    float ft;
    short n;

    act = (unsigned char *) unit->actor;
    idx = (*(unsigned short *) (act + 0x704) & 0x700) >> 8;
    id = (*(unsigned short *) (act + 0x704) & 0xFF) + 1;
    if (idx >= 5) {
        return 0;
    }
    base = *(unsigned char **) (act + 0x100 + idx * 4);
    if (base == 0) {
        return 0;
    }
    off = *(short *) base;
    p = (short *) (base + ((off >> 1) << 1));
    rec = 0;
    for (i = 0; (sk = p[i]) != 0; i += 2) {
        if (sk == id) {
            v = p[i + 1];
            rec = base + ((v >> 1) << 1);
            break;
        }
    }
    if (rec == 0) {
        return 0;
    }
    r = rec;
    key = *(unsigned short *) r;
    ft = *(float *) (act + 0x6F4) / 0.033333335f + 0.5f;
    frame = (int) ft;
    k = key;
    /* Walk the trigger records: key, frame count, then the frame ids. */
    while (k != 0) {
        if ((short) k == trigger) {
            for (i = 0; i < *(short *) (r + 2); i++) {
                idx = *(short *) (r + 4 + (i << 1));
                if (idx == frame) {
                    printf(unitOrgMTDIdFrameText, (short) key);
                    return 1;
                }
            }
            return 2;
        }
        n = *(short *) (r + 2);
        rec = (unsigned char *) ((short *) rec + n + 2);
        r = rec;
        key = k = *(unsigned short *) r;
    }
    return 0;
}

typedef struct XtxFileEntry {
    const char *name;
    unsigned int address;
} XtxFileEntry;

static XtxFileEntry xtxFile[];

void *dataXtxFileGet(int index)
{
    return &xtxFile[index];
}

static const char unitOrgDataXtxAdrGetErrText[32];
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
        printf(unitOrgDataXtxAdrGetErrText, index);
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

static const char unitOrgDataYamamotoBatdatBinText[32];

void dataBatDatLoad(void)
{
    dataFileLoad(unitOrgDataYamamotoBatdatBinText, &batDatBuf);
}

extern unsigned char thinkBuf[0x4000];

void *dataThinkAdrGet(void)
{
    return thinkBuf;
}

/* Returns the index of mapNo in thinkMapTbl, or -1 when it is not mapped. */
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
static const char *thinkName;
static const char *thinkNameBase;
static const char *thinkNameExt;
static const char unitOrgJpText[8];

/* Builds the behavior file name for thinkNo into name and returns 1, or clears name and returns 0 when thinkNo is not mapped. */
int dataThinkNameGet(char *name, int thinkNo)
{
    char num[16];
    int mappedNo;

    mappedNo = thinkMapGet(thinkNo);
    if (mappedNo != -1) {
        strcpy(name, thinkNameBase);
        strcat(name, thinkName);
        sprintf(num, unitOrgJpText, mappedNo);
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

/* Loads the behavior file of the remapped number thinkNo into thinkBuf. */
void dataThinkLoad(int thinkNo)
{
    char path[256];
    char num[16];

    strcpy(path, thinkNameBase);
    strcat(path, thinkName);
    sprintf(num, unitOrgJpText, thinkNo);
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

static const char *mapNameBase;
static const char *mapName;
static const char *mapNameExt;
static const char unitOrgDataMapLoadErrText[32];

/* Starts a non-blocking load of map mapNo into its fixed buffer; numbers of 100 or more print an error. */
void dataMapLoad(int mapNo)
{
    char path[256];
    char num[16];

    if (mapNo < 100) {
        strcpy(path, mapNameBase);
        strcat(path, mapName);
        sprintf(num, unitOrgJpText, mapNo);
        strcat(path, num);
        strcat(path, mapNameExt);
        dataFileLoadNB(path, dataMapLoadAdrGet(mapNo));
        return;
    }
    printf(unitOrgDataMapLoadErrText, mapNo);
}

extern const char unitOrgChaoText[8];
extern const char unitOrgKosmText[8];
extern const char unitOrgShioText[8];
extern const char unitOrgShitText[8];
extern const char unitOrgZiggText[8];
extern const char unitOrgMomoText[8];
extern const char unitOrgJrText[8];
extern const char unitOrgCeciText[8];
extern const char unitOrgCathText[8];
extern const char unitOrgVirgText[8];
extern const char unitOrgVx01Text[8];
extern const char unitOrgVx02Text[8];
extern const char unitOrgVx03Text[8];
extern const char unitOrgVx04Text[8];
extern const char unitOrgVx05Text[8];
extern const char unitOrgVx06Text[8];
extern const char unitOrgAg01Text[8];
extern const char unitOrgAg02Text[8];
extern const char unitOrgAg03Text[8];
extern const char unitOrgAg04Text[8];
extern const char unitOrgAg05Text[8];
extern const char unitOrgUknText[8];
extern const char unitOrgSldText[8];
extern const char unitOrgGuno001Text[16];
extern const char unitOrgGuno002Text[16];
extern const char unitOrgGuno003Text[16];
extern const char unitOrgGuno004Text[16];
extern const char unitOrgGuno005Text[16];
extern const char unitOrgGuno006Text[16];
extern const char unitOrgGuno007Text[16];
extern const char unitOrgGuno008Text[16];
extern const char unitOrgGuno009Text[16];
extern const char unitOrgGuno010Text[16];
extern const char unitOrgGuno011Text[16];
extern const char unitOrgGuno012Text[16];
extern const char unitOrgGuno013Text[16];
extern const char unitOrgGuno014Text[16];
extern const char unitOrgGuno015Text[16];
extern const char unitOrgGuno016Text[16];
extern const char unitOrgGuno017Text[16];
extern const char unitOrgGuno018Text[16];
extern const char unitOrgGuno019Text[16];
extern const char unitOrgGuno020Text[16];
extern const char unitOrgGuno021Text[16];
extern const char unitOrgGuno022Text[16];
extern const char unitOrgGuno023Text[16];
extern const char unitOrgGuno024Text[16];
extern const char unitOrgGuno025Text[16];
extern const char unitOrgGuno114Text[16];
extern const char unitOrgFema001Text[16];
extern const char unitOrgFema002Text[16];
extern const char unitOrgFema003Text[16];
extern const char unitOrgFema004Text[16];
extern const char unitOrgFema005Text[16];
extern const char unitOrgFema006Text[16];
extern const char unitOrgFere001Text[16];
extern const char unitOrgFere002Text[16];
extern const char unitOrgFere003Text[16];
extern const char unitOrgFero001Text[16];
extern const char unitOrgFeso001Text[16];
extern const char unitOrgFeso002Text[16];
extern const char unitOrgFeso003Text[16];
extern const char unitOrgFeso004Text[16];
extern const char unitOrgUtma001Text[16];
extern const char unitOrgUtma002Text[16];
extern const char unitOrgUtma003Text[16];
extern const char unitOrgUtma004Text[16];
extern const char unitOrgUtma005Text[16];
extern const char unitOrgUtma006Text[16];
extern const char unitOrgUtma007Text[16];
extern const char unitOrgUtma008Text[16];
extern const char unitOrgUtma009Text[16];
extern const char unitOrgUtma010Text[16];
extern const char unitOrgUtma011Text[16];
extern const char unitOrgUtma012Text[16];
extern const char unitOrgUtma013Text[16];
extern const char unitOrgUtma014Text[16];
extern const char unitOrgUtre001Text[16];
extern const char unitOrgUtre002Text[16];
extern const char unitOrgUtre003Text[16];
extern const char unitOrgUtre004Text[16];
extern const char unitOrgUtre005Text[16];
extern const char unitOrgUtre006Text[16];
extern const char unitOrgUtro001Text[16];
extern const char unitOrgUtro002Text[16];
extern const char unitOrgUtro003Text[16];
extern const char unitOrgUtro004Text[16];
extern const char unitOrgUtro005Text[16];
extern const char unitOrgUtro006Text[16];
extern const char unitOrgUtro007Text[16];
extern const char unitOrgUtso001Text[16];
extern const char unitOrgUtso002Text[16];
extern const char unitOrgUtso003Text[16];
extern const char unitOrgUtso004Text[16];
extern const char unitOrgUtso005Text[16];
extern const char unitOrgUtso006Text[16];
extern const char unitOrgUtso007Text[16];
extern const char unitOrgBoss001Text[16];
extern const char unitOrgBoss002Text[16];
extern const char unitOrgBoss003Text[16];
extern const char unitOrgBoss004Text[16];
extern const char unitOrgBoss005Text[16];
extern const char unitOrgBoss006Text[16];
extern const char unitOrgBoss007Text[16];
extern const char unitOrgBoss008Text[16];
extern const char unitOrgBoss009Text[16];
extern const char unitOrgBoss010Text[16];
extern const char unitOrgBoss011Text[16];
extern const char unitOrgBoss012Text[16];
extern const char unitOrgBoss013Text[16];
extern const char unitOrgBoss014Text[16];
extern const char unitOrgBoss015Text[16];
extern const char unitOrgBoss016Text[16];
extern const char unitOrgBoss017Text[16];
extern const char unitOrgBoss018Text[16];
extern const char unitOrgBoss019Text[16];
extern const char unitOrgBoss020Text[16];
extern const char unitOrgBoss021Text[16];
extern const char unitOrgBoss022Text[16];
extern const char unitOrgBoss023Text[16];
extern const char unitOrgBoss024Text[16];
extern const char unitOrgBoss025Text[16];
extern const char unitOrgBoss026Text[16];
extern const char unitOrgBoss027Text[16];
extern const char unitOrgBoss028Text[16];
extern const char unitOrgBoss029Text[16];
extern const char unitOrgBoss030Text[16];
extern const char unitOrgBoss031Text[16];
extern const char unitOrgBoss032Text[16];
extern const char unitOrgBoss033Text[16];
extern const char unitOrgSkjJr1Text[8];
extern const char unitOrgSkjJr2Text[8];
extern const char unitOrgSkjMomo1Text[16];
extern const char unitOrgSkjShion1Text[16];
extern const char unitOrgSkjShion2Text[16];
extern const char unitOrgSkjShion3Text[16];
extern const char unitOrgSkjShion4Text[16];
extern const char unitOrgSkjKosmos1Text[16];
extern const char unitOrgChaoSp1Text[16];
extern const char unitOrgChaoSp2Text[16];
extern const char unitOrgChaoSp3Text[16];
extern const char unitOrgChaoSp4Text[16];
extern const char unitOrgChaoSp5Text[16];
extern const char unitOrgChaoSp6Text[16];
extern const char unitOrgChaoSp7Text[16];
extern const char unitOrgChaoSp8Text[16];
extern const char unitOrgKosmSp1Text[16];
extern const char unitOrgKosmSp2Text[16];
extern const char unitOrgKosmSp3Text[16];
extern const char unitOrgKosmSp4Text[16];
extern const char unitOrgKosmSp5Text[16];
extern const char unitOrgKosmSp6Text[16];
extern const char unitOrgKosmSp7Text[16];
extern const char unitOrgShioSp1Text[16];
extern const char unitOrgShioSp2Text[16];
extern const char unitOrgShioSp3Text[16];
extern const char unitOrgShioSp4Text[16];
extern const char unitOrgShioSp5Text[16];
extern const char unitOrgShioSp6Text[16];
extern const char unitOrgShioSp7Text[16];
extern const char unitOrgShioSp8Text[16];
extern const char unitOrgShitSp1Text[16];
extern const char unitOrgShitSp2Text[16];
extern const char unitOrgShitSp3Text[16];
extern const char unitOrgShitSp4Text[16];
extern const char unitOrgShitSp5Text[16];
extern const char unitOrgShitSp6Text[16];
extern const char unitOrgShitSp7Text[16];
extern const char unitOrgShitSp8Text[16];
extern const char unitOrgZiggSp1Text[16];
extern const char unitOrgZiggSp2Text[16];
extern const char unitOrgZiggSp3Text[16];
extern const char unitOrgZiggSp4Text[16];
extern const char unitOrgZiggSp5Text[16];
extern const char unitOrgZiggSp6Text[16];
extern const char unitOrgZiggSp7Text[16];
extern const char unitOrgMomoSp1Text[16];
extern const char unitOrgMomoSp2Text[16];
extern const char unitOrgMomoSp3Text[16];
extern const char unitOrgMomoSp4Text[16];
extern const char unitOrgMomoSp5Text[16];
extern const char unitOrgMomoSp6Text[16];
extern const char unitOrgMomoSp7Text[16];
extern const char unitOrgMomoSp8Text[16];
extern const char unitOrgJrSp1Text[8];
extern const char unitOrgJrSp2Text[8];
extern const char unitOrgJrSp3Text[8];
extern const char unitOrgJrSp4Text[8];
extern const char unitOrgJrSp5Text[8];
extern const char unitOrgJrSp6Text[8];
extern const char unitOrgJrSp7Text[8];
extern const char unitOrgJrSp8Text[8];
extern const char unitOrgKosmWp1Text[16];
extern const char unitOrgKosmWp2Text[16];
extern const char unitOrgKosmWp3Text[16];
extern const char unitOrgKosmWp4Text[16];
extern const char unitOrgKosmWp5Text[16];
extern const char unitOrgKosmWp6Text[16];
extern const char unitOrgZiggWp1Text[16];
extern const char unitOrgZiggWp2Text[16];
extern const char unitOrgZiggWp3Text[16];
extern const char unitOrgZiggWp4Text[16];
extern const char unitOrgSut0Text[8];
extern const char unitOrgAx0Text[8];
extern const char unitOrgBd0Text[8];
extern const char unitOrgBd1Text[8];
extern const char unitOrgBdf0Text[8];
extern const char unitOrgHg0Text[8];
extern const char unitOrgHg1Text[8];
extern const char unitOrgHmr0Text[8];
extern const char unitOrgHmr1Text[8];
extern const char unitOrgHmr2Text[8];
extern const char unitOrgMg0Text[8];
extern const char unitOrgMg1Text[8];
extern const char unitOrgRf0Text[8];
extern const char unitOrgRf1Text[8];
extern const char unitOrgRf2Text[8];
extern const char unitOrgBam0Text[8];
extern const char unitOrgBs0Text[8];
extern const char unitOrgLsr0Text[8];
extern const char unitOrgPbk0Text[8];
extern const char unitOrgFl0Text[8];
extern const char unitOrgGnd0Text[8];
extern const char unitOrgGlg0Text[8];
extern const char unitOrgHb0Text[8];
extern const char unitOrgMsp0Text[8];
extern const char unitOrgMsp2Text[8];
extern const char unitOrgBl0Text[8];
extern const char unitOrgCb0Text[8];
extern const char unitOrgEac0Text[8];
extern const char unitOrgBp0Text[8];
extern const char unitOrgLgc0Text[8];
extern const char unitOrgAmp2Text[8];
extern const char unitOrgShb0Text[8];
extern const char unitOrgFsd0Text[8];
extern const char unitOrgVsd0Text[8];
extern const char unitOrgBbc0Text[8];
extern const char unitOrgEad0Text[8];
extern const char unitOrgEcm1Text[8];
extern const char unitOrgEcm2Text[8];
extern const char unitOrgSmp1Text[8];
extern const char unitOrgWir0Text[8];
extern const char unitOrgSol0Text[8];
extern const char unitOrgDef0Text[8];

#include "data_unit_org_get_data.inc"


void dataVPadModeSet(int mode)
{
    padMode = mode;
    padData = 0;
}

#define VIRTUAL_PAD_BIT_COUNT 14

static unsigned short padConvTbl[14];
/* Maps the low 14 bits of virtualPad to pad bits through padConvTbl and stores the result in padData. */
void dataVPadSet(int virtualPad)
{
    unsigned short result = 0;
    int shift = 0;
    const unsigned short *entry = padConvTbl;
    virtualPad &= 0xFFFF;
    /* The loop enters its body before testing the count, as in the original; keep the
     * table pointer advance and 16-bit mask. */
    do {
        if ((virtualPad >> shift) & 1) {
            result |= *entry;
        }
        shift++;
        entry++;
    } while (shift < VIRTUAL_PAD_BIT_COUNT);
    padData = result;
}

/* MAIN's two pad records (2 * 0x68 bytes). */
extern XglPadRecord PadData[2];

/*
 * Returns a direction word of pad 0 selected by padNo: 0 the held directions, 1 the newly
 * pressed ones, 2 the auto-repeated ones. In virtual-pad mode (dataVPadModeSet) the word
 * built by dataVPadSet is returned for any padNo.
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



char unitOrgDataYamamotoMotText[24] = "data\\yamamoto\\mot\\";

char unitOrgBinText[8] = ".bin";

char unitOrgDataYamamotoMapText[24] = "data\\yamamoto\\map\\";

char unitOrgMapText[8] = "map";

char unitOrgDataYamamotoThinkText[24] = "data\\yamamoto\\think\\";

char unitOrgThinkText[8] = "think";

char unitOrgDataYamamotoStatCurXtxText[32] = "data\\yamamoto\\stat\\cur.xtx";

char unitOrgDataYamamotoStatManuXtxText[32] = "data\\yamamoto\\stat\\manu.xtx";

char unitOrgDataYamamotoStatStatXtxText[32] = "data\\yamamoto\\stat\\stat.xtx";

extern const char unitOrgV6Text[8];
extern const char unitOrgV3Text[8];
extern const char unitOrgA1Text[8];
extern const char unitOrgV4Text[8];
extern const char unitOrgV1Text[8];
extern const char unitOrgV5Text[8];
extern const char unitOrgA3Text[8];
extern const char unitOrgA5Text[8];
extern const char unitOrgUkText[8];
extern const char unitOrgA2Text[8];
extern const char unitOrgA4Text[8];
extern const char unitOrgA6Text[8];
extern const char unitOrgV2Text[8];
extern const char unitOrgDText[8];
extern const char unitOrgLText[8];
extern const char unitOrgRText[8];
extern const char unitOrgDataYamamotoPcText[24];
extern const char unitOrgChaosText[];
extern const char unitOrgKosmosText[];
extern const char unitOrgShionText[];
extern const char unitOrgZiggyText[];
extern const char unitOrgVirgilText[];
extern const char unitOrgSkjJrText[];
extern const char unitOrgDataYamamotoWpnText[24];
extern const char unitOrgChoWingText[];
extern const char unitOrgKosHw01Text[];
extern const char unitOrgKosHw02Text[];
extern const char unitOrgKosHw03Text[];
extern const char unitOrgKosHw04Text[];
extern const char unitOrgKosHw05Text[];
extern const char unitOrgKosHw06Text[];
extern const char unitOrgKosBw01Text[];
extern const char unitOrgKosBw02Text[];
extern const char unitOrgKosBw03Text[];
extern const char unitOrgKosBw04Text[];
extern const char unitOrgMmoRd01Text[];
extern const char unitOrgMmoRd02Text[];
extern const char unitOrgMmoRd03Text[];
extern const char unitOrgMmoRd04Text[];
extern const char unitOrgMmoRd05Text[];
extern const char unitOrgMmoRd06Text[];
extern const char unitOrgMmoRd07Text[];
extern const char unitOrgMmoRd08Text[];
extern const char unitOrgMmoRd09Text[];
extern const char unitOrgMmoRd13Text[];
extern const char unitOrgMmoRd14Text[];
extern const char unitOrgMmoRd15Text[];
extern const char unitOrgMmoRd16Text[];
extern const char unitOrgMmoRd17Text[];
extern const char unitOrgZigSw01Text[];
extern const char unitOrgZigBw01Text[];
extern const char unitOrgZigBw02Text[];
extern const char unitOrgJrGun01Text[];
extern const char unitOrgJrGun02Text[];
extern const char unitOrgJrGun03Text[];
extern const char unitOrgJrGun04Text[];
extern const char unitOrgJrGun05Text[];
extern const char unitOrgJrGun06Text[];
extern const char unitOrgJrGun07Text[];
extern const char unitOrgJrGun08Text[];
extern const char unitOrgSonMboxText[];

static char mdlFileName[256] = "";

static char mdlFileName2[256] = "";

static char motFileName[256] = "";


static const char *agwsName[13] = { unitOrgV1Text, unitOrgV2Text, unitOrgV3Text, unitOrgV4Text, unitOrgV5Text, unitOrgV6Text, unitOrgA1Text, unitOrgA2Text, unitOrgA3Text, unitOrgA4Text, unitOrgA5Text, unitOrgA6Text, unitOrgUkText };

static const char *agwsLRName[3] = { unitOrgRText, unitOrgLText, unitOrgDText };

static int bakpTbl[16] = {
    0, 0x2ed, 0, 0,
    0x305, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

int mdlAdrTbl[7] = { 16777216, 18350080, 19922944, 21495808, 22544384, 23592960, 24641536 };

void *motAdrTbl[7] = {
    (void *)0x0179E000, (void *)0x017DE000, (void *)0x0181E000, (void *)0x0185E000,
    (void *)0x01890000, (void *)0x018C2000, (void *)0x018F4000
};

static const char *pcNameBase = unitOrgDataYamamotoPcText;

static PackMdlEntry pcPack_0[15] = { { .charaId = 1, .mdlName = unitOrgChaosText }, { .charaId = 2, .mdlName = unitOrgKosmosText }, { .charaId = 3, .mdlName = unitOrgShionText }, { .charaId = 5, .mdlName = unitOrgZiggyText }, { .charaId = 6, .mdlName = unitOrgMomoText }, { .charaId = 7, .mdlName = unitOrgJrText }, { .charaId = 10, .mdlName = unitOrgVirgilText }, { .charaId = 187, .mdlName = unitOrgSkjJrText }, { .charaId = 188, .mdlName = unitOrgSkjJrText }, { .charaId = 189, .mdlName = unitOrgSkjMomo1Text }, { .charaId = 190, .mdlName = unitOrgSkjShion1Text }, { .charaId = 191, .mdlName = unitOrgSkjShion2Text }, { .charaId = 192, .mdlName = unitOrgSkjShion3Text }, { .charaId = 194, .mdlName = unitOrgSkjKosmos1Text }, { .charaId = 0, .mdlName = 0 } };

static const char *wpnNameBase = unitOrgDataYamamotoWpnText;

static PcPackEntry pcPack_1[38] = { { .weaponId = 11, .name = unitOrgChoWingText }, { .weaponId = 12, .name = unitOrgKosHw01Text }, { .weaponId = 13, .name = unitOrgKosHw02Text }, { .weaponId = 14, .name = unitOrgKosHw03Text }, { .weaponId = 15, .name = unitOrgKosHw04Text }, { .weaponId = 16, .name = unitOrgKosHw05Text }, { .weaponId = 17, .name = unitOrgKosHw06Text }, { .weaponId = 18, .name = unitOrgKosBw01Text }, { .weaponId = 19, .name = unitOrgKosBw02Text }, { .weaponId = 20, .name = unitOrgKosBw03Text }, { .weaponId = 21, .name = unitOrgKosBw04Text }, { .weaponId = 24, .name = unitOrgMmoRd01Text }, { .weaponId = 25, .name = unitOrgMmoRd02Text }, { .weaponId = 26, .name = unitOrgMmoRd03Text }, { .weaponId = 27, .name = unitOrgMmoRd04Text }, { .weaponId = 28, .name = unitOrgMmoRd05Text }, { .weaponId = 29, .name = unitOrgMmoRd06Text }, { .weaponId = 30, .name = unitOrgMmoRd07Text }, { .weaponId = 31, .name = unitOrgMmoRd08Text }, { .weaponId = 32, .name = unitOrgMmoRd09Text }, { .weaponId = 36, .name = unitOrgMmoRd13Text }, { .weaponId = 37, .name = unitOrgMmoRd14Text }, { .weaponId = 38, .name = unitOrgMmoRd15Text }, { .weaponId = 39, .name = unitOrgMmoRd16Text }, { .weaponId = 40, .name = unitOrgMmoRd17Text }, { .weaponId = 41, .name = unitOrgZigSw01Text }, { .weaponId = 42, .name = unitOrgZigBw01Text }, { .weaponId = 43, .name = unitOrgZigBw02Text }, { .weaponId = 46, .name = unitOrgJrGun01Text }, { .weaponId = 47, .name = unitOrgJrGun02Text }, { .weaponId = 48, .name = unitOrgJrGun03Text }, { .weaponId = 49, .name = unitOrgJrGun04Text }, { .weaponId = 50, .name = unitOrgJrGun05Text }, { .weaponId = 51, .name = unitOrgJrGun06Text }, { .weaponId = 52, .name = unitOrgJrGun07Text }, { .weaponId = 53, .name = unitOrgJrGun08Text }, { .weaponId = 69, .name = unitOrgSonMboxText }, { .weaponId = 0, .name = 0 } };

WepSpEntry wepSpTbl[16] = { { .tecId = 48, .weaponId = 11, .unk8 = 27 }, { .tecId = 53, .weaponId = 13, .unk8 = 0 }, { .tecId = 54, .weaponId = 14, .unk8 = 0 }, { .tecId = 55, .weaponId = 15, .unk8 = 0 }, { .tecId = 58, .weaponId = 16, .unk8 = 0 }, { .tecId = 59, .weaponId = 17, .unk8 = 32 }, { .tecId = 83, .weaponId = 36, .unk8 = 32 }, { .tecId = 84, .weaponId = 37, .unk8 = 32 }, { .tecId = 86, .weaponId = 38, .unk8 = 32 }, { .tecId = 87, .weaponId = 37, .unk8 = 32 }, { .tecId = 88, .weaponId = 39, .unk8 = 32 }, { .tecId = 90, .weaponId = 40, .unk8 = 32 }, { .tecId = 92, .weaponId = 51, .unk8 = 32 }, { .tecId = 95, .weaponId = 51, .unk8 = 32 }, { .tecId = 97, .weaponId = 52, .unk8 = 32 }, { .tecId = 0, .weaponId = 0, .unk8 = 0 } };

static unsigned short padConvTbl[14] = { 4096, 16384, 32768, 8192, 16, 64, 128, 32, 4, 1, 8, 2, 2048, 256 };

const char unitOrgDef0Text[8] = "def0";

const char unitOrgSol0Text[8] = "sol0";

const char unitOrgWir0Text[8] = "wir0";

const char unitOrgSmp1Text[8] = "smp1";

const char unitOrgEcm2Text[8] = "ecm2";

const char unitOrgEcm1Text[8] = "ecm1";

const char unitOrgEad0Text[8] = "ead0";

const char unitOrgBbc0Text[8] = "bbc0";

const char unitOrgVsd0Text[8] = "vsd0";

const char unitOrgFsd0Text[8] = "fsd0";

const char unitOrgShb0Text[8] = "shb0";

const char unitOrgAmp2Text[8] = "amp2";

const char unitOrgLgc0Text[8] = "lgc0";

const char unitOrgBp0Text[8] = "bp0";

const char unitOrgEac0Text[8] = "eac0";

const char unitOrgCb0Text[8] = "cb0";

const char unitOrgBl0Text[8] = "bl0";

const char unitOrgMsp2Text[8] = "msp2";

const char unitOrgMsp0Text[8] = "msp0";

const char unitOrgHb0Text[8] = "hb0";

const char unitOrgGlg0Text[8] = "glg0";

const char unitOrgGnd0Text[8] = "gnd0";

const char unitOrgFl0Text[8] = "fl0";

const char unitOrgPbk0Text[8] = "pbk0";

const char unitOrgLsr0Text[8] = "lsr0";

const char unitOrgBs0Text[8] = "bs0";

const char unitOrgBam0Text[8] = "bam0";

const char unitOrgRf2Text[8] = "rf2";

const char unitOrgRf1Text[8] = "rf1";

const char unitOrgRf0Text[8] = "rf0";

const char unitOrgMg1Text[8] = "mg1";

const char unitOrgMg0Text[8] = "mg0";

const char unitOrgHmr2Text[8] = "hmr2";

const char unitOrgHmr1Text[8] = "hmr1";

const char unitOrgHmr0Text[8] = "hmr0";

const char unitOrgHg1Text[8] = "hg1";

const char unitOrgHg0Text[8] = "hg0";

const char unitOrgBdf0Text[8] = "bdf0";

const char unitOrgBd1Text[8] = "bd1";

const char unitOrgBd0Text[8] = "bd0";

const char unitOrgAx0Text[8] = "ax0";

const char unitOrgSut0Text[8] = "sut0";

const char unitOrgZiggWp4Text[16] = "zigg_wp4";

const char unitOrgZiggWp3Text[16] = "zigg_wp3";

const char unitOrgZiggWp2Text[16] = "zigg_wp2";

const char unitOrgZiggWp1Text[16] = "zigg_wp1";

const char unitOrgKosmWp6Text[16] = "kosm_wp6";

const char unitOrgKosmWp5Text[16] = "kosm_wp5";

const char unitOrgKosmWp4Text[16] = "kosm_wp4";

const char unitOrgKosmWp3Text[16] = "kosm_wp3";

const char unitOrgKosmWp2Text[16] = "kosm_wp2";

const char unitOrgKosmWp1Text[16] = "kosm_wp1";

const char unitOrgJrSp8Text[8] = "jr_sp8";

const char unitOrgJrSp7Text[8] = "jr_sp7";

const char unitOrgJrSp6Text[8] = "jr_sp6";

const char unitOrgJrSp5Text[8] = "jr_sp5";

const char unitOrgJrSp4Text[8] = "jr_sp4";

const char unitOrgJrSp3Text[8] = "jr_sp3";

const char unitOrgJrSp2Text[8] = "jr_sp2";

const char unitOrgJrSp1Text[8] = "jr_sp1";

const char unitOrgMomoSp8Text[16] = "momo_sp8";

const char unitOrgMomoSp7Text[16] = "momo_sp7";

const char unitOrgMomoSp6Text[16] = "momo_sp6";

const char unitOrgMomoSp5Text[16] = "momo_sp5";

const char unitOrgMomoSp4Text[16] = "momo_sp4";

const char unitOrgMomoSp3Text[16] = "momo_sp3";

const char unitOrgMomoSp2Text[16] = "momo_sp2";

const char unitOrgMomoSp1Text[16] = "momo_sp1";

const char unitOrgZiggSp7Text[16] = "zigg_sp7";

const char unitOrgZiggSp6Text[16] = "zigg_sp6";

const char unitOrgZiggSp5Text[16] = "zigg_sp5";

const char unitOrgZiggSp4Text[16] = "zigg_sp4";

const char unitOrgZiggSp3Text[16] = "zigg_sp3";

const char unitOrgZiggSp2Text[16] = "zigg_sp2";

const char unitOrgZiggSp1Text[16] = "zigg_sp1";

const char unitOrgShitSp8Text[16] = "shit_sp8";

const char unitOrgShitSp7Text[16] = "shit_sp7";

const char unitOrgShitSp6Text[16] = "shit_sp6";

const char unitOrgShitSp5Text[16] = "shit_sp5";

const char unitOrgShitSp4Text[16] = "shit_sp4";

const char unitOrgShitSp3Text[16] = "shit_sp3";

const char unitOrgShitSp2Text[16] = "shit_sp2";

const char unitOrgShitSp1Text[16] = "shit_sp1";

const char unitOrgShioSp8Text[16] = "shio_sp8";

const char unitOrgShioSp7Text[16] = "shio_sp7";

const char unitOrgShioSp6Text[16] = "shio_sp6";

const char unitOrgShioSp5Text[16] = "shio_sp5";

const char unitOrgShioSp4Text[16] = "shio_sp4";

const char unitOrgShioSp3Text[16] = "shio_sp3";

const char unitOrgShioSp2Text[16] = "shio_sp2";

const char unitOrgShioSp1Text[16] = "shio_sp1";

const char unitOrgKosmSp7Text[16] = "kosm_sp7";

const char unitOrgKosmSp6Text[16] = "kosm_sp6";

const char unitOrgKosmSp5Text[16] = "kosm_sp5";

const char unitOrgKosmSp4Text[16] = "kosm_sp4";

const char unitOrgKosmSp3Text[16] = "kosm_sp3";

const char unitOrgKosmSp2Text[16] = "kosm_sp2";

const char unitOrgKosmSp1Text[16] = "kosm_sp1";

const char unitOrgChaoSp8Text[16] = "chao_sp8";

const char unitOrgChaoSp7Text[16] = "chao_sp7";

const char unitOrgChaoSp6Text[16] = "chao_sp6";

const char unitOrgChaoSp5Text[16] = "chao_sp5";

const char unitOrgChaoSp4Text[16] = "chao_sp4";

const char unitOrgChaoSp3Text[16] = "chao_sp3";

const char unitOrgChaoSp2Text[16] = "chao_sp2";

const char unitOrgChaoSp1Text[16] = "chao_sp1";

const char unitOrgSkjKosmos1Text[16] = "skj_kosmos1";

const char unitOrgSkjShion4Text[16] = "skj_shion4";

const char unitOrgSkjShion3Text[16] = "skj_shion3";

const char unitOrgSkjShion2Text[16] = "skj_shion2";

const char unitOrgSkjShion1Text[16] = "skj_shion1";

const char unitOrgSkjMomo1Text[16] = "skj_momo1";

const char unitOrgSkjJr2Text[8] = "skj_jr2";

const char unitOrgSkjJr1Text[8] = "skj_jr1";

const char unitOrgBoss033Text[16] = "boss_033";

const char unitOrgBoss032Text[16] = "boss_032";

const char unitOrgBoss031Text[16] = "boss_031";

const char unitOrgBoss030Text[16] = "boss_030";

const char unitOrgBoss029Text[16] = "boss_029";

const char unitOrgBoss028Text[16] = "boss_028";

const char unitOrgBoss027Text[16] = "boss_027";

const char unitOrgBoss026Text[16] = "boss_026";

const char unitOrgBoss025Text[16] = "boss_025";

const char unitOrgBoss024Text[16] = "boss_024";

const char unitOrgBoss023Text[16] = "boss_023";

const char unitOrgBoss022Text[16] = "boss_022";

const char unitOrgBoss021Text[16] = "boss_021";

const char unitOrgBoss020Text[16] = "boss_020";

const char unitOrgBoss019Text[16] = "boss_019";

const char unitOrgBoss018Text[16] = "boss_018";

const char unitOrgBoss017Text[16] = "boss_017";

const char unitOrgBoss016Text[16] = "boss_016";

const char unitOrgBoss015Text[16] = "boss_015";

const char unitOrgBoss014Text[16] = "boss_014";

const char unitOrgBoss013Text[16] = "boss_013";

const char unitOrgBoss012Text[16] = "boss_012";

const char unitOrgBoss011Text[16] = "boss_011";

const char unitOrgBoss010Text[16] = "boss_010";

const char unitOrgBoss009Text[16] = "boss_009";

const char unitOrgBoss008Text[16] = "boss_008";

const char unitOrgBoss007Text[16] = "boss_007";

const char unitOrgBoss006Text[16] = "boss_006";

const char unitOrgBoss005Text[16] = "boss_005";

const char unitOrgBoss004Text[16] = "boss_004";

const char unitOrgBoss003Text[16] = "boss_003";

const char unitOrgBoss002Text[16] = "boss_002";

const char unitOrgBoss001Text[16] = "boss_001";

const char unitOrgUtso007Text[16] = "utso_007";

const char unitOrgUtso006Text[16] = "utso_006";

const char unitOrgUtso005Text[16] = "utso_005";

const char unitOrgUtso004Text[16] = "utso_004";

const char unitOrgUtso003Text[16] = "utso_003";

const char unitOrgUtso002Text[16] = "utso_002";

const char unitOrgUtso001Text[16] = "utso_001";

const char unitOrgUtro007Text[16] = "utro_007";

const char unitOrgUtro006Text[16] = "utro_006";

const char unitOrgUtro005Text[16] = "utro_005";

const char unitOrgUtro004Text[16] = "utro_004";

const char unitOrgUtro003Text[16] = "utro_003";

const char unitOrgUtro002Text[16] = "utro_002";

const char unitOrgUtro001Text[16] = "utro_001";

const char unitOrgUtre006Text[16] = "utre_006";

const char unitOrgUtre005Text[16] = "utre_005";

const char unitOrgUtre004Text[16] = "utre_004";

const char unitOrgUtre003Text[16] = "utre_003";

const char unitOrgUtre002Text[16] = "utre_002";

const char unitOrgUtre001Text[16] = "utre_001";

const char unitOrgUtma014Text[16] = "utma_014";

const char unitOrgUtma013Text[16] = "utma_013";

const char unitOrgUtma012Text[16] = "utma_012";

const char unitOrgUtma011Text[16] = "utma_011";

const char unitOrgUtma010Text[16] = "utma_010";

const char unitOrgUtma009Text[16] = "utma_009";

const char unitOrgUtma008Text[16] = "utma_008";

const char unitOrgUtma007Text[16] = "utma_007";

const char unitOrgUtma006Text[16] = "utma_006";

const char unitOrgUtma005Text[16] = "utma_005";

const char unitOrgUtma004Text[16] = "utma_004";

const char unitOrgUtma003Text[16] = "utma_003";

const char unitOrgUtma002Text[16] = "utma_002";

const char unitOrgUtma001Text[16] = "utma_001";

const char unitOrgFeso004Text[16] = "feso_004";

const char unitOrgFeso003Text[16] = "feso_003";

const char unitOrgFeso002Text[16] = "feso_002";

const char unitOrgFeso001Text[16] = "feso_001";

const char unitOrgFero001Text[16] = "fero_001";

const char unitOrgFere003Text[16] = "fere_003";

const char unitOrgFere002Text[16] = "fere_002";

const char unitOrgFere001Text[16] = "fere_001";

const char unitOrgFema006Text[16] = "fema_006";

const char unitOrgFema005Text[16] = "fema_005";

const char unitOrgFema004Text[16] = "fema_004";

const char unitOrgFema003Text[16] = "fema_003";

const char unitOrgFema002Text[16] = "fema_002";

const char unitOrgFema001Text[16] = "fema_001";

const char unitOrgGuno114Text[16] = "guno_114";

const char unitOrgGuno025Text[16] = "guno_025";

const char unitOrgGuno024Text[16] = "guno_024";

const char unitOrgGuno023Text[16] = "guno_023";

const char unitOrgGuno022Text[16] = "guno_022";

const char unitOrgGuno021Text[16] = "guno_021";

const char unitOrgGuno020Text[16] = "guno_020";

const char unitOrgGuno019Text[16] = "guno_019";

const char unitOrgGuno018Text[16] = "guno_018";

const char unitOrgGuno017Text[16] = "guno_017";

const char unitOrgGuno016Text[16] = "guno_016";

const char unitOrgGuno015Text[16] = "guno_015";

const char unitOrgGuno014Text[16] = "guno_014";

const char unitOrgGuno013Text[16] = "guno_013";

const char unitOrgGuno012Text[16] = "guno_012";

const char unitOrgGuno011Text[16] = "guno_011";

const char unitOrgGuno010Text[16] = "guno_010";

const char unitOrgGuno009Text[16] = "guno_009";

const char unitOrgGuno008Text[16] = "guno_008";

const char unitOrgGuno007Text[16] = "guno_007";

const char unitOrgGuno006Text[16] = "guno_006";

const char unitOrgGuno005Text[16] = "guno_005";

const char unitOrgGuno004Text[16] = "guno_004";

const char unitOrgGuno003Text[16] = "guno_003";

const char unitOrgGuno002Text[16] = "guno_002";

const char unitOrgGuno001Text[16] = "guno_001";

const char unitOrgSldText[8] = "sld";

const char unitOrgUknText[8] = "ukn";

const char unitOrgAg05Text[8] = "ag05";

const char unitOrgAg04Text[8] = "ag04";

const char unitOrgAg03Text[8] = "ag03";

const char unitOrgAg02Text[8] = "ag02";

const char unitOrgAg01Text[8] = "ag01";

const char unitOrgVx06Text[8] = "vx06";

const char unitOrgVx05Text[8] = "vx05";

const char unitOrgVx04Text[8] = "vx04";

const char unitOrgVx03Text[8] = "vx03";

const char unitOrgVx02Text[8] = "vx02";

const char unitOrgVx01Text[8] = "vx01";

const char unitOrgVirgText[8] = "virg";

const char unitOrgCathText[8] = "cath";

const char unitOrgCeciText[8] = "ceci";

const char unitOrgJrText[8] = "jr";

const char unitOrgMomoText[8] = "momo";

const char unitOrgZiggText[8] = "zigg";

const char unitOrgShitText[8] = "shit";

const char unitOrgShioText[8] = "shio";

const char unitOrgKosmText[8] = "kosm";

const char unitOrgChaoText[8] = "chao";

const char unitOrgUkText[8] = "uk_";

const char unitOrgA6Text[8] = "a6_";

const char unitOrgA5Text[8] = "a5_";

const char unitOrgA4Text[8] = "a4_";

const char unitOrgA3Text[8] = "a3_";

const char unitOrgA2Text[8] = "a2_";

const char unitOrgA1Text[8] = "a1_";

const char unitOrgV6Text[8] = "v6_";

const char unitOrgV5Text[8] = "v5_";

const char unitOrgV4Text[8] = "v4_";

const char unitOrgV3Text[8] = "v3_";

const char unitOrgV2Text[8] = "v2_";

const char unitOrgV1Text[8] = "v1_";

const char unitOrgDText[8] = "d";

const char unitOrgLText[8] = "l";

const char unitOrgRText[8] = "r";

const char unitOrgDataPlChaGetErrText[32] = "** dataPlChaGet: err %d\n";

const char unitOrgDataEtherLearnSetErrText[32] = "** dataEtherLearnSet: err %d\n";

const char unitOrgDataEtherLearnGetErrText[32] = "** dataEtherLearnGet: err %d\n";

const char unitOrgDataSkillLearnSetErrText[32] = "** dataSkillLearnSet: err %d\n";

const char unitOrgDataSkillLearnGetErrText[32] = "** dataSkillLearnGet: err %d\n";

const char unitOrgDataUnitInitSetErrText[32] = "** dataUnitInitSet: err %d\n";

const char unitOrgDataUnitInitSetErrPLText[32] = "** dataUnitInitSet: err PL\n";

const char unitOrgDataUnitInitSetErrENText[32] = "** dataUnitInitSet: err EN\n";

const char unitOrgDataUnitInitSetErrSKJText[32] = "** dataUnitInitSet: err SKJ\n";

const char unitOrgDataTecGetErrText[24] = "** dataTecGet: err %d\n";

const char unitOrgDataWpnGetErrText[24] = "** dataWpnGet: err %d\n";

const char unitOrgDataAttGetErrText[24] = "** dataAttGet: err %d\n";

const char unitOrgDataEthGetErrText[24] = "** dataEthGet: err %d\n";

const char unitOrgDataItmGetErrText[24] = "** dataItmGet: err %d\n";

const char unitOrgDataAccGetErrText[24] = "** dataAccGet: err %d\n";

const char unitOrgDataFrmGetErrText[24] = "** dataFrmGet: err %d\n";

const char unitOrgDataEngGetErrText[24] = "** dataEngGet: err %d\n";

const char unitOrgDataSklGetErrText[24] = "** dataSklGet: err %d\n";

const char unitOrgDataGainGetErrText[24] = "** dataGainGet: err %d\n";

const char unitOrgReadErrText[24] = "** read: err %s\n";

const char unitOrgReadText[24] = "** read: %s / %d\n";

const char unitOrgNBreadErrText2[24] = "** NBread: err %s\n";

const char unitOrgNBreadText[32] = "** NBread: (%d) %s = %X\n";

const char unitOrgDataFileInfoInitChgErrText[32] = "** dataFileInfoInitChg: err\n";

const char unitOrgSrcText[24] = "** src %X,%X,%X %d\n";

const char unitOrgDstText[24] = "** dst %X,%X,%X\n";

const char unitOrgEnemyMdlFoundText[24] = "** enemy Mdl found %d\n";

const char unitOrgDataLeaderReloadText[24] = "dataLeaderReload\n";

const char unitOrgDataYamamotoPcText[24] = "data\\yamamoto\\pc\\";

const char unitOrgSkjJrText[8] = "skj_jr";

const char unitOrgVirgilText[8] = "virgil";

const char unitOrgZiggyText[8] = "ziggy";

const char unitOrgShionText[8] = "shion";

const char unitOrgKosmosText[8] = "kosmos";

const char unitOrgChaosText[8] = "chaos";

const char unitOrgShionmText[8] = "shionm";

const char unitOrgMomomText[8] = "momom";

const char unitOrgChaosmText[8] = "chaosm";

const char unitOrgJrmText[8] = "jrm";

const char unitOrgKosmos1Text[8] = "kosmos1";

const char unitOrgMomoaText[8] = "momoa";

const char unitOrgMomobText[8] = "momob";

const char unitOrgDataYamamotoWpnText[24] = "data\\yamamoto\\wpn\\";

const char unitOrgSonMboxText[16] = "son_mbox";

const char unitOrgJrGun08Text[16] = "jr_gun08";

const char unitOrgJrGun07Text[16] = "jr_gun07";

const char unitOrgJrGun06Text[16] = "jr_gun06";

const char unitOrgJrGun05Text[16] = "jr_gun05";

const char unitOrgJrGun04Text[16] = "jr_gun04";

const char unitOrgJrGun03Text[16] = "jr_gun03";

const char unitOrgJrGun02Text[16] = "jr_gun02";

const char unitOrgJrGun01Text[16] = "jr_gun01";

const char unitOrgZigBw02Text[16] = "zig_bw02";

const char unitOrgZigBw01Text[16] = "zig_bw01";

const char unitOrgZigSw01Text[16] = "zig_sw01";

const char unitOrgMmoRd17Text[16] = "mmo_rd17";

const char unitOrgMmoRd16Text[16] = "mmo_rd16";

const char unitOrgMmoRd15Text[16] = "mmo_rd15";

const char unitOrgMmoRd14Text[16] = "mmo_rd14";

const char unitOrgMmoRd13Text[16] = "mmo_rd13";

const char unitOrgMmoRd09Text[16] = "mmo_rd09";

const char unitOrgMmoRd08Text[16] = "mmo_rd08";

const char unitOrgMmoRd07Text[16] = "mmo_rd07";

const char unitOrgMmoRd06Text[16] = "mmo_rd06";

const char unitOrgMmoRd05Text[16] = "mmo_rd05";

const char unitOrgMmoRd04Text[16] = "mmo_rd04";

const char unitOrgMmoRd03Text[16] = "mmo_rd03";

const char unitOrgMmoRd02Text[16] = "mmo_rd02";

const char unitOrgMmoRd01Text[16] = "mmo_rd01";

const char unitOrgKosBw04Text[16] = "kos_bw04";

const char unitOrgKosBw03Text[16] = "kos_bw03";

const char unitOrgKosBw02Text[16] = "kos_bw02";

const char unitOrgKosBw01Text[16] = "kos_bw01";

const char unitOrgKosHw06Text[16] = "kos_hw06";

const char unitOrgKosHw05Text[16] = "kos_hw05";

const char unitOrgKosHw04Text[16] = "kos_hw04";

const char unitOrgKosHw03Text[16] = "kos_hw03";

const char unitOrgKosHw02Text[16] = "kos_hw02";

const char unitOrgKosHw01Text[16] = "kos_hw01";

const char unitOrgChoWingText[16] = "cho_wing";

const char unitOrgDataUnitFileLoadMdlLeaderText[40] = "** dataUnitFileLoadMdl: Leader %d\n";

const char unitOrgLexText[8] = ".lex";

const char unitOrgXtxText[8] = ".xtx";

const char unitOrgJntText[8] = ".jnt";

const char unitOrgDataUnitFileLoadMdlHitText[40] = "** dataUnitFileLoadMdl: hit %d / %X\n";

const char unitOrgMDLReadSizeText[24] = "** MDL read Size = %d\n";

const char unitOrgDataUnitFileLoadFaceMdlHitText[48] = "** dataUnitFileLoadFaceMdl: hit %d / %X\n";

const char unitOrgFPKDataNothingText[24] = "** FPK data nothing!\n";

const char unitOrgDataUnitFileLoadMotHitText[40] = "** dataUnitFileLoadMot: hit %d / %X\n";

const char unitOrgMOTReadSizeText[32] = "** MOT read Size = %d %d\n";

const char unitOrgDataUnitFileLoadWepHitText[40] = "** dataUnitFileLoadWep: hit %d / %X\n";

const char unitOrgWEPReadSizeText[24] = "** WEP read Size = %d\n";

const char unitOrgWpnActotText[24] = "** wpnActot=%X %d\n";

const char unitOrgDataUnitFileLoad2Text[32] = "** dataUnitFileLoad2 %X\n";

const char unitOrgDataUnitFileLoad2SubMdlPackText[40] = "** dataUnitFileLoad2Sub: Mdl Pack\n";

const char unitOrgDataUnitFileLoad2SubWpnPackText[40] = "** dataUnitFileLoad2Sub: Wpn Pack\n";

const char unitOrgMTDIdFrameText[32] = "** MTD id=%d frame=%d **\n";
