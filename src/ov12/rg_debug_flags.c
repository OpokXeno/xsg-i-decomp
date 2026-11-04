/*
 * OV12 original TU 12: 0x00a0f340..0x00a0f490 (4 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_debug_flags.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_singleton_id.h"

/* Assertion expression and source location used by the guards below. */
const char debugFlagsAssertFlagsNotNil[16] = "pFlags != NIL";
const char debugFlagsSourcePath[24] = "../rg_debug_flags.euc.c";


static void _InitRgDebugFlags(RgDebugFlags *flags)
{
    if (flags == 0) {
        assert_prog(debugFlagsAssertFlagsNotNil, debugFlagsSourcePath, 16);
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
        assert_prog(debugFlagsAssertFlagsNotNil, debugFlagsSourcePath, 27);
    }
}

static void _WrapperDestruct(void *pFlags)
{
    _DestructRgDebugFlags(pFlags);
    RgHeapFree(InstanceOfRgHeap(), pFlags, debugFlagsSourcePath, 29);
}

RgDebugFlags *InstanceOfRgDebugFlags(void)
{
    RgDebugFlags *pFlags;

    pFlags = RgSingletonIDGet(RG_SINGLETON_DEBUG_FLAGS);
    if (pFlags == 0) {
        pFlags = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgDebugFlags),
                             debugFlagsSourcePath, 39);
        _InitRgDebugFlags(pFlags);
        RgSingletonIDEntry(RG_SINGLETON_DEBUG_FLAGS, pFlags, _WrapperDestruct);
    }
    return pFlags;
}
