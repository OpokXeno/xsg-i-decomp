#include "common.h"

struct FontImage {
    unsigned char unmodeled_00000[0x78040];
    unsigned short proportional_widths[256];
};

struct FontState {
    struct FontImage *font_image;
    void *window_image;
    unsigned short flags;
    unsigned char unmodeled_0a[6];
    unsigned char default_width;
    unsigned char unmodeled_11;
    unsigned char proportional_mode;
};

/* The partial state view shares the complete 0x20-byte retail allocation. */
typedef union FontStateStorage {
    struct FontState state;
    unsigned char bytes[0x20];
} FontStateStorage;
static FontStateStorage FS;
#define D_00881188 (FS.state.flags)

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontDebugMode);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetKanjiClutUV);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", set_ot);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", set_xyz);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetSPcodeSize);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontPrintSub);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontPrintDirectCore);


static void set_xyz(int x, int y, int color);
static void xglFontPrintSub(void *args);

void xglFontPrintf(int x, int y, unsigned int color, void *arg)
{
    if (D_00881188 & 1 & 0xFFFF) {
        set_xyz(x, y, color);
        xglFontPrintSub(arg);
    }
}

static void xglFontPrintDirectCore(const char *text);

void xglFontPrint(int x, int y, int color, const char *text)
{
    if (D_00881188 & 1 & 0xFFFF) {
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
    if (D_00881188 & 1 & 0xFFFF) {
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

typedef union FontPrintQueueStorage {
    unsigned long long alignment;
    FontQueueSlot slots[16];
    unsigned char bytes[0x2860];
} FontPrintQueueStorage;
static FontPrintQueueStorage D_008811A0;
#define FONT_PRINT_QUEUE (D_008811A0.slots)

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
    slot = FONT_PRINT_QUEUE;
    do {
        slot->selfOffset = (unsigned char *)slot - (unsigned char *)FONT_PRINT_QUEUE;
        remaining--;
        slot->link = 0;
        slot++;
    } while (remaining >= 0);

    header = (unsigned char *)FONT_PRINT_QUEUE - FONT_QUEUE_HEADER_OFFSET;
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

unsigned int xglFontGetProportionalSize(int code)
{
    struct FontState *state = &FS.state;
    struct FontImage *font_image = state->font_image;
    unsigned int width;
    signed char average_width;

    width = (unsigned short)(font_image->proportional_widths[code] + 0xff);
    if ((width & 0xff) == 0xff) {
        width = state->default_width << 8;
    }
    if (state->proportional_mode != 0) {
        average_width = ((int)((width & 0xff) + (width >> 8)) >> 1) - 5;
        if (average_width < 0) {
            average_width = 0;
        }
        if (average_width >= 0x0b) {
            average_width = 0x0a;
        }
        width = (((average_width + 9) << 8) + average_width) & 0xffff;
    }
    return width;
}

static unsigned int hex2val(signed char digit)
{
    unsigned int digit_value;

    digit_value = (digit - '0') & 0xff;
    if (digit_value >= 10U) {
        if ((unsigned int)(digit - 'a') < 6U) {
            return (digit - 'W') & 0xff;
        }
        if ((unsigned int)(digit - 'A') < 6U) {
            return (digit - '7') & 0xff;
        }
        return 0U;
    }
    return digit_value;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontAscii2Euc);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontReloadTexture);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontFlush);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetStringWidth2);

extern int xglFontGetStringWidth2(const char *text, int);

int xglFontGetStringWidth(const char *text)
{
    return xglFontGetStringWidth2(text, 0);
}

void *xglFontGetLoadAddress(void)
{
    return FS.state.font_image;
}

/* D_00881188 is the original interior label for the font-state flags. */

unsigned short xglFontGetFlags(void)
{
    return D_00881188;
}

void xglFontSetFlags(int flags)
{
    D_00881188 = flags & 0xFFFD;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontLoad);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontInitial);



/* xglFontInitial supplies this file-read buffer to xglFontLoad.
 * The recovered FontImage view names its proportional-width table. The
 * complete retail capacity also includes bytes beyond that partial view. */
typedef union FontImageStorage {
    struct FontImage image;
    unsigned char bytes[0x78800];
} FontImageStorage;
static FontImageStorage FontImage;
