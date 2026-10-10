#include "common.h"

#include "shared.h"

#include "matrix_print.h"

extern int printf(const char *format, ...);

const char D_004C23D8[] = "\t%8.3f %8.3f %8.3f %8.3f\n";

void MATRIX_rotate4(Matrix4 rotation, float angle, float axisX, float axisY, float axisZ);

static void MATRIX_mul3x4(Matrix4 destination, Matrix4 source, Matrix4 rotation);

void MATRIX_mul4sx3(Matrix4 destination, Matrix4 source, Matrix4 rotation);

int MATRIX_print(const float matrix[4][4])
{
    int result;
    int rowsRemaining;
    const float *rowValues;

    rowsRemaining = 3;
    rowValues = &matrix[0][0];
    do {
        rowsRemaining--;
        result = printf(D_004C23D8, rowValues[0], rowValues[1],
                        rowValues[2], rowValues[3]);
        rowValues += 4;
    } while (rowsRemaining >= 0);
    return result;
}

void MATRIX_identity(float destination[4][4])
{
    float zero = 0.0f;
    float one = 1.0f;

    destination[3][3] = one;
    destination[2][2] = one;
    destination[1][1] = one;
    destination[3][2] = zero;
    destination[3][1] = zero;
    destination[3][0] = zero;
    destination[2][3] = zero;
    destination[2][1] = zero;
    destination[2][0] = zero;
    destination[1][3] = zero;
    destination[1][2] = zero;
    destination[1][0] = zero;
    destination[0][3] = zero;
    destination[0][2] = zero;
    destination[0][0] = one;
    destination[0][1] = zero;
}

void MATRIX_identity4s(float destination[4][4])
{
    float zero = 0.0f;
    float one = 1.0f;

    destination[3][3] = one;
    destination[2][3] = one;
    destination[2][2] = one;
    destination[1][3] = one;
    destination[1][1] = one;
    destination[0][3] = one;
    destination[3][2] = zero;
    destination[3][1] = zero;
    destination[3][0] = zero;
    destination[2][1] = zero;
    destination[2][0] = zero;
    destination[1][2] = zero;
    destination[1][0] = zero;
    destination[0][2] = zero;
    destination[0][0] = one;
    destination[0][1] = zero;
}

void MATRIX_convert4(f32 (*dst)[4], f32 (*src)[4]) {
    f32 zero = 0.0f;
    dst[0][0] = src[0][3] * src[0][0];
    dst[0][1] = src[0][3] * src[0][1];
    dst[0][2] = src[0][3] * src[0][2];
    dst[1][0] = src[1][3] * src[1][0];
    dst[1][1] = src[1][3] * src[1][1];
    dst[1][2] = src[1][3] * src[1][2];
    dst[2][0] = src[2][3] * src[2][0];
    dst[2][1] = src[2][3] * src[2][1];
    dst[2][2] = src[2][3] * src[2][2];
    dst[2][3] = zero;
    dst[1][3] = zero;
    dst[0][3] = zero;
    dst[3][0] = src[3][0];
    dst[3][1] = src[3][1];
    dst[3][2] = src[3][2];
    dst[3][3] = src[3][3];
}

void MATRIX_convert4s(Matrix4 destination, const Matrix4 source)
{
    destination[0][0] = source[0][0];
    destination[0][1] = source[0][1];
    destination[0][2] = source[0][2];
    destination[0][3] = __builtin_sqrtf(source[0][0] * source[0][0] + source[0][1] * source[0][1] + source[0][2] * source[0][2]);

    destination[1][0] = source[1][0];
    destination[1][1] = source[1][1];
    destination[1][2] = source[1][2];
    destination[1][3] = __builtin_sqrtf(source[1][0] * source[1][0] + source[1][1] * source[1][1] + source[1][2] * source[1][2]);

    destination[2][0] = source[2][0];
    destination[2][1] = source[2][1];
    destination[2][2] = source[2][2];
    destination[2][3] = __builtin_sqrtf(source[2][0] * source[2][0] + source[2][1] * source[2][1] + source[2][2] * source[2][2]);

    destination[3][0] = source[3][0];
    destination[3][1] = source[3][1];
    destination[3][2] = source[3][2];
    destination[3][3] = source[3][3];
}

void MATRIX_convert4MulMatrix(Matrix4 destination, Matrix4 matrix, Matrix4 other)
{
    __asm__ __volatile__(
        "lqc2 vf27, 0(%0)\n\t"
        "lqc2 vf28, 16(%0)\n\t"
        "lqc2 vf29, 32(%0)\n\t"
        "lqc2 vf30, 48(%0)\n\t"
        "vmulw.xyz vf27, vf27, vf27w\n\t"
        "vmulw.xyz vf28, vf28, vf28w\n\t"
        "vmulw.xyz vf29, vf29, vf29w\n\t"
        "vsub.w vf27, vf27, vf27\n\t"
        "vsub.w vf28, vf28, vf28\n\t"
        "vsub.w vf29, vf29, vf29\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "lqc2 vf3, 16(%1)\n\t"
        "lqc2 vf4, 32(%1)\n\t"
        "lqc2 vf5, 48(%1)\n\t"
        "sqc2 vf27, 0(%0)\n\t"
        "sqc2 vf28, 16(%0)\n\t"
        "sqc2 vf29, 32(%0)\n\t"
        "sqc2 vf30, 48(%0)\n\t"
        "vmulax.xyzw ACC, vf27, vf2x\n\t"
        "vmadday.xyzw ACC, vf28, vf2y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf2z\n\t"
        "vmaddw.xyzw vf2, vf30, vf2w\n\t"
        "vmulax.xyzw ACC, vf27, vf3x\n\t"
        "vmadday.xyzw ACC, vf28, vf3y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf3z\n\t"
        "vmaddw.xyzw vf3, vf30, vf3w\n\t"
        "vmulax.xyzw ACC, vf27, vf4x\n\t"
        "vmadday.xyzw ACC, vf28, vf4y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf4z\n\t"
        "vmaddw.xyzw vf4, vf30, vf4w\n\t"
        "vmulax.xyzw ACC, vf27, vf5x\n\t"
        "vmadday.xyzw ACC, vf28, vf5y\n\t"
        "vmaddaz.xyzw ACC, vf29, vf5z\n\t"
        "vmaddw.xyzw vf30, vf30, vf5w\n\t"
        "sqc2 vf2, 0(%2)\n\t"
        "sqc2 vf3, 16(%2)\n\t"
        "sqc2 vf4, 32(%2)\n\t"
        "sqc2 vf30, 48(%2)\n\t"
        :
        : "r"(matrix), "r"(other), "r"(destination)
        : "memory"
    );
}

void MATRIX_convert4MulMatrixRev(Matrix4 destination, Matrix4 matrix, Matrix4 other)
{
    __asm__ __volatile__(
        "lqc2 vf27, 0(%0)\n\t"
        "lqc2 vf28, 16(%0)\n\t"
        "lqc2 vf29, 32(%0)\n\t"
        "lqc2 vf30, 48(%0)\n\t"
        "vmulw.xyz vf27, vf27, vf27w\n\t"
        "vmulw.xyz vf28, vf28, vf28w\n\t"
        "vmulw.xyz vf29, vf29, vf29w\n\t"
        "vsub.w vf27, vf27, vf27\n\t"
        "vsub.w vf28, vf28, vf28\n\t"
        "vsub.w vf29, vf29, vf29\n\t"
        "lqc2 vf2, 0(%1)\n\t"
        "lqc2 vf3, 16(%1)\n\t"
        "lqc2 vf4, 32(%1)\n\t"
        "lqc2 vf5, 48(%1)\n\t"
        "vmulax.xyzw ACC, vf2, vf27x\n\t"
        "vmadday.xyzw ACC, vf3, vf27y\n\t"
        "vmaddaz.xyzw ACC, vf4, vf27z\n\t"
        "vmaddw.xyzw vf27, vf5, vf27w\n\t"
        "vmulax.xyzw ACC, vf2, vf28x\n\t"
        "vmadday.xyzw ACC, vf3, vf28y\n\t"
        "vmaddaz.xyzw ACC, vf4, vf28z\n\t"
        "vmaddw.xyzw vf28, vf5, vf28w\n\t"
        "vmulax.xyzw ACC, vf2, vf29x\n\t"
        "vmadday.xyzw ACC, vf3, vf29y\n\t"
        "vmaddaz.xyzw ACC, vf4, vf29z\n\t"
        "vmaddw.xyzw vf29, vf5, vf29w\n\t"
        "vmulax.xyzw ACC, vf2, vf30x\n\t"
        "vmadday.xyzw ACC, vf3, vf30y\n\t"
        "vmaddaz.xyzw ACC, vf4, vf30z\n\t"
        "vmaddw.xyzw vf30, vf5, vf30w\n\t"
        "sqc2 vf27, 0(%2)\n\t"
        "sqc2 vf28, 16(%2)\n\t"
        "sqc2 vf29, 32(%2)\n\t"
        "sqc2 vf30, 48(%2)\n\t"
        :
        : "r"(matrix), "r"(other), "r"(destination)
        : "memory"
    );
}

static void MATRIX_mul3x4(Matrix4 destination, Matrix4 source, Matrix4 rotation)
{
    int column;
    float zero;
    float one;

    for (column = 0; column < 3; column++) {
        float sourceValues[4];

        sourceValues[0] = source[0][column];
        sourceValues[1] = source[1][column];
        sourceValues[2] = source[2][column];
        sourceValues[3] = source[3][column];

        destination[0][column] = sourceValues[0] * rotation[0][0] +
                                sourceValues[1] * rotation[0][1] +
                                sourceValues[2] * rotation[0][2];
        destination[1][column] = sourceValues[0] * rotation[1][0] +
                                sourceValues[1] * rotation[1][1] +
                                sourceValues[2] * rotation[1][2];
        destination[2][column] = sourceValues[0] * rotation[2][0] +
                                sourceValues[1] * rotation[2][1] +
                                sourceValues[2] * rotation[2][2];
        destination[3][column] = sourceValues[0] * rotation[3][0] +
                                 sourceValues[1] * rotation[3][1] +
                                 sourceValues[2] * rotation[3][2] + sourceValues[3];
    }
    zero = 0.0f;
    one = 1.0f;
    destination[2][3] = zero;
    destination[3][3] = one;
    destination[1][3] = zero;
    destination[0][3] = zero;
}

void MATRIX_mul4sx3(Matrix4 destination, Matrix4 source, Matrix4 rotation)
{
    float (*scaleRows)[4] = source;
    float (*destinationRows)[4] = destination;
    float (*sourceRows)[4] = source;
    int column = 0;

    for (column = 0; column < 3; column++) {
        float sourceValues[4];
        float scale;

        sourceValues[0] = sourceRows[0][column];
        sourceValues[1] = sourceRows[1][column];
        sourceValues[2] = sourceRows[2][column];
        sourceValues[3] = sourceRows[3][column];
        destinationRows[0][column] = sourceValues[0] * rotation[0][0] +
                                sourceValues[1] * rotation[0][1] +
                                sourceValues[2] * rotation[0][2];
        destinationRows[1][column] = sourceValues[0] * rotation[1][0] +
                                sourceValues[1] * rotation[1][1] +
                                sourceValues[2] * rotation[1][2];
        destinationRows[2][column] = sourceValues[0] * rotation[2][0] +
                                sourceValues[1] * rotation[2][1] +
                                sourceValues[2] * rotation[2][2];
        scale = scaleRows[column][3];
        sourceValues[0] *= scale;
        sourceValues[1] *= scale;
        sourceValues[2] *= scale;
        destinationRows[3][column] = sourceValues[0] * rotation[3][0] +
                                 sourceValues[1] * rotation[3][1] +
                                 sourceValues[2] * rotation[3][2] + sourceValues[3];
    }
    destination[0][3] = source[0][3];
    destination[1][3] = source[1][3];
    destination[2][3] = source[2][3];
    destination[3][3] = 1.0f;
}

void MATRIX_transpose4(Matrix4 destination, const Matrix4 source)
{
    destination[0][0] = source[0][0];
    destination[0][1] = source[1][0];
    destination[0][2] = source[2][0];
    destination[0][3] = source[3][0];
    destination[1][0] = source[0][1];
    destination[1][1] = source[1][1];
    destination[1][2] = source[2][1];
    destination[1][3] = source[3][1];
    destination[2][0] = source[0][2];
    destination[2][1] = source[1][2];
    destination[2][2] = source[2][2];
    destination[2][3] = source[3][2];
    destination[3][0] = source[0][3];
    destination[3][1] = source[1][3];
    destination[3][2] = source[2][3];
    destination[3][3] = source[3][3];
}

void MATRIX_transpose3(Matrix4 matrix)
{
    float upper01 = matrix[1][0];
    float lower01 = matrix[0][1];
    float upper02 = matrix[2][0];
    float lower02 = matrix[0][2];
    float upper12 = matrix[2][1];
    float lower12 = matrix[1][2];

    matrix[1][0] = lower01;
    matrix[0][1] = upper01;
    matrix[2][0] = lower02;
    matrix[0][2] = upper02;
    matrix[2][1] = lower12;
    matrix[1][2] = upper12;
}

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotate4);

void MATRIX_rotXYZ(void)
{
}

void MATRIX_rotZYX(void)
{
}

void MATRIX_rotX4(Matrix4 matrix, float angle)
{
    Matrix4 rotation;

    MATRIX_rotate4(rotation, angle, 1.0f, 0.0f, 0.0f);
    MATRIX_mul3x4(matrix, matrix, rotation);
}

void MATRIX_rotY4(Matrix4 matrix, float angle)
{
    Matrix4 rotation;

    MATRIX_rotate4(rotation, angle, 0.0f, 1.0f, 0.0f);
    MATRIX_mul3x4(matrix, matrix, rotation);
}

void MATRIX_rotZ4(Matrix4 matrix, float angle)
{
    Matrix4 rotation;

    MATRIX_rotate4(rotation, angle, 0.0f, 0.0f, 1.0f);
    MATRIX_mul3x4(matrix, matrix, rotation);
}

void MATRIX_rotX4s(Matrix4 matrix, float angle)
{
    Matrix4 rotation;

    MATRIX_rotate4(rotation, angle, 1.0f, 0.0f, 0.0f);
    MATRIX_mul4sx3(matrix, matrix, rotation);
}

void MATRIX_rotY4s(Matrix4 matrix, float angle)
{
    Matrix4 rotation;

    MATRIX_rotate4(rotation, angle, 0.0f, 1.0f, 0.0f);
    MATRIX_mul4sx3(matrix, matrix, rotation);
}

void MATRIX_rotZ4s(Matrix4 matrix, float angle)
{
    Matrix4 rotation;

    MATRIX_rotate4(rotation, angle, 0.0f, 0.0f, 1.0f);
    MATRIX_mul4sx3(matrix, matrix, rotation);
}

void MATRIX_translate4(Matrix4 matrix, float x, float y, float z)
{
    matrix[3][0] = matrix[0][0] * x + matrix[1][0] * y + matrix[2][0] * z + matrix[3][0];
    matrix[3][1] = matrix[0][1] * x + matrix[1][1] * y + matrix[2][1] * z + matrix[3][1];
    matrix[3][2] = matrix[0][2] * x + matrix[1][2] * y + matrix[2][2] * z + matrix[3][2];
    matrix[3][3] = matrix[0][3] * x + matrix[1][3] * y + matrix[2][3] * z + matrix[3][3];
}

void MATRIX_scale4(Matrix4 matrix, float x, float y, float z)
{
    matrix[0][0] *= x;
    matrix[1][0] *= x;
    matrix[2][0] *= x;
    matrix[3][0] *= x;
    matrix[0][1] *= y;
    matrix[1][1] *= y;
    matrix[2][1] *= y;
    matrix[3][1] *= y;
    matrix[0][2] *= z;
    matrix[1][2] *= z;
    matrix[2][2] *= z;
    matrix[3][2] *= z;
}

void MATRIX_translate4s(Matrix4 matrix, float x, float y, float z)
{
    float rowScale0 = matrix[0][3];
    float rowScale1 = matrix[1][3];
    float rowScale2 = matrix[2][3];

    matrix[3][0] = matrix[0][0] * rowScale0 * x + matrix[1][0] * rowScale1 * y + matrix[2][0] * rowScale2 * z + matrix[3][0];
    matrix[3][1] = matrix[0][1] * rowScale0 * x + matrix[1][1] * rowScale1 * y + matrix[2][1] * rowScale2 * z + matrix[3][1];
    matrix[3][2] = matrix[0][2] * rowScale0 * x + matrix[1][2] * rowScale1 * y + matrix[2][2] * rowScale2 * z + matrix[3][2];
}

void MATRIX_scale4s(Matrix4 matrix, float x, float y, float z)
{
    matrix[0][3] *= x;
    matrix[1][3] *= y;
    matrix[2][3] *= z;
}

void MATRIX_toQuat4(const float matrix[16], float quaternion[4])
{
    float components[4];
    int D_004C23F8[3] = { 1, 2, 0 };
    float trace = matrix[0 * 5] + matrix[1 * 5] + matrix[2 * 5];
    float root;
    int dominantAxis;
    int nextAxis;
    int lastAxis;

    if (trace > 0.0f) {
        root = __builtin_sqrtf(trace + 1.0f);
        quaternion[3] = root * 0.5f;
        root = 0.5f / root;
        quaternion[0] = (matrix[1 * 4 + 2] - matrix[2 * 4 + 1]) * root;
        quaternion[1] = (matrix[2 * 4 + 0] - matrix[0 * 4 + 2]) * root;
        quaternion[2] = (matrix[0 * 4 + 1] - matrix[1 * 4 + 0]) * root;
        return;
    }
    dominantAxis = 0;
    if (matrix[0 * 5] < matrix[1 * 5]) {
        dominantAxis = 1;
    }
    if (matrix[dominantAxis * 5] < matrix[2 * 5]) {
        dominantAxis = 2;
    }
    nextAxis = D_004C23F8[dominantAxis];
    lastAxis = D_004C23F8[nextAxis];
    root = __builtin_sqrtf(matrix[dominantAxis * 5] -
                           (matrix[nextAxis * 5] + matrix[lastAxis * 5]) + 1.0f);
    components[dominantAxis] = root * 0.5f;
    if (root != 0.0f) {
        root = 0.5f / root;
    }
    components[3] = (matrix[nextAxis * 4 + lastAxis] - matrix[lastAxis * 4 + nextAxis]) * root;
    components[nextAxis] = (matrix[dominantAxis * 4 + nextAxis] + matrix[nextAxis * 4 + dominantAxis]) * root;
    components[lastAxis] = (matrix[dominantAxis * 4 + lastAxis] + matrix[lastAxis * 4 + dominantAxis]) * root;
    quaternion[0] = components[0];
    quaternion[1] = components[1];
    quaternion[2] = components[2];
    quaternion[3] = components[3];
}

void QUAT_toAxis(const float quaternion[4], float axisAngle[4])
{
    extern unsigned long long acos(double value);
    extern double fptodp(float value);
    extern float dptofp(unsigned long long value);
    const float D_004D7D28 = 0.000001f;
    double cosine = fptodp(quaternion[3]);
    unsigned long long angleDouble = acos(cosine);
    float angle = dptofp(angleDouble);
    float sine;

    axisAngle[3] = angle * -2.0f;
    sine = sinf(angle);
    if (D_004D7D28 < __builtin_fabsf(sine)) {
        axisAngle[0] = quaternion[0] / sine;
        axisAngle[1] = quaternion[1] / sine;
        axisAngle[2] = quaternion[2] / sine;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", QUAT_toMatrix4);

void QUAT_interpS(float firstWeight, float secondWeight,
                  Vector4 *destination, const Vector4 *first,
                  const Vector4 *second)
{
    extern unsigned long long acos(unsigned long long value);
    extern unsigned long long fptodp(float value);
    extern float dptofp(unsigned long long value);
    const float D_004D7D2C = 0.000001f;
    float firstX = first->x;
    float dot = firstX * second->x + first->y * second->y
              + first->z * second->z + first->w * second->w;
    Vector4 adjusted;
    float interpolationThreshold;

    if (dot < 0.0f) {
        adjusted.x = -second->x;
        adjusted.y = -second->y;
        adjusted.z = -second->z;
        adjusted.w = -second->w;
    } else {
        adjusted.x = second->x;
        adjusted.y = second->y;
        adjusted.z = second->z;
        adjusted.w = second->w;
    }
    interpolationThreshold = 1.0f - dot;
    if (D_004D7D2C < interpolationThreshold) {
        double promotedDot = dot;
        unsigned long long encodedDot;
        float angle;
        float sine;

        __builtin_memcpy(&encodedDot, &promotedDot, sizeof(encodedDot));
        angle = dptofp(acos(encodedDot));
        sine = sinf(angle);

        firstWeight = sinf(firstWeight * angle);
        firstWeight = firstWeight / sine;
        secondWeight = sinf(secondWeight * angle);
        firstX = first->x;
        secondWeight = secondWeight / sine;
    }
    destination->x = firstWeight * firstX + secondWeight * adjusted.x;
    destination->y = firstWeight * first->y + secondWeight * adjusted.y;
    destination->z = firstWeight * first->z + secondWeight * adjusted.z;
    destination->w = firstWeight * first->w + secondWeight * adjusted.w;
}

void QUAT_interpL(float destination[4], const float first[4], const float second[4],
                  float first_weight, float second_weight)
{
    float dot = first[0] * second[0];
    float adjusted[4];

    dot += first[1] * second[1];
    dot += first[2] * second[2];
    dot += first[3] * second[3];

    if (dot < 0.0f) {
        adjusted[0] = -second[0];
        adjusted[1] = -second[1];
        adjusted[2] = -second[2];
        adjusted[3] = -second[3];
    } else {
        adjusted[0] = second[0];
        adjusted[1] = second[1];
        adjusted[2] = second[2];
        adjusted[3] = second[3];
    }

    destination[0] = first_weight * first[0] + second_weight * adjusted[0];
    destination[1] = first_weight * first[1] + second_weight * adjusted[1];
    destination[2] = first_weight * first[2] + second_weight * adjusted[2];
    destination[3] = first_weight * first[3] + second_weight * adjusted[3];
}

void CUR_MATRIX_Unit(void)
{
    __asm__ __volatile__(
        "vmove.xyzw vf30, vf0\n\t"
        "vmr32.xyzw vf29, vf0\n\t"
        "vmr32.xyzw vf28, vf29\n\t"
        "vmr32.xyzw vf27, vf28\n\t"
        :
        :
        : "memory"
    );
}

void CUR_MATRIX_Unit4s(void)
{
    __asm__ __volatile__(
        "vmove.xyzw vf30, vf0\n\t"
        "vmr32.xyzw vf29, vf0\n\t"
        "vmr32.xyzw vf28, vf29\n\t"
        "vmr32.xyzw vf27, vf28\n\t"
        "vmove.w vf27, vf0\n\t"
        "vmove.w vf28, vf0\n\t"
        "vmove.w vf29, vf0\n\t"
        :
        :
        : "memory"
    );
}

void CUR_MATRIX_Set(Matrix4 matrix)
{
    __asm__ __volatile__(
        "lqc2 vf27, 0(%0)\n\t"
        "lqc2 vf28, 16(%0)\n\t"
        "lqc2 vf29, 32(%0)\n\t"
        "lqc2 vf30, 48(%0)\n\t"
        :
        : "r"(matrix)
        : "memory"
    );
}

void CUR_MATRIX_Get(Matrix4 matrix)
{
    __asm__ __volatile__(
        "sqc2 vf27, 0(%0)\n\t"
        "sqc2 vf28, 16(%0)\n\t"
        "sqc2 vf29, 32(%0)\n\t"
        "sqc2 vf30, 48(%0)\n\t"
        :
        : "r"(matrix)
        : "memory"
    );
}

void CUR_MATRIX_GetT(Vector4 *translation)
{
    __asm__ __volatile__(
        "vmove.xyz vf20, vf30\n\t"
        "vmove.w vf20, vf0\n\t"
        "sqc2 vf20, 0(%0)\n\t"
        :
        : "r"(translation)
        : "memory"
    );
}

void CUR_MATRIX_SetT(const Vector4 *translation)
{
    __asm__ __volatile__(
        "lqc2 vf1, 0(%0)\n\t"
        "vmove.xyz vf30, vf1\n\t"
        :
        : "r"(translation)
        : "memory"
    );
}

void CUR_MATRIX_SetR(Matrix4 rotation)
{
    __asm__ __volatile__(
        "lqc2 vf20, 0(%0)\n\t"
        "lqc2 vf21, 16(%0)\n\t"
        "lqc2 vf22, 32(%0)\n\t"
        "vmove.xyz vf27, vf20\n\t"
        "vmove.xyz vf28, vf21\n\t"
        "vmove.xyz vf29, vf22\n\t"
        :
        : "r"(rotation)
        : "memory"
    );
}

void CUR_MATRIX_4s3(Matrix4 local)
{
    __asm__ __volatile__(
        "lqc2 vf20, 0(%0)\n\t"
        "lqc2 vf21, 16(%0)\n\t"
        "lqc2 vf22, 32(%0)\n\t"
        "lqc2 vf23, 48(%0)\n\t"
        "vmulax.xyz ACC, vf27, vf20x\n\t"
        "vmadday.xyz ACC, vf28, vf20y\n\t"
        "vmaddz.xyz vf20, vf29, vf20z\n\t"
        "vmulax.xyz ACC, vf27, vf21x\n\t"
        "vmadday.xyz ACC, vf28, vf21y\n\t"
        "vmaddz.xyz vf21, vf29, vf21z\n\t"
        "vmulax.xyz ACC, vf27, vf22x\n\t"
        "vmadday.xyz ACC, vf28, vf22y\n\t"
        "vmaddz.xyz vf22, vf29, vf22z\n\t"
        "vmulw.xyz vf24, vf27, vf27w\n\t"
        "vmulw.xyz vf25, vf28, vf28w\n\t"
        "vmulw.xyz vf26, vf29, vf29w\n\t"
        "vmove.w vf30, vf0\n\t"
        "vmove.xyz vf27, vf20\n\t"
        "vmove.xyz vf28, vf21\n\t"
        "vmove.xyz vf29, vf22\n\t"
        "vmulax.xyz ACC, vf24, vf23x\n\t"
        "vmadday.xyz ACC, vf25, vf23y\n\t"
        "vmaddaz.xyz ACC, vf26, vf23z\n\t"
        "vmaddw.xyz vf30, vf30, vf0w\n\t"
        :
        : "r"(local)
        : "memory"
    );
}

void CUR_MATRIX_Txyz4s(const Vector4 *displacement)
{
    __asm__ __volatile__(
        "lqc2 vf4, 0(%0)\n\t"
        "vmulx.w vf20, vf27, vf4x\n\t"
        "vmuly.w vf21, vf28, vf4y\n\t"
        "vmulz.w vf22, vf29, vf4z\n\t"
        "vmulaw.xyz ACC, vf27, vf20w\n\t"
        "vmaddaw.xyz ACC, vf28, vf21w\n\t"
        "vmaddaw.xyz ACC, vf29, vf22w\n\t"
        "vmaddw.xyz vf30, vf30, vf0w\n\t"
        :
        : "r"(displacement)
        : "memory"
    );
}

void CUR_MATRIX_Rx4s(const int *angle)
{
    int bits = *angle;

    __asm__ __volatile__(
        "qmtc2.ni %0, vf4\n\t"
        "vcallms 0xE8\n\t"
        "vmove.x vf20, vf1\n\t"
        "qmtc2.ni %0, vf4\n\t"
        "vcallms 0x20\n\t"
        "vmulax.xyz ACC, vf28, vf20x\n\t"
        "vmaddx.xyz vf9, vf29, vf1x\n\t"
        "vmove.w vf30, vf0\n\t"
        "vmulax.xyz ACC, vf29, vf20x\n\t"
        "vmsubx.xyz vf29, vf28, vf1x\n\t"
        "vmove.xyz vf28, vf9\n\t"
        :
        : "r"(bits)
        : "memory"
    );
}

void CUR_MATRIX_Ry4s(const int *angle)
{
    int bits = *angle;

    __asm__ __volatile__(
        "qmtc2.ni %0, vf4\n\t"
        "vcallms 0xE8\n\t"
        "vmove.x vf20, vf1\n\t"
        "qmtc2.ni %0, vf4\n\t"
        "vcallms 0x20\n\t"
        "vmulax.xyz ACC, vf27, vf20x\n\t"
        "vmsubx.xyz vf9, vf29, vf1x\n\t"
        "vmove.w vf30, vf0\n\t"
        "vmulax.xyz ACC, vf29, vf20x\n\t"
        "vmaddx.xyz vf29, vf27, vf1x\n\t"
        "vmove.xyz vf27, vf9\n\t"
        :
        : "r"(bits)
        : "memory"
    );
}

void CUR_MATRIX_Rz4s(const int *angle)
{
    int bits = *angle;

    __asm__ __volatile__(
        "qmtc2.ni %0, vf4\n\t"
        "vcallms 0xE8\n\t"
        "vmove.x vf22, vf1\n\t"
        "qmtc2.ni %0, vf4\n\t"
        "vcallms 0x20\n\t"
        "vmulax.xyz ACC, vf27, vf22x\n\t"
        "vmaddx.xyz vf9, vf28, vf1x\n\t"
        "vmove.w vf30, vf0\n\t"
        "vmulax.xyz ACC, vf28, vf22x\n\t"
        "vmsubx.xyz vf28, vf27, vf1x\n\t"
        "vmove.xyz vf27, vf9\n\t"
        :
        : "r"(bits)
        : "memory"
    );
}

void CUR_MATRIX_Rzyx4s(const int *angles)
{
    int bits;

    bits = angles[2];
    __asm__ __volatile__(
        "qmtc2.ni %0, vf4\n\t"
        "vcallms 0xE8\n\t"
        "vmove.x vf20, vf1\n\t"
        "qmtc2.ni %0, vf4\n\t"
        "vcallms 0x20\n\t"
        "vaddx.y vf20, vf0, vf1x\n\t"
        :
        : "r"(bits)
        : "memory"
    );

    bits = angles[1];
    __asm__ __volatile__(
        "qmtc2.ni %0, vf4\n\t"
        "vcallms 0xE8\n\t"
        "vmove.x vf21, vf1\n\t"
        "qmtc2.ni %0, vf4\n\t"
        "vcallms 0x20\n\t"
        "vaddx.y vf21, vf0, vf1x\n\t"
        :
        : "r"(bits)
        : "memory"
    );

    bits = angles[0];
    __asm__ __volatile__(
        "qmtc2.ni %0, vf4\n\t"
        "vcallms 0xE8\n\t"
        "vmove.x vf22, vf1\n\t"
        "qmtc2.ni %0, vf4\n\t"
        "vcallms 0x20\n\t"
        "vmulax.xyz ACC, vf27, vf22x\n\t"
        "vmaddx.xyz vf9, vf28, vf1x\n\t"
        "vmulax.xyz ACC, vf28, vf22x\n\t"
        "vmsubx.xyz vf28, vf27, vf1x\n\t"
        "vmulax.xyz ACC, vf9, vf21x\n\t"
        "vmsuby.xyz vf8, vf29, vf21y\n\t"
        "vmulax.xyz ACC, vf29, vf21x\n\t"
        "vmaddy.xyz vf29, vf9, vf21y\n\t"
        "vmove.xyz vf27, vf8\n\t"
        "vmulax.xyz ACC, vf28, vf20x\n\t"
        "vmaddy.xyz vf9, vf29, vf20y\n\t"
        "vmove.w vf30, vf0\n\t"
        "vmulax.xyz ACC, vf29, vf20x\n\t"
        "vmsuby.xyz vf29, vf28, vf20y\n\t"
        "vmove.xyz vf28, vf9\n\t"
        :
        : "r"(bits)
        : "memory"
    );
}

void CUR_MATRIX_Sxyz4s(const Vector4 *scale)
{
    __asm__ __volatile__(
        "lqc2 vf19, 0(%0)\n\t"
        "vmulx.w vf27, vf27, vf19x\n\t"
        "vmuly.w vf28, vf28, vf19y\n\t"
        "vmulz.w vf29, vf29, vf19z\n\t"
        :
        : "r"(scale)
        : "memory"
    );
}
