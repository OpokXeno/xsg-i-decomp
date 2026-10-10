#include "common.h"

#include "shared.h"

#include "ssd_init.h"

/*
 * libkernel syscall stub (main:0x00200550).
 */

extern int iSignalSema(int sema_id);

extern void SleepThread(void);

/*
 * RssdInitIop is this TU's own IOP sound RPC client setup (its body is still
 * scaffold below); sceSifRpcLoop is the SCE SDK SIF RPC server loop.
 */

extern void RssdInitIop(void);

extern void sceSifRpcLoop(void *queue);

enum {
    RSSD_CMD_QUIT    = 0x02,
    RSSD_CMD_RESUME  = 0x05,
    RSSD_CMD_SUSPEND = 0x06,
    RSSD_FLAG_BUSY   = 0x02
};

/*
 * libkernel syscall stubs.
 */

extern int DeleteSema(int sema_id);

extern int DeleteThread(int thread_id);

extern int TerminateThread(int thread_id);

/*
 * This TU's own RPC completion handler (its body is still scaffold below);
 * sceSifRemoveRpc is the SCE SDK RPC server deregistration call.
 */

extern void RssdFuncCallCompleted(int status);

extern void *sceSifRemoveRpc(void *sd, void *qd);

/*
 * Initializes the IOP sound RPC client, then runs the SIF RPC receive loop
 * on the queue reserved at RssdWork.rpc_queue. It never returns.
 */

/* SIF RPC middleware bulk memory copy. */

extern void SsdCopyMemory(void *dst, void *src, int size);

extern int printf(const char *format, ...);

const char D_004D4AA0[] = "Rssd get result error !\n";

/*
 * libkernel syscall stub (main:0x00200550).
 */

extern int SignalSema(int sema_id);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", SsdInit);

void SsdQuit(void)
{
    RssdRequest request;

    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_QUIT, &request, 0, 0);
    RssdFuncCallCompleted(1);
    DeleteSema(RssdWork.sema_id);
    sceSifRemoveRpc(RssdWork.rpc_server, RssdWork.rpc_queue);
    TerminateThread(RssdWork.rpc_thread_id);
    DeleteThread(RssdWork.rpc_thread_id);
    TerminateThread(RssdWork.next_wave_thread_id);
    DeleteThread(RssdWork.next_wave_thread_id);
    RssdWork.flags = 0;
}

void RSsdSifRpcThread(void)
{
    RssdInitIop();
    sceSifRpcLoop(RssdWork.rpc_queue);
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", RssdInitIop);

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", RssdSifRpcServer);

void RssdBusy(RssdRequest *request)
{
    int request_size;
    short request_channel_count;

    RssdWork.flags |= RSSD_FLAG_BUSY;
    RssdWork.request_parameter = request->arg[0].value;
    request_channel_count = request->arg[2].rate_channel.request_channel_count;
    request_size = request->arg[3].value;
    RssdWork.sample_rate = request->arg[2].rate_channel.sample_rate;
    RssdWork.request_channel_count = request_channel_count;
    RssdWork.request_size = request_size;
    SignalSema(RssdWork.busy_sema_id);
}

void RssdSpuRead(RssdRequest *request)
{
    int size;
    int more_data;

    size = request->arg[1].value;
    more_data = request->arg[2].value;
    SsdCopyMemory(RssdWork.spu_write_ptr, request + 1, size);
    RssdWork.spu_write_ptr += size;
    if (more_data == 0)
        RssdWork.flags &= ~4;
}

void RssdBackgroundNextWave(const int *request)
{
    RssdWork.flags &= ~4;
    if (request[4] >= 0)
        WakeupThread(RssdWork.next_wave_thread_id);
}

void RssdBackNextWaveThread(void)
{
    RssdRequest request;
    RssdWorkFlags *work;
    unsigned char *write_ptr;
    int remaining_size;
    int remaining_after_transfer;
    int maximum_transfer_size;
    int transfer_size;

    work = &RssdWork;
    maximum_transfer_size = 0x10000;
    for (;;) {
        SleepThread();
        remaining_size = work->spu_bytes_remaining;
        if (remaining_size == 0)
            continue;

        transfer_size = remaining_size <= maximum_transfer_size ? remaining_size : maximum_transfer_size;
        write_ptr = work->spu_write_ptr;
        remaining_after_transfer = remaining_size - transfer_size;
        work->spu_write_ptr = write_ptr + transfer_size;
        request.arg[2].value = remaining_after_transfer == 0;
        work->flags |= 0x24;
        work->spu_bytes_remaining = remaining_after_transfer;
        request.arg[0].pointer = write_ptr;
        request.arg[1].value = transfer_size;
        RssdCallFunc(33, &request, write_ptr, transfer_size);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_init", RssdCallFunc);

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

int RssdGetCallCompletedCode(void)
{
    return (RssdWork.flags >> 3) & 1;
}

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

void SsdResume(void)
{
    RssdRequest request;

    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_RESUME, &request, 0, 0);
}

void SsdSuspend(void)
{
    RssdRequest request;

    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SUSPEND, &request, 0, 0);
}
