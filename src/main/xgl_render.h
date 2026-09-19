#ifndef SRC_MAIN_XGL_RENDER_H
#define SRC_MAIN_XGL_RENDER_H

#include "shared.h"

/*
 * GS clear-environment block (0x004A9080, size 0x60). xglRenderClearDepth
 * and xglRenderClearFrame each write only the 8-byte tag at +0x10 (a
 * GS-register-shaped value selecting what a queued clear targets);
 * xglRenderClearColor splits a packed 0xAABBGGRR colour into the four
 * one-byte-per-word fields at +0x20..+0x2c. Bytes no claimed function
 * touches stay unmodeled.
 */
typedef struct GsClearEnv {
    unsigned char unmodeled_00[0x10];
    u64 target;
    unsigned char unmodeled_18[0x08];
    u32 color_r;
    u32 color_g;
    u32 color_b;
    u32 color_a;
    unsigned char unmodeled_30[0x30];
} GsClearEnv;

/*
 * GS display-environment block (0x004A9000, size 0x50). xglRenderDispOff/On
 * only touch the 8-byte flag word at +0x00 (bit 0x2 gates display output).
 */
typedef struct GsDispEnv {
    u64 flags;
    unsigned char unmodeled_08[0x48];
} GsDispEnv;

extern GsClearEnv ClearEnv;
extern GsDispEnv DispEnv;

#endif
