#include "common.h"

/*
 * The script VM's per-thread context, recovered as `JThread` in
 * src/main/chr.h (main's chr TU). Every Java native receives
 * (thread, arguments, result) as the sibling natives elsewhere in the
 * TU map (src/main/runtime.c, src/main/window.c, src/main/system.c) show.
 */
typedef struct JThread JThread;

extern unsigned int xglLRand(void);

void Java_xeno_vm_Math_random__(JThread *thread, void *arguments, unsigned int *result)
{
    *result = xglLRand();
}

extern float atan2f(float y, float x);

/* The call block of a native taking two plain floats (java signature "(FF)"). */
typedef struct {
    float y;
    float x;
} MathAtan2Call;

void Java_xeno_vm_Math_atan2__FF(JThread *thread, MathAtan2Call *arguments, float *result)
{
    *result = atan2f(arguments->y, arguments->x);
}

extern float xglSin(float angle);

/* Calls xglSin here, not xglCos; sin__F below calls xglCos. */
void Java_xeno_vm_Math_cos__F(JThread *thread, float *arguments, float *result)
{
    *result = xglSin(*arguments);
}

extern float xglCos(float angle);

void Java_xeno_vm_Math_sin__F(JThread *thread, float *arguments, float *result)
{
    *result = xglCos(*arguments);
}
