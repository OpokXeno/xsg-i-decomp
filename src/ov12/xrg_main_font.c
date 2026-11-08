/*
 * OV12 original TU 93: 0x00a4e780..0x00a4e8f0 (1 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/xrg_main_font.h"


const char mainFontAssertStringNotNil[24] = "pszString != NIL";
const char mainFontSourcePath[32] = "../xrg_main_font.euc.c";

/* Prints pszString at (x, y) behind a fixed font command prefix; bright selects the prefix colour bytes. */
void XrgDispMainFont(int x, int y, const char *pszString, int bright)
{
    char font_text[128];
    char *font_cursor;

    if (pszString == 0) {
        assert_prog(mainFontAssertStringNotNil, mainFontSourcePath, 17);
    }

    /* xglFontPrint receives this fixed command prefix followed by the text. */
    font_text[0] = 13;
    font_text[1] = 2;
    font_cursor = &font_text[2];
    /* The bright flag selects the colour bytes at offsets 3 to 5 of this prefix. */
    if (bright != 0) {
        *font_cursor = 12;
        font_cursor = &font_text[6];
        font_text[3] = -128;
        font_text[4] = 32;
        font_text[5] = 32;
    } else {
        *font_cursor = 12;
        font_cursor = &font_text[6];
        font_text[3] = -128;
        font_text[4] = -128;
        font_text[5] = -128;
    }

    *font_cursor++ = 14;
    *font_cursor++ = 2;
    *font_cursor++ = 2;
    *font_cursor++ = 0;
    *font_cursor++ = 0;
    *font_cursor++ = 0;
    *font_cursor++ = 25;
    *font_cursor++ = 3;
    *font_cursor++ = 21;
    *font_cursor++ = 2;
    *font_cursor++ = 4;
    *font_cursor++ = 0;
    *font_cursor++ = 21;
    *font_cursor++ = 2;
    *font_cursor++ = 2;
    *font_cursor = 4;

    strcpy(font_cursor + 1, pszString);
    xglFontPrint(x, y, 0xfff0, font_text);
}
