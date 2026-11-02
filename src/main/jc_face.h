#ifndef SRC_MAIN_JC_FACE_H
#define SRC_MAIN_JC_FACE_H

#include "main/xgl_2.h"
#include "main/jnt.h"
#include "main/jc_face.h"

/*
 * jnt.c's functions, declared here as defined there (hair_test.c includes
 * main/jnt.h with its own views of them, so they cannot move there yet).
 */
extern JntProductionConsumer JNT_getFilter(void *work, JntProducer *producer);
extern int JNT_setInterrupt(JntWork *work, JntProducer *producer);
extern void xglMatrixUnit4s(Matrix4 destination);
extern JntStaticChannel *JNT_getStaticVal(JntWork *work,
                                          JntStaticChannel *channel);
extern JntStaticChannel *JNT_getVal2(JntWork *work, JntStaticChannel *channel,
                                     float frame);

extern JntStaticChannel *JNT_getVal_HOGE2(JntWork *work,
                                          JntStaticChannel *channel,
                                          float frame);

extern JntStaticChannel *JNT_getVal(JntWork *work, JntStaticChannel *channel,
                                    float frame);

extern JntStaticChannel *JNT_getVal_staticChain(JntWork *work,
                                                JntStaticChannel *channel,
                                                float frame);

extern JntStaticChannel *JNT_constructMatrix(JntWork *work,
                                             JntStaticChannel *channel);
extern JntStaticChannel *JNT_interpolate(JntWork *work,
                                         JntStaticChannel *channel);
extern void *JNT_getStaticVal2(JntWork *work, JntStaticChannel *channel);

#endif
