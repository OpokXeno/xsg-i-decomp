#include "common.h"
#include "chr.h"

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getPlayer__);

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

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_stop__);

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

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_signal__I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getSignal__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setRotate__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setTranslate__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getRotate__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getTranslate__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setVisible__Z);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setVisible__IZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setCollision__Z);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setHand__I);

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

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setArgs__III);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getArgs__II);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setArgs__ILjava_lang_Object_I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getSerial__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getState__);

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

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setScale__FFF);

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

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getFlags__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setFlags__I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setEdgeFall__I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setShadow__II);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setShadow__aB);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setID__I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setElevatorMode__I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setParent__Lxeno_Chr_III);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setMotionFlags__IZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setFilter__I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setFilterParam__aF);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setClip__I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setSymmetryY__I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setSortOffset__F);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setPointLightCol__IFFF);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setPointLightPos__IFFF);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setPointLightReset__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_talkto__Ljava_lang_String_);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_touchto__Ljava_lang_String_);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_childGetPeer__II);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setPeer__Ljava_lang_Object_);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_dispRadar__Z);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_look_camera__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_look_char__Ljava_lang_Object_);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_look_unit__Ljava_lang_Object_);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_look_default__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_look_point__FFF);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_look_eye_set__FF);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_look_eye_control__I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_look_speed__F);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_look_eye_speed__F);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_renderCommand__I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_shadow_clip_scale__F);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_shadow_map_id__I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_shadow_map_reset__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_hairStop__II);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_pixelAlpha__I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_pixelAlphaParts__II);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_pixelAlphaPartsReset__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setMotNoUpdate__I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setWeaponR__Lxeno_Chr_);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_resetWeaponR__Lxeno_Chr_);

void Java_xeno_Chr_resetHand__(void)
{
}

void Java_xeno_Chr_resetEnv__(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_ignoreShape__I);
