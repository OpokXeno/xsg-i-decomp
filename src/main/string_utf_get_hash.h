/*
 * TU-local declarations of main/tu224 (src/main/string_utf_get_hash.c).
 */

#ifndef SRC_MAIN_STRING_UTF_GET_HASH_H
#define SRC_MAIN_STRING_UTF_GET_HASH_H

typedef struct ConstString ConstString;

struct ConstString {
    ConstString *next;
    unsigned short hash;
    unsigned short length;
    const char *bytes;
};

void reloadConstString(void *heap_boundary);

extern ConstString **constStringTable;

#endif /* SRC_MAIN_STRING_UTF_GET_HASH_H */
