/*
 * TU-local declarations of main/tu101 (src/main/xgl_light.c).
 */

#ifndef SRC_MAIN_XGL_LIGHT_H
#define SRC_MAIN_XGL_LIGHT_H

#include "shared.h"

/*
 * One parallel light entry of XglLightSet's array: intensity (+0x00) is the
 * quadword xglLightIntensityParallel copies in verbatim; direction (+0x10)
 * is the quadword this file's xglLightDirection normalizes into place.
 * Cited only as evidence, not as claimed source, for xglLightIntensityParallel
 * (0x0022acf8), still INCLUDE_ASM in this TU.
 */
typedef struct {
    Vector4 intensity;
    Vector4 direction;
} XglParallelLight;

/*
 * Partial view of the light set xglLightDirection's first argument points
 * to. ambientIntensity (+0x00) is the quadword xglLightIntensityAmbient
 * (0x0022ace8, still INCLUDE_ASM in this TU) copies in verbatim; it is cited
 * only as evidence for the array's start offset, not as claimed source.
 * parallel[3] (+0x10) holds the three XglParallelLight entries
 * xglLightIntensityParallel/xglLightDirection index (both bound-checked
 * against 3). Nothing beyond +0x70 is modeled here.
 */
typedef struct {
    Vector4 ambientIntensity;
    XglParallelLight parallel[3];
} XglLightSet;

#endif /* SRC_MAIN_XGL_LIGHT_H */
