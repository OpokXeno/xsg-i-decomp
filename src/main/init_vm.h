#ifndef INIT_VM_RECOVERY_PRIVATE_H
#define INIT_VM_RECOVERY_PRIVATE_H

#include "shared.h"

extern SceneString *loadConstString(const char *bytes, int length);

/*
 * Partial view of the VM class object newClass() allocates: only the
 * members initWrapperClass touches. name (+4) and type_flags (+0xA) are
 * written directly; type_code (+0x39) and element_size (+0x3A) are the two
 * low bytes initWrapperClass packs for a primitive wrapper class only
 * (initPrimitiveTypes, main 0x002f4390..0x002f4454, calls initWrapperClass
 * once per JVM primitive with its one-character type descriptor and byte
 * size: 'Z'/1 boolean, 'B'/1 byte, 'C'/2 char, 'S'/2 short, 'I'/4 int,
 * 'J'/8 long, 'F'/4 float, 'D'/8 double).
 */
typedef struct VMClass VMClass;
struct VMClass {
    unsigned char unmodeled_00[4];
    SceneString *name;
    unsigned char unmodeled_08[2];
    unsigned short type_flags;
    unsigned char unmodeled_0c[0x2d];
    signed char type_code;
    unsigned char element_size;
};

extern VMClass *newClass(void);

/*
 * xheap.c's (main/tu225) own free-block heap header. initVM never reads or
 * writes through it, only forwards a null base to reset the heap, exactly
 * as src/main/jni.h restates the same opaque tag for the same reason.
 */
typedef struct xheap_block xheap_block;

extern void xheap_init(int keep_heap, xheap_block *heap, int size);

/*
 * JNI_createThread is defined in main/tu241 (src/main/toolkit.c); this TU
 * only stores its result into initVMThread and never dereferences it, so
 * the returned handle is modeled as an opaque pointer here rather than
 * restating toolkit.c's own TU-local thread-object type.
 */
extern void *JNI_createThread(int kind, int stack_words, int frame_words);

extern void loadStaticClass(SceneClass **class_slot, const char *name);

extern void *initVMThread;

extern SceneString *NAME_Init;
extern SceneString *NAME_Constructor;
extern SceneString *TYPE_Chr_talk;
extern SceneString *TYPE_Stage_entered;
extern SceneString *TYPE_Void;
extern SceneString *ATTR_SourceFile;
extern SceneString *ATTR_InnerClasses;
extern SceneString *ATTR_Code;
extern SceneString *ATTR_Exceptions;
extern SceneString *ATTR_LineNumberTable;
extern SceneString *ATTR_ConstantValue;
extern SceneString *ATTR_LocalVariableTable;

/* Neither ever dereferenced here: initVM only resets both to empty. */
extern void *classEntryPool;
extern void *constStringTable;

extern void (*jthreadResetFunc)(void);

/* Never dereferenced here: initVM only clears the list head. */
extern void *jthreadTop;

/* The whole 8-byte record (config/symbols/main.txt), cleared a halfword at
 * a time; no instruction in initVM reads any of it back. */
extern unsigned short XTK_peerGroup[4];

extern const char D_004CD0C0[]; /* "<clinit>" */
extern const char D_004CD0D0[]; /* "SourceFile" */
extern const char D_004CD0E0[]; /* "InnerClasses" */
extern const char D_004CD0F0[]; /* "Exceotions" */
extern const char D_004CD100[]; /* "LineNumberTable" */
extern const char D_004CD110[]; /* "ConstantValue" */
extern const char D_004CD120[]; /* "LocalVariableTable" */
extern const char D_004DBFE0[]; /* "<init>" */
extern const char D_004DBFE8[]; /* "()V" */
extern const char D_004DBFF0[]; /* "(I)V" */
extern const char D_004DBFF8[]; /* "Code" */

extern SceneClass *classObject;
extern SceneClass *classString;
extern SceneClass *classStringBuffer;

extern const char D_004CD628[]; /* "java/lang/Object" */
extern const char D_004CD640[]; /* "java/lang/StringBuffer" */
extern const char D_004CD658[]; /* "java/lang/String" */

#endif /* INIT_VM_RECOVERY_PRIVATE_H */
