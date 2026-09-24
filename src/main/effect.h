/*
 * TU-local declarations of main/tu245 (src/main/effect.c).
 */

#ifndef SRC_MAIN_EFFECT_H
#define SRC_MAIN_EFFECT_H

#include "shared.h"

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
 * The Unit class's native peer, reached through the same "peer" Java field
 * on classJava_xeno_Unit. src/main/unit.h recovers the whole record as
 * `UnitPeer` for main/tu249 and leaves this offset inside a gap; a TU-local
 * header cannot be included from another TU, so only the one member this TU
 * writes is modelled here, under its own tag.
 */
typedef struct EffectUnitPeer EffectUnitPeer;

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

/*
 * The Effect class's native peer, reached through its "peer" Java field.
 * Touched members only, from this TU's functions:
 *   +0x80/0x84/0x88 translate x/y/z (getTranslate__/setTranslate__/
 *                   setTransOffset__FFF)
 *   +0x90/0x94/0x98 rotate x/y/z, radians (getRotate__)
 *   +0xA0/0xA4/0xA8 scale x/y/z (setScale__FFF)
 *   +0x6BC          caster_chr (setCaster__Lxeno_Chr_)
 *   +0x6C0          target_chr (setTarget__Lxeno_Chr_)
 *   +0x6C4          caster_unit (setCaster__Lxeno_Unit_)
 *   +0x6C8          target_unit (setTarget__Lxeno_Unit_)
 *   +0xA8C          flags byte: bit 0x10 clip, bit 0x20 force loop
 *                   (getForceLoop__/getClip__)
 * Every other offset is an unmodeled gap.
 */
typedef struct NativeEffectPeer {
    unsigned char unmodeled_000[0x80];
    float translate[3]; /* +0x80: x, y, z */
    unsigned char unmodeled_08c[0x90 - 0x8c];
    float rotate[3]; /* +0x90: x, y, z, radians */
    unsigned char unmodeled_09c[0xa0 - 0x9c];
    float scale_x;
    float scale_y;
    float scale_z;
    unsigned char unmodeled_0ac[0x6bc - 0xac];
    void *caster_chr;            /* +0x6BC: the xeno.Chr instance's peer */
    void *target_chr;            /* +0x6C0: the xeno.Chr instance's peer */
    EffectUnitPeer *caster_unit; /* +0x6C4 */
    EffectUnitPeer *target_unit; /* +0x6C8 */
    unsigned char unmodeled_6cc[0xa8c - 0x6cc];
    unsigned char flags;
} NativeEffectPeer;

/*
 * Touched member only: setCaster__Lxeno_Unit_ writes the effect peer back
 * into the unit peer it takes as caster (sw at 0x002faed0), so the two
 * records point at each other. setTarget__Lxeno_Unit_ writes no back
 * pointer.
 */
struct EffectUnitPeer {
    unsigned char unmodeled_000[0xe8];
    NativeEffectPeer *caster_effect; /* +0xE8 */
};

extern void *classJava_xeno_Effect;

extern void *classJava_xeno_Chr;

/* Field-name string constants, byte-identical in the original binary:
   D_004DC130 "id", D_004DC138 "args", D_004DC140 "peer", D_004DC148 "px",
   D_004DC150 "py", D_004DC158 "pz", D_004DC160 "rx", D_004DC168 "ry",
   D_004DC170 "rz". None has a config/symbols/main.txt name yet. */
extern const char D_004DC130[];
extern const char D_004DC138[];
extern const char D_004DC140[];
extern const char D_004DC148[];
extern const char D_004DC150[];
extern const char D_004DC158[];
extern const char D_004DC160[];
extern const char D_004DC168[];
extern const char D_004DC170[];

/* Radian-to-degree conversion constant (value 3.14159274f, i.e. pi); no
   config/symbols/main.txt name yet. */
extern float D_004D83BC;

/* canon: config/header-canon.json, as src/math/main/002ff538-chr-scl/
   private.h spells them. */
extern SceneString *loadConstString(const char *bytes, int length);
extern JavaField *lookupClassField(void *class_object, void *name, int flags);

extern void FX_call(int command, int id, const void *data, int length);
extern void sefClearEffectCf(void *effect_peer);

#endif /* SRC_MAIN_EFFECT_H */
