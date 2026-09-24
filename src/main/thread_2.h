/*
 * TU-local declarations of main/tu232 (src/main/thread_2.c).
 */

#ifndef SRC_MAIN_THREAD_2_H
#define SRC_MAIN_THREAD_2_H

#include "shared.h"

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU) and src/main/jni.h. Every Java native in
 * this TU map receives (thread, arguments[, result]) the same way; the
 * `thread` argument itself is never dereferenced by any function here, so
 * it stays the incomplete tag those TUs define, restated here verbatim
 * because a TU-local header cannot be included from another TU.
 */
typedef struct JThread JThread;

/*
 * The same JThread object as far as this TU's own natives reach it, not
 * through the `thread` argument above but through setTarget's and
 * start__/stop__'s own call blocks. src/main/chr.h documents the whole
 * record for main/tu248; a TU-local header cannot be included from another
 * TU, so only the fields this TU's functions read or write are modeled
 * here, under this TU's own tag:
 *
 *   +0x10 object  setTarget stores its target argument here
 *                 (sw s2,16(s0) at 0x002f65d4).
 *   +0x14 reset   setTarget installs JTHREAD_default here
 *                 (sw v1,20(s0) at 0x002f65e8).
 *   +0x18 method  setTarget stores the resolved method here
 *                 (sw v0,24(s0) at 0x002f65dc).
 *   +0x24 flags   start__ sets bit 0x10 (lw/or/sw at
 *                 0x002f6618..0x002f6624); stop__ clears it (lw/and/sw at
 *                 0x002f6640..0x002f664c).
 */
typedef struct Thread2Handle {
    unsigned char unmodeled_00[0x10];
    SceneObject object;                          /* +0x10 */
    void (*reset)(struct Thread2Handle *thread); /* +0x14 */
    SceneMethod *method;                          /* +0x18 */
    unsigned char unmodeled_1c[0x24 - 0x1c];
    unsigned int flags;                           /* +0x24 */
} Thread2Handle;

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
    Thread2Handle *thread;    /* +0x0 */
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

/* Still INCLUDE_ASM in src/main/jthread.c; this is the `reset` hook
 * setTarget installs, called with the Thread2Handle it resets. */
extern void JTHREAD_default(Thread2Handle *thread);

/*
 * Canonical spelling already used identically in src/main/scene_1.h and
 * src/main/script.c (config/header-canon.json).
 */
extern SceneMethod *findMethod(SceneClass *scene_class, SceneString *name,
                               void *signature_or_type);

/* Canonical spelling (config/header-canon.json). */
extern SceneString *loadConstString(const char *bytes, int length);

/*
 * The cached "void" method-signature type findMethod's third argument
 * accepts, already declared the same way (as a plain pointer) in
 * src/main/scene_1.h and src/main/script.c.
 */
extern void *TYPE_Void;

#endif /* SRC_MAIN_THREAD_2_H */
