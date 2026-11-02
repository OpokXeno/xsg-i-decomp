#ifndef INCLUDE_MAIN_TOOLKIT_H
#define INCLUDE_MAIN_TOOLKIT_H

#include "main/find_native_method.h"
#include "main/jni.h"

typedef struct JavaThread {
    unsigned int : 32; /* +0 */
    unsigned int : 32; /* +4 */
    unsigned int : 32; /* +8 */
    unsigned int : 32; /* +12 */
    SceneObject java_object; /* +16: written at 0x2f9480 (sw s1,16(v0)) */
} JavaThread;

/* Functions and data used by other translation units. */
extern void *getPeer_Chr(SceneObject java_character);

#endif /* INCLUDE_MAIN_TOOLKIT_H */
