#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "xgl_render.h"

extern int s_nClearFrame;
extern void xglRenderInit(void);
static void xglRenderMove(void);
extern void xglPadRead(void);
extern void xglSendSePacket(void);

extern void xglDmaDirectNormal(u32 channel, u32 address, u32 count);
extern void SyncDCache(void *start, void *end);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderDrawFlip);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderCopyDisp2Draw);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderDrawFlipPk);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderVSyncCallback);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderSyncInit);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderSyncMove);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderDrawEnvInit);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderDrawEnvMove);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderDispEnvInit);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderDispEnvMove);

void xglRenderClearDepth(void)
{
    ClearEnv.target = 0x00032001;
}

void xglRenderClearFrame(void)
{
    ClearEnv.target = 0x00030003;
}

void xglRenderClearColor(u32 color)
{
    ClearEnv.color_r = color & 0xff;
    ClearEnv.color_g = (color >> 8) & 0xff;
    ClearEnv.color_b = (color >> 16) & 0xff;
    ClearEnv.color_a = (color >> 24) & 0xff;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderClearEnvInit);

void xglRenderClearEnvMove(void)
{
    SyncDCache(&ClearEnv, (u8 *) &ClearEnv + sizeof(ClearEnv));
    xglDmaDirectNormal(2, (u32) &ClearEnv, 6);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderSetReso);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderSwapBase);

void xglRenderDispOff(void)
{
    DispEnv.flags &= ~(u64) 2;
}

void xglRenderDispOn(void)
{
    DispEnv.flags |= 2;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderInit);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderFinalPacket);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderGlobalFadeInit);

/*
 * The packed global fade color that xglRenderGlobalFade (still asm, this TU)
 * draws over the scene: red/green/blue scaled to a byte (0-255) at
 * +0x00/+0x08/+0x10 and alpha scaled to a byte (0-128, the PS2 GS
 * convention) at +0x18, the same GS_RGBAQ-style packing xglRenderClearColor
 * above unpacks for ClearEnv's own color fields. Its storage belongs to
 * xglRenderGlobalFadeInit (still asm, this TU).
 */
extern int s_nGblFade;

void xglRenderGlobalFadeSet(float red, float green, float blue, float alpha)
{
    if (red < 0.0f)
    {
        red = 0.0f;
    }
    if (green < 0.0f)
    {
        green = 0.0f;
    }
    if (blue < 0.0f)
    {
        blue = 0.0f;
    }
    if (alpha < 0.0f)
    {
        alpha = 0.0f;
    }
    if (red > 1.0f)
    {
        red = 1.0f;
    }
    if (green > 1.0f)
    {
        green = 1.0f;
    }
    if (blue > 1.0f)
    {
        blue = 1.0f;
    }
    if (alpha > 1.0f)
    {
        alpha = 1.0f;
    }
    s_nGblFade = (((int) (red * 255.0f)) & 0xFF) + (((int) (alpha * 128.0f)) << 0x18);
    s_nGblFade = s_nGblFade + (((((int) (blue * 255.0f)) & 0xFF) << 0x10) + ((((int) (green * 255.0f)) & 0xFF) << 8));
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderGlobalFade);

void xglRenderClearOn(void)
{
    s_nClearFrame = 2;
}

void xglRenderClearOff(void)
{
    s_nClearFrame = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderClear);

INCLUDE_ASM("asm/main/nonmatchings/xgl_render", xglRenderMove);

void xglRenderEntry(void)
{
    xglSleep();
    xglRenderInit();
    for (;;) {
        xglRenderMove();
        xglSendSePacket();
        xglPadRead();
        xglSleep();
    }
}
