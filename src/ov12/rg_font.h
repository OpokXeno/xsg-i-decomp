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

/*
 * DisposeRgFont (ov12:0x00a3e5e8) only passes the pointer through to
 * RgHeapFree; RgFontSetColor (ov12:0x00a3e650) is the sole function of this
 * allocation that dereferences it, and it only ever touches the color field
 * at offset 0x10. Nothing before it is read or written by either function,
 * so the span stays an explicit unmodeled range rather than a guessed field.
 */
struct RgFont {
    unsigned char unmodeled_00[0x10];
    RgFontColor color; /* 0x10 */
};

#endif /* SRC_OV12_RG_FONT_H */
