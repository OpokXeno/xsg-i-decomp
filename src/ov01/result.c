/*
 * OV01 original TU 12: 0x00a2c9b8..0x00a2c9f8 (4 functions)
 */
#include "common.h"

extern void SeisanInit(void);
/*
 * Returns nonzero while the settlement (seisan) screen is still running:
 * main:0x002a18c0 yields (SeisanWork byte +0x01 != 0xff).
 */
extern int SeisanMain(void);

void resultProcInit(void)
{
    SeisanInit();
}

/* entryPhase90 advances to entryPhase100 once this returns 0. */
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
