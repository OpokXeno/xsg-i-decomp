#include "common.h"
#include "shared.h"
#include "ssd_init.h"

/*
 * libkernel syscall stub (main:0x00200550).
 */
extern int iSignalSema(int sema_id);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdInit);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdQuit);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", RSsdSifRpcThread);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", RssdInitIop);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", RssdSifRpcServer);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", RssdBusy);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", RssdSpuRead);

void RssdBackgroundNextWave(const int *request)
{
    RssdWork.flags &= ~4;
    if (request[4] >= 0)
        WakeupThread(RssdWork.next_wave_thread_id);
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", RssdBackNextWaveThread);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", RssdCallFunc);

/*
 * SIF RPC end callback for the RSSD work area: clears flag bit 3, copies the
 * 32-byte receive record into RssdWork, reports a status (-1 on a nonzero
 * result, else flag bit 5) through the registered one-shot completion
 * callback, then signals the waiting thread's semaphore.
 *
 * It runs from the SIF RPC interrupt context, so it ends with the
 * ee-interrupt-handler-return primitive: `sync.l`
 * orders every earlier load/store, including the iSignalSema effects, before
 * `ei` re-enables interrupts on the way out.
 */
void RssdSifRpcCallback(void)
{
    int status;

    RssdWork.flags &= ~0x8;
    RssdWork.response = *RssdWork.response_source;

    if (RssdWork.response.result != 0)
        status = -1;
    else
        status = (RssdWork.flags >> 5) & 1;

    if (RssdWork.complete_callback != 0)
        RssdWork.complete_callback(status, &RssdWork.response, RssdWork.callback_arg);

    RssdWork.complete_callback = 0;
    iSignalSema(RssdWork.sema_id);
    __asm__ __volatile__("sync.l" : : : "memory");
    __asm__ __volatile__("ei" : : : "memory");
    return;
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", RssdFuncCallCompleted);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", RssdGetCallCompletedCode);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdGetResultValue);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdGetResultParam);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdSetServerCallback);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdSetFuncCallback);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdSetStreamEndCallback);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdResume);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdSuspend);
