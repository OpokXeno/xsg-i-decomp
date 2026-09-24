/*
 * TU-local declarations of main/tu227 (src/main/find_native_method.c).
 */

#ifndef SRC_MAIN_FIND_NATIVE_METHOD_H
#define SRC_MAIN_FIND_NATIVE_METHOD_H

#include "shared.h"

typedef signed char s8;

extern void *xmalloc(int size, int type);

/*
 * Partial view of the class descriptor an object is instantiated from.
 * Only the members newObject and newArray (via lookupArray's result) touch
 * are modeled; the rest are unmodeled_XX spans.
 */
/*
 * Also touched by loadClass, which caches its own findClass lookup at +0x8
 * (read back on the next call, re-resolved only while it is still zero).
 */
typedef struct ClassEntry {
    u8 unmodeled_00[0x8];
    void *resolvedClass;      /* +0x8: cached findClass(this) result */
    u8 unmodeled_0c[0xc];
    void *classPointer;  /* +0x18: copied into a new instance's header word */
    u8 unmodeled_1c[0x16];
    u16 staticFieldCount; /* +0x32: zeroed for a class newClass has just made */
    u8 unmodeled_34[0x4];
    int instanceSize;    /* +0x38: xmalloc size for a new instance of this class */
} ClassEntry;

/*
 * Partial view of the element-type descriptor newArray resolves an array
 * class from.  Only the members this function touches are modeled.
 */
typedef struct ElementType {
    u8 unmodeled_00[0xa];
    u16 flags;           /* +0xa: bit 0x100 marks a byte-sized element type */
    u8 unmodeled_0c[0x2e];
    u8 elementSize;       /* +0x3a: per-element byte size for byte-sized elements */
} ElementType;

extern ClassEntry *lookupArray(ElementType *elementType);

/* Header word every heap object newObject creates begins with. */
typedef struct ObjectHeader {
    void *classPointer;
} ObjectHeader;

/* Header every heap array newArray creates begins with. */
typedef struct ArrayHeader {
    void *classPointer;
    int length;
    void *data;
} ArrayHeader;

void *newObject(ClassEntry *clazz);

ClassEntry *newClass(void);

void *newArray(ElementType *elementType, int length);

int loadClass(int classKey, int skipLoad);

void getClassFromSignature(const char *signature);

void methodDescripter(u8 *descriptor, s16 *paramSize, s16 *returnSize, s8 *returnType);

#endif /* SRC_MAIN_FIND_NATIVE_METHOD_H */
