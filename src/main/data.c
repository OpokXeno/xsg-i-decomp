#include "common.h"
#include "shared.h"
#include "data.h"

INCLUDE_ASM("asm/main/nonmatchings/data", dataBoxPtrGet);

INCLUDE_ASM("asm/main/nonmatchings/data", dataBoxChk);

int dataBoxInc(int category, int id)
{
    unsigned short *quantity;

    quantity = dataBoxPtrGet(category) + id - 1;
    if (*quantity < 99) {
        *quantity += 1;
        return 1;
    }
    return 0;
}

int dataBoxDec(int category, int id)
{
    unsigned short *quantity;

    quantity = dataBoxPtrGet(category) + id - 1;
    if (*quantity != 0) {
        *quantity -= 1;
        return 1;
    }
    return 0;
}

INCLUDE_ASM("asm/main/nonmatchings/data", dataItmBoxChk);

int dataItmBoxInc(int id)
{
    return dataBoxInc(0, id);
}

INCLUDE_ASM("asm/main/nonmatchings/data", dataItmBoxDec);

INCLUDE_ASM("asm/main/nonmatchings/data", dataWpnBoxChk);

INCLUDE_ASM("asm/main/nonmatchings/data", dataWpnBoxInc);

INCLUDE_ASM("asm/main/nonmatchings/data", dataWpnBoxDec);

INCLUDE_ASM("asm/main/nonmatchings/data", dataBltBoxChk);

INCLUDE_ASM("asm/main/nonmatchings/data", dataBltBoxInc);

INCLUDE_ASM("asm/main/nonmatchings/data", dataBltBoxDec);

INCLUDE_ASM("asm/main/nonmatchings/data", dataAccBoxChk);

INCLUDE_ASM("asm/main/nonmatchings/data", dataAccBoxInc);

INCLUDE_ASM("asm/main/nonmatchings/data", dataAccBoxDec);

INCLUDE_ASM("asm/main/nonmatchings/data", dataEvtBoxChk);

INCLUDE_ASM("asm/main/nonmatchings/data", dataEvtBoxInc);

INCLUDE_ASM("asm/main/nonmatchings/data", dataEvtBoxDec);

INCLUDE_ASM("asm/main/nonmatchings/data", dataMoneyBoxChk);

INCLUDE_ASM("asm/main/nonmatchings/data", dataMoneyBoxInc);

INCLUDE_ASM("asm/main/nonmatchings/data", dataMoneyBoxDec);
