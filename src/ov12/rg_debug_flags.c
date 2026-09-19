/*
 * OV12 original TU 12: 0x00a0f340..0x00a0f490 (4 functions)
 */
#include "common.h"
#include "rg_debug_flags.h"

/*
 * ov12:0x00a52438 "pFlags != NIL" (the assert_prog expression text) and
 * ov12:0x00a52448 "../rg_debug_flags.euc.c" (the assert_prog source-file
 * text) are asm-owned .rodata (config/tu/ov12/tu012.json data_ownership).
 */
extern const char D_00A52438[];
extern const char D_00A52448[];

static void _InitRgDebugFlags(RgDebugFlags *flags)
{
    if (flags == 0) {
        assert_prog(D_00A52438, D_00A52448, 16);
    }
    flags->modeEnabled = 0;
    flags->flags[0] = 1;
    flags->flags[1] = 1;
    flags->flags[2] = 1;
    flags->flags[3] = 1;
    flags->flags[4] = 1;
}

static void _DestructRgDebugFlags(RgDebugFlags *flags)
{
    if (flags == 0) {
        assert_prog(D_00A52438, D_00A52448, 27);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_debug_flags", _WrapperDestruct_00A0F3D8);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_debug_flags", InstanceOfRgDebugFlags);
