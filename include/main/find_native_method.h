#ifndef INCLUDE_MAIN_FIND_NATIVE_METHOD_H
#define INCLUDE_MAIN_FIND_NATIVE_METHOD_H

#include "shared.h"
#include "main/string_utf_get_hash.h"

typedef signed char s8;

/* Class-file magic number (JVM specification, 4.1). */
#define JVM_CLASS_MAGIC 0xCAFEBABE

/* Constant pool tags (JVM specification, 4.4) and the two the VM writes over
 * an entry once it has resolved it (the entry then holds the object itself). */
enum {
    JVM_CONSTANT_UTF8 = 1,
    JVM_CONSTANT_INTEGER = 3,
    JVM_CONSTANT_FLOAT = 4,
    JVM_CONSTANT_LONG = 5,
    JVM_CONSTANT_DOUBLE = 6,
    JVM_CONSTANT_CLASS = 7,
    JVM_CONSTANT_STRING = 8,
    JVM_CONSTANT_FIELDREF = 9,
    JVM_CONSTANT_METHODREF = 10,
    JVM_CONSTANT_INTERFACE_METHODREF = 11,
    JVM_CONSTANT_NAME_AND_TYPE = 12,
    VM_CONSTANT_RESOLVED_CLASS = 23,
    VM_CONSTANT_RESOLVED_STRING = 24
};

/* Access flags (JVM specification, 4.5 and 4.6). */
enum {
    JVM_ACC_PUBLIC = 0x0001,
    JVM_ACC_STATIC = 0x0008,
    JVM_ACC_FINAL = 0x0010,
    JVM_ACC_NATIVE = 0x0100,
    JVM_ACC_INTERFACE = 0x0200,
    JVM_ACC_ABSTRACT = 0x0400,
    /* addMethod marks constructors with the ACC_STRICT bit. */
    VM_ACC_CONSTRUCTOR = 0x0800
};

/* JavaClass.flags */
#define VM_CLASS_FLAG_PRIMITIVE 0x100

/* SceneField.flags (above the class-file access flags). */
#define VM_FIELD_FLAG_HAS_CONSTANT_VALUE 0x4000
#define VM_FIELD_FLAG_UNRESOLVED_TYPE 0x8000  /* type.descriptor not yet a class */

/* JavaClass.state, in the order processClass reaches them. */
enum {
    CLASS_STATE_MEMBERS_RESOLVED = 4,
    CLASS_STATE_BEFORE_CONSTANTS = 5,
    CLASS_STATE_CONSTANTS_RESOLVED = 6,
    CLASS_STATE_PREPARED = 8,
    CLASS_STATE_INITIALIZED = 11
};

void *newObject(JavaClass *clazz);
void *newClass(void);
void loadStaticClass(void *class_slot, const char *name);

/* Functions and data used by other translation units. */
int instanceOf(JavaClass *fromClass, JavaClass *toClass);
void methodDescripter(u8 *descriptor, s16 *paramSize, s16 *returnSize, s8 *returnType);

#endif /* INCLUDE_MAIN_FIND_NATIVE_METHOD_H */
