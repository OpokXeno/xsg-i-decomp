#ifndef INIT_VM_RECOVERY_PRIVATE_H
#define INIT_VM_RECOVERY_PRIVATE_H

#include "shared.h"
#include "main/find_native_method.h"
#include "main/init_class_db.h"
#include "main/init_vm.h"
#include "main/string_utf_get_hash.h"

/*
 * Primitive classes are JavaClass records newClass() allocates:
 * initWrapperClass sets name (+4) and flags bit 0x100 (+0xA) and packs the
 * descriptor character (typeCode, +0x39) and byte size (elementSize, +0x3A)
 * (initPrimitiveTypes, main 0x002f4390..0x002f4454, calls initWrapperClass
 * once per JVM primitive: 'Z'/1 boolean, 'B'/1 byte, 'C'/2 char, 'S'/2
 * short, 'I'/4 int, 'J'/8 long, 'F'/4 float, 'D'/8 double).
 */

/*
 * xheap.c's (main/tu225) own free-block heap header. initVM never reads or
 * writes through it, only forwards a null base to reset the heap, exactly
 * as src/main/jni.h restates the same opaque tag for the same reason.
 */
typedef struct xheap_block xheap_block;

extern JavaClass *classStringBuffer;
extern SceneString *TYPE_Chr_talk;
extern SceneString *TYPE_Stage_entered;
extern SceneString *ATTR_SourceFile;
extern SceneString *ATTR_InnerClasses;
extern SceneString *ATTR_LocalVariableTable;

/* Neither ever dereferenced here: initVM only resets both to empty. */
extern void *classEntryPool;
extern const char clinitName[]; /* "<clinit>" */
extern const char sourceFileAttrName[]; /* "SourceFile" */
extern const char innerClassesAttrName[]; /* "InnerClasses" */
extern const char exceptionsAttrName[]; /* "Exceotions" */
extern const char lineNumberTableAttrName[]; /* "LineNumberTable" */
extern const char constantValueAttrName[]; /* "ConstantValue" */
extern const char localVariableTableAttrName[]; /* "LocalVariableTable" */
extern const char initVmConstructorName[]; /* "<init>" */
extern const char voidMethodDescriptor[]; /* "()V" */
extern const char intVoidMethodDescriptor[]; /* "(I)V" */
extern const char codeAttrName[]; /* "Code" */
extern const char objectClassName[]; /* "java/lang/Object" */
extern const char stringBufferClassName[]; /* "java/lang/StringBuffer" */
extern const char stringClassName[]; /* "java/lang/String" */

extern void xheap_init(int keep_heap, xheap_block *heap, int size);

/*
 * JNI_createThread is defined in main/tu241 (src/main/toolkit.c); this TU
 * only stores its result into initVMThread and never dereferences it, so
 * the returned handle is modeled as an opaque pointer here rather than
 * restating toolkit.c's own TU-local thread-object type.
 */
extern void *JNI_createThread(int kind, int stack_words, int frame_words);

#endif /* INIT_VM_RECOVERY_PRIVATE_H */
