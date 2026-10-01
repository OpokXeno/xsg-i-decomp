/*
 * TU-local declarations of main/tu146 (src/main/transform_vector.c).
 */

#ifndef SRC_MAIN_TRANSFORM_VECTOR_H
#define SRC_MAIN_TRANSFORM_VECTOR_H

#include "shared.h"

extern void xglMatrixMul(Matrix4 destination, Matrix4 left, Matrix4 right);

extern void xglVectorMulMat(Vector4 *destination, Matrix4 matrix,
                            const Vector4 *vector);

extern void xglVectorMulAdd(Vector4 *destination, const Vector4 *left,
                            const Vector4 *right, const Vector4 *addend);

/*
 * transformVector/transformVector2's fourth argument: a buffer its callers
 * (still asm; e.g. drawRect2, 0x00265338) build per draw call. world is the
 * caller's saved matrix-stack top and local is the address of the caller's
 * own matrix; xglMatrixMul combines them into matrix, which every vector in
 * the batch is then multiplied against. screenScale and screenOffset are
 * the xglVectorMulAdd operands applied to each result after its perspective
 * divide, the same argument roles StudioCamera's own screenScale/
 * screenOffset fill at every other accepted xglVectorMulAdd call site
 * (src/ov01/gr_gp_init.c, src/main/m_math.c).
 */
typedef struct TransformParams {
    Matrix4 matrix;
    Matrix4 world;
    Vector4 screenScale;
    Vector4 screenOffset;
    Matrix4 *local;
} TransformParams;

void transformVector(Vector4 *destination, const Vector4 *vectors, int count,
                     TransformParams *params);

void transformVector2(Vector4 *destination, const Vector4 *vectors, int count,
                      TransformParams *params);

#endif /* SRC_MAIN_TRANSFORM_VECTOR_H */
