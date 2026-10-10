#include "common.h"

#include "shared.h"

#include "main/xgl_2.h"

#include "xgl_2.h"

extern unsigned int D_004ADE80[];

static unsigned long long iRandSeed;

extern void xglDmaDirectSrcChain(unsigned int channel, unsigned int address);

extern StudioCamera *xglStudioGetCamera2(int cameraId);

extern char Vu0CallSin[];

extern char Vu0CallCos[];

extern const int D_004D27C8[3];

float xglAtan2(float y, float x)
{
    int reciprocal = 0;
    int quadrant = 0;
    const float coefficient1 = 0.0161657371f;
    const float coefficient2 = 0.0429096147f;
    const float coefficient3 = 0.0752896369f;
    const float coefficient4 = 0.106562637f;
    const float coefficient5 = 0.142088994f;
    const float coefficient6 = 0.199935511f;
    const float coefficient7 = 0.333331466f;
    const float unitCoefficient = 1.0f;
    float polynomial = 0.00286622578f;
    float square;

    if (y == 0.0f) {
        if (x >= 0.0f) return 0.0f;
        __builtin_memcpy(&reciprocal, &y, sizeof reciprocal);
        if (reciprocal < 0) return -3.14159274f;
        return 3.14159274f;
    }
    if (x == 0.0f) {
        if (y < 0.0f) return -1.57079637f;
        return 1.57079637f;
    }
    if (y >= 0.0f) {
        if (x >= 0.0f) quadrant = 0;
        else quadrant = 1;
    } else {
        quadrant = 2;
        if (x >= 0.0f) quadrant = 2;
        else quadrant = 3;
    }
    y = __builtin_fabsf(y);
    x = __builtin_fabsf(x);
    if (x < y) {
        y = x / y;
        reciprocal = 1;
    } else {
        y = y / x;
    }
    square = y * y;
    polynomial *= square;
    polynomial -= coefficient1;
    polynomial *= square;
    polynomial += coefficient2;
    polynomial *= square;
    polynomial -= coefficient3;
    polynomial *= square;
    polynomial += coefficient4;
    polynomial *= square;
    polynomial -= coefficient5;
    polynomial *= square;
    polynomial += coefficient6;
    polynomial *= square;
    polynomial -= coefficient7;
    polynomial *= square;
    polynomial += unitCoefficient;
    polynomial *= y;
    if (reciprocal) polynomial = 1.57079637f - polynomial;
    if (quadrant == 1) polynomial = 3.14159274f - polynomial;
    if (quadrant == 2) polynomial = -polynomial;
    if (quadrant == 3) polynomial = polynomial - 3.14159274f;
    return polynomial;
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy64);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy64b);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy16);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy16b);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy8);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy8b);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy4);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy4b);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy2);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMemCopy2b);

void xglVectorDiv(Vector4 *destination, const Vector4 *numerator,
                  const Vector4 *divisor)
{
    destination->x = numerator->x / divisor->x;
    destination->y = numerator->y / divisor->y;
    destination->z = numerator->z / divisor->z;
    destination->w = numerator->w / divisor->w;
}

void xglVectorDivXYZ(Vector4 *destination, const Vector4 *numerator,
                     const Vector4 *divisor)
{
    destination->x = numerator->x / divisor->x;
    destination->y = numerator->y / divisor->y;
    destination->z = numerator->z / divisor->z;
    destination->w = numerator->w;
}

void xglVectorMulAdd(Vector4 *destination, const Vector4 *left,
                     const Vector4 *right, const Vector4 *addend)
{
    destination->x = left->x * right->x + addend->x;
    destination->y = left->y * right->y + addend->y;
    destination->z = left->z * right->z + addend->z;
    destination->w = left->w * right->w + addend->w;
}

void xglVectorMulAddXYZ(Vector4 *destination, const Vector4 *left,
                        const Vector4 *right, const Vector4 *addend)
{
    destination->x = left->x * right->x + addend->x;
    destination->y = left->y * right->y + addend->y;
    destination->z = left->z * right->z + addend->z;
    destination->w = left->w;
}

void xglVectorMulSub(Vector4 *destination, const Vector4 *left,
                     const Vector4 *right, const Vector4 *subtrahend)
{
    destination->x = left->x * right->x - subtrahend->x;
    destination->y = left->y * right->y - subtrahend->y;
    destination->z = left->z * right->z - subtrahend->z;
    destination->w = left->w * right->w - subtrahend->w;
}

void xglVectorMulSubXYZ(Vector4 *destination, const Vector4 *left,
                        const Vector4 *right, const Vector4 *subtrahend)
{
    destination->x = left->x * right->x - subtrahend->x;
    destination->y = left->y * right->y - subtrahend->y;
    destination->z = left->z * right->z - subtrahend->z;
    destination->w = left->w;
}

void xglVectorDivAdd(Vector4 *destination, const Vector4 *numerator,
                     const Vector4 *divisor, const Vector4 *addend)
{
    destination->x = numerator->x / divisor->x + addend->x;
    destination->y = numerator->y / divisor->y + addend->y;
    destination->z = numerator->z / divisor->z + addend->z;
    destination->w = numerator->w / divisor->w + addend->w;
}

void xglVectorDivAddXYZ(Vector4 *destination, const Vector4 *numerator,
                        const Vector4 *divisor, const Vector4 *addend)
{
    destination->x = numerator->x / divisor->x + addend->x;
    destination->y = numerator->y / divisor->y + addend->y;
    destination->z = numerator->z / divisor->z + addend->z;
    destination->w = numerator->w;
}

void xglVectorDivSub(Vector4 *destination, const Vector4 *numerator,
                     const Vector4 *divisor, const Vector4 *subtrahend)
{
    destination->x = numerator->x / divisor->x - subtrahend->x;
    destination->y = numerator->y / divisor->y - subtrahend->y;
    destination->z = numerator->z / divisor->z - subtrahend->z;
    destination->w = numerator->w / divisor->w - subtrahend->w;
}

void xglVectorDivSubXYZ(Vector4 *destination, const Vector4 *numerator,
                        const Vector4 *divisor, const Vector4 *subtrahend)
{
    destination->x = numerator->x / divisor->x - subtrahend->x;
    destination->y = numerator->y / divisor->y - subtrahend->y;
    destination->z = numerator->z / divisor->z - subtrahend->z;
    destination->w = numerator->w;
}

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

void xglVectorLength(float *destination, const Vector4 *vector)
{
    register float length asm("$2");

    __asm__ __volatile__(
        "lqc2 vf3, 0(%1)\n\t"
        "vmul.xyz vf3xyz, vf3xyz, vf3xyz\n\t"
        "vaddy.x vf3x, vf3x, vf3y\n\t"
        "vaddz.x vf3x, vf3x, vf3z\n\t"
        "vsqrt Q, vf3x\n\t"
        "vwaitq\n\t"
        "vaddq.x vf4x, vf0x, Q\n\t"
        "qmfc2 %0, vf4"
        : "=r"(length)
        : "r"(vector)
        : "memory"
    );
    *destination = length;
    __asm__ __volatile__("" : : "r"(destination));
}

float xglPointLength(const Vector4 *point1, const Vector4 *point2)
{
    float length;

    __asm__ __volatile__(
        "lqc2 vf2, 0(%1)\n\t"
        "lqc2 vf3, 0(%2)\n\t"
        "vsub.xyz vf2xyz, vf2xyz, vf3xyz\n\t"
        "vmul.xyz vf3xyz, vf2xyz, vf2xyz\n\t"
        "vaddy.x vf3x, vf3x, vf3y\n\t"
        "vaddz.x vf3x, vf3x, vf3z\n\t"
        "vsqrt Q, vf3x\n\t"
        "vwaitq\n\t"
        "vaddq.x vf4x, vf0x, Q\n\t"
        "qmfc2 %0, vf4"
        : "=r"(length)
        : "r"(point1), "r"(point2)
        : "memory"
    );
    return length;
}

float xglPointLengthXZ(const Vector4 *point1, const Vector4 *point2)
{
    float length;

    __asm__ __volatile__(
        "lqc2 vf2, 0(%1)\n\t"
        "lqc2 vf3, 0(%2)\n\t"
        "vsub.xyz vf2xyz, vf2xyz, vf3xyz\n\t"
        "vmul.xyz vf3xyz, vf2xyz, vf2xyz\n\t"
        "vaddz.x vf3x, vf3x, vf3z\n\t"
        "vsqrt Q, vf3x\n\t"
        "vwaitq\n\t"
        "vaddq.x vf4x, vf0x, Q\n\t"
        "qmfc2 %0, vf4"
        : "=r"(length)
        : "r"(point1), "r"(point2)
        : "memory"
    );
    return length;
}

void xglVectorNormal(Vector4 *destination, const Vector4 *source)
{
    __asm__ __volatile__(
        "lqc2 vf2, 0(%1)\n\t"
        "vmul.xyz vf3xyz, vf2xyz, vf2xyz\n\t"
        "vaddy.x vf3x, vf3x, vf3y\n\t"
        "vaddz.x vf3x, vf3x, vf3z\n\t"
        "vrsqrt Q, vf0w, vf3x\n\t"
        "vwaitq\n\t"
        "vmulq.xyz vf2xyz, vf2xyz, Q\n\t"
        "sqc2 vf2, 0(%0)\n\t"
        "nop\n\t"
        :
        : "r"(destination), "r"(source)
        : "memory"
    );
}

void xglVectorClamp(Vector4 *destination, const Vector4 *source,
                    float minimum, float maximum)
{
    destination->x = source->x < minimum ? minimum
                    : maximum < source->x ? maximum : source->x;
    destination->y = source->y < minimum ? minimum
                    : maximum < source->y ? maximum : source->y;
    destination->z = source->z < minimum ? minimum
                    : maximum < source->z ? maximum : source->z;
    destination->w = source->w < minimum ? minimum
                    : maximum < source->w ? maximum : source->w;
}

void xglVectorClampXYZ(Vector4 *destination, const Vector4 *source,
                       float minimum, float maximum)
{
    destination->x = source->x < minimum ? minimum
                    : maximum < source->x ? maximum : source->x;
    destination->y = source->y < minimum ? minimum
                    : maximum < source->y ? maximum : source->y;
    destination->z = source->z < minimum ? minimum
                    : maximum < source->z ? maximum : source->z;
    destination->w = source->w;
}

void xglPlaneParameter(Vector4 *destination, const Vector4 *p0,
                       const Vector4 *p1, const Vector4 *p2)
{
    Vector4 edge0;
    Vector4 edge1;

    __asm__ __volatile__(
        "lqc2 vf3, 0(%1)\n\t"
        "lqc2 vf2, 0(%2)\n\t"
        "vsub.xyz vf2xyz, vf2xyz, vf3xyz\n\t"
        "sqc2 vf2, 0(%0)\n\t"
        :
        : "r"(&edge0), "r"(p0), "r"(p1)
        : "memory"
    );

    __asm__ __volatile__(
        "lqc2 vf3, 0(%1)\n\t"
        "lqc2 vf2, 0(%2)\n\t"
        "vsub.xyz vf2xyz, vf2xyz, vf3xyz\n\t"
        "sqc2 vf2, 0(%0)\n\t"
        :
        : "r"(&edge1), "r"(p1), "r"(p2)
        : "memory"
    );

    xglVectorOuter(destination, &edge0, &edge1);
    xglVectorNormal(destination, destination);
    xglVectorInner(&destination->w, destination, p0);
    destination->w = -destination->w;
}

void xglVectorMulMat(Vector4 *destination, const Matrix4 matrix,
                     const Vector4 *vector)
{
    __asm__ __volatile__(
        "lqc2 vf1, 0(%2)\n\t"
        "lqc2 vf27, 0(%1)\n\t"
        "lqc2 vf28, 16(%1)\n\t"
        "lqc2 vf29, 32(%1)\n\t"
        "lqc2 vf30, 48(%1)\n\t"
        "vmulax.xyzw ACCxyzw, vf27xyzw, vf1x\n\t"
        "vmadday.xyzw ACCxyzw, vf28xyzw, vf1y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf29xyzw, vf1z\n\t"
        "vmaddw.xyzw vf31xyzw, vf30xyzw, vf1w\n\t"
        "sqc2 vf31, 0(%0)\n\t"
        "nop\n\t"
        :
        : "r"(destination), "r"(matrix), "r"(vector)
        : "memory"
    );
}

float xglRotTransPers(Vector4 *out, const Matrix4 matrix, Vector4 *point, int cameraId)
{
    StudioCamera *camera;

    xglMatrixStackPush();
    camera = xglStudioGetCamera2(cameraId);
    xglMatrixStackLoad(camera->viewMatrix);
    if (matrix != 0) {
        xglMatrixStackMul(matrix);
    }
    xglMatrixStackRTPS(out, point, &camera->screenScale, &camera->screenOffset);
    xglMatrixStackPop(1);
    return out->w;
}

void xglRotTransPersN(Vector4 *destination, const Matrix4 matrix,
                      const Vector4 *points, int count, int cameraId)
{
    StudioCamera *camera;

    xglMatrixStackPush();
    camera = xglStudioGetCamera2(cameraId);
    xglMatrixStackLoad(camera->viewMatrix);
    if (matrix != 0) {
        xglMatrixStackMul(matrix);
    }
    while (count > 0) {
        xglMatrixStackRTPS(destination, points,
                           &camera->screenScale, &camera->screenOffset);
        ++destination;
        ++points;
        --count;
    }
    xglMatrixStackPop(1);
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMatrixUnit);

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMatrixUnit4s);

void xglMatrixMul(Matrix *destination, Matrix *left, Matrix *right)
{
    __asm__ __volatile__(
        "lqc2 vf2, 0(%2)\n\t"
        "lqc2 vf3, 16(%2)\n\t"
        "lqc2 vf4, 32(%2)\n\t"
        "lqc2 vf5, 48(%2)\n\t"
        "lqc2 vf27, 0(%1)\n\t"
        "lqc2 vf28, 16(%1)\n\t"
        "lqc2 vf29, 32(%1)\n\t"
        "lqc2 vf30, 48(%1)\n\t"
        "vmulax.xyzw ACCxyzw, vf27xyzw, vf2x\n\t"
        "vmadday.xyzw ACCxyzw, vf28xyzw, vf2y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf29xyzw, vf2z\n\t"
        "vmaddw.xyzw vf2xyzw, vf30xyzw, vf2w\n\t"
        "vmulax.xyzw ACCxyzw, vf27xyzw, vf3x\n\t"
        "vmadday.xyzw ACCxyzw, vf28xyzw, vf3y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf29xyzw, vf3z\n\t"
        "vmaddw.xyzw vf3xyzw, vf30xyzw, vf3w\n\t"
        "vmulax.xyzw ACCxyzw, vf27xyzw, vf4x\n\t"
        "vmadday.xyzw ACCxyzw, vf28xyzw, vf4y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf29xyzw, vf4z\n\t"
        "vmaddw.xyzw vf4xyzw, vf30xyzw, vf4w\n\t"
        "vmulax.xyzw ACCxyzw, vf27xyzw, vf5x\n\t"
        "vmadday.xyzw ACCxyzw, vf28xyzw, vf5y\n\t"
        "vmaddaz.xyzw ACCxyzw, vf29xyzw, vf5z\n\t"
        "vmaddw.xyzw vf30xyzw, vf30xyzw, vf5w\n\t"
        "sqc2 vf2, 0(%0)\n\t"
        "sqc2 vf3, 16(%0)\n\t"
        "sqc2 vf4, 32(%0)\n\t"
        "sqc2 vf30, 48(%0)\n\t"
        "nop\n\t"
        :
        : "r"(destination), "r"(left), "r"(right)
        : "memory"
    );
}

void xglMatrixReverse(Matrix *destination, const Matrix *source)
{
    Matrix transposed;
    int row;
    int column;

    for (row = 0; row < 4; ++row) {
        for (column = 0; column < 4; ++column) {
            transposed.elements[row * 4 + column] =
                source->elements[column * 4 + row];
        }
    }
    /* The original SDK copy reuses scratch $2 for all four rows. */
    __asm__ __volatile__(
        "lq $2,0(%1)\n\t"
        "sq $2,0(%0)\n\t"
        "lq $2,16(%1)\n\t"
        "sq $2,16(%0)\n\t"
        "lq $2,32(%1)\n\t"
        "sq $2,32(%0)\n\t"
        "lq $2,48(%1)\n\t"
        "sq $2,48(%0)"
        :
        : "r"(destination), "r"(&transposed)
        : "$2", "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", xglMatrixInverse);

void xglMatrixScale(Matrix4 destination, const Matrix4 source,
                    const float scale[4])
{
    int column;

    for (column = 0; column < 4; ++column) {
        destination[0][column] = source[0][column] * scale[0];
        destination[1][column] = source[1][column] * scale[1];
        destination[2][column] = source[2][column] * scale[2];
        destination[3][column] = source[3][column] * scale[3];
    }
}

void xglMatrixTrans(Matrix4 destination, const Matrix4 source,
                    const float translation[4])
{
    int column;

    for (column = 0; column < 4; ++column) {
        destination[0][column] = source[0][column];
        destination[1][column] = source[1][column];
        destination[2][column] = source[2][column];
        destination[3][column] = source[0][column] * translation[0]
                                + source[1][column] * translation[1]
                                + source[2][column] * translation[2]
                                + source[3][column] * translation[3];
    }
}

void xglMatrixRotV(Matrix *destination, Matrix *source, Vector4 *axis,
                   float angle)
{
    Vector4 oneMinusCosAxis;
    Vector4 sinAxis;
    Matrix rotation;
    float cosineForScale;
    float sine;
    float cosineForRow0;
    float cosineForRow1;
    float cosineForRow2;
    float ax;
    float ay;
    float az;
    float oneMinusCosX;
    float oneMinusCosY;
    float oneMinusCosZ;
    float sinX;
    float sinY;
    float sinZ;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosineForScale)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    xglVectorScaleXYZ(1.0f - cosineForScale, &oneMinusCosAxis, axis);

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(sine)
        : "r"((unsigned int)Vu0CallSin / 8), "r"(angle)
        : "memory"
    );
    xglVectorScaleXYZ(sine, &sinAxis, axis);

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosineForRow0)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    ax = axis->x;
    oneMinusCosX = oneMinusCosAxis.x;
    oneMinusCosY = oneMinusCosAxis.y;
    oneMinusCosZ = oneMinusCosAxis.z;
    ay = axis->y;
    sinY = sinAxis.y;
    sinZ = sinAxis.z;
    rotation.elements[0] = oneMinusCosX * ax + cosineForRow0;
    rotation.elements[1] = oneMinusCosY * ax + sinZ;
    rotation.elements[2] = oneMinusCosZ * ax - sinY;
    rotation.elements[3] = 0.0f;
    rotation.elements[4] = oneMinusCosX * ay - sinZ;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosineForRow1)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    sinX = sinAxis.x;
    az = axis->z;
    rotation.elements[5] = oneMinusCosY * ay + cosineForRow1;
    rotation.elements[6] = oneMinusCosZ * ay + sinX;
    rotation.elements[7] = 0.0f;
    rotation.elements[8] = oneMinusCosX * az + sinY;
    rotation.elements[9] = oneMinusCosY * az - sinX;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosineForRow2)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    rotation.elements[10] = oneMinusCosZ * az + cosineForRow2;
    rotation.elements[11] = 0.0f;
    rotation.elements[12] = 0.0f;
    rotation.elements[13] = 0.0f;
    rotation.elements[14] = 0.0f;
    rotation.elements[15] = 1.0f;

    xglMatrixMul(destination, source, &rotation);
}

void xglMatrixRotX(Matrix4 destination, const Matrix4 source, float angle)
{
    float cosSin[4];
    float *pair = cosSin;
    float sine;
    float cosine;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(sine)
        : "r"((unsigned int)Vu0CallSin / 8), "r"(angle)
        : "memory"
    );
    cosSin[1] = sine;
    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosine)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    cosSin[0] = cosine;

    __asm__ __volatile__(
        "lq $2,0(%1)\n\t"
        "lqc2 vf28,16(%1)\n\t"
        "lqc2 vf29,32(%1)\n\t"
        "lqc2 vf1,0(%2)\n\t"
        "sq $2,0(%0)\n\t"
        "lq $2,48(%1)\n\t"
        "sq $2,48(%0)\n\t"
        "vmulax.xyzw ACCxyzw,vf28xyzw,vf1x\n\t"
        "vmaddy.xyzw vf27xyzw,vf29xyzw,vf1y\n\t"
        "vmulax.xyzw ACCxyzw,vf29xyzw,vf1x\n\t"
        "vmsuby.xyzw vf30xyzw,vf28xyzw,vf1y\n\t"
        "sqc2 vf27,16(%0)\n\t"
        "sqc2 vf30,32(%0)"
        :
        : "r"(destination), "r"(source), "r"(pair)
        : "$2", "memory"
    );
}

void xglMatrixRotY(Matrix4 destination, const Matrix4 source, float angle)
{
    float cosSin[4];
    float *pair = cosSin;
    float sine;
    float cosine;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(sine)
        : "r"((unsigned int)Vu0CallSin / 8), "r"(angle)
        : "memory"
    );
    cosSin[1] = sine;
    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosine)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    cosSin[0] = cosine;

    __asm__ __volatile__(
        "lq $2,16(%1)\n\t"
        "lqc2 vf27,0(%1)\n\t"
        "lqc2 vf29,32(%1)\n\t"
        "lqc2 vf1,0(%2)\n\t"
        "sq $2,16(%0)\n\t"
        "lq $2,48(%1)\n\t"
        "sq $2,48(%0)\n\t"
        "vmulax.xyzw ACCxyzw,vf27xyzw,vf1x\n\t"
        "vmsuby.xyzw vf28xyzw,vf29xyzw,vf1y\n\t"
        "vmulax.xyzw ACCxyzw,vf29xyzw,vf1x\n\t"
        "vmaddy.xyzw vf30xyzw,vf27xyzw,vf1y\n\t"
        "sqc2 vf28,0(%0)\n\t"
        "sqc2 vf30,32(%0)"
        :
        : "r"(destination), "r"(source), "r"(pair)
        : "$2", "memory"
    );
}

void xglMatrixRotZ(Matrix4 destination, const Matrix4 source, float angle)
{
    float cosSin[4];
    float *pair = cosSin;
    float sine;
    float cosine;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(sine)
        : "r"((unsigned int)Vu0CallSin / 8), "r"(angle)
        : "memory"
    );
    cosSin[1] = sine;
    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,vf1"
        : "=r"(cosine)
        : "r"((unsigned int)Vu0CallCos / 8), "r"(angle)
        : "memory"
    );
    cosSin[0] = cosine;

    __asm__ __volatile__(
        "lq $2,32(%1)\n\t"
        "lqc2 vf27,0(%1)\n\t"
        "lqc2 vf28,16(%1)\n\t"
        "lqc2 vf1,0(%2)\n\t"
        "sq $2,32(%0)\n\t"
        "lq $2,48(%1)\n\t"
        "sq $2,48(%0)\n\t"
        "vmulax.xyzw ACCxyzw,vf27xyzw,vf1x\n\t"
        "vmaddy.xyzw vf29xyzw,vf28xyzw,vf1y\n\t"
        "vmulax.xyzw ACCxyzw,vf28xyzw,vf1x\n\t"
        "vmsuby.xyzw vf30xyzw,vf27xyzw,vf1y\n\t"
        "sqc2 vf29,0(%0)\n\t"
        "sqc2 vf30,16(%0)"
        :
        : "r"(destination), "r"(source), "r"(pair)
        : "$2", "memory"
    );
}

void xglMatrixFrustum(Matrix *destination, Matrix *source,
                      float left, float right, float bottom, float top,
                      float nearPlane, float farPlane)
{
    Matrix projection;

    projection.elements[0] = (2.0f * nearPlane) / (right - left);
    projection.elements[1] = 0.0f;
    projection.elements[2] = 0.0f;
    projection.elements[3] = 0.0f;
    projection.elements[4] = 0.0f;
    projection.elements[5] = (2.0f * nearPlane) / (top - bottom);
    projection.elements[6] = 0.0f;
    projection.elements[7] = 0.0f;
    projection.elements[8] = (right + left) / (right - left);
    projection.elements[9] = (top + bottom) / (top - bottom);
    projection.elements[10] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    projection.elements[11] = -1.0f;
    projection.elements[12] = 0.0f;
    projection.elements[13] = 0.0f;
    projection.elements[14] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
    projection.elements[15] = 0.0f;
    xglMatrixMul(destination, source, &projection);
}

void xglMatrixStackUnit(void)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackUnit >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry)
        : "memory"
    );
}

void xglMatrixStackMul(const Matrix4 matrix)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackMul >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "lqc2 $vf4,0(%1)\n\t"
        "lqc2 $vf5,16(%1)\n\t"
        "lqc2 $vf6,32(%1)\n\t"
        "lqc2 $vf7,48(%1)\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(matrix)
        : "memory"
    );
}

void xglMatrixStackReverse(void)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackReverse >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry)
        : "memory"
    );
}

void xglMatrixStackInverse(void)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackInverse >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry)
        : "memory"
    );
}

void xglMatrixStackScale(const float scale[4])
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackScale >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "lqc2 $vf4,0(%1)\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(scale)
        : "memory"
    );
}

void xglMatrixStackTrans(const float translation[4])
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackTrans >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "lqc2 $vf4,0(%1)\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(translation)
        : "memory"
    );
}

void xglMatrixStackRotV(const float axis[4], float angle)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackRotV >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "lqc2 $vf5,0(%1)\n\t"
        "qmtc2 %2,$vf4\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(axis), "r"(angle)
        : "memory"
    );
}

void xglMatrixStackRotX(float angle)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackRotX >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %1,$vf4\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(angle)
        : "memory"
    );
}

void xglMatrixStackRotY(float angle)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackRotY >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %1,$vf4\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(angle)
        : "memory"
    );
}

void xglMatrixStackRotZ(float angle)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackRotZ >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %1,$vf4\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(angle)
        : "memory"
    );
}

void xglMatrixStackFrustum(float left, float right, float bottom, float top,
                           float nearPlane, float farPlane)
{
    /* the quadword pair in VU0 data memory the microprogram reads */
    volatile float *window = (volatile float *)0x11004800;
    unsigned int entry = (unsigned int)Vu0CallMatrixStackFrustum >> 3;

    __asm__ __volatile__("ctc2.i %0,$vi27" : : "r"(entry) : "memory");
    window[0] = left;
    window[1] = bottom;
    window[2] = nearPlane;
    window[4] = right;
    window[5] = top;
    window[6] = farPlane;
    ((volatile int *)window)[7] = 0;
    ((volatile int *)window)[3] = 0;
    __asm__ __volatile__("sync" : : : "memory");
    __asm__ __volatile__("vcallmsr $vi27\n\t"
                         "nop"
                         : : : "memory");
}

void xglMatrixStackSave(float matrix[4][4])
{
    __asm__ __volatile__(
        "ctc2.i %0,$vi0\n\t"
        "sqc2 $vf28,0(%0)\n\t"
        "sqc2 $vf29,16(%0)\n\t"
        "sqc2 $vf30,32(%0)\n\t"
        "sqc2 $vf31,48(%0)\n\t"
        "nop"
        :
        : "r"(matrix)
        : "memory"
    );
}

void xglMatrixStackLoad(float matrix[4][4])
{
    __asm__ __volatile__(
        "ctc2.i %0,$vi0\n\t"
        "lqc2 $vf28,0(%0)\n\t"
        "lqc2 $vf29,16(%0)\n\t"
        "lqc2 $vf30,32(%0)\n\t"
        "lqc2 $vf31,48(%0)\n\t"
        "nop"
        :
        : "r"(matrix)
        : "memory"
    );
}

void xglMatrixStackPushUnit(void)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackPushUnit >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry)
        : "memory"
    );
}

void xglMatrixStackPush(void)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackPush >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry)
        : "memory"
    );
}

void xglMatrixStackPop(int count)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackPop >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "ctc2 %1,$vi1\n\t"
        "vcallmsr $vi27\n\t"
        "nop"
        :
        : "r"(entry), "r"(count)
        : "memory"
    );
}

void xglMatrixStackMulVector(Vector4 *out, const Vector4 *vector)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackMulVector >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "vnop\n\t"
        "lqc2 $vf4,0(%2)\n\t"
        "vcallmsr $vi27\n\t"
        "cfc2.i $0,$vi0\n\t"
        "sqc2 $vf5,0(%1)\n\t"
        "nop"
        :
        : "r"(entry), "r"(out), "r"(vector)
        : "memory"
    );
}

void xglMatrixStackRTPS(Vector4 *out, const Vector4 *point,
                        const Vector4 *scale, const Vector4 *offset)
{
    unsigned int entry = (unsigned int)Vu0CallMatrixStackRTPS >> 3;

    __asm__ __volatile__(
        "ctc2.i %0,$vi27\n\t"
        "lqc2 $vf4,0(%2)\n\t"
        "lqc2 $vf5,0(%3)\n\t"
        "lqc2 $vf6,0(%4)\n\t"
        "vcallmsr $vi27\n\t"
        "cfc2.i $0,$vi0\n\t"
        "sqc2 $vf7,0(%1)\n\t"
        "nop"
        :
        : "r"(entry), "r"(out), "r"(point), "r"(scale), "r"(offset)
        : "memory"
    );
}

void xglMatrix2Quaternion(float quaternion[4], const Matrix *matrix)
{
    float diagonalX = matrix->elements[0];
    float diagonalY = matrix->elements[5];
    float diagonalZ = matrix->elements[10];
    float root;
    float scale;
    int largestAxis;
    int nextAxis;
    int lastAxis;
    int nextAxes[3];
    float *largestComponent;
    float *nextComponent;
    float *lastComponent;
    scale = (diagonalX + diagonalY) + diagonalZ;
    if (scale > 0.0f) {
        scale = __builtin_sqrtf(scale + 1.0f);
        root = 0.5f * scale;
        quaternion[3] = root;
        scale = 0.5f / (root + root);
        quaternion[0] = (matrix->elements[6] - matrix->elements[9]) * scale;
        quaternion[1] = (matrix->elements[8] - matrix->elements[2]) * scale;
        quaternion[2] = (matrix->elements[1] - matrix->elements[4]) * scale;
    } else {
        __builtin_memcpy(nextAxes, D_004D27C8, sizeof(nextAxes));
        largestAxis = 0;
        if (diagonalX < diagonalY) largestAxis = 1;
        if (matrix->elements[5 * largestAxis] < diagonalZ) largestAxis = 2;
        nextAxis = nextAxes[largestAxis];
        lastAxis = nextAxes[nextAxis];
        largestComponent = &quaternion[largestAxis];
        root = 0.5f * __builtin_sqrtf(matrix->elements[5 * largestAxis] - (matrix->elements[5 * nextAxis] + matrix->elements[5 * lastAxis]) + 1.0f);
        *largestComponent = root;
        nextComponent = &quaternion[nextAxis];
        lastComponent = &quaternion[lastAxis];
        scale = 0.5f / (root + root);
        quaternion[3] = (matrix->elements[4 * nextAxis + lastAxis] - matrix->elements[4 * lastAxis + nextAxis]) * scale;
        *nextComponent = (matrix->elements[4 * largestAxis + nextAxis] + matrix->elements[4 * nextAxis + largestAxis]) * scale;
        *lastComponent = (matrix->elements[4 * largestAxis + lastAxis] + matrix->elements[4 * lastAxis + largestAxis]) * scale;
    }
}

void xglQuaternion2Matrix(Matrix4 destination, const Vector4 *quaternion)
{
    __asm__ __volatile__(
        "lqc2 vf31, 0(%1)\n\t"
        "vaddw.x vf27x, vf0x, vf0w\n\t"
        "vaddw.y vf28y, vf0y, vf0w\n\t"
        "vaddw.z vf29z, vf0z, vf0w\n\t"
        "vadd.xyzw vf14xyzw, vf31xyzw, vf31xyzw\n\t"
        "vsub.w vf27w, vf27w, vf27w\n\t"
        "vsub.w vf28w, vf28w, vf28w\n\t"
        "vsub.w vf29w, vf29w, vf29w\n\t"
        "vmulx.xyzw vf15xyzw, vf31xyzw, vf14x\n\t"
        "vmuly.xyzw vf16xyzw, vf31xyzw, vf14y\n\t"
        "vmulz.xyzw vf17xyzw, vf31xyzw, vf14z\n\t"
        "vnop\n\t"
        "vnop\n\t"
        "vaddy.x vf20x, vf15x, vf16y\n\t"
        "vaddz.y vf20y, vf16y, vf17z\n\t"
        "vaddx.z vf20z, vf17z, vf15x\n\t"
        "vnop\n\t"
        "vsubw.z vf27z, vf15z, vf16w\n\t"
        "vaddw.y vf27y, vf15y, vf17w\n\t"
        "vsuby.x vf27x, vf27x, vf20y\n\t"
        "vsubw.x vf28x, vf16x, vf17w\n\t"
        "vaddw.z vf28z, vf16z, vf15w\n\t"
        "vsubz.y vf28y, vf28y, vf20z\n\t"
        "vsubw.y vf29y, vf17y, vf15w\n\t"
        "vaddw.x vf29x, vf17x, vf16w\n\t"
        "vsubx.z vf29z, vf29z, vf20x\n\t"
        "sqc2 vf0, 48(%0)\n\t"
        "sqc2 vf27, 0(%0)\n\t"
        "sqc2 vf28, 16(%0)\n\t"
        "sqc2 vf29, 32(%0)\n\t"
        "nop"
        :
        : "r"(destination), "r"(quaternion)
        : "memory"
    );
}

ACCEPTED_ASM("src/main/xgl_2", quat);

void xglRandSeedInit(void)
{
    iRandSeed = 0x12345678ULL;
}

void xglGeometryInit(void)
{
    float random_seed = 0.1f;

    __asm__ __volatile__(
        "qmtc2 %0,vf1\n\t"
        "vrinit R,vf1x"
        :
        : "r"(random_seed)
        : "memory"
    );
    iRandSeed = 0x12345678ULL;
    xglDmaDirectSrcChain(0, (unsigned int)D_004ADE80);
}

unsigned short xglSRand(void)
{
    iRandSeed = iRandSeed * 1103515245ULL + 12345ULL;
    return (unsigned short)((iRandSeed >> 16) & 0x7fff);
}

unsigned int xglLRand(void)
{
    int first = iRandSeed * 1103515245LL + 12345LL;
    unsigned long long next = (unsigned int)(first * 1103515245 + 12345);

    iRandSeed = next;
    return (unsigned int)(first << 16) + (unsigned int)(next >> 16);
}

float xglFRand(void)
{
    float value;

    __asm__ __volatile__(
        "vrnext.x vf1, R\n\t"
        "qmfc2.ni %0, vf1"
        : "=r"(value)
        :
        : "memory"
    );
    return value;
}

void xglFSrand(float seed)
{
    __asm__ __volatile__(
        "qmtc2.ni %0, vf1\n\t"
        "vrinit R, vf1x\n\t"
        "nop"
        :
        : "r"(seed)
        : "memory"
    );
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_2", F2I);

float I2F(int value)
{
    return (float) value;
}

float xglSin(float angle)
{
    unsigned int entry = (unsigned int)Vu0CallSin >> 3;
    float result;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,$vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,$vf1"
        : "=r"(result)
        : "r"(entry), "r"(angle)
        : "memory"
    );
    return result;
}

float xglCos(float angle)
{
    unsigned int entry = (unsigned int)Vu0CallCos >> 3;
    float result;

    __asm__ __volatile__(
        "ctc2.i %1,$vi27\n\t"
        "vnop\n\t"
        "qmtc2 %2,$vf4\n\t"
        "vcallmsr $vi27\n\t"
        "qmfc2.i %0,$vf1"
        : "=r"(result)
        : "r"(entry), "r"(angle)
        : "memory"
    );
    return result;
}
