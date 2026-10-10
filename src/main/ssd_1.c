#include "common.h"

/*
 * RssdWork, RssdRequest and RssdCallFunc: the one spelling lives in main/tu110's
 * own header, src/main/ssd_init.h. The generated include/main/ssd_init.h cannot
 * carry RssdWork's type yet, so that
 * header is included directly.
 */

typedef struct RssdRpcResponse {
    short error_code;              /* +0x00: negative -> per-command error */
    short result;                  /* +0x02: nonzero -> error status (-1) */
    int _unmodeled_04;             /* +0x04: 32-bit word, meaning not recovered */
    int value;                     /* +0x08: command result value */
    int _unmodeled_0c[5];          /* +0x0c..0x1f: 32-bit words, not read here */
} RssdRpcResponse;

/* canon: config/header-canon.json chose src/core/main-00240188/private.h over 1 other accepted spelling */
/*
 * RssdWork is the RSSD RPC / background-wave work area, main:0x004aa080,
 * ELF symbol size 0x200 bytes. The accepted RssdWorkFlags in
 * xeno/core/types.h (from RssdBackgroundNextWave, main:0x0023ff28) only
 * carries the leading `flags` word; this function evidences more of the same
 * object, so the type is extended here under the same tag and leading field
 * Only the evidenced members are modeled here. Bytes this TU does not access
 * stay explicit unmodeled spans.
 */
struct SsdMemoryBlock;

typedef struct RssdWorkFlags {
    int flags;                            /* +0x000: bit 3 cleared on RPC completion; bit 5 is the success status */
    unsigned char _unmodeled_004[4];      /* +0x004..0x007 */
    int request_parameter;                /* +0x008: copied by RssdBusy from its request */
    unsigned char _unmodeled_00c[4];      /* +0x00c..0x00f */
    unsigned short sample_rate;           /* +0x010: samples per second used by SsdGetTimeCode */
    short request_channel_count;          /* +0x012: copied by RssdBusy from its request */
    int request_size;                     /* +0x014: copied by RssdBusy from its request */
    unsigned char _unmodeled_018[0x08];   /* +0x018..0x01f */
    /*
     * Stored verbatim by SsdSetServerCallback (main:0x002403b0)
     * `sw $5,36($2)` / `sw $4,32($2)`, $2 = &RssdWork. No recovered function
     * reads them back, so their call signature is not evidenced and they
     * stay untyped pointers named after the store site.
     */
    void *server_callback;                /* +0x020: SsdSetServerCallback arg0 */
    void *server_callback_arg;            /* +0x024: SsdSetServerCallback arg1 */
    void (*complete_callback)(int status, RssdRpcResponse *response,
                              void *arg); /* +0x028: one-shot, cleared after use */
    void *callback_arg;                   /* +0x02c: third complete_callback argument */
    unsigned char _unmodeled_030[0x04];   /* +0x030..0x033 */
    RssdRpcResponse *response_source;     /* +0x034: SIF RPC receive buffer */
    unsigned char _unmodeled_038[0x48];   /* +0x038..0x07f */
    RssdRpcResponse response;             /* +0x080..0x09f: copy of *response_source */
    unsigned char _unmodeled_0a0[0xa8];   /* +0x0a0..0x147 */
    /*
     * The SCE SDK RPC server registration record: RssdInitIop
     * (main:0x0023fbb8, still asm) passes its address as the `sd` argument of
     * sceSifRegisterRpc (`addiu $4,$17,0x148`), and SsdQuit (main:0x0023fb08)
     * passes the same address to sceSifRemoveRpc to unregister it on the way
     * out. Its internal layout belongs to the SIF RPC middleware, not this
     * TU, so the reserved extent runs to the next evidenced field at +0x18c.
     */
    unsigned char rpc_server[0x44];       /* +0x148..0x18b: sceSifRpcServerData_t */
    /*
     * The SIF RPC receive queue RSsdSifRpcThread (main:0x0023fb90) hands to
     * sceSifRpcLoop after RssdInitIop returns (`addiu a0,v0,-24052` off
     * `lui v0,0x4b`, v0 = &RssdWork, i.e. &RssdWork + 0x18c). Its internal
     * layout belongs to the SIF RPC middleware, not this TU, and is not
     * evidenced by any recovered function; the reserved extent runs to the
     * next evidenced field at +0x1a4.
     */
    unsigned char rpc_queue[0x18];        /* +0x18c..0x1a3: sceSifRpcLoop queue */
    /*
     * The two service threads SsdInit (main:0x0023f960) creates, each stored
     * as the (thread id, stack) pair the create/start idiom produces
     * ($20 = &RssdWork throughout that function):
     * - +0x1a4/+0x1a8: RSsdSifRpcThread, `sw $2,0x1a4($20)` in the delay slot
     *   of `jal StartThread` at 0x0023fa88 and `sw $5,0x1a8($20)` at
     *   0x0023fa74 with $5 = MYthreadStack;
     * - +0x1ac/+0x1b0: RssdBackNextWaveThread, `sw $2,0x1ac($20)` in the
     *   delay slot of `jal StartThread` at 0x0023facc and `sw $3,0x1b0($20)`
     *   in the delay slot of its `jal CreateThread` at 0x0023fabc with
     *   $3 = MYwaveTransThStack.
     * Both ids are confirmed by a second, independent reader: SsdQuit
     * (main:0x0023fb08) passes +0x1a4 to TerminateThread and DeleteThread
     * (0x0023fb5c/0x0023fb64) and +0x1ac to the same pair
     * (0x0023fb6c/0x0023fb74), next to its DeleteSema of +0x1b4.
     * RssdBackgroundNextWave wakes +0x1ac, which is that same thread.
     */
    int rpc_thread_id;                    /* +0x1a4: RSsdSifRpcThread */
    void *rpc_thread_stack;               /* +0x1a8: MYthreadStack */
    int next_wave_thread_id;              /* +0x1ac: RssdBackNextWaveThread */
    void *next_wave_thread_stack;         /* +0x1b0: MYwaveTransThStack */
    int sema_id;                          /* +0x1b4: signalled when the RPC completes */
    int busy_sema_id;                     /* +0x1b8: signalled by RssdBusy */
    unsigned char _unmodeled_1bc[4];      /* +0x1bc..0x1bf */
    struct SsdMemoryBlock *first_block;   /* +0x1c0: first allocator-list node */
    int memory_end;                       /* +0x1c4: allocator arena end */
    unsigned char _unmodeled_1c8[4];     /* +0x1c8..0x1cb */
    int spu_bytes_remaining;              /* +0x1cc: drained by RssdBackNextWaveThread */
    /*
     * Running destination pointer for streamed sample data. RssdSpuRead
     * (main:0x0023feb8) copies each request's payload here with
     * SsdCopyMemory and advances it by the copied byte count; SsdSpuDirectRead
     * (main:0x00240690, still asm) sets it from its own destination argument.
     */
    unsigned char *spu_write_ptr;         /* +0x1d0: streamed sample write position */
    int wave_id;                          /* +0x1d4: active wave id */
    unsigned char _unmodeled_1d8[4];
    unsigned short wave_chunk_index;     /* +0x1dc: streamed chunk index */
    unsigned char _unmodeled_1de[0x0a];   /* +0x1de..0x1e7 */
    /*
     * Two (callback, argument) pairs stored verbatim by main/tu112:
     * SsdSetSampleDmaCallback (main:0x002410a0) `sw $4,488($2)` /
     * `sw $5,492($2)` and SsdSetSampleKeyoffCallback (main:0x002410b8)
     * `sw $4,496($2)` / `sw $5,500($2)`, $2 = &RssdWork. No recovered
     * function reads them back, so their call signature is not evidenced
     * and they stay untyped pointers named after the store site.
     */
    void *sample_dma_callback;            /* +0x1e8: SsdSetSampleDmaCallback arg0 */
    void *sample_dma_callback_arg;        /* +0x1ec: SsdSetSampleDmaCallback arg1 */
    void *sample_keyoff_callback;         /* +0x1f0: SsdSetSampleKeyoffCallback arg0 */
    void *sample_keyoff_callback_arg;     /* +0x1f4: SsdSetSampleKeyoffCallback arg1 */
    /*
     * Stored verbatim by SsdSetStreamEndCallback (main:0x002403e0)
     * `sw $5,508($2)` / `sw $4,504($2)`, $2 = &RssdWork. No recovered
     * function reads them back, so their call signature is not evidenced
     * and they stay untyped pointers named after the store site.
     */
    void *stream_end_callback;            /* +0x1f8: SsdSetStreamEndCallback arg0 */
    void *stream_end_callback_arg;        /* +0x1fc: SsdSetStreamEndCallback arg1 */
} RssdWorkFlags;

/* RssdWorkFlags.flags bit 5: set/cleared around an RssdCallFunc call to report the RPC's outcome. */
#define RSSD_FLAG_SUCCESS 0x20


extern RssdWorkFlags RssdWork;

/*
 * One argument word of an RSSD request. Most commands pass plain integers;
 * SsdTransferSampling/SsdTransferSamplingNext (main/tu112) and the sequence
 * data wrappers of main/tu113 pass a buffer address in the same slot, so the
 * word is a union of the two views (both are 32-bit on the EE).
 */
typedef union RssdRequestWord {
    int value;
    void *pointer;
    struct {
        unsigned short sample_rate;
        short request_channel_count;
    } rate_channel;
} RssdRequestWord;

/*
 * The 32-byte RSSD RPC request record.
 *
 * RssdCallFunc (main:0x0023fff0, still INCLUDE_ASM) copies all 32 bytes of a
 * non-null request into the SIF RPC buffer RssdWork.response_source with
 * four unaligned ldl/ldr -> sdl/sdr pairs (0x00240050..0x0024008c), then
 * writes two fields of that copy from its own arguments: the +0x00 halfword
 * (`sh $21,0($17)`, command) and the +0x0c word (`sw $16,12($17)`, size).
 * No caller writes header[0..3] (+0x00..+0x0f), but every wrapper reserves
 * the whole record on its stack (all of them have a 0x30-byte frame, however
 * many argument words the command uses).
 * arg[0..3] (+0x10..+0x1f) are the command's own argument words.
 */
typedef struct RssdRequest {
    int header[4];
    RssdRequestWord arg[4];
} RssdRequest;

/*
 * Sends one RSSD command over SIF RPC (end function RssdSifRpcCallback):
 * `request` may be null, otherwise it is copied as above, and `size` bytes
 * of `data` are copied after the record (SsdCopyMemory into buffer +0x20).
 * Returns -1 when the rounded payload exceeds the buffer, otherwise the
 * sceSifCallRpc result.
 */
int RssdCallFunc(int command, RssdRequest *request, void *data, int size);

/*
 * SIF RPC end callback for the RSSD work area: clears flag bit 3, copies the
 * 32-byte receive record into RssdWork, reports a status (-1 on a nonzero
 * result, else flag bit 5) through the registered one-shot completion
 * callback, then signals the waiting thread's semaphore.
 *
 * It runs from the SIF RPC interrupt context, so it ends with the
 * ee-interrupt-handler-return primitive (docs/ps2-capabilities.md): `sync.l`
 * orders every earlier load/store, including the iSignalSema effects, before
 * `ei` re-enables interrupts on the way out.
 */



extern void *sceSifGetNextRequest(void *queue);

extern void sceSifExecRequest(void *request);

typedef struct RssdEffectData {
    unsigned int tag;
    unsigned char unmodeled_04[4];
    unsigned int size;
} RssdEffectData;

enum {
    RSSD_CMD_STOP_EFFECT_FILE_ID       = 0x7a,
    RSSD_CMD_CHECK_PLAY_EFFECT_ALL     = 0x80,
    RSSD_CMD_CHECK_PLAY_EFFECT         = 0x81,
    RSSD_CMD_CHECK_PLAY_EFFECT_FILE_ID = 0x82,
    RSSD_CMD_STOP_WAVE                 = 0x92,
    RSSD_CMD_INIT_SAMPLING             = 0xd0,
    RSSD_CMD_DISPOSE_SAMPLING          = 0xd1,
    RSSD_CMD_TRANSFER_SAMPLING         = 0xd2,
    RSSD_CMD_TRANSFER_SAMPLING_NEXT    = 0xd3,
    RSSD_CMD_PLAY_SAMPLING             = 0xd4,
    RSSD_CMD_STOP_SAMPLING             = 0xd5,
    RSSD_CMD_SET_SAMPLING_PARAM        = 0xd6,
    RSSD_CMD_SET_SAMPLING_EFFECT       = 0xd7
};

enum {
    RSSD_CMD_SEND_FUNC_PACKET          = 0x04,
    RSSD_CMD_NEXT_WAVE_DATA             = 0x21,
    RSSD_CMD_DISPOSE_WAVE_BANK          = 0x22,
    RSSD_CMD_CHECK_WAVE_DATA            = 0x24,
    RSSD_CMD_SET_SEGMENT_ALLOC_MODE     = 0x2c,
    RSSD_CMD_RESET_SEGMENT_ALLOC_MODE   = 0x2d,
    RSSD_CMD_DISPOSE_EFFECT_DATA        = 0x61,
    RSSD_CMD_CHECK_EFFECT_DATA          = 0x62,
    RSSD_CMD_PLAY_EFFECT_NORMAL         = 0x70,
    RSSD_CMD_PLAY_EFFECT_PARAM          = 0x71,
    RSSD_CMD_STOP_EFFECT                = 0x79,
    RSSD_CMD_SET_EFFECT_PARAM           = 0x7b,
    RSSD_CMD_SET_PLAY_EFFECT_PARAM      = 0x7c,
    RSSD_CMD_CONTINUE_ONE_EFFECT        = 0x7d,
    RSSD_CMD_FADEOUT_EFFECT             = 0x7e,
    RSSD_CMD_FADEOUT_EFFECT_FILE_ID     = 0x7f,
    RSSD_CMD_PLAY_WAVE_NORMAL           = 0x90,
    RSSD_CMD_PLAY_WAVE_PARAM            = 0x91,
    RSSD_CMD_CHECK_SPU_MEMORY           = 0x100
};

/*
 * Returns the SPU-DMA busy bit of RssdWork; a nonzero wait first spins until
 * it clears (main:0x002409b0). Same prototype as src/main/xgl_sound.c and
 * src/ov01/snd.h.
 */

extern int SsdSpuDmaCompleted(int wait);

/*
 * Waits for the previous SPU DMA transfer to complete (SsdSpuDmaCompleted,
 * wait = 1), then sends size bytes of data as the next chunk of streaming
 * wave data for wave. Always returns 0.
 */

/* Forwards value (an SPU memory size or address) unchanged; no further evidenced use in this function. */

enum { RSSD_CMD_STOP_ALL_EFFECT = 0x78 };

extern int printf(const char *format, ...);

/*
 * Sends count 32-byte packets read from packets over SIF RPC; count << 5 is
 * the RssdCallFunc payload size, sizeof(RssdRequest). No-op when count is 0.
 */

typedef struct SsdWaveHeader {
    unsigned int tag;
    unsigned char unmodeled_04[4];
    int size;
    unsigned char unmodeled_0c[6];
    unsigned short id;
} SsdWaveHeader;

#define SSD_WAVE_TAG 0x6d647773

enum {
    RSSD_CMD_ADD_WAVE_DATA = 0x20
};

void SsdSendFuncPacket(void *packets, int count)
{
    RssdRequest request;

    if (count != 0) {
        request.arg[0].pointer = packets;
        request.arg[1].value = count;
        RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
        RssdCallFunc(RSSD_CMD_SEND_FUNC_PACKET, &request, packets, count << 5);
    }
}

int SsdSpuDirectRead(unsigned char *spuPtr, int transSize, int size)
{
    RssdRequest request;

    SsdSpuDmaCompleted(1);
    RssdWork.spu_write_ptr = spuPtr;
    RssdWork.spu_bytes_remaining = size;
    RssdWork.flags = (RssdWork.flags | 4) & ~RSSD_FLAG_SUCCESS;
    RssdWork.wave_chunk_index = 0;
    request.arg[0].value = transSize;
    request.arg[1].value = size;
    if (RssdCallFunc(0x1b, &request, 0, 0) < 0) {
        RssdWork.flags &= ~4;
        printf("Raad spu direct read error !\n");
    }
    return printf("Ssd spu read  SpuPtr=0x%08x  TransSize=0x%0x\n", transSize, size);
}

int SsdAddWaveData(SsdWaveHeader *data, int size, int wave)
{
    /*
     * The request record with plain int argument words: the original's stores
     * do not alias the wave header's id halfword, which RssdRequestWord's
     * halfword view would make them do.
     */
    typedef struct RssdAddWaveRequest {
        int header[4];
        int arg[4];
    } RssdAddWaveRequest;
    RssdAddWaveRequest request;
    int chunk;
    int id;

    SsdSpuDmaCompleted(1);
    if (data->tag != SSD_WAVE_TAG) {
        printf("Rssd add wave data  data error !\n");
        return -1;
    }
    if (size <= 0) {
        size = data->size;
    } else {
        size = data->size < size ? data->size : size;
    }
    id = data->id;
    chunk = size > 0x10000 ? 0x10000 : size;
    request.arg[0] = (int)data;
    request.arg[2] = wave;
    RssdWork.spu_bytes_remaining = size - chunk;
    RssdWork.spu_write_ptr = (unsigned char *)data + chunk;
    RssdWork.flags |= RSSD_FLAG_SUCCESS | 4;
    RssdWork.wave_id = id;
    request.arg[1] = chunk;
    RssdWork.wave_chunk_index = 0;
    if (RssdCallFunc(RSSD_CMD_ADD_WAVE_DATA, (RssdRequest *)&request, data, chunk) < 0) {
        RssdWork.flags &= ~4;
        printf("Raad add wave data !\n");
        return -1;
    }
    return id;
}

int SsdNextWaveData(void *data, int size, int wave)
{
    RssdRequest request;

    SsdSpuDmaCompleted(1);
    request.arg[0].pointer = data;
    request.arg[1].value = size;
    request.arg[2].value = wave;
    RssdWork.flags |= RSSD_FLAG_SUCCESS | 4;
    RssdCallFunc(RSSD_CMD_NEXT_WAVE_DATA, &request, data, size);
    return 0;
}

int SsdSetSegmentAllocMode(int segment, int mode)
{
    RssdRequest request;

    request.arg[0].value = segment;
    request.arg[1].value = mode;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SEGMENT_ALLOC_MODE, &request, 0, 0);
    return 0;
}

int SsdResetSegmentAllocMode(int segment)
{
    RssdRequest request;

    request.arg[0].value = segment;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_RESET_SEGMENT_ALLOC_MODE, &request, 0, 0);
    return 0;
}

void SsdDisposeWaveBank(int wave_bank)
{
    RssdRequest request;

    request.arg[0].value = wave_bank;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_DISPOSE_WAVE_BANK, &request, 0, 0);
}

int SsdCheckWaveData(int wave)
{
    RssdRequest request;

    request.arg[0].value = wave;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_CHECK_WAVE_DATA, &request, 0, 0);
    return 0;
}

int SsdSpuDmaCompleted(int wait)
{
    int busy = (RssdWork.flags >> 2) & 1;

    if (wait) {
        while (busy) {
            void *request;

            while ((request = sceSifGetNextRequest(RssdWork.rpc_queue)) != 0) {
                sceSifExecRequest(request);
            }
            busy = (RssdWork.flags >> 2) & 1;
        }
    }
    return busy;
}

void SsdCheckSpuMemory(int value)
{
    RssdRequest request;

    request.arg[0].value = value;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_CHECK_SPU_MEMORY, &request, 0, 0);
}

int SsdAddEffectData(RssdEffectData *effectData, int unused)
{
    RssdRequest request;
    int size;

    if (effectData->tag != 0x73646573) {
        printf("Rssd effect data error !\n", unused, effectData);
        return -1;
    }

    size = effectData->size;
    request.arg[0].pointer = effectData;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(0x60, &request, effectData, size);
    return RssdWork.response.value;
}

void SsdDisposeEffectData(int effect_bank)
{
    RssdRequest request;

    request.arg[0].value = effect_bank;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_DISPOSE_EFFECT_DATA, &request, 0, 0);
}

int SsdCheckEffectData(int effect_bank)
{
    RssdRequest request;

    request.arg[0].value = effect_bank;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_CHECK_EFFECT_DATA, &request, 0, 0);
    return 0;
}

void SsdPlayEffectNormal(int effect_id, int source_id)
{
    RssdRequest request;

    request.arg[0].value = effect_id;
    request.arg[1].value = source_id;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_PLAY_EFFECT_NORMAL, &request, 0, 0);
}

void SsdPlayEffectParam(int effect_id, int source_id, int volume, int pan)
{
    RssdRequest request;

    request.arg[0].value = effect_id;
    request.arg[1].value = source_id;
    request.arg[2].value = volume;
    request.arg[3].value = pan;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_PLAY_EFFECT_PARAM, &request, 0, 0);
}

void SsdSetPlayEffectParam(int effect_id, int source_id, int volume, int pan)
{
    RssdRequest request;

    request.arg[0].value = effect_id;
    request.arg[1].value = source_id;
    request.arg[2].value = volume;
    request.arg[3].value = pan;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_PLAY_EFFECT_PARAM, &request, 0, 0);
}

void SsdSetEffectParam(int effect_id, int source_id, int volume, int pan)
{
    RssdRequest request;

    request.arg[0].value = effect_id;
    request.arg[1].value = source_id;
    request.arg[2].value = volume;
    request.arg[3].value = pan;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_EFFECT_PARAM, &request, 0, 0);
}

void SsdStopAllEffect(void)
{
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_STOP_ALL_EFFECT, 0, 0, 0);
}

void SsdStopEffect(int effect_id, int source_id)
{
    RssdRequest request;

    request.arg[0].value = effect_id;
    request.arg[1].value = source_id;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_STOP_EFFECT, &request, 0, 0);
}

void SsdContinueOneEffect(int effect_id, int source_id)
{
    RssdRequest request;

    request.arg[0].value = effect_id;
    request.arg[1].value = source_id;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_CONTINUE_ONE_EFFECT, &request, 0, 0);
}

void SsdFadeoutEffect(int effect_id, int source_id, int fade_time)
{
    RssdRequest request;

    request.arg[0].value = effect_id;
    request.arg[1].value = source_id;
    request.arg[2].value = fade_time;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_FADEOUT_EFFECT, &request, 0, 0);
}

void SsdFadeoutEffectFileID(int file_id, int fade_time)
{
    RssdRequest request;

    request.arg[0].value = file_id;
    request.arg[1].value = fade_time;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_FADEOUT_EFFECT_FILE_ID, &request, 0, 0);
}

void SsdPlayWaveNormal(int wave, int source)
{
    RssdRequest request;

    request.arg[0].value = wave;
    request.arg[1].value = source;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_PLAY_WAVE_NORMAL, &request, 0, 0);
}

void SsdPlayWaveParam(int wave, int source, int volume, int pan)
{
    RssdRequest request;

    request.arg[0].value = wave;
    request.arg[1].value = source;
    request.arg[2].value = volume;
    request.arg[3].value = pan;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_PLAY_WAVE_PARAM, &request, 0, 0);
}

void SsdStopWave(int wave, int source)
{
    RssdRequest request;

    request.arg[0].value = wave;
    request.arg[1].value = source;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_STOP_WAVE, &request, 0, 0);
}

void SsdStopEffectFileID(int file_id)
{
    RssdRequest request;

    request.arg[0].value = file_id;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_STOP_EFFECT_FILE_ID, &request, 0, 0);
}

int SsdCheckPlayEffectAll(void)
{
    RssdRequest request;

    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_CHECK_PLAY_EFFECT_ALL, &request, 0, 0);
    return 0;
}

int SsdCheckPlayEffect(int effect_id, int source_id)
{
    RssdRequest request;

    request.arg[0].value = effect_id;
    request.arg[1].value = source_id;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_CHECK_PLAY_EFFECT, &request, 0, 0);
    return 0;
}

int SsdCheckPlayEffectFileID(int file_id)
{
    RssdRequest request;

    request.arg[0].value = file_id;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_CHECK_PLAY_EFFECT_FILE_ID, &request, 0, 0);
    return 0;
}

void SsdInitSampling(int voiceCount, int workValue, int workSize)
{
    RssdRequest request;

    request.arg[0].value = voiceCount;
    request.arg[1].value = workValue;
    request.arg[2].value = workSize;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_INIT_SAMPLING, &request, 0, 0);
}

void SsdDisposeSampling(void)
{
    RssdRequest request;

    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_DISPOSE_SAMPLING, &request, 0, 0);
}

void SsdSetSampleDmaCallback(void *callback, void *arg)
{
    RssdWork.sample_dma_callback = callback;
    RssdWork.sample_dma_callback_arg = arg;
}

void SsdSetSampleKeyoffCallback(void *callback, void *arg)
{
    RssdWork.sample_keyoff_callback = callback;
    RssdWork.sample_keyoff_callback_arg = arg;
}

void SsdTransferSampling(int voice, void *data, int size)
{
    RssdRequest request;

    request.arg[0].value = voice;
    request.arg[1].pointer = data;
    request.arg[2].value = size;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_TRANSFER_SAMPLING, &request, data, size);
}

void SsdTransferSamplingNext(void *data, int size)
{
    RssdRequest request;

    request.arg[0].pointer = data;
    request.arg[1].value = size;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_TRANSFER_SAMPLING_NEXT, &request, data, size);
}

void SsdPlaySampling(int voice, int pitch, int volume, int pan)
{
    RssdRequest request;

    request.arg[0].value = voice;
    request.arg[1].value = pitch;
    request.arg[2].value = volume;
    request.arg[3].value = pan;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_PLAY_SAMPLING, &request, 0, 0);
}

void SsdStopSampling(int voice)
{
    RssdRequest request;

    request.arg[0].value = voice;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_STOP_SAMPLING, &request, 0, 0);
}

void SsdSetSamplingParam(int voice, int pitch, int volume, int pan)
{
    RssdRequest request;

    request.arg[0].value = voice;
    request.arg[1].value = pitch;
    request.arg[2].value = volume;
    request.arg[3].value = pan;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SAMPLING_PARAM, &request, 0, 0);
}

void SsdSetSamplingEffect(int voice, int effectFlag1, int effectFlag2)
{
    RssdRequest request;

    request.arg[0].value = voice;
    request.arg[1].value = effectFlag1;
    request.arg[2].value = effectFlag2;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_SET_SAMPLING_EFFECT, &request, 0, 0);
}

int RssdRequestCall(int command)
{
    return RssdCallFunc(command, 0, 0, 0);
}
