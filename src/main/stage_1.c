#include "common.h"
#include "shared.h"
#include "stage_1.h"

#include "main/jni.h"
#include "main/stage_1.h"

/* Prefix of the CameraWork object: creation fills the object tag and camera id;
 * TCAMERA_update reads the four channel modes and frames. */
typedef struct StageCameraPrefix {
    DispatchTable *class_ref;
    int camera_id;
    unsigned int : 32;
    int mode[4];
    int frame[4];
} StageCameraPrefix;

#define STAGE_PEER_HALFWORDS 24
#define STAGE_PEER_ID_INDEX 2

const char cameraClassName[12] = "xeno/Camera";

const char cameraInstanceName[8] = "cam0";

const char initMethodName[8] = "init";

const char initMethodSignature[8] = "()V";

/* Twenty-four halfwords occupy the stage peer block returned by STAGE_create. */
static u16 defaultStage[STAGE_PEER_HALFWORDS];

/* Stores stage_id in the peer block's id halfword and returns that block. */
void *STAGE_create(u16 stage_id)
{
    defaultStage[STAGE_PEER_ID_INDEX] = stage_id;
    return defaultStage;
}

/*
 * Initializes a stage object: runs its constructor, links the camera, creates the Chr objects held
 * in its fields and runs their init methods, then runs the init method of the parent's class.
 */
void STAGE_instance(SceneVm *vm, SceneObject stage_object)
{
    SceneObject arguments[1];
    SceneObject parent_object;
    SceneObject primary_object;
    JavaClass *stage_class;
    JavaClass *camera_class;
    JavaMethod *method;
    SceneField *field;
    SceneField *nested;
    SceneString *method_name;
    int remaining;
    int nested_remaining;

    primary_object = stage_object;
    parent_object = stage_object;
    stage_class = ((ObjectHeader *)primary_object)->classPointer->clazz;
    if (stage_class->name != 0)
        parent_object = primary_object;
    /* Run the constructor, when the class declares one. */
    method = findMethod(stage_class, NAME_Constructor, TYPE_Void);
    if (method != 0) {
        arguments[0] = primary_object;
        JNI_initThread(vm);
        JNI_callMethod(vm, method, arguments, 0);
    }

    /* The camera field shares `field` with the walk below; its word four is
     * the tagged location that resolveInstanceField set to an offset. */
    field = (SceneField *)lookupClassField(
        classJava_xeno_Stage, loadConstString(cameraInstanceName, -1), 0);
    if (field != 0) {
        camera_class = loadClass(loadConstString(cameraClassName, -1), 0);
        ((StageCameraPrefix *)tcamera)->class_ref = camera_class->classPointer;
        *(SceneObject *)(stage_object + field->location.offset) = tcamera;
    }

    /* Walk the instance fields, resolving unresolved field types and creating a Chr object for each
     * Chr-typed field. */
    stage_class = ((ObjectHeader *)stage_object)->classPointer->clazz;
    remaining = stage_class->fieldCount - stage_class->staticFieldCount;
    remaining--;
    field = stage_class->fields + stage_class->staticFieldCount;
    for (; remaining >= 0; remaining--, field++) {
        if (field->flags & JAVA_FIELD_UNRESOLVED) {
            JavaClass *resolved = getClassFromSignature(
                field->type.descriptor->bytes, stage_class->classLoader);
            if (resolved == 0)
                continue;
            field->type.resolved = resolved;
            field->flags &= JAVA_FIELD_FLAGS_MASK;
        }
        if (instanceOf(field->type.resolved, classJava_xeno_Chr)) {
            JavaMethod *init_method;
            SceneObject child;

            method_name = loadConstString(initMethodName, -1);
            init_method = findMethod(field->type.resolved,
                                     method_name,
                                     loadConstString(initMethodSignature, -1));
            if (init_method != 0) {
                SceneObject *field_slot;
                JavaClass *child_class;

                child = newObject(field->type.resolved);
                arguments[0] = child;
                JNI_initThread(vm);
                JNI_callMethod(vm, init_method, arguments, 0);
                field_slot = (SceneObject *)(parent_object + field->location.offset);
                child_class = field->type.resolved;
                *field_slot = child;
                nested_remaining = child_class->fieldCount - child_class->staticFieldCount;
                nested_remaining--;
                nested = child_class->fields + child_class->staticFieldCount;
                for (; nested_remaining >= 0; nested_remaining--, nested++) {
                    if (nested->flags & JAVA_FIELD_UNRESOLVED) {
                        nested->type.resolved = getClassFromSignature(
                                nested->type.descriptor->bytes,
                                stage_class->classLoader);
                        nested->flags &= JAVA_FIELD_FLAGS_MASK;
                    }
                    if (nested->type.resolved == stage_class) {
                        *(SceneObject *)(child + nested->location.offset) = parent_object;
                    }
                }
            }
        }
    }

    /* Run the init method of the parent object's class. */
    stage_class = ((ObjectHeader *)parent_object)->classPointer->clazz;
    method_name = loadConstString(initMethodName, -1);
    method = findMethod(stage_class, method_name,
                        loadConstString(initMethodSignature, -1));
    if (method != 0) {
        arguments[0] = stage_object;
        JNI_initThread(vm);
        JNI_callMethod(vm, method, arguments, 0);
    }
}
