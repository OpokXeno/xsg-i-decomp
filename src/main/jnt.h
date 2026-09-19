/*
 * TU-local declarations of main/tu261 (src/main/jnt.c).
 */

#ifndef SRC_MAIN_JNT_H
#define SRC_MAIN_JNT_H

typedef struct JntMatrix JntMatrix;

typedef JntMatrix *JntMatrixBuffer;

typedef unsigned int JntMatrixSelect;

typedef struct JntProducer JntProducer;

void JNT_setFlags(int flags);

void JNT_setClipR(float clip_radius);

void JNT_animSetFlags(int anim_flags);

int JNT_getFlags(void);

void JNT_setMatrix(JntMatrixBuffer matrix_buffer);

void JNT_setMatrix2(JntMatrixBuffer matrix_buffer, JntMatrixSelect matrix_select);

void JNT_setInterpMatrix(JntMatrixBuffer matrix_buffer, float interpolation);

void JNT_setCurve(void *static_value_records, void *value_records);

void *JNT_getRootElement(void *joint);

void JNT_initProducer(JntProducer *producer);

void *JNT_getElement(void *elements, int index);

void *JNT_nextElement(void *element);

void JNT_onSmoothHair(void);

void JNT_offSmoothHair(void);

#endif /* SRC_MAIN_JNT_H */
