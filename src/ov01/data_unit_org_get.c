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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSpecLearnSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSpecLearnGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataSpecDataGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataNormIdxGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPlUnitInit);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataDummySet);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataCdSync);

void dataCdSyncClear(void)
{
    cdReqNum = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataNBreadCB);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFileLoadNB);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFileInfoInit);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFileInfoInitChg);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataEnemyMdlChk);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataLeaderChk);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataLeaderCidGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataLeaderCidReset);

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

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadMotSp);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoadMotSp2);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoad2);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataUnitFileLoad2Sub);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPackWpnMdl2);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataMotAdrSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFpkAdrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataTid2WepSp);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFileLoadWepSp);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFileLoadWepSp2);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataFileLoadWepSpEnd);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataDefWpnGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataMtdRead);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataXtxFileGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataXtxAdrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataXtxLoad);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataBatDatLoad);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataThinkAdrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", thinkMapGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataThinkNameGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataThinkDataSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataThinkFileLoad);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataThinkLoad);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataMapLoadAdrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataMapCameraAdrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataMapLoad);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataVPadModeSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataVPadSet);

INCLUDE_ASM("asm/nonmatchings/ov01/data_unit_org_get", dataPadRead);
