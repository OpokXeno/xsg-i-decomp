/*
 * OV12 original TU 85: 0x00a49f70..0x00a4a620 (2 functions)
 */
#include "common.h"
#include "init_xrg_input.h"

void InitXrgInput(XrgInput *pInput, int padId)
{
    int i;

    pInput->padId = padId;
    for (i = 0; i < 2; i++) {
        pInput->angle[i] = 0.0f;
        pInput->magnitude[i] = 0.0f;
    }
    pInput->flags = 0;

    for (i = 0; i < 2; i++) {
        pInput->buttonTimer[i * 2] = 100000000.0f;
        pInput->buttonTimer[i * 2 + 1] = 100000000.0f;
    }

    pInput->pressFlags = 0;
    pInput->latchedFlags = 0;
    pInput->flags = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov12/init_xrg_input", XrgInputDeviceCheck);
