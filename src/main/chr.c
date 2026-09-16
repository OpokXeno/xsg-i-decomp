#include "common.h"
#include "chr.h"

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getPlayer__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setPlayer__);

INCLUDE_ASM("asm/main/nonmatchings/chr", CHR_moveXZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_move__IFFZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_move__FFFZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_move__ILjava_lang_Object_Z);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_move__Ljava_lang_Object_FZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_move__Lxeno_util_Spline_IZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotate__Lxeno_util_Spline_IZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", CHR_rotX);

INCLUDE_ASM("asm/main/nonmatchings/chr", CHR_rotY);

INCLUDE_ASM("asm/main/nonmatchings/chr", CHR_rotZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotX__FFZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotX__IFZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotX__ILjava_lang_Object_Z);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotX__Ljava_lang_Object_FZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotY__FFZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotY__IFZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotY__ILjava_lang_Object_Z);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotY__Ljava_lang_Object_FZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotZ__FFZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotZ__IFZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotZ__ILjava_lang_Object_Z);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotZ__Ljava_lang_Object_FZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_start__ILjava_lang_Object_);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_stop__);

INCLUDE_ASM("asm/main/nonmatchings/chr", CHR_motion);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_mtn__IIIIIFZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_mtn__IIFZ);

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

INCLUDE_ASM("asm/main/nonmatchings/chr", copyArgs_002FEFB0);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setArgs__III);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getArgs__II);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setArgs__ILjava_lang_Object_I);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getSerial__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_getState__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_mtnGetRoot__ILxeno_util_Vector4f_);

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

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_sclZ__FFZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_sclZ__IFZ);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotCNS__ILjava_lang_Object_);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_rotCNS__IFFF);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_setRotCNSParam__IFFFFF);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_relax__II);

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

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_resetHand__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_resetEnv__);

INCLUDE_ASM("asm/main/nonmatchings/chr", Java_xeno_Chr_ignoreShape__I);
