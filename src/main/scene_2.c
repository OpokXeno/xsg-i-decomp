#include "common.h"
#include "shared.h"

typedef struct {
    SceneObject scene_object;
    int mode;
    void *method_name;
} SceneStartArguments;

extern void SCENE_start(SceneVm *vm, SceneObject scene_object, int mode,
                        void *method_name);

void Java_xeno_Scene_start__ILjava_lang_Object_(SceneVm *vm,
                                                 SceneStartArguments *arguments)
{
    SCENE_start(vm, arguments->scene_object, arguments->mode,
                arguments->method_name);
}

void Java_xeno_Scene_stop__(SceneVm *vm, SceneStartArguments *arguments)
{
    (void)vm;
    (void)arguments;
}
