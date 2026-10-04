#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "main/xgl_packet.h"
#include "xgl_render.h"

extern int sceGsSyncV(int mode);

static int s_nGblFadeInit;
static int s_nGblFade;
static int s_nClearFrame;
int FrameCount;
volatile int VSyncCount;
int ScanLineInterpolate;
unsigned char DrawEnv[0x100] = { 0 };
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

extern void xglRenderVSyncCallback(void);
extern void *sceGsSyncVCallback(void (*func)(void));
extern signed char FLAG_FRAME_60;

static void xglRenderSyncInit(void)
{
    sceGsSyncVCallback(xglRenderVSyncCallback);
    VSyncCount = 0;
}

static void xglRenderSyncMove(void)
{
    if (FLAG_FRAME_60 == 1) {
        sceGsSyncV(0);
        return;
    }

    while (sceGsSyncV(0) != 0) {
    }
    while ((unsigned int)VSyncCount < 2U) {
    }

    if ((unsigned int)VSyncCount >= 3U) {
        sRender.frame_delta = 1;
    }
    VSyncCount = 0;
}

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

void xglRenderSwapBase(void)
{
    u32 accumulated_frames;
    u16 next_display_base;
    u16 next_draw_base;
    u16 previous_width;

    accumulated_frames = sRender.frame_count + sRender.frame_delta;
    next_draw_base = sRender.draw_buffer_base;
    next_display_base = sRender.display_buffer_base;
    previous_width = sRender.width;

    sRender.width = next_draw_base;
    sRender.height = previous_width;
    sRender.display_buffer_base = next_draw_base;
    sRender.draw_buffer_base = next_display_base;
    sRender.render_status = 0;
    sRender.frame_delta = 0;
    sRender.frame_count = accumulated_frames;
    sRender.frame_status = 0;
}

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

void xglRenderGlobalFadeInit(void)
{
    s_nGblFadeInit = 1;
    sRender.fade_callback = 0;
    s_nGblFade = 0;
}

/*
 * The packed global fade color that xglRenderGlobalFade (still asm, this TU)
 * draws over the scene: red/green/blue scaled to a byte (0-255) at
 * +0x00/+0x08/+0x10 and alpha scaled to a byte (0-128, the PS2 GS
 * convention) at +0x18, the same GS_RGBAQ-style packing xglRenderClearColor
 * above unpacks for ClearEnv's own color fields. Its storage belongs to
 * xglRenderGlobalFadeInit (still asm, this TU).
 */
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

typedef union GifCommandData {
    u64 value;
    struct {
        u32 low;
        u32 high;
    } words;
} GifCommandData;

typedef struct GifTag {
    u32 control_low;
    u32 control_high;
    u32 registers_low;
    u32 registers_high;
} GifTag;

typedef struct GifAdCommand {
    GifCommandData data;
    u32 register_address;
    u32 unused;
} GifAdCommand;

typedef struct GifRgbaq {
    GifCommandData red_green;
    u32 blue;
    u32 alpha;
} GifRgbaq;

typedef struct GifXyz2 {
    u16 x;
    u16 unused_x;
    u16 y;
    u16 unused_y;
    u32 z;
    u32 unused;
} GifXyz2;

typedef struct TestEnvironmentPacket {
    u64 dma_tag;
    u32 vif_nop;
    u32 vif_direct;
    GifTag gif_tag;
    GifAdCommand primitive;
    GifRgbaq color;
    GifXyz2 top_left;
    GifXyz2 bottom_right;
} TestEnvironmentPacket;

/* VIF DIRECT sends one GIFtag, one A+D PRIM, one RGBAQ and two XYZ2 values. */
static TestEnvironmentPacket TestEnv_0_004A8E80 = {
    .dma_tag = 0,
    .vif_nop = 0,
    .vif_direct = 0x51000005,
    .gif_tag = { 1 | (1 << 15), 0x40034000, 0x551E, 0 },
    .primitive = { { .value = 0x0000000000071001ULL }, 0x47, 0 },
    .color = { { .value = 0 }, 0, 0 },
    .top_left = { 0x6FF8, 0, 0x71F7, 0, 0x40000000, 0 },
    .bottom_right = { 0x8FF8, 0, 0x8DF7, 0, 0x40000000, 0 },
};

GsDispEnv DispEnv = { 0 };
GsClearEnv ClearEnv = { 0 };
XglRenderState sRender = { 0 };

static void xglRenderClear(void)
{
    TestEnv_0_004A8E80.color.red_green.words.low = ClearEnv.color_r;
    TestEnv_0_004A8E80.color.red_green.words.high = ClearEnv.color_g;
    TestEnv_0_004A8E80.color.blue = ClearEnv.color_b;
    TestEnv_0_004A8E80.color.alpha = 0x80;
    sceVif1PkRef(xglPacketGetCurrent(), &TestEnv_0_004A8E80, 6, 0, 0, 0);
}

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

static int s_nGblFadeInit = 0;
static int s_nGblFade = 0;
static int s_nClearFrame = 0;
int FrameCount = 0;
volatile int VSyncCount = 0;
int ScanLineInterpolate = 0;
