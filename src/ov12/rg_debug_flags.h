/*
 * TU-local declarations of ov12/tu012 (src/ov12/rg_debug_flags.c).
 */

#ifndef SRC_OV12_RG_DEBUG_FLAGS_H
#define SRC_OV12_RG_DEBUG_FLAGS_H

#include "shared.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

/*
 * The overlay's debug-mode toggle block. _InitRgDebugFlags evidences all six
 * members: it enables flags[0..4] and clears modeEnabled. rg_main.c's
 * _IsEndOfMode reads modeEnabled through InstanceOfRgDebugFlags(), the
 * singleton accessor this TU defines.
 */
typedef struct RgDebugFlags {
    int flags[5];    /* 0x00: individual debug toggles, all enabled at init */
    int modeEnabled; /* 0x14: nonzero enables the game-mode debug shortcut */
} RgDebugFlags;

RgDebugFlags *InstanceOfRgDebugFlags(void);

#endif /* SRC_OV12_RG_DEBUG_FLAGS_H */
