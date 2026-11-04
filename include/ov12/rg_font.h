#ifndef INCLUDE_OV12_RG_FONT_H
#define INCLUDE_OV12_RG_FONT_H

#include "shared.h"
#include "ov12/xrg_paint2d.h"

struct RgBxxPic;

typedef XrgPaint2DRGBA RgFontColor;

typedef struct RgFontGlyphSize {
    short width;
    short height;
} RgFontGlyphSize;

/* One 14-byte entry in the font's glyph table. */
typedef struct RgFontGlyph {
    signed char character;
    unsigned char unmodeled_01;
    short texture_u;
    short texture_v;
    RgFontGlyphSize size;
    short y_offset;
    short advance;
} RgFontGlyph;

/*
 * A bitmap font: its picture, the glyph table and the current draw color.
 * DisposeRgFont only passes the pointer through to RgHeapFree and
 * RgFontSetColor only touches the color, so the 4 bytes at +0x0c stay an
 * unmodeled range.
 */
typedef struct RgFont {
    struct RgBxxPic *picture;
    RgFontGlyph *glyphs;
    int glyph_count;
    unsigned char unmodeled_0c[4];
    RgFontColor color; /* +0x10 */
} RgFont;

/* Functions and data used by other translation units. */
RgFont *CreateRgFont(RgFontGlyph *glyph_table, int glyph_count,
                     struct RgBxxPic *picture);
void DisposeRgFont(RgFont *pFont);
void RgFontSetColor(RgFont *pFont, const RgFontColor *color);
void RgFontSetDefaultColor(RgFont *pFont);
int RgFontPut(XrgPaint2D *paint, RgFont *font, signed char character, int x,
              int y, int draw);
int RgFontStr(XrgPaint2D *paint, RgFont *font, const char *text, int x, int y,
              int right_align);

#endif /* INCLUDE_OV12_RG_FONT_H */
