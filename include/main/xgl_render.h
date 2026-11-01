#ifndef INCLUDE_MAIN_XGL_RENDER_H
#define INCLUDE_MAIN_XGL_RENDER_H

#include "shared.h"
#include "main/xgl_2.h"

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

extern XglRenderState sRender;

#endif /* INCLUDE_MAIN_XGL_RENDER_H */
