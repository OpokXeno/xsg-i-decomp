#include "common.h"

extern void sceSifRegisterRpc(void *server, int rpc_number, void *(*function)(),
                              void *buffer, void *(*cfunction)(), void *cbuffer, void *queue);

#include "shared.h"

#include "ssd_init.h"
void RssdBusy(RssdRequest *request);
void RssdSpuRead(RssdRequest *request);

/*
 * libkernel syscall stub (main:0x00200550).
 */

void *RssdSifRpcServer(unsigned int command, void *data, int size);

extern const char rssdBindErrorFormat[];

extern const char rssdSifTransferSizeOverMessage[];

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

extern int RssdFuncCallCompleted(int status);

extern void *sceSifRemoveRpc(void *sd, void *qd);

/*
 * Initializes the IOP sound RPC client, then runs the SIF RPC receive loop
 * on the queue reserved at RssdWork.rpc_queue. It never returns.
 */

/* SIF RPC middleware bulk memory copy. */

extern void SsdCopyMemory(void *dst, void *src, int size);

extern int printf(const char *format, ...);

extern const char rssdGetResultErrorMessage[32];

/*
 * libkernel syscall stub (main:0x00200550).
 */

extern int SignalSema(int sema_id);

extern int CreateSema(SsdSemaCreateInfo *sema);
extern int CreateThread(SsdThreadCreateInfo *thread);
extern int StartThread(int thread_id, void *args);
extern int WaitSema(int sema_id);
extern void *_gp;
static unsigned char MYclientPtr[65600];
static unsigned char MYserverPtr[65568];
static unsigned char MYthreadStack[16384];
static unsigned char MYwaveTransThStack[16384];
extern void RSsdSifRpcThread(void);
extern void RssdBackNextWaveThread(void);

void SsdInit(int init_size)
{
    SsdSemaCreateInfo sema;
    SsdThreadCreateInfo thread;
    void *gp_value;

    if (RssdWork.flags & 1)
        return;

    RssdWork.rpc_buffer_size = 0x10000;
    sema.init_count = 1;
    sema.max_count = 1;
    RssdWork.client_buffer_base = MYclientPtr;
    RssdWork.server_buffer_base = MYserverPtr;
    RssdWork.init_parameter = init_size;
    sema.option = 0;
    RssdWork.response_source = (RssdRpcResponse *)(((unsigned int)MYclientPtr + 63) & -64);
    RssdWork.server_buffer = (void *)(((unsigned int)MYserverPtr + 63) & -64);
    RssdWork.flags = 0;
    RssdWork.request_parameter = 0;
    RssdWork.sample_rate = 0;
    RssdWork.request_channel_count = 0;
    RssdWork.request_size = 0;
    RssdWork.server_call_count = 0;
    RssdWork.server_callback = 0;
    RssdWork.server_callback_arg = 0;
    RssdWork.complete_callback = 0;
    RssdWork.callback_arg = 0;
    RssdWork.stream_end_callback = 0;
    RssdWork.stream_end_callback_arg = 0;
    RssdWork.sema_id = CreateSema(&sema);

    sema.max_count = 1;
    sema.init_count = 0;
    sema.option = 0;
    RssdWork.busy_sema_id = CreateSema(&sema);

    RssdWork.rpc_thread_stack = MYthreadStack;
    thread.func = RSsdSifRpcThread;
    thread.stack = RssdWork.rpc_thread_stack;
    thread.stack_size = 16384;
    thread.initial_priority = 6;
    gp_value = &_gp;
    thread.gp_reg = gp_value;
    thread.option = 0;
    RssdWork.rpc_thread_id = CreateThread(&thread);
    StartThread(RssdWork.rpc_thread_id, 0);

    RssdWork.next_wave_thread_stack = MYwaveTransThStack;
    thread.func = RssdBackNextWaveThread;
    thread.stack = RssdWork.next_wave_thread_stack;
    thread.stack_size = 16384;
    thread.initial_priority = 6;
    thread.gp_reg = gp_value;
    thread.option = 0;
    RssdWork.next_wave_thread_id = CreateThread(&thread);
    StartThread(RssdWork.next_wave_thread_id, 0);

    WaitSema(RssdWork.busy_sema_id);
    DeleteSema(RssdWork.busy_sema_id);
}

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

void RssdInitIop(void)
{
    RssdRequest request;

    if (sceSifBindRpc(RssdWork.rpc_client, 2304, 1) < 0)
        return;
    while (sceSifCheckStatRpc(RssdWork.rpc_client) != 0)
        ;
    if (RssdWork.rpc_client_serve == 0) {
        printf(rssdBindErrorFormat, 2304);
        return;
    }
    RssdWork.flags = 1;
    sceSifSetRpcQueue(RssdWork.rpc_queue, GetThreadId());
    sceSifRegisterRpc(RssdWork.rpc_server, 2305, RssdSifRpcServer,
                      RssdWork.server_buffer, 0, 0, RssdWork.rpc_queue);
    request.arg[0].value = RssdWork.init_parameter;
    RssdCallFunc(1, &request, 0, 0);
}

void *RssdSifRpcServer(unsigned int command, void *data, int size)
{
    RssdRpcResponse *packet = data;
    RssdRequest *request = data;
    int channel;

    RssdWork.server_packet = *packet;
    RssdWork.server_packet.error_code = command;
    RssdWork.server_packet.result = 0;
    RssdWork.server_call_count++;
    switch (command) {
    case 4096:
        RssdBusy(data);
        break;
    case 27:
        RssdSpuRead(data);
        break;
    case 37:
        RssdBackgroundNextWave(data);
        break;
    case 218:
        if (RssdWork.sample_dma_callback != 0)
            RssdWork.sample_dma_callback(RssdWork.sample_dma_callback_arg);
        break;
    case 219:
        if (RssdWork.sample_keyoff_callback != 0)
            RssdWork.sample_keyoff_callback(0, RssdWork.sample_keyoff_callback_arg);
        break;
    case 10:
        if (RssdWork.stream_end_callback != 0) {
            for (channel = 0; channel < 4; channel++) {
                if ((request->arg[0].value >> channel) & 1)
                    RssdWork.stream_end_callback(channel, RssdWork.stream_end_callback_arg);
            }
        }
        break;
    default:
        if (RssdWork.server_callback != 0)
            RssdWork.server_callback(command, data, RssdWork.server_callback_arg);
        break;
    }
    return data;
}

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

int RssdCallFunc(int command, RssdRequest *request, void *data, int size)
{
    RssdRpcResponse *packet;
    RssdRpcResponse *source;
    void *record;
    int total_size;
    int result;

    WaitSema(RssdWork.sema_id);
    if (data == 0)
        size = 0;
    RssdWork.flags |= 0x8;
    RssdWork.response.error_code = -1;
    packet = RssdWork.response_source;
    if (request != 0) {
        record = request;
        source = record;
        *packet = *source;
    }
    packet->error_code = command;
    packet->data_size = size;
    total_size = ((size + 15) & -16) + sizeof(*packet);
    if (total_size > 0x10020) {
        printf(rssdSifTransferSizeOverMessage);
        RssdWork.flags &= ~0x8;
        SignalSema(RssdWork.sema_id);
        return -1;
    }
    if (size != 0)
        SsdCopyMemory(packet + 1, data, size);
    FlushCache(0);
    result = sceSifCallRpc(RssdWork.rpc_client, command, 1, packet, total_size,
                           packet, sizeof(*packet), RssdSifRpcCallback, &RssdWork);
    if (result < 0) {
        RssdWork.flags &= ~0x8;
        SignalSema(RssdWork.sema_id);
    } else {
        RssdFuncCallCompleted(1);
    }
    return result;
}

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

/*
 * Polls RssdGetCallCompletedCode until the pending RPC call has completed
 * (code 0) or, with status 0, just once; returns the last code. The return
 * value is real: it stays in $2 while the loop tests a copy in $3.
 */
int RssdFuncCallCompleted(int status)
{
    int code;

    do {
        code = RssdGetCallCompletedCode();
    } while (status != 0 && code != 0);
    return code;
}

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
        printf(rssdGetResultErrorMessage);
    }
    return result;
}

int SsdGetResultParam(RssdRpcResponse *out)
{
    int flags;
    int result;

    flags = RssdWork.flags;
    if (flags & 0x8) {
        result = (flags & RSSD_FLAG_SUCCESS) ? -1 : -2;
    } else if (RssdWork.response.error_code >= 0) {
        *out = RssdWork.response;
        result = (flags & RSSD_FLAG_SUCCESS) == 0;
    } else {
        result = -3;
    }
    return result;
}

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

const char rssdSifTransferSizeOverMessage[32] = "Rssd sif transfer size over !!\n";

const char rssdBindErrorFormat[32] = "Rssd(%04x) bind error !\n";

const char rssdGetResultErrorMessage[32] = "Rssd get result error !\n";
