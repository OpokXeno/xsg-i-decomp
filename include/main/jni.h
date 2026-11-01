#ifndef INCLUDE_MAIN_JNI_H
#define INCLUDE_MAIN_JNI_H

#include "shared.h"
#include "main/data_buffer.h"
#include "main/find_native_method.h"

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
    /* wait_kind selects elapsed frames, a native peer, or a thread. */
    union {
        int elapsed_frames;
        void *object;
        struct JThreadNode *thread;
    } wait_target;                    /* +0x30, four-byte tagged storage */
    int wait_parameter;                 /* +0x34 */
    u16 stack_offset;                   /* +0x38 */
    short stack_limit;                  /* +0x3a */
    u16 resume_frames;                  /* +0x3c */
    u16 frame_depth;                    /* +0x3e */
    short frame_limit;                  /* +0x40 */
} JThread;

extern SceneClass *classJava_xeno_Scene;

/* Proposal: script "Stage" class handle, parallel to the already-shared
 * classJava_xeno_Scene (src/core/main-0025a6d8/private.h). Evidenced by
 * gp-relative access at .sbss+0x4dc7f0 (context.json data_refs). */
extern SceneClass *classJava_xeno_Stage;

extern SceneClass *classJava_xeno_Chr;
extern SceneClass *classJava_xeno_Unit;
extern SceneClass *classJava_xeno_Enepc;
extern SceneClass *classJava_xeno_util_Window;
extern SceneClass *classJava_xeno_Uwamono;
extern SceneClass *classJava_xeno_util_Vector4f;
extern SceneClass *classJava_xeno_util_Layout;
extern SceneClass *classJava_xeno_Effect;
extern SceneClass *classJava_xeno_Light;
extern SceneClass *classJava_xeno_util_Menu;
extern SceneClass *classJava_xeno_util_TCHParams;
extern SceneClass *classJava_xeno_util_Spline;
extern SceneClass *classJava_xeno_util_Input;
extern SceneClass *classJava_xeno_Camera;
extern SceneClass *classJava_xeno_Movie;

void JNI_popFrame(void);

#endif /* INCLUDE_MAIN_JNI_H */
