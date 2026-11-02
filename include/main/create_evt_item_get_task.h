#ifndef INCLUDE_MAIN_CREATE_EVT_ITEM_GET_TASK_H
#define INCLUDE_MAIN_CREATE_EVT_ITEM_GET_TASK_H

#include "shared.h"

/* Functions and data used by other translation units. */
/* Event-item IDs index a 0x80-byte formatted name slot. */
extern char EvtItemTbl[255 * 0x80];
extern ItemBoxRecord ItemBoxTbl[601];
extern const char *GetItemName(int category, int id);
extern void MAP_updateUnitItemSymbol(MapUnitRecordItemView *unit);
extern void MAP_updateUnitItemBox(MapUnitRecordItemView *unit);
extern void MAP_drawUnitItemBox(UnitItemBox *box);
char *dataEvtItmNameGet(int index);
void InitItemSymbol(MapUnitRecordItemView *unit);
int HexToStr(int value, char *result);

#endif /* INCLUDE_MAIN_CREATE_EVT_ITEM_GET_TASK_H */
