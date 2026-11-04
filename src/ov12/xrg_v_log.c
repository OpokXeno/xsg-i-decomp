/*
 * OV12 original TU 90: 0x00a4e1f0..0x00a4e340 (8 functions)
 */
#include "common.h"
#include "ov12/xrg_v_log.h"

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

void XrgLog(const char *format, const char *source_file, int line, ...)
{
    va_list args;

    va_start(args, line);
    XrgVLog(format, source_file, line, args);
    va_end(args);
}

void XrgLogSys(const char *format, const char *source_file, int line, ...)
{
    va_list args;

    va_start(args, line);
    XrgVLogSys(format, source_file, line, args);
    va_end(args);
}

void XrgOut(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    XrgVOut(format, args);
    va_end(args);
}
