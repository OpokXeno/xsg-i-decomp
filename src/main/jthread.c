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

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_default);

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_defaultScene);

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_defaultStage);

void JTHREAD_init(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_waitFor);

/*
 * This TU's own view of the VM thread list node that src/main/chr.h (main's
 * chr TU) defines in full as `JThread`; only the `next` link this loop walks
 * is named here (chr.h's `previous` at +0x04), and the rest stays unmodeled.
 */
typedef struct JThreadNode {
    unsigned char unmodeled_00[8];
    struct JThreadNode *next;           /* +0x08 */
} JThreadNode;

extern JThreadNode *jthreadTop;

void JTHREAD_info(void)
{
    JThreadNode *thread = jthreadTop;

    if (thread != 0) {
        do {
            thread = thread->next;
        } while (thread != 0);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_getSymbol);

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_cntl);

INCLUDE_ASM("asm/main/nonmatchings/jthread", JTHREAD_get);
