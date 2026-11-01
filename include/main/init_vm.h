#ifndef INCLUDE_MAIN_INIT_VM_H
#define INCLUDE_MAIN_INIT_VM_H

#include "shared.h"
#include "main/find_native_method.h"

typedef union VMSlot {
    int i;
    float f;
    void *ref;
    unsigned char b;
    short s;
    unsigned short c;
} VMSlot;

struct VMClass {
    unsigned char unmodeled_00[4];
    SceneString *name;
    unsigned char unmodeled_08[2];
    unsigned short type_flags;
    unsigned char unmodeled_0c[0x2d];
    signed char type_code;
    unsigned char element_size;
};

extern JavaClass *classClass;
extern JavaClass *classObject;
extern SceneClass *classString;

#endif /* INCLUDE_MAIN_INIT_VM_H */
