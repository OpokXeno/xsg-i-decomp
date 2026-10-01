#include "common.h"
#include "shared.h"
#include "xgl_light.h"

typedef unsigned int XglLightQuadword __attribute__((mode(TI)));

/* Light vectors occupy aligned quadwords consumed by the VU light matrix program. */
typedef struct {
    XglLightQuadword intensity;
    XglLightQuadword direction;
} XglPackedParallelLight;

typedef struct {
    XglLightQuadword ambientIntensity;
    XglPackedParallelLight parallel[3];
} XglPackedLightSet;

INCLUDE_ASM("asm/main/nonmatchings/xgl_light", xglLightSetDefault);

/* Volatile preserves the quadword update observed by the VU light calculation. */
void xglLightIntensityAmbient(volatile XglPackedLightSet *lightSet, const XglLightQuadword *intensity)
{
    lightSet->ambientIntensity = *intensity;
}

/* Volatile preserves the quadword update observed by the VU light calculation. */
void xglLightIntensityParallel(XglPackedLightSet *lightSet, unsigned int index, const XglLightQuadword *intensity)
{
    /* EE pointers are 32-bit addresses; the selected array entry stays aligned. */
    unsigned int lightSetAddress = (unsigned int)lightSet;
    if (index < 3U) {
        XglPackedParallelLight *parallelLight = (XglPackedParallelLight *)
            (index * sizeof(XglPackedParallelLight) + lightSetAddress +
             sizeof(lightSet->ambientIntensity));
        volatile XglLightQuadword *destination = &parallelLight->intensity;
        *destination = *intensity;
    }
}

extern void xglVectorNormal(Vector4 *destination, const Vector4 *source);

void xglLightDirection(XglLightSet *lightSet, unsigned int index, const Vector4 *source)
{
    if (index < 3U) {
        xglVectorNormal(&lightSet->parallel[index].direction, source);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_light", xglLightAngle);

/*
 * xglLightCalcMatrix (ee-vu-cop2): dispatches the VU0 macro-mode
 * Vu0CallLightCalcMatrix microprogram (src/main/vu0/Vu0MicroCode.dvp) on the
 * light set's seven quadwords (ambientIntensity and the three parallel
 * lights' intensity/direction pairs) and writes its two 4x4 output matrices
 * back at lightSet+0x70 and lightSet+0xB0: the first is the transpose of the
 * three light directions (each row padded with w=0, plus a trailing
 * (0,0,0,1) row) and the second has the three light intensities (w=0) and
 * ambientIntensity (w=1) as its columns -- see that file's own comment for
 * the per-lane derivation. Only the VU0 dispatch is inline assembly.
 */
extern char Vu0CallLightCalcMatrix[];

void xglLightCalcMatrix(XglLightSet *lightSet)
{
    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "lqc2 vf4,0(%1)\n\t"
        "lqc2 vf5,16(%1)\n\t"
        "lqc2 vf6,32(%1)\n\t"
        "lqc2 vf7,48(%1)\n\t"
        "lqc2 vf8,64(%1)\n\t"
        "lqc2 vf9,80(%1)\n\t"
        "lqc2 vf10,96(%1)\n\t"
        "vcallmsr $vi27\n\t"
        "cfc2.i $0,$vi0\n\t"
        "sqc2 vf20,112(%1)\n\t"
        "sqc2 vf21,128(%1)\n\t"
        "sqc2 vf22,144(%1)\n\t"
        "sqc2 vf23,160(%1)\n\t"
        "sqc2 vf24,176(%1)\n\t"
        "sqc2 vf25,192(%1)\n\t"
        "sqc2 vf26,208(%1)\n\t"
        "sqc2 vf27,224(%1)\n\t"
        "nop"
        :
        : "r"((unsigned int)Vu0CallLightCalcMatrix / 8), "r"(lightSet)
        : "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_light", xglLightInit);
