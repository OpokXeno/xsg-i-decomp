#include "common.h"
#include "effect.h"
#include "main/jni.h"
#include "main/unit.h"

#define EFFECT_PI 3.1415927f

/* Runtime Java fields occupy one EE VM value word. The descriptor
 * supplies its byte offset; its declared Java/native kind selects
 * the integer, floating or reference representation below. No fixed
 * Java-object field positions are assumed. */
typedef union EffectFieldValue {
    SchedulerState * effect;
    MapUnitRecord * unit;
    EffectCallArgs * arguments;
    float floating;
    int integer;
} EffectFieldValue;

char effectIdName[8] = "id";

char effectArgsName[8] = "args";

char effectPeerName[8] = "peer";

char effectPositionXName[8] = "px";

char effectPositionYName[8] = "py";

char effectPositionZName[8] = "pz";

char effectRotationXName[8] = "rx";

char effectRotationYName[8] = "ry";

char effectRotationZName[8] = "rz";

void Java_xeno_Effect_call__I(JThread *thread, EffectCommandCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    int id;
    EffectFieldValue *field_value;
    EffectCallArgs *args;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectIdName, -1), 0);
        id = *(int *)(object + field->offset);
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectArgsName, -1), 0);
        field_value = (void *)(object + field->offset);
        args = field_value->arguments;
        FX_call(arguments->command, id, args->data, args->length);
    }
}

void Java_xeno_Effect_disp__Z(JThread *thread, EffectBooleanCall *arguments)
{
    EffectFieldValue *field_value;
    unsigned char *object;
    JavaField *field;
    SchedulerState *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            if (arguments->value != 0) {
                sefRewindEffectCf(peer);
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
    SchedulerState *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            peer->scale.x = arguments->x;
            peer->scale.y = arguments->y;
            peer->scale.z = arguments->z;
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
    SchedulerState *peer;
    SceneObject object_result;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        scale.class_ref = classJava_xeno_util_Vector4f->classPointer;
        scale.x = peer->scale.x;
        scale.y = peer->scale.y;
        scale.z = peer->scale.z;
        scale.w = peer->scale.w;
        object_result = &scale;
        /* Store the object-pointer representation in the raw VM result slot. */
        __builtin_memcpy(result, &object_result, sizeof(object_result));
    }
}

void Java_xeno_Effect_getTranslate__(JThread *thread, EffectCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    EffectFieldValue *field_value;
    Vector4 *translate;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
        field_value = (void *)(object + field->offset);
        translate = &field_value->effect->position;

        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPositionXName, -1), 0);
        *(float *)(object + field->offset) = translate->x;
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPositionYName, -1), 0);
        *(float *)(object + field->offset) = translate->y;
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPositionZName, -1), 0);
        *(float *)(object + field->offset) = translate->z;
    }
}

void Java_xeno_Effect_setTranslate__(JThread *thread, EffectCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    EffectFieldValue *field_value;
    Vector4 *translate;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
        field_value = (void *)(object + field->offset);
        translate = &field_value->effect->position;

        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPositionXName, -1), 0);
        translate->x = *(float *)(object + field->offset);
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPositionYName, -1), 0);
        translate->y = *(float *)(object + field->offset);
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPositionZName, -1), 0);
        translate->z = *(float *)(object + field->offset);
    }
}

void Java_xeno_Effect_getRotate__(JThread *thread, EffectCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    EffectFieldValue *field_value;
    Vector4 *rotate;
    float pi;

    object = arguments->object;
    if (object != 0) {
        pi = EFFECT_PI;
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
        field_value = (void *)(object + field->offset);
        rotate = &field_value->effect->rotation;

        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectRotationXName, -1), 0);
        *(float *)(object + field->offset) = (rotate->x / pi) * 180.0f;
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectRotationYName, -1), 0);
        *(float *)(object + field->offset) = (rotate->y / pi) * 180.0f;
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectRotationZName, -1), 0);
        *(float *)(object + field->offset) = (rotate->z / pi) * 180.0f;
    }
}

void Java_xeno_Effect_setRotate__(JThread *thread, EffectCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    EffectFieldValue *field_value;
    SchedulerState *peer;
    Vector4 *rotate;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        rotate = &peer->rotation;

        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectRotationXName, -1), 0);
        rotate->x = (*(float *)(object + field->offset) / 180.0f) * EFFECT_PI;
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectRotationYName, -1), 0);
        rotate->y = (*(float *)(object + field->offset) / 180.0f) * EFFECT_PI;
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectRotationZName, -1), 0);
        rotate->z = (*(float *)(object + field->offset) / 180.0f) * EFFECT_PI;
    }
}

void Java_xeno_Effect_setCaster__Lxeno_Chr_(JThread *thread, EffectChrCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    EffectFieldValue *field_value;
    SchedulerState *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        object = arguments->chr;
        field = lookupClassField(classJava_xeno_Chr, loadConstString(effectPeerName, -1), 0);
        peer->caster =*(void **)(object + field->offset);
    }
}

void Java_xeno_Effect_setTarget__Lxeno_Chr_(JThread *thread, EffectChrCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    EffectFieldValue *field_value;
    SchedulerState *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        object = arguments->chr;
        field = lookupClassField(classJava_xeno_Chr, loadConstString(effectPeerName, -1), 0);
        peer->target =*(void **)(object + field->offset);
    }
}

void Java_xeno_Effect_setCaster__Lxeno_Unit_(JThread *thread, EffectUnitCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    SchedulerState *peer;
    EffectFieldValue *field_value;
    EffectFieldValue *unit_value;
    MapUnitRecord *unit_peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            object = arguments->unit;
            field = lookupClassField(classJava_xeno_Unit, loadConstString(effectPeerName, -1), 0);
            unit_value = (void *)(object + field->offset);
            unit_peer = unit_value->unit;
            if (unit_peer != 0) {
                peer->casterUnit = unit_peer;
                unit_peer->effectCf[0] = (int)peer;
            }
        }
    }
}

void Java_xeno_Effect_setTarget__Lxeno_Unit_(JThread *thread, EffectUnitCall *arguments)
{
    unsigned char *object;
    JavaField *field;
    SchedulerState *peer;
    EffectFieldValue *field_value;
    EffectFieldValue *unit_value;
    MapUnitRecord *unit_peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            object = arguments->unit;
            field = lookupClassField(classJava_xeno_Unit, loadConstString(effectPeerName, -1), 0);
            unit_value = (void *)(object + field->offset);
            unit_peer = unit_value->unit;
            if (unit_peer != 0) {
                peer->targetUnit = unit_peer;
            }
        }
    }
}

void Java_xeno_Effect_setTransOffset__FFF(JThread *thread, EffectVectorCall *arguments)
{
    EffectFieldValue *field_value;
    unsigned char *object;
    JavaField *field;
    SchedulerState *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
        if (peer != 0) {
            peer->position.x = peer->position.x + arguments->x;
            peer->position.y = peer->position.y + arguments->y;
            peer->position.z = peer->position.z + arguments->z;
        }
    }
}

void Java_xeno_Effect_getForceLoop__(JThread *thread, EffectCall *arguments, signed char *result)
{
    EffectFieldValue *field_value;
    unsigned char *object;
    JavaField *field;
    SchedulerState *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
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
    SchedulerState *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
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
    SchedulerState *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
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
    SchedulerState *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
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
    SchedulerState *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
        field_value = (void *)(object + field->offset);
        peer = field_value->effect;
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
    SchedulerState *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
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
    SchedulerState *peer;

    object = arguments->object;
    if (object != 0) {
        field = lookupClassField(classJava_xeno_Effect, loadConstString(effectPeerName, -1), 0);
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
