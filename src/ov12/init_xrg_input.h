/*
 * TU-local declarations of ov12/tu085 (src/ov12/init_xrg_input.c).
 */

#ifndef SRC_OV12_INIT_XRG_INPUT_H
#define SRC_OV12_INIT_XRG_INPUT_H

#include "shared.h"
#include "ov12/init_xrg_input.h"

/* XrgInput::flags, as XrgInputDeviceCheck derives them each frame. */
#define XRG_INPUT_FLAG_LEFT_SHOULDER 0x1       /* L1/L2 held after a fresh press */
#define XRG_INPUT_FLAG_RIGHT_SHOULDER 0x2      /* R1/R2 held after a fresh press */
#define XRG_INPUT_FLAG_BOTH_SHOULDERS 0x4      /* both sides held, pressed together */
#define XRG_INPUT_FLAG_STICK_CLICK 0x80        /* L3 or R3 held */
#define XRG_INPUT_FLAG_BOTH_STICK_CLICKS 0x100 /* L3 and R3 held */
#define XRG_INPUT_FLAG_LEFT_PAIR 0x200         /* L1 and L2 pressed within the pair window */
#define XRG_INPUT_FLAG_RIGHT_PAIR 0x400        /* R1 and R2 pressed within the pair window */
#define XRG_INPUT_FLAG_BOTH_PAIRS 0x800        /* both pairs at once; replaces the two above */

/* Shoulder buttons of each side, and both stick clicks. */
#define XRG_INPUT_LEFT_SHOULDER_BUTTONS (PAD_L1 | PAD_L2)
#define XRG_INPUT_RIGHT_SHOULDER_BUTTONS (PAD_R1 | PAD_R2)
#define XRG_INPUT_STICK_CLICKS (PAD_L3 | PAD_R3)

/* Seconds: both sides count as pressed together, and one pair counts as pressed together. */
#define XRG_INPUT_BOTH_SIDES_WINDOW 0.2f
#define XRG_INPUT_PAIR_WINDOW 0.1f
/* Stick values below this magnitude read as centered. */
#define XRG_INPUT_STICK_DEAD_ZONE 0.3f
/* A button timer that is not counting (no press yet). */
#define XRG_INPUT_TIMER_IDLE 100000000.0f

/*
 * XrgInput is the 0x30-byte per-pad input state _InitControlInput allocates
 * with RgHeapAlloc(size 0x30, ov12:0x00a09968) and stores at RgControlInput's
 * buffer field (src/ov12/rg_robot_control.h); InitXrgInput writes it (VA
 * 0x00a49f70..0x00a49fec).
 *
 * padId (+0x00) indexes PadData[2] (XrgInputDeviceCheck, ov12:0x00a4a048..).
 * angle[2]/magnitude[2] (+0x04/+0x0C) are the per-stick polar values
 * XrgInputDeviceCheck derives with atan2f and its deadzone clamp
 * (ov12:0x00a4a238..0x00a4a260), one pair per analog stick.
 * flags (+0x14) accumulates the digital/analog state bits
 * XrgInputDeviceCheck sets from held/pressed masks and stick magnitude
 * (ov12:0x00a4a26c onward).
 * buttonTimer[4] (+0x18) is the L1/L2/R1/R2 elapsed-time-since-press timer:
 * reset to 0 on that button's press (ov12:0x00a4a0a8/0xb8/0xc4/0xd0) and
 * advanced by the frame time every call (ov12:0x00a4a5b0..0x00a4a5d8).
 * pressFlags (+0x28) latches the L1/L2 and R1/R2 press combination
 * (ov12:0x00a4a270..0x00a4a290).
 * latchedFlags (+0x2C) carries flags' upper bits across frames and clears
 * them once the matching shoulder button is released
 * (ov12:0x00a4a534..0x00a4a5a0).
 */
struct XrgInput {
    int padId;               /* +0x00 */
    float angle[2];          /* +0x04 */
    float magnitude[2];      /* +0x0C */
    unsigned int flags;      /* +0x14 */
    float buttonTimer[4];    /* +0x18 */
    unsigned int pressFlags; /* +0x28 */
    unsigned int latchedFlags; /* +0x2C */
};

#endif /* SRC_OV12_INIT_XRG_INPUT_H */
