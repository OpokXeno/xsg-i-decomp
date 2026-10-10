/*
 * OV12 original TU 31: 0x00a1f7e0..0x00a1fa30 (6 functions)
 */
#include "common.h"
#include "rg_dull_flag.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);


void InitRgDullFlag(RgDullFlag *pDull)
{
    if (pDull == 0) {
        assert_prog("pDull != NIL", "../rg_dull_flag.euc.c", 14);
    }
    pDull->time = 0.0f;
    pDull->dullTime = 0.0f;
}

/* The assertion text "fTime >= RG_FCONST(0.0)". */


void RgDullFlagSetDullTime(RgDullFlag *pDull, float fTime)
{
    if (pDull == 0) {
        assert_prog("pDull != NIL", "../rg_dull_flag.euc.c", 25);
    }
    if (!(fTime >= 0.0f)) {
        assert_prog("fTime >= RG_FCONST(0.0)", "../rg_dull_flag.euc.c", 26);
    }
    pDull->dullTime = fTime;
}

void RgDullFlagSetFlag(RgDullFlag *pDull)
{
    if (pDull == 0) {
        assert_prog("pDull != NIL", "../rg_dull_flag.euc.c", 33);
    }
    pDull->time = pDull->dullTime;
}

void RgDullFlagResetFlag(RgDullFlag *pDull)
{
    if (pDull == 0) {
        assert_prog("pDull != NIL", "../rg_dull_flag.euc.c", 40);
    }
    pDull->time = 0.0f;
}

int RgDullFlagGet(RgDullFlag *pDull)
{
    if (pDull == 0) {
        assert_prog("pDull != NIL", "../rg_dull_flag.euc.c", 50);
    }
    if (pDull->time <= 0.0f) {
        return 0;
    }
    return 1;
}


void RgDullFlagPassTime(RgDullFlag *pDull, float fTime)
{
    if (pDull == 0) {
        assert_prog("pDull != NIL", "../rg_dull_flag.euc.c", 60);
    }
    if (!(fTime > 0.0f)) {
        assert_prog("fTime > RG_FCONST(0.0)", "../rg_dull_flag.euc.c", 61);
    }
    pDull->time -= fTime;
    if (pDull->time <= 0.0f) {
        pDull->time = 0.0f;
    }
}
