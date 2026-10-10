#include "common.h"

#include "shared.h"

#include "main/xgl_thread.h"

#include "main/xgl_packet.h"

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
 * GS display-environment block (0x004A9000, size 0x50). xglRenderDispOff/On
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
 * GS draw-environment block (0x004A8F00, size 0x100): a GIF A+D packet of
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

extern GsDrawEnv DrawEnv;

extern GsClearEnv ClearEnv;
extern GsDispEnv DispEnv;

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

extern XglRenderState sRender;

/*
 * One GIF packet quadword on the scratchpad. The halfword view makes the
 * packet stores and the sRender halfword reads share an alias set, so the
 * stores stay ahead of the reads.
 */
typedef union ScratchpadQuad {
    u64 dword[2];
    u16 half[8];
} ScratchpadQuad;

extern int sceGsSyncV(int mode);

extern int sceGsResetGraph(int mode, int interlace, int output, int ffmode);

extern void sceGsResetPath(void);

extern void xglRenderSetReso(int index);

extern void xglRenderGlobalFadeInit(void);

extern void xglRenderClearOff(void);

extern void nmlModelConstruct(void);

extern int nmlModelIsBackBufferRequest(void);

extern void sceVif1PkCnt(XglPacket *packet, int count);

extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);

extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);

extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);

extern void FlushCache(int mode);

extern void xglDmaDirectNormal(u32 channel, u32 address, u32 count);

/* VIF error registers used by the original renderer initialization. */

#define VIF0_ERR_ADDRESS 0x10003820

#define VIF1_ERR_ADDRESS 0x10003C20

static int s_nGblFadeInit = 0;

static int s_nGblFade = 0;

static int s_nClearFrame = 0;

int FrameCount = 0;

int LoopCount = 0;

/* The GS VSync interrupt callback increments this counter while
 * xglRenderSyncMove polls it on the render thread. */
volatile int VSyncCount = 0;

int ScanLineInterpolate = 0;

GsDrawEnv DrawEnv = { 0 };

extern void xglRenderInit(void);

static void xglRenderMove(void);

extern void xglPadRead(void);

extern void xglSendSePacket(void);

extern void SyncDCache(void *start, void *end);

extern int xglRenderVSyncCallback(void);

extern void *sceGsSyncVCallback(int (*func)(void));

extern signed char FLAG_FRAME_60;

/*
 * The packed global fade color that xglRenderGlobalFade (still asm, this TU)
 * draws over the scene: red/green/blue scaled to a byte (0-255) at
 * +0x00/+0x08/+0x10 and alpha scaled to a byte (0-128, the PS2 GS
 * convention) at +0x18, the same GS_RGBAQ-style packing xglRenderClearColor
 * above unpacks for ClearEnv's own color fields. Its storage belongs to
 * xglRenderGlobalFadeInit (still asm, this TU).
 */

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

extern int sceGsSyncPath(int mode, int timeout);

extern void xglStudioInit(void);

extern void xglStudioMainCameraInit(void);

extern void nmlModelFlush(void);

extern void nmlModelFlushClear(void);

extern int nmlModelSendPacketChangeSignal(void);

extern void sefDrawEffect3D(void);

extern void xglFontFlush(void);

extern void xglPacketInterpolate(void);

extern void xglPacketMove(void);

extern void xglPacketSend(void);

extern void sceGsSetDefDispEnv(GsDispEnv *env, int psm, int width, int height, int dx, int dy);

extern void sceGsPutDrawEnv(GsDrawEnv *env);

extern void sceGsPutDispEnv(GsDispEnv *env);

#define GS_DISPFB2 ((volatile u64 *)0x12000070)

#define GS_DISPLAY2 ((volatile u64 *)0x12000080)

#define GS_EXTBUF ((volatile u64 *)0x120000B0)

#define GS_EXTDATA ((volatile u64 *)0x120000C0)

#define GS_EXTWRITE ((volatile u64 *)0x120000D0)

#define GS_CSR ((volatile u64 *)0x12001000)

static XglRenderReso asDispReso[6] = {
    { 0x31, 0, 640, 448 },
    { 0x31, 0, 512, 448 },
    { 0x31, 0, 448, 448 },
    { 0x31, 0, 384, 448 },
    { 0x31, 0, 320, 448 },
    { 0x31, 0, 256, 448 },
};

static XglRenderVram asVramBase[6] = {
    { 0x0000, { 0x008C, 0x0118, 0x01A4 }, 0x008C, 0x0118, 0x01A4, { 0x0230, 0x02BC, 0x01A4, 0x01C4, 0x01E4 } },
    { 0x0000, { 0x0070, 0x00E0, 0x0150 }, 0x0070, 0x00E0, 0x0150, { 0x01C0, 0x0230, 0x01C0, 0x01E0, 0x0200 } },
    { 0x0000, { 0x0062, 0x00C4, 0x0126 }, 0x0062, 0x00C4, 0x0126, { 0x0188, 0x01EA, 0x0188, 0x01A8, 0x01C8 } },
    { 0x0000, { 0x0054, 0x00A8, 0x00FC }, 0x0054, 0x00A8, 0x00FC, { 0x0150, 0x01A4, 0x01A4, 0x01C4, 0x01E4 } },
    { 0x0000, { 0x0046, 0x008C, 0x00D2 }, 0x0046, 0x008C, 0x00D2, { 0x0118, 0x015E, 0x01A4, 0x01C4, 0x01E4 } },
    { 0x0000, { 0x0038, 0x0070, 0x00A8 }, 0x0038, 0x0070, 0x00A8, { 0x00E0, 0x0118, 0x0150, 0x0170, 0x0190 } },
};

/*
 * The packed global fade color that xglRenderGlobalFade (still asm, this TU)
 * draws over the scene: red/green/blue scaled to a byte (0-255) at
 * +0x00/+0x08/+0x10 and alpha scaled to a byte (0-128, the PS2 GS
 * convention) at +0x18, the same GS_RGBAQ-style packing xglRenderClearColor
 * above unpacks for ClearEnv's own color fields. Its storage belongs to
 * xglRenderGlobalFadeInit (still asm, this TU).
 */

/* VIF DIRECT sends one GIFtag, one A+D PRIM, one RGBAQ and two XYZ2 values. */

/* VIF error registers used by the original renderer initialization. */




/* VIF DIRECT sends one GIFtag, one A+D PRIM, one RGBAQ and two XYZ2 values. */

void xglRenderDrawFlip(void)
{
    u64 *scratch;
    u16 previous_base;
    u64 frame;

    scratch = (u64 *)0x70000000;
    previous_base = sRender.display_buffer_base;
    sRender.display_buffer_base = sRender.vram.row.flip_base;
    frame = sRender.display_buffer_base | ((u64)(sRender.reso.row.frame_width / 64) << 16) |
            ((u64)sRender.reso.row.frame_psm << 24);
    scratch[0] = ((u64)0x10000000 << 32) | 0x8002;
    scratch[1] = 14;
    scratch[3] = 76;
    scratch[2] = frame;
    scratch[4] = frame;
    scratch[5] = 77;
    sRender.vram.row.flip_base = previous_base;
    __asm__ __volatile__("sync" : : : "memory");
    FlushCache(0);
    xglDmaDirectNormal(2, 0x70000000, 3);
}

void xglRenderCopyDisp2Draw(void)
{
    ScratchpadQuad *header;
    ScratchpadQuad *texflush;
    ScratchpadQuad *bitbltbuf;
    ScratchpadQuad *trxpos;
    ScratchpadQuad *trxreg;
    ScratchpadQuad *trxdir;

    header = (ScratchpadQuad *)0x70000000;
    texflush = (ScratchpadQuad *)0x70000010;
    bitbltbuf = (ScratchpadQuad *)0x70000020;
    trxpos = (ScratchpadQuad *)0x70000030;
    trxreg = (ScratchpadQuad *)0x70000040;
    trxdir = (ScratchpadQuad *)0x70000050;
    header->dword[1] = 14;
    texflush->dword[0] = 0;
    texflush->dword[1] = 63;
    bitbltbuf->dword[0] = (u64)(sRender.display_buffer_base << 5) | ((u64)8 << 16) | ((u64)sRender.reso.row.frame_psm << 24) | ((u64)(sRender.draw_buffer_base << 5) << 32) | ((u64)8 << 48) | ((u64)sRender.reso.row.frame_psm << 56);
    bitbltbuf->dword[1] = 80;
    trxpos->dword[0] = 0;
    trxpos->dword[1] = 81;
    trxreg->dword[0] = ((u64)448 << 32) | 0x200;
    trxreg->dword[1] = 82;
    trxdir->dword[0] = 2;
    trxdir->dword[1] = 83;
    header->dword[0] = ((u64)0x10000000 << 32) | 0x8005;
    __asm__ __volatile__("sync" : : : "memory");
    FlushCache(0);
    xglDmaDirectNormal(2, 0x70000000, 6);
}

void xglRenderDrawFlipPk(XglPacket *packet)
{
    ScratchpadQuad *header;
    ScratchpadQuad *texflush;
    ScratchpadQuad *bitbltbuf;
    ScratchpadQuad *trxpos;
    ScratchpadQuad *trxreg;
    ScratchpadQuad *trxdir;

    header = (ScratchpadQuad *)0x70000000;
    texflush = (ScratchpadQuad *)0x70000010;
    bitbltbuf = (ScratchpadQuad *)0x70000020;
    trxpos = (ScratchpadQuad *)0x70000030;
    trxreg = (ScratchpadQuad *)0x70000040;
    trxdir = (ScratchpadQuad *)0x70000050;
    header->dword[1] = 14;
    texflush->dword[0] = 0;
    texflush->dword[1] = 63;
    bitbltbuf->dword[0] = (u64)(sRender.draw_buffer_base << 5) | ((u64)8 << 16) | ((u64)sRender.reso.row.frame_psm << 24) | ((u64)(sRender.vram.row.flip_base << 5) << 32) | ((u64)8 << 48) | ((u64)sRender.reso.row.frame_psm << 56);
    bitbltbuf->dword[1] = 80;
    trxpos->dword[0] = 0;
    trxpos->dword[1] = 81;
    trxreg->dword[0] = ((u64)448 << 32) | 0x200;
    trxreg->dword[1] = 82;
    trxdir->dword[0] = 2;
    trxdir->dword[1] = 83;
    header->dword[0] = ((u64)0x10000000 << 32) | 0x8005;
    __asm__ __volatile__("sync" : : : "memory");
    sceVif1PkCnt(packet, 0);
    sceVif1PkOpenDirectHLCode(packet, 0);
    sceVif1PkAddDirectDataN(packet, (void *)0x70000000, 6);
    sceVif1PkCloseDirectHLCode(packet);
}

int xglRenderVSyncCallback(void)
{
    /* GIF and VIF1 hardware status gate the frame callback update. */
    volatile u32 *const gif_status = (volatile u32 *)0x10003020;
    volatile u32 *const vif1_status = (volatile u32 *)0x10003C00;

    VSyncCount++;
    if (VSyncCount == 2) {
        if ((*gif_status & 0xC00) != 0 || (*vif1_status & 3) != 0) {
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

static void xglRenderDrawEnvInit(void)
{
    DrawEnv.gif_tag.nloop = 15;
    DrawEnv.gif_tag.eop = 1;
    DrawEnv.gif_tag.pre = 0;
    DrawEnv.gif_tag.prim = 0;
    DrawEnv.gif_tag.flg = 0;
    DrawEnv.gif_tag.nreg = 1;
    DrawEnv.gif_registers.regs0 = 14;
    DrawEnv.frame1.fbp = sRender.vram.row.height;
    DrawEnv.frame1.fbw = sRender.reso.row.frame_width / 64;
    DrawEnv.frame1.psm = sRender.reso.row.frame_psm;
    DrawEnv.frame1.fbmsk = 0;
    DrawEnv.frame1_address = 76;
    DrawEnv.zbuf1.zbp = sRender.vram.row.depth_base;
    DrawEnv.zbuf1.psm = sRender.reso.row.depth_psm;
    DrawEnv.zbuf1.zmsk = 0;
    DrawEnv.zbuf1_address = 78;
    DrawEnv.xyoffset1.ofx = (2048 - (sRender.reso.row.frame_width >> 1)) << 4;
    DrawEnv.xyoffset1.ofy = (2048 - (sRender.reso.row.frame_height >> 1)) << 4;
    DrawEnv.xyoffset1_address = 24;
    DrawEnv.scissor1.scax0 = 0;
    DrawEnv.scissor1.scax1 = sRender.reso.row.frame_width - 1;
    DrawEnv.scissor1.scay0 = 0;
    DrawEnv.scissor1.scay1 = sRender.reso.row.frame_height - 1;
    DrawEnv.scissor1_address = 64;
    DrawEnv.test1.ate = 0;
    DrawEnv.test1.atst = 0;
    DrawEnv.test1.aref = 0;
    DrawEnv.test1.afail = 0;
    DrawEnv.test1.date = 0;
    DrawEnv.test1.datm = 0;
    DrawEnv.test1.zte = 1;
    DrawEnv.test1.ztst = 2;
    DrawEnv.test1_address = 71;
    DrawEnv.frame2.fbp = sRender.vram.row.height;
    DrawEnv.frame2.fbw = sRender.reso.row.frame_width / 64;
    DrawEnv.frame2.psm = sRender.reso.row.frame_psm;
    DrawEnv.frame2.fbmsk = 0;
    DrawEnv.frame2_address = 77;
    DrawEnv.zbuf2.zbp = sRender.vram.row.depth_base;
    DrawEnv.zbuf2.psm = sRender.reso.row.depth_psm;
    DrawEnv.zbuf2.zmsk = 0;
    DrawEnv.zbuf2_address = 79;
    DrawEnv.xyoffset2.ofx = (2048 - (sRender.reso.row.frame_width >> 1)) << 4;
    DrawEnv.xyoffset2.ofy = (2048 - (sRender.reso.row.frame_height >> 1)) << 4;
    DrawEnv.xyoffset2_address = 25;
    DrawEnv.scissor2.scax0 = 0;
    DrawEnv.scissor2.scax1 = sRender.reso.row.frame_width - 1;
    DrawEnv.scissor2.scay0 = 0;
    DrawEnv.scissor2.scay1 = sRender.reso.row.frame_height - 1;
    DrawEnv.scissor2_address = 65;
    DrawEnv.test2.ate = 0;
    DrawEnv.test2.atst = 0;
    DrawEnv.test2.aref = 0;
    DrawEnv.test2.afail = 0;
    DrawEnv.test2.date = 0;
    DrawEnv.test2.datm = 0;
    DrawEnv.test2.zte = 1;
    DrawEnv.test2.ztst = 2;
    DrawEnv.test2_address = 72;
    DrawEnv.prmodecont.ac = 1;
    DrawEnv.prmodecont_address = 26;
    DrawEnv.texa.ta0 = 0x80;
    DrawEnv.texa.aem = 0;
    DrawEnv.texa.ta1 = 0x80;
    DrawEnv.texa_address = 59;
    DrawEnv.colclamp.clamp = 1;
    DrawEnv.colclamp_address = 70;
    DrawEnv.dthe.dthe = 0;
    DrawEnv.dthe_address = 69;
    DrawEnv.dimx.dm00 = 4;
    DrawEnv.dimx.dm01 = 2;
    DrawEnv.dimx.dm02 = 5;
    DrawEnv.dimx.dm03 = 3;
    DrawEnv.dimx.dm10 = 0;
    DrawEnv.dimx.dm11 = 6;
    DrawEnv.dimx.dm12 = 1;
    DrawEnv.dimx.dm13 = 7;
    DrawEnv.dimx.dm20 = 5;
    DrawEnv.dimx.dm21 = 3;
    DrawEnv.dimx.dm22 = 4;
    DrawEnv.dimx.dm23 = 2;
    DrawEnv.dimx.dm30 = 1;
    DrawEnv.dimx.dm31 = 7;
    DrawEnv.dimx.dm32 = 0;
    DrawEnv.dimx.dm33 = 6;
    DrawEnv.dimx_address = 68;
}

void xglRenderDrawEnvMove(void)
{
    DrawEnv.frame1.fbp = sRender.vram.row.height;
    DrawEnv.frame2.fbp = sRender.vram.row.height;
    DrawEnv.zbuf1.zbp = sRender.vram.row.depth_base;
    DrawEnv.zbuf2.zbp = sRender.vram.row.depth_base;
    SyncDCache(&DrawEnv, &DrawEnv + 1);
    sceGsPutDrawEnv(&DrawEnv);
}

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

void xglRenderDispEnvMove(void)
{
    DispEnv.dispfb1.field.fbp = sRender.vram.row.width;
    switch (ScanLineInterpolate) {
    case 0:
        DispEnv.pmode.en1 = 0;
        break;
    case 1:
        DispEnv.dispfb2.field.fbp = sRender.vram.row.width;
        DispEnv.pmode.en1 = DispEnv.pmode.en2;
        DispEnv.pmode.alp = 0x80;
        DispEnv.display2.field.dx = DispEnv.display1.field.dx;
        DispEnv.display2.field.dy = DispEnv.display1.field.dy - 1;
        break;
    case 2:
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
    *GS_CSR = 16;
}

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

static void xglRenderClearEnvInit(void)
{
    int half_width;
    int half_height;

    ClearEnv.gif_tag_high = 0x50034000;
    ClearEnv.gif_registers_low = 0xE551E;
    ClearEnv.gif_tag_low = 0x8001;
    ClearEnv.gif_registers_high = 0;
    xglRenderClearFrame();
    ClearEnv.target_address = 71;
    xglRenderClearColor(0x80000000);
    half_width = sRender.reso.row.frame_width >> 1;
    half_height = sRender.reso.row.frame_height >> 1;
    ClearEnv.left = (2048 - half_width) << 4;
    ClearEnv.top = (2048 - half_height) << 4;
    ClearEnv.right = (2048 + half_width) << 4;
    ClearEnv.bottom = (2048 + half_height) << 4;
    ClearEnv.restore_target = 0x50000;
    ClearEnv.restore_target_address = 71;
}

void xglRenderClearEnvMove(void)
{
    SyncDCache(&ClearEnv, (u8 *) &ClearEnv + sizeof(ClearEnv));
    xglDmaDirectNormal(2, (u32) &ClearEnv, 6);
}

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
    DispEnv.flags &= ~(u64) 2;
}

void xglRenderDispOn(void)
{
    DispEnv.flags |= 2;
}

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
    *vif0_error |= 1;
    vif1_error = (volatile u32 *)VIF1_ERR_ADDRESS;
    *vif1_error |= 1;
    sceGsResetGraph(0, 1, 2, 0);
    xglRenderSyncInit();
    xglRenderSetReso(1);
    FrameCount = 0;
    ScanLineInterpolate = 1;
    LoopCount = 0;
    sRender.render_status = 0;
    sRender.frame_count = 0;
    sRender.frame_delta = 0;
    sRender.frame_status = 0;
    sRender.scene_disabled = 0;
    for (i = 7; i >= 0; i--) {
        sRender.final_packet_callback[i] = 0;
    }
    xglRenderGlobalFadeInit();
    xglRenderClearOff();
    nmlModelConstruct();
}

static void xglRenderFinalPacket(void)
{
    XglPacket *packet;
    XglRenderFadeCallback function;
    int i;

    packet = xglPacketGetCurrent();
    if (nmlModelIsBackBufferRequest() == 0) {
        for (i = 0; i < 8; i++) {
            function = sRender.final_packet_callback[i];
            if (function != 0) {
                function(packet);
            }
        }
    }
}

void xglRenderGlobalFadeInit(void)
{
    s_nGblFadeInit = 1;
    sRender.fade_callback = 0;
    s_nGblFade = 0;
}

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
        header = (u64 *)0x70000000;
        header[1] = 14;
        alpha_entry = (u64 *)0x70000010;
        alpha_entry[0] = ((u64)alpha << 32) | 100;
        alpha_entry[1] = 66;
        test_entry = (u64 *)0x70000020;
        test_entry[0] = 0x30000;
        test_entry[1] = 71;
        color_entry = (u64 *)0x70000030;
        color_entry[0] = ((u64)(s_nGblFade & 0xFF)) | ((u64)(((unsigned int)s_nGblFade >> 8) & 0xFF) << 8) | ((u64)(((unsigned int)s_nGblFade >> 16) & 0xFF) << 16) | ((u64)0x3F800000 << 32);
        color_entry[1] = 1;
        prim_entry = (u64 *)0x70000040;
        prim_entry[0] = 70;
        prim_entry[1] = 0;
        vertex0_entry = (u64 *)0x70000050;
        vertex0_entry[0] = 0x72007000;
        vertex0_entry[1] = 4;
        vertex1_entry = (u64 *)0x70000060;
        vertex1_entry[0] = 0x8E009000;
        vertex1_entry[1] = 4;
        restore_entry = (u64 *)0x70000070;
        restore_entry[0] = 0x50000;
        restore_entry[1] = 71;
        header[0] = ((u64)0x10000000 << 32) | 0x8007;
        sceVif1PkCnt(packet, 0);
        sceVif1PkOpenDirectHLCode(packet, 0);
        sceVif1PkAddDirectDataN(packet, header, 8);
        sceVif1PkCloseDirectHLCode(packet);
    }
}

void xglRenderClearOn(void)
{
    s_nClearFrame = 2;
}

void xglRenderClearOff(void)
{
    s_nClearFrame = 0;
}

static void xglRenderClear(void)
{
    TestEnv_0_004A8E80.color.red_green.words.low = ClearEnv.color_r;
    TestEnv_0_004A8E80.color.red_green.words.high = ClearEnv.color_g;
    TestEnv_0_004A8E80.color.blue = ClearEnv.color_b;
    TestEnv_0_004A8E80.color.alpha = 0x80;
    sceVif1PkRef(xglPacketGetCurrent(), &TestEnv_0_004A8E80, 6, 0, 0, 0);
}

static void xglRenderMove(void)
{
    if (s_nClearFrame == 1 && sRender.scene_disabled == 0) {
        xglRenderClear();
    } else if (s_nClearFrame >= 2) {
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
    if (ScanLineInterpolate == 2 && sRender.scene_disabled == 0) {
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
