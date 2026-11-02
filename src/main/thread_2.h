/*
 * TU-local declarations of main/tu232 (src/main/thread_2.c).
 */

#ifndef SRC_MAIN_THREAD_2_H
#define SRC_MAIN_THREAD_2_H

#include "shared.h"
#include "main/init_vm.h"
#include "main/jthread.h"
#include "main/string_utf_get_hash.h"

/*
 * Every Java native in this TU map receives (thread, arguments[, result])
 * with the script VM's per-thread context JThread of include/main/jni.h.
 */

/*
 * The storage record every java.lang.String carries: src/main/scene_1.h
 * documents the same layout as SceneStringStorage (length at +4, byte
 * pointer at +8); a TU-local header cannot be included from another TU, so
 * it is modeled again here under this TU's own tag (lw at
 * 0x002f65b0/0x002f65bc).
 */
typedef struct Thread2StringStorage {
    unsigned char unmodeled_00[4];
    int length;        /* +0x4 */
    const char *bytes; /* +0x8 */
} Thread2StringStorage;

/*
 * A java.lang.String instance, as far as setTarget reads one: the object
 * header, then the storage pointer (lw at 0x002f65a8). Same layout
 * src/main/scene_1.h's SceneString documents.
 */
typedef struct Thread2StringArg {
    unsigned char unmodeled_00[4];
    Thread2StringStorage *storage; /* +0x4 */
} Thread2StringArg;

/*
 * setTarget's call block (lw at 0x002f65a0/0x002f65a4/0x002f65a8): field+0
 * is the receiving Thread's own native handle; field+4 is the target
 * Object argument, stored directly into the handle's object field;
 * field+8 is the method-name String argument.
 */
typedef struct Thread2SetTargetArguments {
    JThread *thread;          /* +0x0 */
    SceneObject target;       /* +0x4 */
    Thread2StringArg *name;   /* +0x8 */
} Thread2SetTargetArguments;

/*
 * Still INCLUDE_ASM in src/main/jni.c; this is chr.h's documented
 * signature for the same allocator (kind, frame words, stack words all
 * plain ints), returning the JThread it allocates.
 */
extern JThread *JNI_createThread(int kind, int stack_words, int frame_words);

/* Already recovered in src/main/jni.c; restated here verbatim. */
extern void JNI_initThread(SceneVm *vm);

/* Canonical spelling of include/main/get_method_signature_class.h. */
extern JavaMethod *findMethod(JavaClass *class_obj, SceneString *name,
                              SceneString *type);

#endif /* SRC_MAIN_THREAD_2_H */
