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

#endif /* SRC_MAIN_TOOLKIT_H */
