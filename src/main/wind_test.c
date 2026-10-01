#include "common.h"
#include "shared.h"

INCLUDE_ASM("asm/main/nonmatchings/wind_test", InitTest_002D9430);

INCLUDE_ASM("asm/main/nonmatchings/wind_test", MoveCamera);

INCLUDE_ASM("asm/main/nonmatchings/wind_test", MoveWind);

INCLUDE_ASM("asm/main/nonmatchings/wind_test", MoveWind2);

extern signed char wtype;
extern float shake;
extern const char D_004CBB80[];
extern const char D_004CBB90[];
extern const char D_004CBBA0[];
extern const char D_004CBBB8[];
extern const char D_004DB9D0[];
extern float D_0058518C[];
extern double fptodp(float value);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

static void PrintDisp(void)
{
    const char *position_format;

    xglFontDebugPrintf(0, 0, D_004CBB80);
    if (wtype == 2) {
        xglFontDebugPrintf(8, 8, D_004DB9D0);
    } else {
        xglFontDebugPrintf(8, 8, D_004CBB90);
    }
    position_format = D_004CBBA0;
    xglFontDebugPrintf(0x64, 0xC8, position_format, fptodp(D_0058518C[0]));
    xglFontDebugPrintf(0x64, 0xD0, D_004CBBB8, fptodp(shake));
}

INCLUDE_ASM("asm/main/nonmatchings/wind_test", WindTest);
