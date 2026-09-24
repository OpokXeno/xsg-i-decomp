/*
 * TU-local declarations of main/tu220 (src/main/m_ef_obj.c).
 */

#ifndef SRC_MAIN_M_EF_OBJ_H
#define SRC_MAIN_M_EF_OBJ_H

#include "shared.h"

/*
 * MMathRotateMatrixYXZ (main:0x002efbb0): defined as C in src/main/m_math.c,
 * declared TU-locally here the same way ov01/tu022 (src/ov01/m_ef.h)
 * declares other main math-library entry points until main's own TU claims
 * them.
 */
extern Matrix4 *MMathRotateMatrixYXZ(Matrix4 *out, const Matrix4 *matrix, const Vector4 *angles);

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

extern MEfObjCamParams mefCamParams;

#endif /* SRC_MAIN_M_EF_OBJ_H */
