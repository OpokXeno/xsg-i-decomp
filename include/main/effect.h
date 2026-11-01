#ifndef INCLUDE_MAIN_EFFECT_H
#define INCLUDE_MAIN_EFFECT_H

#include "shared.h"
#include "main/jni.h"

struct UnitScaleVector {
    SceneObjectClassRef *class_ref;
    float z;
    float y;
    float x;
    float w;
    unsigned char unmodeled_14[4];
};

#endif /* INCLUDE_MAIN_EFFECT_H */
