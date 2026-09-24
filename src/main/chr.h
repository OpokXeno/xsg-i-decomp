/* 248_chr.h: TU-local declarations of main 248_chr.
 */
#ifndef SRC_MAIN_CHR_H
#define SRC_MAIN_CHR_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

/*
 * The JVM's class-field handle, as `lookupClassField` returns it. Only the
 * object field offset (+0x10) is observed: `lw v1,0x10(v0)` at 0x002ff5b0,
 * 0x002ff5d8 and the same pair in CHR_sclY/CHR_sclZ, CHR_rotX (0x002fd6ac)
 * and JTHREAD_defaultChr (0x00305548). Verbatim the canonical spelling of
 * include/shared.h.
 */
typedef struct JavaField {
    unsigned int : 32; /* +0 */
    unsigned int : 32; /* +4 */
    unsigned int : 32; /* +8 */
    unsigned int : 32; /* +12 */
    int offset;        /* +16: read at 0x2f9438/0x2f94ac/... (lw v1,16(v0)) */
} JavaField;

/* Call block: object +0x0, first +0x4, second +0x8, wait byte +0xC. */
typedef union ChrScaleArgument {
    int integer;
    float floating;
} ChrScaleArgument;
typedef struct ChrScaleCall {
    u8 *object;
    ChrScaleArgument first;
    ChrScaleArgument second;
    u8 wait;
} ChrScaleCall;

/*
 * Call blocks of Java_xeno_Chr_move__* and Java_xeno_Chr_rot{X,Y,Z}__*: their
 * trampolines forward the pointer unread, and CHR_moveXZ/CHR_rotX/CHR_rotY/
 * CHR_rotZ (still asm here) read it with a different field type per mode
 * (CHR_moveXZ: object +0x0 in every mode; modes 0-1 then +0x4/+0x8/+0xC and
 * the wait byte +0x10, modes 2-3 +0x4/+0x8 and the wait byte +0xC;
 * 0x002fcf3c..0x002fd178), so it stays an incomplete type until those callees
 * are recovered.
 */
typedef struct ChrMoveCall ChrMoveCall;
typedef struct ChrRotCall ChrRotCall;

/*
 * Call block of Java_xeno_Chr_mtn__*: their trampolines forward the pointer
 * unread to CHR_motion (still asm here, 0x002fe5f8..0x002fe804), so it stays
 * an incomplete type until that callee is recovered.
 */
typedef struct ChrMotionCall ChrMotionCall;

/* Verbatim the engine's four-float vector record of include/shared.h and of
 * src/main/near_dir.h, which defines the actor below with it. */
typedef struct Vector4 {
    float x;
    float y;
    float z;
    float w;
} Vector4;

/*
 * UnduDataGetHeader's return type, defined by src/math/main/review09-002f6c50
 * and declared with it by src/main/layout.h and src/main/call_java_method.h
 * (canon: config/header-canon.json). This TU only stores the pointer
 * Java_xeno_Chr_setID__I receives (Actor.data_header below) and never reads
 * its components, so it stays an opaque forward declaration here rather than
 * a second definition of a type another TU owns.
 */
typedef struct LayoutHeader LayoutHeader;

/*
 * The engine's actor record, recovered head, defined here exactly as
 * src/main/near_dir.h (main/tu257) and src/main/set_motion.h (main/tu193)
 * define it; the full field-by-field evidence is in near_dir.h's copy and in
 * .work/analysis/layout-actor-family.md.
 *
 * This TU's `peer` is that record and not a separate object: CHR_sclX reads
 * the Java object's `peer` field, ORs bit 1 into +0x00 of what it finds
 * (0x002ff6b4) and indexes actSequence with its +0x80 byte (0x002ff5f8) --
 * the same word ACT_info prints as "FLAGS %08x" and the same byte ACT_create
 * writes as the actor's own slot number (0x00305de0). The three scale floats
 * CHR_sclX/Y/Z read at +0x60/+0x64/+0x68 are the "SCL" column of that printer
 * (0x00306308..0x00306338), which ACT_init and ACT_create set to 1.0f.
 *
 * The type stops at +0x70 because that is where the evidence stops. The slot
 * number at +0x80 is past it and keeps the byte view below.
 */
/*
 * This TU's own Chr natives touch further Actor fields past the +0x70 head
 * above; the byte ranges the evidence does not reach stay unmodeled_XX gaps.
 *
 *   +0x90 shadow_kind        Java_xeno_Chr_setShadow__II's first byte
 *                            argument (sb v1,0x90(a1) at 0x002ffec4).
 *   +0x91 shadow_size        its second byte argument (sb v0,0x91(a1) at
 *                            0x002ffed0).
 *   +0x4d0 status_flags      Java_xeno_Chr_setID__I ORs in bit 0x1000 after
 *                            storing data_header (lhu/sh 0x4d0 at
 *                            0x300088/0x300094).
 *   +0x4e0 data_header       the UnduDataGetHeader result Java_xeno_Chr_setID__I
 *                            stores here (sw v0,0x4e0(s0) at 0x30008c).
 *   +0x62c look_eye_speed    Java_xeno_Chr_look_eye_speed__F's float argument
 *                            (swc1 f0,0x62c(v0) at 0x300e6c).
 *   +0x66c look_speed        Java_xeno_Chr_look_speed__F's float argument
 *                            (swc1 f0,0x66c(v0) at 0x300e0c).
 *   +0x670 shadow_clip_scale Java_xeno_Chr_shadow_clip_scale__F's float
 *                            argument (swc1 f0,0x670(v0) at 0x300f2c).
 *   +0x674 look_mode         Java_xeno_Chr_look_camera__ always sets it to 2
 *                            (sh a0,0x674(v0) at 0x300a80).
 *   +0x678 look_eye_control  Java_xeno_Chr_look_eye_control__I's int argument
 *                            (sw a0,0x678(v0) at 0x300dac).
 *   +0x690 shadow_map        Java_xeno_Chr_shadow_map_reset__ always clears
 *                            it (sw zero,0x690(v0) at 0x301004).
 *   +0x754 hair_stop_a       Java_xeno_Chr_hairStop__II's first int argument
 *                            (sw a1,0x754(a0) at 0x301060).
 *   +0x758 hair_stop_b       its second int argument (sw v0,0x758(a0) at
 *                            0x30106c).
 *   +0x9a4 render_command    Java_xeno_Chr_renderCommand__I's int argument
 *                            (sw a0,0x9a4(v0) at 0x300ecc).
 *   +0x9a8 pixel_alpha       Java_xeno_Chr_pixelAlpha__I's int argument
 *                            (sw a0,0x9a8(v0) at 0x3010cc).
 *   +0x9ac pixel_alpha_parts Java_xeno_Chr_pixelAlphaPartsReset__ always
 *                            clears it (sw zero,0x9ac(v0) at 0x3011a4).
 *   +0x9e0 sort_offset       Java_xeno_Chr_setSortOffset__F's float argument
 *                            (swc1 f0,0x9e0(v0) at 0x30053c).
 *   +0x9f4 talk_message      Java_xeno_Chr_talkto__Ljava_lang_String_'s
 *                            message id (sw v0,0x9f4(v1) at 0x3007c4).
 *   +0x9f8 touch_message     Java_xeno_Chr_touchto__Ljava_lang_String_'s
 *                            message id (sw v0,0x9f8(v1) at 0x30082c).
 */
/*
 * Five further fields, evidenced by other Chr natives, all still gaps in
 * the byte ranges listed above:
 *
 *   +0x81 signal        Java_xeno_Chr_signal__I stores its argument's low
 *                        byte here (sb v1,0x81(v0) at 0x002fe8d8) and
 *                        Java_xeno_Chr_getSignal__ reads it back unsigned
 *                        (lbu a0,0x81(v1) at 0x002fe95c).
 *   +0xc0 args           the byte buffer Java_xeno_Chr_setArgs__III writes
 *                        1-4 bytes into at a caller-supplied offset
 *                        (jal copyArgs at 0x002ff060) and
 *                        Java_xeno_Chr_setArgs__ILjava_lang_Object_I writes
 *                        two full words into, also at that offset
 *                        (sw a1,0xc0(v0) / sw a0,0xc4(v0) at
 *                        0x002ff18c/0x002ff190).
 *   +0x67c look_target   Java_xeno_Chr_look_char__Ljava_lang_Object_ stores
 *                        the other character's own peer pointer here
 *                        (sw a0,0x67c(v0) at 0x00300af4);
 *                        Java_xeno_Chr_look_unit__Ljava_lang_Object_ stores
 *                        a xeno.Unit's own peer word the same way
 *                        (sw v0,0x67c(s0) at 0x00300b88). The two callers
 *                        disagree on what it points to, so it stays void*.
 *   +0x9a0 render_flags  a second flags word: Java_xeno_Chr_setClip__I sets
 *                        or clears bit 0x200 (ori/andi at
 *                        0x30043c/0x300444), Java_xeno_Chr_setSymmetryY__I
 *                        bit 0x10 (0x3004bc/0x3004c4) and
 *                        Java_xeno_Chr_ignoreShape__I bit 0x400
 *                        (0x3013d4/0x3013dc).
 *   +0x9c0 filter_param  the four floats Java_xeno_Chr_setFilterParam__aF
 *                        copies from its argument's value object, in the
 *                        reordered sequence its own source's +0x0/+0xc/
 *                        +0x8/+0x4 give them (swc1 at
 *                        0x3003c0..0x3003dc).
 *
 *   +0x4dc translate_y   Java_xeno_Chr_setTranslate__ mirrors the position.y
 *                        it just wrote here too (swc1 $f1,0x4dc(s2) at
 *                        0x002feb98); no other evidenced reader in this TU.
 */
typedef struct Actor {
    u32 flags;
    void (*update)(struct Actor *actor);
    void (*draw)(struct Actor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    unsigned char unmodeled_70[0x81 - 0x70];
    u8 signal;                                   /* +0x81 */
    unsigned char unmodeled_82[0x90 - 0x82];
    unsigned char shadow_kind;                  /* +0x90 */
    unsigned char shadow_size;                  /* +0x91 */
    unsigned char unmodeled_92[0xc0 - 0x92];
    unsigned char args[0x4d0 - 0xc0];            /* +0xc0 */
    unsigned short status_flags;                /* +0x4d0 */
    unsigned char unmodeled_4d2[0x4dc - 0x4d2];
    float translate_y;                          /* +0x4dc */
    LayoutHeader *data_header;                  /* +0x4e0 */
    unsigned char unmodeled_4e4[0x62c - 0x4e4];
    float look_eye_speed;                       /* +0x62c */
    unsigned char unmodeled_630[0x66c - 0x630];
    float look_speed;                           /* +0x66c */
    float shadow_clip_scale;                    /* +0x670 */
    short look_mode;                            /* +0x674 */
    unsigned char unmodeled_676[0x678 - 0x676];
    int look_eye_control;                       /* +0x678 */
    void *look_target;                          /* +0x67c */
    unsigned char unmodeled_680[0x690 - 0x680];
    int shadow_map;                             /* +0x690 */
    unsigned char unmodeled_694[0x754 - 0x694];
    int hair_stop_a;                            /* +0x754 */
    int hair_stop_b;                            /* +0x758 */
    unsigned char unmodeled_75c[0x9a0 - 0x75c];
    int render_flags;                           /* +0x9a0 */
    int render_command;                         /* +0x9a4 */
    int pixel_alpha;                            /* +0x9a8 */
    int pixel_alpha_parts;                      /* +0x9ac */
    unsigned char unmodeled_9b0[0x9c0 - 0x9b0];
    float filter_param[4];                      /* +0x9c0 */
    unsigned char unmodeled_9d0[0x9e0 - 0x9d0];
    float sort_offset;                          /* +0x9e0 */
    unsigned char unmodeled_9e4[0x9f4 - 0x9e4];
    int talk_message;                           /* +0x9f4 */
    int touch_message;                          /* +0x9f8 */
} Actor;

/* The actor's own slot in the 64-entry `actor` array at main 0x0043c1e0,
 * which ACT_create writes there (sb a2,0x80(s0) at 0x00305de0) and ACT_info
 * prints as "ACT[%02x]". It selects this actor's actSequence entry. It lies
 * past the recovered head of Actor, so it keeps the byte view, exactly as
 * src/main/near_dir.c and src/main/set_motion.c keep it. */
#define ACTOR_SLOT_NUMBER_OFFSET 0x80

/*
 * The VM thread. This is the first argument of every `Java_xeno_*` native
 * method and the record `JNI_createThread` (0x002f0650) allocates, the
 * `jthreadTop`/`jthreadCurrent` list walks and `JTHREAD_waitFor` (0x00305730)
 * blocks. The earlier TU-local name for it here was `ChrContext`.
 *
 * The head below is complete from +0x00 to +0x40 except for three words and
 * one halfword that no instruction in any of the six EE images touches; those
 * are unnamed bit-fields, in the spelling JavaField above already uses, and
 * the search that concluded it is recorded in the job report. Everything past
 * +0x44 is the frame array `frames` points into and is not modelled.
 *
 *   +0x00             no instruction in the six images reads or writes it.
 *   +0x04 previous    JNI_createThread links the new thread behind
 *   +0x08 next        jthreadCurrent (sw v1,0x4(a1) / sw a1,0x8(v1) at
 *                     0x002f06e4/0x002f06e8, and sw zero,0x4(a1) for the
 *                     first one); JTHREAD_get (0x00305958), JTHREAD_cntl
 *                     (0x003058c8) and JTHREAD_info walk `next` from
 *                     jthreadTop.
 *   +0x0c kind        JNI_createThread's first argument (sb s2,0xC(a1) at
 *                     0x002f0698). SCRIPT_init and SCRIPT_reset pass 0 for
 *                     the script VM (0x00261478, 0x002612e4), getPeer_Chr
 *                     passes 1 for a character's own thread (0x002f96e0),
 *                     getPeer_Stage and sceneObserver pass 2 (0x002f9b94,
 *                     0x00262900) and Java_xeno_vm_Thread_create passes 4
 *                     (0x002f6554). JTHREAD_cntl runs the reset hook only for
 *                     threads whose kind is nonzero (0x003058fc).
 *   +0x0d wait_kind   JTHREAD_waitFor switches on `wait_kind & 0xf` over a
 *                     seven-entry table (jtbl_004D15B0, 0x00305734..0x00305758):
 *                     1 counts frames in wait_target/wait_parameter, 2 waits
 *                     for another thread's flag 0x8, 3 for a menu or window
 *                     object's +0x14 to reach 2, 4 and 6 for the actor at
 *                     wait_target to clear Actor.flags bit 1, 5 for its motion
 *                     byte +0x81 to equal wait_parameter and 7 for a bit of
 *                     GameLoopState. CHR_sclX below writes 4.
 *   +0x0e             no instruction in the six images reads or writes it.
 *   +0x10 object      the Java object the thread runs for: JTHREAD_get
 *                     compares it with its argument (lw v1,0x10(a1) at
 *                     0x00305964) and JTHREAD_defaultChr looks its `peer`
 *                     field up on it (0x00305534).
 *   +0x14 reset       the per-thread hook JTHREAD_cntl calls with the thread
 *                     (lw v0,0x14(s0); jalr v0 at 0x00305910/0x0030591c).
 *   +0x18 method      the Java method JTHREAD_default and JTHREAD_defaultChr
 *                     hand to JNI_callMethod (0x003055f4/0x00305590).
 *   +0x1c, +0x20      no instruction in the six images reads or writes them.
 *   +0x24 flags       the thread's state word. JTHREAD_waitFor clears 0x4 and
 *                     sets 0x2 when a wait is satisfied (0x00305838), and for
 *                     the two actor waits clears 0x1 as well (0x0030582c);
 *                     JTHREAD_cntl runs only threads carrying 0x10
 *                     (0x003058e0); JTHREAD_default clears 0x10 unless 0x8 is
 *                     set (0x00305610); the interpreter takes the resume path
 *                     on 0x2 (virtualMachine 0x002f1fa0). JNI_createThread and
 *                     JNI_initThread zero it.
 *   +0x28 stack       the operand stack: JNI_createThread points it past the
 *                     frame array (0x002f06b4) and the interpreter indexes it
 *                     by stack_offset words (virtualMachine 0x002f1ff0).
 *   +0x2c frames      the frame array, 24 bytes per frame, indexed by
 *                     frame_depth (virtualMachine 0x002f1fc4..0x002f1fe8).
 *   +0x30 wait_target what the thread waits on, per wait_kind: another thread
 *                     (System_waitFor 0x002f64ac), a menu or window object, or
 *                     the actor whose sequence must finish -- which is what
 *                     CHR_sclX writes (sw s2,0x30(s0) at 0x002ff6f4).
 *   +0x34 wait_parameter
 *                     the wait's second operand: the frame count of a sleep
 *                     (JTHREAD_waitFor 0x00305764) or the motion number of a
 *                     motion wait (0x00305808); Java_xeno_vm_System_sleep and
 *                     System_waitFor write it.
 *   +0x38 stack_offset
 *   +0x3a stack_limit the interpreter adds the method's local and stack counts
 *                     to stack_offset on entry and calls JNI_threadException
 *                     when the sum reaches stack_limit (virtualMachine
 *                     0x002f1fb4..0x002f201c); JNI_createThread sizes the
 *                     allocation from its third argument and stores it in
 *                     stack_limit (0x002f06b0).
 *   +0x3c resume_frames
 *                     how many frames a woken thread still has to re-enter:
 *                     the interpreter's resume path takes frame
 *                     `frame_depth - resume_frames` and counts it down
 *                     (virtualMachine 0x002f20a4..0x002f20bc). A blocking
 *                     native method sets it to the current frame_depth, which
 *                     is what CHR_sclX below and System_waitFor (0x002f64b4)
 *                     do. JNI_initThread and JNI_createThread zero it.
 *   +0x3e frame_depth the index of the running frame in `frames`
 *                     (virtualMachine 0x002f1fb8/0x002f1fcc).
 *   +0x40 frame_limit JNI_createThread's second argument, the frame capacity
 *                     it sized the allocation from (sh a1,0x40(a1) at
 *                     0x002f06a4).
 */
typedef struct JThread {
    unsigned int : 32;                  /* +0x00 */
    struct JThread *previous;           /* +0x04 */
    struct JThread *next;               /* +0x08 */
    u8 kind;                            /* +0x0c */
    u8 wait_kind;                       /* +0x0d */
    unsigned short : 16;                /* +0x0e */
    void *object;                       /* +0x10 */
    void (*reset)(struct JThread *thread); /* +0x14 */
    void *method;                       /* +0x18 */
    unsigned int : 32;                  /* +0x1c */
    unsigned int : 32;                  /* +0x20 */
    u32 flags;                          /* +0x24 */
    u32 *stack;                         /* +0x28 */
    void *frames;                       /* +0x2c */
    void *wait_target;                  /* +0x30 */
    int wait_parameter;                 /* +0x34 */
    u16 stack_offset;                   /* +0x38 */
    short stack_limit;                  /* +0x3a */
    u16 resume_frames;                  /* +0x3c */
    u16 frame_depth;                    /* +0x3e */
    short frame_limit;                  /* +0x40 */
} JThread;

/* The thread is blocked until the actor at wait_target clears Actor.flags
 * bit 1 -- case 4 of JTHREAD_waitFor's table (.L003057E0, 0x00305730). */
#define JTHREAD_WAIT_ACTOR 4

/*
 * One entry of `actSequence`, head only, defined here exactly as
 * src/main/near_dir.h (main/tu257) defines it for the same object.
 *
 * `flags` records which channels of the entry have already been started and
 * `state_flags` which ones are still requested: ACT_updateSequence
 * (0x0030ab90) clears a handler slot whose bit has gone out of `state_flags`
 * (0x0030ac34..0x0030ac54) and clears Actor.flags bit 1 when the entry's own
 * mask at +0x08 no longer intersects it (0x0030ac58). SEQ_scale sets the
 * `flags` bit of an axis when it starts it (0x003096bc) and clears the
 * `state_flags` bit when the axis reaches its target (0x003097e8); CHR_sclX/Y/Z
 * below request an axis by setting its `state_flags` bit (0x20 for x, 0x40 for
 * y, 0x80 for z).
 */
typedef struct {
    u32 flags;
    u32 state_flags;
} SequenceState;

/* The entry array's stride: the sequence index is multiplied by 0x260 in
 * every one of its users (CHR_sclX 0x002ff604..0x002ff614). */
#define SEQUENCE_STRIDE 0x260

/*
 * The four handler slots of a sequence entry, +0x24..+0x30. ACT_updateSequence
 * walks exactly these four words once a frame, skips one that is not a word-
 * aligned text address and otherwise calls it with the actor
 * (0x0030abe8..0x0030ac18). Slot 0 (+0x24) carries the movement handler, slot
 * 1 (+0x28) the rotation handler CHR_rotX/Y/Z install (0x002fd704), slot 2
 * (+0x2c) the motion handler and slot 3 (+0x30) the scale handler CHR_sclX/Y/Z
 * install.
 *
 * The slots are a named offset and not members because +0x0c..+0x20 of the
 * entry lie between them and the recovered head above: ACT_createChr,
 * ACT_initSequence, ACT_initSequenceAt and Java_xeno_Chr_setPeer write all
 * five of those words and this TU evidences none of their roles, so a struct
 * reaching +0x24 could only be built by inventing them. The expression below
 * is the one the accepted body already had.
 */
#define SEQUENCE_SCALE_HANDLER(entry) \
    (*(void (**)(void))((u8 *)(entry) + 0x30))

/*
 * The scale channel of a sequence entry, at +0x1b8 of the entry.
 *
 * The entry carries three channels of this shape -- movement at +0x38,
 * rotation at +0xb8, scale at +0x1b8 -- and the rotation one is witnessed at
 * every offset the scale one is, which is how the channel's shape is known:
 *
 *   +0x00 track     the spline track: SEQ_scaleSPL (0x00309538) loads the
 *                   spline from +0x1b8, samples it at `frame` (+0x1bc),
 *                   writes the three floats into Actor.scale and stops when
 *                   `last_frame` (+0x1be) is passed; Java_xeno_Chr_scale writes
 *                   the same spline and last frame in (sw a1,0x0(s1) and
 *                   sh v1,0x6(s1) at 0x002ff4a8/0x002ff4a0, s1 = entry+0x1b8)
 *                   beside the handler slot. SEQ_moveSPL reads the movement
 *                   channel's `flags` at its +0x08 and Java_xeno_Chr_rotate
 *                   writes the rotation channel's (sh v1,0x8(s1) at
 *                   0x002fd560, s1 = entry+0xb8). SplineTrack is verbatim the type
 *                   src/main/near_dir.h declares for the same object.
 *   +0x0c           the word between the 12-byte track and the 16-byte grid
 *                   the four blocks below sit on. No instruction in any of the
 *                   six images touches it in any of the three channels
 *                   (+0x44, +0xc4 and +0x1c4 of an entry never appear), so it
 *                   names no field.
 *   +0x10 frames    the interpolation length in frames, per axis. CHR_sclX
 *                   writes the Java `sclX(int frames, float value, ...)`
 *                   argument here (sw v0,0x10(a1) at 0x002ff658) or -1 when
 *                   the caller gave a rate instead; SEQ_scale divides by it
 *                   and skips the axis when it is not positive (0x0030966c).
 *   +0x1c           the fourth word of that block. The rotation channel uses
 *                   its own copy as a per-axis bitmask (CHR_rotX clears bit 0
 *                   of +0xd4 at 0x002fd748, CHR_rotY bit 1, CHR_rotZ bit 2);
 *                   the scale channel's copy is untouched in the six images,
 *                   so it is left unnamed here.
 *   +0x20 start     the axis value SEQ_scale latches from Actor.scale when it
 *                   starts the axis (swc1 f3,0x0(t1), t1 = entry+0x1d8, at
 *                   0x00309674).
 *   +0x30 target    the value the axis is interpolated to: CHR_sclX writes
 *                   `argument + Actor.scale.x` here (swc1 f0,0x30(a1) at
 *                   0x002ff668) and SEQ_scale compares against it
 *                   (0x003097b4).
 *   +0x40 step      the per-frame increment: CHR_sclX writes the caller's rate
 *                   divided by 30.0f (swc1 f1,0x40(a1) at 0x002ff69c) and
 *                   SEQ_scale derives it from target, start and frames when
 *                   the caller gave a frame count instead (0x003096a4).
 *
 * The three axes are x, y and z in that order and the fourth lane of each
 * float block is untouched in the six images, so each block is the three
 * floats it evidences and no more.
 */
typedef struct {
    void *spline;
    short frame;
    short last_frame;
    u16 flags;
} SplineTrack;

typedef struct SequenceScale {
    SplineTrack track;   /* +0x00 */
    unsigned int : 32;   /* +0x0c */
    int frames[3];       /* +0x10 */
    unsigned int : 32;   /* +0x1c */
    float start[3];      /* +0x20 */
    unsigned int : 32;   /* +0x2c */
    float target[3];     /* +0x30 */
    unsigned int : 32;   /* +0x3c */
    float step[3];       /* +0x40 */
} SequenceScale;

/* Where that channel sits inside the entry; see SEQUENCE_SCALE_HANDLER for
 * why the entry's own head stops before it. */
#define SEQUENCE_SCALE_OFFSET 0x1b8

/*
 * Call block of Java_xeno_Chr_setVisible__IZ (0x002fee70..0x002feecc): the
 * boolean argument is a single byte at +0x8 (lbu a2,8(s1) at 0x002feeb0),
 * not the 4-byte union ChrScaleCall's `second` models.
 */
typedef struct ChrVisibleCall {
    u8 *object;
    int part;
    unsigned char visible;
} ChrVisibleCall;

/*
 * Call block of Java_xeno_Chr_setShadow__II (0x002ffe70..0x002ffed8): both
 * arguments are single bytes, at +0x4 and +0x8 (lbu v1,4(s0) / lbu v0,8(s0)
 * at 0x002ffec0/0x002ffec8), each in its own 4-byte argument slot.
 */
typedef struct ChrShadowCall {
    u8 *object;
    unsigned char kind;
    unsigned char unmodeled_5[3];
    unsigned char size;
} ChrShadowCall;

/*
 * The object a java.lang.String's own +0x4 reference points to: only its
 * own +0x8 word is evidenced (lw v0,8(a1) at 0x003007b8/0x00300820), read
 * back by talkto/touchto as the message id they forward to the peer.
 */
typedef struct JavaStringValue {
    unsigned int : 32;
    unsigned int : 32;
    int message_id;
} JavaStringValue;

/*
 * Head of a java.lang.String object as talkto/touchto read it: only the
 * reference at +0x4 is evidenced (lw v1,4(s1) at 0x003007a0/0x00300808).
 */
typedef struct JavaString {
    unsigned int : 32;
    JavaStringValue *value;
} JavaString;

/*
 * Call block of Java_xeno_Chr_talkto__Ljava_lang_String_ and
 * Java_xeno_Chr_touchto__Ljava_lang_String_: the Java String argument at
 * +0x4 (lw v1,4(s1) at 0x00300780/0x003007e8).
 */
typedef struct ChrTalkCall {
    u8 *object;
    JavaString *message;
} ChrTalkCall;

/*
 * Call block of Java_xeno_Chr_setWeaponR__Lxeno_Chr_ and
 * Java_xeno_Chr_resetWeaponR__Lxeno_Chr_: the other xeno.Chr Java argument
 * at +0x4 (lw s1,4(s0) at 0x003012e4/0x00301350), read through the same
 * peer field offset as `object`.
 */
typedef struct ChrWeaponCall {
    u8 *object;
    u8 *other;
} ChrWeaponCall;

/* Call block shape shared by every native below that reads only the object:
 * Java_xeno_Chr_getSignal__ and Java_xeno_Chr_getState__. */
typedef struct ChrObjectCall {
    u8 *object;
} ChrObjectCall;

/*
 * Call block of Java_xeno_Chr_signal__I: the value argument is a single
 * byte at +0x4 (lbu v1,4(s1) at 0x002fe8c8), in its own 4-byte slot.
 */
typedef struct ChrSignalCall {
    u8 *object;
    u8 value;
} ChrSignalCall;

/*
 * Call block shared by the boolean setters below: the flag is a single byte
 * at +0x4 (lbu v0,4(s1) at 0x002fee2c, 0x002fef0c and 0x003009f4), used by
 * Java_xeno_Chr_setVisible__Z, Java_xeno_Chr_setCollision__Z and
 * Java_xeno_Chr_dispRadar__Z.
 */
typedef struct ChrBoolCall {
    u8 *object;
    u8 flag;
} ChrBoolCall;

/*
 * Call block shared by the int setters below: the value is a full word at
 * +0x4 (lw v0,4(s0) at 0x002ffe30 and the same offset in
 * Java_xeno_Chr_setElevatorMode__I, Java_xeno_Chr_setClip__I,
 * Java_xeno_Chr_setSymmetryY__I and Java_xeno_Chr_ignoreShape__I).
 */
typedef struct ChrIntCall {
    u8 *object;
    int value;
} ChrIntCall;

/*
 * Call block of Java_xeno_Chr_setScale__FFF: three floats at +0x4/+0x8/+0xc
 * (lwc1 $f1,4(s1) / $f0,8(s1) / $f1,12(s1) at
 * 0x002ff2f0/0x002ff30c/0x002ff314).
 */
typedef struct ChrVector3Call {
    u8 *object;
    float x;
    float y;
    float z;
} ChrVector3Call;

/*
 * Call block of Java_xeno_Chr_setArgs__III: object +0x0, offset +0x4, the
 * value to copy from +0x8 and the byte count +0xc (lw v1,8(v0) / lw
 * s0,0xc(v0) / lw s1,0(v0) / lw s2,4(v0) at
 * 0x002ff014..0x002ff028).
 */
typedef struct ChrArgsWordCall {
    u8 *object;
    int offset;
    int value;
    int size;
} ChrArgsWordCall;

/*
 * Call block of Java_xeno_Chr_setArgs__ILjava_lang_Object_I: object +0x0,
 * offset +0x4, the Object argument at +0x8 (lw s1,8(v0) at 0x002ff144),
 * read the same shape ChrScaleCall's `first`/`second` already model
 * (lw a1,8(s1) / lw a0,4(s1) at 0x002ff164/0x002ff16c); the int at +0xc it
 * never reads.
 */
typedef struct ChrArgsObjectCall {
    u8 *object;
    int offset;
    ChrScaleCall *source;
    unsigned char unmodeled_0c[4];
} ChrArgsObjectCall;

/*
 * Call block of Java_xeno_Chr_getArgs__II: object +0x0, offset +0x4, the
 * byte count +0x8, in that order (lw s1,0(v0) / lw s2,4(v0) / lw s0,8(v0)
 * at 0x002ff0ac..0x002ff0b8); unlike ChrArgsWordCall above there is no value
 * word to read.
 */
typedef struct ChrArgsReadCall {
    u8 *object;
    int offset;
    int size;
} ChrArgsReadCall;

/*
 * The four-float Java value object Java_xeno_Chr_setFilterParam__aF's
 * array argument points to, reached through its own +0x8 pointer (lw
 * v1,8(a0) at 0x003003a8); the array header before that pointer is never
 * read.
 */
typedef struct ChrFilterParamValue {
    float components[4];
} ChrFilterParamValue;

typedef struct ChrFilterParamArray {
    unsigned char unmodeled_0[8];
    ChrFilterParamValue *value;                 /* +0x8 */
} ChrFilterParamArray;

/* Call block of Java_xeno_Chr_setFilterParam__aF: object +0x0, the array
 * argument at +0x4 (lw a0,4(s1) at 0x003003a0). */
typedef struct ChrFilterParamCall {
    u8 *object;
    ChrFilterParamArray *array;
} ChrFilterParamCall;

/*
 * GameLoopState is a 0x2a030-byte global (main VA 0x00338680) whose declared
 * type is TU-local by canon (config/header-canon.json): this TU evidences
 * only the word at +0x4, which Java_xeno_Chr_getPlayer__ copies into a
 * character's own `peer` field (lw v0,-31100(v1) with v1=0x340000 at
 * 0x002fcd28, 0x340000-31100 = 0x338684).
 */
typedef unsigned int GameLoopStateWords[];
extern GameLoopStateWords GameLoopState;

/*
 * ACT_setVisible/ACT_setHand/ACT_setArms/ACT_resetArms are defined in
 * main/act_2.c and main/near_dir.c (still ASM there) and declared locally
 * here as the sibling calls at 0x002feec8, 0x002fefa4, 0x003012fc and
 * 0x00301368 pass them: an Actor pointer, then the character natives'
 * remaining Java arguments. src/ov01/battle_init.c already declares
 * ACT_setHand the same way for its own TU.
 */
extern void ACT_setVisible(Actor *actor, int part, unsigned char visible);
extern void ACT_setHand(Actor *actor, int hand);
extern void ACT_setArms(Actor *actor, Actor *other, int acc_id, int flags);
extern void ACT_resetArms(Actor *actor, Actor *other, int acc_id);

/* canon: config/header-canon.json (src/math/main/review09-002f6c50/private.h),
 * verbatim again here as src/main/layout.h and src/main/call_java_method.h
 * already do. Java_xeno_Chr_setID__I passes a "type selector" 0 and this
 * unit's id (a0=0, a1=id at 0x300070/0x300074), the same two-argument shape
 * both other accepted call sites use. */
extern LayoutHeader *UnduDataGetHeader(int map_index, int unit_index);

extern void *classJava_xeno_Chr;
/* Verbatim the canonical spelling of include/shared.h (see JavaField above
 * for why chr.h does not include shared.h itself):
 * Java_xeno_Chr_look_unit__Ljava_lang_Object_ looks its target's own peer
 * field up on xeno.Unit's class, not xeno.Chr's (lw a0,-13252(gp) at
 * 0x00300b5c). */
extern void *classJava_xeno_Unit;
extern int JNI_isInstanceOf(void *object, void *class_object);
extern void *loadConstString(const char *text, int length);
extern JavaField *lookupClassField(void *class_object, void *name, int flags);
extern const char chr_peer_string[];
extern const char chr_algorithm_string[];
extern const char D_004DC1A0[];
extern const char D_004DC1A8[];
extern const char D_004DC1B0[];
/*
 * "rx"/"ry"/"rz", the rotation-field counterparts of the "px"/"py"/"pz"
 * strings above: Java_xeno_Chr_stop__ reads the peer's rotation into the
 * Java fields these name (lui/addiu at 0x002fe54c/0x002fe57c/0x002fe5ac),
 * and Java_xeno_Chr_getRotate__ (still asm here) uses the same three.
 */
extern const char D_004DC1B8[];
extern const char D_004DC1C0[];
extern const char D_004DC1C8[];
extern u8 actSequence[0x9800];
extern float defaultOffset[];
extern void SEQ_scale(void);

#endif
