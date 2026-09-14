/*
 * TU-local declarations of main/tu148 (src/main/script.c).
 */

#ifndef SRC_MAIN_SCRIPT_H
#define SRC_MAIN_SCRIPT_H

#include "shared.h"

/*
 * MINIMAL PARTIAL VIEW (explicit limitation, per review 16064):
 * GameModeCfEvent reads only three evidenced offsets, so only those are
 * modeled. The remainder of both objects is unmodeled in this TU; no
 * complete-sized struct is claimed and no reserved/padding spans are
 * invented. Offsets proven by the pinned original bytes (0x00245af8):
 *  - GameLoopState+0x10 : 32-bit flags (lw/sw, cleared with & -2,
 *    event-taken path sets bit31 with | 0x80000000).
 *  - GameLoopState+0x29F40 : event byte (lbu via lui 0x3 + addu + -24768;
 *    base 0x338680 + 0x29F40 = 0x3625C0). Accessed here by explicit
 *    offset so no full 0x2A030 layout is invented.
 *  - PadData+0x28 : 16-bit halfword, masked with 0x10C, compared to 268.
 *  - PadData+0x2A : 16-bit halfword, masked with 0x800.
 * Neutral half_28/half_2a names: the 0x10C-vs-0x800 caller roles are not
 * proven, so no functional pad_input_* labels are claimed.
 */
typedef struct {
    u8 pad_00[0x10];
    u32 flags;
} GameLoopFlagsPrefix;

/*
 * This common address view is used only for the proved pointer-sized access at
 * byte offset +8 and for addressed scalar accesses.  It is not a complete
 * GameLoopState layout or a claim about elements at offsets +0/+4.
 */
typedef void *GameLoopStateAddressView[];

typedef struct ScriptObserverTask ScriptObserverTask;

/*
 * The VM thread, completed from the two evidenced words this unit reads.
 *
 * A SceneVm handle and a SceneThread are the same object: JTHREAD_defaultScene
 * (0x00305638) is handed a thread and passes it unchanged as JNI_callMethod's
 * first argument, which is the SceneVm parameter every other caller here fills
 * with stageVM or evtVM.  src/main/scene_1.h carries the same declaration for
 * the same reason.
 *
 *   +0x10 object  the instance the thread belongs to.  JTHREAD_get
 *                 (0x00305958) walks the jthreadTop list comparing this word
 *                 with its argument, so it is the search key; CallMethod reads
 *                 the Stage object of the current script through it (lw 0x10
 *                 at 0x00261b9c, and the same load in CallMethod_I,
 *                 CallMethod_II and createTalkTask at 0x00261690); and
 *                 DB_evtMonitor follows its class chain to print the thread as
 *                 "class.method" (0x002641dc..0x0026421c).
 *   +0x14 entry   the hook JTHREAD_cntl calls once per visit with the thread
 *                 as its only argument (lw 0x14/jalr at
 *                 0x00305910..0x00305920); SCENE_start installs
 *                 JTHREAD_defaultScene here.
 *   +0x18 method  the method `entry` runs: JTHREAD_defaultScene loads it,
 *                 returns when it is null and otherwise calls it on `object`
 *                 (lw 0x18 at 0x00305654).
 *   +0x24 flags   bit 3 is the pending-call bit this unit tests after
 *                 JNI_callMethod (talktoObserver 0x00261998, funcObserver
 *                 0x00261f28): JTHREAD_defaultScene keeps the thread runnable
 *                 while it is set (andi 8 at 0x0030567c) and JTHREAD_waitFor
 *                 clears it in the thread it wakes (0x003057c0).  Bit 4, which
 *                 src/main/scene_1.c sets, is "there is a method to run".
 *
 * Nothing between those members is recovered, and the record does not end at
 * +0x28: JTHREAD_waitFor uses +0x0d, +0x30 and +0x34, and DB_evtMonitor prints
 * a call stack from +0x2c with its depth at +0x3e.
 */
struct SceneThread {
    unsigned char unmodeled_00[16];      /* +0x00 */
    SceneObject object;                  /* +0x10 */
    void (*entry)(SceneThread *thread);  /* +0x14 */
    SceneMethod *method;                 /* +0x18 */
    unsigned char unmodeled_1c[8];       /* +0x1c */
    unsigned int flags;                  /* +0x24 */
};

#define SCENE_THREAD_CALL_PENDING 0x8u

/*
 * The observer task of the script system, behind the scheduler's 0x80-byte
 * pool node.
 *
 * The first 16 bytes are the XglTaskPrefix xglTaskEntryNext maintains
 * (src/main/xgl_task.c); everything after them belongs to the entry point that
 * allocated the node.  Four entry points of this unit fill one in and they
 * agree on every member below: createTalkTask (0x00261620) for talktoObserver,
 * and CallMethod (0x00261b58), CallMethod_I (0x00261c28) and CallMethod_II
 * (0x00261d08) for funcObserver.  attrObserver, sceneObserver and
 * sceneObserver2, still assembler in this TU, read the same offsets.
 *
 *   +0x10 state_flags     bit 0 is the "a VM slot has been claimed" latch:
 *                         every entry point clears the word (sw $0,0x10 at
 *                         0x002616f4, 0x00261774, 0x00261bec, 0x00261cc8,
 *                         0x00261da8), funcObserver tests it before it calls
 *                         getEmptyVM, and getEmptyVM sets it when it takes a
 *                         slot.
 *   +0x14 object          the object the task works on.  CallMethod and its
 *                         two siblings store the Stage object they resolved
 *                         (sw 0x14 at 0x00261bf8) and funcObserver re-reads it
 *                         to notice the Stage changed; createTalkTask stores
 *                         its own first argument, the actor whose talk command
 *                         is running (sw $21,0x14 at 0x002617d8), which is
 *                         what talkCancel hands to actTalkAfter (lw 0x14 at
 *                         0x002619fc).
 *   +0x18 receiver        the method receiver talktoObserver calls on:
 *                         createTalkTask stores the current script's own Stage
 *                         object here (sw $30,0x14... 0x18 at 0x002617c4).
 *   +0x1c owner           &GameLoopState, stored by every entry point (sw 0x1c
 *                         at 0x002616f8, 0x00261778, 0x00261be8, 0x00261cc4,
 *                         0x00261da4) and read by both task bodies for its
 *                         flags word.
 *   +0x20 argument1       the second element of the JNI argument array:
 *                         CallMethod_I and CallMethod_II write their int
 *                         argument here (sw 0x20 at 0x00261cd8, 0x00261dbc).
 *   +0x24 argument2       the third element.  CallMethod_II writes its second
 *                         int argument (sw 0x24 at 0x00261dc0); createTalkTask
 *                         writes 0 for the no-argument talk signature
 *                         (0x00261704) and the window it built for the other
 *                         one (sw $16,0x24 at 0x002617c0), which talkCancel
 *                         reads back to close and dispose it (lw 0x24 at
 *                         0x00261a00).
 *   +0x30 method_name     the JNI method name bytes (sw 0x30 at 0x002617cc,
 *                         0x00261bfc, 0x00261cdc, 0x00261dc4).
 *   +0x34 method_signature
 *                         the JNI signature bytes (sw 0x34 at 0x0026170c,
 *                         0x0026178c, 0x00261c00, 0x00261ce0, 0x00261dc8).
 *   +0x38 vm_slot         the event-VM index getEmptyVM claimed, -1 when none;
 *                         attrObserver reads it as a signed byte (lb s1,56(s2)
 *                         at 0x002622a0).
 *   +0x3c method          the method the body resolved, 0 until findMethod
 *                         found it (sw 0x3c at 0x002617d0).
 *
 * The record is not complete.  The eight bytes at +0x28 and the two at +0x3a
 * are touched by nothing in any of the six images and stay byte ranges, and
 * the node runs to 0x80 bytes.
 */
struct ScriptObserverTask {
    XglTaskPrefix task;              /* +0x00 */
    u32 state_flags;                 /* +0x10 */
    SceneObject object;              /* +0x14 */
    SceneObject receiver;            /* +0x18 */
    GameLoopFlagsPrefix *owner;      /* +0x1c */
    int argument1;                   /* +0x20 */
    int argument2;                   /* +0x24 */
    u8 unmodeled_28[8];              /* +0x28 */
    const char *method_name;         /* +0x30 */
    const char *method_signature;    /* +0x34 */
    signed char vm_slot;             /* +0x38 */
    u8 unmodeled_39;                 /* +0x39 */
    u8 unmodeled_3a[2];              /* +0x3a */
    SceneMethod *method;             /* +0x3c */
};

#define OBSERVER_TASK_VM_CLAIMED 1u

/*
 * The actor's own script object, 0x4c0 bytes into the engine's 0xa70-strided
 * actor record (src/main/near_dir.h, `Actor`): SCRIPT_execTalkto reads it to
 * check the actor is a xeno/Chr instance before it queues the talk task (lw
 * 0x4c0 at 0x00261540) and DB_evtMonitor reads it to find the actor's VM
 * thread (lw 0x4c0 at 0x0026443c).  The recovered head of `Actor` ends at
 * +0x70 and nothing between there and +0x4c0 is recovered, so this offset
 * cannot become a member without inventing that span (docs/naming.md) and
 * takes docs/style.md rule 2's named-offset fallback.
 */
#define ACTOR_SCRIPT_OBJECT_OFFSET 0x4c0

#endif /* SRC_MAIN_SCRIPT_H */
