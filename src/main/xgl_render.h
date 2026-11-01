#ifndef SRC_MAIN_XGL_RENDER_H
#define SRC_MAIN_XGL_RENDER_H

#include "main/xgl_2.h"
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
    u32 gif_tag_low;
    u32 gif_tag_high;
    u32 gif_registers_low;
    u32 gif_registers_high;
    u64 target;
    u64 target_address;
    u32 color_r;
    u32 color_g;
    u32 color_b;
    u32 color_a;
    u32 left;
    u32 top;
    unsigned char unmodeled_38[0x08];
    u32 right;
    u32 bottom;
    unsigned char unmodeled_48[0x08];
    u64 restore_target;
    u64 restore_target_address;
} GsClearEnv;

typedef union GsClearPacket {
    GsClearEnv value;
    XglQuadword quadwords[6];
} GsClearPacket;

/*
 * GS display-environment block (0x004A9000, size 0x50). xglRenderDispOff/On
 * only touch the 8-byte flag word at +0x00 (bit 0x2 gates display output).
 */
typedef struct GsDispEnv {
    u64 flags;
    unsigned char unmodeled_08[0x48];
} GsDispEnv;

typedef void (*XglRenderFadeCallback)(XglPacket *packet);

/* The selected render flip body reads these resolution fields. */
typedef struct XglRenderReso {
    unsigned char unmodeled_00[2];
    short frame_psm;                   /* +0x02 */
    short frame_width;                 /* +0x04 */
    unsigned char unmodeled_06[2];
} XglRenderReso;

typedef struct XglRenderState {
    XglRenderReso reso;                /* +0x00 */
    unsigned char unmodeled_08[0x08];
    u16 width;                         /* +0x10 */
    u16 height;                        /* +0x12 */
    u16 flip_base;                     /* +0x14 */
    unsigned char unmodeled_16[4];
    u16 texture_buffer_base;            /* +0x1a */
    unsigned char unmodeled_1c[4];
    u16 display_buffer_base;            /* +0x20 */
    u16 draw_buffer_base;               /* +0x22 */
    XglRenderFadeCallback final_packet_callback[8]; /* +0x24 */
    XglRenderFadeCallback fade_callback; /* +0x44 */
    u32 render_status;                  /* +0x48 */
    u32 frame_delta;                    /* +0x4c */
    u32 frame_count;                    /* +0x50 */
    u32 frame_status;                   /* +0x54 */
    unsigned char scene_disabled;       /* +0x58 */
    unsigned char unmodeled_59[3];
} XglRenderState;

extern GsClearPacket ClearEnv;
extern GsDispEnv DispEnv;
extern XglRenderState sRender;

#endif
