/*
 * TU-local declarations of main/tu139 (src/main/scene_1.c).
 */

#ifndef SRC_MAIN_SCENE_1_H
#define SRC_MAIN_SCENE_1_H

#include "shared.h"

typedef struct SceneWindow SceneWindow;

typedef struct SceneType SceneType;

typedef struct SceneStringStorage SceneStringStorage;

/*
 * The VM's own text record: a length and the bytes it counts.
 *
 * SCENE_start reads both of them out of the storage a java.lang.String hands
 * it and passes them straight to loadConstString (lw 4 and lw 8 at
 * 0x0025a7f4/0x0025a7f8), whose second parameter is the byte count -1 stands
 * for elsewhere in this unit.  The same +8 byte pointer is what the engine's
 * own debug printer reads to print a name: DB_evtMonitor (src/main/script.c's
 * TU, 0x00264208..0x0026421c) prints "%s.%s" from the class name's +8 and the
 * running method's name record +8, the two records include/shared.h models as
 * SceneClassName.binary_name and SceneTypeDescriptor.signature.  Word 0 is
 * read by nothing here and stays a byte range.
 */
struct SceneStringStorage {
    unsigned char unmodeled_00[4];  /* +0x00 */
    int length;                     /* +0x04 */
    const char *bytes;              /* +0x08 */
};

/*
 * A java.lang.String instance, as far as SCENE_start reads one: the object
 * header every SceneObject starts with, then the text record the string's
 * characters live in (lw 4 at 0x0025a7ec).  Nothing here reads any further.
 */
struct SceneString {
    SceneObjectHeader header;     /* +0x00 */
    SceneStringStorage *storage;  /* +0x04 */
};

/*
 * The VM thread, as far as the two units that touch it establish it.
 *
 * It is the same object a SceneVm handle points at: JTHREAD_defaultScene
 * (0x00305638) is handed a thread and passes it unchanged as JNI_callMethod's
 * first argument, which every other caller in this unit passes a SceneVm to.
 *
 *   +0x10 object   the instance the thread belongs to.  JTHREAD_get
 *                  (0x00305958) walks the jthreadTop list comparing this word
 *                  with its argument, so it is the search key; CallMethod
 *                  (src/main/script.c, lw 0x10 at 0x00261b9c) reads the Stage
 *                  object out of the script's thread through it, and
 *                  DB_evtMonitor follows its class chain to print the thread.
 *   +0x14 entry    the hook JTHREAD_cntl calls once per visit with the thread
 *                  as its only argument (lw 0x14/jalr at
 *                  0x00305910..0x00305920).  SCENE_start installs
 *                  JTHREAD_defaultScene here.
 *   +0x18 method   the method `entry` runs: JTHREAD_defaultScene loads it,
 *                  returns when it is null and otherwise calls it on `object`
 *                  (lw 0x18 at 0x00305654, the JNI_callMethod at 0x00305668).
 *                  SCENE_start is what stores it.
 *   +0x24 flags    bit 4 is "there is a method to run": JTHREAD_cntl skips a
 *                  thread without it (andi 0x10 at 0x003058e4) and
 *                  JTHREAD_defaultScene clears it after the call unless bit 3
 *                  survived (and -0x11 / andi 8 at 0x00305670..0x00305684).
 *                  Bit 3 is the pending-call bit src/main/script.c tests after
 *                  JNI_callMethod, and JTHREAD_waitFor clears it in the thread
 *                  it wakes (0x003057c0).  Bits 0 and 5, which SCENE_start
 *                  sets together on the calling VM, have no reader in this
 *                  unit and are not named.
 *
 * Nothing between those members is recovered, so they stay byte ranges, and
 * the record does not end at +0x28: JTHREAD_waitFor uses +0x0d, +0x30 and
 * +0x34 and DB_evtMonitor prints a call stack from +0x2c and +0x3e.
 */
struct SceneThread {
    unsigned char unmodeled_00[16];      /* +0x00 */
    SceneObject object;                  /* +0x10 */
    void (*entry)(SceneThread *thread);  /* +0x14 */
    SceneMethod *method;                 /* +0x18 */
    unsigned char unmodeled_1c[8];       /* +0x1c */
    unsigned int flags;                  /* +0x24 */
};

#define SCENE_THREAD_HAS_METHOD 0x10u

/*
 * SCENE_instance only ever writes the window's word 0 (0x0025a310); nothing
 * in this TU reads or writes any other window offset, so the window is
 * modeled as just the shared object header.
 */
struct SceneWindow {
    SceneObjectHeader header;
};

void SCENE_instance(SceneVm *vm, SceneObject scene_object);

extern void *classJava_xeno_Chr;

extern JavaField *lookupClassField(void *class_object, void *name, int flags);

extern SceneString *NAME_Constructor;

extern const char scene_constructor_format[];

extern const char scene_message_field_name[];

extern const char scene_init_name[];

extern const char scene_window_class_name[];

extern SceneString *loadConstString2(const char *bytes, int length);

extern SceneClass *getClassFromSignature(const char *signature,
                                         void *class_loader);

extern int instanceOf(SceneClass *scene_class, SceneClass *parent_class);

extern SceneClass *loadClass(SceneString *name, int initialize);

extern SceneObject newObject(SceneClass *scene_class);

extern SceneWindow *TWIN_create2(int component_id);

extern void TWIN_initScene(SceneWindow *window);

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

void SCENE_cleanup(SceneVm *vm, SceneObject scene_object);

void SCENE_start(SceneVm *vm, SceneObject scene_object, int mode,
                 void *method_name);

extern SceneType *TYPE_Void;

extern SceneClass *classJava_xeno_Scene;

extern const char scene_cleanup_name[];

extern const char scene_void_signature[];

/* The observed third argument is either a loaded signature string or a cached type. */
extern SceneMethod *findMethod(SceneClass *scene_class, SceneString *name,
                               void *signature_or_type);

/* canon: config/header-canon.json chose src/core/main-0025a6d8/private.h over 0 other accepted spellings */
extern SceneString *loadConstString(const char *bytes, int length);

/* canon: config/header-canon.json chose src/core/main-0025a6d8/private.h over 1 other accepted spelling */
extern void JNI_initThread(SceneVm *vm);

/*
 * Fourth argument is an optional VM-result output pointer.  The original
 * JNI_callMethod preserves incoming a3 (002f0580) and forwards it unchanged
 * (002f05fc) to virtualMachine, which stores one result word through it
 * (002f36a0/002f36a4/002f36ac: lw v1,0(v0); lw v0,64(sp); sw v1,0(v0)).
 * NULL requests no output.  It is not an integer result value.
 */
extern void JNI_callMethod(SceneVm *vm, SceneMethod *method,
                           SceneObject *arguments, int *output);

/* canon: config/header-canon.json chose src/core/main-0025a6d8/private.h over 0 other accepted spellings */
extern int JNI_isInstanceOf(SceneObject object, SceneClass *target_class);

extern SceneThread *JTHREAD_get(SceneObject object);

extern void JTHREAD_defaultScene(SceneThread *thread);

#endif /* SRC_MAIN_SCENE_1_H */
