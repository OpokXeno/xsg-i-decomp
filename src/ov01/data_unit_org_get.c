/*
 * OV01 original TU 5: 0x00a191c0..0x00a1d348 (93 functions)
 */
#include "common.h"
#include "data_unit_org_get.h"

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitOrgGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPlChaGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitInitGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataNormInitGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSpecInitGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataEtherInitGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataExpTblGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataParaTblGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataDefEquipGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSpecTblGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataEtherTecSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataEtherLearnSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataEtherLearnGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSkillLearnSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSkillLearnGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSpecBaseGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataBakpBaseGet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSpecLearnGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSpecDataGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataNormIdxGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPlUnitInit);

void dataDummySet(void)
{
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataBattleInit);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitInitSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPosTblGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPosLineGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataCidGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataWidGet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataMotNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataMotNameGetSub);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoad);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPackPcMdlNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPackWpnMdlNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadMdl);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadFaceMdl);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadMot);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadWep);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataWpnLRChk);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataChildActorCreate);

/* calcUPGet's return type comes from its definer, ov01/tu004 calc.c. */
#include "ov01/calc.h"

extern CalcUnitParam *calcUPGet(ObjectTask *unit);
extern void *dataUnitFileGet(ObjectTask *unit, short charaId);
extern int dataUnitFileLoadMot(ObjectTask *unit, int, int, void *);

int dataUnitFileLoadMotSp(ObjectTask *unit, int motionId, int slot) {
    dataUnitFileLoadMot(unit, motionId, slot, dataUnitFileGet(unit, calcUPGet(unit)->charaId));
    return 1;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadMotSp2);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataXtxFileGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataXtxAdrGet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataThinkNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataThinkDataSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataThinkFileLoad);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataThinkLoad);

/* dataMapLoad passes the map number it is loading; every map loads into
 * the same fixed EE RAM region, so the number is ignored. */
void *dataMapLoadAdrGet(int mapNo)
{
    return (void *)0x01b9e000;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataMapCameraAdrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataMapLoad);

void dataVPadModeSet(int mode)
{
    padMode = mode;
    padData = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataVPadSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPadRead);
