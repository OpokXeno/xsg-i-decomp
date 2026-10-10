/*
 * TU-local declarations of ov02/tu002 (src/ov02/title.c).
 */

#ifndef SRC_OV02_TITLE_H
#define SRC_OV02_TITLE_H

#include "shared.h"

typedef signed short s16;

/* Only the ten bytes touched by particle_reset are recovered here. */
typedef struct TitleParticleView {
    u16 x;
    s16 z;
    s16 y;
    s16 depth;
    s16 speed;
} TitleParticleView;

/* copyframe reads the screen size and framebuffer-page halfwords of sRender
 * (the same object and offsets as main/tu130's DrawImageRenderState view). */
typedef struct {
    u8 unmodeled_00[0x10];
    u16 width;
    u16 height;
    u16 framebuffer_page;
} TitleRenderState;

extern TitleRenderState sRender;

extern void sceVif1PkCnt(XglPacket *packet, int count);

extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);

extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);

extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);

#endif /* SRC_OV02_TITLE_H */
