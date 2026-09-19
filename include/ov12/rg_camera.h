#ifndef INCLUDE_OV12_RG_CAMERA_H
#define INCLUDE_OV12_RG_CAMERA_H

#include "shared.h"

typedef struct RgDrawStudio RgDrawStudio;

/*
 * _InitAbstructCamera (ov12:0x00a10f78) attests the leading span: the draw
 * studio pointer at +0x00 (its own "pStudio != NIL" assert), the eye and
 * target vectors it clears at +0x10/+0x20, the up vector it copies from
 * XrgVectorY() at +0x30, and the seven-word action-state block it resets at
 * +0x40..+0x58 (all zero except index 1, +0x44, the actionFlags word above,
 * which it also clears; the other six indices' individual roles are not
 * evidenced yet). +0x04..+0x0f is untouched by any claimed function.
 */
struct RgCamera {
    RgDrawStudio *studio;              /* +0x00 */
    unsigned char unmodeled_04[0x0c];  /* +0x04 */
    RgVector eye;                      /* +0x10 */
    RgVector target;                   /* +0x20 */
    RgVector up;                       /* +0x30 */
    int actionState[7];                /* +0x40..+0x58 */
};

#endif /* INCLUDE_OV12_RG_CAMERA_H */
