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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_font", CreateRgFont);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_font", RgFontStr);
