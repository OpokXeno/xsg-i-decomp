/*
 * OV12 original TU 61: 0x00a31768..0x00a31970 (2 functions)
 */
#include "common.h"
#include "ov12/rg_error.h"
#include "ov12/xrg_exit.h"
#include "ov12/xrg_v_log.h"

const char errorSeparatorLine[] = "------------------------------------------\n";
const char errorSourcePath[] = "../rg_error.euc.c";
const char errorFatalBanner[] = "RG : FATAL-ERROR\n";
const char errorFatalLocationFormat[] = "\n\nerror in %s at %d\n";

/*
 * Prints a fatal error banner, the formatted message and the source location,
 * then exits with status 1.
 */
void RgError(const char *format, const char *source_file, int line, ...)
{
    va_list ap;
    va_start(ap, line);

    XrgLog(errorSeparatorLine, errorSourcePath, 18);
    XrgLog(errorFatalBanner, errorSourcePath, 19);
    XrgVLog(format, errorSourcePath, 20, ap);
    XrgLog(errorFatalLocationFormat, errorSourcePath, 21, source_file, line);
    XrgLog(errorSeparatorLine, errorSourcePath, 22);

    va_end(ap);

    XrgExit(1);
}

const char errorWarningBanner[] = "RG : WARNING!\n";
const char errorWarningLocationFormat[] = "\n\nwarning in %s at %d\n";

/* Prints a warning banner, the formatted message and the source location. */
void RgWarn(const char *format, const char *source_file, int line, ...)
{
    va_list ap;
    va_start(ap, line);

    XrgLog(errorSeparatorLine, errorSourcePath, 31);
    XrgLog(errorWarningBanner, errorSourcePath, 32);
    XrgVLog(format, errorSourcePath, 33, ap);
    XrgLog(errorWarningLocationFormat, errorSourcePath, 34, source_file, line);
    XrgLog(errorSeparatorLine, errorSourcePath, 35);

    va_end(ap);
}
