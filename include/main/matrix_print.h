#ifndef INCLUDE_MAIN_MATRIX_PRINT_H
#define INCLUDE_MAIN_MATRIX_PRINT_H

#include "shared.h"

void CUR_MATRIX_Unit4s(void);
void CUR_MATRIX_Get(Matrix4 matrix);
void CUR_MATRIX_SetT(const Vector4 *translation);
void CUR_MATRIX_Sxyz4s(const Vector4 *scale);

/* Functions and data used by other translation units. */
void CUR_MATRIX_Rx4s(const int *angle);
void CUR_MATRIX_Ry4s(const int *angle);
void CUR_MATRIX_Rz4s(const int *angle);
void MATRIX_convert4(f32 (*dst)[4], f32 (*src)[4]);
void MATRIX_transpose3(Matrix4 matrix);
void MATRIX_translate4s(Matrix4 matrix, float x, float y, float z);
void MATRIX_rotX4s(Matrix4 matrix, float angle);
void MATRIX_rotY4s(Matrix4 matrix, float angle);
void MATRIX_rotZ4s(Matrix4 matrix, float angle);
/*
 * The matrix_print current-matrix API transfers native 64-byte matrices,
 * loads a 16-byte translation payload, and transports rotation words into
 * VU0 without a numerical integer conversion.
 */
void CUR_MATRIX_Set(Matrix4 matrix);
void CUR_MATRIX_Txyz4s(const Vector4 *displacement);
void CUR_MATRIX_Rzyx4s(const int *angles);

#endif /* INCLUDE_MAIN_MATRIX_PRINT_H */
