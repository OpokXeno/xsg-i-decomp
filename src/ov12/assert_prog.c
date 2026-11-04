/*
 * OV12 original TU 18: 0x00a13200..0x00a13260 (1 functions)
 */
#include "common.h"
#include "ov12/assert_prog.h"
#include "ov12/xrg_exit.h"
#include "ov12/xrg_v_log.h"


const char assertProgSeparatorText[56] = "------------------------------------------------\n";
const char assertProgMessageFormat[32] = "NIS ASSERT (in %s at %d) : %s\n";

void assert_prog(const char *expression, const char *source_file, int line)
{
    XrgOut(assertProgSeparatorText);
    XrgOut(assertProgMessageFormat, source_file, line, expression);
    XrgExit(1);
}
