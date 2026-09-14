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

INCLUDE_ASM("asm/main/nonmatchings/xgl_mc", xglMcEUC2SJIS);

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
