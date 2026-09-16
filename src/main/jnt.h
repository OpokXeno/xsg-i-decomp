/*
 * TU-local declarations of main/tu261 (src/main/jnt.c).
 */

#ifndef SRC_MAIN_JNT_H
#define SRC_MAIN_JNT_H

typedef struct JntMatrix JntMatrix;

typedef JntMatrix *JntMatrixBuffer;

typedef unsigned int JntMatrixSelect;

void JNT_setMatrix(JntMatrixBuffer matrix_buffer);

void JNT_setMatrix2(JntMatrixBuffer matrix_buffer, JntMatrixSelect matrix_select);

void JNT_setInterpMatrix(JntMatrixBuffer matrix_buffer, float interpolation);

void JNT_setCurve(void *static_value_records, void *value_records);

void *JNT_getRootElement(void *joint);

void *JNT_getElement(void *elements, int index);

void *JNT_nextElement(void *element);

#endif /* SRC_MAIN_JNT_H */
