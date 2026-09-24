/*
 * OV12 original TU 11: 0x00a0ece8..0x00a0f340 (10 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_shot_thread.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

/*
 * Scaffold-owned (.rodata still owner: asm): the assert_prog expression and
 * source-file strings this allocation's assertions cite. ov12:0x00a523f0
 * holds "pThread != NIL", ov12:0x00a52400 holds "../rg_shot_thread.euc.c".
 */
extern const char D_00A523F0[];
extern const char D_00A52400[];

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);

typedef struct XrgActor XrgActor;

extern XrgActor *RgWeaponGetActor(RgWeapon *weapon);
extern int RgWeaponGetControlFlag(RgWeapon *weapon);

extern int RgWeaponIsAttachedShot(RgWeapon *weapon);
extern void RgWeaponSetdown(RgWeapon *weapon);
extern void RgWeaponShotStop(RgWeapon *weapon);

extern RgShotMotCont *CreateRgShotMotCont(XrgActor *pParentActor,
                                          XrgActor *attachActor,
                                          unsigned int eArmType);

extern void RgShotMotContStop(RgShotMotCont *motCont);
extern void DisposeRgShotMotCont(RgShotMotCont *motCont);

/*
 * _SetShotThread (ov12:0x00a0ece8) is not part of this allocation and stays
 * INCLUDE_ASM. DisposeRgShotThread's call (ov12:0x00a0eee8, weapon 0,
 * motionId -1, shotIndex 0) and RgShotThreadInit's plain tail jump into it
 * (ov12:0x00a0ef6c, forwarding whatever its own caller passed) are this
 * allocation's only evidence for its parameter types.
 */
static void _SetShotThread(RgShotThread *thread, RgWeapon *weapon,
                           int motionId, int shotIndex);

/*
 * Supersedes the comment above: _SetShotThread is part of this allocation
 * and is fully defined below, not INCLUDE_ASM. Its 4th parameter is passed
 * unchanged into CreateRgShotMotCont's pParentActor slot
 * (ov12:0x00a0ed8c/0x00a0ed94, src/ov12/rg_shotmot_control.c
 * CreateRgShotMotCont), so it is genuinely an XrgActor pointer.
 * RgShotThreadInit (ov12:0x00a0ef60, outside this allocation) already
 * declares its own matching parameter `int shotIndex` and just forwards
 * it, so this parameter keeps that name and type here too and is
 * reinterpreted as a pointer where it is used as one.
 */
void _SetShotThread(RgShotThread *thread, RgWeapon *weapon, int motionId,
                    int shotIndex)
{
    RgShotMotCont *motCont;
    XrgActor *parentActor;

    if (thread == 0) {
        assert_prog(D_00A523F0, D_00A52400, 0x32);
    }
    motCont = thread->motCont;
    if (motCont != 0) {
        DisposeRgShotMotCont(motCont);
        thread->motCont = 0;
    }
    /*
     * RgShotThreadInit (ov12:0x00a0ef60, already published, outside this
     * allocation) forwards this word as the int shotIndex it already
     * declares; CreateRgShotMotCont's own published prototype
     * (src/ov12/rg_shotmot_control.c) reads the same word as its
     * XrgActor *pParentActor argument, so it is reinterpreted here.
     */
    parentActor = (XrgActor *) shotIndex;
    if (weapon != 0 && parentActor != 0 && RgWeaponGetActor(weapon) != 0) {
        thread->weapon = weapon;
        thread->status = 0;
        if (motionId != 2 && !(RgWeaponGetControlFlag(weapon) & 0x20)) {
            thread->motCont = CreateRgShotMotCont(parentActor,
                                                  RgWeaponGetActor(weapon),
                                                  motionId);
        }
    } else {
        motCont = thread->motCont;
        thread->status = 5;
        thread->weapon = 0;
        if (motCont != 0) {
            DisposeRgShotMotCont(motCont);
            thread->motCont = 0;
        }
    }
    thread->elapsedTime = 0.0f;
    thread->motionId = -1;
}

static void _InitShotThread(RgShotThread *thread)
{
    if (thread == 0) {
        assert_prog(D_00A523F0, D_00A52400, 0x59);
    }
    thread->weapon = 0;
    thread->motCont = 0;
    thread->motionId = -1;
    thread->status = 5;
    thread->elapsedTime = 0.0f;
    thread->active = 0;
}

RgShotThread *CreateRgShotThread(void)
{
    RgShotThread *thread;

    thread = RgHeapAlloc(InstanceOfRgHeap(), 0x24, D_00A52400, 0x69);
    _InitShotThread(thread);
    return thread;
}

void DisposeRgShotThread(RgShotThread *thread)
{
    if (thread == 0) {
        assert_prog(D_00A523F0, D_00A52400, 0x71);
    }
    _SetShotThread(thread, 0, -1, 0);
    RgHeapFree(InstanceOfRgHeap(), thread, D_00A52400, 0x73);
}

int RgShotThreadGetStatus(RgShotThread *thread)
{
    if (thread == 0) {
        assert_prog(D_00A523F0, D_00A52400, 0x7D);
    }
    return thread->status;
}

void RgShotThreadInit(RgShotThread *thread, RgWeapon *weapon, int motionId,
                      int shotIndex)
{
    _SetShotThread(thread, weapon, motionId, shotIndex);
}

void RgShotThreadSleep(RgShotThread *thread)
{
    if (thread == 0) {
        assert_prog(D_00A523F0, D_00A52400, 0x90);
    }
    if (thread->status != 5) {
        thread->status = 0;
        thread->motionId = -1;
        if (thread->motCont != 0) {
            RgShotMotContStop(thread->motCont);
        }
        if (RgWeaponIsAttachedShot(thread->weapon) != 0) {
            RgWeaponShotStop(thread->weapon);
        }
        RgWeaponSetdown(thread->weapon);
    }
}

void RgShotThreadMotOff(RgShotThread *thread)
{
    if (thread == 0) {
        assert_prog(D_00A523F0, D_00A52400, 0xA7);
    }
    if (thread->motCont != 0) {
        DisposeRgShotMotCont(thread->motCont);
        thread->motCont = 0;
    }
}

extern void RgShotMotContPlay(RgShotMotCont *motCont, int motion);

/*
 * Parameter block RgShotThreadStart receives from its caller (still
 * INCLUDE_ASM _ExecShotOrAttackCmd, ov12:0x00a07758): the motion id to
 * play plus the frame/duration values it copies into the thread's own
 * frame/duration fields (ov12:0x00a0f0c4/0x00a0f0e0).
 */
typedef struct RgShotMotStartInfo {
    int motion;
    float frame;
    float duration;
} RgShotMotStartInfo;

void RgShotThreadStart(RgShotThread *thread, RgShotMotStartInfo *info)
{
    int motion;

    if (thread == 0) {
        assert_prog(D_00A523F0, D_00A52400, 0xB6);
    }
    if (thread->status != 5) {
        if (thread->status == 0 || thread->motionId != info->motion) {
            thread->motionId = info->motion;
            thread->frame = info->frame;
            motion = info->motion;
            thread->status = 1;
            thread->duration = info->duration;
            thread->elapsedTime = 0.0f;
            if (motion != -1 && thread->motCont != 0) {
                RgShotMotContPlay(thread->motCont, motion);
            }
        }
        thread->active = 1;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_shot_thread", RgShotThreadPassTime);
