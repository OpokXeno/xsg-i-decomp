#ifndef INCLUDE_MAIN_DATA_H
#define INCLUDE_MAIN_DATA_H

#include "shared.h"

/* Inventory categories: the first argument of dataBoxInc/dataBoxChk/dataBoxDec. */
#define ITEM_CATEGORY_ITEM 0
#define ITEM_CATEGORY_WEAPON 1
#define ITEM_CATEGORY_BULLET 2
#define ITEM_CATEGORY_ACCESSORY 3
#define ITEM_CATEGORY_EVENT 10 /* event items: names come from EvtItemTbl */

/* Each category keeps DATA_BOX_SLOTS quantities of at most DATA_BOX_QUANTITY_MAX. */
#define DATA_BOX_SLOTS 255
#define DATA_BOX_QUANTITY_MAX 99

/* Money: dataMoneyBoxInc caps the purse at MONEY_BOX_MAX and reports a total
 * of MONEY_BOX_LIMIT or more as failure. */
#define MONEY_BOX_LIMIT 1000000000ULL
#define MONEY_BOX_MAX 999999999

/* Functions and data used by other translation units. */
extern int dataBoxInc(int category, int id);
int dataMoneyBoxInc(int amount);
extern int dataBoxChk(int category, int id);
int dataItmBoxChk(int id);
int dataItmBoxInc(int id);
void dataItmBoxDec(int id);
int dataWpnBoxChk(int weapon_id);
void dataWpnBoxInc(int weapon_id);
void dataWpnBoxDec(int weapon_id);
int dataBltBoxChk(int bullet_id);
void dataBltBoxInc(int bullet_id);
void dataBltBoxDec(int bullet_id);
int dataAccBoxChk(int accessory_id);
void dataAccBoxInc(int accessory_id);
void dataAccBoxDec(int accessory_id);
int dataEvtBoxChk(int event_id);
void dataEvtBoxInc(int event_id);
void dataEvtBoxDec(int event_id);
int dataBoxDec(int category, int id);
int dataMoneyBoxChk(void);
int dataMoneyBoxDec(int amount);

#endif /* INCLUDE_MAIN_DATA_H */
