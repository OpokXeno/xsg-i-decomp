/*
 * TU-local declarations of main/tu253 (src/main/jthread.c).
 */

#ifndef SRC_MAIN_JTHREAD_H
#define SRC_MAIN_JTHREAD_H

#include "shared.h"
#include "main/jni.h"

/* The actual JNI thread owns object+0x10, method+0x18 and flags+0x24.
 * This owner uses that same record through its historical handle name. */
typedef JThread JThreadHandle;

/*
 * The Unit class's native peer, reached through its "peer" Java field
 * (lookupClassField(classJava_xeno_Unit, "peer", 0), the jthreadPeerName string
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
 * GameLoopState is a 0x2a030-byte global (main VA 0x00338680) whose declared
 * type is TU-local by canon (config/header-canon.json): this TU evidences
 * only the word at +0x10, which JTHREAD_waitFor tests bit 0x800 of (lw
 * v0,-31088(v1) with v1=0x340000 at 0x00305824, 0x340000-31088 = 0x338690).
 */
typedef unsigned int GameLoopStateWords[];

/* The "peer" field-name string both functions below look up, byte-identical
   in the original binary. */
extern const char jthreadPeerName[]; /* "peer" */

/* The "Class.method" debug format JTHREAD_defaultStage prints, byte-identical
   in the original binary. */
extern const char jthreadDottedNameFormat[]; /* "%s.%s" */

extern GameLoopStateWords GameLoopState;

void JTHREAD_defaultUnit(JThreadHandle *thread);

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

#endif /* SRC_MAIN_JTHREAD_H */
