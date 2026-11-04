/*
 * OV12 original TU 72: 0x00a3e518..0x00a3ea10 (6 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_font.h"

const char D_00A56F20[] = "../rg_font.euc.c";

const char D_00A56F38[] = "pFontTable != NIL";

const char D_00A56F50[] = "pPic != NIL";

const char D_00A56F60[] = "pFont != NIL";

const char D_00A56F70[] = "pPaint != NIL";

const char D_00A56F80[] = "pszStr != NIL";

/*
 * These are external file-backed witnesses, not candidate-emitted data.
 * Neither address has an entry in config/symbols/ov12.txt, so this
 * allocation keeps the splat default names rather than inventing new ones.
 */
extern const char D_00A56F20[];
extern const char D_00A56F60[];
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
int RgFontPut(void *paint, RgFont *font, signed char character, int x, int y, int draw);

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

extern void XrgPaint2DUseTexture(void *paint, int texture);
extern void XrgPaint2DAlpha(void *paint, int blend);
extern void XrgPaint2DSetUVOffset(void *paint, int u, int v);
extern void XrgPaint2DSetUVSize(void *paint, int width, int height);
extern void XrgPaint2DColor(void *paint, const RgFontColor *color);
extern void XrgPaint2DDrawXYWH(void *paint, int mode, int x, int y, int width,
                               int height);

int RgFontPut(void *paint, RgFont *font, signed char character, int x, int y,
              int draw)
{
    RgFontGlyph *glyph;
    unsigned int glyph_count;
    unsigned int glyph_index;
    int advance;
    int texture_u;
    int texture_v;
    int width;
    int height;
    int y_offset;
    int texture;

    if (paint == 0)
        assert_prog(D_00A56F70, D_00A56F20, 79);
    if (font == 0)
        assert_prog(D_00A56F60, D_00A56F20, 80);
    glyph_count = font->glyph_count;
    glyph_index = 0;
    if (glyph_count != 0) {
        glyph = font->glyphs;
        do {
            if (glyph->character == character) {
                texture_u = glyph->texture_u;
                texture_v = glyph->texture_v;
                width = glyph->size.width;
                height = glyph->size.height;
                y_offset = glyph->y_offset;
                advance = glyph->advance;
                if (character != ' ' && draw != 0) {
                    texture = font->picture;
                    if (texture == 0)
                        assert_prog(D_00A56F50, D_00A56F20, 92);
                    XrgPaint2DUseTexture(paint, texture);
                    XrgPaint2DAlpha(paint, 0);
                    XrgPaint2DSetUVOffset(paint, texture_u, texture_v);
                    XrgPaint2DSetUVSize(paint, width, height);
                    XrgPaint2DColor(paint, &font->color);
                    XrgPaint2DDrawXYWH(paint, 0, x, y + y_offset, width, height);
                    if (advance < 1)
                        advance += width;
                }
                return advance;
            }
            glyph_index++;
            glyph++;
        } while (glyph_index < glyph_count);
    }
    return 0;
}

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
