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

/*
 * MATRIX_convert4MulMatrix (ee-vu-cop2, docs/ps2-capabilities.md): rescales
 * `matrix`'s first three rows by their own W lane and zeroes that lane (the
 * same per-row transform MATRIX_convert4 performs in plain C, here run on the
 * VU0 macro pipeline and written back into `matrix` itself), then multiplies
 * `other` by the converted `matrix` into `destination`: each destination row
 * is one of `other`'s rows transformed by `matrix`'s (converted) basis, the
 * same row/lane pattern as xglMatrixMul (src/main/xgl_2.c).
 */
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

/*
 * MATRIX_convert4MulMatrixRev (ee-vu-cop2, docs/ps2-capabilities.md): the
 * mirror of MATRIX_convert4MulMatrix with `matrix` and `other` trading
 * places -- `matrix` gets the same convert4-style rescale-by-W/zero-W prep
 * as `matrix` does in MATRIX_convert4MulMatrix, in place, then each
 * destination row is one of the (converted) `matrix`'s rows transformed by
 * `other`'s basis (vf2-vf5, loaded from `other`, supply the ACC chain;
 * vf27-vf30, loaded from `matrix`, are the rows being transformed and are
 * overwritten in place with the result).
 */
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

/*
 * Swaps the three off-diagonal pairs of the 3x3 basis in place, transposing
 * it; row/column 3 (the translation/W terms) is left untouched.
 */
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

/*
 * MATRIX_rotate4, MATRIX_mul3x4 and MATRIX_mul4sx3 are original functions of
 * this same TU (INCLUDE_ASM below, not allocated to this attempt); these
 * declarations only give the compiler their signature for the calls made
 * here.
 */
void MATRIX_rotate4(Matrix4 rotation, float angle, float axisX, float axisY, float axisZ);
void MATRIX_mul3x4(Matrix4 destination, Matrix4 source, Matrix4 rotation);
void MATRIX_mul4sx3(Matrix4 destination, Matrix4 source, Matrix4 rotation);

/*
 * MATRIX_rotX4/Y4/Z4: build a rotation matrix about the named axis and
 * concatenate it onto `matrix` in place via MATRIX_mul3x4.
 */
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

/*
 * MATRIX_rotX4s/Y4s/Z4s: the "4s" (row-scale-carrying) counterpart of
 * MATRIX_rotX4/Y4/Z4, concatenating through MATRIX_mul4sx3 instead.
 */
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

/*
 * CUR_MATRIX_Unit (ee-vu-cop2, docs/ps2-capabilities.md): resets the
 * persistent VU0 "current matrix" basis (vf27-vf29) and translation (vf30)
 * to VF00's architectural identity (x=0, y=0, z=0, w=1): vf30 is set outright
 * from vf0, then vf29/vf28/vf27 are each rotated in from the previous
 * register by vmr32 (xyzw -> yzwx), leaving vf29=(0,0,1,0), vf28=(0,1,0,0)
 * and vf27=(1,0,0,0) -- the four rows of the identity matrix.
 */
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

/*
 * CUR_MATRIX_Unit4s (ee-vu-cop2, docs/ps2-capabilities.md): the "4s"
 * (row-scale-carrying) counterpart of CUR_MATRIX_Unit -- the same identity
 * basis/translation reset, with basis rows 0-2's own W lane (their per-row
 * scale) additionally set to VF00's 1.0, the identity scale.
 */
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

/*
 * CUR_MATRIX_Set (ee-vu-cop2, docs/ps2-capabilities.md): loads `matrix`'s
 * four rows into the persistent VU0 "current matrix" basis (vf27-vf29) and
 * translation (vf30), replacing whatever state was resident there.
 */
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

/*
 * CUR_MATRIX_Get (ee-vu-cop2, docs/ps2-capabilities.md): stores the
 * persistent VU0 "current matrix" basis (vf27-vf29) and translation (vf30)
 * into `matrix`'s four rows, the mirror of CUR_MATRIX_Set.
 */
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

/*
 * CUR_MATRIX_GetT (ee-vu-cop2, docs/ps2-capabilities.md): copies the
 * persistent VU0 "current matrix" translation row's XYZ lanes (vf30) into
 * scratch vf20, forces its own W lane to VF00's architectural 1.0
 * (discarding whatever per-row "4s" scale is currently in vf30's own W),
 * and stores the result into `translation`.
 */
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

/*
 * CUR_MATRIX_SetT (ee-vu-cop2, docs/ps2-capabilities.md): loads
 * `translation` into vf1 and copies its XYZ lanes into the persistent VU0
 * "current matrix" translation row (vf30), leaving vf30's own W lane (its
 * per-row "4s" scale) untouched.
 */
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

/*
 * CUR_MATRIX_SetR (ee-vu-cop2, docs/ps2-capabilities.md): loads
 * `rotation`'s three rows into vf20-vf22 and copies their XYZ lanes into
 * the persistent VU0 "current matrix" basis (vf27-vf29), leaving each
 * basis row's own W lane (its per-row "4s" scale) untouched; the
 * translation row (vf30) is not read or written.
 */
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

/*
 * CUR_MATRIX_4s3 (ee-vu-cop2, docs/ps2-capabilities.md): concatenates `local`
 * (a "4s"-format matrix: the W lane of rows 0-2 carries a per-row scale) onto
 * the persistent VU0 "current matrix" basis (vf27-vf29) and translation
 * (vf30), the resident state the CUR_MATRIX_* family reads and updates
 * across calls (CUR_MATRIX_Set/Get, above, transfer it to/from memory; this
 * routine only transforms it and leaves no store of its own -- the caller
 * reads it back with CUR_MATRIX_Get/GetT). `local`'s rows 0-2 are first
 * rotated/scaled by the current basis (vf20-vf22), row 3 (the translation)
 * is transformed by the basis scaled by its own W lane (vf24-vf26) and
 * accumulated onto vf30 (whose W lane is set to VF00's 1.0 first), and the
 * rotated rows then replace the basis.
 */
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

/*
 * CUR_MATRIX_Txyz4s (ee-vu-cop2, docs/ps2-capabilities.md): the persistent
 * VU0 "current matrix" analogue of MATRIX_translate4s (this TU, plain C) --
 * `displacement`'s XYZ lanes are each first multiplied by their matching
 * basis row's own W lane (vf27/vf28/vf29's per-row "4s" scale) into scratch
 * vf20-vf22, the three weighted rows are accumulated through one VU0 ACC
 * chain, and the total is added onto the translation row (vf30, whose own W
 * lane is left untouched by vmaddw.xyz's lane mask).
 */
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

/*
 * CUR_MATRIX_Rx4s/Ry4s/Rz4s (ee-vu-cop2, docs/ps2-capabilities.md): rotate
 * the persistent VU0 "current matrix" basis (vf27-vf29) about the named axis
 * by the angle (radians) at `*angle`, in place. The compiled access to
 * `*angle` is a plain `lw`, not a COP1 load, so `angle` is typed as a raw
 * bit-pattern word here rather than `float *`; the bit pattern is sent to
 * the resident VU0 trigonometric microprogram: qmtc2.ni carries it into vf4,
 * vcallms 0xE8 (the microprogram entry Vu0CallCos,
 * config/symbols/main.vu0-symbols.ld) launches it and leaves the cosine in
 * vf1.x, and a second launch at 0x20 (Vu0CallSin) leaves the sine in vf1.x;
 * the two axis rows are then combined cos +/- sin through one VU0 ACC chain
 * and written back into the basis.
 */
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

/*
 * CUR_MATRIX_Rzyx4s (ee-vu-cop2, docs/ps2-capabilities.md): rotates the
 * persistent VU0 "current matrix" basis by angles[2] (Z), then angles[1]
 * (Y), then angles[0] (X), in that order (the composite Z*Y*X rotation the
 * name records; angles[0..2] are a Vector4's x/y/z memory layout, its w not
 * read), combining all three into one VU0 ACC chain instead of three
 * separate calls to CUR_MATRIX_Rz4s/Ry4s/Rx4s. As in CUR_MATRIX_Rx4s, the
 * compiled access to each angle is a plain `lw`, so `angles` is typed as raw
 * bit-pattern words rather than `float *`/`Vector4 *`; each is sent to the
 * resident trigonometric microprogram exactly as in CUR_MATRIX_Rx4s
 * (qmtc2.ni/vcallms 0xE8 for cosine, vcallms 0x20 for sine).
 */
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

/*
 * CUR_MATRIX_Sxyz4s (ee-vu-cop2, docs/ps2-capabilities.md): the persistent
 * VU0 "current matrix" analogue of MATRIX_scale4s (this TU, plain C) --
 * scales each basis row's own W lane (vf27/vf28/vf29's per-row "4s" scale)
 * in place by the matching component of `scale`; the translation row
 * (vf30) is not read or written.
 */
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
