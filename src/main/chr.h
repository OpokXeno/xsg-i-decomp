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

/* Verbatim the engine's four-float vector record of include/shared.h and of
 * src/main/near_dir.h, which defines the actor below with it. */
typedef struct Vector4 {
    float x;
    float y;
    float z;
    float w;
} Vector4;

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

extern void *classJava_xeno_Chr;
extern int JNI_isInstanceOf(void *object, void *class_object);
extern void *loadConstString(const char *text, int length);
extern JavaField *lookupClassField(void *class_object, void *name, int flags);
extern const char chr_peer_string[];
extern const char chr_algorithm_string[];
extern u8 actSequence[0x9800];
extern float defaultOffset[];
extern void SEQ_scale(void);

#endif
