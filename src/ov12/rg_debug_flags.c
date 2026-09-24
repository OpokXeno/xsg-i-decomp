/*
 * OV12 original TU 12: 0x00a0f340..0x00a0f490 (4 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_singleton_id.h"
#include "rg_debug_flags.h"

/*
 * ov12:0x00a52438 "pFlags != NIL" (the assert_prog expression text) and
 * ov12:0x00a52448 "../rg_debug_flags.euc.c" (the assert_prog source-file
 * text) are asm-owned .rodata (config/tu/ov12/tu012.json data_ownership).
 */
extern const char D_00A52438[];
extern const char D_00A52448[];

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *pointer, const char *source_file,
                       int line);

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

static void _WrapperDestruct(RgDebugFlags *pFlags)
{
    _DestructRgDebugFlags(pFlags);
    RgHeapFree(InstanceOfRgHeap(), pFlags, D_00A52448, 29);
}

RgDebugFlags *InstanceOfRgDebugFlags(void)
{
    RgDebugFlags *pFlags;

    pFlags = RgSingletonIDGet(13);
    if (pFlags == 0) {
        pFlags = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgDebugFlags),
                             D_00A52448, 39);
        _InitRgDebugFlags(pFlags);
        RgSingletonIDEntry(13, (RgSimpleDB *) pFlags,
                           (void (*)(RgSimpleDB *)) _WrapperDestruct);
    }
    return pFlags;
}
