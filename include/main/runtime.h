#ifndef INCLUDE_MAIN_RUNTIME_H
#define INCLUDE_MAIN_RUNTIME_H

#include "shared.h"

/*
 * A java.lang.String argument, as the natives of src/main/runtime.c
 * (CaptureStart, setEventTimer) and src/main/window.c (the print natives) read
 * it: offset +0x00 (the class word every object reference begins with, per
 * shared.h) is untouched by those TUs, and +0x04 holds the interned string
 * record the String was loaded from (lw v1,4(v0) at 0x002f7514/0x002f78ac).
 */
typedef struct StringRef {
    void *unmodeled_00;
    struct InternedString *value;
} StringRef;

/*
 * The interned constant-string record `StringRef.value` points to. The
 * natives read the byte count at +0x04 (length) and the bytes at +0x08 (lw
 * v0,8(v0) at 0x002f7518/0x002f78b0; window.c's print natives pass them to
 * MSG_print2 as its length/text pair). Same object as `ConstString` of
 * src/main/string_utf_get_hash.h (main/tu224): next-in-bucket link and
 * packed hash/length occupy the same leading 8 bytes there.
 */
struct InternedString {
    unsigned char unmodeled_00[4];
    int length;                 /* +0x04 */
    char *bytes;                /* +0x08 */
};

/* Functions and data used by other translation units. */
extern void Java_xeno_util_Runtime_jump_sub(int destination, int argument);
extern void Java_xeno_util_Runtime_jumpCF__II();

#endif /* INCLUDE_MAIN_RUNTIME_H */
