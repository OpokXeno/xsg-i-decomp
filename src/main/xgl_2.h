/*
 * TU-local declarations of main/tu100 (src/main/xgl_2.c).
 */

#ifndef SRC_MAIN_XGL_2_H
#define SRC_MAIN_XGL_2_H

#include "shared.h"

typedef struct Matrix {
    float elements[16];
} Matrix;

extern float xglAtan2(float x, float y);

void xglVectorClamp(Vector4 *destination, const Vector4 *source,
                    float minimum, float maximum);

void xglVectorClampXYZ(Vector4 *destination, const Vector4 *source,
                       float minimum, float maximum);

void xglMatrixScale(Matrix4 destination, const Matrix4 source,
                    const float scale[4]);

void xglMatrixTrans(Matrix4 destination, const Matrix4 source,
                    const float translation[4]);

void xglRandSeedInit(void);

extern unsigned long long iRandSeed;

unsigned int xglLRand(void);

void xglVectorDiv(Vector4 *destination, const Vector4 *numerator,
                  const Vector4 *divisor);

void xglVectorDivXYZ(Vector4 *destination, const Vector4 *numerator,
                     const Vector4 *divisor);

void xglVectorMulAdd(Vector4 *destination, const Vector4 *left,
                     const Vector4 *right, const Vector4 *addend);

void xglVectorMulAddXYZ(Vector4 *destination, const Vector4 *left,
                        const Vector4 *right, const Vector4 *addend);

void xglVectorMulSub(Vector4 *destination, const Vector4 *left,
                     const Vector4 *right, const Vector4 *subtrahend);

void xglVectorMulSubXYZ(Vector4 *destination, const Vector4 *left,
                        const Vector4 *right, const Vector4 *subtrahend);

void xglVectorDivAdd(Vector4 *destination, const Vector4 *numerator,
                     const Vector4 *divisor, const Vector4 *addend);

void xglVectorDivAddXYZ(Vector4 *destination, const Vector4 *numerator,
                        const Vector4 *divisor, const Vector4 *addend);

void xglVectorDivSub(Vector4 *destination, const Vector4 *numerator,
                     const Vector4 *divisor, const Vector4 *subtrahend);

void xglVectorDivSubXYZ(Vector4 *destination, const Vector4 *numerator,
                        const Vector4 *divisor, const Vector4 *subtrahend);

extern void xglMatrixMul(Matrix *destination, Matrix *left, Matrix *right);

void xglVectorScale(float scale, Vector4 *destination, const Vector4 *source);

void xglVectorScaleXYZ(float scale, Vector4 *destination, const Vector4 *source);

void xglVectorScaleAdd(float scale, Vector4 *destination,
                       const Vector4 *source, const Vector4 *other);

void xglVectorScaleAddXYZ(float scale, Vector4 *destination,
                          const Vector4 *source, const Vector4 *other);

void xglVectorScaleSub(float scale, Vector4 *destination,
                       const Vector4 *source, const Vector4 *other);

void xglVectorScaleSubXYZ(float scale, Vector4 *destination,
                          const Vector4 *source, const Vector4 *other);

void xglVectorInter(float fraction, Vector4 *destination,
                    const Vector4 *start, const Vector4 *end);

void xglVectorInterXYZ(float fraction, Vector4 *destination,
                       const Vector4 *start, const Vector4 *end);

void xglVectorInner(float *result, const Vector4 *left, const Vector4 *right);

float xglVectorInner4(const Vector4 *left, const Vector4 *right);

void xglVectorOuter(Vector4 *destination, const Vector4 *left,
                    const Vector4 *right);

/*
 * xglMatrixStack* (main/tu100, from the accepted src/main/xgl_2/stack.s):
 * the eighteen thin VU0 matrix-stack wrappers. Each Vu0Call* symbol below is
 * a VU0 instruction-memory byte address exported by src/main/vu0/
 * Vu0MicroCode.dvp through config/symbols/main.vu0-symbols.ld
 * (exact_vu_microcode, verified on the shared tree): the original
 * `lui %hi(SYM)/addiu %lo(SYM)` pair is the EE address-of-symbol idiom
 * against these absolute linker symbols, not an embedded literal, which is
 * why the wrappers only reproduce once the microprogram (and its symbol
 * table) exist. Only the address is ever taken; none of these is called as a
 * function.
 */
extern void Vu0CallMatrixStackUnit(void);
extern void Vu0CallMatrixStackMul(void);
extern void Vu0CallMatrixStackReverse(void);
extern void Vu0CallMatrixStackInverse(void);
extern void Vu0CallMatrixStackScale(void);
extern void Vu0CallMatrixStackTrans(void);
extern void Vu0CallMatrixStackRotV(void);
extern void Vu0CallMatrixStackRotX(void);
extern void Vu0CallMatrixStackRotY(void);
extern void Vu0CallMatrixStackRotZ(void);
extern void Vu0CallMatrixStackFrustum(void);
extern void Vu0CallMatrixStackPushUnit(void);
extern void Vu0CallMatrixStackPush(void);
extern void Vu0CallMatrixStackPop(void);
extern void Vu0CallMatrixStackMulVector(void);
extern void Vu0CallMatrixStackRTPS(void);

void xglMatrixStackMul(const Matrix4 matrix);
void xglMatrixStackReverse(void);
void xglMatrixStackInverse(void);
void xglMatrixStackScale(const float scale[4]);
void xglMatrixStackTrans(const float translation[4]);
void xglMatrixStackRotV(const float axis[4], float angle);
void xglMatrixStackRotZ(float angle);
void xglMatrixStackFrustum(float left, float right, float bottom, float top,
                           float nearPlane, float farPlane);
void xglMatrixStackLoad(float matrix[4][4]);
void xglMatrixStackPushUnit(void);
void xglMatrixStackPush(void);
void xglMatrixStackPop(int count);
void xglMatrixStackMulVector(Vector4 *out, const Vector4 *vector);
void xglMatrixStackRTPS(Vector4 *out, const Vector4 *point,
                        const Vector4 *scale, const Vector4 *offset);

#endif /* SRC_MAIN_XGL_2_H */
