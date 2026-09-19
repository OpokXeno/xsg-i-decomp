#ifndef INIT_VM_RECOVERY_PRIVATE_H
#define INIT_VM_RECOVERY_PRIVATE_H

#include "shared.h"

extern SceneString *loadConstString(const char *bytes, int length);

/*
 * Partial view of the VM class object newClass() allocates: only the
 * members initWrapperClass touches. name (+4) and type_flags (+0xA) are
 * written directly; type_code (+0x39) and element_size (+0x3A) are the two
 * low bytes initWrapperClass packs for a primitive wrapper class only
 * (initPrimitiveTypes, main 0x002f4390..0x002f4454, calls initWrapperClass
 * once per JVM primitive with its one-character type descriptor and byte
 * size: 'Z'/1 boolean, 'B'/1 byte, 'C'/2 char, 'S'/2 short, 'I'/4 int,
 * 'J'/8 long, 'F'/4 float, 'D'/8 double).
 */
typedef struct VMClass VMClass;
struct VMClass {
    unsigned char unmodeled_00[4];
    SceneString *name;
    unsigned char unmodeled_08[2];
    unsigned short type_flags;
    unsigned char unmodeled_0c[0x2d];
    signed char type_code;
    unsigned char element_size;
};

extern VMClass *newClass(void);

#endif /* INIT_VM_RECOVERY_PRIVATE_H */
