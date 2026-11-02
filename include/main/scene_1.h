#ifndef INCLUDE_MAIN_SCENE_1_H
#define INCLUDE_MAIN_SCENE_1_H

#include "shared.h"
#include "main/find_native_method.h"
#include "main/jni.h"

/* Functions and data used by other translation units. */
void SCENE_instance(SceneVm *vm, SceneObject scene_object);
void SCENE_cleanup(SceneVm *vm, SceneObject scene_object);
void SCENE_start(SceneVm *vm, SceneObject scene_object, int mode,
                 void *method_name);

#endif /* INCLUDE_MAIN_SCENE_1_H */
