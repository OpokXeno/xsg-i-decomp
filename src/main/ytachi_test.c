#include "common.h"

extern void alignXAxis(int, int);
extern void alignYAxis(int, int);
extern void alignZAxis(int, int);
extern void func_00A0A5F0(void);

void YtachiTest(void)
{
    func_00A0A5F0();
}

void dummy_func(void)
{
    alignXAxis(0, 0);
    alignYAxis(0, 0);
    alignZAxis(0, 0);
    func_00A0A5F0();
}
