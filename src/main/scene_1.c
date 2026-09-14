#include "common.h"
#include "shared.h"
#include "scene_1.h"

void SCENE_instance(SceneVm *vm, SceneObject scene_object)
{
    SceneObject arguments[2];
    SceneObject primary_object;
    SceneObject parent_object;
    char constructor_signature[256];
    SceneClass *scene_class;
    SceneMethod *constructor;
    SceneString *constructor_signature_object;
    SceneString *method_name;
    SceneWindow *window;
    int remaining;
    SceneField *field;

    primary_object = scene_object;
    scene_class = ((SceneObjectHeader *)primary_object)->class_ref->scene_class;
    constructor = findMethod(scene_class, NAME_Constructor, TYPE_Void);
    sprintf(constructor_signature, scene_constructor_format,
            scene_class->name->binary_name);
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
     * class, as the original does (the same registers as the field scan).
     * lookupClassField returns the same 20-byte field record the scan walks.
     */
    field = (SceneField *)lookupClassField(
        classJava_xeno_Scene,
        loadConstString(scene_message_field_name, -1), 0);
    /*
     * Turns the native window into a xeno/util/Window instance: copy the
     * loaded class's own class-ref word into the window's object header,
     * the same idiom newObject uses to seed a freshly allocated instance.
     * The window is then stored into the scene's "msg" field below.
     */
    scene_class = loadClass(loadConstString(scene_window_class_name, -1), 0);
    window = TWIN_create2(-1);
    window->header.class_ref = scene_class->instance_class_ref;
    TWIN_initScene(window);
    *(SceneWindow **)(primary_object + field->instance_offset) = window;

    scene_class = ((SceneObjectHeader *)primary_object)->class_ref->scene_class;
    remaining = scene_class->field_count - scene_class->static_field_count;
    remaining--;
    field = scene_class->fields + scene_class->static_field_count;
    while (remaining >= 0) {
        SceneMethod *init_method;
        SceneMethod *ctor_method;
        SceneObject field_object;

        if (field->flags & 0x8000) {
            SceneClass *resolved = getClassFromSignature(
                field->type_or_descriptor->signature, scene_class->class_loader);
            if (resolved == 0) {
                field++;
                remaining--;
                continue;
            }
            field->type_or_descriptor = (SceneTypeDescriptor *)resolved;
            field->flags &= 0x7fff;
        }

        if (instanceOf((SceneClass *)field->type_or_descriptor,
                       classJava_xeno_Chr)) {
            method_name = loadConstString(scene_init_name, -1);
            init_method = findMethod((SceneClass *)field->type_or_descriptor,
                                     method_name,
                                     loadConstString(scene_void_signature, -1));
            if (init_method != 0) {
                field_object = newObject((SceneClass *)field->type_or_descriptor);
                ctor_method = findMethod((SceneClass *)field->type_or_descriptor,
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
                    SceneClass *component_class;
                    SceneField *nested;
                    int nested_remaining;

                    *(SceneObject *)(parent_object + field->instance_offset) =
                        field_object;
                    component_class = (SceneClass *)field->type_or_descriptor;
                    nested_remaining = component_class->field_count -
                                       component_class->static_field_count;
                    nested_remaining--;
                    nested = component_class->fields +
                             component_class->static_field_count;
                    while (nested_remaining >= 0) {
                        if (nested->flags & 0x8000) {
                            nested->type_or_descriptor =
                                (SceneTypeDescriptor *)getClassFromSignature(
                                    nested->type_or_descriptor->signature,
                                    scene_class->class_loader);
                            nested->flags &= 0x7fff;
                        }
                        if ((SceneClass *)nested->type_or_descriptor == scene_class) {
                            *(SceneObject *)(field_object +
                                              nested->instance_offset) =
                                parent_object;
                        }
                        nested++;
                        nested_remaining--;
                    }
                }
            }
        } else if (instanceOf((SceneClass *)field->type_or_descriptor,
                              classJava_xeno_Unit)) {
            method_name = loadConstString(scene_init_name, -1);
            init_method = findMethod((SceneClass *)field->type_or_descriptor,
                                     method_name,
                                     loadConstString(scene_void_signature, -1));
            if (init_method != 0) {
                field_object = newObject((SceneClass *)field->type_or_descriptor);
                ctor_method = findMethod((SceneClass *)field->type_or_descriptor,
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
                    SceneClass *component_class;
                    SceneField *nested;
                    int nested_remaining;

                    *(SceneObject *)(parent_object + field->instance_offset) =
                        field_object;
                    component_class = (SceneClass *)field->type_or_descriptor;
                    nested_remaining = component_class->field_count -
                                       component_class->static_field_count;
                    nested_remaining--;
                    nested = component_class->fields +
                             component_class->static_field_count;
                    while (nested_remaining >= 0) {
                        if (nested->flags & 0x8000) {
                            nested->type_or_descriptor =
                                (SceneTypeDescriptor *)getClassFromSignature(
                                    nested->type_or_descriptor->signature,
                                    scene_class->class_loader);
                            nested->flags &= 0x7fff;
                        }
                        if ((SceneClass *)nested->type_or_descriptor == scene_class) {
                            *(SceneObject *)(field_object +
                                              nested->instance_offset) =
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

    scene_class = ((SceneObjectHeader *)parent_object)->class_ref->scene_class;
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
 * Return-type caveat : `long` is a proven
 * zero-extra-instruction codegen lever blocking ee-gcc2.96 -O2 sibling-call
 * (`int`/`void`/int-temp all sibcall to the 24B `j` form; `long` keeps the
 * 32B `jal` form; `unsigned char`/`short` add truncation). `void` vs
 * predicate `int` vs `long` dispose semantics are unproven from callers/data:
 * no direct `jal SCENE_dispose` callers in SLUS_204.69, JNI_isInstanceOf
 * returns int 0/1 (see SCENE_start predicate use), and the original forwards
 * a0/v0 unchanged with gp-loaded classJava_xeno_Scene. For 0/1 values
 * int/long are indistinguishable (upper already zero). No cleaner pure-C
 * hypothesis exists in evidence.
 */
long SCENE_dispose(SceneObject scene_object)
{
    return JNI_isInstanceOf(scene_object, classJava_xeno_Scene);
}

void SCENE_cleanup(SceneVm *vm, SceneObject scene_object)
{
    SceneObject arguments[1];
    SceneClass *scene_class =
        ((SceneObjectHeader *)scene_object)->class_ref->scene_class;
    SceneMethod *cleanup_method = findMethod(
        scene_class, loadConstString(scene_cleanup_name, -1),
        loadConstString(scene_void_signature, -1));

    if (cleanup_method != 0) {
        arguments[0] = scene_object;
        JNI_initThread(vm);
        /* NULL output: no VM result word is requested for the void cleanup call. */
        JNI_callMethod(vm, cleanup_method, arguments, 0);
    }
}

void SCENE_start(SceneVm *vm, SceneObject scene_object, int mode,
                 void *method_name)
{
    SceneClass *scene_class;

    if (!JNI_isInstanceOf(scene_object, classJava_xeno_Scene))
        return;

    if (mode != 1) {
        SceneString *name;
        SceneThread *thread8;
        SceneMethod *method8;

        if (mode < 2)
            return;
        if (mode != 8)
            return;

        scene_class = ((SceneObjectHeader *)scene_object)->class_ref->scene_class;
        name = loadConstString((const char *)method_name, -1);
        method8 = findMethod(scene_class, name, TYPE_Void);
        thread8 = JTHREAD_get(scene_object);
        if (thread8 != 0) {
            thread8->method = method8;
            thread8->entry = JTHREAD_defaultScene;
            thread8->flags |= SCENE_THREAD_HAS_METHOD;
            if (thread8 == (SceneThread *)vm)
                thread8->flags |= 0x21;
        }
    } else {
        SceneStringStorage *storage;
        SceneString *name;
        SceneMethod *method1;
        SceneThread *thread1;

        scene_class = ((SceneObjectHeader *)scene_object)->class_ref->scene_class;
        storage = ((SceneString *)method_name)->storage;
        name = loadConstString(storage->bytes, storage->length);
        method1 = findMethod(scene_class, name, TYPE_Void);
        thread1 = JTHREAD_get(scene_object);
        if (thread1 != 0) {
            thread1->method = method1;
            thread1->entry = JTHREAD_defaultScene;
            thread1->flags |= SCENE_THREAD_HAS_METHOD;
        }
        if (thread1 != (SceneThread *)vm)
            return;
        thread1->flags |= 0x21;
    }
}
