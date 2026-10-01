#include "common.h"
#include "shared.h"
#include "transform_vector.h"

void transformVector(Vector4 *destination, const Vector4 *vectors, int count,
                     TransformParams *params)
{
    Vector4 *offset = &params->screenOffset;
    Vector4 *scale = &params->screenScale;
    int remaining = count;
    float invW;

    xglMatrixMul(params->matrix, params->world, *params->local);
    if (remaining > 0) {
        do {
            xglVectorMulMat(destination, params->matrix, vectors);
            invW = 1.0f / destination->w;
            destination->x *= invW;
            destination->y *= invW;
            destination->z *= invW;
            destination->w *= invW;
            xglVectorMulAdd(destination, destination, scale, offset);
            vectors++;
            destination++;
        } while (--remaining);
    }
}

void transformVector2(Vector4 *destination, const Vector4 *vectors, int count,
                      TransformParams *params)
{
    Vector4 *offset = &params->screenOffset;
    Vector4 *scale = &params->screenScale;
    int remaining = count;
    float invW;

    xglMatrixMul(params->matrix, params->world, *params->local);
    if (remaining > 0) {
        do {
            xglVectorMulMat(destination, params->matrix, vectors);
            invW = 1.0f / destination->w;
            destination->x *= invW;
            destination->y *= invW;
            destination->w *= invW;
            xglVectorMulAdd(destination, destination, scale, offset);
            vectors++;
            destination++;
        } while (--remaining);
    }
}
