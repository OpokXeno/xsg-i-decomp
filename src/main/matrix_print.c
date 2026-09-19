#include "common.h"
#include "shared.h"
#include "matrix_print.h"

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_print);

/*
 * The MATRIX initializer pair writes a four-by-four float storage block in
 * row-major order.  MATRIX_identity4s uses the fourth value of each of the
 * first three rows as a basis scale; its name's historical suffix is not
 * established by the available ELF evidence.
 */

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

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_convert4s);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_convert4MulMatrix);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_convert4MulMatrixRev);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_mul3x4);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_mul4sx3);

/*
 * Exchanges rows and columns: destination[row][col] = source[col][row] for
 * every element, so the fourth row/column (the translation/W terms) is
 * transposed the same as the 3x3 basis.
 */
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

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_transpose3);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotate4);

void MATRIX_rotXYZ(void)
{
}

void MATRIX_rotZYX(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotX4);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotY4);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotZ4);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotX4s);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotY4s);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotZ4s);

/*
 * Transforms the XYZ displacement by the matrix's current basis rows and
 * accumulates it into every column of the translation row (row 3).
 */
void MATRIX_translate4(Matrix4 matrix, float x, float y, float z)
{
    matrix[3][0] = matrix[0][0] * x + matrix[1][0] * y + matrix[2][0] * z + matrix[3][0];
    matrix[3][1] = matrix[0][1] * x + matrix[1][1] * y + matrix[2][1] * z + matrix[3][1];
    matrix[3][2] = matrix[0][2] * x + matrix[1][2] * y + matrix[2][2] * z + matrix[3][2];
    matrix[3][3] = matrix[0][3] * x + matrix[1][3] * y + matrix[2][3] * z + matrix[3][3];
}

/*
 * Scales the basis columns 0-2 in place by x, y and z; column 3
 * (the translation/W column) is left untouched.
 */
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

/*
 * Like MATRIX_translate4, but each basis row is first scaled by the value
 * MATRIX_scale4s stashed in that row's own W column (rowScale0..2) before
 * being weighted by the XYZ displacement and accumulated into the
 * translation row's first three columns; the translation row's own W
 * (column 3) is left untouched.
 */
void MATRIX_translate4s(Matrix4 matrix, float x, float y, float z)
{
    float rowScale0 = matrix[0][3];
    float rowScale1 = matrix[1][3];
    float rowScale2 = matrix[2][3];

    matrix[3][0] = matrix[0][0] * rowScale0 * x + matrix[1][0] * rowScale1 * y + matrix[2][0] * rowScale2 * z + matrix[3][0];
    matrix[3][1] = matrix[0][1] * rowScale0 * x + matrix[1][1] * rowScale1 * y + matrix[2][1] * rowScale2 * z + matrix[3][1];
    matrix[3][2] = matrix[0][2] * rowScale0 * x + matrix[1][2] * rowScale1 * y + matrix[2][2] * rowScale2 * z + matrix[3][2];
}

/*
 * Scales the W column of basis rows 0-2 in place by x, y and z; this is the
 * per-row scale MATRIX_translate4s later reads back as rowScale0..2.
 */
void MATRIX_scale4s(Matrix4 matrix, float x, float y, float z)
{
    matrix[0][3] *= x;
    matrix[1][3] *= y;
    matrix[2][3] *= z;
}

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_toQuat4);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", QUAT_toAxis);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", QUAT_toMatrix4);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", QUAT_interpS);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", QUAT_interpL);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_Unit);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_Unit4s);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_Set);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_Get);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_GetT);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_SetT);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_SetR);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_4s3);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_Txyz4s);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_Rx4s);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_Ry4s);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_Rz4s);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_Rzyx4s);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", CUR_MATRIX_Sxyz4s);
