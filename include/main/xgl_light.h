#ifndef INCLUDE_MAIN_XGL_LIGHT_H
#define INCLUDE_MAIN_XGL_LIGHT_H

#include "shared.h"

/* Light vectors occupy aligned quadwords consumed by the VU light matrix program. */
typedef unsigned int XglLightQuadword __attribute__((mode(TI)));

/*
 * Light vectors are copied as aligned quadwords and consumed as float
 * components by the direction and rotation helpers.
 */
typedef union {
    XglLightQuadword quadword;
    Vector4 value;
} XglLightVector;

/*
 * main/tu149's light editor (DB_lightWrite, updateLight) indexes these
 * vectors as float[4] rows (component offsets fold into the row's index, not
 * the load displacement), so each vector also has its RgVector row.
 */
typedef struct StudioLightEntry {
    union { Vector4 color; RgVector color_row; };
    union { Vector4 direction; RgVector direction_row; };
} StudioLightEntry;

/* The light block of a studio record, also the light record of an Actor
 * (+0x510). Whole-block copies (game.c GameStateSave/RestoreCameraLight) are
 * ld/sd, while the Studio records holding it are only 8-aligned (xgl_studio
 * .bss): the block is 8-aligned, which the doubleword view of its first vector
 * gives. */
typedef struct StudioLight {
    union {
        struct {
            union { Vector4 ambient_color; RgVector ambient_color_row; u64 ambient_color_words[2]; };
            StudioLightEntry lights[3];
        };
        Vector4 parameters[7];          /* the seven quadwords xglLightCalcMatrix loads: ambient, then three colour/direction pairs */
    };
    float normal_matrix[4][4];          /* +0x70: map_1.c's MapDrawLight view */
    float color_modifier_matrix[4][4];  /* +0xb0: map_1.c's MapDrawLight view */
} StudioLight;

/* Functions and data used by other translation units. */
void xglLightCalcMatrix(StudioLight *lightSet);
void xglLightSetDefault(StudioLight *lightSet);
void xglLightAngle(StudioLight *lightSet, unsigned int index,
                   const Vector4 *angle);

#endif /* INCLUDE_MAIN_XGL_LIGHT_H */
