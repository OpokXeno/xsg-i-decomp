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

#endif /* SRC_MAIN_JNT_H */
