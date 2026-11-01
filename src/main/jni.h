/*
 * TU-local declarations of main/tu221 (src/main/jni.c).
 */

#ifndef SRC_MAIN_JNI_H
#define SRC_MAIN_JNI_H

#include "shared.h"
#include "main/data_buffer.h"
#include "main/find_native_method.h"

/*
 * The allocation header of src/main/xheap.c's free-block heap
 * (main/tu225). This TU never reads or writes a block itself -- it only
 * forwards the heap base JNI_initSystem's own caller supplies -- so the
 * type stays the incomplete tag xheap.c defines, exactly as
 * src/main/unit.c forwards chr.h's JThread without completing it.
 */
typedef struct xheap_block xheap_block;

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

/*
 * A loaded PDB class-search group: entry_count many PdbClassEntry records
 * follow immediately after this header, at +0x8 (main 0x002f0a90
 * lhu $17,6($3) / addiu $16,$3,8).
 */
typedef struct PdbClassGroup {
    unsigned char unmodeled_00[6];
    unsigned short entry_count; /* +0x6 */
} PdbClassGroup;

/*
 * One 16-byte entry of a PdbClassGroup: the byte range DataBuffer_init scans
 * for a class definition (main 0x002f0a90 lw $5/$6,8/0xc($16)).
 */
typedef struct PdbClassEntry {
    unsigned char unmodeled_00[8];
    void *data;  /* +0x8 */
    int length;  /* +0xC */
} PdbClassEntry;

extern SceneClass *classJava_xeno_Enepc;
extern SceneClass *classJava_xeno_Scene;
extern SceneClass *classJava_xeno_util_Window;
extern SceneClass *classJava_xeno_Uwamono;
extern SceneClass *classJava_xeno_Chr;
extern SceneClass *classJava_xeno_util_Format;
extern SceneClass *classJava_xeno_Unit;
extern SceneClass *classJava_xeno_vm_System;
extern SceneClass *classJava_xeno_util_Vector4f;
extern SceneClass *classJava_xeno_util_Runtime;
extern SceneClass *classJava_xeno_util_Layout;
extern SceneClass *classJava_xeno_Effect;
extern SceneClass *classJava_xeno_Light;
extern SceneClass *classJava_xeno_util_Menu;
extern SceneClass *classJava_xeno_util_Toolkit;
extern SceneClass *classJava_xeno_Stage;
extern SceneClass *classJava_xeno_PlayControl;
extern SceneClass *classJava_xeno_util_TCHParams;
extern SceneClass *classJava_xeno_util_Spline;
extern SceneClass *classJava_xeno_util_Input;
extern SceneClass *classJava_xeno_Camera;
extern SceneClass *classJava_xeno_Movie;
extern void *jthreadTop;
extern void *jthreadCurrent;
extern void (*jthreadResetFunc)(void);

/*
 * The fixed bank of 8 script class ids JNI_loadClassDB allocates from for a
 * negative id and stores into for an id below 0x100; an id at or above 0x100
 * instead reads back the entry at its low byte.
 */
extern int classDB[8];

/* The fully-qualified class names JNI_loadNativeClass resolves. */
extern const u8 xenoVmSystemClassName[16]; /* "xeno/vm/System" */

extern const u8 xenoUtilFormatClassName[24]; /* "xeno/util/Format" */
extern const u8 xenoUtilMenuClassName[16]; /* "xeno/util/Menu" */
extern const u8 xenoUtilWindowClassName[24]; /* "xeno/util/Window" */
extern const u8 xenoUtilInputClassName[16]; /* "xeno/util/Input" */
extern const u8 xenoUtilLayoutClassName[24]; /* "xeno/util/Layout" */
extern const u8 xenoUtilRuntimeClassName[24]; /* "xeno/util/Runtime" */
extern const u8 xenoUtilToolkitClassName[24]; /* "xeno/util/Toolkit" */
extern const u8 xenoUtilTchParamsClassName[24]; /* "xeno/util/TCHParams" */
extern const u8 xenoUtilSplineClassName[24]; /* "xeno/util/Spline" */
extern const u8 xenoUtilVector4fClassName[24]; /* "xeno/util/Vector4f" */
extern const u8 xenoCameraClassName[16]; /* "xeno/Camera" */
extern const u8 xenoEffectClassName[16]; /* "xeno/Effect" */
extern const u8 xenoLightClassName[16]; /* "xeno/Light" */
extern const u8 xenoChrClassName[16]; /* "xeno/Chr" */
extern const u8 xenoEnepcClassName[16]; /* "xeno/Enepc" */
extern const u8 xenoUnitClassName[16]; /* "xeno/Unit" */
extern const u8 xenoUwamonoClassName[16]; /* "xeno/Uwamono" */
extern const u8 xenoStageClassName[16]; /* "xeno/Stage" */
extern const u8 xenoSceneClassName[16]; /* "xeno/Scene" */
extern const u8 xenoPlayControlClassName[24]; /* "xeno/PlayControl" */
extern const u8 xenoMovieClassName[24]; /* "xeno/Movie" */

/*
 * The 32-entry virtual-machine register bank JNI_getRegister reads by
 * (index & 0x1f).
 */
extern int VMRegister[32];

extern void xheap_init(int keep_heap, xheap_block *heap, int size);
extern void *xheap_push(void);
extern void *xheap_pop(void);
extern void xheap_current_clear(void);
extern void initClassDB(void);
extern void *xmalloc(int size, int type);
extern int instanceOf(SceneClass *scene_class, SceneClass *parent_class);
extern void JTHREAD_waitFor(void);

extern void virtualMachine(SceneVm *vm, SceneMethod *method,
                           SceneObject *arguments, int *output);

/*
 * Looks up the bounded set of PdbClassGroup entries registered for class_id.
 * Defined at main 0x002f5d10.
 */
extern void PDB_getEntry(int class_id, void **groups, int *group_count);

/*
 * Initializes buffer to read length bytes starting at base, little-endian
 * unless big_endian is set. Defined at main 0x002f5f70
 * (src/main/data_buffer.c).
 */
extern void DataBuffer_init(DataBuffer *buffer, DataBufferByte *base, int length, int big_endian);

#endif /* SRC_MAIN_JNI_H */
