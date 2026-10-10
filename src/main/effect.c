#include "common.h"

#include "effect.h"

/* Runtime Java fields occupy one EE VM value word. The descriptor
 * supplies its byte offset; its declared Java/native kind selects
 * the integer, floating or reference representation below. No fixed
 * Java-object field positions are assumed. */
typedef union EffectFieldValue {
    NativeEffectPeer * effect;
    EffectUnitPeer * unit;
    EffectCallArgs * arguments;
    float floating;
    int integer;
    void * native_pointer;
} EffectFieldValue;


char D_004DC130[8] = "id";

char D_004DC138[8] = "args";

char D_004DC140[8] = "peer";

char D_004DC148[8] = "px";

char D_004DC150[8] = "py";

char D_004DC158[8] = "pz";

char D_004DC160[8] = "rx";

char D_004DC168[8] = "ry";

char D_004DC170[8] = "rz";

#define EFFECT_PI 3.1415927f

struct UnitScaleVector {
    SceneObjectClassRef *class_ref;
    float z;
    float y;
    float x;
    float w;
    unsigned char unmodeled_14[4];
};

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

void Java_xeno_Effect_disp__Z(JThread *thread, EffectBooleanCall *arguments)
{
    EffectFieldValue *field_value;
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            if (arguments->value != 0) {
                sefRewindEffectCf((SchedulerState *)peer);
                peer->display = 1;
            } else {
                peer->display = 0;
            }
        }
    }
}

void Java_xeno_Effect_setScale__FFF(JThread *thread, EffectVectorCall *arguments)
{
    EffectFieldValue *field_value;
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            peer->scale_x = arguments->x;
            peer->scale_y = arguments->y;
            peer->scale_z = arguments->z;
        }
    }
}

void Java_xeno_Effect_getScale__(JThread *thread, EffectCall *arguments, unsigned int *result)
{
    EffectFieldValue *field_value;
    /* This native returns a Vector4f object reference through the VM result
     * word. The static object retains its normal Java class-reference header. */
    static struct UnitScaleVector scale;
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;
    SceneObject object_result;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        scale.class_ref = classJava_xeno_util_Vector4f->instance_class_ref;
        scale.x = peer->scale_x;
        scale.y = peer->scale_y;
        scale.z = peer->scale_z;
        scale.w = peer->scale_w;
        object_result = (SceneObject)&scale;
        /* Store the object-pointer representation in the raw VM result slot. */
        __builtin_memcpy(result, &object_result, sizeof(object_result));
    }
}

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
        pi = EFFECT_PI;
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

void Java_xeno_Effect_setRotate__(JThread *thread, EffectCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;
    float *rotate;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        peer = *(NativeEffectPeer **)(object + field->offset);
        rotate = peer->rotate;

        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC160, -1), 0);
        rotate[0] = (*(float *)(object + field->offset) / 180.0f) * EFFECT_PI;
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC168, -1), 0);
        rotate[1] = (*(float *)(object + field->offset) / 180.0f) * EFFECT_PI;
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC170, -1), 0);
        rotate[2] = (*(float *)(object + field->offset) / 180.0f) * EFFECT_PI;
    }
}

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
    EffectFieldValue *field_value;
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            peer->translate[0] = peer->translate[0] + arguments->x;
            peer->translate[1] = peer->translate[1] + arguments->y;
            peer->translate[2] = peer->translate[2] + arguments->z;
        }
    }
}

void Java_xeno_Effect_getForceLoop__(JThread *thread, EffectCall *arguments, signed char *result)
{
    EffectFieldValue *field_value;
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            *result = peer->flags & 0x20;
        }
    }
}

void Java_xeno_Effect_setForceLoop__Z(JThread *thread, EffectBooleanCall *arguments)
{
    EffectFieldValue *field_value;
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            if (arguments->value == 1) {
                peer->flags |= 0x20;
            } else {
                peer->flags &= ~0x20U;
            }
        }
    }
}

void Java_xeno_Effect_getClip__(JThread *thread, EffectCall *arguments, signed char *result)
{
    EffectFieldValue *field_value;
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            *result = peer->flags & 0x10;
        }
    }
}

void Java_xeno_Effect_setClip__Z(JThread *thread, EffectBooleanCall *arguments)
{
    EffectFieldValue *field_value;
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            if (arguments->value == 1) {
                peer->flags |= 0x10;
            } else {
                peer->flags &= ~0x10U;
            }
        }
    }
}

void Java_xeno_Effect_clearEffect__(JThread *thread, EffectCall *arguments)
{
    EffectFieldValue *field_value;
    unsigned char *object;
    JavaField *field;
    void *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->native_pointer;
        if (peer != 0) {
            sefClearEffectCf(peer);
        }
    }
}

void Java_xeno_Effect_setMotion__Z(JThread *thread, EffectBooleanCall *arguments)
{
    EffectFieldValue *field_value;
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            if (arguments->value == 1) {
                peer->flags |= 0x80;
            } else {
                peer->flags &= ~0x80U;
            }
        }
    }
}

void Java_xeno_Effect_noAttach__Z(JThread *thread, EffectBooleanCall *arguments)
{
    EffectFieldValue *field_value;
    unsigned char *object;
    JavaField *field;
    NativeEffectPeer *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(D_004DC140, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            if (arguments->value == 1) {
                peer->flags |= 0x400;
            } else {
                peer->flags &= ~0x400U;
            }
        }
    }
}
