/*
 * OV12 original TU 85: 0x00a49f70..0x00a4a620 (2 functions)
 */
#include "common.h"
#include "shared.h"
#include "init_xrg_input.h"
#include "ov12/xrg_rand_int.h"

/*
 * Partial view of XglPadRecord (include/main/xgl_pad.h), 0x68 bytes per pad:
 * the held and pressed halfwords and, from +0x30, the block whose last four
 * bytes are the signed stick positions, scaled by 1/128 (measured: with the
 * canonical record's 8-byte alignment the original's address arithmetic is
 * not reproduced, so the stick block is addressed by its byte offset).
 */
typedef struct XglPadRecordSticks {
    u8 unmodeled_00[8];
    signed char position[4];
} XglPadRecordSticks;

typedef struct XglPadRecordView {
    u8 unmodeled_00[0x28];
    u16 buttons;
    u16 pressed;
    u8 unmodeled_2c[0x30 - 0x2c];
    XglPadRecordSticks sticks;
    u8 unmodeled_3c[0x68 - 0x3c];
} XglPadRecordView;

#define XGL_PAD_RECORD_STICKS_OFFSET 0x30

extern XglPadRecordView PadData[2];

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
        pInput->buttonTimer[i * 2] = XRG_INPUT_TIMER_IDLE;
        pInput->buttonTimer[i * 2 + 1] = XRG_INPUT_TIMER_IDLE;
    }

    pInput->pressFlags = 0;
    pInput->latchedFlags = 0;
    pInput->flags = 0;
}

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
    XglPadRecordSticks *sticks;
    int stickOffset;

    padIdx = pInput->padId;
    buttons = PadData[padIdx].buttons;
    pressed = PadData[padIdx].pressed;
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

    stickOffset = padIdx * sizeof(XglPadRecordView) + XGL_PAD_RECORD_STICKS_OFFSET;
    sticks = (XglPadRecordSticks *)((char *)PadData + stickOffset);
    stickX[0] = (float)sticks->position[3] * 0.0078125f;
    stickY[0] = (float)sticks->position[2] * 0.0078125f;
    stickX[1] = (float)sticks->position[1] * 0.0078125f;
    stickY[1] = (float)sticks->position[0] * 0.0078125f;

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
        if (clamped < XRG_INPUT_STICK_DEAD_ZONE) {
            magnitude[i] = 0.0f;
        }
        pInput->magnitude[i] = magnitude[i];
        pInput->angle[i] = atan2f(vec[1], vec[0]);
    }

    pInput->flags = 0;
    if (pressed & XRG_INPUT_LEFT_SHOULDER_BUTTONS) {
        pInput->latchedFlags |= XRG_INPUT_FLAG_LEFT_SHOULDER;
    }
    if (pressed & XRG_INPUT_RIGHT_SHOULDER_BUTTONS) {
        pInput->latchedFlags |= XRG_INPUT_FLAG_RIGHT_SHOULDER;
    }

    minShoulderTimer[0] = (pInput->buttonTimer[0] < pInput->buttonTimer[1]) ? pInput->buttonTimer[0] : pInput->buttonTimer[1];
    minShoulderTimer[1] = (pInput->buttonTimer[2] < pInput->buttonTimer[3]) ? pInput->buttonTimer[2] : pInput->buttonTimer[3];
    if (minShoulderTimer[0] < XRG_INPUT_BOTH_SIDES_WINDOW) {
        if (minShoulderTimer[1] < XRG_INPUT_BOTH_SIDES_WINDOW) {
            pInput->latchedFlags |= XRG_INPUT_FLAG_BOTH_SHOULDERS;
        }
    }

    held = buttons & XRG_INPUT_LEFT_SHOULDER_BUTTONS;
    if (held) {
        if (pInput->latchedFlags & XRG_INPUT_FLAG_LEFT_SHOULDER) {
            pInput->flags |= XRG_INPUT_FLAG_LEFT_SHOULDER;
        }
    } else {
        pInput->latchedFlags &= ~XRG_INPUT_FLAG_LEFT_SHOULDER;
    }

    held = buttons & XRG_INPUT_RIGHT_SHOULDER_BUTTONS;
    if (held) {
        if (pInput->latchedFlags & XRG_INPUT_FLAG_RIGHT_SHOULDER) {
            pInput->flags |= XRG_INPUT_FLAG_RIGHT_SHOULDER;
        }
    } else {
        pInput->latchedFlags &= ~XRG_INPUT_FLAG_RIGHT_SHOULDER;
    }

    if ((buttons & XRG_INPUT_RIGHT_SHOULDER_BUTTONS) &&
        (buttons & XRG_INPUT_LEFT_SHOULDER_BUTTONS)) {
        if (pInput->latchedFlags & XRG_INPUT_FLAG_BOTH_SHOULDERS) {
            pInput->flags |= XRG_INPUT_FLAG_BOTH_SHOULDERS;
        }
    } else {
        pInput->latchedFlags &= ~XRG_INPUT_FLAG_BOTH_SHOULDERS;
    }

    if (pInput->flags & XRG_INPUT_FLAG_BOTH_SHOULDERS) {
        pInput->flags &= ~(XRG_INPUT_FLAG_LEFT_SHOULDER | XRG_INPUT_FLAG_RIGHT_SHOULDER);
        pInput->latchedFlags &= ~(XRG_INPUT_FLAG_LEFT_SHOULDER | XRG_INPUT_FLAG_RIGHT_SHOULDER);
    }

    if (stickY[0] > 0.7f && stickY[1] < -0.7f) {
        pInput->flags |= 0x20;
    }
    if (stickY[0] < -0.8f && stickY[1] > 0.8f) {
        pInput->flags |= 0x40;
    }
    pInput->flags &= ~(XRG_INPUT_FLAG_STICK_CLICK | XRG_INPUT_FLAG_BOTH_STICK_CLICKS);
    if (buttons & XRG_INPUT_STICK_CLICKS) {
        pInput->flags |= XRG_INPUT_FLAG_STICK_CLICK;
    }
    if ((buttons & XRG_INPUT_STICK_CLICKS) == XRG_INPUT_STICK_CLICKS) {
        pInput->flags |= XRG_INPUT_FLAG_BOTH_STICK_CLICKS;
    }

    pInput->flags &= ~(XRG_INPUT_FLAG_LEFT_PAIR | XRG_INPUT_FLAG_RIGHT_PAIR | XRG_INPUT_FLAG_BOTH_PAIRS);
    if (pInput->buttonTimer[0] < XRG_INPUT_PAIR_WINDOW && pInput->buttonTimer[1] < XRG_INPUT_PAIR_WINDOW) {
        pInput->flags |= XRG_INPUT_FLAG_LEFT_PAIR;
    }
    if (pInput->buttonTimer[2] < XRG_INPUT_PAIR_WINDOW && pInput->buttonTimer[3] < XRG_INPUT_PAIR_WINDOW) {
        pInput->flags |= XRG_INPUT_FLAG_RIGHT_PAIR;
    }

    if ((pInput->flags & (XRG_INPUT_FLAG_LEFT_PAIR | XRG_INPUT_FLAG_RIGHT_PAIR)) ==
        (XRG_INPUT_FLAG_LEFT_PAIR | XRG_INPUT_FLAG_RIGHT_PAIR)) {
        pInput->flags = (pInput->flags & ~(XRG_INPUT_FLAG_LEFT_PAIR | XRG_INPUT_FLAG_RIGHT_PAIR)) |
                        XRG_INPUT_FLAG_BOTH_PAIRS;
    }

    pInput->pressFlags |= pInput->flags & (XRG_INPUT_FLAG_LEFT_PAIR | XRG_INPUT_FLAG_RIGHT_PAIR | XRG_INPUT_FLAG_BOTH_PAIRS);
    if (!(buttons & XRG_INPUT_LEFT_SHOULDER_BUTTONS)) {
        pInput->pressFlags &= ~XRG_INPUT_FLAG_LEFT_PAIR;
    }
    if (!(buttons & XRG_INPUT_RIGHT_SHOULDER_BUTTONS)) {
        pInput->pressFlags &= ~XRG_INPUT_FLAG_RIGHT_PAIR;
    }
    if ((buttons & (XRG_INPUT_LEFT_SHOULDER_BUTTONS | XRG_INPUT_RIGHT_SHOULDER_BUTTONS)) == 0) {
        pInput->pressFlags &= ~XRG_INPUT_FLAG_BOTH_PAIRS;
    }

    for (i = 0; i < 2; i++) {
        for (k = 0; k < 2; k++) {
            pInput->buttonTimer[i * 2 + k] += frameTime;
        }
    }
}
