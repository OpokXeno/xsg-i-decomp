#ifndef INCLUDE_MAIN_XGL_2_H
#define INCLUDE_MAIN_XGL_2_H

#include "shared.h"

typedef unsigned int XglQuadword __attribute__((mode(TI)));

typedef union Matrix {
    float elements[16];
    XglQuadword quadwords[4];
} Matrix;

extern void xglVectorLength(float *destination, const Vector4 *vector);
extern void xglMatrixUnit(float matrix[4][4]);
void xglMatrixInverse(Matrix4 destination, const Matrix4 source);

void xglMatrixScale(Matrix4 destination, const Matrix4 source,
                    const float scale[4]);

extern void xglMatrixStackUnit(void);
void xglMatrixStackMul(const Matrix4 matrix);
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

#endif /* INCLUDE_MAIN_XGL_2_H */
