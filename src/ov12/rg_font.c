/*
 * OV12 original TU 72: 0x00a3e518..0x00a3ea10 (6 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_font.h"

/*
 * These are external file-backed witnesses, not candidate-emitted data.
 * Neither address has an entry in config/symbols/ov12.txt, so this
 * allocation keeps the splat default names rather than inventing new ones.
 */
extern const char D_00A56F20[]; /* "../rg_font.euc.c" */
extern const char D_00A56F60[]; /* "pFont != NIL" */

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void RgHeapFree(RgHeap *heap, void *pointer, const char *source_file,
                       int line);

extern const char D_00A56F38[];
extern const char D_00A56F50[];
extern const char D_00A56F70[];
extern const char D_00A56F80[];
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern int RgFontPut(void *paint, RgFont *font, signed char character,
                     int x, int y, int draw);

int CreateRgFont(void *glyph_table, int glyph_count, int picture)
{
    RgFont *font;

    font = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgFont), D_00A56F20, 24);
    if (glyph_table == 0) {
        assert_prog(D_00A56F38, D_00A56F20, 25);
    }
    if (picture == 0) {
        assert_prog(D_00A56F50, D_00A56F20, 26);
    }
    font->picture = picture;
    font->glyphs = glyph_table;
    font->glyph_count = glyph_count;
    font->color.r = 0x80;
    font->color.g = 0x80;
    font->color.b = 0x80;
    font->color.a = 0x7F;
    return (int)font;
}

void DisposeRgFont(RgFont *pFont)
{
    if (pFont == 0)
        assert_prog(D_00A56F60, D_00A56F20, 39);
    RgHeapFree(InstanceOfRgHeap(), pFont, D_00A56F20, 40);
}

void RgFontSetColor(RgFont *pFont, const RgFontColor *color)
{
    if (pFont == 0)
        assert_prog(D_00A56F60, D_00A56F20, 50);
    pFont->color.r = color->r;
    pFont->color.g = color->g;
    pFont->color.b = color->b;
    pFont->color.a = color->a;
}

void RgFontSetDefaultColor(RgFont *pFont)
{
    if (pFont == 0)
        assert_prog(D_00A56F60, D_00A56F20, 61);
    pFont->color.r = 0x80;
    pFont->color.g = 0x80;
    pFont->color.b = 0x80;
    pFont->color.a = 0x7F;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_font", RgFontPut);

int RgFontStr(void *paint, int font_id, const char *text, int x, int y,
              int right_align)
{
    const char *string_start;
    int text_width;

    if (paint == 0) {
        assert_prog(D_00A56F70, D_00A56F20, 114);
    }
    if (font_id == 0) {
        assert_prog(D_00A56F60, D_00A56F20, 115);
    }
    if (text == 0) {
        assert_prog(D_00A56F80, D_00A56F20, 116);
    }
    if (right_align != 0) {
        string_start = text;
        text_width = 0;
        while (*text != '\0') {
            text_width += RgFontPut(paint, (RgFont *)(unsigned int)font_id,
                                    *text, 0, 0, 0);
            text++;
        }
        text = string_start;
        x -= text_width;
    }
    while (*text != '\0') {
        x += RgFontPut(paint, (RgFont *)(unsigned int)font_id, *text, x, y, 1);
        text++;
    }
    return x;
}
