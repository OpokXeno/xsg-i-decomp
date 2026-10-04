#ifndef INCLUDE_OV12_RG_FONT_H
#define INCLUDE_OV12_RG_FONT_H

typedef struct RgFontGlyph RgFontGlyph;

typedef struct RgFontGlyphSize RgFontGlyphSize;

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

#endif /* INCLUDE_OV12_RG_FONT_H */
