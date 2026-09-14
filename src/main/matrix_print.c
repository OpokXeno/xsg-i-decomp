#include "common.h"
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

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_transpose4);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_transpose3);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotate4);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotXYZ);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotZYX);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotX4);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotY4);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotZ4);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotX4s);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotY4s);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_rotZ4s);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_translate4);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_scale4);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_translate4s);

INCLUDE_ASM("asm/main/nonmatchings/matrix_print", MATRIX_scale4s);

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
