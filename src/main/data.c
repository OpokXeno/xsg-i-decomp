#include "common.h"
#include "shared.h"
#include "data.h"
#include "main/data.h"
#include "main/sdv.h"

extern u16 accBox[DATA_BOX_SLOTS];
extern u16 bltBox[DATA_BOX_SLOTS];
extern u16 evtBox[DATA_BOX_SLOTS];
extern u16 itmBox[DATA_BOX_SLOTS];
extern u16 wpnBox[DATA_BOX_SLOTS];
/* Returns the quantity table of an item category, or 0 for an unknown category. */
u16 *dataBoxPtrGet(int category) {
    switch (category) {
    case ITEM_CATEGORY_ITEM:
        return itmBox;
    case ITEM_CATEGORY_WEAPON:
        return wpnBox;
    case ITEM_CATEGORY_BULLET:
        return bltBox;
    case ITEM_CATEGORY_ACCESSORY:
        return accBox;
    case ITEM_CATEGORY_EVENT:
        return evtBox;
    default:
        return 0;
    }
}

/* Returns the quantity of item `id` (1-based) in the category's table. */
int dataBoxChk(int category, int id)
{
    return dataBoxPtrGet(category)[id - 1];
}

/* Adds one to the quantity of item `id` unless it is at DATA_BOX_QUANTITY_MAX; returns 1 when added. */
int dataBoxInc(int category, int id)
{
    unsigned short *quantity;

    quantity = dataBoxPtrGet(category) + id - 1;
    if (*quantity < DATA_BOX_QUANTITY_MAX) {
        *quantity += 1;
        return 1;
    }
    return 0;
}

/* Removes one from the quantity of item `id` when it is nonzero; returns 1 when removed. */
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
    return dataBoxChk(ITEM_CATEGORY_ITEM, id);
}

int dataItmBoxInc(int id)
{
    return dataBoxInc(ITEM_CATEGORY_ITEM, id);
}

void dataItmBoxDec(int id)
{
    dataBoxDec(ITEM_CATEGORY_ITEM, id);
}

int dataWpnBoxChk(int weapon_id)
{
    return dataBoxChk(ITEM_CATEGORY_WEAPON, weapon_id);
}

void dataWpnBoxInc(int weapon_id)
{
    dataBoxInc(ITEM_CATEGORY_WEAPON, weapon_id);
}

void dataWpnBoxDec(int weapon_id)
{
    dataBoxDec(ITEM_CATEGORY_WEAPON, weapon_id);
}

int dataBltBoxChk(int bullet_id)
{
    return dataBoxChk(ITEM_CATEGORY_BULLET, bullet_id);
}

void dataBltBoxInc(int bullet_id)
{
    dataBoxInc(ITEM_CATEGORY_BULLET, bullet_id);
}

void dataBltBoxDec(int bullet_id)
{
    dataBoxDec(ITEM_CATEGORY_BULLET, bullet_id);
}

int dataAccBoxChk(int accessory_id)
{
    return dataBoxChk(ITEM_CATEGORY_ACCESSORY, accessory_id);
}

void dataAccBoxInc(int accessory_id)
{
    dataBoxInc(ITEM_CATEGORY_ACCESSORY, accessory_id);
}

void dataAccBoxDec(int accessory_id)
{
    dataBoxDec(ITEM_CATEGORY_ACCESSORY, accessory_id);
}

int dataEvtBoxChk(int event_id)
{
    return dataBoxChk(ITEM_CATEGORY_EVENT, event_id);
}

void dataEvtBoxInc(int event_id)
{
    dataBoxInc(ITEM_CATEGORY_EVENT, event_id);
}

void dataEvtBoxDec(int event_id)
{
    dataBoxDec(ITEM_CATEGORY_EVENT, event_id);
}

int dataMoneyBoxChk(void)
{
    return moneyBox;
}

/* Adds `amount` to the money box, clamping it to MONEY_BOX_MAX at the limit; returns 0 when the limit is reached. */
int dataMoneyBoxInc(int amount)
{
    moneyBox += amount;
    if (moneyBox >= MONEY_BOX_LIMIT) {
        moneyBox = MONEY_BOX_MAX;
        return 0;
    }

    return 1;
}

/* Subtracts `amount` from the money box when it holds enough; returns 1 when done, 0 otherwise. */
int dataMoneyBoxDec(int amount)
{
    if (moneyBox < amount) {
        return 0;
    }

    moneyBox -= amount;
    return 1;
}
