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

int dataItmBoxChk(int id)
{
    return dataBoxChk(0, id);
}

int dataItmBoxInc(int id)
{
    return dataBoxInc(0, id);
}

void dataItmBoxDec(int id)
{
    dataBoxDec(0, id);
}

int dataWpnBoxChk(int weapon_id)
{
    return dataBoxChk(1, weapon_id);
}

void dataWpnBoxInc(int weapon_id)
{
    dataBoxInc(1, weapon_id);
}

void dataWpnBoxDec(int weapon_id)
{
    dataBoxDec(1, weapon_id);
}

int dataBltBoxChk(int bullet_id)
{
    return dataBoxChk(2, bullet_id);
}

void dataBltBoxInc(int bullet_id)
{
    dataBoxInc(2, bullet_id);
}

void dataBltBoxDec(int bullet_id)
{
    dataBoxDec(2, bullet_id);
}

int dataAccBoxChk(int accessory_id)
{
    return dataBoxChk(3, accessory_id);
}

void dataAccBoxInc(int accessory_id)
{
    dataBoxInc(3, accessory_id);
}

void dataAccBoxDec(int accessory_id)
{
    dataBoxDec(3, accessory_id);
}

int dataEvtBoxChk(int event_id)
{
    return dataBoxChk(10, event_id);
}

void dataEvtBoxInc(int event_id)
{
    dataBoxInc(10, event_id);
}

void dataEvtBoxDec(int event_id)
{
    dataBoxDec(10, event_id);
}

INCLUDE_ASM("asm/main/nonmatchings/data", dataMoneyBoxChk);

INCLUDE_ASM("asm/main/nonmatchings/data", dataMoneyBoxInc);

INCLUDE_ASM("asm/main/nonmatchings/data", dataMoneyBoxDec);
