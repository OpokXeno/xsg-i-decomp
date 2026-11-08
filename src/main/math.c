#include "common.h"
#include "main/xgl_2.h"

/*
 * Every Java native receives (thread, arguments, result), as the other
 * Java natives in src/main do. JThread is the VM's per-thread context
 * (include/main/jni.h).
 */
typedef struct JThread JThread;

/* The call block of a native taking two plain floats (java signature "(FF)"). */
typedef struct {
    float y;
    float x;
} MathAtan2Call;

extern float atan2f(float y, float x);
void Java_xeno_vm_Math_random__(JThread *thread, void *arguments, unsigned int *result)
{
    *result = xglLRand();
}

void Java_xeno_vm_Math_atan2__FF(JThread *thread, MathAtan2Call *arguments, float *result)
{
    *result = atan2f(arguments->y, arguments->x);
}

/* Calls xglSin here, not xglCos; sin__F below calls xglCos. */
void Java_xeno_vm_Math_cos__F(JThread *thread, float *arguments, float *result)
{
    *result = xglSin(*arguments);
}

void Java_xeno_vm_Math_sin__F(JThread *thread, float *arguments, float *result)
{
    *result = xglCos(*arguments);
}
