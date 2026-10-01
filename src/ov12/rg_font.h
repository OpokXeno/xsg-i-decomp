/*
 * TU-local declarations of ov12/tu072 (src/ov12/rg_font.c).
 */

#ifndef SRC_OV12_RG_FONT_H
#define SRC_OV12_RG_FONT_H

typedef struct RgFontColor RgFontColor;

/*
 * The four-component color RgFontSetColor (ov12:0x00a3e650) copies into a
 * font: plain lw/sw words, never lwc1/swc1, so the components are integers,
 * not floats.
 */
struct RgFontColor {
    int r;
    int g;
    int b;
    int a;
};

typedef struct RgFont RgFont;

typedef struct RgFontGlyph RgFontGlyph;
typedef struct RgFontGlyphSize RgFontGlyphSize;

struct RgFontGlyphSize {
    short width;
    short height;
};

/* One 14-byte entry in the font's glyph table. */
struct RgFontGlyph {
    signed char character;
    unsigned char unmodeled_01;
    short texture_u;
    short texture_v;
    RgFontGlyphSize size;
    short y_offset;
    short advance;
};

/*
 * DisposeRgFont (ov12:0x00a3e5e8) only passes the pointer through to
 * RgHeapFree; RgFontSetColor (ov12:0x00a3e650) is the sole function of this
 * allocation that dereferences it, and it only ever touches the color field
 * at offset 0x10. Nothing before it is read or written by either function,
 * so the span stays an explicit unmodeled range rather than a guessed field.
 */
struct RgFont {
    int picture;
    RgFontGlyph *glyphs;
    int glyph_count;
    unsigned char unmodeled_0c[4];
    RgFontColor color; /* 0x10 */
};

#endif /* SRC_OV12_RG_FONT_H */
