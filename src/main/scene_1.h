/*
 * TU-local declarations of main/tu139 (src/main/scene_1.c).
 */

#ifndef SRC_MAIN_SCENE_1_H
#define SRC_MAIN_SCENE_1_H

#include "shared.h"
#include "main/jni.h"
#include "main/scene_1.h"
#include "main/find_native_method.h"


#include "main/twsys_init.h"
#include "main/init_vm.h"
#include "main/jthread.h"
#include "main/string_utf_get_hash.h"

typedef struct SceneType SceneType;

typedef struct SceneStringStorage SceneStringStorage;


extern const char scene_constructor_format[];
extern const char scene_message_field_name[];
extern const char scene_init_name[];
extern const char scene_window_class_name[];
extern const char scene_cleanup_name[];
extern const char scene_void_signature[];

extern JavaField *lookupClassField(void *class_object, void *name, int flags);
extern JavaClass *getClassFromSignature(const char *signature,
                                         void *class_loader);

extern JavaClass *loadClass(SceneString *name, int initialize);
extern void TWIN_initScene(TComponent *window);

/*
 * Return-type caveat (independent review 2026-09-10): `long` is a proven
 * zero-extra-instruction codegen lever blocking ee-gcc2.96 -O2 sibling-call
 * (`int`/`void`/int-temp all sibcall to the 24B `j` form; `long` keeps the
 * 32B `jal` form; `unsigned char`/`short` add truncation). `void` vs
 * predicate `int` vs `long` dispose semantics are unproven from callers/data:
 * no direct `jal SCENE_dispose` callers in SLUS_204.69, JNI_isInstanceOf
 * returns int 0/1 (see SCENE_start predicate use), and the original forwards
 * a0/v0 unchanged with gp-loaded classJava_xeno_Scene. For 0/1 values
 * int/long are indistinguishable (upper already zero). No cleaner pure-C
 * hypothesis exists in evidence.
 */
long SCENE_dispose(SceneObject scene_object);

/* The observed third argument is either a loaded signature string or a cached type. */
extern JavaMethod *findMethod(JavaClass *clazz, SceneString *name,
                               void *signature_or_type);

/* canon: config/header-canon.json chose src/core/main-0025a6d8/private.h over 1 other accepted spelling */
extern void JNI_initThread(SceneVm *vm);

#endif /* SRC_MAIN_SCENE_1_H */
