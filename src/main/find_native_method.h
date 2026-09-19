/*
 * TU-local declarations of main/tu227 (src/main/find_native_method.c).
 */

#ifndef SRC_MAIN_FIND_NATIVE_METHOD_H
#define SRC_MAIN_FIND_NATIVE_METHOD_H

#include "shared.h"

extern void *xmalloc(int size, int type);

/*
 * Partial view of the class descriptor an object is instantiated from.
 * Only the members newObject and newArray (via lookupArray's result) touch
 * are modeled; the rest are unmodeled_XX spans.
 */
typedef struct ClassEntry {
    u8 unmodeled_00[0x18];
    void *classPointer;  /* +0x18: copied into a new instance's header word */
    u8 unmodeled_1c[0x1c];
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

void *newArray(ElementType *elementType, int length);

#endif /* SRC_MAIN_FIND_NATIVE_METHOD_H */
