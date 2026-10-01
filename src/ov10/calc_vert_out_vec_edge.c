/*
 * OV10 original TU 0: 0x00a00000..0x00a00410 (4 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov10/cgp.h"

extern int s_nDebMode;
extern int s_nMode;
extern CardGameWork D_00A4FA10;
extern char D_00A4BBA0[];
extern char D_00A4BBB0[];
extern void xglRenderClearFrame(void);
extern void xglRenderGlobalFadeInit(void);
extern void xglSleep(void);
extern void xglSoundLoadEffect(const char *bank_name, void *buffer, int mode);
extern int CardMainInit(CardGameWork *work);
extern int CardMainProc(CardGameWork *work);
extern int printf(const char *format, ...);

INCLUDE_ASM("asm/nonmatchings/ov10/calc_vert_out_vec_edge", CalcVertOutVecEdge);

INCLUDE_ASM("asm/nonmatchings/ov10/calc_vert_out_vec_edge", yNewRenderInit);

extern void yNewRenderInit(void);

void CardGameRoot(void)
{
    xglRenderClearFrame();
    s_nMode = 1;
    yNewRenderInit();
    s_nMode = 1;
    s_nDebMode = 0;
    xglRenderGlobalFadeInit();
    xglSoundLoadEffect(D_00A4BBA0, (void *)0x01B00000, 2);
    if (CardMainInit(&D_00A4FA10) < 0)
        printf(D_00A4BBB0);

    while (CardMainProc(&D_00A4FA10))
        xglSleep();

    xglSoundLoadEffect(0, 0, 1);
    xglSoundLoadEffect(0, 0, 2);
    xglRenderGlobalFadeInit();
}

void CardGameRoot(void);

void YmineTest(void) {
    CardGameRoot();
}
