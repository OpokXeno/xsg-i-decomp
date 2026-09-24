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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPlChaGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitInitGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataNormInitGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSpecInitGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataEtherInitGet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataParaTblGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataDefEquipGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSpecTblGet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataEtherLearnSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataEtherLearnGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSkillLearnSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSkillLearnGet);

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
 * ids; dataSpecLearnGet subtracts the caller's entry from a learned special
 * id before indexing PlCharacter.special, the same subtraction
 * dataSpecLearnSet performs through dataSpecBaseGet (still asm).
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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSpecDataGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataNormIdxGet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPosTblGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPosLineGet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataWpnLRChk);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", thinkMapGet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataVPadSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPadRead);
