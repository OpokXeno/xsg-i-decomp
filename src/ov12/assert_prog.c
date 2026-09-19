/*
 * OV12 original TU 18: 0x00a13200..0x00a13260 (1 functions)
 */
#include "common.h"

extern void XrgOut(const char *format, ...);
extern void XrgExit(int status);

extern const char D_00A529E8[];
extern const char D_00A52A20[];

void assert_prog(const char *expression, const char *source_file, int line)
{
    XrgOut(D_00A529E8);
    XrgOut(D_00A52A20, source_file, line, expression);
    XrgExit(1);
}
