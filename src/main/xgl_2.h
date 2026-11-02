/*
 * TU-local declarations of main/tu100 (src/main/xgl_2.c).
 */

#ifndef SRC_MAIN_XGL_2_H
#define SRC_MAIN_XGL_2_H

#include "shared.h"
#include "main/xgl_2.h"

typedef unsigned int XglQuadword __attribute__((mode(TI)));

extern unsigned long long iRandSeed;

void xglMatrix2Quaternion(float quaternion[4], const Matrix *matrix);

void xglVectorClamp(Vector4 *destination, const Vector4 *source,
                    float minimum, float maximum);

void xglVectorClampXYZ(Vector4 *destination, const Vector4 *source,
                       float minimum, float maximum);

void xglMatrixTrans(Matrix4 destination, const Matrix4 source,
                    const float translation[4]);

void xglRandSeedInit(void);
void xglVectorDiv(Vector4 *destination, const Vector4 *numerator,
                  const Vector4 *divisor);

void xglVectorDivXYZ(Vector4 *destination, const Vector4 *numerator,
                     const Vector4 *divisor);

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
void xglVectorScaleAdd(float scale, Vector4 *destination,
                       const Vector4 *source, const Vector4 *other);

void xglVectorScaleSub(float scale, Vector4 *destination,
                       const Vector4 *source, const Vector4 *other);

void xglVectorScaleSubXYZ(float scale, Vector4 *destination,
                          const Vector4 *source, const Vector4 *other);

void xglVectorInterXYZ(float fraction, Vector4 *destination,
                       const Vector4 *start, const Vector4 *end);

float xglVectorInner4(const Vector4 *left, const Vector4 *right);

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
void xglMatrixStackRotV(const float axis[4], float angle);
void xglMatrixStackPushUnit(void);
void xglMatrixStackRTPS(Vector4 *out, const Vector4 *point,
                        const Vector4 *scale, const Vector4 *offset);

#endif /* SRC_MAIN_XGL_2_H */
