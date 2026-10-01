/*
 * OV01 original TU 5: 0x00a191c0..0x00a1d348 (93 functions)
 */
#include "common.h"
#include "data_unit_org_get.h"
#include "ov01/calc.h"

/*
 * orgData is a 33-entry table of 0x180-byte unit-origin records; callers
 * pass a one-based index, so dataUnitOrgGet returns orgData[id - 1].
 */
typedef struct UnitOrgRecord {
    unsigned char unmodeled_0[0x180];
} UnitOrgRecord;

extern const char D_00A456A8[];
extern UnitOrgRecord orgData[];

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
    if (cid >= 0x21) {
        printf(D_00A456C8, cid);
        return 0;
    }
    return &plChaData[cid - 1];
}

/*
 * The loaded battle-data table starts eight bytes before D_426F48.
 * Its selector words at +0x08/+0x10/+0x14/+0x18 locate the unit, normal,
 * special and ether initialization tables. Entries are one-based, with
 * respective strides 0x34, 0x0C, 0x0C and 0x18.
 */
extern int D_426F48;

unsigned char *dataUnitInitGet(int entry)
{
    unsigned char *base = (unsigned char *)&D_426F48;
    int selector = *(volatile int *)base;
    base -= 0x8;
    return base + selector + entry * 0x34 - 0x34;
}

extern int D_426F50;

unsigned char *dataNormInitGet(int entry)
{
    unsigned char *base = (unsigned char *)&D_426F50;
    int selector = *(volatile int *)base;
    base -= 0x10;
    return base + selector + entry * 0xC - 0xC;
}

extern int D_426F54;

unsigned char *dataSpecInitGet(int entry)
{
    unsigned char *base = (unsigned char *)&D_426F54;
    int selector = *(volatile int *)base;
    base -= 0x14;
    return base + selector + entry * 0xC - 0xC;
}

extern int D_426F58;

unsigned char *dataEtherInitGet(int entry)
{
    unsigned char *base = (unsigned char *)&D_426F58;
    int selector = *(volatile int *)base;
    base -= 0x18;
    return base + selector + entry * 0x18 - 0x18;
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
unsigned char *dataParaTblGet(int entry)
{
    unsigned char *base = (unsigned char *)&D_426F7C;
    int selector = *(volatile int *)base;
    base -= 0x3C;
    return base + selector + (entry << 5) - 0x20;
}

extern int D_426F84;
unsigned char *dataDefEquipGet(int entry)
{
    unsigned char *base = (unsigned char *)&D_426F84;
    int selector = *(volatile int *)base;
    base -= 0x44;
    return base + selector + (entry << 5) - 0x20;
}

extern int D_426F80;
unsigned char *dataSpecTblGet(int entry)
{
    unsigned char *base = (unsigned char *)&D_426F80;
    int selector = *(volatile int *)base;
    base -= 0x40;
    return base + selector + (entry << 5) - 0x20;
}

extern void dataEtherLearnSet(int etherType, int techniqueId, int selector);
extern const char D_00A456E8[];
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

    if (cid >= 0x11) {
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

    if (cid >= 0x11) {
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

    if (cid >= 0x11) {
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

    if (cid >= 0x11) {
        printf(D_00A45790, cid);
        return 0;
    }
    pl = dataPlChaGet(cid);
    index = skillId - 1;
    return pl->learnedSkill[index / 8] & (1 << (index % 8));
}

typedef struct ThinkMapTable {
    unsigned char unmodeled_000[0x18C];
    int specBase[0x11]; /* +0x18C */
} ThinkMapTable;

extern ThinkMapTable thinkMapTbl;

int dataSpecBaseGet(int cid)
{
    return thinkMapTbl.specBase[cid];
}

/*
 * specTbl's tail is the character-id-indexed backup special-technique base
 * table dataBakpBaseGet reads.
 */
typedef struct SpecTable {
    unsigned char unmodeled_00[0x3C];
    int bakpBase[1]; /* +0x3C, indexed dynamically */
} SpecTable;

extern SpecTable specTbl;

int dataBakpBaseGet(int cid)
{
    return specTbl.bakpBase[cid];
}

void dataSpecLearnSet(int cid, int specialId)
{
    PlCharacter *pl;

    if (cid >= 0x11) {
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
extern const char D_00A457D0[];

short dataSpecLearnGet(int cid, int specialId)
{
    short learned;

    if (cid >= 0x11) {
        printf(D_00A457D0, cid);
        return 0;
    }
    learned = 0;
    if (specialId != 0) {
        learned = dataPlChaGet(cid)->special[specialId - thinkMapTbl.specBase[cid]].id;
    }
    return learned;
}

PlSpecialSlot *dataSpecDataGet(int cid, int specialId)
{
    PlCharacter *pl;
    int i;

    if (cid >= 0x11) {
        return 0;
    }
    if (specialId == 0) {
        return 0;
    }
    pl = dataPlChaGet(cid);
    for (i = 0; i < 8; i++) {
        if (pl->special[i].id == specialId) {
            return &pl->special[i];
        }
    }
    return 0;
}

int dataNormIdxGet(ObjectTask *unit, int normalId)
{
    int i;
    int offset;
    int result;

    i = 0;
    offset = 0x70;
    result = -1;
    while (i < 6) {
        if (*(short *)((unsigned char *)calcUPGet(unit) + offset + 6) == normalId) {
            result = i;
            break;
        }
        i++;
        offset += 2;
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
extern BattleUnitInfo playerData[7];
extern BattleUnitInfo enemyData[5];

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

extern unsigned char posTbl[];
unsigned char *dataPosTblGet(int entry)
{
    if (entry < 0) {
        entry = -entry;
    }
    return posTbl + (entry << 5);
}

short dataPosLineGet(int entry)
{
    if (entry < 0) {
        entry = -entry;
    }
    return *(short *)(posTbl + (entry << 5) + 0x10);
}

extern unsigned char D_00A45890[]; /* "** dataCidGet: err %d\n" */
extern short cidChgTbl[];

short dataCidGet(unsigned int cid)
{
    if (cid >= 0xC4) {
        printf(D_00A45890, cid);
        return 0;
    }
    return cidChgTbl[cid];
}

extern unsigned char D_00A458A8[]; /* "** dataWidGet: err %d" */
extern short widChgTbl[];

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
extern const char D_00A459E0[];
extern const char D_00A459F8[];

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
extern char *motNameBase;
extern char *motNameExt;

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

extern const char D_00A45AC8[];

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadMdl);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadFaceMdl);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadMot);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadWep);

int dataWpnLRChk(int unused, int value, int flag, void *unit)
{
    if (flag != 1 || value != *(int *)((unsigned char *)unit + 0xA0)) {
        return 0;
    }
    return 1;
}

/*
 * The engine's actor record ACT_create hands out (main/near_dir.h and
 * main/set_motion.h model the same 0xa70-byte record as their own TU-local
 * copy: +0x00 flags, +0x04 an update callback, +0x08 a draw callback).
 * dataChildActorCreate is this TU's only writer of that head.
 */
typedef struct ChildActor {
    int flags;                               /* +0x00 */
    void (*update)(struct ChildActor *self); /* +0x04 */
    void (*draw)(struct ChildActor *self);   /* +0x08 */
} ChildActor;

extern ChildActor *ACT_create(int parent, int callerId);
extern void ACT_initMotion(ChildActor *actor);
extern const char D_00A45F38[];

void *dataChildActorCreate(int callerId)
{
    ChildActor *actor;

    actor = ACT_create(-1, callerId);
    if (actor == 0) {
        printf(D_00A45F38);
        return 0;
    }
    ACT_initMotion(actor);
    actor->update = 0;
    actor->draw = 0;
    actor->flags = 0x208;
    return actor;
}

int dataUnitFileLoadMotSp(ObjectTask *unit, int motionId, int slot) {
    dataUnitFileLoadMot(unit, motionId, slot, dataUnitFileGet(unit, calcUPGet(unit)->charaId));
    return 1;
}

int dataUnitFileLoadMotSp2(UnitEquipInfo *unit, int motionId, int slot, int p3, int p4)
{
    EquipActor *actor;

    dataMotAdrSet(unit->motionTable, slot);
    if (p4 == 5) {
        actor = unit->equipActor[0];
        if (actor != 0) {
            actor->motionAdr = unit->motionTable->slot[slot];
        }
        actor = unit->equipActor[1];
        if (actor != 0) {
            actor->motionAdr = unit->motionTable->slot[slot];
        }
    } else if (p3 >= 0) {
        actor = unit->equipActor[p3 & 3];
        if (actor != 0) {
            actor->motionAdr = unit->motionTable->slot[slot];
        }
    }
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoad2);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoad2Sub);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPackWpnMdl2);

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
    unsigned char unmodeled_0[8];
} XtxFileEntry;

extern XtxFileEntry xtxFile[];

void *dataXtxFileGet(int index)
{
    return &xtxFile[index];
}

extern const char D_00A45FE8[];
extern unsigned short xtxAdr[0x13];

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

extern const char D_00A46008[];
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
    int *map = (int *)&thinkMapTbl;
    for (i = 0; i < 100; i++) {
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
extern char *thinkName;
extern char *thinkNameBase;
extern char *thinkNameExt;
extern const char D_00A46028[];

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataThinkDataSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataThinkFileLoad);

extern char *thinkName;
extern char *thinkNameBase;
extern char *thinkNameExt;
extern const char D_00A46028[];

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

extern char *strcpy(char *destination, const char *source);
extern char *strcat(char *destination, const char *source);
extern int sprintf(char *buffer, const char *format, ...);
extern char *mapNameBase;
extern char *mapName;
extern char *mapNameExt;
extern const char D_00A46028[];
extern const char D_00A46030[];
extern void dataFileLoadNB(void *buffer, void *address);

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

void dataVPadModeSet(int mode)
{
    padMode = mode;
    padData = 0;
}

extern unsigned short padConvTbl[14];
void dataVPadSet(int virtualPad)
{
    unsigned short result = 0;
    int shift = 0;
    const unsigned short *entry = padConvTbl;
    virtualPad &= 0xFFFF;
    do {
        if ((virtualPad >> shift) & 1) {
            result |= *entry;
        }
        shift++;
        entry++;
    } while (shift < 14);
    padData = result;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPadRead);
