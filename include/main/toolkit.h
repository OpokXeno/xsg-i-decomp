#ifndef INCLUDE_MAIN_TOOLKIT_H
#define INCLUDE_MAIN_TOOLKIT_H

typedef struct JavaThread {
    unsigned int : 32; /* +0 */
    unsigned int : 32; /* +4 */
    unsigned int : 32; /* +8 */
    unsigned int : 32; /* +12 */
    void *java_object; /* +16: written at 0x2f9480 (sw s1,16(v0)) */
} JavaThread;

#endif /* INCLUDE_MAIN_TOOLKIT_H */
