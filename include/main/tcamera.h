#ifndef INCLUDE_MAIN_TCAMERA_H
#define INCLUDE_MAIN_TCAMERA_H

#include "shared.h"

/*
 * Partial camera view: TCAMERA_setFov/fovSPL use fieldOfView at +0x94;
 * TCAMERA_setView uses rotation at +0xa0 and translation at +0xd0.
 * Unevidenced spans remain explicitly unmodeled. The playback controller
 * (src/main/play.c) reads the three mode words at +0x0c through TCAMERA_get.
 */
typedef struct TCamera {
    unsigned char unmodeled_00[0x0c];
    unsigned int mode[3];          /* +0x0c */
    unsigned char unmodeled_18[0x94 - 0x18];
    float fieldOfView;             /* +0x94 */
    unsigned char unmodeled_98[8];
    Vector4 rotation;              /* +0xa0 */
    unsigned char unmodeled_b0[0x20];
    Vector4 translation;           /* +0xd0 */
} TCamera;

/* Functions and data used by other translation units. */
void TCAMERA_update(void);
void TCAMERA_init(void);

#endif /* INCLUDE_MAIN_TCAMERA_H */
