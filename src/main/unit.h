/* 249_unit.h: TU-local declarations of main 249_unit.
 */
#ifndef SRC_MAIN_UNIT_H
#define SRC_MAIN_UNIT_H

#include "main/unit.h"
#include "main/exm_2.h"
#include "main/jni.h"
#include "main/mdl.h"
#include "main/string_utf_get_hash.h"
#include "main/tya_elevator_task.h"

typedef unsigned char u8;

typedef unsigned int u32;

/*
 * The Java xeno.Unit native peer record is MapUnitRecord (include/main/exm_2.h);
 * the field evidence that used to sit here moved to the struct comment there.
 */

/* Call block shapes: `object` is always the Java xeno.Unit instance the
 * native was invoked on, at +0x00 of every block below. */
typedef struct UnitObjectCall {
    u8 *object;
} UnitObjectCall;

typedef struct UnitSignalCall {
    u8 *object;
    u8 value;
} UnitSignalCall;

typedef struct UnitBoolCall {
    u8 *object;
    u8 flag;
} UnitBoolCall;

typedef struct UnitIntCall {
    u8 *object;
    int value;
} UnitIntCall;

typedef struct UnitVisibleCall {
    u8 *object;
    int mode;
    u8 visible;
} UnitVisibleCall;

typedef struct UnitArgsGetCall {
    u8 *object;
    int offset;
    int size;
} UnitArgsGetCall;

/*
 * The Object argument of setArgs__ILjava_lang_Object_I: the bytes it
 * supplies begin at +0x4 (the call reads nothing before them).
 */
typedef struct UnitArgsSource {
    unsigned char unmodeled_00[4];
    u8 data[1]; /* +0x4 */
} UnitArgsSource;

/*
 * setArgs__ILjava_lang_Object_I's call block. The native reads the object,
 * the Object argument at +0x8 and the byte count at +0xc; the word at +0x4
 * it never reads.
 */
typedef struct UnitArgsSetCall {
    u8 *object;
    unsigned char unmodeled_04[4];
    UnitArgsSource *source; /* +0x8 */
    int size;               /* +0xc */
} UnitArgsSetCall;

/*
 * setArgs__III's call block: object +0x0, offset +0x4, the raw value word to
 * copy from +0x8 and the byte count +0xc (lw v1,8(v0) / lw s0,0xc(v0) / lw
 * s1,0(v0) / lw s2,4(v0) at 0x00303f2c..0x00303f40).
 */
typedef struct UnitArgsWordCall {
    u8 *object;
    int offset;
    int value;
    int size;
} UnitArgsWordCall;

typedef struct UnitVector3Call {
    u8 *object;
    float x;
    float y;
    float z;
} UnitVector3Call;

typedef struct UnitAxisCall {
    u8 *object;
    float x;
    float y;
    float z;
    float w;
} UnitAxisCall;

typedef struct UnitPivotVector {
    unsigned char unmodeled_00[4];
    float z; /* +0x4 */
    float y; /* +0x8 */
    float x; /* +0xc */
} UnitPivotVector;

typedef struct UnitPivotOutCall {
    u8 *object;
    UnitPivotVector *vector;
} UnitPivotOutCall;

/*
 * The xeno.util.Vector4f getAxis__Lxeno_util_Vector4f_ writes: the same
 * object-header/z/y/x layout as UnitPivotVector above (both natives target
 * the same Java class), plus a fourth field right after x that only this
 * native writes (always the constant 1.0f).
 */
typedef struct UnitAxisVector {
    unsigned char unmodeled_00[4];
    float z; /* +0x4 */
    float y; /* +0x8 */
    float x; /* +0xc */
    float w; /* +0x10 */
} UnitAxisVector;

typedef struct UnitAxisOutCall {
    u8 *object;
    UnitAxisVector *vector;
} UnitAxisOutCall;

/*
 * The interpreter value slot a native's return value goes into, the third
 * argument of every native here that returns something: getSerial__/
 * getState__/getArgs__II above store a plain result word in it, getScale__
 * the reference to the Vector4f object it hands back (sw v1,0(s1) at
 * 0x003041e0), so the slot holds either.
 */
typedef union UnitResultValue {
    u32 word;
    UnitScaleVector *object;
} UnitResultValue;

/*
 * suspend__I/resume__I are static natives (no receiver): the call block is
 * only the group id, at +0x00.
 */
typedef struct UnitGroupCall {
    int group;
} UnitGroupCall;

typedef struct UnitShadowCall {
    u8 *object;
    int x;
    int y;
} UnitShadowCall;

typedef struct UnitFloatCall {
    u8 *object;
    float value;
} UnitFloatCall;

typedef struct UnitArgsRow {
    int word[4];
} UnitArgsRow;

/*
 * setArgs__IIIII stores four words per row at 16-byte strides from +0x1a0
 * of the unit's native peer.
 */
typedef struct UnitArgsPeer {
    unsigned char unmodeled_00[0x1a0];
    UnitArgsRow rows[1]; /* +0x1a0 */
} UnitArgsPeer;

typedef struct UnitArgsRowCall {
    u8 *object;
    int row;
    int word[4];
} UnitArgsRowCall;

typedef struct UnitFloatArray {
    unsigned char unmodeled_00[4];
    unsigned int length; /* +0x4 */
    float *data;         /* +0x8 */
} UnitFloatArray;

typedef struct UnitFilterParamCall {
    u8 *object;
    UnitFloatArray *values;
} UnitFilterParamCall;

extern UnitSequenceEntry unitSequence[64];
extern UnitScaleVector scale_0_007C0A58;

extern JavaField *lookupClassField(void *class_object, void *name, int flags);

#endif
