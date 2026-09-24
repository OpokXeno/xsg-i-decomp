/*
 * OV12 original TU 90: 0x00a4e1f0..0x00a4e340 (8 functions)
 */
#include "common.h"

/* The EE GCC argument pointer is a plain byte pointer: XrgLog and XrgOut hand
 * the address of their spilled register arguments down as it. */
typedef char *va_list;

/*
 * The release build's output sinks: XrgVLog/XrgVOut pass them the format and
 * argument pointer (XrgVLog moves its fourth argument, the va_list, into the
 * second slot), and they print nothing.
 */
static void _vout(const char *format, va_list args)
{
}

static void _vout_sys(const char *format, va_list args)
{
}

void XrgVLog(const char *format, const char *source_file, int line, va_list args)
{
    _vout(format, args);
}

void XrgVLogSys(const char *format, const char *source_file, int line, va_list args)
{
    _vout_sys(format, args);
}

void XrgVOut(const char *format, va_list args)
{
    _vout(format, args);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_v_log", XrgLog);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_v_log", XrgLogSys);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_v_log", XrgOut);
