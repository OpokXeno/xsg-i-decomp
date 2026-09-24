#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecSaveDataGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecDataGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", BitToTecNo);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecNextTLevPointGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecTLevLimitCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecTLevUp);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecNextSpeedPointGet);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecSpeedLimitCheck);

int MenuTecSaveDataGet(int chrNo);
int MenuTecNextSpeedPointGet(int chrNo, int point);

/* dataPlChaGet (ov01/data_unit_org_get.c VA 0x00a19210, still INCLUDE_ASM
 * there and not yet linkable by that name): returns the running character's
 * control record, whose word at +0xC this function decrements. Kept under
 * the scaffold's undefined_funcs_auto.txt placeholder name until that TU
 * recovers it. */
int *func_A19210(int chrNo);

void MenuTecSpeedUp(int chrNo, int point)
{
    int maskedChrNo = chrNo & 0xFFFF;
    int saveData = MenuTecSaveDataGet(maskedChrNo);
    int *unit = func_A19210(maskedChrNo);

    if (point >= 0) {
        int points = MenuTecNextSpeedPointGet(chrNo, point);

        ((unsigned char *)(saveData + point))[8] = 1;
        unit[3] -= points;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecNextWaitPointGet);

int MenuTecWaitLimitCheck(int chrNo, int point)
{
    void *record = (void *)(point + MenuTecSaveDataGet(chrNo));
    int limitReached = 0;

    if (point >= 0) {
        limitReached = ((unsigned char *)record)[0x10] != 0;
    }
    return limitReached;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecWaitUp);

typedef struct MenuWorkState {
    unsigned char unmodeled_00[0x40];
    signed char characterNo;
    unsigned char unmodeled_41[3];
    signed char point;
    unsigned char unmodeled_45[0x3b];
} MenuWorkState;
extern MenuWorkState MenuWork;
int MenuTecTLevLimitCheck(int chrNo, int point);
int MenuTecNextTLevPointGet(int chrNo, int point);

int MenuTecCharTLevUpCheck(void)
{
    int *characterData;
    int nextPoint;

    if (MenuTecTLevLimitCheck(MenuWork.characterNo, MenuWork.point) != 0) {
        characterData = func_A19210(MenuWork.characterNo);
        nextPoint = MenuTecNextTLevPointGet(MenuWork.characterNo, MenuWork.point);
        if (characterData[3] >= nextPoint)
            return 1;
    }

    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecCharSpeedUpCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecCharWaitUpCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecPasMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecInfoMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecMenuMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecExMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecL1R1Main);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecStatusMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecListMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", subMenuTecListChange_6);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecListMain2);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", subListChange_11);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecSetWinMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecSortSet00);

/* dataUnitOrgGet (ov01/data_unit_org_get.c VA 0x00a191c0, still INCLUDE_ASM
 * there and not yet linkable by that name): returns the running character's
 * parameter record (the full record is src/main/menu_para_pt_rate_get.c's
 * CharParaData); MenuTecEquipCheck only reads the six equipped technique ids
 * at +0x82. Kept under the scaffold's undefined_funcs_auto.txt placeholder
 * name until that TU recovers it. */
typedef struct MenuTecEquipRecord {
    unsigned char unmodeled_00[0x82];
    short techniqueId[6]; /* +0x82 */
} MenuTecEquipRecord;
MenuTecEquipRecord *func_A191C0(int chrNo);

int MenuTecEquipCheck(int chrNo, int tecNo)
{
    int maskedTecNo = tecNo & 0xFFFF;
    MenuTecEquipRecord *equip = func_A191C0(chrNo & 0xFFFF);
    int count = 0;
    int i;

    for (i = 0; i < 6; i++) {
        if (equip->techniqueId[i] == maskedTecNo)
            count++;
    }

    return count;
}

/* dataTecGet (ov01/data_unit_org_get.c VA 0x00a1a378, still INCLUDE_ASM there
 * and not yet linkable by that name): returns a pointer to the technique's
 * metadata record, or a NULL pointer plus a diagnostic print for a
 * non-positive index. Kept under the scaffold's undefined_funcs_auto.txt
 * placeholder name until that TU recovers it. */
unsigned char *func_A1A378(int tecBit);

/* Returns whether either of the technique's two low target-metadata bits is
 * set (elf_names annotation). */
int MenuTecTrgCheck(int tecBit)
{
    return (func_A1A378(tecBit & 0xFFFF)[0] & 3) != 0;
}

int BitToTecNo(int bit);
int MenuTecSaveDataGet(int chrNo);

int MenuTecTypeCheck(int chrNo, int bit)
{
    int tecNo;

    chrNo = chrNo & 0xFFFF;
    tecNo = BitToTecNo(bit);
    return ((unsigned char *)(MenuTecSaveDataGet(chrNo) + tecNo))[8] == 1;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecListMake00);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecListMake00_2);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecListMake01);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTec);
