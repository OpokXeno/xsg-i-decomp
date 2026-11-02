/*
 * TU-local declarations of main/tu146 (src/main/transform_vector.c).
 */

#ifndef SRC_MAIN_TRANSFORM_VECTOR_H
#define SRC_MAIN_TRANSFORM_VECTOR_H

#include "shared.h"
#include "main/transform_vector.h"
#include "main/xgl_2.h"

extern void xglMatrixMul(Matrix4 destination, Matrix4 left, Matrix4 right);

extern void xglVectorMulMat(Vector4 *destination, Matrix4 matrix,
                            const Vector4 *vector);

void transformVector2(Vector4 *destination, const Vector4 *vectors, int count,
                      TransformParams *params);

#endif /* SRC_MAIN_TRANSFORM_VECTOR_H */
