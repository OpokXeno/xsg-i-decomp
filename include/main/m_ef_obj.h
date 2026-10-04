#ifndef INCLUDE_MAIN_M_EF_OBJ_H
#define INCLUDE_MAIN_M_EF_OBJ_H

#include "shared.h"

/*
 * mefCamParams: main-owned camera/rotation parameter block that
 * MEfObjExec1st writes every frame, still asm-owned (config/symbols/main.txt
 * "mefCamParams = 0x0043AF40; // size:0x50"). Only the fields this function
 * stores are named; the remaining 4 bytes after rotation are untouched by
 * this TU. ov01/tu022 (src/ov01/m_ef.h) reads the same block through its own
 * TU-local view of the same offsets.
 */
typedef struct MEfObjCamParams {
    StudioCamera *screen;
    Matrix4 *matrix;
    Vector4 *rotation;
    u8 unmodeled_0c[4];
    Matrix4 basis;
} MEfObjCamParams;

int MEfObjDestroy(void *object);

#endif /* INCLUDE_MAIN_M_EF_OBJ_H */
