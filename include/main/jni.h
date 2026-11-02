#ifndef INCLUDE_MAIN_JNI_H
#define INCLUDE_MAIN_JNI_H

#include "shared.h"
#include "main/data_buffer.h"
#include "main/find_native_method.h"

/*
 * The VM/native-thread object every JNI_* entry point in the engine shares.
 * The scene VM (SceneThread in scene_1.h and script.h before the merge) is the
 * same record: JTHREAD_defaultScene is handed a thread and passes it unchanged
 * as JNI_callMethod's first argument, and a SceneVm handle points at it.
 *
 * Evidence per span (merged from every unit's view):
 *   +0x0c kind, +0x0d wait_kind        JTHREAD_cntl / JTHREAD_waitFor
 *   +0x10 object                       the instance the thread belongs to; JTHREAD_get
 *                                      (0x00305958) searches on it and CallMethod reads the
 *                                      Stage object through it (lw 0x10 at 0x00261b9c)
 *   +0x14 reset (scene views: entry)   the hook JTHREAD_cntl calls once per visit with the
 *                                      thread (lw 0x14/jalr at 0x00305910..0x00305920);
 *                                      SCENE_start installs JTHREAD_defaultScene here
 *   +0x18 method                       the method `reset` runs for scene threads:
 *                                      JTHREAD_defaultScene loads it, returns when it is null
 *                                      and otherwise calls it on `object` (lw 0x18 at 0x00305654)
 *   +0x24 flags                        bit 3 pending call (JNI_callMethod's result test in
 *                                      script.c, cleared by JTHREAD_waitFor), bit 4 "there is a
 *                                      method to run" (set by scene_1.c), bit 0x10 runnable
 *                                      (JTHREAD_cntl, stage_2.h)
 *   +0x28 stack (scene views: call_frame) null once the thread has no call left to run
 *   +0x2c frames                       0x18-byte VMFrame records DB_evtMonitor prints
 *   +0x30 wait_target, +0x34 wait_parameter, +0x38..+0x40  JTHREAD_waitFor and the
 *                                      script call-stack depth (+0x3e, signed, frame_depth)
 *
 * The record does not end at +0x28; the members below are every span a unit
 * here reads, and nothing between them is recovered.
 */
/* One four-byte VM operand-stack / local-variable slot. */
typedef union VMSlot {
    int i;
    float f;
    void *ref;
    unsigned char b;
    short s;
    unsigned short c;
} VMSlot;

/* A frame's local variables or operand stack: a window of thread->stack. */
typedef struct VMSlots {
    short top;
    unsigned short size;
    VMSlot *base;
} VMSlots;

/*
 * One 0x18-byte call frame of JThread.frames (virtualMachine): the current
 * and next bytecode offsets, the method an invoke of this frame is calling
 * (kept so a suspended native call can be resumed; DB_evtMonitor prints its
 * name with pc), then the frame's locals and operand stack.
 */
typedef struct VMFrame {
    unsigned short pc;                  /* +0x00 */
    unsigned short nextPc;              /* +0x02 */
    JavaMethod *callee;                 /* +0x04 */
    VMSlots locals;                     /* +0x08 */
    VMSlots stack;                      /* +0x10 */
} VMFrame;

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
    VMSlot *stack;                      /* +0x28 */
    VMFrame *frames;                    /* +0x2c */
    /* wait_kind selects elapsed frames, a native peer, or a thread. */
    union {
        int elapsed_frames;
        void *object;
        struct JThread *thread;
    } wait_target;                    /* +0x30, four-byte tagged storage */
    int wait_parameter;                 /* +0x34 */
    u16 stack_offset;                   /* +0x38 */
    short stack_limit;                  /* +0x3a */
    short resume_frames;                /* +0x3c: signed, the VM loads it lh */
    short frame_depth;                  /* +0x3e, signed call-stack depth */
    short frame_limit;                  /* +0x40 */
} JThread;

/* JThread.kind (the index of tiAlgName, "JTHREAD_ALG_*"): the Java object kind a thread runs. */
enum {
    JTHREAD_KIND_SYSTEM = 0,
    JTHREAD_KIND_CHR = 1,
    JTHREAD_KIND_STAGE = 2,
    JTHREAD_KIND_SCENE = 3,
    JTHREAD_KIND_DEFAULT = 4,
    JTHREAD_KIND_UNIT = 5
};

/* JThread.flags */
enum {
    JTHREAD_FLAG_WAITING = 0x01,        /* generic wait (set with DONE by the thread-exit natives) */
    JTHREAD_FLAG_WOKEN = 0x02,          /* a wait was satisfied */
    JTHREAD_FLAG_CALL_RUNNING = 0x04,   /* a call is already running on the thread */
    JTHREAD_FLAG_DONE = 0x08,           /* the top-level method returned */
    JTHREAD_FLAG_RUNNABLE = 0x10,       /* JTHREAD_cntl considers the thread */
    JTHREAD_FLAG_STALE_CALL = 0x20,     /* a stale call JNI_callMethod resets */
    JTHREAD_FLAG_FRAME_RETURNED = 0x80, /* the interpreter left a frame and reloads its state */
    JTHREAD_FLAGS_BUSY = JTHREAD_FLAG_WAITING | JTHREAD_FLAG_CALL_RUNNING,
    JTHREAD_FLAGS_LEAVE_FRAME = JTHREAD_FLAG_DONE | JTHREAD_FLAG_FRAME_RETURNED
};

/* JThread.wait_kind (low nibble): what JTHREAD_waitFor waits for. */
#define JTHREAD_WAIT_KIND_MASK 0xf
enum {
    JTHREAD_WAIT_FRAMES = 1,       /* wait_parameter frames elapsed */
    JTHREAD_WAIT_THREAD = 2,       /* another thread is done */
    JTHREAD_WAIT_COMPONENT = 3,    /* a window component reached its state 2 */
    JTHREAD_WAIT_ACTOR_IDLE = 4,   /* an actor's flag 0x2 is clear */
    JTHREAD_WAIT_SIGNAL = 5,       /* an actor's signal equals wait_parameter */
    JTHREAD_WAIT_ACTOR_IDLE_2 = 6, /* same test as JTHREAD_WAIT_ACTOR_IDLE */
    JTHREAD_WAIT_GAME = 7          /* the game state's flag 0x800 is clear */
};

/* The virtual-machine register bank (VMRegister) and its index mask. */
#define VM_REGISTER_COUNT 32
#define VM_REGISTER_MASK (VM_REGISTER_COUNT - 1)

extern JavaClass *classJava_xeno_Scene;

/* Proposal: script "Stage" class handle, parallel to the already-shared
 * classJava_xeno_Scene (src/core/main-0025a6d8/private.h). Evidenced by
 * gp-relative access at .sbss+0x4dc7f0 (context.json data_refs). */
extern JavaClass *classJava_xeno_Stage;

extern JavaClass *classJava_xeno_Chr;
extern JavaClass *classJava_xeno_Unit;
extern JavaClass *classJava_xeno_Enepc;
extern JavaClass *classJava_xeno_util_Window;
extern JavaClass *classJava_xeno_Uwamono;
extern JavaClass *classJava_xeno_util_Vector4f;
extern JavaClass *classJava_xeno_util_Layout;
extern JavaClass *classJava_xeno_Effect;
extern JavaClass *classJava_xeno_Light;
extern JavaClass *classJava_xeno_util_Menu;
extern JavaClass *classJava_xeno_util_TCHParams;
extern JavaClass *classJava_xeno_util_Spline;
extern JavaClass *classJava_xeno_util_Input;
extern JavaClass *classJava_xeno_Camera;
extern JavaClass *classJava_xeno_Movie;

void JNI_popFrame(void);

/* Functions and data used by other translation units. */
int JNI_isInstanceOf(SceneObject object, JavaClass *target_class);
/*
 * Fourth argument is an optional VM-result output pointer.  The original
 * JNI_callMethod preserves incoming a3 (002f0580) and forwards it unchanged
 * (002f05fc) to virtualMachine, which stores one result word through it
 * (002f36a0/002f36a4/002f36ac: lw v1,0(v0); lw v0,64(sp); sw v1,0(v0)).
 * NULL requests no output.  It is not an integer result value.
 */
void JNI_callMethod(SceneVm *vm, JavaMethod *method, SceneObject *arguments, int *output);

#endif /* INCLUDE_MAIN_JNI_H */
