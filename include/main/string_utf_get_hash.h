#ifndef INCLUDE_MAIN_STRING_UTF_GET_HASH_H
#define INCLUDE_MAIN_STRING_UTF_GET_HASH_H

#include "shared.h"

/* xmalloc categories of the class loader's allocations (xheap_info totals). */
enum {
    VM_HEAP_LOOKUP_TABLE = 2,
    VM_HEAP_FIELDS = 10,
    VM_HEAP_METHODS = 11,
    VM_HEAP_INTERFACES = 12,
    VM_HEAP_OBJECT = 14,
    VM_HEAP_DISPATCH_TABLE = 15,
    VM_HEAP_STRING = 20
};

/* The class and constant-string lookup tables are hash tables of 512 chains. */
#define VM_HASH_BUCKETS 512
#define VM_HASH_MASK (VM_HASH_BUCKETS - 1)
#define VM_HASH_LAST_BUCKET (VM_HASH_BUCKETS - 1)

/* java.lang.String.hashCode: hash = hash * 31 + byte. */
#define JAVA_HASH_MULTIPLIER 31

/* A ConstString is a 12-byte header; loadConstString2 appends the bytes and a NUL. */
#define VM_CONST_STRING_HEADER_SIZE 12

void reloadConstString(void *heap_boundary);

/* Functions and data used by other translation units. */
SceneString *loadConstString(const char *bytes, int length);
SceneString *loadConstString2(const char *bytes, int length);

#endif /* INCLUDE_MAIN_STRING_UTF_GET_HASH_H */
