/*
 * TU-local declarations of main/tu224 (src/main/string_utf_get_hash.c).
 */

#ifndef SRC_MAIN_STRING_UTF_GET_HASH_H
#define SRC_MAIN_STRING_UTF_GET_HASH_H

#include "shared.h"

typedef struct ConstString ConstString;

struct ConstString {
    ConstString *next;
    unsigned short hash;
    unsigned short length;
    /* char *bytes plus a trailing char data[1]: loadConstString2 stores
       entry->bytes as entry + 12 (its own trailing storage) and writes a NUL
       terminator into that trailing array, so the pointee is a mutable,
       object-owned buffer, not a borrowed const string
       (asm/main/nonmatchings/string_utf_get_hash loadConstString2). */
    char *bytes;
    char data[1];
};

extern void *xmalloc(int size, int type);

extern unsigned int strlen(const char *string);

extern int memcmp(const void *left, const void *right, unsigned int count);

extern void *memcpy(void *destination, const void *source, unsigned int count);

int StringUtf_getHash(const char *bytes, int length);

void *StringUtf_create(int length);

ConstString *findConstString(const char *bytes, int length);

void reloadConstString(void *heap_boundary);

extern ConstString **constStringTable;

extern int constStringCount;

SceneString *loadConstString(const char *bytes, int length);

SceneString *loadConstString2(const char *bytes, int length);

#endif /* SRC_MAIN_STRING_UTF_GET_HASH_H */
