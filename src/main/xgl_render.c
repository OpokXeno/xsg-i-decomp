#include "common.h"
#include "main/xgl_2.h"
#include "shared.h"
#include "main/xgl_thread.h"
#include "main/xgl_packet.h"
#include "main/Undulate.h"
#include "main/nml_model_set.h"
#include "main/xgl_dma.h"
#include "main/xgl_pad.h"
#include "main/xgl_studio.h"

/* VIF error registers used by the original renderer initialization. */
#define VIF0_ERR_ADDRESS 0x10003820

#define VIF1_ERR_ADDRESS 0x10003C20
#define VIF_ERR_MASK_ERROR_INTERRUPT 1
/* GIF_STAT and VIF1_STAT, polled by the VSync callback. */
#define GIF_STAT_ADDRESS 0x10003020
#define VIF1_STAT_ADDRESS 0x10003C00
#define GIF_STAT_APATH_MASK 0xC00 /* a GIF transfer path is active */
#define VIF1_STAT_VPS_MASK 3 /* the VIF1 pipeline is not idle */
/* CSR write that acknowledges the end-of-drawing-window interrupt (EDWINT). */
#define GS_CSR_EDWINT 0x10

/* Display resolution table rows (asDispReso / asVramBase). */
#define RENDER_RESO_640 0
#define RENDER_RESO_512 1
#define RENDER_RESO_448 2
#define RENDER_RESO_384 3
#define RENDER_RESO_320 4
#define RENDER_RESO_256 5
#define RENDER_COUNT_RESO 6
#define RENDER_HEIGHT 448
#define RENDER_COPY_WIDTH 0x200 /* xglRenderCopyDisp2Draw copies a 512 x RENDER_HEIGHT frame */
#define RENDER_COPY_BUFFER_WIDTH 8 /* the same width in 64-pixel units (BITBLTBUF SBW/DBW) */
#define GS_COORD_CENTER 2048 /* primitive coordinate system origin */

/* ScanLineInterpolate modes. */
#define GS_PMODE_EN2 2 /* PMODE.EN2: xglRenderDispOn/Off toggle the display output */

#define SCANLINE_INTERP_OFF 0
#define SCANLINE_INTERP_CIRCUIT_BLEND 1 /* second display circuit shifted one line and blended */
#define SCANLINE_INTERP_PACKET 2 /* interpolated by xglPacketInterpolate */
/* PMODE.ALP of the blended circuit. */
#define RENDER_CIRCUIT_BLEND_ALPHA 0x80

/*
 * s_nClearFrame: ClearOn sets RENDER_CLEAR_ON, which counts down to RENDER_CLEAR_DRAW.
 * The clear is drawn while the value is RENDER_CLEAR_DRAW, until ClearOff.
 */
#define RENDER_CLEAR_OFF 0
#define RENDER_CLEAR_DRAW 1
#define RENDER_CLEAR_ON 2

/* Vsyncs per frame: the render thread waits for two, and a third means the frame ran late. */
#define RENDER_VSYNC_PER_FRAME 2
#define RENDER_VSYNC_LATE 3
/* The final-packet callback table. */
#define RENDER_FINAL_CALLBACK_COUNT 8

/* GS TEST_1 values: clear modes of ClearEnv and the depth-test state of the draw environment. */
#define GS_ZTST_ALWAYS 1
#define GS_ZTST_GEQUAL 2
#define GS_TEST_ZTE_ALWAYS 0x30000
#define GS_TEST_ZTE_GEQUAL 0x50000
#define GS_TEST_CLEAR_DEPTH 0x00032001 /* alpha test never passes but writes Z only */
#define GS_TEST_CLEAR_FRAME 0x00030003
/* Default clear colour: black with GS alpha 0x80. */
#define RENDER_CLEAR_COLOR_DEFAULT 0x80000000
/* Fade ALPHA_1: (Cs - Cd) * FIX >> 7 + Cd with FIX in the upper word. */
#define GS_ALPHA_FADE_BLEND 100
/* Clear sprite GIF tag: PRE with a sprite PRIM, and the register list
 * A+D (state), RGBAQ, XYZ2, XYZ2 (two corners), plus a closing A+D for the clear packet. */
#define RENDER_CLEAR_GIF_REGS (GIF_REG_AD | (GIF_REG_RGBAQ << 4) | (GIF_REG_XYZ2 << 8) | (GIF_REG_XYZ2 << 12))
/* Fade primitive: sprite with alpha blending. */
#define GS_PRIM_SPRITE_ABE 70
/* TRXDIR: local to local transfer. */
#define GS_TRXDIR_LOCAL_TO_LOCAL 2
/* Scale of the 0..1 fade components to GS bytes (alpha is 0..0x80). */
#define RENDER_FADE_COLOR_SCALE 255.0f
#define RENDER_FADE_ALPHA_SCALE 128.0f
/* Quadword counts of the scratchpad GIF packets handed to DMA / VIF1. */
#define RENDER_FLIP_QWORDS 3
#define RENDER_COPY_QWORDS 6
#define RENDER_FADE_QWORDS 8
#define RENDER_CLEAR_QWORDS 6
#define GS_DISPFB2 ((volatile u64 *)0x12000070)
#define GS_DISPLAY2 ((volatile u64 *)0x12000080)
#define GS_EXTBUF ((volatile u64 *)0x120000B0)
#define GS_EXTDATA ((volatile u64 *)0x120000C0)
#define GS_EXTWRITE ((volatile u64 *)0x120000D0)
#define GS_CSR ((volatile u64 *)0x12001000)

/*
 * GS clear-environment block, 0x60 bytes. xglRenderClearDepth and
 * xglRenderClearFrame each write only the 8-byte target at +0x10 (a
 * GS-register-shaped value selecting what a queued clear targets);
 * xglRenderClearColor splits a packed 0xAABBGGRR colour into the four
 * one-byte-per-word fields at +0x20..+0x2c. Bytes that no function here
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

/* GS PMODE register: display circuit enables and the alpha blend value. */
typedef struct GsPmode {
    unsigned int en1 : 1;
    unsigned int en2 : 1;
    unsigned int crtmd : 3;
    unsigned int mmod : 1;
    unsigned int amod : 1;
    unsigned int slbg : 1;
    unsigned int alp : 8;
    unsigned int unmodeled_16 : 16;
    unsigned int unmodeled_32 : 32;
} GsPmode;

typedef struct GsDispfb {
    unsigned int fbp : 9;
    unsigned int fbw : 6;
    unsigned int psm : 5;
    unsigned int unmodeled_20 : 12;
    unsigned int dbx : 11;
    unsigned int dby : 11;
    unsigned int unmodeled_54 : 10;
} GsDispfb;

typedef struct GsDisplay {
    unsigned int dx : 12;
    unsigned int dy : 11;
    unsigned int magh : 4;
    unsigned int magv : 2;
    unsigned int unmodeled_29 : 3;
    unsigned int dw : 12;
    unsigned int dh : 11;
    unsigned int unmodeled_55 : 9;
} GsDisplay;

typedef union GsDispfbRegister {
    u64 value;
    GsDispfb field;
} GsDispfbRegister;

typedef union GsDisplayRegister {
    u64 value;
    GsDisplay field;
} GsDisplayRegister;

/*
 * GS display-environment block, 0x50 bytes. xglRenderDispOff and xglRenderDispOn
 * only touch the 8-byte flag word at +0x00 (bit 0x2 gates display output).
 */
typedef struct GsDispEnv {
    union {
        u64 flags;
        GsPmode pmode;
    };
    unsigned char unmodeled_08[8];
    GsDispfbRegister dispfb1;          /* +0x10 */
    GsDisplayRegister display1;        /* +0x18 */
    unsigned char unmodeled_20[8];
    GsDispfbRegister dispfb2;          /* +0x28 */
    GsDisplayRegister display2;        /* +0x30 */
    u64 extbuf;                        /* +0x38 */
    u64 extdata;                       /* +0x40 */
    u64 extwrite;                      /* +0x48 */
} GsDispEnv;

/*
 * Packed GS registers of the draw-environment block below, laid out as
 * the GS manual defines them (bit positions from the first field).
 */
typedef struct GsFrame {
    u64 fbp : 9;
    u64 unmodeled_09 : 7;
    u64 fbw : 6;
    u64 unmodeled_22 : 2;
    u64 psm : 6;
    u64 unmodeled_30 : 2;
    u64 fbmsk : 32;
} GsFrame;

typedef struct GsZbuf {
    u64 zbp : 9;
    u64 unmodeled_09 : 15;
    u64 psm : 4;
    u64 unmodeled_28 : 4;
    u64 zmsk : 1;
    u64 unmodeled_33 : 31;
} GsZbuf;

typedef struct GsXyoffset {
    u64 ofx : 16;
    u64 unmodeled_16 : 16;
    u64 ofy : 16;
    u64 unmodeled_48 : 16;
} GsXyoffset;

typedef struct GsScissor {
    u64 scax0 : 11;
    u64 unmodeled_11 : 5;
    u64 scax1 : 11;
    u64 unmodeled_27 : 5;
    u64 scay0 : 11;
    u64 unmodeled_43 : 5;
    u64 scay1 : 11;
    u64 unmodeled_59 : 5;
} GsScissor;

typedef struct GsTest {
    u64 ate : 1;
    u64 atst : 3;
    u64 aref : 8;
    u64 afail : 2;
    u64 date : 1;
    u64 datm : 1;
    u64 zte : 1;
    u64 ztst : 2;
    u64 unmodeled_19 : 45;
} GsTest;

typedef struct GsTexa {
    u64 ta0 : 8;
    u64 unmodeled_08 : 7;
    u64 aem : 1;
    u64 unmodeled_16 : 16;
    u64 ta1 : 8;
    u64 unmodeled_40 : 24;
} GsTexa;

typedef struct GsDimx {
    u64 dm00 : 3;
    u64 unmodeled_03 : 1;
    u64 dm01 : 3;
    u64 unmodeled_07 : 1;
    u64 dm02 : 3;
    u64 unmodeled_11 : 1;
    u64 dm03 : 3;
    u64 unmodeled_15 : 1;
    u64 dm10 : 3;
    u64 unmodeled_19 : 1;
    u64 dm11 : 3;
    u64 unmodeled_23 : 1;
    u64 dm12 : 3;
    u64 unmodeled_27 : 1;
    u64 dm13 : 3;
    u64 unmodeled_31 : 1;
    u64 dm20 : 3;
    u64 unmodeled_35 : 1;
    u64 dm21 : 3;
    u64 unmodeled_39 : 1;
    u64 dm22 : 3;
    u64 unmodeled_43 : 1;
    u64 dm23 : 3;
    u64 unmodeled_47 : 1;
    u64 dm30 : 3;
    u64 unmodeled_51 : 1;
    u64 dm31 : 3;
    u64 unmodeled_55 : 1;
    u64 dm32 : 3;
    u64 unmodeled_59 : 1;
    u64 dm33 : 3;
    u64 unmodeled_63 : 1;
} GsDimx;

typedef struct GsPrmodecont {
    u64 ac : 1;
    u64 unmodeled_01 : 63;
} GsPrmodecont;

typedef struct GsColclamp {
    u64 clamp : 1;
    u64 unmodeled_01 : 63;
} GsColclamp;

typedef struct GsDthe {
    u64 dthe : 1;
    u64 unmodeled_01 : 63;
} GsDthe;

typedef struct GsGifRegisters {
    u64 regs0 : 4;
    u64 unmodeled_04 : 60;
} GsGifRegisters;

typedef struct GsGifTagControl {
    u64 nloop : 15;
    u64 eop : 1;
    u64 unmodeled_16 : 30;
    u64 pre : 1;
    u64 prim : 11;
    u64 flg : 2;
    u64 nreg : 4;
} GsGifTagControl;

/*
 * GS draw-environment block, 0x100 bytes: a GIF A+D packet of
 * fifteen register writes, each an 8-byte value followed by its register
 * address, as sceGsPutDrawEnv sends it.
 */
typedef struct GsDrawEnv {
    GsGifTagControl gif_tag;
    GsGifRegisters gif_registers;
    GsFrame frame1;
    u64 frame1_address;
    GsZbuf zbuf1;
    u64 zbuf1_address;
    GsXyoffset xyoffset1;
    u64 xyoffset1_address;
    GsScissor scissor1;
    u64 scissor1_address;
    GsTest test1;
    u64 test1_address;
    GsFrame frame2;
    u64 frame2_address;
    GsZbuf zbuf2;
    u64 zbuf2_address;
    GsXyoffset xyoffset2;
    u64 xyoffset2_address;
    GsScissor scissor2;
    u64 scissor2_address;
    GsTest test2;
    u64 test2_address;
    GsPrmodecont prmodecont;
    u64 prmodecont_address;
    GsTexa texa;
    u64 texa_address;
    GsColclamp colclamp;
    u64 colclamp_address;
    GsDthe dthe;
    u64 dthe_address;
    GsDimx dimx;
    u64 dimx_address;
} GsDrawEnv;

typedef union GsDrawPacket {
    GsDrawEnv value;
    XglQuadword quadwords[16];
} GsDrawPacket;

typedef void (*XglRenderFadeCallback)(XglPacket *packet);

/* One row of asDispReso: the display geometry sceGsSetDefDispEnv and the draw environment take. */
typedef struct XglRenderReso {
    short depth_psm;                   /* +0x00 */
    short frame_psm;                   /* +0x02 */
    short frame_width;                 /* +0x04 */
    short frame_height;                /* +0x06 */
} XglRenderReso;

/* One row of asVramBase: the GS frame-buffer pages of a resolution. */
typedef struct XglRenderVram {
    unsigned short depth_base;         /* +0x08 */
    unsigned short unmodeled_0a[3];
    u16 width;                         /* +0x10 */
    u16 height;                        /* +0x12 */
    unsigned short flip_base;          /* +0x14 */
    unsigned short unmodeled_16[5];
} XglRenderVram;

typedef union XglRenderResoSlot {
    XglRenderReso row;
    short value[4];
} XglRenderResoSlot;

typedef union XglRenderVramSlot {
    XglRenderVram row;
    unsigned short value[12];
} XglRenderVramSlot;

typedef struct XglRenderState {
    XglRenderResoSlot reso;            /* +0x00 */
    XglRenderVramSlot vram;            /* +0x08 */
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

/*
 * One GIF packet quadword on the scratchpad. The halfword view makes the
 * packet stores and the sRender halfword reads share an alias set, so the
 * stores stay ahead of the reads.
 */
typedef union ScratchpadQuad {
    u64 dword[2];
    u16 half[8];
} ScratchpadQuad;

/* A 64-bit GIF command value, as one quadword or as two 32-bit words. */
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

typedef union RenderTestPacket {
    TestEnvironmentPacket value;
    XglQuadword quadwords[6];
} RenderTestPacket;

extern GsDrawPacket DrawEnv;
extern GsClearPacket ClearEnv;
extern GsDispEnv DispEnv;
extern XglRenderState sRender;
extern int sceGsSyncV(int mode);
extern int sceGsResetGraph(int mode, int interlace, int output, int ffmode);
extern void sceGsResetPath(void);
extern void xglRenderSetReso(int index);
extern void xglRenderGlobalFadeInit(void);
extern void xglRenderClearOff(void);
extern int nmlModelIsBackBufferRequest(void);
extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);
extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);
extern void FlushCache(int mode);
extern void xglRenderInit(void);
static void xglRenderMove(void);
extern void xglSendSePacket(void);
extern void SyncDCache(void *start, void *end);
extern int xglRenderVSyncCallback(void);
extern void *sceGsSyncVCallback(int (*func)(void));
extern int sceGsSyncPath(int mode, int timeout);
extern int nmlModelSendPacketChangeSignal(void);
extern void sefDrawEffect3D(void);
extern void xglFontFlush(void);
extern void xglPacketInterpolate(void);
extern void xglPacketMove(void);
extern void xglPacketSend(void);
extern void sceGsSetDefDispEnv(GsDispEnv *env, int psm, int width, int height, int dx, int dy);
extern void sceGsPutDrawEnv(GsDrawEnv *env);
extern void sceGsPutDispEnv(GsDispEnv *env);

static int s_nGblFadeInit = 0;

/*
 * Global fade colour as one packed int: red, green and blue as bytes 0 to 2 and
 * alpha as byte 3, each scaled from 0..1 by xglRenderGlobalFadeSet.
 */
static int s_nGblFade = 0;

static int s_nClearFrame = 0;

int FrameCount = 0;

int LoopCount = 0;

/* The GS VSync interrupt callback increments this counter while
 * xglRenderSyncMove polls it on the render thread. */
volatile u32 VSyncCount = 0;

int ScanLineInterpolate = 0;

static RenderTestPacket TestEnv_0_004A8E80;

static XglRenderReso asDispReso[RENDER_COUNT_RESO];

static XglRenderVram asVramBase[RENDER_COUNT_RESO];

static XglRenderReso asDispReso[RENDER_COUNT_RESO] = {
    { SCE_GS_PSMZ24, SCE_GS_PSMCT32, 640, RENDER_HEIGHT },
    { SCE_GS_PSMZ24, SCE_GS_PSMCT32, 512, RENDER_HEIGHT },
    { SCE_GS_PSMZ24, SCE_GS_PSMCT32, 448, RENDER_HEIGHT },
    { SCE_GS_PSMZ24, SCE_GS_PSMCT32, 384, RENDER_HEIGHT },
    { SCE_GS_PSMZ24, SCE_GS_PSMCT32, 320, RENDER_HEIGHT },
    { SCE_GS_PSMZ24, SCE_GS_PSMCT32, 256, RENDER_HEIGHT },
};

static XglRenderVram asVramBase[RENDER_COUNT_RESO] = {
    { 0x0000, { 0x008C, 0x0118, 0x01A4 }, 0x008C, 0x0118, 0x01A4, { 0x0230, 0x02BC, 0x01A4, 0x01C4, 0x01E4 } },
    { 0x0000, { 0x0070, 0x00E0, 0x0150 }, 0x0070, 0x00E0, 0x0150, { 0x01C0, 0x0230, 0x01C0, 0x01E0, 0x0200 } },
    { 0x0000, { 0x0062, 0x00C4, 0x0126 }, 0x0062, 0x00C4, 0x0126, { 0x0188, 0x01EA, 0x0188, 0x01A8, 0x01C8 } },
    { 0x0000, { 0x0054, 0x00A8, 0x00FC }, 0x0054, 0x00A8, 0x00FC, { 0x0150, 0x01A4, 0x01A4, 0x01C4, 0x01E4 } },
    { 0x0000, { 0x0046, 0x008C, 0x00D2 }, 0x0046, 0x008C, 0x00D2, { 0x0118, 0x015E, 0x01A4, 0x01C4, 0x01E4 } },
    { 0x0000, { 0x0038, 0x0070, 0x00A8 }, 0x0038, 0x0070, 0x00A8, { 0x00E0, 0x0118, 0x0150, 0x0170, 0x0190 } },
};

/* VIF DIRECT sends one GIFtag, one A+D PRIM, one RGBAQ and two XYZ2 values. */
static RenderTestPacket TestEnv_0_004A8E80 = { .value = {
    .dma_tag = 0,
    .vif_nop = 0,
    .vif_direct = VIF_CODE_DIRECTHL | 5,
    .gif_tag = { 1 | GIF_TAG_EOP, (4 << GIF_TAG_NREG_SHIFT) | (GS_PRIM_SPRITE << GIF_TAG_PRIM_SHIFT) | GIF_TAG_PRE, RENDER_CLEAR_GIF_REGS, 0 },
    .primitive = { { .value = 0x0000000000071001ULL }, SCE_GS_TEST_1, 0 },
    .color = { { .value = 0 }, 0, 0 },
    .top_left = { 0x6FF8, 0, 0x71F7, 0, 0x40000000, 0 },
    .bottom_right = { 0x8FF8, 0, 0x8DF7, 0, 0x40000000, 0 },
} };

GsDrawPacket DrawEnv = { .value = { 0 } };

GsDispEnv DispEnv = { 0 };

GsClearPacket ClearEnv = { .value = { 0 } };

XglRenderState sRender = { 0 };

/*
 * Swaps the display and flip buffer bases, then sends FRAME_1 and FRAME_2 writes for
 * the new display base through the GIF DMA channel.
 */
void xglRenderDrawFlip(void)
{
    u64 *scratch;
    u16 previous_base;
    u64 frame;

    scratch = (u64 *)SCRATCHPAD_BASE;
    previous_base = sRender.display_buffer_base;
    sRender.display_buffer_base = sRender.vram.row.flip_base;
    frame = sRender.display_buffer_base | ((u64)(sRender.reso.row.frame_width / 64) << GS_FRAME_FBW_SHIFT) |
            ((u64)sRender.reso.row.frame_psm << GS_FRAME_PSM_SHIFT);
    scratch[0] = ((u64)GIF_TAG_HIGH_NREG_1 << 32) | (GIF_TAG_EOP | 2);
    scratch[1] = GIF_REG_AD;
    scratch[3] = SCE_GS_FRAME_1;
    scratch[2] = frame;
    scratch[4] = frame;
    scratch[5] = SCE_GS_FRAME_2;
    sRender.vram.row.flip_base = previous_base;
    __asm__ __volatile__("sync" : : : "memory");
    FlushCache(0);
    xglDmaDirectNormal(XGL_DMA_CHANNEL_GIF, SCRATCHPAD_BASE, RENDER_FLIP_QWORDS);
}

/* Copies the display buffer into the draw buffer with a local-to-local GIF transfer through DMA. */
void xglRenderCopyDisp2Draw(void)
{
    ScratchpadQuad *header;
    ScratchpadQuad *texflush;
    ScratchpadQuad *bitbltbuf;
    ScratchpadQuad *trxpos;
    ScratchpadQuad *trxreg;
    ScratchpadQuad *trxdir;

    header = (ScratchpadQuad *)SCRATCHPAD_BASE;
    texflush = (ScratchpadQuad *)(SCRATCHPAD_BASE + 0x10);
    bitbltbuf = (ScratchpadQuad *)(SCRATCHPAD_BASE + 0x20);
    trxpos = (ScratchpadQuad *)(SCRATCHPAD_BASE + 0x30);
    trxreg = (ScratchpadQuad *)(SCRATCHPAD_BASE + 0x40);
    trxdir = (ScratchpadQuad *)(SCRATCHPAD_BASE + 0x50);
    header->dword[1] = GIF_REG_AD;
    texflush->dword[0] = 0;
    texflush->dword[1] = SCE_GS_TEXFLUSH;
    bitbltbuf->dword[0] = (u64)(sRender.display_buffer_base << 5) | ((u64)RENDER_COPY_BUFFER_WIDTH << GS_BITBLTBUF_SBW_SHIFT) | ((u64)sRender.reso.row.frame_psm << GS_BITBLTBUF_SPSM_SHIFT) | ((u64)(sRender.draw_buffer_base << 5) << GS_BITBLTBUF_DBP_SHIFT) | ((u64)RENDER_COPY_BUFFER_WIDTH << GS_BITBLTBUF_DBW_SHIFT) | ((u64)sRender.reso.row.frame_psm << GS_BITBLTBUF_DPSM_SHIFT);
    bitbltbuf->dword[1] = SCE_GS_BITBLTBUF;
    trxpos->dword[0] = 0;
    trxpos->dword[1] = SCE_GS_TRXPOS;
    trxreg->dword[0] = ((u64)RENDER_HEIGHT << 32) | RENDER_COPY_WIDTH;
    trxreg->dword[1] = SCE_GS_TRXREG;
    trxdir->dword[0] = GS_TRXDIR_LOCAL_TO_LOCAL;
    trxdir->dword[1] = SCE_GS_TRXDIR;
    header->dword[0] = ((u64)GIF_TAG_HIGH_NREG_1 << 32) | (GIF_TAG_EOP | 5);
    __asm__ __volatile__("sync" : : : "memory");
    FlushCache(0);
    xglDmaDirectNormal(XGL_DMA_CHANNEL_GIF, SCRATCHPAD_BASE, RENDER_COPY_QWORDS);
}

/* Copies the draw buffer into the flip buffer and adds the transfer to packet as a VIF1 direct block. */
void xglRenderDrawFlipPk(XglPacket *packet)
{
    ScratchpadQuad *header;
    ScratchpadQuad *texflush;
    ScratchpadQuad *bitbltbuf;
    ScratchpadQuad *trxpos;
    ScratchpadQuad *trxreg;
    ScratchpadQuad *trxdir;

    header = (ScratchpadQuad *)SCRATCHPAD_BASE;
    texflush = (ScratchpadQuad *)(SCRATCHPAD_BASE + 0x10);
    bitbltbuf = (ScratchpadQuad *)(SCRATCHPAD_BASE + 0x20);
    trxpos = (ScratchpadQuad *)(SCRATCHPAD_BASE + 0x30);
    trxreg = (ScratchpadQuad *)(SCRATCHPAD_BASE + 0x40);
    trxdir = (ScratchpadQuad *)(SCRATCHPAD_BASE + 0x50);
    header->dword[1] = GIF_REG_AD;
    texflush->dword[0] = 0;
    texflush->dword[1] = SCE_GS_TEXFLUSH;
    bitbltbuf->dword[0] = (u64)(sRender.draw_buffer_base << 5) | ((u64)RENDER_COPY_BUFFER_WIDTH << GS_BITBLTBUF_SBW_SHIFT) | ((u64)sRender.reso.row.frame_psm << GS_BITBLTBUF_SPSM_SHIFT) | ((u64)(sRender.vram.row.flip_base << 5) << GS_BITBLTBUF_DBP_SHIFT) | ((u64)RENDER_COPY_BUFFER_WIDTH << GS_BITBLTBUF_DBW_SHIFT) | ((u64)sRender.reso.row.frame_psm << GS_BITBLTBUF_DPSM_SHIFT);
    bitbltbuf->dword[1] = SCE_GS_BITBLTBUF;
    trxpos->dword[0] = 0;
    trxpos->dword[1] = SCE_GS_TRXPOS;
    trxreg->dword[0] = ((u64)RENDER_HEIGHT << 32) | RENDER_COPY_WIDTH;
    trxreg->dword[1] = SCE_GS_TRXREG;
    trxdir->dword[0] = GS_TRXDIR_LOCAL_TO_LOCAL;
    trxdir->dword[1] = SCE_GS_TRXDIR;
    header->dword[0] = ((u64)GIF_TAG_HIGH_NREG_1 << 32) | (GIF_TAG_EOP | 5);
    __asm__ __volatile__("sync" : : : "memory");
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    sceVif1PkAddDirectDataN(packet, (void *)SCRATCHPAD_BASE, RENDER_COPY_QWORDS);
    sceVif1PkCloseDirectHLCode(packet);
}

/*
 * VSync callback: counts the vertical sync, and sets frame_status when the GIF or VIF1
 * path is still busy at the second sync of a frame.
 */
int xglRenderVSyncCallback(void)
{
    /* GIF and VIF1 hardware status gate the frame callback update. */
    volatile u32 *const gif_status = (volatile u32 *)GIF_STAT_ADDRESS;
    volatile u32 *const vif1_status = (volatile u32 *)VIF1_STAT_ADDRESS;

    VSyncCount++;
    if (VSyncCount == RENDER_VSYNC_PER_FRAME) {
        if ((*gif_status & GIF_STAT_APATH_MASK) != 0 || (*vif1_status & VIF1_STAT_VPS_MASK) != 0) {
            sRender.frame_status = 1;
        }
    }
    return 0;
}

static void xglRenderSyncInit(void)
{
    sceGsSyncVCallback(xglRenderVSyncCallback);
    VSyncCount = 0;
}

/*
 * Waits for the frame's vertical syncs (one when FLAG_FRAME_60 is set). frame_delta is set
 * when three or more syncs were counted.
 */
static void xglRenderSyncMove(void)
{
    if (FLAG_FRAME_60 == 1) {
        sceGsSyncV(0);
        return;
    }

    while (sceGsSyncV(0) != 0) {
    }
    while (VSyncCount < RENDER_VSYNC_PER_FRAME) {
    }

    if (VSyncCount >= RENDER_VSYNC_LATE) {
        sRender.frame_delta = 1;
    }
    VSyncCount = 0;
}

/* Fills DrawEnv with the frame, z-buffer, offset, scissor and test state for the current resolution. */
static void xglRenderDrawEnvInit(void)
{
    /* GIF tag: fifteen A+D writes, one register each. */
    DrawEnv.value.gif_tag.nloop = 15;
    DrawEnv.value.gif_tag.eop = 1;
    DrawEnv.value.gif_tag.pre = 0;
    DrawEnv.value.gif_tag.prim = 0;
    DrawEnv.value.gif_tag.flg = 0;
    DrawEnv.value.gif_tag.nreg = 1;
    DrawEnv.value.gif_registers.regs0 = GIF_REG_AD;
    DrawEnv.value.frame1.fbp = sRender.vram.row.height;
    DrawEnv.value.frame1.fbw = sRender.reso.row.frame_width / 64;
    DrawEnv.value.frame1.psm = sRender.reso.row.frame_psm;
    DrawEnv.value.frame1.fbmsk = 0;
    DrawEnv.value.frame1_address = SCE_GS_FRAME_1;
    DrawEnv.value.zbuf1.zbp = sRender.vram.row.depth_base;
    DrawEnv.value.zbuf1.psm = sRender.reso.row.depth_psm;
    DrawEnv.value.zbuf1.zmsk = 0;
    DrawEnv.value.zbuf1_address = SCE_GS_ZBUF_1;
    DrawEnv.value.xyoffset1.ofx = (GS_COORD_CENTER - (sRender.reso.row.frame_width >> 1)) << 4;
    DrawEnv.value.xyoffset1.ofy = (GS_COORD_CENTER - (sRender.reso.row.frame_height >> 1)) << 4;
    DrawEnv.value.xyoffset1_address = SCE_GS_XYOFFSET_1;
    DrawEnv.value.scissor1.scax0 = 0;
    DrawEnv.value.scissor1.scax1 = sRender.reso.row.frame_width - 1;
    DrawEnv.value.scissor1.scay0 = 0;
    DrawEnv.value.scissor1.scay1 = sRender.reso.row.frame_height - 1;
    DrawEnv.value.scissor1_address = SCE_GS_SCISSOR_1;
    DrawEnv.value.test1.ate = 0;
    DrawEnv.value.test1.atst = 0;
    DrawEnv.value.test1.aref = 0;
    DrawEnv.value.test1.afail = 0;
    DrawEnv.value.test1.date = 0;
    DrawEnv.value.test1.datm = 0;
    DrawEnv.value.test1.zte = 1;
    DrawEnv.value.test1.ztst = GS_ZTST_GEQUAL;
    DrawEnv.value.test1_address = SCE_GS_TEST_1;
    /* Second draw context: the same frame, z-buffer, offset, scissor and test values. */
    DrawEnv.value.frame2.fbp = sRender.vram.row.height;
    DrawEnv.value.frame2.fbw = sRender.reso.row.frame_width / 64;
    DrawEnv.value.frame2.psm = sRender.reso.row.frame_psm;
    DrawEnv.value.frame2.fbmsk = 0;
    DrawEnv.value.frame2_address = SCE_GS_FRAME_2;
    DrawEnv.value.zbuf2.zbp = sRender.vram.row.depth_base;
    DrawEnv.value.zbuf2.psm = sRender.reso.row.depth_psm;
    DrawEnv.value.zbuf2.zmsk = 0;
    DrawEnv.value.zbuf2_address = SCE_GS_ZBUF_2;
    DrawEnv.value.xyoffset2.ofx = (GS_COORD_CENTER - (sRender.reso.row.frame_width >> 1)) << 4;
    DrawEnv.value.xyoffset2.ofy = (GS_COORD_CENTER - (sRender.reso.row.frame_height >> 1)) << 4;
    DrawEnv.value.xyoffset2_address = SCE_GS_XYOFFSET_2;
    DrawEnv.value.scissor2.scax0 = 0;
    DrawEnv.value.scissor2.scax1 = sRender.reso.row.frame_width - 1;
    DrawEnv.value.scissor2.scay0 = 0;
    DrawEnv.value.scissor2.scay1 = sRender.reso.row.frame_height - 1;
    DrawEnv.value.scissor2_address = SCE_GS_SCISSOR_2;
    DrawEnv.value.test2.ate = 0;
    DrawEnv.value.test2.atst = 0;
    DrawEnv.value.test2.aref = 0;
    DrawEnv.value.test2.afail = 0;
    DrawEnv.value.test2.date = 0;
    DrawEnv.value.test2.datm = 0;
    DrawEnv.value.test2.zte = 1;
    DrawEnv.value.test2.ztst = GS_ZTST_GEQUAL;
    DrawEnv.value.test2_address = SCE_GS_TEST_2;
    /* Remaining state: primitive mode, texture alpha, colour clamp, dither and dimming matrix. */
    DrawEnv.value.prmodecont.ac = 1;
    DrawEnv.value.prmodecont_address = SCE_GS_PRMODECONT;
    DrawEnv.value.texa.ta0 = GS_ALPHA_OPAQUE;
    DrawEnv.value.texa.aem = 0;
    DrawEnv.value.texa.ta1 = GS_ALPHA_OPAQUE;
    DrawEnv.value.texa_address = SCE_GS_TEXA;
    DrawEnv.value.colclamp.clamp = 1;
    DrawEnv.value.colclamp_address = SCE_GS_COLCLAMP;
    DrawEnv.value.dthe.dthe = 0;
    DrawEnv.value.dthe_address = SCE_GS_DTHE;
    DrawEnv.value.dimx.dm00 = 4;
    DrawEnv.value.dimx.dm01 = 2;
    DrawEnv.value.dimx.dm02 = 5;
    DrawEnv.value.dimx.dm03 = 3;
    DrawEnv.value.dimx.dm10 = 0;
    DrawEnv.value.dimx.dm11 = 6;
    DrawEnv.value.dimx.dm12 = 1;
    DrawEnv.value.dimx.dm13 = 7;
    DrawEnv.value.dimx.dm20 = 5;
    DrawEnv.value.dimx.dm21 = 3;
    DrawEnv.value.dimx.dm22 = 4;
    DrawEnv.value.dimx.dm23 = 2;
    DrawEnv.value.dimx.dm30 = 1;
    DrawEnv.value.dimx.dm31 = 7;
    DrawEnv.value.dimx.dm32 = 0;
    DrawEnv.value.dimx.dm33 = 6;
    DrawEnv.value.dimx_address = SCE_GS_DIMX;
}

/* Updates the frame and z-buffer bases in DrawEnv from the current VRAM row and sends it. */
void xglRenderDrawEnvMove(void)
{
    DrawEnv.value.frame1.fbp = sRender.vram.row.height;
    DrawEnv.value.frame2.fbp = sRender.vram.row.height;
    DrawEnv.value.zbuf1.zbp = sRender.vram.row.depth_base;
    DrawEnv.value.zbuf2.zbp = sRender.vram.row.depth_base;
    SyncDCache(&DrawEnv.value, &DrawEnv.value + 1);
    sceGsPutDrawEnv(&DrawEnv.value);
}

/* Sets the default display environment for the current resolution, with the second display copied from the first. */
static void xglRenderDispEnvInit(void)
{
    sceGsSetDefDispEnv(&DispEnv, sRender.reso.row.frame_psm, sRender.reso.row.frame_width, sRender.reso.row.frame_height, 0, 0);
    DispEnv.pmode.en2 = 0;
    DispEnv.pmode.amod = 0;
    DispEnv.dispfb2 = DispEnv.dispfb1;
    DispEnv.display2 = DispEnv.display1;
    DispEnv.extbuf = 0;
    DispEnv.extdata = 0;
    DispEnv.extwrite = 0;
}

/*
 * Updates the display environment for the scan-line interpolation mode, sends it, and
 * writes the second display and extended registers directly.
 */
void xglRenderDispEnvMove(void)
{
    DispEnv.dispfb1.field.fbp = sRender.vram.row.width;
    switch (ScanLineInterpolate) {
    case SCANLINE_INTERP_OFF:
        DispEnv.pmode.en1 = 0;
        break;
    case SCANLINE_INTERP_CIRCUIT_BLEND:
        DispEnv.dispfb2.field.fbp = sRender.vram.row.width;
        DispEnv.pmode.en1 = DispEnv.pmode.en2;
        DispEnv.pmode.alp = RENDER_CIRCUIT_BLEND_ALPHA;
        DispEnv.display2.field.dx = DispEnv.display1.field.dx;
        DispEnv.display2.field.dy = DispEnv.display1.field.dy - 1;
        break;
    case SCANLINE_INTERP_PACKET:
        /* the field interpolation of mode 2 runs in xglRenderMove */
        break;
    }
    SyncDCache(&DispEnv, &DispEnv + 1);
    sceGsPutDispEnv(&DispEnv);
    *GS_DISPFB2 = DispEnv.dispfb2.value;
    *GS_DISPLAY2 = DispEnv.display2.value;
    *GS_EXTBUF = DispEnv.extbuf;
    *GS_EXTDATA = DispEnv.extdata;
    *GS_EXTWRITE = DispEnv.extwrite;
    *GS_CSR = GS_CSR_EDWINT;
}

void xglRenderClearDepth(void)
{
    ClearEnv.value.target = GS_TEST_CLEAR_DEPTH;
}

void xglRenderClearFrame(void)
{
    ClearEnv.value.target = GS_TEST_CLEAR_FRAME;
}

void xglRenderClearColor(u32 color)
{
    ClearEnv.value.color_r = color & 0xff;
    ClearEnv.value.color_g = (color >> 8) & 0xff;
    ClearEnv.value.color_b = (color >> 16) & 0xff;
    ClearEnv.value.color_a = (color >> 24) & 0xff;
}

/*
 * Initializes ClearEnv: the GIF tag, the frame clear target, the default black colour and
 * the clear rectangle around the frame.
 */
static void xglRenderClearEnvInit(void)
{
    int half_width;
    int half_height;

    ClearEnv.value.gif_tag_high = (5 << GIF_TAG_NREG_SHIFT) | (GS_PRIM_SPRITE << GIF_TAG_PRIM_SHIFT) | GIF_TAG_PRE;
    ClearEnv.value.gif_registers_low = RENDER_CLEAR_GIF_REGS | (GIF_REG_AD << 16);
    ClearEnv.value.gif_tag_low = GIF_TAG_EOP | 1;
    ClearEnv.value.gif_registers_high = 0;
    xglRenderClearFrame();
    ClearEnv.value.target_address = SCE_GS_TEST_1;
    xglRenderClearColor(RENDER_CLEAR_COLOR_DEFAULT);
    half_width = sRender.reso.row.frame_width >> 1;
    half_height = sRender.reso.row.frame_height >> 1;
    ClearEnv.value.left = (GS_COORD_CENTER - half_width) << 4;
    ClearEnv.value.top = (GS_COORD_CENTER - half_height) << 4;
    ClearEnv.value.right = (GS_COORD_CENTER + half_width) << 4;
    ClearEnv.value.bottom = (GS_COORD_CENTER + half_height) << 4;
    ClearEnv.value.restore_target = GS_TEST_ZTE_GEQUAL;
    ClearEnv.value.restore_target_address = SCE_GS_TEST_1;
}

void xglRenderClearEnvMove(void)
{
    SyncDCache(&ClearEnv.value, &ClearEnv.value + 1);
    xglDmaDirectNormal(XGL_DMA_CHANNEL_GIF, (u32) &ClearEnv.value, RENDER_CLEAR_QWORDS);
}

/* Selects a resolution row, rebuilds the draw, display and clear environments and reinitializes the studio cameras. */
void xglRenderSetReso(int resolution)
{
    sRender.reso.row = asDispReso[resolution];
    sRender.vram.row = asVramBase[resolution];
    sRender.display_buffer_base = sRender.vram.row.width;
    sRender.draw_buffer_base = sRender.vram.row.height;
    xglRenderDrawEnvInit();
    xglRenderDispEnvInit();
    xglRenderClearEnvInit();
    xglStudioInit();
    xglStudioMainCameraInit();
}

/* Swaps the display and draw buffer bases and resets the per-frame render state. */
void xglRenderSwapBase(void)
{
    u32 accumulated_frames;
    u16 next_display_base;
    u16 next_draw_base;
    u16 previous_width;

    accumulated_frames = sRender.frame_count + sRender.frame_delta;
    next_draw_base = sRender.draw_buffer_base;
    next_display_base = sRender.display_buffer_base;
    previous_width = sRender.vram.row.width;

    sRender.vram.row.width = next_draw_base;
    sRender.vram.row.height = previous_width;
    sRender.display_buffer_base = next_draw_base;
    sRender.draw_buffer_base = next_display_base;
    sRender.render_status = 0;
    sRender.frame_delta = 0;
    sRender.frame_count = accumulated_frames;
    sRender.frame_status = 0;
}

void xglRenderDispOff(void)
{
    DispEnv.flags &= ~(u64) GS_PMODE_EN2;
}

void xglRenderDispOn(void)
{
    DispEnv.flags |= GS_PMODE_EN2;
}

/* Resets the GS and VIF, sets the 512-wide resolution with circuit-blend interpolation and clears the render state. */
void xglRenderInit(void)
{
    int i;
    /* Each channel has its own observable VIF ERR register. */
    volatile u32 *vif0_error;
    volatile u32 *vif1_error;

    sceGsResetPath();
    /* Read-modify-write the 32-bit VIF ERR MMIO registers to mask bit 0.
     * The two channels are independent hardware locations. */
    vif0_error = (volatile u32 *)VIF0_ERR_ADDRESS;
    *vif0_error |= VIF_ERR_MASK_ERROR_INTERRUPT;
    vif1_error = (volatile u32 *)VIF1_ERR_ADDRESS;
    *vif1_error |= VIF_ERR_MASK_ERROR_INTERRUPT;
    sceGsResetGraph(0, 1, 2, 0);
    xglRenderSyncInit();
    xglRenderSetReso(RENDER_RESO_512);
    FrameCount = 0;
    ScanLineInterpolate = SCANLINE_INTERP_CIRCUIT_BLEND;
    LoopCount = 0;
    sRender.render_status = 0;
    sRender.frame_count = 0;
    sRender.frame_delta = 0;
    sRender.frame_status = 0;
    sRender.scene_disabled = 0;
    for (i = RENDER_FINAL_CALLBACK_COUNT - 1; i >= 0; i--) {
        sRender.final_packet_callback[i] = 0;
    }
    xglRenderGlobalFadeInit();
    xglRenderClearOff();
    nmlModelConstruct();
}

/* Runs the final-packet callbacks on the current packet unless the back buffer is requested. */
static void xglRenderFinalPacket(void)
{
    XglPacket *packet;
    XglRenderFadeCallback function;
    int i;

    packet = xglPacketGetCurrent();
    if (nmlModelIsBackBufferRequest() == 0) {
        for (i = 0; i < RENDER_FINAL_CALLBACK_COUNT; i++) {
            function = sRender.final_packet_callback[i];
            if (function != 0) {
                function(packet);
            }
        }
    }
}

/* Enables the global fade and clears its colour and fade callback. */
void xglRenderGlobalFadeInit(void)
{
    s_nGblFadeInit = 1;
    sRender.fade_callback = 0;
    s_nGblFade = 0;
}

/* Sets the global fade colour from red, green, blue and alpha values, clamped to 0..1 and packed as GS bytes. */
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
    s_nGblFade = (((int) (red * RENDER_FADE_COLOR_SCALE)) & 0xFF) + (((int) (alpha * RENDER_FADE_ALPHA_SCALE)) << 0x18);
    s_nGblFade = s_nGblFade + (((((int) (blue * RENDER_FADE_COLOR_SCALE)) & 0xFF) << 0x10) + ((((int) (green * RENDER_FADE_COLOR_SCALE)) & 0xFF) << 8));
}

/*
 * Calls the fade callback, then draws the global fade colour over the frame when its alpha
 * is nonzero, as a VIF1 direct packet.
 */
static void xglRenderGlobalFade(void)
{
    unsigned char alpha;
    XglPacket *packet;
    u64 *header;
    u64 *alpha_entry;
    u64 *test_entry;
    u64 *color_entry;
    u64 *prim_entry;
    u64 *vertex0_entry;
    u64 *vertex1_entry;
    u64 *restore_entry;

    alpha = (unsigned int)s_nGblFade >> 24;
    packet = xglPacketGetCurrent();
    if (sRender.fade_callback != 0) {
        sRender.fade_callback(packet);
    }
    if (alpha != 0 && s_nGblFadeInit != 0) {
        header = (u64 *)SCRATCHPAD_BASE;
        header[1] = GIF_REG_AD;
        alpha_entry = (u64 *)(SCRATCHPAD_BASE + 0x10);
        alpha_entry[0] = ((u64)alpha << 32) | GS_ALPHA_FADE_BLEND;
        alpha_entry[1] = SCE_GS_ALPHA_1;
        test_entry = (u64 *)(SCRATCHPAD_BASE + 0x20);
        test_entry[0] = GS_TEST_ZTE_ALWAYS;
        test_entry[1] = SCE_GS_TEST_1;
        color_entry = (u64 *)(SCRATCHPAD_BASE + 0x30);
        color_entry[0] = ((u64)(s_nGblFade & 0xFF)) | ((u64)(((unsigned int)s_nGblFade >> 8) & 0xFF) << 8) | ((u64)(((unsigned int)s_nGblFade >> 16) & 0xFF) << 16) | ((u64)0x3F800000 << 32);
        color_entry[1] = SCE_GS_RGBAQ;
        prim_entry = (u64 *)(SCRATCHPAD_BASE + 0x40);
        prim_entry[0] = GS_PRIM_SPRITE_ABE;
        prim_entry[1] = SCE_GS_PRIM;
        vertex0_entry = (u64 *)(SCRATCHPAD_BASE + 0x50);
        vertex0_entry[0] = 0x72007000;
        vertex0_entry[1] = SCE_GS_XYZF2;
        vertex1_entry = (u64 *)(SCRATCHPAD_BASE + 0x60);
        vertex1_entry[0] = 0x8E009000;
        vertex1_entry[1] = SCE_GS_XYZF2;
        restore_entry = (u64 *)(SCRATCHPAD_BASE + 0x70);
        restore_entry[0] = GS_TEST_ZTE_GEQUAL;
        restore_entry[1] = SCE_GS_TEST_1;
        header[0] = ((u64)GIF_TAG_HIGH_NREG_1 << 32) | (GIF_TAG_EOP | 7);
        sceVif1PkCnt(packet, 0);
        sceVif1PkOpenDirectHLCode(packet, 0);
        sceVif1PkAddDirectDataN(packet, header, RENDER_FADE_QWORDS);
        sceVif1PkCloseDirectHLCode(packet);
    }
}

void xglRenderClearOn(void)
{
    s_nClearFrame = RENDER_CLEAR_ON;
}

void xglRenderClearOff(void)
{
    s_nClearFrame = RENDER_CLEAR_OFF;
}

/* Copies the ClearEnv colour into the clear test packet and sends it through VIF1. */
static void xglRenderClear(void)
{
    TestEnv_0_004A8E80.value.color.red_green.words.low = ClearEnv.value.color_r;
    TestEnv_0_004A8E80.value.color.red_green.words.high = ClearEnv.value.color_g;
    TestEnv_0_004A8E80.value.color.blue = ClearEnv.value.color_b;
    TestEnv_0_004A8E80.value.color.alpha = GS_ALPHA_OPAQUE;
    sceVif1PkRef(xglPacketGetCurrent(), &TestEnv_0_004A8E80.value, 6, 0, 0, 0);
}

/*
 * Per-frame render step: clear, 3D effects, models, final packets, fonts and global fade,
 * then packet interpolation, packet move, vsync sync and the GS path reset. Scene passes
 * are skipped while scene_disabled is set.
 */
static void xglRenderMove(void)
{
    if (s_nClearFrame == RENDER_CLEAR_DRAW && sRender.scene_disabled == 0) {
        xglRenderClear();
    } else if (s_nClearFrame >= RENDER_CLEAR_ON) {
        s_nClearFrame--;
    }
    if (sRender.scene_disabled == 0) {
        sefDrawEffect3D();
    }
    if (sRender.scene_disabled == 0) {
        nmlModelFlush();
    } else {
        nmlModelFlushClear();
    }
    if (sRender.scene_disabled == 0) {
        xglRenderFinalPacket();
    }
    xglFontFlush();
    if (sRender.scene_disabled == 0) {
        xglRenderGlobalFade();
    }
    if (ScanLineInterpolate == SCANLINE_INTERP_PACKET && sRender.scene_disabled == 0) {
        xglPacketInterpolate();
    }
    xglPacketMove();
    xglRenderSyncMove();
    FrameCount++;
    LoopCount++;
    sceGsSyncPath(0, 0);
    sceGsResetGraph(1, 0, 0, 0);
    xglRenderSwapBase();
    xglRenderDrawEnvMove();
    xglRenderDispEnvMove();
    xglRenderClearEnvMove();
    xglPacketSend();
    nmlModelSendPacketChangeSignal();
}

/* Render thread entry: initializes the renderer, then runs the frame loop forever. */
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
