#include "common.h"
#include "shared.h"

INCLUDE_ASM("asm/main/nonmatchings/wind_test", InitTest_002D9430);

INCLUDE_ASM("asm/main/nonmatchings/wind_test", MoveCamera);

INCLUDE_ASM("asm/main/nonmatchings/wind_test", MoveWind);

INCLUDE_ASM("asm/main/nonmatchings/wind_test", MoveWind2);

static signed char wtype;
static float shake;
static Vector4 wpos;
#define D_0058518C (&wpos.w)
extern double fptodp(float value);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

static void PrintDisp(void)
{
    const char *position_format;

    xglFontDebugPrintf(0, 0, "\013WindTest");
    if (wtype == 2) {
        xglFontDebugPrintf(8, 8, "\013point");
    } else {
        xglFontDebugPrintf(8, 8, "\013directional");
    }
    position_format = "\013\033\030\033\036\033\037WindPow:%f";
    xglFontDebugPrintf(0x64, 0xC8, position_format, fptodp(D_0058518C[0]));
    xglFontDebugPrintf(0x64, 0xD0,
                       "\013\033\030\033\035\033\034RandPow:%f",
                       fptodp(shake));
}

INCLUDE_ASM("asm/main/nonmatchings/wind_test", WindTest);
