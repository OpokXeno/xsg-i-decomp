/*
 * OV02 original TU 0: 0x00a00000..0x00a00850 (6 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_thread.h"

extern int xglMcMain(void);
extern void xglFontPrintDirectOT(int color, void *param);
extern int D_00A10D28;

static int device_check_sync(void)
{
    int status;

    status = xglMcMain();
    xglSleep();
    xglFontPrintDirectOT(-1, &D_00A10D28);
    return status;
}

INCLUDE_ASM("asm/nonmatchings/ov02/logo", hdd_check);

INCLUDE_ASM("asm/nonmatchings/ov02/logo", mc_check);

INCLUDE_ASM("asm/nonmatchings/ov02/logo", LogoFirst);

INCLUDE_ASM("asm/nonmatchings/ov02/logo", ipuplay);

extern int ipuplay(char *path);
extern void xglRenderClearColor(u32 color);
extern void xglRenderClearDepth(void);
extern void xglRenderClearFrame(void);
extern char D_00A115A8[];
extern char D_00A115B8[];

void Logo(void)
{
    xglRenderClearFrame();
    xglRenderClearColor(0x80000000);
    xglSleep();
    xglRenderClearDepth();
    if (ipuplay(D_00A115A8) == 0)
    {
        /*
         * The original calls ipuplay with jal and returns through the
         * shared epilogue instead of a sibling jump. Under this TU's
         * compiler the call stays out of tail position only inside a
         * loop construct, which is the shape a do/while (0) statement
         * gives it; the call alone as the block's last statement
         * compiles to `j ipuplay`.
         */
        do {
            ipuplay(D_00A115B8);
        } while (0);
    }
}
