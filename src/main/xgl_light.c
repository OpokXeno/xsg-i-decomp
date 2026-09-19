#include "common.h"
#include "shared.h"
#include "xgl_light.h"

INCLUDE_ASM("asm/main/nonmatchings/xgl_light", xglLightSetDefault);

INCLUDE_ASM("asm/main/nonmatchings/xgl_light", xglLightIntensityAmbient);

INCLUDE_ASM("asm/main/nonmatchings/xgl_light", xglLightIntensityParallel);

extern void xglVectorNormal(Vector4 *destination, const Vector4 *source);

void xglLightDirection(XglLightSet *lightSet, unsigned int index, const Vector4 *source)
{
    if (index < 3U) {
        xglVectorNormal(&lightSet->parallel[index].direction, source);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_light", xglLightAngle);

INCLUDE_ASM("asm/main/nonmatchings/xgl_light", xglLightCalcMatrix);

INCLUDE_ASM("asm/main/nonmatchings/xgl_light", xglLightInit);
