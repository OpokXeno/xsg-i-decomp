#include "xgl_vector_scalar.h"

void xglVectorScale(float scale, Vector4 *destination, const Vector4 *source)
{
    destination->x = source->x * scale;
    destination->y = source->y * scale;
    destination->z = source->z * scale;
    destination->w = source->w * scale;
}

void xglVectorScaleXYZ(float scale, Vector4 *destination, const Vector4 *source)
{
    destination->x = source->x * scale;
    destination->y = source->y * scale;
    destination->z = source->z * scale;
    destination->w = source->w;
}

void xglVectorScaleAdd(float scale, Vector4 *destination,
                       const Vector4 *source, const Vector4 *other)
{
    destination->x = source->x * scale + other->x;
    destination->y = source->y * scale + other->y;
    destination->z = source->z * scale + other->z;
    destination->w = source->w * scale + other->w;
}

void xglVectorScaleAddXYZ(float scale, Vector4 *destination,
                          const Vector4 *source, const Vector4 *other)
{
    destination->x = source->x * scale + other->x;
    destination->y = source->y * scale + other->y;
    destination->z = source->z * scale + other->z;
    destination->w = source->w;
}

void xglVectorScaleSub(float scale, Vector4 *destination,
                       const Vector4 *source, const Vector4 *other)
{
    destination->x = source->x * scale - other->x;
    destination->y = source->y * scale - other->y;
    destination->z = source->z * scale - other->z;
    destination->w = source->w * scale - other->w;
}

void xglVectorScaleSubXYZ(float scale, Vector4 *destination,
                          const Vector4 *source, const Vector4 *other)
{
    destination->x = source->x * scale - other->x;
    destination->y = source->y * scale - other->y;
    destination->z = source->z * scale - other->z;
    destination->w = source->w;
}

void xglVectorInter(float fraction, Vector4 *destination,
                    const Vector4 *start, const Vector4 *end)
{
    destination->x = (end->x - start->x) * fraction + start->x;
    destination->y = (end->y - start->y) * fraction + start->y;
    destination->z = (end->z - start->z) * fraction + start->z;
    destination->w = (end->w - start->w) * fraction + start->w;
}

void xglVectorInterXYZ(float fraction, Vector4 *destination,
                       const Vector4 *start, const Vector4 *end)
{
    destination->x = (end->x - start->x) * fraction + start->x;
    destination->y = (end->y - start->y) * fraction + start->y;
    destination->z = (end->z - start->z) * fraction + start->z;
    destination->w = end->w;
}

void xglVectorInner(float *result, const Vector4 *left, const Vector4 *right)
{
    float dot = left->x * right->x;
    *result = dot;
    dot += left->y * right->y;
    *result = dot;
    dot += left->z * right->z;
    *result = dot;
}

float xglVectorInner4(const Vector4 *left, const Vector4 *right)
{
    return left->x * right->x + left->y * right->y
         + left->z * right->z + left->w * right->w;
}

void xglVectorOuter(Vector4 *destination, const Vector4 *left,
                    const Vector4 *right)
{
    destination->x = left->y * right->z - left->z * right->y;
    destination->y = left->z * right->x - left->x * right->z;
    destination->z = left->x * right->y - left->y * right->x;
    destination->w = left->w;
}
