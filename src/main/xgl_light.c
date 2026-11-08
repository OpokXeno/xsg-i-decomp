#include "common.h"
#include "shared.h"
#include "xgl_light.h"
#include "main/xgl_light.h"
#include "main/xgl_2.h"
#include "quadword.h"

extern char Vu0CallLightCalcMatrix[];

void xglLightIntensityAmbient(StudioLight *lightSet,
                              const XglLightQuadword *intensity);

void xglLightIntensityParallel(StudioLight *lightSet, unsigned int index,
                               const XglLightQuadword *intensity);

/* Sets the default ambient intensity and the intensity and direction of the three parallel lights. */
void xglLightSetDefault(StudioLight *lightSet)
{
    static XglLightVector asIntensity[4] = {
        { .value = { 0.25f, 0.25f, 0.25f, 1.0f } },
        { .value = { 0.8f, 0.8f, 0.8f, 1.0f } },
        { .value = { 0.5f, 0.5f, 0.5f, 1.0f } },
        { .value = { 0.5f, 0.5f, 0.5f, 1.0f } }
    };
    static XglLightVector asDirection[4] = {
        { .value = { 0.0f, 0.0f, 0.0f, 1.0f } },
        { .value = { -0.26195645f, 0.53120846f, -0.019844394f, 1.0f } },
        { .value = { 2.3561945f, 1.2733574f, 3.1415927f, 1.0f } },
        { .value = { 0.36123082f, 3.775898f, 0.028536133f, 1.0f } }
    };
    int i;

    for (i = 0; i < 4; i++) {
        if (i == 0) {
            xglLightIntensityAmbient(lightSet, &asIntensity[0].quadword);
        } else {
            xglLightIntensityParallel(lightSet, i - 1, &asIntensity[i].quadword);
            xglLightAngle(lightSet, i - 1, &asDirection[i].value);
        }
    }
}

void xglLightIntensityAmbient(StudioLight *lightSet, const XglLightQuadword *intensity)
{
    /* SDK-style quadword transfer: original uses fixed scratch $2. */
    QuadwordCopy(lightSet, intensity);
}

/* Copies intensity into the color of parallel light index; indices 3 and up are ignored. */
void xglLightIntensityParallel(StudioLight *lightSet, unsigned int index, const XglLightQuadword *intensity)
{
    /* EE pointers are 32-bit addresses; the selected array entry stays aligned. */
    unsigned int lightSetAddress = (unsigned int)lightSet;
    if (index < 3U) {
        StudioLightEntry *parallelLight = (StudioLightEntry *)
            (index * sizeof(StudioLightEntry) + lightSetAddress +
             sizeof(lightSet->ambient_color));
        /* SDK-style quadword transfer to the selected intensity. */
        QuadwordCopy(&parallelLight->color, intensity);
    }
}

/* Stores the normalized source vector as the direction of parallel light index (0 to 2). */
void xglLightDirection(StudioLight *lightSet, unsigned int index, const Vector4 *source)
{
    if (index < 3U) {
        xglVectorNormal(&lightSet->lights[index].direction, source);
    }
}

/*
 * Builds a rotation from angle's z, y and x components, applied in that order, and stores the
 * matrix's third row as the direction of parallel light index (0 to 2).
 */
void xglLightAngle(StudioLight *lightSet, unsigned int index, const Vector4 *angle)
{
    /* EE pointers are 32-bit addresses; the selected array entry stays aligned. */
    unsigned int lightSetAddress = (unsigned int)lightSet;
    Matrix4 matrix;

    if (index < 3U) {
        StudioLightEntry *parallelLight;

        xglMatrixStackUnit();
        xglMatrixStackRotZ(angle->z);
        xglMatrixStackRotY(angle->y);
        xglMatrixStackRotX(angle->x);
        xglMatrixStackSave(matrix);
        parallelLight = (StudioLightEntry *)
            (index * sizeof(StudioLightEntry) + lightSetAddress +
             sizeof(lightSet->ambient_color));
        /* SDK-style quadword transfer of the matrix's third row. */
        QuadwordCopy(&parallelLight->direction, &matrix[2]);
    }
}

/*
 * Runs the VU0 microprogram Vu0CallLightCalcMatrix (src/main/vu0/Vu0MicroCode.dvp) on the seven
 * quadwords at lightSet+0x00..0x60 and stores two 4x4 matrices: the transposed light directions at
 * lightSet+0x70, and the light intensities (w=0) with ambient intensity (w=1) as columns at +0xB0.
 * Only the VU0 dispatch is inline assembly.
 */
void xglLightCalcMatrix(StudioLight *lightSet)
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
        "sqc2 vf27,224(%1)"
        :
        : "r"((unsigned int)Vu0CallLightCalcMatrix / 8), "r"(lightSet)
        : "memory"
    );
}

/*
 * Sets the ambient intensity to 1, and the intensity of each parallel light to 0 with its direction
 * set through xglLightDirection from the vector (1, 1, 1, 1).
 */
void xglLightInit(StudioLight *lightSet)
{
    static XglLightVector sIntensity0 = { .value = { 0.0f, 0.0f, 0.0f, 0.0f } };
    static XglLightVector sIntensity1 = { .value = { 1.0f, 1.0f, 1.0f, 1.0f } };
    int i;

    xglLightIntensityAmbient(lightSet, &sIntensity1.quadword);
    for (i = 0; i < 3; i++) {
        xglLightIntensityParallel(lightSet, i, &sIntensity0.quadword);
        xglLightDirection(lightSet, i, &sIntensity1.value);
    }
}
