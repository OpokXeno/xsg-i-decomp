/*
 * TU-local declarations of main/tu241 (src/main/toolkit.c).
 */

#ifndef SRC_MAIN_TOOLKIT_H
#define SRC_MAIN_TOOLKIT_H

#include "shared.h"

typedef struct JavaThread {
    unsigned int : 32; /* +0 */
    unsigned int : 32; /* +4 */
    unsigned int : 32; /* +8 */
    unsigned int : 32; /* +12 */
    void *java_object; /* +16: written at 0x2f9480 (sw s1,16(v0)) */
} JavaThread;

typedef struct NativeUnitPeer {
    unsigned int : 32; /* +0 */
    unsigned int : 32; /* +4 */
    unsigned int : 32; /* +8 */
    unsigned int : 32; /* +12 */
    float position_x;  /* +16: written at 0x2f94c4 (swc1 $f0,16(s0)) */
    float position_y;  /* +20: written at 0x2f94f4 (swc1 $f0,20(s0)) */
    float position_z;  /* +24: written at 0x2f9524 (swc1 $f0,24(s0)) */
    unsigned int : 32; /* +28 */
    float rotation_x;  /* +32: written at 0x2f9554 (swc1 $f0,32(s0)) */
    float rotation_y;  /* +36: written at 0x2f9584 (swc1 $f0,36(s0)) */
    float rotation_z;  /* +40: written at 0x2f95b4 (swc1 $f0,40(s0)) */
    /* +44..+207: no evidenced access by this function; offsets preserved
       as anonymous words, no meanings claimed. */
    unsigned int : 32; /* +44 */
    unsigned int : 32; /* +48 */
    unsigned int : 32; /* +52 */
    unsigned int : 32; /* +56 */
    unsigned int : 32; /* +60 */
    unsigned int : 32; /* +64 */
    unsigned int : 32; /* +68 */
    unsigned int : 32; /* +72 */
    unsigned int : 32; /* +76 */
    unsigned int : 32; /* +80 */
    unsigned int : 32; /* +84 */
    unsigned int : 32; /* +88 */
    unsigned int : 32; /* +92 */
    unsigned int : 32; /* +96 */
    unsigned int : 32; /* +100 */
    unsigned int : 32; /* +104 */
    unsigned int : 32; /* +108 */
    unsigned int : 32; /* +112 */
    unsigned int : 32; /* +116 */
    unsigned int : 32; /* +120 */
    unsigned int : 32; /* +124 */
    unsigned int : 32; /* +128 */
    unsigned int : 32; /* +132 */
    unsigned int : 32; /* +136 */
    unsigned int : 32; /* +140 */
    unsigned int : 32; /* +144 */
    unsigned int : 32; /* +148 */
    unsigned int : 32; /* +152 */
    unsigned int : 32; /* +156 */
    unsigned int : 32; /* +160 */
    unsigned int : 32; /* +164 */
    unsigned int : 32; /* +168 */
    unsigned int : 32; /* +172 */
    unsigned int : 32; /* +176 */
    unsigned int : 32; /* +180 */
    unsigned int : 32; /* +184 */
    unsigned int : 32; /* +188 */
    unsigned int : 32; /* +192 */
    unsigned int : 32; /* +196 */
    unsigned int : 32; /* +200 */
    unsigned int : 32; /* +204 */
    void *java_object; /* +208: written at 0x2f9488 (sw s1,208(s0)) */
} NativeUnitPeer;

/* this domain's accepted files spell it differently; the canonical form is repeated here */
extern SceneString *loadConstString(const char *bytes, int length);

/* canon: config/header-canon.json chose src/math/main/002ff538-chr-scl/private.h over 1 other accepted spelling */
extern JavaField *lookupClassField(void *class_object, void *name, int flags);

/*
 * getPeer_Uwamono (main VA 0x002f93f8, 588 bytes, GLOBAL binding).
 * Native peer helper: reads the unit id Java field, creates the Uwamono
 * peer, attaches a JNI thread ward, copies position/rotation Java fields
 * into the peer, normalizes rotations from degrees to radians, and stores
 * the peer back into the Java peer field.
 */
void *getPeer_Uwamono(void *java_unit);

/*
 * pi: the float at .lit4 0x004d83b0 (bytes db 0f 49 40 = 3.1415927...),
 * loaded via gp-relative lwc1 at 0x2f95e8 (lwc1 $f4,-30656(gp)).
 * Used as rotation / 180.0f * pi (degree-to-radian scaling).
 */
extern const float pi;

extern const char unit_field_id[];

extern const char unit_field_algorithm[];

extern const char unit_field_px[];

extern const char unit_field_py[];

extern const char unit_field_pz[];

extern const char unit_field_rx[];

extern const char unit_field_ry[];

extern const char unit_field_rz[];

extern const char unit_field_peer[];

extern NativeUnitPeer *Unit_CreateUwamono(int identifier, int unit_id);

extern JavaThread *JNI_createThread(int kind, int stack_words,
                                    int frame_words);

/*
 * The script VM's per-thread context, already recovered as `JThread` in
 * src/main/jni.h/src/main/chr.h. No function in this TU reads any of its
 * members, so it stays the incomplete tag those TUs define, restated here
 * verbatim because a TU-local header cannot be included from another TU.
 */
typedef struct JThread JThread;

/*
 * classJava_xeno_Unit is already declared in shared.h; classJava_xeno_Chr
 * is not, since its layout-sensitive uses live in other TUs' own headers
 * (src/main/chr.h, src/main/scene_1.h, src/main/layout.h). This TU only
 * ever passes it to JNI_isInstanceOf, so it is restated verbatim with
 * those TUs' spelling.
 */
extern void *classJava_xeno_Chr;

/* Canonical spelling (config/header-canon.json) for this primitive. */
extern int JNI_isInstanceOf(SceneObject object, SceneClass *target_class);

/*
 * The engine's actor record a xeno.Chr's "peer" field points to, already
 * completed independently by src/main/chr.h/act_2.h and others. This TU
 * only forwards the pointer to ACT_loadResource/ACT_loadMotion without
 * reading any member, so it stays the incomplete tag those TUs define.
 */
typedef struct Actor Actor;

/*
 * The native peer a Java xeno.Unit's "peer" field points to, as far as
 * Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_II reads and
 * writes it: the resource handle RES_loadFile returns, stored back at
 * +0xDC (main VA 0x002f9fec, sw v0,220(s0)). src/main/unit.h documents the
 * same class's peer under its own independent partial view (UnitPeer),
 * whose unmodeled span between +0xa8 and +0x118 covers this offset without
 * contradiction.
 */
typedef struct ToolkitUnitPeer {
    unsigned char unmodeled_000[0xDC];
    int resource; /* +0xDC */
} ToolkitUnitPeer;

/*
 * Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_I's call block:
 * the Java Object argument and the int id (lw at 0x2f9c20/0x2f9c34).
 */
typedef struct ToolkitResourceCall {
    u8 *object; /* +0x0 */
    int id;     /* +0x4 */
} ToolkitResourceCall;

/*
 * Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_II's call block:
 * the Java Object argument and its two int arguments (lw at
 * 0x2f9ed0/0x2f9ee4/0x2f9edc).
 */
typedef struct ToolkitResourceIndexCall {
    u8 *object;      /* +0x0 */
    int id;          /* +0x4 */
    int resource_id; /* +0x8 */
} ToolkitResourceIndexCall;

/*
 * The storage record every java.lang.String carries: src/main/scene_1.h
 * documents the same layout as SceneStringStorage (length at +4, byte
 * pointer at +8). This TU only reads the byte pointer (lw at 0x2f9d74).
 */
typedef struct ToolkitStringStorage {
    unsigned char unmodeled_00[8];
    const char *bytes; /* +0x8 */
} ToolkitStringStorage;

/*
 * A java.lang.String instance, as far as this TU reads one: the object
 * header, then the storage pointer (lw at 0x2f9d6c). Same layout
 * src/main/scene_1.h's SceneString documents.
 */
typedef struct ToolkitStringArg {
    unsigned char unmodeled_00[4];
    ToolkitStringStorage *storage; /* +0x4 */
} ToolkitStringArg;

/*
 * Java_xeno_util_Toolkit_loadResource__Ljava_lang_String_'s call block:
 * the Java String argument (lw at 0x2f9d5c).
 */
typedef struct ToolkitStringCall {
    ToolkitStringArg *name; /* +0x0 */
} ToolkitStringCall;

/* Still INCLUDE_ASM in src/main/act_2.c (main/tu255); local hypothesis
 * prototypes until that TU's own header exposes one. */
extern void ACT_loadResource(Actor *peer, int id);
extern int ACT_loadMotion(Actor *peer, int id, int category);

/* Still INCLUDE_ASM in src/main/map_2.c (main/tu269); local hypothesis
 * prototype until that TU's own header exposes one. */
extern int MAP_loadUnitResource(ToolkitUnitPeer *peer, int id);

/* Defined in src/main/script.c, which does not yet expose its own header;
 * reads the resourceID global. */
extern int XTK_getResourceID(void);

/* Still INCLUDE_ASM in src/main/script.c; local hypothesis prototype
 * until that TU's own header exposes one. */
extern int XTK_findFile(const char *path);

/* Still INCLUDE_ASM in src/main/res_get_path.c; same spelling that TU
 * already hypothesizes for it (command, callback, resource_id, flags). */
extern int RES_loadFile(int command, int callback, int resource_id,
                        int flags);

/*
 * Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_I
 * (main VA 0x002f9c00, 340 bytes, GLOBAL binding).
 * Loads the id-selected resource into the peer of a xeno.Chr or xeno.Unit
 * object: a Chr peer gets ACT_loadResource/ACT_loadMotion (tail call,
 * using the toolkit's current resource id as the motion category); a Unit
 * peer whose "algorithm" field has none of bits 0x100-0x800 set gets
 * MAP_loadUnitResource (tail call); any other case returns that masked
 * algorithm value (0 for an object that is neither).
 */
int Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_I(
    JThread *thread, ToolkitResourceCall *arguments);

/*
 * Java_xeno_util_Toolkit_loadResource__Ljava_lang_String_
 * (main VA 0x002f9d58, 52 bytes, GLOBAL binding).
 * Reads the byte pointer out of the Java String argument's storage record
 * and writes XTK_findFile's result through the output pointer.
 */
void Java_xeno_util_Toolkit_loadResource__Ljava_lang_String_(
    JThread *thread, ToolkitStringCall *arguments, int *result);

/*
 * Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_II
 * (main VA 0x002f9eb0, 348 bytes, GLOBAL binding).
 * Same object-type dispatch as ...Object_I's, with a second int argument:
 * a Chr peer gets ACT_loadResource/ACT_loadMotion (tail call, with the
 * fixed motion category 3; the toolkit's current resource id is still
 * queried but its result is unused, matching the compiled call sequence).
 * A Unit peer whose "algorithm" field has none of bits 0x100-0x800 set
 * gets MAP_loadUnitResource, then RES_loadFile(-1, 2, resource_id +
 * 0x03000000, 0) with its result stored into the peer's +0xDC slot.
 */
int Java_xeno_util_Toolkit_loadResource__Ljava_lang_Object_II(
    JThread *thread, ToolkitResourceIndexCall *arguments);

#endif /* SRC_MAIN_TOOLKIT_H */
