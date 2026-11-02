#ifndef INCLUDE_MAIN_FX_SCREEN_MASK_H
#define INCLUDE_MAIN_FX_SCREEN_MASK_H

#include "shared.h"

/* Functions and data used by other translation units. */
extern void FX_call(int command, int effect_index, const void *arguments,
                    int argument_count);
void FX_ScreenMask(int unused, int color, int duration, int mode);

#endif /* INCLUDE_MAIN_FX_SCREEN_MASK_H */
