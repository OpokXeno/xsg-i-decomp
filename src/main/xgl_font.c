#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontDebugMode);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetKanjiClutUV);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", set_ot);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", set_xyz);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetSPcodeSize);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontPrintSub);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontPrintDirectCore);

extern unsigned char D_00881188[];

static void set_xyz(int x, int y, int color);
static void xglFontPrintSub(void *args);

void xglFontPrintf(int x, int y, unsigned int color, void *arg)
{
    if (*(unsigned short *)D_00881188 & 1 & 0xFFFF) {
        set_xyz(x, y, color);
        xglFontPrintSub(arg);
    }
}

static void xglFontPrintDirectCore(const char *text);

void xglFontPrint(int x, int y, int color, const char *text)
{
    if (*(unsigned short *)D_00881188 & 1 & 0xFFFF) {
        set_xyz(x, y, color);
        xglFontPrintDirectCore(text);
    }
}

/* Prints text straight away into ordering-table slot ot (passed to set_ot). */
extern void xglFontPrintDirectOT(int ot, const char *text);

void xglFontPrintDirect(const char *text)
{
    xglFontPrintDirectOT(0, text);
}

static void set_ot(int ot);

void xglFontPrintDirectOT(int ot, const char *text)
{
    if (*(unsigned short *)D_00881188 & 1 & 0xFFFF) {
        set_ot(ot);
        xglFontPrintDirectCore(text);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontPrintExtFunc);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontDebugPrintf);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontDebugHex);

/*
 * Each 4-byte slot of the font print-queue buffer at D_008811A0 (16 slots,
 * scaffold-owned splat name): link is the byte offset, from this array's
 * base, of the next queued record for this bucket (0 marks the bucket
 * empty -- xglFontFlush follows it as a chain), and selfOffset is the
 * slot's own byte offset within the array.
 */
typedef struct FontQueueSlot {
    unsigned short link;
    unsigned short selfOffset;
} FontQueueSlot;

extern FontQueueSlot D_008811A0[16];

/*
 * buffer_reset also reaches two fields 0x20 bytes before the queue buffer:
 * a reset flag byte and the queue's free-cursor pointer. The header these
 * belong to is not otherwise evidenced within this allocation, so they are
 * reached by byte offset from the queue symbol instead of a named member
 * (matching src/ov11/res.c's RES_IsDebugMode).
 */
#define FONT_QUEUE_HEADER_OFFSET 0x20
#define FONT_QUEUE_HEADER_RESET_FLAG_OFFSET 0x11
#define FONT_QUEUE_HEADER_CURSOR_OFFSET 0x1C

static void buffer_reset(void)
{
    FontQueueSlot *slot;
    unsigned char *header;
    int remaining;

    remaining = 15;
    slot = D_008811A0;
    do {
        slot->selfOffset = (unsigned char *)slot - (unsigned char *)D_008811A0;
        remaining--;
        slot->link = 0;
        slot++;
    } while (remaining >= 0);

    header = (unsigned char *)D_008811A0 - FONT_QUEUE_HEADER_OFFSET;
    *(FontQueueSlot **)(header + FONT_QUEUE_HEADER_CURSOR_OFFSET) = slot;
    header[FONT_QUEUE_HEADER_RESET_FLAG_OFFSET] = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontFlushSub);

/*
 * Draws one glyph from the font texture: u and v are texel coordinates in
 * 1/16 units, size packs the glyph width (high byte) and height (low byte),
 * flags' low nibble selects the draw attributes and bit 4 can double the
 * drawn size.
 */
static void xglFontFlushSub(int u, int v, int size, int flags);

/* Hex digit glyphs are 6x8 texels, 8 texels apart at v 0x2F00 (row 752). */
static void xglFontFlushSubHex(int digit)
{
    xglFontFlushSub(digit << 7, 0x2F00, (6 << 8) | 8, 0x13);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontFlushSubCRLF);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontFlushCore);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontCheckProportional);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetProportionalSize);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", hex2val);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontAscii2Euc);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontReloadTexture);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontFlush);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetStringWidth2);

extern int xglFontGetStringWidth2(const char *text, int);

int xglFontGetStringWidth(const char *text)
{
    return xglFontGetStringWidth2(text, 0);
}

/* The font resource's load address (scaffold-owned real symbol, offset 0:
 * xglFontGetLoadAddress reads its whole first word; the object's further
 * extent is not evidenced within this allocation). */
extern unsigned char FS[];

void *xglFontGetLoadAddress(void)
{
    return *(void **)FS;
}

/* The font module's flags word (scaffold-owned splat name; xglFontGetFlags
 * and xglFontSetFlags are its only accessors in this allocation, and its
 * further extent is not evidenced here). */
extern unsigned char D_00881188[];

unsigned short xglFontGetFlags(void)
{
    return *(unsigned short *)D_00881188;
}

void xglFontSetFlags(int flags)
{
    *(unsigned short *)D_00881188 = flags & 0xFFFD;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontLoad);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontInitial);
