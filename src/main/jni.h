/*
 * TU-local declarations of main/tu221 (src/main/jni.c).
 */

#ifndef SRC_MAIN_JNI_H
#define SRC_MAIN_JNI_H

#include "shared.h"

/*
 * The allocation header of src/main/xheap.c's free-block heap
 * (main/tu225). This TU never reads or writes a block itself -- it only
 * forwards the heap base JNI_initSystem's own caller supplies -- so the
 * type stays the incomplete tag xheap.c defines, exactly as
 * src/main/unit.c forwards chr.h's JThread without completing it.
 */
typedef struct xheap_block xheap_block;

extern void xheap_init(int keep_heap, xheap_block *heap, int size);

extern void *xheap_push(void);

extern void *xheap_pop(void);

extern void xheap_current_clear(void);

extern void initClassDB(void);

extern int instanceOf(SceneClass *scene_class, SceneClass *parent_class);

extern void JTHREAD_waitFor(void);

extern void virtualMachine(SceneVm *vm, SceneMethod *method,
                           SceneObject *arguments, int *output);

extern void (*jthreadResetFunc)(void);

/*
 * The VM/native-thread object every JNI_* entry point in this TU shares with
 * the rest of the engine. This is the same record src/main/chr.h documents
 * member by member (its "JThread" comment carries the evidence for every
 * offset); it is restated here verbatim because a TU-local header cannot be
 * included from another TU. This TU touches flags (JNI_callMethod's state
 * tests; JNI_initThread zeroes it), stack_offset, resume_frames and
 * frame_depth.
 */
typedef struct JThread {
    unsigned int : 32;                  /* +0x00 */
    struct JThread *previous;           /* +0x04 */
    struct JThread *next;               /* +0x08 */
    u8 kind;                            /* +0x0c */
    u8 wait_kind;                       /* +0x0d */
    unsigned short : 16;                /* +0x0e */
    void *object;                       /* +0x10 */
    void (*reset)(struct JThread *thread); /* +0x14 */
    void *method;                       /* +0x18 */
    unsigned int : 32;                  /* +0x1c */
    unsigned int : 32;                  /* +0x20 */
    u32 flags;                          /* +0x24 */
    u32 *stack;                         /* +0x28 */
    void *frames;                       /* +0x2c */
    void *wait_target;                  /* +0x30 */
    int wait_parameter;                 /* +0x34 */
    u16 stack_offset;                   /* +0x38 */
    short stack_limit;                  /* +0x3a */
    u16 resume_frames;                  /* +0x3c */
    u16 frame_depth;                    /* +0x3e */
    short frame_limit;                  /* +0x40 */
} JThread;

#endif /* SRC_MAIN_JNI_H */
