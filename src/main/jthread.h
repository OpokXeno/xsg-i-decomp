/*
 * TU-local declarations of main/tu253 (src/main/jthread.c).
 */

#ifndef SRC_MAIN_JTHREAD_H
#define SRC_MAIN_JTHREAD_H

#include "shared.h"

/*
 * The VM thread object JTHREAD_defaultUnit/JTHREAD_defaultChr are installed
 * on as the `entry` hook. src/main/scene_1.h and src/main/script.h already
 * own the same object under the tag `SceneThread` (both restate it with
 * `entry` and more of its fields for their own TUs' needs); that tag
 * belongs to those two TUs, so this one restates only the fields these two
 * functions touch under its own tag instead:
 *
 *   +0x10 object  the Java instance the thread belongs to (lw at
 *                 0x00305470/0x00305538); JNI_callMethod's caller-supplied
 *                 argument block stores it at index 0 (sw at
 *                 0x003054dc/0x003055a4).
 *   +0x18 method  the method to invoke, forwarded straight to
 *                 JNI_callMethod (lw at 0x003054c8/0x00305590).
 *   +0x24 flags   bit 4 (0x10) gates the call below; while it is set, bit 0
 *                 (0x1) is additionally tested and, if set, cleared while
 *                 bit 1 (0x2) is set instead (0x003054b0..0x003054c4). After
 *                 the call, bit 3 (0x8) being set is what clears bit 4
 *                 (0x003054e0..0x003054f4).
 *
 * Nothing else is read or written here; the record is not modeled beyond
 * +0x24.
 */
typedef struct JThreadHandle {
    unsigned char unmodeled_00[0x10];
    SceneObject object;             /* +0x10 */
    unsigned char unmodeled_14[0x18 - 0x14];
    SceneMethod *method;            /* +0x18 */
    unsigned char unmodeled_1c[0x24 - 0x1c];
    unsigned int flags;             /* +0x24 */
} JThreadHandle;

/*
 * The Unit class's native peer, reached through its "peer" Java field
 * (lookupClassField(classJava_xeno_Unit, "peer", 0), the D_004DC080 string
 * below). src/main/unit.h recovers the whole record as `UnitPeer` for
 * main/tu249; a TU-local header cannot be included from another TU, so
 * only the one word this TU reads is modelled here, under its own tag:
 * bit 1 (0x2) of the flags word at +0x00 gates the call below (andi 0x2 at
 * 0x00305490).
 */
typedef struct JThreadUnitPeer {
    unsigned int flags; /* +0x00 */
} JThreadUnitPeer;

/*
 * The Chr class's native peer, reached the same way through its own "peer"
 * Java field. src/main/chr.h recovers the whole record as `Actor` for
 * main/tu248 with the same flags word at +0x00; only that one word is read
 * here (andi 0x2 at 0x00305558), under its own tag for the same reason.
 */
typedef struct JThreadChrPeer {
    unsigned int flags; /* +0x00 */
} JThreadChrPeer;

/*
 * The Stage instance's own fixed layout, read directly by offset (unlike
 * Unit/Chr's peer, which goes through lookupClassField): +0x2c holds the
 * "stage" SceneObject this function debug-prints and forwards to
 * JNI_callMethod as its own object argument (lw at 0x003056b4; sw at
 * 0x00305708). Nothing else of it is read here.
 */
typedef struct JThreadStageObject {
    unsigned char unmodeled_00[0x2c];
    SceneObject stage; /* +0x2c */
} JThreadStageObject;

/*
 * classJava_xeno_Unit is already declared in shared.h; classJava_xeno_Chr
 * is not, since its layout-sensitive uses live in other TUs' own headers
 * (src/main/chr.h, src/main/scene_1.h, src/main/layout.h,
 * src/main/toolkit.h). This TU only ever passes it to lookupClassField, so
 * it is restated verbatim with those TUs' spelling.
 */
extern void *classJava_xeno_Chr;

/*
 * lookupClassField/loadConstString/JNI_callMethod are the runtime's
 * class-field lookup and JVM-call primitives, already recovered with this
 * exact signature in src/main/jni.c (JNI_callMethod) and restated verbatim
 * in several other TUs' own headers (e.g. src/main/scene_1.h); a TU-local
 * header cannot be included from another TU, so they are declared verbatim
 * again here.
 */
extern JavaField *lookupClassField(void *class_object, void *name, int flags);
extern SceneString *loadConstString(const char *bytes, int length);
extern void JNI_callMethod(SceneVm *vm, SceneMethod *method,
                           SceneObject *arguments, int *output);

/*
 * The screen-space debug text primitive, already recovered with this exact
 * signature in several other TUs (e.g. src/main/script.c) and restated
 * verbatim here for the same reason as the primitives above.
 */
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

/* The "peer" field-name string both functions below look up, byte-identical
   in the original binary. */
extern const char D_004DC080[]; /* "peer" */

/* The "Class.method" debug format JTHREAD_defaultStage prints, byte-identical
   in the original binary. */
extern const char D_004DC088[]; /* "%s.%s" */

/*
 * GameLoopState is a 0x2a030-byte global (main VA 0x00338680) whose declared
 * type is TU-local by canon (config/header-canon.json): this TU evidences
 * only the word at +0x10, which JTHREAD_waitFor tests bit 0x800 of (lw
 * v0,-31088(v1) with v1=0x340000 at 0x00305824, 0x340000-31088 = 0x338690).
 */
typedef unsigned int GameLoopStateWords[];
extern GameLoopStateWords GameLoopState;

#endif /* SRC_MAIN_JTHREAD_H */
