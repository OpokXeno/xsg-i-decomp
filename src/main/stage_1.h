#ifndef SRC_MAIN_STAGE_1_H
#define SRC_MAIN_STAGE_1_H

#include "shared.h"
#include "main/jni.h"
#include "main/find_native_method.h"

extern u16 defaultStage[];

void *STAGE_create(u16 stage_id);

extern SceneString *NAME_Constructor;
extern SceneType *TYPE_Void;
extern unsigned char tcamera[];

extern const char D_004C2420[];
extern const char D_004DA628[];
extern const char D_004DA630[];
extern const char D_004DA638[];

extern SceneMethod *findMethod(SceneClass *scene_class, SceneString *name,
                               void *signature_or_type);
extern void JNI_initThread(SceneVm *vm);
extern void JNI_callMethod(SceneVm *vm, SceneMethod *method,
                           SceneObject *arguments, int *output);
extern SceneString *loadConstString(const char *bytes, int length);
extern JavaField *lookupClassField(void *class_object, void *name, int flags);
extern SceneClass *loadClass(SceneString *name, int initialize);
extern SceneClass *getClassFromSignature(const char *signature,
                                         void *class_loader);
extern int instanceOf(SceneClass *scene_class, SceneClass *parent_class);

#endif
