#include "common.h"
#include "thread_2.h"

/*
 * Allocates a script VM thread (kind 4, 4 stack words, 0x40 frame words) and writes its handle
 * back through the result pointer.
 */
void Java_xeno_vm_Thread_create__(JThread *thread, void *arguments, JThread **result)
{
    *result = JNI_createThread(JTHREAD_KIND_DEFAULT, 4, 0x40);
}

/*
 * Stores the target object, the named void-returning method looked up on its class, and the
 * default reset hook (JTHREAD_default) in the thread's native handle.
 */
void Java_xeno_vm_Thread_setTarget__Ljava_lang_Object_Ljava_lang_String_(
    JThread *thread, Thread2SetTargetArguments *arguments)
{
    SceneObject target;
    JavaClass *target_class;
    JThread *handle;
    JavaMethod *method;

    target = arguments->target;
    target_class = ((ObjectHeader *)target)->classPointer->clazz;
    handle = arguments->thread;
    method = findMethod(target_class,
                        loadConstString(arguments->name->storage->bytes,
                                        arguments->name->storage->length),
                        TYPE_Void);
    handle->object = target;
    handle->method = method;
    handle->reset = JTHREAD_default;
}

/* Resets the thread's native handle and sets its runnable flag (0x10). */
void Java_xeno_vm_Thread_start__(JThread *thread, JThread **arguments)
{
    JThread *handle;

    handle = *arguments;
    JNI_initThread(handle);
    handle->flags |= JTHREAD_FLAG_RUNNABLE;
}

/* Clears the thread's runnable flag (0x10). */
void Java_xeno_vm_Thread_stop__(JThread *thread, JThread **arguments)
{
    JThread *handle;

    handle = *arguments;
    handle->flags &= ~JTHREAD_FLAG_RUNNABLE;
}
