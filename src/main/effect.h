/*
 * TU-local declarations of main/tu245 (src/main/effect.c).
 */

#ifndef SRC_MAIN_EFFECT_H
#define SRC_MAIN_EFFECT_H

#include "shared.h"
#include "main/jni.h"
#include "main/exm_2.h"
#include "main/fx_screen_mask.h"
#include "main/sef.h"
#include "main/string_utf_get_hash.h"

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU) and src/main/jni.h. Every Java native in
 * this TU map receives (thread, arguments[, result]) the same way; no
 * function in this TU reads any of its members, so it stays the incomplete
 * tag those TUs define, restated here verbatim because a TU-local header
 * cannot be included from another TU.
 */
typedef struct JThread JThread;

/*
 * Java_xeno_Effect_call__I's two-word call block: the Effect instance and
 * the int command forwarded to FX_call (lw at 0x002fa6d8).
 */
typedef struct EffectCommandCall {
    unsigned char *object; /* +0x0 */
    int command;           /* +0x4 */
} EffectCommandCall;

/*
 * setScale__FFF/setTransOffset__FFF's four-word call block: the Effect
 * instance and the three float components (lwc1 at 0x002fa7d0/.../
 * 0x002fafb0/...).
 */
typedef struct EffectVectorCall {
    unsigned char *object; /* +0x0 */
    float x;                /* +0x4 */
    float y;                /* +0x8 */
    float z;                /* +0xC */
} EffectVectorCall;

/*
 * getTranslate__/setTranslate__/getRotate__/getForceLoop__/getClip__/
 * clearEffect__'s one-word call block: only the Effect instance.
 */
typedef struct EffectCall {
    unsigned char *object; /* +0x0 */
} EffectCall;

/* Boolean setter call block: the Java object and its one-byte value. */
typedef struct EffectBooleanCall {
    unsigned char *object; /* +0x0 */
    unsigned char value;   /* +0x4 */
} EffectBooleanCall;

/*
 * setCaster__Lxeno_Chr_/setTarget__Lxeno_Chr_'s two-word call block: the
 * Effect instance and the xeno.Chr instance whose native peer becomes this
 * effect's caster or target (lw at 0x002fad60/0x002fadf8).
 */
typedef struct EffectChrCall {
    unsigned char *object; /* +0x0 */
    unsigned char *chr;    /* +0x4 */
} EffectChrCall;

/*
 * setCaster__Lxeno_Unit_/setTarget__Lxeno_Unit_'s two-word call block: the
 * Effect instance and the xeno.Unit instance whose native peer becomes this
 * effect's caster or target (lw at 0x002fae94/0x002faf3c).
 */
typedef struct EffectUnitCall {
    unsigned char *object; /* +0x0 */
    unsigned char *unit;   /* +0x4 */
} EffectUnitCall;

/*
 * The "args" Java field Java_xeno_Effect_call__I reads off the Effect
 * instance: length at +4, native data pointer at +8 (lw at
 * 0x002fa714/0x002fa704). +0 is never read by this TU.
 */
typedef struct EffectCallArgs {
    unsigned int unmodeled_00; /* +0x0 */
    int length;                /* +0x4 */
    void *data;                /* +0x8 */
} EffectCallArgs;

/* Field-name string constants, byte-identical in the original binary:
   effectIdName "id", effectArgsName "args", effectPeerName "peer", effectPositionXName "px",
   effectPositionYName "py", effectPositionZName "pz", effectRotationXName "rx", effectRotationYName "ry",
   effectRotationZName "rz". None has a config/symbols/main.txt name yet. */
extern char effectIdName[];

extern char effectArgsName[];
extern char effectPeerName[];
extern char effectPositionXName[];
extern char effectPositionYName[];
extern char effectPositionZName[];
extern char effectRotationXName[];
extern char effectRotationYName[];
extern char effectRotationZName[];

/* Radian-to-degree conversion constant (value 3.14159274f, i.e. pi); no
   config/symbols/main.txt name yet. */
extern float radianToDegreeScale;

extern JavaField *lookupClassField(void *class_object, void *name, int flags);

#endif /* SRC_MAIN_EFFECT_H */
