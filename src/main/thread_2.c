#include "common.h"
#include "thread_2.h"

/*
 * Java_xeno_vm_Thread_create__ (main VA 0x002f6550, 52 bytes, GLOBAL
 * binding). Allocates a script VM thread (kind 4, 4 stack words, 0x40
 * frame words) and writes its handle back through the result pointer.
 */
void Java_xeno_vm_Thread_create__(JThread *thread, void *arguments, JThread **result)
{
    *result = JNI_createThread(4, 4, 0x40);
}

/*
 * Java_xeno_vm_Thread_setTarget__Ljava_lang_Object_Ljava_lang_String_
 * (main VA 0x002f6588, 120 bytes, GLOBAL binding). Resolves the named
 * void-returning method on the target object's class and stores it, the
 * target object and the default reset hook (JTHREAD_default) into this
 * Thread's own native handle.
 */
void Java_xeno_vm_Thread_setTarget__Ljava_lang_Object_Ljava_lang_String_(
    JThread *thread, Thread2SetTargetArguments *arguments)
{
    SceneObject target;
    SceneClass *target_class;
    Thread2Handle *handle;
    SceneMethod *method;

    target = arguments->target;
    target_class = ((SceneObjectHeader *)target)->class_ref->scene_class;
    handle = arguments->thread;
    method = findMethod(target_class,
                        loadConstString(arguments->name->storage->bytes,
                                        arguments->name->storage->length),
                        TYPE_Void);
    handle->object = target;
    handle->method = method;
    handle->reset = JTHREAD_default;
}

/*
 * Java_xeno_vm_Thread_start__ (main VA 0x002f6600, 52 bytes, GLOBAL
 * binding). Resets the thread's native handle and sets its start-pending
 * bit (0x10).
 */
void Java_xeno_vm_Thread_start__(JThread *thread, Thread2Handle **arguments)
{
    Thread2Handle *handle;

    handle = *arguments;
    JNI_initThread(handle);
    handle->flags |= 0x10;
}

/*
 * Java_xeno_vm_Thread_stop__ (main VA 0x002f6638, 24 bytes, GLOBAL
 * binding). Clears the thread's start-pending bit (0x10).
 */
void Java_xeno_vm_Thread_stop__(JThread *thread, Thread2Handle **arguments)
{
    Thread2Handle *handle;

    handle = *arguments;
    handle->flags &= ~0x10;
}
