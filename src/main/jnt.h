/*
 * TU-local declarations of main/tu261 (src/main/jnt.c).
 */

#ifndef SRC_MAIN_JNT_H
#define SRC_MAIN_JNT_H

#include "main/jnt.h"

typedef struct JntProducer JntProducer;

typedef struct JntWork JntWork;

typedef struct JntStaticChannel JntStaticChannel;

typedef void *(*JntProductionConsumer)(JntWork *work,
                                      JntStaticChannel *channels,
                                      int argument);

int JNT_getFlags(void);
void JNT_setMatrix(JntMatrixBuffer matrix_buffer);
void JNT_setMatrix2(JntMatrixBuffer matrix_buffer, JntMatrixSelect matrix_select);
void JNT_setInterpMatrix(JntMatrixBuffer matrix_buffer, float interpolation);
void JNT_onSmoothHair(void);
void JNT_offSmoothHair(void);

#endif /* SRC_MAIN_JNT_H */
