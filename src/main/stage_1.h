#ifndef SRC_MAIN_STAGE_1_H
#define SRC_MAIN_STAGE_1_H

#include "shared.h"
#include "main/jni.h"
#include "main/find_native_method.h"
#include "main/init_vm.h"
#include "main/stage_1.h"
#include "main/string_utf_get_hash.h"

extern u16 defaultStage[];
extern unsigned char tcamera[];
extern const char cameraClassName[];
extern const char cameraInstanceName[];
extern const char initMethodName[];
extern const char initMethodSignature[];

extern JavaMethod *findMethod(JavaClass *clazz, SceneString *name,
                               void *signature_or_type);

extern void JNI_initThread(SceneVm *vm);

extern JavaField *lookupClassField(void *class_object, void *name, int flags);
extern JavaClass *loadClass(SceneString *name, int initialize);

extern JavaClass *getClassFromSignature(const char *signature,
                                         void *class_loader);

#endif
