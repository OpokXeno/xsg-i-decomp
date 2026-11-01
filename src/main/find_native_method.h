/*
 * TU-local declarations of main/tu227 (src/main/find_native_method.c).
 */

#ifndef SRC_MAIN_FIND_NATIVE_METHOD_H
#define SRC_MAIN_FIND_NATIVE_METHOD_H

#include "shared.h"

typedef signed char s8;

typedef struct JavaClass JavaClass;

typedef struct ClassNameKey ClassNameKey;

/* Header every heap array newArray creates begins with. */
typedef struct ArrayHeader {
    void *classPointer;
    int length;
    void *data;
} ArrayHeader;

extern void *xmalloc(int size, int type);
void *newObject(SceneClass *scene_class);
extern JavaClass *lookupArray(ElementType *elementType);
void *newArray(ElementType *elementType, int length);
JavaClass *loadClass(ClassNameKey *name, void *classLoader);
SceneClass *getClassFromSignature(const char *signature, void *classLoader);
void methodDescripter(u8 *descriptor, s16 *paramSize, s16 *returnSize, s8 *returnType);

/*
 * Partial view of the element-type descriptor newArray and lookupArray use.
 * Only the members those functions touch are modeled.
 */
#ifndef INCLUDE_MAIN_FIND_NATIVE_METHOD_H
typedef struct ElementType {
    u8 unmodeled_00[4];
    ClassNameKey *name; /* +0x4: lookupArray reads the class-name bytes */
    u8 unmodeled_08[2];
    u16 flags;           /* +0xa: bit 0x100 marks a byte-sized element type */
    void *classLoader; /* +0xc: forwarded to lookupClassEntry */
    u8 unmodeled_10[0x29];
    s8 typeCode; /* +0x39: signed character supplied to the primitive name format */
    u8 elementSize;       /* +0x3a: per-element byte size for byte-sized elements */
} ElementType;
#endif

/* Header word every heap object newObject creates begins with. */
#ifndef INCLUDE_MAIN_FIND_NATIVE_METHOD_H
typedef struct ObjectHeader {
    void *classPointer;
} ObjectHeader;
#endif

#endif /* SRC_MAIN_FIND_NATIVE_METHOD_H */
