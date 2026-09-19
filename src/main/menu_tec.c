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

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecWaitLimitCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecWaitUp);

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecCharTLevUpCheck);

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

INCLUDE_ASM("asm/main/nonmatchings/menu_tec", MenuTecEquipCheck);

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
