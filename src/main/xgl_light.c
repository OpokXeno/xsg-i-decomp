#include "common.h"
#include "shared.h"
#include "xgl_light.h"
#include "main/xgl_light.h"
#include "main/xgl_2.h"

extern char Vu0CallLightCalcMatrix[];

void xglLightIntensityAmbient(StudioLight *lightSet,
                              const XglLightQuadword *intensity);

void xglLightIntensityParallel(StudioLight *lightSet, unsigned int index,
                               const XglLightQuadword *intensity);

/* The row copy is one lq/sq pair through explicit address registers, as the original emits it. */
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
    __asm__ __volatile__(
        "lq $2,0(%1)\n\t"
        "sq $2,0(%0)"
        :
        : "r"(lightSet), "r"(intensity)
        : "$2", "memory"
    );
}

void xglLightIntensityParallel(StudioLight *lightSet, unsigned int index, const XglLightQuadword *intensity)
{
    /* EE pointers are 32-bit addresses; the selected array entry stays aligned. */
    unsigned int lightSetAddress = (unsigned int)lightSet;
    if (index < 3U) {
        StudioLightEntry *parallelLight = (StudioLightEntry *)
            (index * sizeof(StudioLightEntry) + lightSetAddress +
             sizeof(lightSet->ambient_color));
        /* SDK-style quadword transfer to the selected intensity. */
        __asm__ __volatile__(
            "lq $2,0(%1)\n\t"
            "sq $2,0(%0)"
            :
            : "r"(&parallelLight->color), "r"(intensity)
            : "$2", "memory"
        );
    }
}

void xglLightDirection(StudioLight *lightSet, unsigned int index, const Vector4 *source)
{
    if (index < 3U) {
        xglVectorNormal(&lightSet->lights[index].direction, source);
    }
}

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
        __asm__ __volatile__(
            "lq $2,0(%1)\n\t"
            "sq $2,0(%0)"
            :
            : "r"(&parallelLight->direction), "r"(&matrix[2])
            : "$2", "memory"
        );
    }
}

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
