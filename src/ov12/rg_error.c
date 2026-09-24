/*
 * OV12 original TU 61: 0x00a31768..0x00a31970 (2 functions)
 */
#include "common.h"

INCLUDE_ASM("asm/nonmatchings/ov12/rg_error", RgError);

typedef char *va_list;
#define va_start(ap, last) ((ap) = (va_list)__builtin_next_arg(last) - (8 - __builtin_args_info(2)) * 8)
#define va_end(ap) ((void)0)

extern void XrgLog(const char *format, const char *source_file, int line, ...);
extern void XrgVLog(const char *format, const char *source_file, int line,
                    va_list ap);

/*
 * ov12:0x00a55478 contains the source filename "../rg_error.euc.c".
 * ov12:0x00a55448 contains the format string
 *      "------------------------------------------\n".
 * ov12:0x00a554c0 contains the format string "RG : WARNING!\n".
 * ov12:0x00a554d0 contains the format string
 *      "\n\nwarning in %s at %d\n".
 */
extern const char D_00A55478[];
extern const char D_00A55448[];
extern const char D_00A554C0[];
extern const char D_00A554D0[];

void RgWarn(const char *format, const char *source_file, int line, ...)
{
    va_list ap;
    va_start(ap, line);

    XrgLog(D_00A55448, D_00A55478, 31);
    XrgLog(D_00A554C0, D_00A55478, 32);
    XrgVLog(format, D_00A55478, 33, ap);
    XrgLog(D_00A554D0, D_00A55478, 34, source_file, line);
    XrgLog(D_00A55448, D_00A55478, 35);

    va_end(ap);
}
