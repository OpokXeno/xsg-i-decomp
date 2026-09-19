/*
 * OV12 original TU 95: 0x00a4f280..0x00a4f4c4 (5 functions)
 */
#include "common.h"
#include "xrg_event.h"

void XrgSetEventLevel(int level)
{
    if (level == -1) {
        s_bSetEventLevel = 0;
        return;
    }
    s_bSetEventLevel = 1;
    s_eEventLevel = level;
}

int XrgEventGetLevel(void)
{
    int usable;

    if (s_bSetEventLevel != 0) {
        return s_eEventLevel;
    }
    usable = xglFlagsGet1(0x65);
    return (xglFlagsGet1(0x12D) == 0) ? (usable != 0) : 2;
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_event", XrgEventIsUsableEnemy);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_event", XrgEventIsUsablePlayer);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_event", XrgEventIsUsableWeapon);
