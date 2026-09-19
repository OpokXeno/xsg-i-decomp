/*
 * OV02 original TU 5: 0x00a023a8..0x00a02d68 (7 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_cd.h"

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_object_task_main", tskUmnObjectTaskMain);

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_object_task_main", UmnObjectTaskCreate);

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_object_task_main", UmnChangeTopLevel);

/* UmnTextLoad is ov02:0x00a0f578 (src/ov02/umn_event_text_symbol_check.c). */
extern int UmnTextLoad(int work, int mode);

/* "data\\endou\\umn\\cube.xtx", "data\\endou\\umn\\cube.lex" and
 * "data\\endou\\umn\\umn00.xtx" filename buffers loaded below. */
extern const char D_00A11D40[];
extern const char D_00A11D58[];
extern const char D_00A11D70[];

extern int UmnBgCubeXtx;
extern int UmnBgCubeLex;
extern int UmnTexAddr;
extern int UmnWorkEnd;

static void UmnFirstLoad(void) {
    UmnWorkEnd = UmnTextLoad(UmnWorkEnd, 0);
    xglCdReadFile(D_00A11D40, (void *)UmnBgCubeXtx, 0, 1);
    xglCdReadFile(D_00A11D58, (void *)UmnBgCubeLex, 0, 1);
    xglCdReadFile(D_00A11D70, (void *)UmnTexAddr, 0, 1);
}

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_object_task_main", UmnInit);

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_object_task_main", UmnMain);

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_object_task_main", UmnMain2);
