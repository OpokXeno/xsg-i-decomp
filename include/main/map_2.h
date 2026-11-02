#ifndef INCLUDE_MAIN_MAP_2_H
#define INCLUDE_MAIN_MAP_2_H

#include "shared.h"
/* MapUnitRecord (the game-wide MapUnit[] entry, 0x300 bytes) is canonical in
 * exm_2.h; the MAP view that used to be defined here is merged into it. */
#include "main/exm_2.h"

MapUnitRecord *MAP_createUnit(int slot, int serial);
void MAP_drawUnit(void);

/* Functions and data used by other translation units. */
void MAP_updateUnit(void);
void MAP_initUnit(void);
void MAP_drawUnitAt(MapUnitRecord *unit);

#endif /* INCLUDE_MAIN_MAP_2_H */
