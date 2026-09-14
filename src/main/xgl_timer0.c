#include "common.h"
#include "shared.h"
#include "xgl_timer0.h"

/*
 * EE Timer 0.  T0_COUNT is the free-running counter and T0_MODE its control
 * word; bit 7 (0x80) of T0_MODE is CUE, which starts the count.
 */
#define T0_COUNT ((volatile u32 *)0x10000000)
#define T0_MODE  ((volatile u32 *)0x10000010)
#define T0_MODE_CUE 128

void xglTimer0Reset(int mode)
{
    *T0_COUNT = 0;
    *T0_MODE = mode + T0_MODE_CUE;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_timer0", xglTimer0Get);

INCLUDE_ASM("asm/main/nonmatchings/xgl_timer0", getbit1);

INCLUDE_ASM("asm/main/nonmatchings/xgl_timer0", getbits);

INCLUDE_ASM("asm/main/nonmatchings/xgl_timer0", xglArxExtract);
