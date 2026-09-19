#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/tya", getfbp);

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaBmpOutput);

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaSiPicOutput);

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaCaptureMain);

static void tyaCaptureNull(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaCaptureActor);

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaCaptureShadow);

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaCaptureUnit);

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaCaptureUnit2);

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaCaptureStart);

INCLUDE_ASM("asm/main/nonmatchings/tya", tyaCaptureEnd);
