#ifndef SRC_MAIN_FORMAT_H
#define SRC_MAIN_FORMAT_H

#include "shared.h"
#include "main/find_native_method.h"
#include "main/init_vm.h"

/* The byte buffer fields read by the format natives at +0x4 and +0x8. */
typedef struct FormatByteArray {
    unsigned char unmodeled_00[4];
    int length;
    unsigned char *data;
} FormatByteArray;

/* The String value pointer written/read at +0x4 by the natives. */
typedef struct FormatString {
    unsigned char unmodeled_00[4];
    FormatByteArray *value;
} FormatString;

/* The byte-class handle is owned by init_vm.c; this TU only passes it to
 * newArray, so keep the VM class object opaque here. */
typedef struct VMClass VMClass;
extern VMClass *classByte;
extern const char D_004DC098[];
extern FormatString *JAVA_tmpString;

extern FormatByteArray *newArray(void *element_class, int length);
extern unsigned char *STRING_int(unsigned char *buffer, int value);

#endif /* SRC_MAIN_FORMAT_H */
