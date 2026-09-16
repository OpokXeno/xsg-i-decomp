/*
 * TU-local declarations of main/tu215 (src/main/sdv.c).
 */

#ifndef SRC_MAIN_SDV_H
#define SRC_MAIN_SDV_H

#include "menu_skill.h"

typedef short int16_t;

/*
 * Fresh Spark hypothesis F1: renamed partial camera-offset view.
 * Changed from the predecessor's SdvCameraOffset/offset_mode/offset_source
 * spelling: identical 48-byte layout and else-if branch shape, but distinct
 * type/field/local identifiers (SdvCamOffset/cam_kind/cam_vec, pos/ang) to
 * test the ABI saved-register hypothesis that local-alias lifetime spelling
 * keeps mode in s1 and source in s2. Unnamed 32-bit units preserve the
 * observed +0x10/+0x20 vector offsets and assert no historical semantics.
 */
typedef struct {
    int kind;
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
    float pos[3];
    unsigned int : 32;
    float ang[3];
    unsigned int : 32;
} SdvCamOffset;

extern int _sdvAmbFrame;
extern int _sdvAmbState;

extern void sdvSetAmbStateSub(int state, int effect_no, int force);
extern void *xglStudioGetLight2(void);
extern void xglLightIntensityAmbient(void *light, void *ambient);
extern void func_A2C3D8(void *map_rgb);
extern unsigned char _sdvAmbient[16];
extern unsigned char _sdvMapRgb[16];

#endif /* SRC_MAIN_SDV_H */
