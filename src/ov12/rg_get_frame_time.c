/*
 * OV12 original TU 3: 0x00a024f8..0x00a02510 (1 functions)
 */
#include "common.h"

/* One PS2 NTSC field at 30 Hz (1.0f / 30.0f), the engine's fixed frame time. */
float RgGetFrameTime(void)
{
    return 0.033333335f;
}
