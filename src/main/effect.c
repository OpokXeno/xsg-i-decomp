#include "common.h"
#include "effect.h"

void Java_xeno_Effect_call__I(JThread *thread, EffectCommandCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    int id;
    EffectCallArgs *args;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC130, -1), 0);
        id = *(int *)(object + field->offset);
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC138, -1), 0);
        args = *(EffectCallArgs **)(object + field->offset);
        FX_call(arguments->command, id, args->data, args->length);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/effect", Java_xeno_Effect_disp__Z);

void Java_xeno_Effect_setScale__FFF(JThread *thread, EffectVectorCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        peer = *(NativeEffectPeer **)(object + field->offset);
        if (peer != 0) {
            peer->scale_x = arguments->x;
            peer->scale_y = arguments->y;
            peer->scale_z = arguments->z;
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/effect", Java_xeno_Effect_getScale__);

void Java_xeno_Effect_getTranslate__(JThread *thread, EffectCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    float *translate;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        translate = (*(NativeEffectPeer **)(object + field->offset))->translate;

        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC148, -1), 0);
        *(float *)(object + field->offset) = translate[0];
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC150, -1), 0);
        *(float *)(object + field->offset) = translate[1];
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC158, -1), 0);
        *(float *)(object + field->offset) = translate[2];
    }
}

void Java_xeno_Effect_setTranslate__(JThread *thread, EffectCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    float *translate;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        translate = (*(NativeEffectPeer **)(object + field->offset))->translate;

        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC148, -1), 0);
        translate[0] = *(float *)(object + field->offset);
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC150, -1), 0);
        translate[1] = *(float *)(object + field->offset);
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC158, -1), 0);
        translate[2] = *(float *)(object + field->offset);
    }
}

void Java_xeno_Effect_getRotate__(JThread *thread, EffectCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    float *rotate;
    float pi;

    object = arguments->object;
    if (object != 0) {
        pi = D_004D83BC;
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        rotate = (*(NativeEffectPeer **)(object + field->offset))->rotate;

        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC160, -1), 0);
        *(float *)(object + field->offset) = (rotate[0] / pi) * 180.0f;
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC168, -1), 0);
        *(float *)(object + field->offset) = (rotate[1] / pi) * 180.0f;
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC170, -1), 0);
        *(float *)(object + field->offset) = (rotate[2] / pi) * 180.0f;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/effect", Java_xeno_Effect_setRotate__);

void Java_xeno_Effect_setCaster__Lxeno_Chr_(JThread *thread, EffectChrCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        peer = *(NativeEffectPeer **)(object + field->offset);
        object = arguments->chr;
        field = lookupClassField(classJava_xeno_Chr, loadConstString(D_004DC140, -1), 0);
        peer->caster_chr = *(void **)(object + field->offset);
    }
}

void Java_xeno_Effect_setTarget__Lxeno_Chr_(JThread *thread, EffectChrCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        peer = *(NativeEffectPeer **)(object + field->offset);
        object = arguments->chr;
        field = lookupClassField(classJava_xeno_Chr, loadConstString(D_004DC140, -1), 0);
        peer->target_chr = *(void **)(object + field->offset);
    }
}

void Java_xeno_Effect_setCaster__Lxeno_Unit_(JThread *thread, EffectUnitCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;
    EffectUnitPeer *unit_peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        peer = *(NativeEffectPeer **)(object + field->offset);
        if (peer != 0) {
            object = arguments->unit;
            field = lookupClassField(classJava_xeno_Unit, loadConstString(D_004DC140, -1), 0);
            unit_peer = *(EffectUnitPeer **)(object + field->offset);
            if (unit_peer != 0) {
                peer->caster_unit = unit_peer;
                unit_peer->caster_effect = peer;
            }
        }
    }
}

void Java_xeno_Effect_setTarget__Lxeno_Unit_(JThread *thread, EffectUnitCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;
    EffectUnitPeer *unit_peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        peer = *(NativeEffectPeer **)(object + field->offset);
        if (peer != 0) {
            object = arguments->unit;
            field = lookupClassField(classJava_xeno_Unit, loadConstString(D_004DC140, -1), 0);
            unit_peer = *(EffectUnitPeer **)(object + field->offset);
            if (unit_peer != 0) {
                peer->target_unit = unit_peer;
            }
        }
    }
}

void Java_xeno_Effect_setTransOffset__FFF(JThread *thread, EffectVectorCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        peer = *(NativeEffectPeer **)(object + field->offset);
        if (peer != 0) {
            peer->translate[0] = peer->translate[0] + arguments->x;
            peer->translate[1] = peer->translate[1] + arguments->y;
            peer->translate[2] = peer->translate[2] + arguments->z;
        }
    }
}

void Java_xeno_Effect_getForceLoop__(JThread *thread, EffectCall *arguments, signed char *result)
{
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        peer = *(NativeEffectPeer **)(object + field->offset);
        if (peer != 0) {
            *result = peer->flags & 0x20;
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/effect", Java_xeno_Effect_setForceLoop__Z);

void Java_xeno_Effect_getClip__(JThread *thread, EffectCall *arguments, signed char *result)
{
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        peer = *(NativeEffectPeer **)(object + field->offset);
        if (peer != 0) {
            *result = peer->flags & 0x10;
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/effect", Java_xeno_Effect_setClip__Z);

void Java_xeno_Effect_clearEffect__(JThread *thread, EffectCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    void *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        peer = *(void **)(object + field->offset);
        if (peer != 0) {
            sefClearEffectCf(peer);
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/effect", Java_xeno_Effect_setMotion__Z);

INCLUDE_ASM("asm/main/nonmatchings/effect", Java_xeno_Effect_noAttach__Z);
