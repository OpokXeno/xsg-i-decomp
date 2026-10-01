#include "common.h"

#include "main/xgl_2.h"
extern void xglMatrixStackLoad(float matrix[4][4]);

float *MotTransXYZ(float *xyz)
{
    Matrix4 matrix;
    float x;
    float y;
    float z;

    x = *xyz++;
    y = *xyz++;
    z = *xyz++;
    xglMatrixStackSave(matrix);
    matrix[3][0] += matrix[0][0] * x;
    matrix[3][1] += matrix[0][1] * x;
    matrix[3][2] += matrix[0][2] * x;
    matrix[3][0] += matrix[1][0] * y;
    matrix[3][1] += matrix[1][1] * y;
    matrix[3][2] += matrix[1][2] * y;
    matrix[3][0] += matrix[2][0] * z;
    matrix[3][1] += matrix[2][1] * z;
    matrix[3][2] += matrix[2][2] * z;
    xglMatrixStackLoad(matrix);
    return xyz;
}

void GetMdlFileName(void)
{
}

void MotInit(void)
{
}

void MotCalc(void)
{
}

void MotSetEx(void)
{
}
