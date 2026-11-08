/*
 * OV01 original TU 12: 0x00a2c9b8..0x00a2c9f8 (4 functions)
 */
#include "common.h"

extern void SeisanInit(void);
/*
 * Returns nonzero while the settlement (seisan) screen is still running
 * (SeisanWork byte +0x01 is not 0xff).
 */
extern int SeisanMain(void);

void resultProcInit(void)
{
    SeisanInit();
}

/* Runs the settlement screen and returns nonzero while it runs; entryPhase90 advances to entryPhase100 once this returns 0. */
int resultProc(void)
{
    return SeisanMain();
}

void resultDisp(void)
{
}

void resultBackDisp(void)
{
}
