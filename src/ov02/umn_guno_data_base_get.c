/*
 * OV02 original TU 10: 0x00a097d8..0x00a0baf8 (10 functions)
 */
#include "common.h"
#include "shared.h"

extern void *UmnGunoDataBaseTop;

int UmnGunoDataBaseGet(int id)
{
    int base = (int) UmnGunoDataBaseTop;

    if ((u32) (id - 0x22) < 0x1D) {
        return base + id * 0x68 - 0xDD0;
    }
    return base;
}

INCLUDE_ASM("asm/nonmatchings/ov02/umn_guno_data_base_get", tskUmnDataBasePas);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_guno_data_base_get", tskUmnDataBaseInfo);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_guno_data_base_get", tskUmnDataBaseName);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_guno_data_base_get", tskUmnDataBaseAnalisis);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_guno_data_base_get", tskUmnDataBaseMenu);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_guno_data_base_get", tskUmnDataBaseExWin);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_guno_data_base_get", tskUmnDataBaseKeyWord);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_guno_data_base_get", UmnDataBaseModel);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_guno_data_base_get", UmnDataBase);
