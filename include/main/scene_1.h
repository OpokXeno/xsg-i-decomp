#ifndef INCLUDE_MAIN_SCENE_1_H
#define INCLUDE_MAIN_SCENE_1_H

#include "shared.h"
#include "main/find_native_method.h"
#include "main/jni.h"

/*
 * SCENE_instance only ever writes the window's word 0 (0x0025a310); nothing
 * in this TU reads or writes any other window offset, so the window is
 * modeled as just the shared object header.
 */
struct SceneWindow {
    SceneObjectHeader header;
};

/*
 * The VM's own text record: a length and the bytes it counts.
 *
 * SCENE_start reads both of them out of the storage a java.lang.String hands
 * it and passes them straight to loadConstString (lw 4 and lw 8 at
 * 0x0025a7f4/0x0025a7f8), whose second parameter is the byte count -1 stands
 * for elsewhere in this unit.  The same +8 byte pointer is what the engine's
 * own debug printer reads to print a name: DB_evtMonitor (src/main/script.c's
 * TU, 0x00264208..0x0026421c) prints "%s.%s" from the class name's +8 and the
 * running method's name record +8, the two records include/shared.h models as
 * SceneClassName.binary_name and SceneTypeDescriptor.signature.  Word 0 is
 * read by nothing here and stays a byte range.
 */
struct SceneStringStorage {
    unsigned char unmodeled_00[4];  /* +0x00 */
    int length;                     /* +0x04 */
    const char *bytes;              /* +0x08 */
};

#endif /* INCLUDE_MAIN_SCENE_1_H */
