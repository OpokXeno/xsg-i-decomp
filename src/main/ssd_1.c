#include "common.h"
#include "main/ssd_init.h"
#include "main/nml_packet_add.h"

/* File magic read as a little-endian word: "swdm" in wave bank headers, "seds" in effect data headers. */
#define SSD_WAVE_TAG 0x6d647773
#define SSD_EFFECT_TAG 0x73646573

/* Commands without a table entry below. */
#define RSSD_CMD_SPU_DIRECT_READ 0x1b
#define RSSD_CMD_ADD_EFFECT_DATA 0x60

/*
 * RssdWork, RssdRequest and RssdCallFunc: the one spelling lives in main/tu110's
 * own header, src/main/ssd_init.h. The generated include/main/ssd_init.h cannot
 * carry RssdWork's type yet, so that
 * header is included directly.
 */
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

enum { RSSD_CMD_STOP_ALL_EFFECT = 0x78 };

typedef struct SsdWaveHeader {
    unsigned int tag;
    unsigned char unmodeled_04[4];
    int size;
    unsigned char unmodeled_0c[6];
    unsigned short id;
} SsdWaveHeader;

enum {
    RSSD_CMD_ADD_WAVE_DATA = 0x20
};

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

/*
 * Returns the SPU-DMA busy bit of RssdWork; a nonzero wait first spins until
 * it clears (main:0x002409b0). Same prototype as src/main/xgl_sound.c and
 * src/ov01/snd.h.
 */
extern int SsdSpuDmaCompleted(int wait);

extern int printf(const char *format, ...);

/*
 * Sends count 32-byte packets read from packets over SIF RPC; count << 5 is
 * the RssdCallFunc payload size, sizeof(RssdRequest). No-op when count is 0.
 */
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
    RssdWork.flags = (RssdWork.flags | RSSD_FLAG_SPU_DMA) & ~RSSD_FLAG_SUCCESS;
    RssdWork.wave_chunk_index = 0;
    request.arg[0].value = transSize;
    request.arg[1].value = size;
    if (RssdCallFunc(RSSD_CMD_SPU_DIRECT_READ, &request, 0, 0) < 0) {
        RssdWork.flags &= ~RSSD_FLAG_SPU_DMA;
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
    chunk = size > RSSD_TRANSFER_MAX ? RSSD_TRANSFER_MAX : size;
    request.arg[0] = (int)data;
    request.arg[2] = wave;
    RssdWork.spu_bytes_remaining = size - chunk;
    RssdWork.spu_write_ptr = (unsigned char *)data + chunk;
    RssdWork.flags |= RSSD_FLAG_SUCCESS | RSSD_FLAG_SPU_DMA;
    RssdWork.wave_id = id;
    request.arg[1] = chunk;
    RssdWork.wave_chunk_index = 0;
    if (RssdCallFunc(RSSD_CMD_ADD_WAVE_DATA, (RssdRequest *)&request, data, chunk) < 0) {
        RssdWork.flags &= ~RSSD_FLAG_SPU_DMA;
        printf("Raad add wave data !\n");
        return -1;
    }
    return id;
}

/*
 * Waits for the previous SPU DMA transfer to complete (SsdSpuDmaCompleted,
 * wait = 1), then sends size bytes of data as the next chunk of streaming
 * wave data for wave. Always returns 0.
 */
int SsdNextWaveData(void *data, int size, int wave)
{
    RssdRequest request;

    SsdSpuDmaCompleted(1);
    request.arg[0].pointer = data;
    request.arg[1].value = size;
    request.arg[2].value = wave;
    RssdWork.flags |= RSSD_FLAG_SUCCESS | RSSD_FLAG_SPU_DMA;
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
    int busy = (RssdWork.flags >> RSSD_FLAG_SPU_DMA_SHIFT) & 1;

    if (wait) {
        while (busy) {
            void *request;

            while ((request = sceSifGetNextRequest(RssdWork.rpc_queue)) != 0) {
                sceSifExecRequest(request);
            }
            busy = (RssdWork.flags >> RSSD_FLAG_SPU_DMA_SHIFT) & 1;
        }
    }
    return busy;
}

/* Forwards value (an SPU memory size or address) unchanged; no further evidenced use in this function. */
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

    if (effectData->tag != SSD_EFFECT_TAG) {
        printf("Rssd effect data error !\n", unused, effectData);
        return -1;
    }

    size = effectData->size;
    request.arg[0].pointer = effectData;
    RssdWork.flags |= RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_ADD_EFFECT_DATA, &request, effectData, size);
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
