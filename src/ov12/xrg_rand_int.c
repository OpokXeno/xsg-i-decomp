/*
 * OV12 original TU 88: 0x00a4caf8..0x00a4e1e8 (60 functions)
 */
#include "common.h"
#include "shared.h"
#include "xrg_rand_int.h"

/* newlib rand(), out of recovery scope: this TU only calls it. */
extern int rand(void);

int XrgRandInt(void)
{
    return rand();
}

float XrgRand(float lower, float upper)
{
    return (float) rand() * 4.656613e-10f * (upper - lower) + lower;
}

int XrgRandIntRange(int lower, int upper)
{
    return (rand() % (upper - lower)) + lower;
}

extern RgVector s_aRes_0;

float *XrgVectorZero(void)
{
    return s_aRes_0;
}

void XrgUnitVector(RgVector vector)
{
    XrgClearVector(vector);
    vector[3] = 1.0f;
}

extern RgVector s_aRes_1;

float *XrgVectorX(void)
{
    return s_aRes_1;
}

extern RgVector s_aRes_2;

float *XrgVectorY(void)
{
    return s_aRes_2;
}

extern RgVector s_aRes_3;

float *XrgVectorZ(void)
{
    return s_aRes_3;
}

void XrgSetVector(RgVector destination, float x, float y, float z, float w)
{
    destination[0] = x;
    destination[1] = y;
    destination[2] = z;
    destination[3] = w;
}

void XrgSetIVector(int destination[4], int x, int y, int z, int w)
{
    destination[0] = x;
    destination[1] = y;
    destination[2] = z;
    destination[3] = w;
}

void XrgSetIVectorXYZ(int destination[4], int x, int y, int z)
{
    destination[0] = x;
    destination[1] = y;
    destination[2] = z;
    destination[3] = 0;
}

void XrgSetVectorXYZ(RgVector destination, float x, float y, float z)
{
    destination[0] = x;
    destination[1] = y;
    destination[2] = z;
    destination[3] = 1.0f;
}

void XrgClearVector(RgVector destination)
{
    destination[0] = destination[1] = destination[2] = destination[3] = 0.0f;
}

void XrgAddVector(RgVector destination, RgVector first, RgVector second)
{
    destination[0] = first[0] + second[0];
    destination[1] = first[1] + second[1];
    destination[2] = first[2] + second[2];
    destination[3] = first[3] + second[3];
}

void XrgAddVectorXYZ(RgVector destination, RgVector first, RgVector second)
{
    destination[0] = first[0] + second[0];
    destination[1] = first[1] + second[1];
    destination[2] = first[2] + second[2];
}

void XrgScaleVector(RgVector destination, RgVector source, float scale)
{
    destination[0] = source[0] * scale;
    destination[1] = source[1] * scale;
    destination[2] = source[2] * scale;
    destination[3] = source[3] * scale;
}

void XrgScaleVectorXYZ(RgVector destination, RgVector source, float scale)
{
    destination[0] = source[0] * scale;
    destination[1] = source[1] * scale;
    destination[2] = source[2] * scale;
    destination[3] = source[3];
}

void XrgCopyVectorXYZ(RgVector destination, RgVector source)
{
    destination[0] = source[0];
    destination[1] = source[1];
    destination[2] = source[2];
}

void XrgLinearIntpVector(RgVector destination, RgVector first,
                         RgVector second, float weight)
{
    float complement = 1.0f - weight;

    destination[0] = first[0] * weight + second[0] * complement;
    destination[1] = first[1] * weight + second[1] * complement;
    destination[2] = first[2] * weight + second[2] * complement;
    destination[3] = 1.0f;
}

void XrgCopyVector(RgVector destination, RgVector source)
{
    destination[0] = source[0];
    destination[1] = source[1];
    destination[2] = source[2];
    destination[3] = source[3];
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgLengthVector);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgNormalizeVector);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgOuterVector);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgInnerVector);

float XrgInnerVectorXYZW(RgVector first, RgVector second)
{
    return first[0] * second[0] + first[1] * second[1] +
           first[2] * second[2] + first[3] * second[3];
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", clearRgMatrix);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgCopyMatrix);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgUnitMatrix);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgMulMatrix);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgTransMatrix);

void XrgSubVector(RgVector destination, RgVector first, RgVector second)
{
    destination[0] = first[0] - second[0];
    destination[1] = first[1] - second[1];
    destination[2] = first[2] - second[2];
}

void XrgSubVectorXYZ(RgVector destination, RgVector first, RgVector second)
{
    destination[0] = first[0] - second[0];
    destination[1] = first[1] - second[1];
    destination[2] = first[2] - second[2];
}

void XrgNegateVector(RgVector destination, RgVector source)
{
    destination[0] = -source[0];
    destination[1] = -source[1];
    destination[2] = -source[2];
    destination[3] = source[3];
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgRandVector);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgRandVectorRange);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgApplyVector);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgRotVector);

extern void XrgNormalizeVector(RgVector destination, RgVector source);
extern void XrgOuterVector(RgVector destination, RgVector first, RgVector second);
extern void XrgUnitMatrix(RgMatrix destination);

/*
 * Each XrgCalcMatrix<From>to<To> builds an orthonormal frame (matrix rows 0/1/2
 * as its X/Y/Z axes, ov12:0x00a4d498..0x00a4d698): one axis is normalized
 * directly from an input vector, the second is its cross product with the
 * other input, and the third closes the frame as their cross product. The
 * two input vectors are named for the axis each represents, in the order the
 * original passes them (annotations/overlays/ov12_annotations.csv gives the
 * desired/reference role per function; which role lands in which parameter
 * position is this allocation's own evidence, from XrgNormalizeVector's and
 * XrgOuterVector's argument order at each call site).
 */
void XrgCalcMatrixYtoZ(RgMatrix matrix, RgVector zAxis, RgVector yAxis)
{
    float *rowZ;

    rowZ = matrix + 8;
    XrgUnitMatrix(matrix);
    XrgNormalizeVector(rowZ, zAxis);
    XrgOuterVector(matrix, yAxis, rowZ);
    XrgNormalizeVector(matrix, matrix);
    XrgOuterVector(matrix + 4, rowZ, matrix);
}

void XrgCalcMatrixZtoY(RgMatrix matrix, RgVector zAxis, RgVector yAxis)
{
    float *rowY;

    rowY = matrix + 4;
    XrgUnitMatrix(matrix);
    XrgNormalizeVector(rowY, yAxis);
    XrgOuterVector(matrix, rowY, zAxis);
    XrgNormalizeVector(matrix, matrix);
    XrgOuterVector(matrix + 8, matrix, rowY);
}

void XrgCalcMatrixXtoY(RgMatrix matrix, RgVector xAxis, RgVector yAxis)
{
    float *rowZ;

    XrgUnitMatrix(matrix);
    XrgNormalizeVector(matrix, xAxis);
    rowZ = matrix + 8;
    XrgOuterVector(rowZ, xAxis, yAxis);
    XrgNormalizeVector(rowZ, rowZ);
    XrgOuterVector(matrix + 4, rowZ, matrix);
}

void XrgCalcMatrixXtoZ(RgMatrix matrix, RgVector xAxis, RgVector zAxis)
{
    float *rowY;

    XrgUnitMatrix(matrix);
    XrgNormalizeVector(matrix, xAxis);
    rowY = matrix + 4;
    XrgOuterVector(rowY, zAxis, xAxis);
    XrgNormalizeVector(rowY, rowY);
    XrgOuterVector(matrix + 8, matrix, rowY);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgRotMatrixX);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgRotMatrixY);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgRotMatrixZ);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgInvMatrix);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgCalcRotY);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgClearMatStack);

/* The current matrix-stack top. The data is still scaffold-owned
 * (docs/naming.md); the scaffold defines it under the original ELF symbol
 * name s_paTop, imported through config/symbols/ov12.txt. */
extern float *s_paTop;

void XrgPushMatStack(void)
{
    float *previous_top = s_paTop;

    s_paTop = previous_top + 16;
    XrgCopyMatrix(s_paTop, previous_top);
}

void XrgSetTopMatStack(const RgMatrix source)
{
    XrgCopyMatrix(s_paTop, source);
}

void XrgPopMatStack(void)
{
    s_paTop -= 16;
}

void XrgMulMatrix(RgMatrix destination, RgMatrix left, RgMatrix right);

void XrgMulMatStack(RgMatrix matrix)
{
    XrgMulMatrix(s_paTop, s_paTop, matrix);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgTransMatStack);

void XrgRotMatrixX(RgMatrix destination, RgMatrix source, float angle);
void XrgRotMatrixY(RgMatrix destination, RgMatrix source, float angle);
void XrgRotMatrixZ(RgMatrix destination, RgMatrix source, float angle);

void XrgRotMatStack(float angle_x, float angle_y, float angle_z)
{
    XrgRotMatrixY(s_paTop, s_paTop, angle_y);
    XrgRotMatrixX(s_paTop, s_paTop, angle_x);
    XrgRotMatrixZ(s_paTop, s_paTop, angle_z);
}

float *XrgTopMatStack(void)
{
    return s_paTop;
}

void XrgCopyMatStack(RgMatrix destination)
{
    XrgCopyMatrix(destination, s_paTop);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgCalcPlaneNorm);

void XrgCalcPlaneNorm(RgVector plane);
float XrgInnerVector(RgVector first, RgVector second);

/* The plane is stored as a 4-component vector: XrgCalcPlaneNorm fills the
 * first three floats with the plane normal, and this function stores the
 * negated dot product of that normal with the given point in the fourth
 * float (offset 0xC), the plane distance. */
void XrgCalcPlane(RgVector plane, RgVector point)
{
    XrgCalcPlaneNorm(plane);
    plane[3] = -XrgInnerVector(plane, point);
}

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgIsTriIntersectedByRay);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgQuantAngle);

INCLUDE_ASM("asm/nonmatchings/ov12/xrg_rand_int", XrgQuantAngle4);
