#ifndef INCLUDE_OV01_MENU_H
#define INCLUDE_OV01_MENU_H

#include "shared.h"
#include "ov01/battle_init.h"
#include "ov01/gr_gp_init.h"

/* Functions and data used by other translation units. */
void menuInitBat(void);
void menuOpenBat(BattleUnit *unit);
void menuCloseBat(void);
int menuResultBat(void);
int menuOpenChkBat(void);
void menuOpenEth(BattleUnit *unit);
void menuCloseEth(void);
int menuResultEth(int *ethId);
void menuOpenItm(BattleUnit *unit);
void menuCloseItm(void);
int menuResultItm(int *itmId);
void menuStatDispCreate(void);
void menuStatDispRemove(void);
void menuStatTexTrans(void);
void menuStockStart(void);
void menuStockNext(void);
void menuStockBoost(void);
void menuStockDead(int dead);
void menuTimeSet(int time);
int menuTimeNext(void);
int menuTimeGet(void);
int menuTimeXGet(void);
void menuBoostDisp(int enabled);
void menuCritDispOn(int count);
void menuCritDispOff(void);
void menuMapHSet(int enable);
void menuActCurPut(GrPacket *packet, ObjectTask *cursor, BattleUnit *unit, int frame);

#endif /* INCLUDE_OV01_MENU_H */
