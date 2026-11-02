#ifndef INCLUDE_MAIN_XGL_2_H
#define INCLUDE_MAIN_XGL_2_H

#include "shared.h"

typedef unsigned int XglQuadword __attribute__((mode(TI)));

typedef union Matrix {
    float elements[16];
    Matrix4 rows;
    XglQuadword quadwords[4];
} Matrix;

extern void xglVectorLength(float *destination, const Vector4 *vector);
extern void xglMatrixUnit(float matrix[4][4]);
void xglMatrixInverse(Matrix4 destination, Matrix4 source);

void xglMatrixReverse(Matrix4 destination, Matrix4 source);
void xglVectorMulMat(Vector4 *destination, Matrix4 matrix,
                     const Vector4 *vector);

void xglMatrixScale(Matrix4 destination, Matrix4 source,
                    const float scale[4]);

extern void xglMatrixStackUnit(void);
void xglMatrixStackMul(Matrix4 matrix);
void xglMatrixStackReverse(void);
void xglMatrixStackInverse(void);
void xglMatrixStackScale(const float scale[4]);
void xglMatrixStackTrans(const float translation[4]);
extern void xglMatrixStackRotX(float angle);
extern void xglMatrixStackRotY(float angle);
extern void xglMatrixStackRotZ(float angle);
extern void xglMatrixStackSave(float matrix[4][4]);
unsigned short xglSRand(void);
float xglSin(float angle);
float xglCos(float angle);
void xglVectorScale(Vector4 *destination, const Vector4 *source, float scale);

/* Functions and data used by other translation units. */
float I2F(int value);
void xglMatrixStackMulVector(Vector4 *out, const Vector4 *vector);
int F2I(float value);
void xglVectorOuter(Vector4 *destination, const Vector4 *left,
                    const Vector4 *right);
void xglVectorScaleXYZ(Vector4 *destination, const Vector4 *source, float scale);
extern float xglAtan2(float x, float y);
void xglVectorNormal(Vector4 *destination, const Vector4 *source);
void xglMatrixStackLoad(float matrix[4][4]);
float xglFRand(void);
void xglVectorInner(float *result, const Vector4 *left, const Vector4 *right);
unsigned int xglLRand(void);
void xglVectorScaleAddXYZ(float scale, Vector4 *destination,
                          const Vector4 *source, const Vector4 *other);
void xglVectorInter(float fraction, Vector4 *destination,
                    const Vector4 *start, const Vector4 *end);
void xglMatrixStackPush(void);
void xglMatrixStackPop(int count);
void xglVectorMulAdd(Vector4 *destination, const Vector4 *left,
                     const Vector4 *right, const Vector4 *addend);
void xglMatrixStackFrustum(float left, float right, float bottom, float top,
                           float nearPlane, float farPlane);

#endif /* INCLUDE_MAIN_XGL_2_H */
