#include "common.h"
#include "shared.h"
#include "ssd_init.h"

/*
 * libkernel syscall stub (main:0x00200550).
 */
extern int iSignalSema(int sema_id);

/*
 * RssdInitIop is this TU's own IOP sound RPC client setup (its body is still
 * scaffold below); sceSifRpcLoop is the SCE SDK SIF RPC server loop.
 */
extern void RssdInitIop(void);
extern void sceSifRpcLoop(void *queue);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdInit);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdQuit);

/*
 * Initializes the IOP sound RPC client, then runs the SIF RPC receive loop
 * on the queue reserved at RssdWork.rpc_queue. It never returns.
 */
void RSsdSifRpcThread(void)
{
    RssdInitIop();
    sceSifRpcLoop(RssdWork.rpc_queue);
}

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

/* Returns the bit RssdSifRpcCallback clears on RPC completion. */
int RssdGetCallCompletedCode(void)
{
    return (RssdWork.flags >> 3) & 1;
}

extern int printf(const char *format, ...);
extern const char D_004D4AA0[]; /* "Rssd get result error !" */

int SsdGetResultValue(int *value)
{
    int result;

    if (RssdWork.flags & 0x8) {
        result = (RssdWork.flags & RSSD_FLAG_SUCCESS) ? -1 : -2;
    } else if (RssdWork.response.error_code >= 0) {
        result = (RssdWork.flags & RSSD_FLAG_SUCCESS) == 0;
        if (value != 0)
            *value = RssdWork.response.value;
    } else {
        result = -3;
        printf(D_004D4AA0);
    }
    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdGetResultParam);

void SsdSetServerCallback(void *callback, void *arg)
{
    RssdWork.server_callback = callback;
    RssdWork.server_callback_arg = arg;
}

void SsdSetFuncCallback(void (*callback)(int status, RssdRpcResponse *response,
                                         void *arg),
                        void *arg)
{
    RssdWork.complete_callback = callback;
    RssdWork.callback_arg = arg;
}

void SsdSetStreamEndCallback(void *callback, void *arg)
{
    RssdWork.stream_end_callback = callback;
    RssdWork.stream_end_callback_arg = arg;
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdResume);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdSuspend);
