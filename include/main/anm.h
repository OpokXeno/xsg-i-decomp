#ifndef INCLUDE_MAIN_ANM_H
#define INCLUDE_MAIN_ANM_H

#include "shared.h"

/* Functions and data used by other translation units. */
void *PACK_getEntry(FpkFcvPack *pack, int index);
FpkAnimationEntry *ANM_getEntry(AnmClock *clock, FpkFcvPack *pack);
FpkAnimationEntry *ANM_reset(AnmClock *clock, FpkFcvPack *pack);
void ANM_update(AnmClock *clock);
FpkAnimationEntry *ANM_resetDefault(AnmDefaultState *state, FpkFcvPack *pack);

#endif /* INCLUDE_MAIN_ANM_H */
