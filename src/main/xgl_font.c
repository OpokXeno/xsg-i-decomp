#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontDebugMode);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetKanjiClutUV);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", set_ot);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", set_xyz);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetSPcodeSize);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontPrintSub);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontPrintDirectCore);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontPrintf);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontPrint);

/* Prints text straight away into ordering-table slot ot (passed to set_ot). */
extern void xglFontPrintDirectOT(int ot, const char *text);

void xglFontPrintDirect(const char *text)
{
    xglFontPrintDirectOT(0, text);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontPrintDirectOT);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontPrintExtFunc);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontDebugPrintf);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontDebugHex);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", buffer_reset);

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

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetStringWidth);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetLoadAddress);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontGetFlags);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontSetFlags);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontLoad);

INCLUDE_ASM("asm/main/nonmatchings/xgl_font", xglFontInitial);
