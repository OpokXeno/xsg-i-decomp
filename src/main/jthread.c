#include "common.h"
#include "shared.h"
#include "jthread.h"
#include "main/jni.h"
#include "main/jthread.h"
#include "main/init_vm.h"
#include "main/act_1.h"
#include "main/twsys_init.h"
#include "main/scene_1.h"

/*
 * The VM thread list node, `JThread` in include/main/jni.h. The functions
 * below read these members of it (its `previous` at +0x04 is not read here):
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


/*
 * One entry of a thread's symbol chain: JTHREAD_getSymbol scans a flat
 * array of these, terminated by a zero `valid` word (lw 4(a0) at
 * 0x00305888/0x003058a4).
 */
typedef struct JThreadSymbol {
    int id;    /* +0x00 */
    int valid; /* +0x04, zero terminates the chain */
} JThreadSymbol;

const char jthreadPeerName[] = "peer";

const char jthreadDottedNameFormat[8] = "%s.%s";

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
    field = lookupClassField(classJava_xeno_Unit, loadConstString(jthreadPeerName, -1), 0);
    peer_slot = (JThreadUnitPeer **)(object + field->offset);
    peer = *peer_slot;
    if (!(peer->flags & 0x2) && object != 0) {
        flags = thread->flags;
        if (flags & JTHREAD_FLAG_RUNNABLE) {
            if (flags & JTHREAD_FLAG_WAITING) {
                thread->flags = (flags & ~JTHREAD_FLAG_WAITING) | JTHREAD_FLAG_WOKEN;
            }
            arguments[0] = object;
            JNI_callMethod(thread, thread->method, arguments, 0);
            flags_after_call = thread->flags;
            if (flags_after_call & JTHREAD_FLAG_DONE) {
                thread->flags = flags_after_call & ~JTHREAD_FLAG_RUNNABLE;
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
    field = lookupClassField(classJava_xeno_Chr, loadConstString(jthreadPeerName, -1), 0);
    peer_slot = (JThreadChrPeer **)(object + field->offset);
    peer = *peer_slot;
    if (!(peer->flags & 0x2) && object != 0) {
        flags = thread->flags;
        if (flags & JTHREAD_FLAG_RUNNABLE) {
            if (flags & JTHREAD_FLAG_WAITING) {
                thread->flags = (flags & ~JTHREAD_FLAG_WAITING) | JTHREAD_FLAG_WOKEN;
            }
            arguments[0] = object;
            JNI_callMethod(thread, thread->method, arguments, 0);
            flags_after_call = thread->flags;
            if (flags_after_call & JTHREAD_FLAG_DONE) {
                thread->flags = flags_after_call & ~JTHREAD_FLAG_RUNNABLE;
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
    JavaMethod *method;
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
            if (flags & JTHREAD_FLAG_DONE) {
                thread->flags = flags & ~JTHREAD_FLAG_RUNNABLE;
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
    JavaMethod *method;
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
            if (flags & JTHREAD_FLAG_DONE) {
                thread->flags = flags & ~JTHREAD_FLAG_RUNNABLE;
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
    JavaClass *scene_class;
    JavaMethod *method;
    SceneObject arguments[4];
    int output;

    object = thread->object;
    stage_object = object->stage;
    if (stage_object != 0) {
        scene_class = ((ObjectHeader *)stage_object)->classPointer->clazz;
        method = thread->method;
        if (method != 0) {
            xglFontDebugPrintf(0, 8, jthreadDottedNameFormat,
                scene_class->name->bytes,
                method->name->bytes);
            arguments[0] = stage_object;
            JNI_callMethod(thread, method, arguments, &output);
        }
    }
}

void JTHREAD_init(void)
{
}

/*
 * Blocks the calling thread's script method until the condition its own
 * wait_kind names is satisfied, then clears flag 0x4 (and, for the actor
 * waits, 0x1 too) and sets flag 0x2 on it. Out-of-range wait_kind values do
 * nothing.
 */
void JTHREAD_waitFor(JThread *thread)
{
    switch (thread->wait_kind & JTHREAD_WAIT_KIND_MASK) {
    case JTHREAD_WAIT_FRAMES:
        thread->wait_target.elapsed_frames = thread->wait_target.elapsed_frames + 1;
        if (thread->wait_target.elapsed_frames >= thread->wait_parameter) {
            thread->flags = (thread->flags & ~JTHREAD_FLAG_CALL_RUNNING) | JTHREAD_FLAG_WOKEN;
        }
        break;

    case JTHREAD_WAIT_COMPONENT:
        if (((TComponent *)thread->wait_target.object)->state == 2) {
            thread->flags = (thread->flags & ~JTHREAD_FLAG_CALL_RUNNING) | JTHREAD_FLAG_WOKEN;
        }
        break;

    case JTHREAD_WAIT_THREAD:
    {
        JThread *other = thread->wait_target.thread;
        if (other->flags & JTHREAD_FLAG_DONE) {
            other->flags &= ~JTHREAD_FLAG_DONE;
            thread->flags = (thread->flags & ~JTHREAD_FLAG_CALL_RUNNING) | JTHREAD_FLAG_WOKEN;
        }
        break;
    }

    case JTHREAD_WAIT_ACTOR_IDLE:
        if (!(((Actor *)thread->wait_target.object)->flags & 0x2)) {
            thread->flags = (thread->flags & ~JTHREAD_FLAGS_BUSY) | JTHREAD_FLAG_WOKEN;
        }
        break;

    case JTHREAD_WAIT_ACTOR_IDLE_2:
        if (!(((Actor *)thread->wait_target.object)->flags & 0x2)) {
            thread->flags = (thread->flags & ~JTHREAD_FLAGS_BUSY) | JTHREAD_FLAG_WOKEN;
        }
        break;

    case JTHREAD_WAIT_SIGNAL:
        if (((Actor *)thread->wait_target.object)->signal ==
            thread->wait_parameter) {
            thread->flags = (thread->flags & ~JTHREAD_FLAG_CALL_RUNNING) | JTHREAD_FLAG_WOKEN;
        }
        break;

    case JTHREAD_WAIT_GAME:
        if (!(GameLoopState[0x10 / sizeof(unsigned int)] & 0x800)) {
            thread->flags = (thread->flags & ~JTHREAD_FLAGS_BUSY) | JTHREAD_FLAG_WOKEN;
        }
        break;
    }
}

void JTHREAD_info(void)
{
    JThread *thread = jthreadTop;

    if (thread != 0) {
        do {
            thread = thread->next;
        } while (thread != 0);
    }
}

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

void JTHREAD_cntl(void)
{
    JThread *node = jthreadTop;

    while (node != 0) {
        if ((node->flags & JTHREAD_FLAG_RUNNABLE) != 0 && node->kind != 0) {
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
JThread *JTHREAD_get(SceneObject object)
{
    JThread *node = jthreadTop;

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

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char JTHREADAlgUnitName[24] = "JTHREAD_ALG_UNIT";

const char JTHREADAlgDefaultName[24] = "JTHREAD_ALG_DEFAULT";

const char JTHREADAlgSceneName[24] = "JTHREAD_ALG_SCENE";

const char JTHREADAlgStageName[24] = "JTHREAD_ALG_STAGE";

const char JTHREADAlgChrName[16] = "JTHREAD_ALG_CHR";

const char JTHREADAlgSystemName[24] = "JTHREAD_ALG_SYSTEM";
