#include "common.h"
#include "shared.h"
#include "end_print.h"
#include "main/xgl_jpeg.h"

#define NULL ((void *)0)

typedef struct EndPrintRenderState {
    unsigned char unmodeled_00[4];
    short scissor_width;                 /* +0x04 */
    short scissor_height;                /* +0x06 */
    unsigned char unmodeled_08[0x0c];
    unsigned short draw_back_value;      /* +0x14 */
    unsigned char unmodeled_16[0x0a];
    unsigned short draw_back_reset_value; /* +0x20 */
} EndPrintRenderState;

extern EndPrintRenderState sRender;
/* 511 callback entries plus the adjacent terminal/overflow entry. The
 * scaffold's D_00532A38 label is the interior address PrintFunc[511]. */
static PrintFuncEntry PrintFunc[512];
static PrintFuncEntry *pPrintFuncTop;
extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);

int ePrintWHGet(void)
{
    return 0;
}

extern void eMessageSpriteReset(void);

void endPrintInit(void)
{
    pPrintFuncTop = PrintFunc;
    PrintFunc[0].func = NULL;
    eMessageSpriteReset();
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintWinTexLoad);

extern void xglFontReloadTexture(EndPrintContext *context, int mode);

static void PrintFlush(EndPrintContext *context)
{
    PrintFuncEntry *cursor;
    void (*func)(EndPrintContext *context, int param);
    int param;

    xglFontReloadTexture(context, 2);
    cursor = PrintFunc;
print_flush_loop:
    func = cursor->func;
    if (func != NULL) {
        param = cursor->param;
        cursor++;
        func(context, param);
        goto print_flush_loop;
    }
    endPrintInit();
    xglFontReloadTexture(context, 1);
    eMessageSpriteReset();
}

void endPrintExtFuncPack(void (*func)(EndPrintContext *context, int param), int param)
{
    PrintFuncEntry *entry;

    entry = pPrintFuncTop;
    entry->func = func;
    entry->param = param;
    entry++;
    pPrintFuncTop = entry;
    if ((u32)(PrintFunc + 511) < (u32)entry) {
        endPrintInit();
        entry = pPrintFuncTop;
    }
    entry->func = NULL;
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintInfoSet);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endSpriteSet);

INCLUDE_ASM("asm/main/nonmatchings/end_print", subPrintSprite);

/*
 * uv_clut: a fixed table of UV/CLUT entries; only the CLUT id at +0x04 of
 * each 8-byte entry is read here.
 */
typedef struct UvClutEntry {
    const unsigned char *uv;       /* +0x00 */
    int clut;                      /* +0x04 */
} UvClutEntry;

static const unsigned char cur_uv[32];
static const unsigned char others_uv[200];
static const unsigned char pad_uv[64];
static const unsigned char icom_uv[376];
static const unsigned char face_puti_uv[80];
static const unsigned char tag_uv[416];
static const unsigned char win_uv[96];
static const unsigned char num_uv[80];
static const unsigned char pad2_uv[176];
static const unsigned char face_agws_uv[112];
static const unsigned char name_uv[8];
static const unsigned char others2_uv[24];
static const unsigned char face00_uv[16];
static const unsigned char face01_uv[16];
static const unsigned char face02_uv[16];
static const unsigned char face03_uv[16];
static const unsigned char face04_uv[16];
static const unsigned char thumnail_uv[8];
static const unsigned char ether_uv[40];
static const unsigned char seg_uv[192];
static const unsigned char umn_uv[96];
static const unsigned char btl_msg_uv[184];
static const unsigned char btl_msg_uv2[64];
static const unsigned char btl_msg_uv3[48];
static const unsigned char umn2_uv[32];
static const unsigned char umn3_uv[32];

static UvClutEntry uv_clut[26] = {
    { cur_uv, 0x000FDE00 },
    { others_uv, 0x000FDE00 },
    { pad_uv, 0x000FDE00 },
    { icom_uv, 0x000FDF00 },
    { face_puti_uv, 0x000FEF00 },
    { tag_uv, 0x000FDB00 },
    { win_uv, 0x000FDE00 },
    { num_uv, 0x000FDB00 },
    { pad2_uv, 0x000FDF00 },
    { face_agws_uv, 0x000FEE00 },
    { name_uv, 0x000FDB00 },
    { others2_uv, 0x000FDF00 },
    { face00_uv, 0x000FE300 },
    { face01_uv, 0x000FE600 },
    { face02_uv, 0x000FE700 },
    { face03_uv, 0x000FEA00 },
    { face04_uv, 0x000FEB00 },
    { thumnail_uv, 0 },
    { ether_uv, 0x000FDE00 },
    { seg_uv, 0x000FDB00 },
    { umn_uv, 0x000FDE00 },
    { btl_msg_uv, 0x000FDA00 },
    { btl_msg_uv2, 0x000FD700 },
    { btl_msg_uv3, 0x000FD600 },
    { umn2_uv, 0x000FDF00 },
    { umn3_uv, 0x000FDB00 },
};

/*
 * The sprite descriptor subPrintSprite (still asm) draws; only the CLUT
 * index byte at +0x11 is read by PrintSprite00.
 */
typedef struct PrintSpriteInfo {
    unsigned char unmodeled_00[0x11]; /* +0x00..+0x10 */
    unsigned char clutIndex;          /* +0x11 */
} PrintSpriteInfo;

extern void endPrintInfoSet(EndPrintContext *context, int clut, int flag);
extern void subPrintSprite(EndPrintContext *context, PrintSpriteInfo *sprite);

static void PrintSprite00(EndPrintContext *context, PrintSpriteInfo *sprite)
{
    endPrintInfoSet(context, uv_clut[sprite->clutIndex].clut, 1);
    subPrintSprite(context, sprite);
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintSprite01);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintPoint);

INCLUDE_ASM("asm/main/nonmatchings/end_print", subPrintLine);

extern void subPrintLine(EndPrintContext *context, int line);

static void PrintLine(EndPrintContext *context, int line)
{
    endPrintInfoSet(context, 0, 0);
    subPrintLine(context, line);
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintLine2);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintDirectRibbon);

INCLUDE_ASM("asm/main/nonmatchings/end_print", subPrintRibbon);

extern void subPrintRibbon(EndPrintContext *context, int ribbon);

static void PrintRibbon(EndPrintContext *context, int ribbon)
{
    endPrintInfoSet(context, 0, 0);
    subPrintRibbon(context, ribbon);
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintTagFont);

INCLUDE_ASM("asm/main/nonmatchings/end_print", ScissorSet);

static void ScissorReset(EndPrintContext *context)
{
    u64 *scratch;

    scratch = context->scratch;
    scratch[0] = ((u64)0x10000000 << 32) | 0x8001;
    scratch[1] = 14;
    scratch[2] = ((u64)(sRender.scissor_width - 1) << 16) |
                 ((u64)(sRender.scissor_height - 1) << 48);
    scratch[3] = 64;
    sceVif1PkAddDirectDataN(context->packet, scratch, 2);
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintNumber);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintWindow);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintFrame);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintThumbnail);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintCircleCore);

#include "main/xgl_packet.h"

/* EE scratchpad RAM base (src/ov01/gr_gp_init.c's grPacketSend and
 * src/ov01/m_gs.c's MGsGPTerm build the same VIF1 direct-mode packet at the
 * same address). */
#define SCRATCHPAD_BASE ((void *)0x70000000)

extern void sceVif1PkCnt(XglPacket *packet, int count);
extern void sceVif1PkOpenDirectHLCode(XglPacket *packet, int mode);
extern void sceVif1PkCloseDirectHLCode(XglPacket *packet);

/* PrintCircleCore (this TU, still asm) draws into the scratchpad buffer this
 * function points it at. */
static void PrintCircleCore(XglPacket *packet, void *buffer, int radius);

void endPrintDirectCircle(int radius) {
    XglPacket *vif1Packet;

    vif1Packet = xglPacketGetCurrent();
    sceVif1PkCnt(vif1Packet, 0);
    sceVif1PkOpenDirectHLCode(vif1Packet, 0);
    PrintCircleCore(vif1Packet, SCRATCHPAD_BASE, radius);
    sceVif1PkCloseDirectHLCode(vif1Packet);
}

static void PrintCircle(EndPrintContext *context, int radius)
{
    PrintCircleCore(context->packet, context->scratch, radius);
}

extern void sceVif1PkAddDirectDataN(XglPacket *packet, const void *data, int count);

static void FontTexReload(EndPrintContext *context)
{
    u64 *scratch;

    scratch = context->scratch;
    scratch[0] = ((u64) 0x10000000 << 32) | 0x8000;
    scratch[1] = 0xE;
    scratch[3] = 0x3F;
    scratch[2] = 0;
    sceVif1PkAddDirectDataN(context->packet, scratch, 1);
    xglFontReloadTexture(context, 2);
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", FontTexChange);

static void DrawBackSet(DrawBackContext *context)
{
    u64 *scratch;
    unsigned int draw_back_control;

    scratch = context->scratch;
    draw_back_control = sRender.draw_back_value;
    draw_back_control |= 0x80000;
    scratch[0] = ((u64)0x10000000 << 32) | 0x8003;
    scratch[1] = 14;
    scratch[3] = 63;
    scratch[4] = (u64)0x8000 << 17;
    scratch[5] = 78;
    scratch[6] = draw_back_control;
    scratch[7] = 76;
    scratch[2] = 0;
    sceVif1PkAddDirectDataN(context->packet, scratch, 4);
}

static void DrawBackReset(DrawBackContext *context)
{
    u64 *scratch;
    unsigned int draw_back_control;

    scratch = context->scratch;
    draw_back_control = sRender.draw_back_reset_value;
    draw_back_control |= 0x80000;
    scratch[0] = ((u64)0x10000000 << 32) | 0x8003;
    scratch[1] = 14;
    scratch[3] = 63;
    scratch[4] = (u64)0x3100 << 16;
    scratch[5] = 78;
    scratch[6] = draw_back_control;
    scratch[7] = 76;
    scratch[2] = 0;
    sceVif1PkAddDirectDataN(context->packet, scratch, 4);
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", ScreenClear);

INCLUDE_ASM("asm/main/nonmatchings/end_print", ZScissor);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintBackSprite);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintBackSprite2);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintEtherLine);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintExtFunc);

void endPrintDirectFrameCopy(XglPacket *packet, int tagLow, int tagHigh)
{
    /* This 12-qword VIF/GIF template is retained across calls; element 4 is
     * patched with the per-copy tag below. */
    static u64 FrameCopyEnv[12] = {
        0x0000000000000000ULL,
        0x5100000511000000ULL,
        0x1000000000008004ULL,
        0x000000000000000EULL,
        0x0000000000000000ULL,
        0x0000000000000050ULL,
        0x0000000000000000ULL,
        0x0000000000000051ULL,
        0x000001C000000200ULL,
        0x0000000000000052ULL,
        0x0000000000000002ULL,
        0x0000000000000053ULL,
    };

    if (packet != NULL) {
        FrameCopyEnv[4] = (u64) (u32) (tagLow | 0x80000) |
                          ((u64) tagHigh << 0x20) |
                          ((u64) 0x8000 << 0x24);
        sceVif1PkRef(packet, FrameCopyEnv, 6, 0, 0, 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintDirectLine);

/*
 * The JPEG-decode setup context: only the fields endDecodeJpeg itself
 * reads or writes are named. +0x10/+0x14 are filled from the source and
 * destination words already stored at +0x30/+0x34, and +0x20..+0x27 is
 * cleared before the decode call; the rest of the struct is untouched
 * here.
 */
typedef struct EndDecodeJpegContext {
    unsigned char unmodeled_00[8]; /* +0x00..+0x07 */
    short width;                   /* +0x08 */
    short height;                  /* +0x0a */
    unsigned char unmodeled_0c[4]; /* +0x0c..+0x0f */
    int decodeSource;              /* +0x10 */
    int decodeDestination;         /* +0x14 */
    unsigned char unmodeled_18[8]; /* +0x18..+0x1f */
    short decodeParam[4];          /* +0x20..+0x27 */
    unsigned char unmodeled_28[8]; /* +0x28..+0x2f */
    int source;                    /* +0x30 */
    int destination;               /* +0x34 */
} EndDecodeJpegContext;

void endDecodeJpeg(EndDecodeJpegContext *context) {
    int source;
    int destination;

    source = context->source;
    destination = context->destination;
    context->width = 0x200;
    context->height = 0x1C0;
    context->decodeSource = source;
    context->decodeDestination = destination;
    context->decodeParam[0] = 0;
    context->decodeParam[1] = 0;
    context->decodeParam[2] = 0;
    context->decodeParam[3] = 0;
    xglJpegDecode(&context->decodeSource);
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintJpeg);

#define SPRITE_UV_BYTES(u, v, width, height) \
    (u) & 255, (u) >> 8, (v) & 255, (v) >> 8, \
    (width) & 255, (width) >> 8, (height) & 255, (height) >> 8

static const unsigned char cur_uv[32] = {
    SPRITE_UV_BYTES(0, 192, 16, 16),
    SPRITE_UV_BYTES(64, 160, 16, 16),
    SPRITE_UV_BYTES(0, 176, 16, 16),
    SPRITE_UV_BYTES(64, 176, 16, 16)
};

static const unsigned char others_uv[200] = {
    SPRITE_UV_BYTES(199, 227, 8, 16),
    SPRITE_UV_BYTES(200, 243, 7, 13),
    SPRITE_UV_BYTES(16, 192, 18, 12),
    SPRITE_UV_BYTES(34, 192, 18, 12),
    SPRITE_UV_BYTES(88, 192, 18, 12),
    SPRITE_UV_BYTES(70, 192, 18, 12),
    SPRITE_UV_BYTES(143, 217, 47, 10),
    SPRITE_UV_BYTES(122, 192, 11, 16),
    SPRITE_UV_BYTES(188, 243, 10, 13),
    SPRITE_UV_BYTES(218, 0, 29, 20),
    SPRITE_UV_BYTES(218, 20, 29, 20),
    SPRITE_UV_BYTES(209, 194, 43, 62),
    SPRITE_UV_BYTES(160, 80, 16, 16),
    SPRITE_UV_BYTES(218, 40, 22, 22),
    SPRITE_UV_BYTES(218, 62, 22, 22),
    SPRITE_UV_BYTES(160, 0, 29, 20),
    SPRITE_UV_BYTES(160, 20, 29, 20),
    SPRITE_UV_BYTES(160, 40, 29, 20),
    SPRITE_UV_BYTES(160, 60, 29, 20),
    SPRITE_UV_BYTES(189, 0, 29, 20),
    SPRITE_UV_BYTES(189, 20, 29, 20),
    SPRITE_UV_BYTES(189, 40, 29, 20),
    SPRITE_UV_BYTES(189, 60, 29, 20),
    SPRITE_UV_BYTES(0, 192, 16, 16),
    SPRITE_UV_BYTES(176, 80, 8, 8)
};

static const unsigned char icom_uv[376] = {
    SPRITE_UV_BYTES(388, 67, 20, 24),
    SPRITE_UV_BYTES(408, 67, 20, 24),
    SPRITE_UV_BYTES(428, 67, 20, 24),
    SPRITE_UV_BYTES(448, 67, 20, 24),
    SPRITE_UV_BYTES(468, 67, 20, 24),
    SPRITE_UV_BYTES(488, 67, 20, 24),
    SPRITE_UV_BYTES(388, 91, 20, 24),
    SPRITE_UV_BYTES(408, 91, 20, 24),
    SPRITE_UV_BYTES(428, 91, 20, 24),
    SPRITE_UV_BYTES(448, 91, 20, 24),
    SPRITE_UV_BYTES(468, 91, 20, 24),
    SPRITE_UV_BYTES(488, 91, 20, 24),
    SPRITE_UV_BYTES(388, 115, 20, 24),
    SPRITE_UV_BYTES(408, 115, 20, 24),
    SPRITE_UV_BYTES(428, 115, 20, 24),
    SPRITE_UV_BYTES(388, 139, 20, 24),
    SPRITE_UV_BYTES(408, 139, 20, 24),
    SPRITE_UV_BYTES(428, 139, 20, 24),
    SPRITE_UV_BYTES(448, 139, 20, 24),
    SPRITE_UV_BYTES(468, 139, 20, 24),
    SPRITE_UV_BYTES(488, 139, 20, 24),
    SPRITE_UV_BYTES(388, 163, 20, 24),
    SPRITE_UV_BYTES(408, 163, 20, 24),
    SPRITE_UV_BYTES(428, 163, 20, 24),
    SPRITE_UV_BYTES(448, 163, 20, 24),
    SPRITE_UV_BYTES(468, 163, 20, 24),
    SPRITE_UV_BYTES(488, 163, 20, 24),
    SPRITE_UV_BYTES(388, 187, 20, 24),
    SPRITE_UV_BYTES(428, 187, 20, 24),
    SPRITE_UV_BYTES(408, 187, 20, 24),
    SPRITE_UV_BYTES(448, 187, 20, 24),
    SPRITE_UV_BYTES(484, 187, 16, 16),
    SPRITE_UV_BYTES(468, 187, 16, 16),
    SPRITE_UV_BYTES(388, 211, 20, 24),
    SPRITE_UV_BYTES(256, 162, 20, 24),
    SPRITE_UV_BYTES(276, 162, 20, 24),
    SPRITE_UV_BYTES(296, 162, 20, 24),
    SPRITE_UV_BYTES(316, 162, 20, 24),
    SPRITE_UV_BYTES(336, 162, 20, 24),
    SPRITE_UV_BYTES(356, 162, 20, 24),
    SPRITE_UV_BYTES(276, 186, 20, 24),
    SPRITE_UV_BYTES(296, 186, 20, 24),
    SPRITE_UV_BYTES(256, 186, 20, 24),
    SPRITE_UV_BYTES(209, 232, 32, 24),
    SPRITE_UV_BYTES(448, 115, 20, 24),
    SPRITE_UV_BYTES(388, 67, 20, 24),
    SPRITE_UV_BYTES(408, 67, 20, 24)
};

static const unsigned char pad_uv[64] = {
    SPRITE_UV_BYTES(0, 208, 22, 24),
    SPRITE_UV_BYTES(22, 208, 22, 24),
    SPRITE_UV_BYTES(44, 208, 22, 24),
    SPRITE_UV_BYTES(66, 208, 22, 24),
    SPRITE_UV_BYTES(198, 132, 29, 20),
    SPRITE_UV_BYTES(198, 92, 29, 20),
    SPRITE_UV_BYTES(227, 132, 29, 20),
    SPRITE_UV_BYTES(227, 92, 29, 20)
};

static const unsigned char face_agws_uv[112] = {
    SPRITE_UV_BYTES(768, 93, 63, 48),
    SPRITE_UV_BYTES(832, 93, 63, 48),
    SPRITE_UV_BYTES(896, 93, 63, 48),
    SPRITE_UV_BYTES(960, 93, 63, 48),
    SPRITE_UV_BYTES(768, 141, 63, 48),
    SPRITE_UV_BYTES(832, 141, 63, 48),
    SPRITE_UV_BYTES(896, 141, 63, 48),
    SPRITE_UV_BYTES(960, 141, 63, 48),
    SPRITE_UV_BYTES(768, 93, 63, 32),
    SPRITE_UV_BYTES(832, 93, 63, 32),
    SPRITE_UV_BYTES(896, 93, 63, 32),
    SPRITE_UV_BYTES(960, 93, 63, 32),
    SPRITE_UV_BYTES(768, 141, 63, 32),
    SPRITE_UV_BYTES(832, 141, 63, 32)
};

static const unsigned char tag_uv[416] = {
    SPRITE_UV_BYTES(253, 0, 10, 16),
    SPRITE_UV_BYTES(263, 0, 10, 16),
    SPRITE_UV_BYTES(273, 0, 10, 16),
    SPRITE_UV_BYTES(283, 0, 10, 16),
    SPRITE_UV_BYTES(293, 0, 9, 16),
    SPRITE_UV_BYTES(302, 0, 9, 16),
    SPRITE_UV_BYTES(311, 0, 10, 16),
    SPRITE_UV_BYTES(321, 0, 10, 16),
    SPRITE_UV_BYTES(331, 0, 6, 16),
    SPRITE_UV_BYTES(337, 0, 8, 16),
    SPRITE_UV_BYTES(345, 0, 10, 16),
    SPRITE_UV_BYTES(355, 0, 9, 16),
    SPRITE_UV_BYTES(364, 0, 13, 16),
    SPRITE_UV_BYTES(377, 0, 12, 16),
    SPRITE_UV_BYTES(389, 0, 10, 16),
    SPRITE_UV_BYTES(399, 0, 9, 16),
    SPRITE_UV_BYTES(408, 0, 10, 16),
    SPRITE_UV_BYTES(418, 0, 10, 16),
    SPRITE_UV_BYTES(428, 0, 10, 16),
    SPRITE_UV_BYTES(438, 0, 10, 16),
    SPRITE_UV_BYTES(448, 0, 10, 16),
    SPRITE_UV_BYTES(458, 0, 10, 16),
    SPRITE_UV_BYTES(468, 0, 13, 16),
    SPRITE_UV_BYTES(481, 0, 10, 16),
    SPRITE_UV_BYTES(491, 0, 10, 16),
    SPRITE_UV_BYTES(501, 0, 11, 16),
    SPRITE_UV_BYTES(253, 16, 8, 11),
    SPRITE_UV_BYTES(261, 16, 8, 11),
    SPRITE_UV_BYTES(269, 16, 8, 11),
    SPRITE_UV_BYTES(277, 16, 8, 11),
    SPRITE_UV_BYTES(285, 16, 7, 11),
    SPRITE_UV_BYTES(292, 16, 7, 11),
    SPRITE_UV_BYTES(299, 16, 8, 11),
    SPRITE_UV_BYTES(307, 16, 8, 11),
    SPRITE_UV_BYTES(315, 16, 5, 11),
    SPRITE_UV_BYTES(320, 16, 6, 11),
    SPRITE_UV_BYTES(326, 16, 8, 11),
    SPRITE_UV_BYTES(334, 16, 7, 11),
    SPRITE_UV_BYTES(341, 16, 10, 11),
    SPRITE_UV_BYTES(351, 16, 9, 11),
    SPRITE_UV_BYTES(360, 16, 8, 11),
    SPRITE_UV_BYTES(368, 16, 8, 11),
    SPRITE_UV_BYTES(376, 16, 8, 11),
    SPRITE_UV_BYTES(384, 16, 8, 11),
    SPRITE_UV_BYTES(392, 16, 8, 11),
    SPRITE_UV_BYTES(400, 16, 7, 11),
    SPRITE_UV_BYTES(407, 16, 8, 11),
    SPRITE_UV_BYTES(415, 16, 9, 11),
    SPRITE_UV_BYTES(424, 16, 11, 11),
    SPRITE_UV_BYTES(435, 16, 9, 11),
    SPRITE_UV_BYTES(444, 16, 8, 11),
    SPRITE_UV_BYTES(452, 16, 8, 11)
};

static const unsigned char win_uv[96] = {
    SPRITE_UV_BYTES(141, 141, 3, 3),
    SPRITE_UV_BYTES(0, 141, 128, 3),
    SPRITE_UV_BYTES(144, 141, 3, 3),
    SPRITE_UV_BYTES(141, 0, 3, 128),
    SPRITE_UV_BYTES(144, 0, 3, 128),
    SPRITE_UV_BYTES(141, 144, 3, 3),
    SPRITE_UV_BYTES(0, 144, 128, 3),
    SPRITE_UV_BYTES(144, 144, 3, 3),
    SPRITE_UV_BYTES(0, 0, 128, 128),
    SPRITE_UV_BYTES(154, 208, 44, 19),
    SPRITE_UV_BYTES(316, 187, 20, 24),
    SPRITE_UV_BYTES(512, 186, 165, 24)
};

static const unsigned char num_uv[80] = {
    SPRITE_UV_BYTES(88, 227, 11, 16),
    SPRITE_UV_BYTES(88, 243, 10, 13),
    SPRITE_UV_BYTES(199, 227, 8, 16),
    SPRITE_UV_BYTES(200, 243, 7, 13),
    SPRITE_UV_BYTES(188, 243, 10, 13),
    SPRITE_UV_BYTES(88, 211, 11, 16),
    SPRITE_UV_BYTES(99, 211, 11, 16),
    SPRITE_UV_BYTES(121, 211, 11, 16),
    SPRITE_UV_BYTES(132, 211, 11, 16),
    SPRITE_UV_BYTES(143, 211, 4, 4)
};

static const unsigned char pad2_uv[176] = {
    SPRITE_UV_BYTES(256, 67, 22, 24),
    SPRITE_UV_BYTES(278, 67, 22, 24),
    SPRITE_UV_BYTES(300, 67, 22, 24),
    SPRITE_UV_BYTES(322, 67, 22, 24),
    SPRITE_UV_BYTES(344, 67, 22, 24),
    SPRITE_UV_BYTES(366, 67, 22, 24),
    SPRITE_UV_BYTES(256, 91, 22, 24),
    SPRITE_UV_BYTES(278, 91, 22, 24),
    SPRITE_UV_BYTES(300, 91, 22, 24),
    SPRITE_UV_BYTES(322, 91, 22, 24),
    SPRITE_UV_BYTES(344, 91, 22, 24),
    SPRITE_UV_BYTES(366, 91, 22, 24),
    SPRITE_UV_BYTES(256, 115, 22, 24),
    SPRITE_UV_BYTES(278, 115, 22, 24),
    SPRITE_UV_BYTES(300, 115, 22, 24),
    SPRITE_UV_BYTES(322, 115, 22, 24),
    SPRITE_UV_BYTES(344, 115, 22, 24),
    SPRITE_UV_BYTES(366, 115, 22, 24),
    SPRITE_UV_BYTES(256, 139, 22, 24),
    SPRITE_UV_BYTES(278, 139, 22, 24),
    SPRITE_UV_BYTES(300, 139, 22, 24),
    SPRITE_UV_BYTES(322, 139, 22, 24)
};

static const unsigned char name_uv[8] = {
    SPRITE_UV_BYTES(256, 210, 105, 24)
};

static const unsigned char others2_uv[24] = {
    SPRITE_UV_BYTES(0, 208, 384, 48),
    SPRITE_UV_BYTES(0, 201, 500, 7),
    SPRITE_UV_BYTES(230, 56, 10, 72)
};

static const unsigned char face00_uv[16] = {
    SPRITE_UV_BYTES(512, 0, 64, 93),
    SPRITE_UV_BYTES(576, 0, 64, 93)
};

static const unsigned char face01_uv[16] = {
    SPRITE_UV_BYTES(640, 0, 64, 93),
    SPRITE_UV_BYTES(704, 0, 64, 93)
};

static const unsigned char face02_uv[16] = {
    SPRITE_UV_BYTES(512, 93, 64, 93),
    SPRITE_UV_BYTES(576, 93, 64, 93)
};

static const unsigned char face03_uv[16] = {
    SPRITE_UV_BYTES(640, 93, 64, 93),
    SPRITE_UV_BYTES(704, 93, 64, 93)
};

static const unsigned char face04_uv[16] = {
    SPRITE_UV_BYTES(768, 0, 64, 93),
    SPRITE_UV_BYTES(832, 0, 64, 93)
};

static const unsigned char face_puti_uv[80] = {
    SPRITE_UV_BYTES(896, 0, 20, 24),
    SPRITE_UV_BYTES(916, 0, 20, 24),
    SPRITE_UV_BYTES(936, 0, 20, 24),
    SPRITE_UV_BYTES(956, 0, 20, 24),
    SPRITE_UV_BYTES(896, 24, 20, 24),
    SPRITE_UV_BYTES(916, 24, 20, 24),
    SPRITE_UV_BYTES(936, 24, 20, 24),
    SPRITE_UV_BYTES(976, 24, 20, 24),
    SPRITE_UV_BYTES(976, 0, 20, 24),
    SPRITE_UV_BYTES(996, 0, 20, 24)
};

static const unsigned char thumnail_uv[8] = {
    SPRITE_UV_BYTES(0, 0, 128, 112)
};

static const unsigned char ether_uv[40] = {
    SPRITE_UV_BYTES(0, 0, 38, 48),
    SPRITE_UV_BYTES(38, 0, 38, 48),
    SPRITE_UV_BYTES(76, 0, 38, 48),
    SPRITE_UV_BYTES(160, 0, 13, 16),
    SPRITE_UV_BYTES(114, 0, 46, 56)
};

static const unsigned char seg_uv[192] = {
    SPRITE_UV_BYTES(0, 48, 72, 58),
    SPRITE_UV_BYTES(0, 106, 72, 58),
    SPRITE_UV_BYTES(72, 56, 46, 57),
    SPRITE_UV_BYTES(160, 16, 10, 22),
    SPRITE_UV_BYTES(173, 0, 28, 25),
    SPRITE_UV_BYTES(221, 0, 20, 24),
    SPRITE_UV_BYTES(201, 0, 20, 24),
    SPRITE_UV_BYTES(72, 56, 46, 57),
    SPRITE_UV_BYTES(118, 56, 42, 53),
    SPRITE_UV_BYTES(160, 56, 18, 61),
    SPRITE_UV_BYTES(178, 56, 14, 57),
    SPRITE_UV_BYTES(192, 56, 21, 72),
    SPRITE_UV_BYTES(213, 56, 17, 68),
    SPRITE_UV_BYTES(72, 113, 34, 34),
    SPRITE_UV_BYTES(106, 113, 30, 30),
    SPRITE_UV_BYTES(0, 183, 47, 9),
    SPRITE_UV_BYTES(47, 183, 47, 9),
    SPRITE_UV_BYTES(94, 183, 43, 9),
    SPRITE_UV_BYTES(0, 192, 47, 9),
    SPRITE_UV_BYTES(47, 192, 47, 9),
    SPRITE_UV_BYTES(94, 192, 43, 9),
    SPRITE_UV_BYTES(256, 0, 88, 169),
    SPRITE_UV_BYTES(512, 0, 240, 48),
    SPRITE_UV_BYTES(512, 48, 240, 24)
};

static const unsigned char umn_uv[96] = {
    SPRITE_UV_BYTES(0, 0, 102, 128),
    SPRITE_UV_BYTES(102, 0, 102, 128),
    SPRITE_UV_BYTES(0, 128, 102, 128),
    SPRITE_UV_BYTES(102, 128, 102, 128),
    SPRITE_UV_BYTES(256, 48, 20, 24),
    SPRITE_UV_BYTES(276, 48, 20, 24),
    SPRITE_UV_BYTES(296, 48, 20, 24),
    SPRITE_UV_BYTES(316, 48, 20, 24),
    SPRITE_UV_BYTES(256, 0, 38, 48),
    SPRITE_UV_BYTES(294, 0, 38, 48),
    SPRITE_UV_BYTES(332, 0, 38, 48),
    SPRITE_UV_BYTES(512, 0, 511, 96)
};

static const unsigned char umn2_uv[32] = {
    SPRITE_UV_BYTES(370, 0, 38, 40),
    SPRITE_UV_BYTES(408, 0, 40, 40),
    SPRITE_UV_BYTES(370, 40, 38, 40),
    SPRITE_UV_BYTES(408, 40, 38, 40)
};

static const unsigned char umn3_uv[32] = {
    SPRITE_UV_BYTES(448, 0, 22, 26),
    SPRITE_UV_BYTES(470, 0, 22, 26),
    SPRITE_UV_BYTES(448, 26, 22, 26),
    SPRITE_UV_BYTES(470, 26, 22, 26)
};

static const unsigned char btl_msg_uv[184] = {
    SPRITE_UV_BYTES(256, 47, 22, 24),
    SPRITE_UV_BYTES(278, 47, 22, 24),
    SPRITE_UV_BYTES(300, 47, 22, 24),
    SPRITE_UV_BYTES(322, 47, 22, 24),
    SPRITE_UV_BYTES(344, 47, 22, 24),
    SPRITE_UV_BYTES(256, 71, 22, 24),
    SPRITE_UV_BYTES(278, 71, 22, 24),
    SPRITE_UV_BYTES(300, 71, 22, 24),
    SPRITE_UV_BYTES(322, 71, 22, 24),
    SPRITE_UV_BYTES(344, 71, 22, 24),
    SPRITE_UV_BYTES(366, 71, 22, 24),
    SPRITE_UV_BYTES(256, 95, 22, 24),
    SPRITE_UV_BYTES(278, 95, 22, 24),
    SPRITE_UV_BYTES(300, 95, 22, 24),
    SPRITE_UV_BYTES(322, 95, 22, 24),
    SPRITE_UV_BYTES(344, 95, 22, 24),
    SPRITE_UV_BYTES(366, 95, 22, 24),
    SPRITE_UV_BYTES(256, 119, 22, 24),
    SPRITE_UV_BYTES(278, 119, 22, 24),
    SPRITE_UV_BYTES(300, 119, 22, 24),
    SPRITE_UV_BYTES(322, 119, 22, 24),
    SPRITE_UV_BYTES(344, 119, 22, 24),
    SPRITE_UV_BYTES(366, 119, 22, 24)
};

static const unsigned char btl_msg_uv2[64] = {
    SPRITE_UV_BYTES(353, 143, 22, 24),
    SPRITE_UV_BYTES(375, 143, 22, 24),
    SPRITE_UV_BYTES(397, 143, 22, 24),
    SPRITE_UV_BYTES(419, 143, 22, 24),
    SPRITE_UV_BYTES(281, 143, 18, 46),
    SPRITE_UV_BYTES(299, 143, 18, 46),
    SPRITE_UV_BYTES(317, 143, 18, 46),
    SPRITE_UV_BYTES(335, 143, 18, 46)
};

static const unsigned char btl_msg_uv3[48] = {
    SPRITE_UV_BYTES(160, 0, 20, 24),
    SPRITE_UV_BYTES(180, 0, 20, 24),
    SPRITE_UV_BYTES(200, 0, 20, 24),
    SPRITE_UV_BYTES(160, 24, 9, 10),
    SPRITE_UV_BYTES(169, 24, 13, 10),
    SPRITE_UV_BYTES(182, 24, 11, 10)
};
