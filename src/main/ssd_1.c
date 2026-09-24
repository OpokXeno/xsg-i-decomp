#include "common.h"

/*
 * RssdWork, RssdRequest and RssdCallFunc: the one spelling lives in main/tu110's
 * own header, src/main/ssd_init.h. The generated include/main/ssd_init.h cannot
 * carry RssdWork's type yet, so that
 * header is included directly.
 */
#include "ssd_init.h"

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

INCLUDE_ASM("asm/main/nonmatchings/ssd_1", SsdSpuDirectRead);

INCLUDE_ASM("asm/main/nonmatchings/ssd_1", SsdAddWaveData);

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

INCLUDE_ASM("asm/main/nonmatchings/ssd_1", SsdSpuDmaCompleted);

/* Forwards value (an SPU memory size or address) unchanged; no further evidenced use in this function. */
void SsdCheckSpuMemory(int value)
{
    RssdRequest request;

    request.arg[0].value = value;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_CHECK_SPU_MEMORY, &request, 0, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/ssd_1", SsdAddEffectData);

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

enum { RSSD_CMD_STOP_ALL_EFFECT = 0x78 };

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

/*
 * voiceCount, workValue and workSize forward to IOP SSD.IRX SsdInitSampling
 * (0x000050b0): voiceCount is bound-checked against 5 (`slti v0,s2,5` at
 * 0x50c4) before being forwarded to SsdAllocSamplingVoice/
 * SsdInitSamplingVoice (0x5150/0x5158); workSize is passed as the size
 * argument to AllocSysMemory (0x511c) and also stored into the sampling
 * work area at +0xce (0x5104); workValue is stored into that same work
 * area at +0xca (0x5100) with no further evidenced use in this function,
 * so it keeps a neutral name describing the store rather than an invented
 * role (docs/naming.md).
 */
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

/* Stores the caller-supplied callback and its argument verbatim; see RssdWorkFlags (main/ssd_init.h). */
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

/*
 * voice, data and size forward to IOP SSD.IRX SsdTransferSampling
 * (0x00005238): voice indexes the per-voice sampling table (the same
 * *3*16-byte-stride table SsdPlaySampling/SsdSetSamplingParam index);
 * data and size are forwarded unchanged as RssdCallFunc's own `data`/`size`
 * SIF RPC payload arguments and size is added to a running transfer offset
 * (0x529c) after the DMA write (SsdSpuDmaWrite, 0x5290).
 */
void SsdTransferSampling(int voice, void *data, int size)
{
    RssdRequest request;

    request.arg[0].value = voice;
    request.arg[1].pointer = data;
    request.arg[2].value = size;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_TRANSFER_SAMPLING, &request, data, size);
}

/*
 * data and size forward to IOP SSD.IRX SsdTransferSamplingNext
 * (0x000052b8), the same shape as SsdTransferSampling minus the voice
 * index (it continues the current voice's transfer): both are forwarded
 * unchanged as RssdCallFunc's `data`/`size` SIF RPC payload arguments and
 * size is added to the running transfer offset (0x52fc) after the DMA
 * write (SsdSpuDmaWrite, 0x52f4).
 */
void SsdTransferSamplingNext(void *data, int size)
{
    RssdRequest request;

    request.arg[0].pointer = data;
    request.arg[1].value = size;
    RssdWork.flags &= ~RSSD_FLAG_SUCCESS;
    RssdCallFunc(RSSD_CMD_TRANSFER_SAMPLING_NEXT, &request, data, size);
}

/*
 * voice, pitch, volume and pan forward to IOP SSD.IRX SsdPlaySampling
 * (0x0000531c): voice indexes the per-voice sampling table; pitch is
 * stored as a halfword at the voice's +0x14 (0x53a8); volume and pan are
 * stored at the voice's +0xc/+0x1c (0x5394/0x5398) and forwarded as-is to
 * SsdCalcDirectVoiceVolume (0x53a4) as its second and third argument.
 * Beyond "the voice's own volume-calculation inputs", pitch/volume/pan are
 * not confirmed by a further reader in this file (naming.md: do not invent
 * unsupported terminology beyond what the call site evidences).
 */
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

/*
 * Same (voice, pitch, volume, pan) shape as SsdPlaySampling: IOP SSD.IRX
 * SsdSetSamplingParam (0x00005468) writes the identical per-voice table
 * fields (+0xc/+0x1c word stores at 0x5490/0x5494, +0x14 halfword store at
 * 0x549c) and forwards the same pair to SsdCalcDirectVoiceVolume (0x54a4).
 */
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

/*
 * voice, effectFlag1 and effectFlag2 forward to IOP SSD.IRX
 * SsdSetSamplingEffect (0x000054cc): each flag is tested for zero/nonzero
 * (0x54d0/0x54e0) and independently contributes a bit to a byte stored at
 * the voice table entry's +0x2c and its nested pointer's +6
 * (effectFlag1 -> 0x8, effectFlag2 -> 0x4, both at 0x5508/0x5510), and a
 * word OR'd into that nested pointer's +4 (effectFlag2 takes priority over
 * effectFlag1 there: 0x4000 vs 0x8000, 0x551c). Their domain meaning
 * (which sampling effect each bit selects) is not evidenced further.
 */
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
