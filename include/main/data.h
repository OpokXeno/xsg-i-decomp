#ifndef INCLUDE_MAIN_DATA_H
#define INCLUDE_MAIN_DATA_H

#include "shared.h"

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
