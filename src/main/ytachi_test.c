#include "common.h"
#include "main/align.h"

extern void func_00A0A5F0(void);

void YtachiTest(void)
{
    func_00A0A5F0();
}

/*
 * Calls the three axis alignment helpers and func_00A0A5F0, all with zero
 * arguments.
 */
void dummy_func(void)
{
    alignXAxis(0, 0);
    alignYAxis(0, 0);
    alignZAxis(0, 0);
    func_00A0A5F0();
}
