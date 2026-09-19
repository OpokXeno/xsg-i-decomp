#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWSYS_init);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWSYS_setGRPStatus);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWSYS_getGRPStatus);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWSYS_createComponent);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWSYS_update);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWSYS_draw);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", PARSE_int);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", STRING_int);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", STRING_h2zEUC);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", STRING_toUInt);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TW_setPos);

/*
 * The text-window record TWSYS_createComponent hands back (still assembler
 * in this TU). Only the fields this allocation and its evidenced siblings
 * touch are modeled:
 *  - +0xC/+0xE width/height: TW_setPos (0x0025d660) reads both as unsigned
 *    halfwords for its on-screen centering math.
 *  - +0xA6 line_pitch: TWIN_drawScene2 (0x0025ec24, still assembler)
 *    multiplies the current line index (+0x185) by this halfword to place
 *    each text line, so it is the per-line vertical advance.
 *  - +0x186 layout_offset: set here and by TWIN_create2's own default
 *    (still assembler); no function recovered so far in this TU reads it
 *    back, so only that it is a byte is proven.
 *  - +0x187 line_count: TWIN_update2 and TWIN_popCF/TWIN_popScene (still
 *    assembler) loop up to this byte over the +0x17C line-pointer array.
 */
typedef struct TWindow {
    unsigned char unmodeled_00[0xC];
    unsigned short width;          /* +0xC */
    unsigned short height;         /* +0xE */
    unsigned char unmodeled_10[0xA6 - 0x10];
    short line_pitch;              /* +0xA6 */
    unsigned char unmodeled_a8[0x186 - 0xA8];
    unsigned char layout_offset;   /* +0x186 */
    unsigned char line_count;      /* +0x187 */
} TWindow;

/* Configures a field-conversation text window's line pitch, layout offset,
 * line count and on-screen size. */
void WIN_initCF(TWindow *window)
{
    window->line_pitch = 0x18;
    window->layout_offset = 0x2A;
    window->line_count = 3;
    window->width = 0x1CC;
    window->height = 0x50;
}

/* Configures a scene-message text window's line pitch, layout offset, line
 * count and on-screen size. */
void WIN_initScene(TWindow *window)
{
    window->line_pitch = 0x1A;
    window->layout_offset = 0x30;
    window->line_count = 3;
    window->width = 0x1E0;
    window->height = 0x56;
}

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_init2);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", WIN_checkActiveWindow);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_create2);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_dispose);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_popCF);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_popScene);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_update2);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_draw2);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TWIN_drawScene2);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TMENU_init);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TMENU_create);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TMENU_dispose);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TMENU_addQuery);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TMENU_setItem);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TMENU_addItem);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TMENU_updateDefault);

INCLUDE_ASM("asm/main/nonmatchings/twsys_init", TMENU_drawDefault);
