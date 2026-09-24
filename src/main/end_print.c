#include "common.h"
#include "shared.h"
#include "end_print.h"

#define NULL ((void *)0)

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

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintExtFuncPack);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintInfoSet);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endSpriteSet);

INCLUDE_ASM("asm/main/nonmatchings/end_print", subPrintSprite);

/*
 * uv_clut: a fixed table of UV/CLUT entries; only the CLUT id at +0x04 of
 * each 8-byte entry is read here.
 */
typedef struct UvClutEntry {
    unsigned char unmodeled_00[4]; /* +0x00 */
    int clut;                      /* +0x04 */
} UvClutEntry;

extern UvClutEntry uv_clut[26];

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

INCLUDE_ASM("asm/main/nonmatchings/end_print", ScissorReset);

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

INCLUDE_ASM("asm/main/nonmatchings/end_print", DrawBackSet);

INCLUDE_ASM("asm/main/nonmatchings/end_print", DrawBackReset);

INCLUDE_ASM("asm/main/nonmatchings/end_print", ScreenClear);

INCLUDE_ASM("asm/main/nonmatchings/end_print", ZScissor);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintBackSprite);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintBackSprite2);

INCLUDE_ASM("asm/main/nonmatchings/end_print", PrintEtherLine);

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintExtFunc);

/* VIF1 direct-mode environment for endPrintDirectFrameCopy; only element 4
 * (the tag word written below) is touched here. */
extern u64 FrameCopyEnv_6[12];

void endPrintDirectFrameCopy(XglPacket *packet, int tagLow, int tagHigh)
{
    if (packet != NULL) {
        FrameCopyEnv_6[4] = (u64) (u32) (tagLow | 0x80000) |
                             ((u64) tagHigh << 0x20) |
                             ((u64) 0x8000 << 0x24);
        sceVif1PkRef(packet, FrameCopyEnv_6, 6, 0, 0, 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/end_print", endPrintDirectLine);

extern int xglJpegDecode(void *request);

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
