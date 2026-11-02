#ifndef INCLUDE_MAIN_FIND_NATIVE_METHOD_H
#define INCLUDE_MAIN_FIND_NATIVE_METHOD_H

#include "shared.h"

typedef signed char s8;

void *newObject(JavaClass *clazz);
void *newClass(void);
void loadStaticClass(void *class_slot, const char *name);

/* Functions and data used by other translation units. */
int instanceOf(JavaClass *fromClass, JavaClass *toClass);
void methodDescripter(u8 *descriptor, s16 *paramSize, s16 *returnSize, s8 *returnType);

#endif /* INCLUDE_MAIN_FIND_NATIVE_METHOD_H */
