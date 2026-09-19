#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/js_init", JS_init);

INCLUDE_ASM("asm/main/nonmatchings/js_init", JS_loadClass);

INCLUDE_ASM("asm/main/nonmatchings/js_init", JS_loadConstInteger);

typedef void (*JSNativeMethod)(void);

/*
 * The script-class descriptor JS_loadClass hands back. JS_classAddMethod
 * (still assembler in this TU, 0x0026b260) proves the layout: it reads and
 * increments method_count through this same pointer's +0x8 word, then
 * multiplies the pre-increment count by 12 to place the next entry, so the
 * method table it appends to starts at +0xC. Only the two fields
 * JS_classSetup itself touches are modeled; the rest is unmodeled.
 */
typedef struct JSClass {
    unsigned char unmodeled_00[4];
    JSNativeMethod get_peer; /* +0x4 */
    int method_count;        /* +0x8 */
} JSClass;

/* Configures a freshly loaded script class: installs its native
 * getPeer/getHandle callback and resets its method table. */
void JS_classSetup(JSClass *js_class, JSNativeMethod get_peer) {
    js_class->get_peer = get_peer;
    js_class->method_count = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/js_init", JS_classAddMethod);

INCLUDE_ASM("asm/main/nonmatchings/js_init", JS_findObject);

INCLUDE_ASM("asm/main/nonmatchings/js_init", JS_callMethod);

INCLUDE_ASM("asm/main/nonmatchings/js_init", JS_checkArgs);

INCLUDE_ASM("asm/main/nonmatchings/js_init", JS_exec);

extern int tokenType;

int STR_tokenGetType(void) {
    return tokenType;
}

INCLUDE_ASM("asm/main/nonmatchings/js_init", STR_tokenGetNext);

INCLUDE_ASM("asm/main/nonmatchings/js_init", STR_getLine);

INCLUDE_ASM("asm/main/nonmatchings/js_init", STR_trim);

INCLUDE_ASM("asm/main/nonmatchings/js_init", STR_trim2);
