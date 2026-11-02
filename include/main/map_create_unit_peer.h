#ifndef INCLUDE_MAIN_MAP_CREATE_UNIT_PEER_H
#define INCLUDE_MAIN_MAP_CREATE_UNIT_PEER_H

#include "shared.h"

/* Functions and data used by other translation units. */
void MAP_updateUnitPartsSequence(MapUnitRecord *unit);
void MAP_updateUnitSequence(MapUnitRecord *unit);
extern void SEQ_motionUnit(MapUnitMotionRecord *unit);
void MAP_updateUnitDefault(MapUnitRecord *unit);

#endif /* INCLUDE_MAIN_MAP_CREATE_UNIT_PEER_H */
