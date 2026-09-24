#include "common.h"
#include "chr.h"

void Java_xeno_Chr_getPlayer__(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    u8 *object;

    /*
     * Unlike every other Chr native here, this one does not chase through
     * the peer pointer's target: it stores GameLoopState's own +0x4 word
     * directly into the Java object's `peer` field (0x002fcd28..0x002fcd34).
     */
    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    *(unsigned int *)(object + peer_field->offset) = GameLoopState[1];
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setPlayer__);

INCLUDE_ASM("asm/main/nonmatchings/chr", CHR_moveXZ);

/*
 * CHR_moveXZ is local to this file in the original (glabel ..., local); it
 * stays asm here but needs a prototype for the trampolines below to call.
 * Overload -> mode: IFFZ 0, FFFZ 1, I,Object,Z 2, Object,F,Z 3.
 */
static void CHR_moveXZ(int mode, JThread *thread, ChrMoveCall *arguments,
                       u32 *failure_result);

void Java_xeno_Chr_move__IFFZ(JThread *thread, ChrMoveCall *arguments,
                              u32 *failure_result)
{
    CHR_moveXZ(0, thread, arguments, failure_result);
}

void Java_xeno_Chr_move__FFFZ(JThread *thread, ChrMoveCall *arguments,
                              u32 *failure_result)
{
    CHR_moveXZ(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_move__ILjava_lang_Object_Z(JThread *thread,
                                              ChrMoveCall *arguments,
                                              u32 *failure_result)
{
    CHR_moveXZ(2, thread, arguments, failure_result);
}

void Java_xeno_Chr_move__Ljava_lang_Object_FZ(JThread *thread,
                                              ChrMoveCall *arguments,
                                              u32 *failure_result)
{
    CHR_moveXZ(3, thread, arguments, failure_result);
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_move__Lxeno_util_Spline_IZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotate__Lxeno_util_Spline_IZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", CHR_rotX);

INCLUDE_ASM("asm/main/nonmatchings/chr", CHR_rotY);

INCLUDE_ASM("asm/main/nonmatchings/chr", CHR_rotZ);

/*
 * CHR_rotX/CHR_rotY/CHR_rotZ are local to this file in the original (glabel
 * ..., local); they stay asm here but need a prototype for the trampolines
 * below to call. Overload -> mode: FFZ 1, IFZ 0, I,Object,Z 2, Object,F,Z 3
 * (same as CHR_sclX).
 */
static void CHR_rotX(int mode, JThread *thread, ChrRotCall *arguments,
                     u32 *failure_result);
static void CHR_rotY(int mode, JThread *thread, ChrRotCall *arguments,
                     u32 *failure_result);
static void CHR_rotZ(int mode, JThread *thread, ChrRotCall *arguments,
                     u32 *failure_result);

void Java_xeno_Chr_rotX__FFZ(JThread *thread, ChrRotCall *arguments,
                             u32 *failure_result)
{
    CHR_rotX(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotX__IFZ(JThread *thread, ChrRotCall *arguments,
                             u32 *failure_result)
{
    CHR_rotX(0, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotX__ILjava_lang_Object_Z(JThread *thread,
                                              ChrRotCall *arguments,
                                              u32 *failure_result)
{
    CHR_rotX(2, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotX__Ljava_lang_Object_FZ(JThread *thread,
                                              ChrRotCall *arguments,
                                              u32 *failure_result)
{
    CHR_rotX(3, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotY__FFZ(JThread *thread, ChrRotCall *arguments,
                             u32 *failure_result)
{
    CHR_rotY(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotY__IFZ(JThread *thread, ChrRotCall *arguments,
                             u32 *failure_result)
{
    CHR_rotY(0, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotY__ILjava_lang_Object_Z(JThread *thread,
                                              ChrRotCall *arguments,
                                              u32 *failure_result)
{
    CHR_rotY(2, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotY__Ljava_lang_Object_FZ(JThread *thread,
                                              ChrRotCall *arguments,
                                              u32 *failure_result)
{
    CHR_rotY(3, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotZ__FFZ(JThread *thread, ChrRotCall *arguments,
                             u32 *failure_result)
{
    CHR_rotZ(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotZ__IFZ(JThread *thread, ChrRotCall *arguments,
                             u32 *failure_result)
{
    CHR_rotZ(0, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotZ__ILjava_lang_Object_Z(JThread *thread,
                                              ChrRotCall *arguments,
                                              u32 *failure_result)
{
    CHR_rotZ(2, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotZ__Ljava_lang_Object_FZ(JThread *thread,
                                              ChrRotCall *arguments,
                                              u32 *failure_result)
{
    CHR_rotZ(3, thread, arguments, failure_result);
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_start__ILjava_lang_Object_);

/*
 * JTHREAD_get is local to jthread.c (still asm there) and only that TU
 * declares its own SceneThread-typed view; this TU needs its own prototype
 * over JThread, the type it already completes above (jal JTHREAD_get at
 * 0x002fe49c).
 */
extern JThread *JTHREAD_get(void *object);

void Java_xeno_Chr_stop__(JThread *thread, ChrObjectCall *arguments,
                          u32 *failure_result)
{
    JavaField *field;
    Actor *peer;
    JThread *chr_thread;
    u8 *object;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + field->offset);
    chr_thread = JTHREAD_get(object);
    if (chr_thread != 0) {
        chr_thread->flags &= ~0x10;
    }
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(D_004DC1A0, -1), 0);
    *(float *)(object + field->offset) = peer->position.x;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(D_004DC1A8, -1), 0);
    *(float *)(object + field->offset) = peer->position.y;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(D_004DC1B0, -1), 0);
    *(float *)(object + field->offset) = peer->position.z;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(D_004DC1B8, -1), 0);
    *(float *)(object + field->offset) = peer->rotation.x;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(D_004DC1C0, -1), 0);
    *(float *)(object + field->offset) = peer->rotation.y;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(D_004DC1C8, -1), 0);
    *(float *)(object + field->offset) = peer->rotation.z;
}

INCLUDE_ASM("asm/main/nonmatchings/chr", CHR_motion);

/*
 * CHR_motion is local to this file in the original (glabel ..., local); it
 * stays asm here but needs a prototype for the trampolines below to call.
 * Overload -> mode: IIIIIFZ 1, IIFZ 0.
 */
static void CHR_motion(int mode, JThread *thread, ChrMotionCall *arguments,
                       u32 *failure_result);

void Java_xeno_Chr_mtn__IIIIIFZ(JThread *thread, ChrMotionCall *arguments,
                                u32 *failure_result)
{
    CHR_motion(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_mtn__IIFZ(JThread *thread, ChrMotionCall *arguments,
                             u32 *failure_result)
{
    CHR_motion(0, thread, arguments, failure_result);
}

void Java_xeno_Chr_signal__I(JThread *thread, ChrSignalCall *arguments,
                             u32 *failure_result)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->signal = arguments->value;
}

void Java_xeno_Chr_getSignal__(JThread *thread, ChrObjectCall *arguments,
                               u32 *failure_result)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    *failure_result = peer->signal;
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setRotate__);

/*
 * The state byte at Actor+0xa40, past this TU's recovered head above:
 * growing the struct there would move data_header and every field after it
 * that other accepted functions in this file already read at their current
 * offsets, so this one narrowly evidenced access stays a byte view, exactly
 * as ACTOR_SLOT_NUMBER_OFFSET above does for +0x80. Only
 * Java_xeno_Chr_setTranslate__ reads or writes it here: it tests bit 0 and,
 * when set, stores the literal 2 back (lbu/andi/sb at
 * 0x002feb9c-0x002febb0).
 */
#define ACTOR_TRANSLATE_STATE_OFFSET 0xa40

void Java_xeno_Chr_setTranslate__(JThread *thread, ChrObjectCall *arguments)
{
    JavaField *field;
    Actor *peer;
    Vector4 *position;
    u8 *object;
    float z;

    object = arguments->object;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + field->offset);
    position = &peer->position;

    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(D_004DC1A0, -1), 0);
    position->x = *(float *)(object + field->offset);
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(D_004DC1A8, -1), 0);
    position->y = *(float *)(object + field->offset);
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(D_004DC1B0, -1), 0);
    z = *(float *)(object + field->offset);

    peer->status_flags |= 0x1000;
    position->z = z;
    peer->translate_y = position->y;
    if (((u8 *)peer)[ACTOR_TRANSLATE_STATE_OFFSET] & 1) {
        ((u8 *)peer)[ACTOR_TRANSLATE_STATE_OFFSET] = 2;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getRotate__);

void Java_xeno_Chr_getTranslate__(JThread *thread, ChrScaleCall *arguments,
                                  u32 *failure_result)
{
    JavaField *field;
    Actor *peer;
    Vector4 *position;
    u8 *object;

    /* Each Java field has a runtime class offset, rather than a fixed C member. */
    object = arguments->object;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + field->offset);
    position = &peer->position;

    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(D_004DC1A0, -1), 0);
    *(float *)(object + field->offset) = position->x;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(D_004DC1A8, -1), 0);
    *(float *)(object + field->offset) = position->y;
    field = lookupClassField(classJava_xeno_Chr,
                             loadConstString(D_004DC1B0, -1), 0);
    *(float *)(object + field->offset) = position->z;
}

void Java_xeno_Chr_setVisible__Z(JThread *thread, ChrBoolCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    if (arguments->flag != 0) {
        peer->flags &= ~8;
    } else {
        peer->flags |= 8;
    }
}

void Java_xeno_Chr_setVisible__IZ(JThread *thread, ChrVisibleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    ACT_setVisible(peer, arguments->part, arguments->visible);
}

void Java_xeno_Chr_setCollision__Z(JThread *thread, ChrBoolCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    if (arguments->flag != 0) {
        peer->flags |= 0x40;
    } else {
        peer->flags &= ~0x40;
    }
}

void Java_xeno_Chr_setHand__I(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    ACT_setHand(peer, arguments->first.integer);
}

static void copyArgs(u8 *dst, u8 *src, int count)
{
    u8 byte;

    if (count > 0) {
        count--;
        if (count >= 0) {
            do {
                byte = *src;
                src++;
                count--;
                *dst = byte;
                dst++;
            } while (count >= 0);
        }
    }
}

void Java_xeno_Chr_setArgs__III(JThread *thread, ChrArgsWordCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    int value;
    u8 *object;
    int offset;
    int size;

    value = arguments->value;
    object = arguments->object;
    offset = arguments->offset;
    size = arguments->size;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    if ((unsigned int)(size - 1) < 4U) {
        copyArgs(peer->args + offset, (u8 *)&value, size);
    }
}

void Java_xeno_Chr_getArgs__II(JThread *thread, ChrArgsReadCall *arguments,
                               int *result)
{
    JavaField *peer_field;
    Actor *peer;
    int value;
    u8 *object;
    int offset;
    int size;

    object = arguments->object;
    offset = arguments->offset;
    size = arguments->size;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    if ((unsigned int)(size - 1) < 4U) {
        copyArgs((u8 *)&value, peer->args + offset, size);
        *result = value;
    }
}

void Java_xeno_Chr_setArgs__ILjava_lang_Object_I(JThread *thread,
                                                 ChrArgsObjectCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    ChrScaleCall *source;
    int offset;
    u8 *object;
    int second;
    int first;

    source = arguments->source;
    offset = arguments->offset;
    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    second = source->second.integer;
    peer = *(Actor **)(object + peer_field->offset);
    first = source->first.integer;
    *(int *)(peer->args + offset) = second;
    *(int *)(peer->args + offset + 4) = first;
}

void Java_xeno_Chr_getSerial__(JThread *thread, ChrScaleCall *arguments,
                               unsigned int *result)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    result[0] = ((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET];
}

void Java_xeno_Chr_getState__(JThread *thread, ChrObjectCall *arguments,
                              u32 *failure_result)
{
    JavaField *peer_field;
    Actor *peer;
    SequenceState *entry;
    unsigned char slot;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    slot = ((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET];
    entry = (SequenceState *)(actSequence + slot * SEQUENCE_STRIDE);
    *failure_result = entry->state_flags;
}

/*
 * Unimplemented native: it looks up the peer field descriptor the same way
 * every other Chr native does, but never uses arguments or the descriptor
 * to read the motion root, and never writes to failure_result.
 */
void Java_xeno_Chr_mtnGetRoot__ILxeno_util_Vector4f_(JThread *thread,
                                                     ChrScaleCall *arguments,
                                                     u32 *failure_result)
{
    /*
     * The original calls lookupClassField with jal and returns through the
     * shared epilogue instead of a sibling jump. Under this TU's compiler
     * the call, whose result is discarded, stays out of tail position only
     * inside a loop construct, which is the shape a do/while (0) statement
     * gives it.
     */
    do {
        lookupClassField(classJava_xeno_Chr,
                         loadConstString(chr_peer_string, -1), 0);
    } while (0);
}

void Java_xeno_Chr_setScale__FFF(JThread *thread, ChrVector3Call *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->scale.x = arguments->x;
    peer->scale.y = arguments->y;
    peer->scale.z = arguments->z;
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getScale__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_scale__Lxeno_util_Spline_IZ);

static void CHR_sclX(int mode, JThread *thread, ChrScaleCall *arguments,
                     u32 *failure_result)
{
    JavaField *peer_field;
    JavaField *algorithm_field;
    Actor *peer;
    float *scale_source;
    SequenceState *sequence;
    SequenceScale *scale;
    u8 *object;
    int wait;

    wait = 0;
    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    /*
     * `peer` and `algorithm` are fields of the Java class xeno.Chr, so their
     * position inside the object is the offset the JVM hands back at run time
     * and not a layout this file can name. The original loads it the same way:
     * lw v1,0x10(v0); addu v1,s3,v1; lw s2,0x0(v1) at 0x002ff5b0.
     */
    peer = *(Actor **)(object + peer_field->offset);
    algorithm_field = lookupClassField(
        classJava_xeno_Chr, loadConstString(chr_algorithm_string, -1), 0);

    scale_source = &peer->scale.x;
    if ((*(u32 *)(object + algorithm_field->offset) & 1) == 0)
        scale_source = defaultOffset;

    sequence = (SequenceState *)(actSequence +
        ((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET] * SEQUENCE_STRIDE);
    SEQUENCE_SCALE_HANDLER(sequence) = SEQ_scale;
    sequence->state_flags |= 0x20;
    scale = (SequenceScale *)((u8 *)sequence + SEQUENCE_SCALE_OFFSET);

    switch (mode) {
    case 0:
        scale->frames[0] = arguments->first.integer;
        scale->target[0] = arguments->second.floating + scale_source[0];
        wait = arguments->wait;
        break;
    case 1:
        scale->frames[0] = -1;
        scale->target[0] = arguments->first.floating + scale_source[0];
        scale->step[0] = arguments->second.floating / 30.0f;
        wait = arguments->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= 2;
        /*
         * `wait` is 1 here, and kind 1 is the thread getPeer_Chr creates for a
         * character (0x002f96e0): a blocking sclX only suspends the
         * character's own thread, or a thread carrying flag 0x40. The original
         * compares the two registers (beq v1,s4 at 0x002ff6c0).
         */
        if (thread->kind != wait && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target = peer;
            thread->wait_kind = JTHREAD_WAIT_ACTOR;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

void Java_xeno_Chr_sclX__FFZ(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    CHR_sclX(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_sclX__IFZ(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    CHR_sclX(0, thread, arguments, failure_result);
}

static void CHR_sclY(int mode, JThread *thread, ChrScaleCall *arguments,
                     u32 *failure_result)
{
    JavaField *peer_field;
    JavaField *algorithm_field;
    Actor *peer;
    float *scale_source;
    SequenceState *sequence;
    SequenceScale *scale;
    u8 *object;
    int wait;

    wait = 0;
    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    /*
     * `peer` and `algorithm` are fields of the Java class xeno.Chr, so their
     * position inside the object is the offset the JVM hands back at run time
     * and not a layout this file can name. The original loads it the same way:
     * lw v1,0x10(v0); addu v1,s3,v1; lw s2,0x0(v1) at 0x002ff5b0.
     */
    peer = *(Actor **)(object + peer_field->offset);
    algorithm_field = lookupClassField(
        classJava_xeno_Chr, loadConstString(chr_algorithm_string, -1), 0);

    scale_source = &peer->scale.x;
    if ((*(u32 *)(object + algorithm_field->offset) & 1) == 0)
        scale_source = defaultOffset;

    sequence = (SequenceState *)(actSequence +
        ((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET] * SEQUENCE_STRIDE);
    SEQUENCE_SCALE_HANDLER(sequence) = SEQ_scale;
    sequence->state_flags |= 0x40;
    scale = (SequenceScale *)((u8 *)sequence + SEQUENCE_SCALE_OFFSET);

    switch (mode) {
    case 0:
        scale->frames[1] = arguments->first.integer;
        scale->target[1] = arguments->second.floating + scale_source[1];
        wait = arguments->wait;
        break;
    case 1:
        scale->frames[1] = -1;
        scale->target[1] = arguments->first.floating + scale_source[1];
        scale->step[1] = arguments->second.floating / 30.0f;
        wait = arguments->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= 2;
        /*
         * `wait` is 1 here, and kind 1 is the thread getPeer_Chr creates for a
         * character (0x002f96e0): a blocking sclY only suspends the
         * character's own thread, or a thread carrying flag 0x40. The original
         * compares the two registers (beq v1,s4 at 0x002ff6c0).
         */
        if (thread->kind != wait && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target = peer;
            thread->wait_kind = JTHREAD_WAIT_ACTOR;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

void Java_xeno_Chr_sclY__FFZ(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    CHR_sclY(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_sclY__IFZ(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    CHR_sclY(0, thread, arguments, failure_result);
}

static void CHR_sclZ(int mode, JThread *thread, ChrScaleCall *arguments,
                     u32 *failure_result)
{
    JavaField *peer_field;
    JavaField *algorithm_field;
    Actor *peer;
    float *scale_source;
    SequenceState *sequence;
    SequenceScale *scale;
    u8 *object;
    int wait;

    wait = 0;
    object = arguments->object;
    if (JNI_isInstanceOf(object, classJava_xeno_Chr) == 0) {
        *failure_result = 0;
        return;
    }

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    /*
     * `peer` and `algorithm` are fields of the Java class xeno.Chr, so their
     * position inside the object is the offset the JVM hands back at run time
     * and not a layout this file can name. The original loads it the same way:
     * lw v1,0x10(v0); addu v1,s3,v1; lw s2,0x0(v1) at 0x002ff5b0.
     */
    peer = *(Actor **)(object + peer_field->offset);
    algorithm_field = lookupClassField(
        classJava_xeno_Chr, loadConstString(chr_algorithm_string, -1), 0);

    scale_source = &peer->scale.x;
    if ((*(u32 *)(object + algorithm_field->offset) & 1) == 0)
        scale_source = defaultOffset;

    sequence = (SequenceState *)(actSequence +
        ((u8 *)peer)[ACTOR_SLOT_NUMBER_OFFSET] * SEQUENCE_STRIDE);
    SEQUENCE_SCALE_HANDLER(sequence) = SEQ_scale;
    sequence->state_flags |= 0x80;
    scale = (SequenceScale *)((u8 *)sequence + SEQUENCE_SCALE_OFFSET);

    switch (mode) {
    case 0:
        scale->frames[2] = arguments->first.integer;
        scale->target[2] = arguments->second.floating + scale_source[2];
        wait = arguments->wait;
        break;
    case 1:
        scale->frames[2] = -1;
        scale->target[2] = arguments->first.floating + scale_source[2];
        scale->step[2] = arguments->second.floating / 30.0f;
        wait = arguments->wait;
        break;
    }

    if (wait == 1) {
        peer->flags |= 2;
        /*
         * `wait` is 1 here, and kind 1 is the thread getPeer_Chr creates for a
         * character (0x002f96e0): a blocking sclZ only suspends the
         * character's own thread, or a thread carrying flag 0x40. The original
         * compares the two registers (beq v1,s4 at 0x002ff6c0).
         */
        if (thread->kind != wait && (thread->flags & 0x40) == 0)
            return;
        if ((thread->flags & 0x40) != 0) {
            thread->wait_target = peer;
            thread->wait_kind = JTHREAD_WAIT_ACTOR;
            thread->resume_frames = thread->frame_depth;
            thread->flags |= 4;
        }
        thread->resume_frames = thread->frame_depth;
        thread->flags |= 1;
    }
}

void Java_xeno_Chr_sclZ__FFZ(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    CHR_sclZ(1, thread, arguments, failure_result);
}

/*
 * The IFZ overload forwards the same interpolation mode 1 as FFZ (addiu
 * a0,zero,1 at 0x002ffc74): the original does not give this overload its own
 * mode 0 dispatch the way CHR_sclX/CHR_sclY do.
 */
void Java_xeno_Chr_sclZ__IFZ(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    CHR_sclZ(1, thread, arguments, failure_result);
}

void Java_xeno_Chr_rotCNS__ILjava_lang_Object_(JThread *thread,
                                               ChrScaleCall *arguments,
                                               u32 *failure_result)
{
}

/*
 * Unimplemented native: it fetches its peer the same way every other Chr
 * native does (peer_field then peer) and does nothing else with it.
 */
void Java_xeno_Chr_rotCNS__IFFF(JThread *thread, ChrScaleCall *arguments,
                                u32 *failure_result)
{
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(arguments->object + peer_field->offset);
}

/*
 * Unimplemented native: it fetches its peer the same way every other Chr
 * native does (peer_field then peer) and does nothing else with it.
 */
void Java_xeno_Chr_setRotCNSParam__IFFFFF(JThread *thread,
                                          ChrScaleCall *arguments,
                                          u32 *failure_result)
{
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(arguments->object + peer_field->offset);
}

/*
 * Unimplemented native: it fetches its peer the same way every other Chr
 * native does (peer_field then peer) and does nothing else with it.
 */
void Java_xeno_Chr_relax__II(JThread *thread, ChrScaleCall *arguments,
                             u32 *failure_result)
{
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(arguments->object + peer_field->offset);
}

void Java_xeno_Chr_getFlags__(JThread *thread, ChrScaleCall *arguments,
                              unsigned int *result)
{
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(arguments->object + peer_field->offset);
    result[0] = peer->flags;
}

void Java_xeno_Chr_setFlags__I(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(arguments->object + peer_field->offset);
    peer->flags = arguments->first.integer;
}

void Java_xeno_Chr_setEdgeFall__I(JThread *thread, ChrIntCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(arguments->object + peer_field->offset);
    if (arguments->value == 0) {
        /* Also clears every other status_flags bit: andi v0,v0,8 at
         * 0x002ffe48, not a mask of ~8. */
        peer->status_flags &= 8;
    } else {
        peer->status_flags |= 8;
    }
}

void Java_xeno_Chr_setShadow__II(JThread *thread, ChrShadowCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(arguments->object + peer_field->offset);
    peer->flags |= 0x20;
    peer->shadow_kind = arguments->kind;
    peer->shadow_size = arguments->size;
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setShadow__aB);

void Java_xeno_Chr_setID__I(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(arguments->object + peer_field->offset);
    peer->data_header = UnduDataGetHeader(0, arguments->first.integer);
    peer->status_flags |= 0x1000;
}

void Java_xeno_Chr_setElevatorMode__I(JThread *thread, ChrIntCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;

    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(arguments->object + peer_field->offset);
    if (arguments->value == 0) {
        /* Also clears every other status_flags bit: andi v0,v0,0x20 at
         * 0x003000f8, not a mask of ~0x20. */
        peer->status_flags &= 0x20;
    } else {
        peer->status_flags |= 0x20;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setParent__Lxeno_Chr_III);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setMotionFlags__IZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setFilter__I);

void Java_xeno_Chr_setFilterParam__aF(JThread *thread,
                                      ChrFilterParamCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    ChrFilterParamValue *value;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    value = arguments->array->value;
    peer->filter_param[0] = value->components[0];
    peer->filter_param[1] = value->components[3];
    peer->filter_param[2] = value->components[2];
    peer->filter_param[3] = value->components[1];
}

void Java_xeno_Chr_setClip__I(JThread *thread, ChrIntCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    if (arguments->value != 0) {
        peer->render_flags |= 0x200;
    } else {
        peer->render_flags &= ~0x200;
    }
}

void Java_xeno_Chr_setSymmetryY__I(JThread *thread, ChrIntCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    if (arguments->value != 0) {
        peer->render_flags |= 0x10;
    } else {
        peer->render_flags &= ~0x10;
    }
}

void Java_xeno_Chr_setSortOffset__F(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->sort_offset = arguments->first.floating;
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setPointLightCol__IFFF);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setPointLightPos__IFFF);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setPointLightReset__);

void Java_xeno_Chr_talkto__Ljava_lang_String_(JThread *thread,
                                              ChrTalkCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->talk_message = arguments->message->value->message_id;
}

void Java_xeno_Chr_touchto__Ljava_lang_String_(JThread *thread,
                                               ChrTalkCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->touch_message = arguments->message->value->message_id;
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_childGetPeer__II);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setPeer__Ljava_lang_Object_);

void Java_xeno_Chr_dispRadar__Z(JThread *thread, ChrBoolCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    if (arguments->flag != 0) {
        peer->flags &= ~0x80;
    } else {
        peer->flags |= 0x80;
    }
}

void Java_xeno_Chr_look_camera__(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->look_mode = 2;
}

void Java_xeno_Chr_look_char__Ljava_lang_Object_(JThread *thread,
                                                 ChrWeaponCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    Actor *target;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    target = *(Actor **)(arguments->other + peer_field->offset);
    peer->look_mode = 4;
    peer->look_target = target;
}

void Java_xeno_Chr_look_unit__Ljava_lang_Object_(JThread *thread,
                                                 ChrWeaponCall *arguments)
{
    JavaField *peer_field;
    JavaField *unit_peer_field;
    Actor *peer;
    void *target;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    unit_peer_field = lookupClassField(classJava_xeno_Unit,
                                       loadConstString(chr_peer_string, -1), 0);
    target = *(void **)(arguments->other + unit_peer_field->offset);
    peer->look_mode = 5;
    peer->look_target = target;
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_look_default__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_look_point__FFF);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_look_eye_set__FF);

void Java_xeno_Chr_look_eye_control__I(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->look_eye_control = arguments->first.integer;
}

void Java_xeno_Chr_look_speed__F(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->look_speed = arguments->first.floating;
}

void Java_xeno_Chr_look_eye_speed__F(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->look_eye_speed = arguments->first.floating;
}

void Java_xeno_Chr_renderCommand__I(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->render_command = arguments->first.integer;
}

void Java_xeno_Chr_shadow_clip_scale__F(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->shadow_clip_scale = arguments->first.floating;
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_shadow_map_id__I);

void Java_xeno_Chr_shadow_map_reset__(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->shadow_map = 0;
}

void Java_xeno_Chr_hairStop__II(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->hair_stop_a = arguments->first.integer;
    peer->hair_stop_b = arguments->second.integer;
}

void Java_xeno_Chr_pixelAlpha__I(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->pixel_alpha = arguments->first.integer;
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_pixelAlphaParts__II);

void Java_xeno_Chr_pixelAlphaPartsReset__(JThread *thread, ChrScaleCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    peer->pixel_alpha_parts = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setMotNoUpdate__I);

void Java_xeno_Chr_setWeaponR__Lxeno_Chr_(JThread *thread, ChrWeaponCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    Actor *other_peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    object = arguments->other;
    other_peer = *(Actor **)(object + peer_field->offset);
    ACT_setArms(peer, other_peer, 0x108, 2);
}

void Java_xeno_Chr_resetWeaponR__Lxeno_Chr_(JThread *thread, ChrWeaponCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    Actor *other_peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    object = arguments->other;
    other_peer = *(Actor **)(object + peer_field->offset);
    ACT_resetArms(peer, other_peer, 0x108);
}

void Java_xeno_Chr_resetHand__(void)
{
}

void Java_xeno_Chr_resetEnv__(void)
{
}

void Java_xeno_Chr_ignoreShape__I(JThread *thread, ChrIntCall *arguments)
{
    JavaField *peer_field;
    Actor *peer;
    u8 *object;

    object = arguments->object;
    peer_field = lookupClassField(classJava_xeno_Chr,
                                  loadConstString(chr_peer_string, -1), 0);
    peer = *(Actor **)(object + peer_field->offset);
    if (arguments->value != 0) {
        peer->render_flags |= 0x400;
    } else {
        peer->render_flags &= ~0x400;
    }
}
