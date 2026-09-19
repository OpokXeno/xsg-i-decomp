#include "common.h"
#include "shared.h"

/*
 * The Java VM's per-thread execution context, already recovered as
 * `JThread` in src/main/chr.h (main's chr TU). This TU forwards a pointer
 * to it without touching any member, so only the tag is declared here.
 */
typedef struct JThread JThread;

/*
 * UNIT_rotY/UNIT_rotZ/UNIT_sclX/UNIT_sclY/UNIT_sclZ are this TU's own
 * INCLUDE_ASM functions (0x00301d58, 0x003020a8, 0x00302ef8, 0x00303168,
 * 0x003033d8). Their JNI trampolines below insert the interpolation-mode
 * literal as a new first argument and forward `thread`/`arguments`/
 * `failure_result` unchanged -- the same shape as Java_xeno_Chr_sclX__FFZ /
 * CHR_sclX in src/main/chr.c, which names the same three parameters.
 */
void UNIT_rotY(int mode, JThread *thread, void *arguments,
               u32 *failure_result);
void UNIT_rotZ(int mode, JThread *thread, void *arguments,
               u32 *failure_result);
void UNIT_sclX(int mode, JThread *thread, void *arguments,
               u32 *failure_result);
void UNIT_sclY(int mode, JThread *thread, void *arguments,
               u32 *failure_result);
void UNIT_sclZ(int mode, JThread *thread, void *arguments,
               u32 *failure_result);

/*
 * UNIT_moveXZ/UNIT_rotX/UNIT_motion are this TU's own INCLUDE_ASM functions
 * (0x00301710, 0x00301a38, 0x00302a10). Their JNI trampolines below are the
 * same shape as UNIT_rotY/UNIT_rotZ/UNIT_sclX/UNIT_sclY/UNIT_sclZ above:
 * insert the mode literal ahead of `thread`/`arguments`/`failure_result` and
 * forward those three unchanged.
 */
void UNIT_moveXZ(int mode, JThread *thread, void *arguments,
                 u32 *failure_result);
void UNIT_rotX(int mode, JThread *thread, void *arguments,
               u32 *failure_result);
void UNIT_motion(int mode, JThread *thread, void *arguments,
                 u32 *failure_result);

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_setUpdate);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_rotYCNS__Ljava_lang_Object_IFFF);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_transCNS__Ljava_lang_Object_IFFF);

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_moveXZ);

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_rotX);

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_rotY);

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_rotZ);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_getRotate__);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_getSignal__);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_getTranslate__);

/*
 * An empty native (`jr $31; nop`): the script VM still calls it with the
 * native signature every Java_xeno_* entry receives, but it reads nothing.
 */
void Java_xeno_Unit_invalidate__(JThread *thread, void *arguments,
                                 u32 *failure_result)
{
}

/*
 * Interpolation mode one (a float value): forward thread/arguments/
 * failure_result unchanged and insert mode=1 ahead of them.
 */
void Java_xeno_Unit_move__FFFZ(JThread *thread, void *arguments,
                               u32 *failure_result)
{
    UNIT_moveXZ(1, thread, arguments, failure_result);
}

/* Interpolation mode zero (an int value), otherwise identical. */
void Java_xeno_Unit_move__IFFZ(JThread *thread, void *arguments,
                               u32 *failure_result)
{
    UNIT_moveXZ(0, thread, arguments, failure_result);
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_move__Lxeno_util_Spline_IZ);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_rotate__Lxeno_util_Spline_IZ);

/*
 * The Object-argument overloads of move are empty natives: they take the
 * native signature but read none of it.
 */
void Java_xeno_Unit_move__ILjava_lang_Object_Z(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

void Java_xeno_Unit_move__Ljava_lang_Object_FZ(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_motion);

/*
 * Motion mode zero (the short, two-int overload): forward thread/arguments/
 * failure_result unchanged and insert mode=0 ahead of them.
 */
void Java_xeno_Unit_mtn__IIFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_motion(0, thread, arguments, failure_result);
}

/* Motion mode one (the long, five-int overload), otherwise identical. */
void Java_xeno_Unit_mtn__IIIIIFZ(JThread *thread, void *arguments,
                                 u32 *failure_result)
{
    UNIT_motion(1, thread, arguments, failure_result);
}

/*
 * Interpolation mode one (a float value): forward thread/arguments/
 * failure_result unchanged and insert mode=1 ahead of them.
 */
void Java_xeno_Unit_rotX__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotX(1, thread, arguments, failure_result);
}

/* Interpolation mode zero (an int value), otherwise identical. */
void Java_xeno_Unit_rotX__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotX(0, thread, arguments, failure_result);
}

/*
 * The Object-argument overloads of rotX are empty natives: they take the
 * native signature but read none of it.
 */
void Java_xeno_Unit_rotX__ILjava_lang_Object_Z(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

void Java_xeno_Unit_rotX__Ljava_lang_Object_FZ(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

/*
 * Interpolation mode one (a float value): forward thread/arguments/
 * failure_result unchanged and insert mode=1 ahead of them.
 */
void Java_xeno_Unit_rotY__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotY(1, thread, arguments, failure_result);
}

/* Interpolation mode zero (an int value), otherwise identical. */
void Java_xeno_Unit_rotY__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotY(0, thread, arguments, failure_result);
}

/*
 * The Object-argument overloads of rotY and rotZ are empty natives, like
 * rotX's above: they take the native signature but read none of it.
 */
void Java_xeno_Unit_rotY__ILjava_lang_Object_Z(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

void Java_xeno_Unit_rotY__Ljava_lang_Object_FZ(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

void Java_xeno_Unit_rotZ__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotZ(1, thread, arguments, failure_result);
}

void Java_xeno_Unit_rotZ__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_rotZ(0, thread, arguments, failure_result);
}

void Java_xeno_Unit_rotZ__ILjava_lang_Object_Z(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

void Java_xeno_Unit_rotZ__Ljava_lang_Object_FZ(JThread *thread,
                                               void *arguments,
                                               u32 *failure_result)
{
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_scale__Lxeno_util_Spline_IZ);

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_sclX);

void Java_xeno_Unit_sclX__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_sclX(1, thread, arguments, failure_result);
}

void Java_xeno_Unit_sclX__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_sclX(0, thread, arguments, failure_result);
}

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_sclY);

void Java_xeno_Unit_sclY__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_sclY(1, thread, arguments, failure_result);
}

void Java_xeno_Unit_sclY__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_sclY(0, thread, arguments, failure_result);
}

INCLUDE_ASM("asm/main/nonmatchings/unit", UNIT_sclZ);

void Java_xeno_Unit_sclZ__FFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_sclZ(1, thread, arguments, failure_result);
}

void Java_xeno_Unit_sclZ__IFZ(JThread *thread, void *arguments,
                              u32 *failure_result)
{
    UNIT_sclZ(0, thread, arguments, failure_result);
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setCollision__Z);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setRotate__);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setTranslate__);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setVisible__IZ);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setVisible__Z);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_signal__I);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_start__ILjava_lang_Object_);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_stop__);

void Java_xeno_Unit_validate__(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setParent__Ljava_lang_Object_I);

static void copyArgs(u8 *dst, u8 *src, int count)
{
    u8 byte;

    if (count > 0) {
        count--;
        if (count >= 0) {
            do {
                byte = *src;
                src++;
                count--;
                *dst = byte;
                dst++;
            } while (count >= 0);
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setArgs__III);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_getArgs__II);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setArgs__ILjava_lang_Object_I);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setArgs__IIIII);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_getScale__);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setScale__FFF);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_getSerial__);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_mtnSetMask__I);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_mtnGetRoot__ILxeno_util_Vector4f_);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_getState__);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_getPivot__Lxeno_util_Vector4f_);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setPivot__FFF);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_getAxis__Lxeno_util_Vector4f_);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setAxis__FFFF);

/*
 * Callback shape for MAP_callUnitGroup's per-unit dispatch (src/main/
 * map_create_unit_peer.c, still INCLUDE_ASM): it forwards a pointer to a
 * flags word within the group's unit record. Only that one word's bit 0x10
 * is evidenced, so the record itself stays unmodeled here.
 */
#define UNIT_FLAG_SUSPENDED 0x10

static void unit_suspend(int *flags)
{
    *flags |= UNIT_FLAG_SUSPENDED;
}

static void unit_resume(int *flags)
{
    *flags &= ~UNIT_FLAG_SUSPENDED;
}

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_suspend__I);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_resume__I);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_initElevatorFunc__);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_map_shadow__I);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_renderCommand__I);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setFilter__I);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setFilterParam__aF);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setShadow__II);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_shadow_clip_scale__F);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_shadow_map_id__I);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_shadow_map_reset__);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setSortOffset__F);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setClip__I);

INCLUDE_ASM("asm/main/nonmatchings/unit", Java_xeno_Unit_setMonitorPrio__I);
