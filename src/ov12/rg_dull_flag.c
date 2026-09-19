/*
 * OV12 original TU 31: 0x00a1f7e0..0x00a1fa30 (6 functions)
 */
#include "common.h"
#include "rg_dull_flag.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

/* Referenced by every accessor below: the assertion text "pDull != NIL" and
   this TU's own original file name "../rg_dull_flag.euc.c", both
   scaffold-owned .rodata. */
extern const char D_00A54070[];
extern const char D_00A54080[];

void InitRgDullFlag(RgDullFlag *pDull)
{
    if (pDull == 0) {
        assert_prog(D_00A54070, D_00A54080, 14);
    }
    pDull->time = 0.0f;
    pDull->dullTime = 0.0f;
}

/* The assertion text "fTime >= RG_FCONST(0.0)". */
extern const char D_00A54098[];

void RgDullFlagSetDullTime(RgDullFlag *pDull, float fTime)
{
    if (pDull == 0) {
        assert_prog(D_00A54070, D_00A54080, 25);
    }
    if (!(fTime >= 0.0f)) {
        assert_prog(D_00A54098, D_00A54080, 26);
    }
    pDull->dullTime = fTime;
}

void RgDullFlagSetFlag(RgDullFlag *pDull)
{
    if (pDull == 0) {
        assert_prog(D_00A54070, D_00A54080, 33);
    }
    pDull->time = pDull->dullTime;
}

void RgDullFlagResetFlag(RgDullFlag *pDull)
{
    if (pDull == 0) {
        assert_prog(D_00A54070, D_00A54080, 40);
    }
    pDull->time = 0.0f;
}

int RgDullFlagGet(RgDullFlag *pDull)
{
    if (pDull == 0) {
        assert_prog(D_00A54070, D_00A54080, 50);
    }
    if (pDull->time <= 0.0f) {
        return 0;
    }
    return 1;
}

/* The assertion text "fTime > RG_FCONST(0.0)". */
extern const char D_00A540B0[];

void RgDullFlagPassTime(RgDullFlag *pDull, float fTime)
{
    if (pDull == 0) {
        assert_prog(D_00A54070, D_00A54080, 60);
    }
    if (!(fTime > 0.0f)) {
        assert_prog(D_00A540B0, D_00A54080, 61);
    }
    pDull->time -= fTime;
    if (pDull->time <= 0.0f) {
        pDull->time = 0.0f;
    }
}
