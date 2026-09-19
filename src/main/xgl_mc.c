#include "common.h"
#include "shared.h"
#include "xgl_mc.h"

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", xglMcRequest);

/*
 * The word xglMcGetState reports lives at mw+4; mw+0 is the byte the request
 * state machine of xglMcMain switches on (seven states, jump table at
 * 0x004D2560) and xglMcMain writes the halfwords of the same +4 window per
 * slot (`sh` through `mw + 4 + 2 * mw[8]`, 0x00220600..0x0022069C).  The rest
 * of the record is still assembly, so mw cannot become a struct without
 * inventing the bytes between its members (docs/naming.md); the one access
 * takes docs/style.md rule 2's named-offset fallback.
 */
#define XGL_MC_STATE_OFFSET 4

int xglMcGetState(void)
{
    return *(int *)(mw + XGL_MC_STATE_OFFSET);
}

void xglMcReset(void)
{
    queue_top = 0;
    queue_end = 0;
    xglMcSetMapName(0, 0);
    mw[0] = 0;
}

/*
 * Converts one EUC-JIS double-byte character to Shift-JIS in place: hi and
 * lo hold the character's lead and trail byte, EUC-encoded on entry and
 * Shift-JIS on return. Called from xglMcSetMapName while building a memory
 * card map name.
 */
static void xglMcEUC2SJIS(u8 *hi, u8 *lo)
{
    u32 hi_byte;
    int lo_byte;
    u8 sjis_hi;
    u8 sjis_lo;

    hi_byte = ((*hi) + 0x80) & 0xFF;
    lo_byte = ((*lo) + 0x80) & 0xFF;
    sjis_hi = hi_byte;
    if (sjis_hi & 1)
    {
        lo_byte = lo_byte + 0x1F;
        sjis_hi = (sjis_hi >> 1) + 0x71;
    }
    else
    {
        lo_byte = lo_byte + 0x7D;
        sjis_hi = (sjis_hi >> 1) + 0x70;
    }
    sjis_lo = lo_byte & 0xFF;
    if (sjis_hi >= 0xA0U)
    {
        sjis_hi = (sjis_hi + 0x40) & 0xFF;
    }
    if (sjis_lo >= 0x7FU)
    {
        sjis_lo = (sjis_lo + 1) & 0xFF;
    }
    if ((sjis_hi == 0x87) && (sjis_lo == 0x54))
    {
        sjis_hi = 0x82;
        sjis_lo = 0x50;
    }
    *hi = sjis_hi;
    *lo = sjis_lo;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", xglMcSetMapName);

void xglMcWriteMapName(char *buffer, int slot)
{
    int value = slot + 1;
    int digit = value % 10;

    buffer[229] = digit + 'O';
    value /= 10;
    buffer[227] = value % 10 + 'O';
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", xglMcSetFullPath);

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", execute);

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", create_sub2);

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", xglMcMain);

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", xglMcEasyLoad);

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", xglMcEasySave);

void xglMcInitial(void)
{
    sceMcInit();
    xglMcReset();
}
