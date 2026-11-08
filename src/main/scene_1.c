#include "common.h"
#include "shared.h"
#include "main/scene_1.h"
#include "scene_1.h"
#include "main/jni.h"

/* JThread.flags set when start is called on the running thread itself. */
#define SCENE_THREAD_START_SELF 0x21
#define SCENE_SIGNATURE_SIZE 256

const char scene_window_class_name[24] = "xeno/util/Window";

const char scene_constructor_format[8] = "(L%s;)V";

const char scene_message_field_name[8] = "msg";

const char scene_init_name[8] = "init";

const char scene_void_signature[8] = "()V";

const char scene_cleanup_name[8] = "cleanup";

/*
 * Sets up a scene instance: runs its constructor, creates the window stored in
 * the "msg" field, creates each Chr or Unit field (running its constructor and
 * init, and pointing its fields of the scene's own type back at the scene),
 * then calls init().
 */
void SCENE_instance(SceneVm *vm, SceneObject scene_object)
{
    SceneObject arguments[2];
    SceneObject primary_object;
    SceneObject parent_object;
    char constructor_signature[SCENE_SIGNATURE_SIZE];
    JavaClass *scene_class;
    JavaMethod *constructor;
    SceneString *constructor_signature_object;
    SceneString *method_name;
    TComponent *window;
    int remaining;
    SceneField *field;

    primary_object = scene_object;
    scene_class = ((ObjectHeader *)primary_object)->classPointer->clazz;
    constructor = findMethod(scene_class, NAME_Constructor, TYPE_Void);
    sprintf(constructor_signature, scene_constructor_format,
            scene_class->name->bytes);
    constructor_signature_object = loadConstString2(constructor_signature, -1);

    parent_object = scene_object;
    if (scene_class->name != 0)
        parent_object = primary_object;
    if (constructor != 0) {
        arguments[0] = primary_object;
        JNI_initThread(vm);
        JNI_callMethod(vm, constructor, arguments, 0);
    }

    /*
     * field and scene_class are reused for the message field and the window
     * class, as the original does. lookupClassField returns the same 20-byte
     * field record the scan walks.
     */
    field = (SceneField *)lookupClassField(
        classJava_xeno_Scene,
        loadConstString(scene_message_field_name, -1), 0);
    /*
     * Turns the native window into a xeno/util/Window instance by setting its
     * class reference, then stores it in the scene's "msg" field.
     */
    scene_class = loadClass(loadConstString(scene_window_class_name, -1), 0);
    window = TWIN_create2(-1);
    window->class_ref = scene_class->classPointer;
    TWIN_initScene(window);
    *(TComponent **)(primary_object + field->location.offset) = window;

    /* Create each instance field of the scene class that is a Chr or a Unit. */
    scene_class = ((ObjectHeader *)primary_object)->classPointer->clazz;
    remaining = scene_class->fieldCount - scene_class->staticFieldCount;
    remaining--;
    field = scene_class->fields + scene_class->staticFieldCount;
    while (remaining >= 0) {
        JavaMethod *init_method;
        JavaMethod *ctor_method;
        SceneObject field_object;

        if (field->flags & JAVA_FIELD_UNRESOLVED) {
            JavaClass *resolved = getClassFromSignature(
                field->type.descriptor->bytes, scene_class->classLoader);
            if (resolved == 0) {
                field++;
                remaining--;
                continue;
            }
            field->type.resolved = resolved;
            field->flags &= JAVA_FIELD_FLAGS_MASK;
        }

        if (instanceOf(field->type.resolved,
                       classJava_xeno_Chr)) {
            method_name = loadConstString(scene_init_name, -1);
            init_method = findMethod(field->type.resolved,
                                     method_name,
                                     loadConstString(scene_void_signature, -1));
            if (init_method != 0) {
                field_object = newObject(field->type.resolved);
                ctor_method = findMethod(field->type.resolved,
                                         NAME_Constructor,
                                         constructor_signature_object);
                if (ctor_method != 0) {
                    arguments[0] = field_object;
                    arguments[1] = parent_object;
                    JNI_initThread(vm);
                    JNI_callMethod(vm, ctor_method, arguments, 0);
                }
                arguments[0] = field_object;
                JNI_initThread(vm);
                JNI_callMethod(vm, init_method, arguments, 0);
                {
                    SceneObject *field_slot;
                    JavaClass *component_class;
                    SceneField *nested;
                    int nested_remaining;

                    field_slot = (SceneObject *)(parent_object + field->location.offset);
                    component_class = field->type.resolved;
                    *field_slot = field_object;
                    nested_remaining = component_class->fieldCount -
                                       component_class->staticFieldCount;
                    nested_remaining--;
                    nested = component_class->fields +
                             component_class->staticFieldCount;
                    while (nested_remaining >= 0) {
                        if (nested->flags & JAVA_FIELD_UNRESOLVED) {
                            nested->type.resolved = getClassFromSignature(
                                    nested->type.descriptor->bytes,
                                    scene_class->classLoader);
                            nested->flags &= JAVA_FIELD_FLAGS_MASK;
                        }
                        if (nested->type.resolved == scene_class) {
                            *(SceneObject *)(field_object +
                                              nested->location.offset) =
                                parent_object;
                        }
                        nested++;
                        nested_remaining--;
                    }
                }
            }
        } else if (instanceOf(field->type.resolved,
                              classJava_xeno_Unit)) {
            method_name = loadConstString(scene_init_name, -1);
            init_method = findMethod(field->type.resolved,
                                     method_name,
                                     loadConstString(scene_void_signature, -1));
            if (init_method != 0) {
                field_object = newObject(field->type.resolved);
                ctor_method = findMethod(field->type.resolved,
                                          NAME_Constructor,
                                          constructor_signature_object);
                if (ctor_method != 0) {
                    arguments[0] = field_object;
                    arguments[1] = parent_object;
                    JNI_initThread(vm);
                    JNI_callMethod(vm, ctor_method, arguments, 0);
                }
                arguments[0] = field_object;
                JNI_initThread(vm);
                JNI_callMethod(vm, init_method, arguments, 0);
                {
                    SceneObject *field_slot;
                    JavaClass *component_class;
                    SceneField *nested;
                    int nested_remaining;

                    field_slot = (SceneObject *)(parent_object + field->location.offset);
                    component_class = field->type.resolved;
                    *field_slot = field_object;
                    nested_remaining = component_class->fieldCount -
                                       component_class->staticFieldCount;
                    nested_remaining--;
                    nested = component_class->fields +
                             component_class->staticFieldCount;
                    while (nested_remaining >= 0) {
                        if (nested->flags & JAVA_FIELD_UNRESOLVED) {
                            nested->type.resolved = getClassFromSignature(
                                    nested->type.descriptor->bytes,
                                    scene_class->classLoader);
                            nested->flags &= JAVA_FIELD_FLAGS_MASK;
                        }
                        if (nested->type.resolved == scene_class) {
                            *(SceneObject *)(field_object +
                                              nested->location.offset) =
                                parent_object;
                        }
                        nested++;
                        nested_remaining--;
                    }
                }
            }
        }
        field++;
        remaining--;
    }

    /* Call init() on the parent object. */
    scene_class = ((ObjectHeader *)parent_object)->classPointer->clazz;
    method_name = loadConstString(scene_init_name, -1);
    constructor = findMethod(scene_class, method_name,
                             loadConstString(scene_void_signature, -1));
    if (constructor != 0) {
        arguments[0] = primary_object;
        JNI_initThread(vm);
        JNI_callMethod(vm, constructor, arguments, 0);
    }
}

/*
 * Returns 1 when scene_object is a xeno.Scene instance, else 0. The return type
 * is long because that matches the original's call form; int and long agree for
 * these 0/1 values.
 */
long SCENE_dispose(SceneObject scene_object)
{
    return JNI_isInstanceOf(scene_object, classJava_xeno_Scene);
}

/* Calls the scene object's cleanup() method, if its class has one. */
void SCENE_cleanup(SceneVm *vm, SceneObject scene_object)
{
    SceneObject arguments[1];
    JavaClass *scene_class =
        ((ObjectHeader *)scene_object)->classPointer->clazz;
    JavaMethod *cleanup_method = findMethod(
        scene_class, loadConstString(scene_cleanup_name, -1),
        loadConstString(scene_void_signature, -1));

    if (cleanup_method != 0) {
        arguments[0] = scene_object;
        JNI_initThread(vm);
        /* NULL output: no VM result word is requested for the void cleanup call. */
        JNI_callMethod(vm, cleanup_method, arguments, 0);
    }
}

/*
 * Attaches the scene's default script method to its thread. Mode 1 takes the
 * method name from a SceneString and mode 8 from a C string; other modes do
 * nothing. The thread is flagged as started on itself when it is the calling
 * VM thread.
 */
void SCENE_start(SceneVm *vm, SceneObject scene_object, int mode,
                 void *method_name)
{
    JavaClass *scene_class;

    if (!JNI_isInstanceOf(scene_object, classJava_xeno_Scene))
        return;

    if (mode != 1) {
        SceneString *name;
        JThread *thread8;
        JavaMethod *method8;

        if (mode < 2)
            return;
        if (mode != 8)
            return;

        scene_class = ((ObjectHeader *)scene_object)->classPointer->clazz;
        name = loadConstString((const char *)method_name, -1);
        method8 = findMethod(scene_class, name, TYPE_Void);
        thread8 = JTHREAD_get(scene_object);
        if (thread8 != 0) {
            thread8->method = method8;
            thread8->reset = JTHREAD_defaultScene;
            thread8->flags |= SCENE_THREAD_HAS_METHOD;
            if (thread8 == vm)
                thread8->flags |= SCENE_THREAD_START_SELF;
        }
    } else {
        SceneStringStorage *storage;
        SceneString *name;
        JavaMethod *method1;
        JThread *thread1;

        scene_class = ((ObjectHeader *)scene_object)->classPointer->clazz;
        storage = ((SceneString *)method_name)->storage;
        name = loadConstString(storage->bytes, storage->length);
        method1 = findMethod(scene_class, name, TYPE_Void);
        thread1 = JTHREAD_get(scene_object);
        if (thread1 != 0) {
            thread1->method = method1;
            thread1->reset = JTHREAD_defaultScene;
            thread1->flags |= SCENE_THREAD_HAS_METHOD;
        }
        if (thread1 != vm)
            return;
        thread1->flags |= SCENE_THREAD_START_SELF;
    }
}
