/*
 * OV12 original TU 72: 0x00a3e518..0x00a3ea10 (6 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_font.h"
#include "ov12/xrg_paint2d.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"

/* Default draw color of a font: RgFontColor components. */
#define RG_FONT_DEFAULT_RGB 0x80
#define RG_FONT_DEFAULT_ALPHA 0x7F

const char fontSourcePath[] = "../rg_font.euc.c";

const char fontAssertFontTableNotNil[] = "pFontTable != NIL";

const char fontAssertPicNotNil[] = "pPic != NIL";

const char fontAssertFontNotNil[] = "pFont != NIL";

const char fontAssertPaintNotNil[] = "pPaint != NIL";

const char fontAssertStringNotNil[] = "pszStr != NIL";

/* Creates a font over a glyph table and its picture, with the default color. */
RgFont *CreateRgFont(RgFontGlyph *glyph_table, int glyph_count,
                     RgBxxPic *picture)
{
    RgFont *font;

    font = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgFont), fontSourcePath, 24);
    if (glyph_table == 0) {
        assert_prog(fontAssertFontTableNotNil, fontSourcePath, 25);
    }
    if (picture == 0) {
        assert_prog(fontAssertPicNotNil, fontSourcePath, 26);
    }
    font->picture = picture;
    font->glyphs = glyph_table;
    font->glyph_count = glyph_count;
    font->color.r = RG_FONT_DEFAULT_RGB;
    font->color.g = RG_FONT_DEFAULT_RGB;
    font->color.b = RG_FONT_DEFAULT_RGB;
    font->color.a = RG_FONT_DEFAULT_ALPHA;
    return font;
}

void DisposeRgFont(RgFont *pFont)
{
    if (pFont == 0)
        assert_prog(fontAssertFontNotNil, fontSourcePath, 39);
    RgHeapFree(InstanceOfRgHeap(), pFont, fontSourcePath, 40);
}

void RgFontSetColor(RgFont *pFont, const RgFontColor *color)
{
    if (pFont == 0)
        assert_prog(fontAssertFontNotNil, fontSourcePath, 50);
    pFont->color.r = color->r;
    pFont->color.g = color->g;
    pFont->color.b = color->b;
    pFont->color.a = color->a;
}

void RgFontSetDefaultColor(RgFont *pFont)
{
    if (pFont == 0)
        assert_prog(fontAssertFontNotNil, fontSourcePath, 61);
    pFont->color.r = RG_FONT_DEFAULT_RGB;
    pFont->color.g = RG_FONT_DEFAULT_RGB;
    pFont->color.b = RG_FONT_DEFAULT_RGB;
    pFont->color.a = RG_FONT_DEFAULT_ALPHA;
}

/*
 * Draws the glyph of character at (x, y) when draw is set, except for spaces, and
 * returns its advance. Returns 0 for a character the font does not have.
 */
int RgFontPut(XrgPaint2D *paint, RgFont *font, signed char character, int x,
              int y, int draw)
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
    RgBxxPic *texture;

    if (paint == 0)
        assert_prog(fontAssertPaintNotNil, fontSourcePath, 79);
    if (font == 0)
        assert_prog(fontAssertFontNotNil, fontSourcePath, 80);
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
                        assert_prog(fontAssertPicNotNil, fontSourcePath, 92);
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

/*
 * Draws text starting at (x, y). With right_align set, the text is measured first
 * and placed so that it ends at x. Returns the x position after the last character.
 */
int RgFontStr(XrgPaint2D *paint, RgFont *font, const char *text, int x, int y,
              int right_align)
{
    const char *string_start;
    int text_width;

    if (paint == 0) {
        assert_prog(fontAssertPaintNotNil, fontSourcePath, 114);
    }
    if (font == 0) {
        assert_prog(fontAssertFontNotNil, fontSourcePath, 115);
    }
    if (text == 0) {
        assert_prog(fontAssertStringNotNil, fontSourcePath, 116);
    }
    if (right_align != 0) {
        string_start = text;
        text_width = 0;
        while (*text != '\0') {
            text_width += RgFontPut(paint, font, *text, 0, 0, 0);
            text++;
        }
        text = string_start;
        x -= text_width;
    }
    while (*text != '\0') {
        x += RgFontPut(paint, font, *text, x, y, 1);
        text++;
    }
    return x;
}
