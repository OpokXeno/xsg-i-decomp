#include "common.h"

#include "shared.h"

#include "stage_1.h"

/* Twenty-four halfwords occupy the stage peer block returned by STAGE_create. */

static u16 defaultStage[24];

/* CameraWork's independently evidenced prefix: creation fills the object tag
 * and camera id; TCAMERA_update reads the four channel modes and frames. */
typedef struct StageCameraPrefix {
    SceneObjectClassRef *class_ref;
    int camera_id;
    unsigned int : 32;
    int mode[4];
    int frame[4];
} StageCameraPrefix;

/* Field entries reuse word four for a static address or an instance offset.
 * resolveStaticField/resolveInstanceField establish this tagged location. */
typedef struct StageClassField {
    unsigned char unmodeled_00[4];
    unsigned short flags;
    unsigned char unmodeled_06[2];
    void *type;
    union { int constant_index; int size; int value; } payload;
    union { void *address; int offset; int initial_value; } location;
} StageClassField;

/* Twenty-four halfwords occupy the stage peer block returned by STAGE_create. */

const char D_004C2420[12] = "xeno/Camera";

const char D_004DA628[8] = "cam0";

const char D_004DA630[8] = "init";

const char D_004DA638[8] = "()V";

void *STAGE_create(u16 stage_id)
{
    defaultStage[2] = stage_id;
    return defaultStage;
}

void STAGE_instance(SceneVm *vm, SceneObject stage_object)
{
    SceneObject arguments[1];
    SceneObject parent_object;
    SceneObject primary_object;
    SceneClass *stage_class;
    SceneClass *camera_class;
    SceneMethod *method;
    SceneField *field;
    SceneField *nested;
    SceneString *method_name;
    int remaining;
    int nested_remaining;

    primary_object = stage_object;
    parent_object = stage_object;
    stage_class = ((SceneObjectHeader *)primary_object)->class_ref->scene_class;
    if (stage_class->name != 0)
        parent_object = primary_object;
    method = findMethod(stage_class, NAME_Constructor, TYPE_Void);
    if (method != 0) {
        arguments[0] = primary_object;
        JNI_initThread(vm);
        JNI_callMethod(vm, method, arguments, 0);
    }

    /* The camera field shares `field` with the walk below; its word four is
     * the tagged location that resolveInstanceField set to an offset. */
    field = (SceneField *)lookupClassField(
        classJava_xeno_Stage, loadConstString(D_004DA628, -1), 0);
    if (field != 0) {
        camera_class = loadClass(loadConstString(D_004C2420, -1), 0);
        ((StageCameraPrefix *)tcamera)->class_ref = camera_class->instance_class_ref;
        *(SceneObject *)(stage_object + ((StageClassField *)field)->location.offset) = tcamera;
    }

    stage_class = ((SceneObjectHeader *)stage_object)->class_ref->scene_class;
    remaining = stage_class->field_count - stage_class->static_field_count;
    remaining--;
    field = stage_class->fields + stage_class->static_field_count;
    for (; remaining >= 0; remaining--, field++) {
        if (field->flags & 0x8000) {
            SceneClass *resolved = getClassFromSignature(
                field->type_or_descriptor->signature, stage_class->class_loader);
            if (resolved == 0)
                continue;
            field->type_or_descriptor = (SceneTypeDescriptor *)resolved;
            field->flags &= 0x7fff;
        }
        if (instanceOf((SceneClass *)field->type_or_descriptor, classJava_xeno_Chr)) {
            SceneMethod *init_method;
            SceneObject child;

            method_name = loadConstString(D_004DA630, -1);
            init_method = findMethod((SceneClass *)field->type_or_descriptor,
                                     method_name,
                                     loadConstString(D_004DA638, -1));
            if (init_method != 0) {
                SceneClass *child_class;

                child = newObject((SceneClass *)field->type_or_descriptor);
                arguments[0] = child;
                JNI_initThread(vm);
                JNI_callMethod(vm, init_method, arguments, 0);
                *(SceneObject *)(parent_object + field->instance_offset) = child;
                child_class = (SceneClass *)field->type_or_descriptor;
                nested_remaining = child_class->field_count - child_class->static_field_count;
                nested_remaining--;
                nested = child_class->fields + child_class->static_field_count;
                for (; nested_remaining >= 0; nested_remaining--, nested++) {
                    if (nested->flags & 0x8000) {
                        nested->type_or_descriptor =
                            (SceneTypeDescriptor *)getClassFromSignature(
                                nested->type_or_descriptor->signature,
                                stage_class->class_loader);
                        nested->flags &= 0x7fff;
                    }
                    if ((SceneClass *)nested->type_or_descriptor == stage_class) {
                        *(SceneObject *)(child + nested->instance_offset) = parent_object;
                    }
                }
            }
        }
    }

    stage_class = ((SceneObjectHeader *)parent_object)->class_ref->scene_class;
    method_name = loadConstString(D_004DA630, -1);
    method = findMethod(stage_class, method_name,
                        loadConstString(D_004DA638, -1));
    if (method != 0) {
        arguments[0] = stage_object;
        JNI_initThread(vm);
        JNI_callMethod(vm, method, arguments, 0);
    }
}
