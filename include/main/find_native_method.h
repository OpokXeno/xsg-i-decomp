#ifndef INCLUDE_MAIN_FIND_NATIVE_METHOD_H
#define INCLUDE_MAIN_FIND_NATIVE_METHOD_H

#include "shared.h"

/*
 * Also touched by loadClass, which caches its own findClass lookup at +0x8
 * (read back on the next call, re-resolved only while it is still zero).
 */
typedef struct ClassEntry {
    u8 unmodeled_00[0x8];
    void *resolvedClass;      /* +0x8: cached findClass(this) result */
    void *next;                /* +0xC: next entry in the class-cache chain */
    u8 unmodeled_10[0x8];
    void *classPointer;  /* +0x18: copied into a new instance's header word */
    u8 unmodeled_1c[0x16];
    u16 staticFieldCount; /* +0x32: zeroed for a class newClass has just made */
    u8 unmodeled_34[0x4];
    int instanceSize;    /* +0x38: xmalloc size for a new instance of this class */
    u8 unmodeled_3c[4]; /* Remaining bytes of the 0x40-byte class allocation. */
} ClassEntry;

#endif /* INCLUDE_MAIN_FIND_NATIVE_METHOD_H */
