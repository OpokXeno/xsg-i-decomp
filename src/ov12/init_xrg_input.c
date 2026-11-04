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

extern XrgPadRecord PadData[2];

#define PAD_RECORD_STICKS_OFFSET 0x30
#define PAD_L2 0x0001
#define PAD_R2 0x0002
#define PAD_L1 0x0004
#define PAD_R1 0x0008
#define PAD_UP 0x1000
#define PAD_RIGHT 0x2000
#define PAD_DOWN 0x4000
#define PAD_LEFT 0x8000

void XrgInputDeviceCheck(XrgInput *pInput, float frameTime)
{
    float stickX[2];
    float stickY[2];
    float magnitude[2];
    RgVector vec;
    float len;
    float clamped;
    u16 buttons;
    u16 pressed;
    unsigned int held;
    int i;
    int k;
    float minShoulderTimer[2];
    int padIdx;
    XrgPadSticks *sticks;
    int stickOffset;

    padIdx = pInput->padId;
    buttons = PadData[padIdx].prefix.half_28;
    pressed = PadData[padIdx].prefix.half_2a;
    memset(stickX, 0, 8);
    memset(stickY, 0, 8);
    memset(magnitude, 0, 8);

    if (pressed & PAD_L1) {
        pInput->buttonTimer[0] = 0;
    }
    if (pressed & PAD_L2) {
        pInput->buttonTimer[1] = 0;
    }
    if (pressed & PAD_R1) {
        pInput->buttonTimer[2] = 0;
    }
    if (pressed & PAD_R2) {
        pInput->buttonTimer[3] = 0;
    }

    stickOffset = padIdx * sizeof(XrgPadRecord) + PAD_RECORD_STICKS_OFFSET;
    sticks = (XrgPadSticks *)((char *)PadData + stickOffset);
    stickX[0] = (float)sticks->analog[3] * 0.0078125f;
    stickY[0] = (float)sticks->analog[2] * 0.0078125f;
    stickX[1] = (float)sticks->analog[1] * 0.0078125f;
    stickY[1] = (float)sticks->analog[0] * 0.0078125f;

    if (buttons & PAD_UP) {
        stickX[0] = -1.0f;
    } else if (buttons & PAD_DOWN) {
        stickX[0] = 1.0f;
    }
    if (buttons & PAD_LEFT) {
        stickY[0] = -1.0f;
    } else if (buttons & PAD_RIGHT) {
        stickY[0] = 1.0f;
    }

    for (i = 0; i < 2; i++) {
        vec[0] = stickX[i];
        vec[1] = stickY[i];
        vec[2] = vec[3] = 0.0f;
        len = XrgNormalizeVector(vec, vec);
        clamped = (1.0f < len) ? 1.0f : len;
        magnitude[i] = clamped;
        if (clamped < 0.3f) {
            magnitude[i] = 0.0f;
        }
        pInput->magnitude[i] = magnitude[i];
        pInput->angle[i] = atan2f(vec[1], vec[0]);
    }

    pInput->flags = 0;
    if (pressed & 0x5) {
        pInput->latchedFlags |= 1;
    }
    if (pressed & 0xa) {
        pInput->latchedFlags |= 2;
    }

    minShoulderTimer[0] = (pInput->buttonTimer[0] < pInput->buttonTimer[1]) ? pInput->buttonTimer[0] : pInput->buttonTimer[1];
    minShoulderTimer[1] = (pInput->buttonTimer[2] < pInput->buttonTimer[3]) ? pInput->buttonTimer[2] : pInput->buttonTimer[3];
    if (minShoulderTimer[0] < 0.2f) {
        if (minShoulderTimer[1] < 0.2f) {
            pInput->latchedFlags |= 4;
        }
    }

    held = buttons & 0x5;
    if (held) {
        if (pInput->latchedFlags & 1) {
            pInput->flags |= 1;
        }
    } else {
        pInput->latchedFlags &= ~1;
    }

    held = buttons & 0xa;
    if (held) {
        if (pInput->latchedFlags & 2) {
            pInput->flags |= 2;
        }
    } else {
        pInput->latchedFlags &= ~2;
    }

    if ((buttons & 0xa) && (buttons & 0x5)) {
        if (pInput->latchedFlags & 4) {
            pInput->flags |= 4;
        }
    } else {
        pInput->latchedFlags &= ~4;
    }

    if (pInput->flags & 4) {
        pInput->flags &= ~3;
        pInput->latchedFlags &= ~3;
    }

    if (stickY[0] > 0.7f && stickY[1] < -0.7f) {
        pInput->flags |= 0x20;
    }
    if (stickY[0] < -0.8f && stickY[1] > 0.8f) {
        pInput->flags |= 0x40;
    }
    pInput->flags &= ~0x180;
    if (buttons & 0x600) {
        pInput->flags |= 0x80;
    }
    if ((buttons & 0x600) == 0x600) {
        pInput->flags |= 0x100;
    }

    pInput->flags &= ~0xe00;
    if (pInput->buttonTimer[0] < 0.1f && pInput->buttonTimer[1] < 0.1f) {
        pInput->flags |= 0x200;
    }
    if (pInput->buttonTimer[2] < 0.1f && pInput->buttonTimer[3] < 0.1f) {
        pInput->flags |= 0x400;
    }

    if ((pInput->flags & 0x600) == 0x600) {
        pInput->flags = (pInput->flags & ~0x600) | 0x800;
    }

    pInput->pressFlags |= pInput->flags & 0xe00;
    if (!(buttons & 0x5)) {
        pInput->pressFlags &= ~0x200;
    }
    if (!(buttons & 0xa)) {
        pInput->pressFlags &= ~0x400;
    }
    if ((buttons & 0xf) == 0) {
        pInput->pressFlags &= ~0x800;
    }

    for (i = 0; i < 2; i++) {
        for (k = 0; k < 2; k++) {
            pInput->buttonTimer[i * 2 + k] += frameTime;
        }
    }
}
