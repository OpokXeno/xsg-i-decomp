#ifndef INCLUDE_OV12_RG_DEBUG_FLAGS_H
#define INCLUDE_OV12_RG_DEBUG_FLAGS_H

/*
 * The overlay's debug-mode toggle block. _InitRgDebugFlags enables
 * flags[0..4] and clears modeEnabled. rg_main.c's _IsEndOfMode reads
 * modeEnabled through InstanceOfRgDebugFlags().
 */
typedef struct RgDebugFlags {
    int flags[5];    /* 0x00: individual debug toggles, all enabled at init */
    int modeEnabled; /* 0x14: nonzero enables the game-mode debug shortcut */
} RgDebugFlags;

/* Functions and data used by other translation units. */
RgDebugFlags *InstanceOfRgDebugFlags(void);

#endif /* INCLUDE_OV12_RG_DEBUG_FLAGS_H */
