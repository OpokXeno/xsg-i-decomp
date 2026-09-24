/* 249_unit.h: TU-local declarations of main 249_unit.
 */
#ifndef SRC_MAIN_UNIT_H
#define SRC_MAIN_UNIT_H

typedef unsigned char u8;
typedef unsigned int u32;

/*
 * JNI_isInstanceOf/loadConstString/lookupClassField are the runtime's
 * class-field lookup primitives, already recovered with this exact
 * signature in src/main/chr.h (main/tu248, JNI_isInstanceOf itself defined
 * in src/main/jni.c). This TU cannot include another TU's TU-local header,
 * so they are declared verbatim again here.
 */
extern int JNI_isInstanceOf(SceneObject object, SceneClass *target_class);
extern SceneString *loadConstString(const char *bytes, int length);
extern JavaField *lookupClassField(void *class_object, void *name, int flags);

/*
 * The native peer record every Java xeno.Unit object's "peer" field points
 * to (looked up with lookupClassField(classJava_xeno_Unit, "peer", 0), the
 * D_004DC1D0 string every function below loads). It is not src/main/chr.h's
 * Actor: the same "peer" lookup here reaches fields that do not match
 * Actor's recovered head, so this TU keeps its own partial view.
 *
 *   +0x00 flags     tested/toggled by setCollision__Z (bit 0x100),
 *                   setVisible__Z (0x4), map_shadow__I (0x20) and
 *                   setClip__I (0x40).
 *   +0x0c elevator_task
 *                   the callback initElevatorFunc__ installs (sw at
 *                   0x0030481c) before tail-calling it.
 *   +0x30 scale_x/y/z
 *                   the three floats setScale__FFF writes (swc1).
 *   +0xa0 serial    this unit's own slot number: getSerial__ returns it
 *                   verbatim, and getState__/getPivot__/setPivot__/
 *                   setAxis__ use it to index the 64-entry unitSequence
 *                   table below.
 *   +0xa1 signal    the byte getSignal__/signal__I read and write
 *                   (lbu/sb 0xa1).
 *   +0xa6 monitor_priority
 *                   the halfword setMonitorPrio__I sets to 0 or 1 (sh).
 *   +0x118 motion_mask
 *                   the halfword mtnSetMask__I writes (sh).
 *   +0x1a0 args     the byte buffer getArgs__II reads from (at a caller
 *                   offset, up to 4 bytes) and setArgs__ILjava_lang_Object_I
 *                   writes to (from its Object argument's data, always at
 *                   the start).
 *   +0x1b8 elevator_field
 *                   the address (this unit's Java object plus the "py"
 *                   field's offset) initElevatorFunc__ stores for
 *                   elevator_task to read back.
 *   +0x1c0 elevator_state
 *                   zeroed by initElevatorFunc__.
 *   +0x240 model    passed by address to MDL_setVisible (still asm), whose
 *                   own layout is unrecovered.
 *   +0x2e4 render_command
 *                   the word renderCommand__I writes (sw).
 */

/*
 * Fields this TU's shadow/sort-offset natives touch, ahead of and behind the
 * "model"/render_command pair above:
 *
 *   +0x234 shadow_x/shadow_y
 *                   the two ints setShadow__II writes (sw 564/568($peer)).
 *   +0x23c shadow_clip_scale
 *                   the float shadow_clip_scale__F writes (swc1 572($peer)).
 *   +0x2ec sort_offset
 *                   the int setSortOffset__F writes (cvt.w.s then swc1
 *                   748($peer): the float argument truncated toward zero).
 */

/*
 * Field this TU's setFilter native touches, between render_command and
 * sort_offset above:
 *
 *   +0x2e8 filter_mode
 *                   the word setFilter__I always resets to 0, then sets to
 *                   1 for Java filter value 2 or 2 for Java filter value 3
 *                   (both of those cases also set *failure_result = 4).
 */
/*
 * Field this TU's shadow-map natives touch, in the last four bytes of the
 * span the "model" comment above bounds from MDL_setVisible's argument:
 *
 *   +0x2e0 shadow_map_count
 *                   the word shadow_map_reset__ clears (sw $0, 736($peer)).
 *                   shadow_map_id__I (still asm, 0x00304b20) reads the same
 *                   word, appends only while it is below 8 and stores the
 *                   incremented count back, so it counts the shadow-map
 *                   entries that native fills in ahead of it; MDL_setVisible
 *                   still receives the address of +0x240, unchanged.
 */
typedef struct UnitPeer {
    u32 flags;                                       /* +0x00 */
    unsigned char unmodeled_04[0x0c - 0x04];
    int (*elevator_task)(void *peer, int field_offset); /* +0x0c */
    unsigned char unmodeled_10[0x30 - 0x10];
    float scale_x;                                   /* +0x30 */
    float scale_y;                                   /* +0x34 */
    float scale_z;                                   /* +0x38 */
    unsigned char unmodeled_3c[0xa0 - 0x3c];
    u8 serial;                                        /* +0xa0 */
    u8 signal;                                        /* +0xa1 */
    unsigned char unmodeled_a2[0xa6 - 0xa2];
    short monitor_priority;                           /* +0xa6 */
    unsigned char unmodeled_a8[0x118 - 0xa8];
    short motion_mask;                                /* +0x118 */
    unsigned char unmodeled_11a[0x1a0 - 0x11a];
    unsigned char args[0x1b8 - 0x1a0];                /* +0x1a0 */
    void *elevator_field;                             /* +0x1b8 */
    unsigned char unmodeled_1bc[0x1c0 - 0x1bc];
    int elevator_state;                               /* +0x1c0 */
    unsigned char unmodeled_1c4[0x234 - 0x1c4];
    int shadow_x;                                     /* +0x234 */
    int shadow_y;                                     /* +0x238 */
    float shadow_clip_scale;                          /* +0x23c */
    unsigned char model[0x2e0 - 0x240];               /* +0x240 */
    int shadow_map_count;                             /* +0x2e0 */
    int render_command;                               /* +0x2e4 */
    int filter_mode;                                  /* +0x2e8 */
    int sort_offset;                                  /* +0x2ec */
} UnitPeer;

/*
 * The per-unit table getState__/getPivot__/setPivot__/setAxis__ index with
 * UnitPeer.serial (lui %hi(unitSequence)/addiu %lo(unitSequence) at
 * 0x0030452c, size 0x9800 in config/symbols/main.txt, exactly 64 entries of
 * 0x260 bytes).
 *
 *   +0x04 state   the word getState__ returns (lw at 0x003044b4).
 *   +0x240 pivot_x/y/z
 *                 the three floats getPivot__/setPivot__ read and write.
 *   +0x250 axis_x/y/z/w
 *                 the four floats setAxis__FFFF writes; the class field
 *                 name pool this TU's lookups read from names the first
 *                 three "rx"/"ry"/"rz".
 */
typedef struct UnitSequenceEntry {
    unsigned char unmodeled_00[0x04];
    int state;                                        /* +0x04 */
    unsigned char unmodeled_08[0x240 - 0x08];
    float pivot_x;                                    /* +0x240 */
    float pivot_y;                                    /* +0x244 */
    float pivot_z;                                    /* +0x248 */
    unsigned char unmodeled_24c[0x250 - 0x24c];
    float axis_x;                                      /* +0x250 */
    float axis_y;                                      /* +0x254 */
    float axis_z;                                      /* +0x258 */
    float axis_w;                                      /* +0x25c */
} UnitSequenceEntry;

extern UnitSequenceEntry unitSequence[64];

/* tyaElevatorTask is still asm elsewhere; initElevatorFunc__ installs it as
 * this unit's elevator callback and tail-calls it once to start. */
extern int tyaElevatorTask(void *peer, int field_offset);

/* MDL_setVisible is still asm elsewhere. */
extern int MDL_setVisible(void *model, int mode, u8 visible);

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

#endif
