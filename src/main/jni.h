/*
 * TU-local declarations of main/tu221 (src/main/jni.c).
 */

#ifndef SRC_MAIN_JNI_H
#define SRC_MAIN_JNI_H

#include "shared.h"
#include "main/data_buffer.h"
#include "main/find_native_method.h"
#include "main/init_class_db.h"
#include "main/init_vm.h"
#include "main/jni.h"

/*
 * The allocation header of src/main/xheap.c's free-block heap
 * (main/tu225). This TU never reads or writes a block itself -- it only
 * forwards the heap base JNI_initSystem's own caller supplies -- so the
 * type stays the incomplete tag xheap.c defines, exactly as
 * src/main/unit.c forwards chr.h's JThread without completing it.
 */
typedef struct xheap_block xheap_block;

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
 * A loaded PDB class-search group: entry_count many PdbClassEntry records
 * follow immediately after this header, at +0x8 (main 0x002f0a90
 * lhu $17,6($3) / addiu $16,$3,8).
 */
typedef struct PdbClassGroup {
    unsigned char unmodeled_00[6];
    unsigned short entry_count; /* +0x6 */
    PdbClassEntry entries[0];   /* +0x8 */
} PdbClassGroup;

extern JavaClass *classJava_xeno_util_Format;
extern JavaClass *classJava_xeno_vm_System;
extern JavaClass *classJava_xeno_util_Runtime;
extern JavaClass *classJava_xeno_util_Toolkit;
extern JavaClass *classJava_xeno_PlayControl;
extern struct JThread *jthreadCurrent;
/* The fully-qualified class names JNI_loadNativeClass resolves. */
extern const char xenoVmSystemClassName[16]; /* "xeno/vm/System" */

extern const char xenoUtilFormatClassName[24]; /* "xeno/util/Format" */
extern const char xenoUtilMenuClassName[16]; /* "xeno/util/Menu" */
extern const char xenoUtilWindowClassName[24]; /* "xeno/util/Window" */
extern const char xenoUtilInputClassName[16]; /* "xeno/util/Input" */
extern const char xenoUtilLayoutClassName[24]; /* "xeno/util/Layout" */
extern const char xenoUtilRuntimeClassName[24]; /* "xeno/util/Runtime" */
extern const char xenoUtilToolkitClassName[24]; /* "xeno/util/Toolkit" */
extern const char xenoUtilTchParamsClassName[24]; /* "xeno/util/TCHParams" */
extern const char xenoUtilSplineClassName[24]; /* "xeno/util/Spline" */
extern const char xenoUtilVector4fClassName[24]; /* "xeno/util/Vector4f" */
extern const char xenoCameraClassName[16]; /* "xeno/Camera" */
extern const char xenoEffectClassName[16]; /* "xeno/Effect" */
extern const char xenoLightClassName[16]; /* "xeno/Light" */
extern const char xenoChrClassName[16]; /* "xeno/Chr" */
extern const char xenoEnepcClassName[16]; /* "xeno/Enepc" */
extern const char xenoUnitClassName[16]; /* "xeno/Unit" */
extern const char xenoUwamonoClassName[16]; /* "xeno/Uwamono" */
extern const char xenoStageClassName[16]; /* "xeno/Stage" */
extern const char xenoSceneClassName[16]; /* "xeno/Scene" */
extern const char xenoPlayControlClassName[24]; /* "xeno/PlayControl" */
extern const char xenoMovieClassName[24]; /* "xeno/Movie" */

/*
 * The 32-entry virtual-machine register bank JNI_getRegister reads by
 * (index & 0x1f).
 */
extern int VMRegister[VM_REGISTER_COUNT];

extern void xheap_init(int keep_heap, xheap_block *heap, int size);
extern void *xheap_push(void);
extern void *xheap_pop(void);
extern void xheap_current_clear(void);
extern void *xmalloc(int size, int type);
extern void JTHREAD_waitFor(void);

extern void virtualMachine(SceneVm *vm, JavaMethod *method,
                           SceneObject *arguments, int *output);

/*
 * Looks up the bounded set of PdbClassGroup entries registered for class_id.
 * Defined at main 0x002f5d10.
 */
extern void PDB_getEntry(int class_id, void **groups, int *group_count);

#endif /* SRC_MAIN_JNI_H */
