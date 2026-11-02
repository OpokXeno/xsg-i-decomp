/*
 * TU-local declarations of main/tu227 (src/main/find_native_method.c).
 */

#ifndef SRC_MAIN_FIND_NATIVE_METHOD_H
#define SRC_MAIN_FIND_NATIVE_METHOD_H

#include "shared.h"
#include "main/find_native_method.h"

typedef signed char s8;

/* Header every heap array newArray creates begins with. */
typedef struct ArrayHeader {
    void *classPointer;
    int length;
    void *data;
} ArrayHeader;

extern void *xmalloc(int size, int type);
extern JavaClass *lookupArray(JavaClass *elementType);
void *newArray(JavaClass *elementType, int length);
JavaClass *loadClass(SceneString *name, void *classLoader);
JavaClass *getClassFromSignature(const char *signature, void *classLoader);

#endif /* SRC_MAIN_FIND_NATIVE_METHOD_H */
