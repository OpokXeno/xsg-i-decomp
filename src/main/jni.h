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

/*
 * The fixed bank of 8 script class ids JNI_loadClassDB allocates from for a
 * negative id and stores into for an id below 0x100; an id at or above 0x100
 * instead reads back the entry at its low byte.
 */
extern int classDB[8];

/*
 * Resolves a class already loaded from the class database and installs its
 * native method table, given the address of the engine's cached
 * class-object slot and the class's fully-qualified name. Defined at main
 * 0x002f5120 (src/main/find_native_method.c).
 */
extern void *loadStaticClass(void **class_slot, u8 *class_name);

extern void *classJava_xeno_Camera;
extern void *classJava_xeno_Chr;
extern void *classJava_xeno_Effect;
extern void *classJava_xeno_Enepc;
extern void *classJava_xeno_Light;
extern void *classJava_xeno_Movie;
extern void *classJava_xeno_PlayControl;
extern void *classJava_xeno_Scene;
extern void *classJava_xeno_Stage;
extern void *classJava_xeno_Uwamono;
extern void *classJava_xeno_util_Format;
extern void *classJava_xeno_util_Input;
extern void *classJava_xeno_util_Layout;
extern void *classJava_xeno_util_Menu;
extern void *classJava_xeno_util_Runtime;
extern void *classJava_xeno_util_Spline;
extern void *classJava_xeno_util_TCHParams;
extern void *classJava_xeno_util_Toolkit;
extern void *classJava_xeno_util_Vector4f;
extern void *classJava_xeno_util_Window;
extern void *classJava_xeno_vm_System;

/* The fully-qualified class names JNI_loadNativeClass resolves. */
extern u8 D_004CCA70[]; /* "xeno/vm/System" */
extern u8 D_004CCA80[]; /* "xeno/util/Format" */
extern u8 D_004CCA98[]; /* "xeno/util/Menu" */
extern u8 D_004CCAA8[]; /* "xeno/util/Window" */
extern u8 D_004CCAC0[]; /* "xeno/util/Input" */
extern u8 D_004CCAD0[]; /* "xeno/util/Layout" */
extern u8 D_004CCAE8[]; /* "xeno/util/Runtime" */
extern u8 D_004CCB00[]; /* "xeno/util/Toolkit" */
extern u8 D_004CCB18[]; /* "xeno/util/TCHParams" */
extern u8 D_004CCB30[]; /* "xeno/util/Spline" */
extern u8 D_004CCB48[]; /* "xeno/util/Vector4f" */
extern u8 D_004CCB60[]; /* "xeno/Camera" */
extern u8 D_004CCB70[]; /* "xeno/Effect" */
extern u8 D_004CCB80[]; /* "xeno/Light" */
extern u8 D_004CCB90[]; /* "xeno/Chr" */
extern u8 D_004CCBA0[]; /* "xeno/Enepc" */
extern u8 D_004CCBB0[]; /* "xeno/Unit" */
extern u8 D_004CCBC0[]; /* "xeno/Uwamono" */
extern u8 D_004CCBD0[]; /* "xeno/Stage" */
extern u8 D_004CCBE0[]; /* "xeno/Scene" */
extern u8 D_004CCBF0[]; /* "xeno/PlayControl" */
extern u8 D_004CCC08[]; /* "xeno/Movie" */

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

/*
 * Looks up the bounded set of PdbClassGroup entries registered for class_id.
 * Defined at main 0x002f5d10.
 */
extern void PDB_getEntry(int class_id, void **groups, int *group_count);

/*
 * The scratch reader over a byte range that checkClass scans. Restated as an
 * incomplete tag because a TU-local header cannot be included from another
 * TU: the complete record is main/tu230's src/main/data_buffer.h struct
 * DataBuffer, which this TU never reads or writes directly.
 */
typedef struct DataBuffer DataBuffer;

typedef unsigned char DataBufferByte;

/*
 * Initializes buffer to read length bytes starting at base, little-endian
 * unless big_endian is set. Defined at main 0x002f5f70
 * (src/main/data_buffer.c).
 */
extern void DataBuffer_init(DataBuffer *buffer, DataBufferByte *base, int length, int big_endian);

/*
 * The 32-entry virtual-machine register bank JNI_getRegister reads by
 * (index & 0x1f).
 */
extern int VMRegister[32];

#endif /* SRC_MAIN_JNI_H */
