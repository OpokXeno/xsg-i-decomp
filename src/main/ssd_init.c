#include "common.h"
#include "shared.h"
#include "ssd_init.h"
#include "main/ssd_init.h"

/* Commands the IOP sends to RssdSifRpcServer (the SPU read and next-wave ones echo the client command id). */
enum {
    RSSD_SERVER_STREAM_END     = 10,
    RSSD_SERVER_SPU_READ       = 27,
    RSSD_SERVER_NEXT_WAVE      = 37,
    RSSD_SERVER_SAMPLE_DMA     = 218,
    RSSD_SERVER_SAMPLE_KEYOFF  = 219,
    RSSD_SERVER_BUSY           = 4096
};

/* SIF RPC ids of the IOP sound driver's client and server. */
#define RSSD_RPC_CLIENT_ID 2304
#define RSSD_RPC_SERVER_ID 2305
/* Buffers are aligned to 64 bytes inside their storage. */
#define RSSD_BUFFER_ALIGN 64
#define RSSD_CLIENT_STORAGE_BYTES 65600
#define RSSD_SERVER_STORAGE_BYTES 65568
#define RSSD_THREAD_STACK_BYTES 16384
#define RSSD_THREAD_PRIORITY 6
/* Sound stream channels reported by the stream-end callback bitmask. */
#define RSSD_STREAM_CHANNELS 4

extern const char rssdBindErrorFormat[];
extern const char rssdSifTransferSizeOverMessage[];
extern const char rssdGetResultErrorMessage[32];
extern void *_gp;

extern void sceSifRegisterRpc(void *server, int rpc_number, void *(*function)(),
                              void *buffer, void *(*cfunction)(), void *cbuffer, void *queue);

void RssdBusy(RssdRequest *request);
void RssdSpuRead(RssdRequest *request);

void *RssdSifRpcServer(unsigned int command, void *data, int size);

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

/* SIF RPC middleware bulk memory copy. */
extern void SsdCopyMemory(void *dst, void *src, int size);
extern int printf(const char *format, ...);
extern int SignalSema(int sema_id);
extern int CreateSema(SsdSemaCreateInfo *sema);
extern int CreateThread(SsdThreadCreateInfo *thread);
extern int StartThread(int thread_id, void *args);
extern int WaitSema(int sema_id);

/*
 * Initializes the IOP sound RPC client, then runs the SIF RPC receive loop
 * on the queue reserved at RssdWork.rpc_queue. It never returns.
 */
extern void RSsdSifRpcThread(void);
extern void RssdBackNextWaveThread(void);

const char rssdSifTransferSizeOverMessage[32] = "Rssd sif transfer size over !!\n";

const char rssdBindErrorFormat[32] = "Rssd(%04x) bind error !\n";

const char rssdGetResultErrorMessage[32] = "Rssd get result error !\n";

static unsigned char MYclientPtr[RSSD_CLIENT_STORAGE_BYTES];

static unsigned char MYserverPtr[RSSD_SERVER_STORAGE_BYTES];

static unsigned char MYthreadStack[RSSD_THREAD_STACK_BYTES];

static unsigned char MYwaveTransThStack[RSSD_THREAD_STACK_BYTES];

void SsdInit(int init_size)
{
    SsdSemaCreateInfo sema;
    SsdThreadCreateInfo thread;
    void *gp_value;

    if (RssdWork.flags & RSSD_FLAG_READY)
        return;

    RssdWork.rpc_buffer_size = RSSD_TRANSFER_MAX;
    sema.init_count = 1;
    sema.max_count = 1;
    RssdWork.client_buffer_base = MYclientPtr;
    RssdWork.server_buffer_base = MYserverPtr;
    RssdWork.init_parameter = init_size;
    sema.option = 0;
    RssdWork.response_source = (RssdRpcResponse *)(((unsigned int)MYclientPtr + RSSD_BUFFER_ALIGN - 1) & -RSSD_BUFFER_ALIGN);
    RssdWork.server_buffer = (void *)(((unsigned int)MYserverPtr + RSSD_BUFFER_ALIGN - 1) & -RSSD_BUFFER_ALIGN);
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
    thread.stack_size = RSSD_THREAD_STACK_BYTES;
    thread.initial_priority = RSSD_THREAD_PRIORITY;
    gp_value = &_gp;
    thread.gp_reg = gp_value;
    thread.option = 0;
    RssdWork.rpc_thread_id = CreateThread(&thread);
    StartThread(RssdWork.rpc_thread_id, 0);

    RssdWork.next_wave_thread_stack = MYwaveTransThStack;
    thread.func = RssdBackNextWaveThread;
    thread.stack = RssdWork.next_wave_thread_stack;
    thread.stack_size = RSSD_THREAD_STACK_BYTES;
    thread.initial_priority = RSSD_THREAD_PRIORITY;
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

    if (sceSifBindRpc(RssdWork.rpc_client, RSSD_RPC_CLIENT_ID, 1) < 0)
        return;
    while (sceSifCheckStatRpc(RssdWork.rpc_client) != 0)
        ;
    if (RssdWork.rpc_client_serve == 0) {
        printf(rssdBindErrorFormat, RSSD_RPC_CLIENT_ID);
        return;
    }
    RssdWork.flags = RSSD_FLAG_READY;
    sceSifSetRpcQueue(RssdWork.rpc_queue, GetThreadId());
    sceSifRegisterRpc(RssdWork.rpc_server, RSSD_RPC_SERVER_ID, RssdSifRpcServer,
                      RssdWork.server_buffer, 0, 0, RssdWork.rpc_queue);
    request.arg[0].value = RssdWork.init_parameter;
    RssdCallFunc(RSSD_CMD_INIT, &request, 0, 0);
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
    case RSSD_SERVER_BUSY:
        RssdBusy(data);
        break;
    case RSSD_SERVER_SPU_READ:
        RssdSpuRead(data);
        break;
    case RSSD_SERVER_NEXT_WAVE:
        RssdBackgroundNextWave(data);
        break;
    case RSSD_SERVER_SAMPLE_DMA:
        if (RssdWork.sample_dma_callback != 0)
            RssdWork.sample_dma_callback(RssdWork.sample_dma_callback_arg);
        break;
    case RSSD_SERVER_SAMPLE_KEYOFF:
        if (RssdWork.sample_keyoff_callback != 0)
            RssdWork.sample_keyoff_callback(0, RssdWork.sample_keyoff_callback_arg);
        break;
    case RSSD_SERVER_STREAM_END:
        if (RssdWork.stream_end_callback != 0) {
            for (channel = 0; channel < RSSD_STREAM_CHANNELS; channel++) {
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
        RssdWork.flags &= ~RSSD_FLAG_SPU_DMA;
}

void RssdBackgroundNextWave(const int *request)
{
    RssdWork.flags &= ~RSSD_FLAG_SPU_DMA;
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
    maximum_transfer_size = RSSD_TRANSFER_MAX;
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
        work->flags |= RSSD_FLAG_SUCCESS | RSSD_FLAG_SPU_DMA;
        work->spu_bytes_remaining = remaining_after_transfer;
        request.arg[0].pointer = write_ptr;
        request.arg[1].value = transfer_size;
        RssdCallFunc(RSSD_CMD_NEXT_WAVE_DATA, &request, write_ptr, transfer_size);
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
    RssdWork.flags |= RSSD_FLAG_CALL_PENDING;
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
    if (total_size > RSSD_PACKET_MAX) {
        printf(rssdSifTransferSizeOverMessage);
        RssdWork.flags &= ~RSSD_FLAG_CALL_PENDING;
        SignalSema(RssdWork.sema_id);
        return -1;
    }
    if (size != 0)
        SsdCopyMemory(packet + 1, data, size);
    FlushCache(0);
    result = sceSifCallRpc(RssdWork.rpc_client, command, 1, packet, total_size,
                           packet, sizeof(*packet), RssdSifRpcCallback, &RssdWork);
    if (result < 0) {
        RssdWork.flags &= ~RSSD_FLAG_CALL_PENDING;
        SignalSema(RssdWork.sema_id);
    } else {
        RssdFuncCallCompleted(1);
    }
    return result;
}

void RssdSifRpcCallback(void)
{
    int status;

    RssdWork.flags &= ~RSSD_FLAG_CALL_PENDING;
    RssdWork.response = *RssdWork.response_source;

    if (RssdWork.response.result != 0)
        status = -1;
    else
        status = (RssdWork.flags >> RSSD_FLAG_SUCCESS_SHIFT) & 1;

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
    return (RssdWork.flags >> RSSD_FLAG_CALL_PENDING_SHIFT) & 1;
}

int SsdGetResultValue(int *value)
{
    int result;

    if (RssdWork.flags & RSSD_FLAG_CALL_PENDING) {
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
    if (flags & RSSD_FLAG_CALL_PENDING) {
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
