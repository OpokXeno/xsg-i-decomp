/*
 * OV12 original TU 93: 0x00a4e780..0x00a4e8f0 (1 functions)
 */
#include "common.h"
#include "shared.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern char *strcpy(char *destination, const char *source);

extern const char D_00A59218[];
extern const char D_00A59230[];

void XrgDispMainFont(int x, int y, const char *pszString, int bright)
{
    char font_text[128];
    char *font_cursor;

    if (pszString == 0) {
        assert_prog(D_00A59218, D_00A59230, 17);
    }

    /* xglFontPrint receives this fixed command prefix followed by the text. */
    font_text[0] = 13;
    font_text[1] = 2;
    font_cursor = &font_text[2];
    /* The mode selects one of the two byte pairs in this prefix. */
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
