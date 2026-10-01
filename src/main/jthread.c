#include "common.h"
#include "shared.h"
#include "jthread.h"

/*
 * The Java runtime installs this as the thread's `entry` hook for a
 * xeno.Unit instance. It runs the pending method unless the Unit's native
 * peer (peer->flags bit 0x2) or the thread itself (object == 0) says not
 * to.
 */
void JTHREAD_defaultUnit(JThreadHandle *thread)
{
    SceneObject object;
    JavaField *field;
    JThreadUnitPeer **peer_slot;
    JThreadUnitPeer *peer;
    SceneObject arguments[1];
    unsigned int flags;
    unsigned int flags_after_call;

    object = thread->object;
    field = lookupClassField(classJava_xeno_Unit, loadConstString(D_004DC080, -1), 0);
    peer_slot = (JThreadUnitPeer **)(object + field->offset);
    peer = *peer_slot;
    if (!(peer->flags & 0x2) && object != 0) {
        flags = thread->flags;
        if (flags & 0x10) {
            if (flags & 0x1) {
                thread->flags = (flags & ~0x1) | 0x2;
            }
            arguments[0] = object;
            JNI_callMethod(thread, thread->method, arguments, 0);
            flags_after_call = thread->flags;
            if (flags_after_call & 0x8) {
                thread->flags = flags_after_call & ~0x10;
            }
        }
    }
}

/*
 * The Java runtime installs this as the thread's `entry` hook for a
 * xeno.Chr instance; otherwise identical to JTHREAD_defaultUnit, reached
 * through xeno.Chr's own "peer" field instead.
 */
void JTHREAD_defaultChr(JThreadHandle *thread)
{
    SceneObject object;
    JavaField *field;
    JThreadChrPeer **peer_slot;
    JThreadChrPeer *peer;
    SceneObject arguments[1];
    unsigned int flags;
    unsigned int flags_after_call;

    object = thread->object;
    field = lookupClassField(classJava_xeno_Chr, loadConstString(D_004DC080, -1), 0);
    peer_slot = (JThreadChrPeer **)(object + field->offset);
    peer = *peer_slot;
    if (!(peer->flags & 0x2) && object != 0) {
        flags = thread->flags;
        if (flags & 0x10) {
            if (flags & 0x1) {
                thread->flags = (flags & ~0x1) | 0x2;
            }
            arguments[0] = object;
            JNI_callMethod(thread, thread->method, arguments, 0);
            flags_after_call = thread->flags;
            if (flags_after_call & 0x8) {
                thread->flags = flags_after_call & ~0x10;
            }
        }
    }
}

/*
 * The Java runtime installs this as the thread's `entry` hook for the
 * default script method: when the thread has both a live object and a
 * method to run, it invokes the method and, if the call left flag 0x8 set,
 * clears flag 0x10 the same way JTHREAD_defaultUnit/JTHREAD_defaultChr do.
 */
void JTHREAD_default(JThreadHandle *thread)
{
    SceneObject object;
    SceneMethod *method;
    SceneObject arguments[4];
    unsigned int flags;
    int result;

    object = thread->object;
    if (object != 0) {
        method = thread->method;
        if (method != 0) {
            arguments[0] = object;
            JNI_callMethod(thread, method, arguments, &result);
            flags = thread->flags;
            if (flags & 0x8) {
                thread->flags = flags & ~0x10;
            }
        }
    }
}

/*
 * Identical to JTHREAD_default; installed as the `entry` hook for a
 * xeno.Scene instance's default script method.
 */
void JTHREAD_defaultScene(JThreadHandle *thread)
{
    SceneObject object;
    SceneMethod *method;
    SceneObject arguments[4];
    unsigned int flags;
    int result;

    object = thread->object;
    if (object != 0) {
        method = thread->method;
        if (method != 0) {
            arguments[0] = object;
            JNI_callMethod(thread, method, arguments, &result);
            flags = thread->flags;
            if (flags & 0x8) {
                thread->flags = flags & ~0x10;
            }
        }
    }
}

/*
 * The Java runtime installs this as the thread's `entry` hook for a
 * xeno.Stage instance: when the object's own +0x2c "stage" reference and
 * the thread's method are both live, it prints "Class.method" through
 * xglFontDebugPrintf, then forwards the stage object to JNI_callMethod as
 * the call's own object argument.
 */
void JTHREAD_defaultStage(JThreadHandle *thread)
{
    JThreadStageObject *object;
    SceneObject stage_object;
    SceneClass *scene_class;
    SceneMethod *method;
    /*
     * The method's own name-and-signature record: the same shape as
     * SceneClassName (an unmodeled span, then a string at +8), read
     * through this TU's own cast since src/main/init_class_db.c owns
     * SceneMethod's completed body (lw 0(s0) at 0x003056d8).
     */
    SceneClassName **method_name_slot;
    SceneClassName *method_name;
    SceneObject arguments[4];
    int output;

    object = (JThreadStageObject *)thread->object;
    stage_object = object->stage;
    if (stage_object != 0) {
        scene_class = ((SceneObjectHeader *)stage_object)->class_ref->scene_class;
        method = thread->method;
        if (method != 0) {
            method_name_slot = (SceneClassName **)method;
            method_name = *method_name_slot;
            xglFontDebugPrintf(0, 8, D_004DC088,
                scene_class->name->binary_name,
                method_name->binary_name);
            arguments[0] = stage_object;
            JNI_callMethod(thread, method, arguments, &output);
        }
    }
}

void JTHREAD_init(void)
{
}

/*
 * This TU's own view of the VM thread list node that src/main/chr.h (main's
 * chr TU) defines in full as `JThread`; only the fields the functions below
 * read are named here (chr.h's `previous` at +0x04 stays unmodeled):
 *
 *   +0x08 next           JTHREAD_info, JTHREAD_get (lw a1,8(a1) at
 *                        0x00305970) and JTHREAD_cntl (lw s0,8(s0) at
 *                        0x003058f8/0x0030590c/0x0030592c) all walk it from
 *                        jthreadTop.
 *   +0x0c kind           JTHREAD_cntl runs a node's reset hook only when
 *                        this is nonzero (lbu v0,12(s0) at 0x003058fc).
 *   +0x0d wait_kind      JTHREAD_waitFor switches on `wait_kind & 0xf`
 *                        (lbu v0,13(a1) at 0x00305734).
 *   +0x10 object         JTHREAD_get compares it with its own argument
 *                        (lw v1,16(a1) at 0x00305964).
 *   +0x14 reset          the per-node hook JTHREAD_cntl calls with the node
 *                        itself (lw v0,20(s0); jalr v0 at
 *                        0x00305910/0x0030591c).
 *   +0x24 flags          JTHREAD_cntl only considers nodes carrying bit
 *                        0x10 (lw v0,36(s0) at 0x003058e0); JTHREAD_waitFor
 *                        updates it once a wait is satisfied.
 *   +0x30 wait_target    what JTHREAD_waitFor's node waits on, per
 *                        wait_kind (lw at 0x00305760/0x00305788/0x003057a8/
 *                        0x003057e0/0x00305800).
 *   +0x34 wait_parameter the wait's second operand: a frame count or a
 *                        motion number (lw at 0x00305764/0x00305804).
 */
typedef struct JThreadNode {
    unsigned char unmodeled_00[8];
    struct JThreadNode *next;           /* +0x08 */
    unsigned char kind;                 /* +0x0c */
    unsigned char wait_kind;             /* +0x0d */
    unsigned char unmodeled_0e[0x10 - 0x0e];
    SceneObject object;                 /* +0x10 */
    void (*reset)(struct JThreadNode *thread); /* +0x14 */
    unsigned char unmodeled_18[0x24 - 0x18];
    unsigned int flags;                 /* +0x24 */
    unsigned char unmodeled_28[0x30 - 0x28];
    void *wait_target;                  /* +0x30 */
    int wait_parameter;                 /* +0x34 */
} JThreadNode;

extern JThreadNode *jthreadTop;

/*
 * The actor JTHREAD_waitFor's wait_target points at when wait_kind is 4 or
 * 6. src/main/chr.h (main's chr TU) completes the whole record as `Actor`;
 * only the two words this function reads are restated here, under this
 * TU's own tag: `flags` bit 1 (0x2) gates cases 4/6 (andi 0x2 at
 * 0x003057e8/0x003057fc) and the byte at +0x81 is the motion value case 5
 * compares against wait_parameter (lbu v1,129(v0) at 0x00305808).
 */
typedef struct JThreadActorRef {
    unsigned int flags; /* +0x00 */
    unsigned char unmodeled_04[0x81 - 0x04];
    unsigned char motion; /* +0x81 */
} JThreadActorRef;

/*
 * The menu or window object JTHREAD_waitFor's wait_target points at when
 * wait_kind is 3. Several TUs complete this family of objects in full
 * (e.g. src/main/tslider_create.h's own `state` at the same +0x14); only
 * that one halfword is restated here, under this TU's own tag (lhu
 * v1,20(v0) at 0x00305790, compared against 2).
 */
typedef struct JThreadWindowRef {
    unsigned char unmodeled_00[0x14];
    unsigned short state; /* +0x14 */
} JThreadWindowRef;

/*
 * Blocks the calling thread's script method until the condition its own
 * wait_kind names is satisfied, then clears flag 0x4 (and, for the actor
 * waits, 0x1 too) and sets flag 0x2 on it. Out-of-range wait_kind values do
 * nothing.
 */
void JTHREAD_waitFor(JThreadNode *thread)
{
    switch (thread->wait_kind & 0xf) {
    case 1:
        thread->wait_target = (void *)((int)thread->wait_target + 1);
        if ((int)thread->wait_target >= thread->wait_parameter) {
            thread->flags = (thread->flags & ~0x4) | 0x2;
        }
        break;

    case 3:
        if (((JThreadWindowRef *)thread->wait_target)->state == 2) {
            thread->flags = (thread->flags & ~0x4) | 0x2;
        }
        break;

    case 2:
    {
        JThreadNode *other = (JThreadNode *)thread->wait_target;
        if (other->flags & 0x8) {
            other->flags &= ~0x8;
            thread->flags = (thread->flags & ~0x4) | 0x2;
        }
        break;
    }

    case 4:
        if (!(((JThreadActorRef *)thread->wait_target)->flags & 0x2)) {
            thread->flags = (thread->flags & ~0x5) | 0x2;
        }
        break;

    case 6:
        if (!(((JThreadActorRef *)thread->wait_target)->flags & 0x2)) {
            thread->flags = (thread->flags & ~0x5) | 0x2;
        }
        break;

    case 5:
        if (((JThreadActorRef *)thread->wait_target)->motion ==
            thread->wait_parameter) {
            thread->flags = (thread->flags & ~0x4) | 0x2;
        }
        break;

    case 7:
        if (!(GameLoopState[0x10 / sizeof(unsigned int)] & 0x800)) {
            thread->flags = (thread->flags & ~0x5) | 0x2;
        }
        break;
    }
}

void JTHREAD_info(void)
{
    JThreadNode *thread = jthreadTop;

    if (thread != 0) {
        do {
            thread = thread->next;
        } while (thread != 0);
    }
}

/*
 * One entry of a thread's symbol chain: JTHREAD_getSymbol scans a flat
 * array of these, terminated by a zero `valid` word (lw 4(a0) at
 * 0x00305888/0x003058a4).
 */
typedef struct JThreadSymbol {
    int id;    /* +0x00 */
    int valid; /* +0x04, zero terminates the chain */
} JThreadSymbol;

/*
 * Searches the given symbol chain for an entry whose id matches, returning
 * that entry or 0 once the chain's terminating zero `valid` word is
 * reached.
 */
JThreadSymbol *JTHREAD_getSymbol(JThreadSymbol *symbols, int id)
{
    while (symbols->valid != 0) {
        if (id == symbols->id) {
            return symbols;
        }
        symbols++;
    }
    return 0;
}

/*
 * The thread-list reset hook: jthreadResetFunc collects a pending
 * whole-runtime reset request that a node's own reset hook may raise. This
 * runs the reset hook of every active (kind != 0), pending-work (flags &
 * 0x10) node in turn, stopping early the moment that happens, then runs the
 * collected runtime reset once and clears it.
 */
extern void (*jthreadResetFunc)(void);

void JTHREAD_cntl(void)
{
    JThreadNode *node = jthreadTop;

    while (node != 0) {
        if ((node->flags & 0x10) != 0 && node->kind != 0) {
            if (node->reset != 0) {
                node->reset(node);
            }
            if (jthreadResetFunc != 0) {
                break;
            }
        }
        node = node->next;
    }
    if (jthreadResetFunc != 0) {
        jthreadResetFunc();
        jthreadResetFunc = 0;
    }
}

/*
 * Searches the active thread list from jthreadTop for the node whose
 * object matches, returning it or 0 when none does.
 */
JThreadNode *JTHREAD_get(SceneObject object)
{
    JThreadNode *node = jthreadTop;

    if (node != 0) {
        do {
            if (node->object == object) {
                return node;
            }
            node = node->next;
        } while (node != 0);
    }
    return 0;
}
