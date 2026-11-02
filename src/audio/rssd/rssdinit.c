#include "rssd/rssd.h"
#include "rssd/rssdfunc.h"
#include "rssd/rssdstrm.h"
#include "ssd/ssd_dbg.h"
#include "ssd/ssd_err.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_mem.h"
#include "ssd/ssd_musc.h"
#include "ssd/ssd_out.h"
#include "ssd/ssd_pcm.h"
#include "ssd/ssd_sec.h"
#include "ssd/ssd_seq.h"
#include "ssd/ssd_smpl.h"
#include "ssd/ssd_vag.h"
#include "ssd/ssd_voic.h"
#include "ssd/ssd_wave.h"

/* IRX module identity read by the loader: name and version 2.5. */
typedef struct ModuleInfo {
    const char *name;
    unsigned short version;
} ModuleInfo;

/* RssdSifServerThread / RssdSifClientThread creation parameters. */
#define RSSD_THREAD_ATTRIBUTES 0x02000000 /* TH_C */
#define RSSD_THREAD_STACK_SIZE 4096
#define RSSD_SERVER_THREAD_PRIORITY 10
#define RSSD_CLIENT_THREAD_PRIORITY 9

/* Polls of RssdGetVoiceEnvX that reuse one cached SSD interval measurement. */
#define RSSD_ENVELOPE_CACHE_FRAMES 20

/* Bytes of timing fields (measure, beat, ticks, time signature) sent by RssdGetSeqPlayInfo. */
#define RSSD_SEQ_PLAY_INFO_SIZE 16

ModuleInfo Module = { "PS2_SSDRemote", 0x0205 };

/* The SSD definitions in src/audio/ssd declare fewer or narrower parameters than the
 * original RSSD call sites use (an extra argument, or int / unsigned char / unsigned
 * short loads, or a signed char result), so the SSD headers cannot declare these without changing the calls. */
extern signed char SsdGetFuncErrorCode(void);
extern void SsdSetVagStreamPitch(int, int, int);
extern void SsdSetVagStreamPanpot(int, int, int);
extern void SsdSetSeqNoteFader(int, int, int);
extern void SsdSetSeqPanpotFader(int, int, int);
extern void SsdMidiDevSetMidiEvent(int, int);
extern void SsdMidiDevSetHWEffect(int, unsigned char);
extern void SsdDebAddEffect(void *, int, int, unsigned short);

RssdGlobalWork RssdWork;
void RssdExecFunc(RssdRpcPacket *packet);
void *RssdSifRpcServer(int command, void *buffer, int size);
int RssdSifServerThread(void *argument);
int RssdSifClientThread(void *argument);
void RssdSifRpcCallback(void *argument);
void RssdGetVoiceEnvX(void);
void RssdGetSeqPlayInfo(RssdRpcPacket *packet);

int start(void)
{
    IopThreadParameter thread;
    IopSemaphoreParameter semaphore;
    CpuDisableIntr();
    RssdWork.allocation = AllocSysMemory(SMEM_Low, RSSD_RPC_BUFFER_SIZE, 0);
    RssdWork.serverBuffer = AllocSysMemory(SMEM_Low, RSSD_RPC_BUFFER_SIZE, 0);
    CpuEnableIntr();
    RssdWork.clientBuffer = RssdWork.allocation;
    RssdWork.clientData = RssdWork.allocation + sizeof(RssdRpcPacket);
    semaphore.initialCount = 1;
    semaphore.maximumCount = 1;
    semaphore.attributes = 0;
    RssdWork.completionSemaphore = CreateSema(&semaphore);
    thread.attributes = RSSD_THREAD_ATTRIBUTES;
    thread.entry = RssdSifServerThread;
    thread.stackSize = RSSD_THREAD_STACK_SIZE;
    thread.priority = RSSD_SERVER_THREAD_PRIORITY;
    thread.option = 0;
    RssdWork.serverThread = CreateThread(&thread);
    StartThread(RssdWork.serverThread, 0);
    thread.attributes = RSSD_THREAD_ATTRIBUTES;
    thread.entry = RssdSifClientThread;
    thread.stackSize = RSSD_THREAD_STACK_SIZE;
    thread.priority = RSSD_CLIENT_THREAD_PRIORITY;
    thread.option = 0;
    RssdWork.clientThread = CreateThread(&thread);
    StartThread(RssdWork.clientThread, 0);
    return 0;
}

int RssdSifServerThread(void *argument)
{
    SifRpcQueueData queue;
    SifRpcServerData server;
    if (!sceSifCheckInit())
        sceSifInit();
    sceSifInitRpc(0);
    sceSifSetRpcQueue(&queue, GetThreadId());
    sceSifRegisterRpc(&server, RSSD_RPC_SERVER_ID, RssdSifRpcServer,
        RssdWork.serverBuffer, 0, 0, &queue);
    sceSifRpcLoop(&queue);
    return 0;
}

void *RssdSifRpcServer(int command, void *bufferData, int size)
{
    RssdRpcPacket *buffer = bufferData;
    if (command == RSSD_RPC_BATCH) {
        int remaining = buffer->arguments[1];
        RssdRpcPacket *record = buffer + 1;
        if (remaining) {
            do {
                RssdExecFunc(record);
                --remaining;
                ++record;
            } while (remaining);
        }
    } else {
        RssdExecFunc(buffer);
        ++RssdWork.serverCallCount;
    }
    return buffer;
}

void RssdExecFunc(RssdRpcPacket *packet)
{
    SsdResetFuncErrorCode();
    switch (packet->command) {
    case RSSD_CMD_INIT:
        RssdInit(packet);
        break;
    case RSSD_CMD_RESUME:
        SsdResume();
        break;
    case RSSD_CMD_SUSPEND:
        SsdSuspend();
        break;
    case RSSD_CMD_SET_OUTPUT_MODE:
        SsdSetOutputMode(packet->arguments[0]);
        break;
    case RSSD_CMD_SET_SPU_VOLUME:
        SsdSetSpuVolume(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_SET_CD_VOLUME:
        SsdSetCDVolume(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_SET_REVERB_PARAMETER:
        SsdSetReverbParameter(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        SsdReverbClearCompleted(packet->arguments[0] >> 8, 1);
        SsdEnableReverbParam(packet->arguments[0] >> 8);
        break;
    case RSSD_CMD_SPU_DIRECT_READ:
        RssdSpuDirectRead(packet);
        break;
    case RSSD_CMD_ADD_WAVE_DATA:
        RssdAddWaveData(packet);
        break;
    case RSSD_CMD_NEXT_WAVE_DATA:
        RssdNextWaveData(packet);
        break;
    case RSSD_CMD_DISPOSE_WAVE_BANK:
        SsdDisposeWaveBank(packet->arguments[0]);
        break;
    case RSSD_CMD_CHECK_WAVE_DATA:
        packet->value = SsdCheckWaveData(packet->arguments[0]);
        break;
    case RSSD_CMD_SET_SEGMENT_ALLOC_MODE:
        SsdSetSegmentAllocMode(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_RESET_SEGMENT_ALLOC_MODE:
        SsdResetSegmentAllocMode(packet->arguments[0]);
        break;
    case RSSD_CMD_PLAY_SEQUENCE_DATA:
        RssdPlaySequenceData(packet);
        break;
    case RSSD_CMD_ADD_SEQUENCE_DATA:
        RssdAddSequenceData(packet);
        break;
    case RSSD_CMD_ADD_SEQ_EFFECT_DATA:
        RssdAddSeqEffectData(packet);
        break;
    case RSSD_CMD_DISPOSE_SEQUENCE:
        SsdDisposeSequence(packet->arguments[0]);
        break;
    case RSSD_CMD_CHECK_SEQUENCE:
        packet->value = SsdCheckSequence(packet->arguments[0]);
        break;
    case RSSD_CMD_START_SEQUENCE:
        SsdStartSequence(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_RESUME_SEQUENCE:
        SsdResumeSequence(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_STOP_SEQUENCE:
        SsdStopSequence(packet->arguments[0]);
        break;
    case RSSD_CMD_PAUSE_SEQUENCE:
        SsdPauseSequence(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_GET_SEQ_PLAY_STATUS:
        packet->value = SsdGetSeqPlayStatus(packet->arguments[0]);
        break;
    case RSSD_CMD_SET_SEQ_MASTER_TEMPO:
        SsdSetSeqMasterTempo(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_SET_SEQ_MASTER_VOLUME:
        SsdSetSeqMasterVolume(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_SET_SEQ_MASTER_NOTE:
        SsdSetSeqMasterNote(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_SET_SEQ_MASTER_PANPOT:
        SsdSetSeqMasterPanpot(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_SET_SEQ_MUTE:
        SsdSetSeqMute(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_SET_SEQ_NOTE_FADER:
        SsdSetSeqNoteFader(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_SET_SEQ_VOLUME_FADER:
        SsdSetSeqVolumeFader(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_SET_SEQ_PANPOT_FADER:
        SsdSetSeqPanpotFader(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_SET_SEQUENCE_OUTPUT_VOLUME:
        SsdSetSequenceOutputVolume(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_SET_EFFECT_OUTPUT_VOLUME:
        SsdSetEffectOutputVolume(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_SET_SEQ_PATCH_NUMBER:
        SsdSetSeqPatchNumber(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_SET_TRACK_PATCH_SET:
        SsdSetTrackPatchSet(packet->arguments[0], (SsdTrackPatchSet *)packet->data);
        break;
    case RSSD_CMD_SET_SEQ_SIGNAL:
        SsdSetSeqSignal(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_ADD_EFFECT_DATA:
        RssdAddEffectData(packet);
        break;
    case RSSD_CMD_DISPOSE_EFFECT_DATA:
        SsdDisposeEffectData(packet->arguments[0]);
        break;
    case RSSD_CMD_CHECK_EFFECT_DATA:
        packet->value = SsdCheckEffectData(packet->arguments[0]);
        break;
    case RSSD_CMD_PLAY_EFFECT_NORMAL:
        SsdPlayEffectNormal(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_PLAY_EFFECT_PARAM:
        SsdPlayEffectParam(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case RSSD_CMD_SET_EFFECT_PARAM:
        SsdSetEffectParam(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case RSSD_CMD_SET_PLAY_EFFECT_PARAM:
        SsdSetPlayEffectParam(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case RSSD_CMD_STOP_ALL_EFFECT:
        SsdStopAllEffect();
        break;
    case RSSD_CMD_FADEOUT_EFFECT:
        SsdFadeoutEffect(packet->arguments[0], 0, packet->arguments[1]);
        break;
    case RSSD_CMD_FADEOUT_EFFECT_FILE_ID:
        SsdFadeoutEffectFileID(packet->arguments[0], 0);
        break;
    case RSSD_CMD_CONTINUE_ONE_EFFECT:
        SsdContinueOneEffect(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_FADEOUT_EFFECT_FROM:
        SsdFadeoutEffect(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_FADEOUT_EFFECT_FILE_ID_FROM:
        SsdFadeoutEffectFileID(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_CHECK_PLAY_EFFECT_ALL:
        packet->value = SsdCheckPlayEffectAll();
        break;
    case RSSD_CMD_CHECK_PLAY_EFFECT:
        packet->value = SsdCheckPlayEffect(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_CHECK_PLAY_EFFECT_FILE_ID:
        packet->value = SsdCheckPlayEffectFileID(packet->arguments[0]);
        break;
    case RSSD_CMD_PLAY_WAVE_NORMAL:
        SsdPlayWaveNormal(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_PLAY_WAVE_PARAM:
        SsdPlayWaveParam(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case RSSD_CMD_STOP_WAVE:
        SsdStopWave(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_INIT_PCM_STREAM:
        SsdInitEEPcmStream(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_DISPOSE_PCM_STREAM:
        SsdDisposeEEPcmStream();
        break;
    case RSSD_CMD_PLAY_PCM_STREAM:
        SsdPlayPcmStream(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_STOP_PCM_STREAM:
        SsdStopPcmStream(packet->arguments[0]);
        break;
    case RSSD_CMD_SET_PCM_STREAM_DATA:
        SsdSetEEPcmStreamData(packet);
        break;
    case RSSD_CMD_GET_PCM_STREAM_STATUS:
        packet->value = SsdGetEEPcmStreamStatus();
        break;
    case RSSD_CMD_SET_PCM_STREAM_VOLUME:
        SsdSetPcmStreamVolume(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_CLEAR_RING_BUFFER:
        SsdEEClearRingBuffer(packet->arguments[0]);
        break;
    case RSSD_CMD_INIT_VAG_STREAM_STEREO:
        SsdInitEEVagStreamStereo(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_INIT_VAG_STREAM_MONO:
        SsdInitEEVagStreamMono(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_DISPOSE_VAG_STREAM:
        SsdDisposeEEVagStream();
        break;
    case RSSD_CMD_PLAY_VAG_STREAM:
        SsdPlayEEVagStream(packet);
        break;
    case RSSD_CMD_STOP_VAG_STREAM:
        SsdStopEEVagStream(packet);
        break;
    case RSSD_CMD_SET_VAG_STREAM_DATA_STEREO:
        SsdSetEEVagStreamDataStereo(packet);
        break;
    case RSSD_CMD_SET_VAG_STREAM_DATA_MONO:
        SsdSetEEVagStreamDataMono(packet);
        break;
    case RSSD_CMD_GET_VAG_STREAM_STATUS_STEREO:
        packet->value = SsdGetEEVagStreamStatusStereo(packet);
        break;
    case RSSD_CMD_GET_VAG_STREAM_STATUS_MONO:
        packet->value = SsdGetEEVagStreamStatusMono(packet);
        break;
    case RSSD_CMD_GET_VAG_STREAM_STATUS_ALL:
        packet->value = SsdGetEEVagStreamStatusAll(packet);
        break;
    case RSSD_CMD_SET_VAG_STREAM_PITCH:
        SsdSetVagStreamPitch(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_SET_VAG_STREAM_VOLUME:
        SsdSetVagStreamVolume(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_SET_VAG_STREAM_PANPOT:
        SsdSetVagStreamPanpot(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_SET_VAG_STREAM_PARAM:
        SsdSetVagStreamParam(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case RSSD_CMD_INIT_SAMPLING:
        RssdInitSampling(packet);
        break;
    case RSSD_CMD_DISPOSE_SAMPLING:
        SsdDisposeSampling();
        break;
    case RSSD_CMD_TRANSFER_SAMPLING:
        RssdTransferSampling(packet);
        break;
    case RSSD_CMD_TRANSFER_SAMPLING_NEXT:
        RssdTransferSamplingNext(packet);
        break;
    case RSSD_CMD_PLAY_SAMPLING:
        SsdPlaySampling(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case RSSD_CMD_STOP_SAMPLING:
        SsdStopSampling(packet->arguments[0]);
        break;
    case RSSD_CMD_SET_SAMPLING_PARAM:
        SsdSetSamplingParam(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case RSSD_CMD_SET_SAMPLING_EFFECT:
        SsdSetSamplingEffect(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_CHECK_SPU_MEMORY:
        packet->value = SsdCheckSpuMemory(packet->arguments[0]);
        break;
    case RSSD_CMD_GET_VOICE_ENV:
        RssdGetVoiceEnvX();
        break;
    case RSSD_CMD_GET_SEQ_PLAY_INFO:
        RssdGetSeqPlayInfo(packet);
        break;
    case RSSD_CMD_DEB_OPEN_DEVICE:
        SsdDebOpenDevice();
        break;
    case RSSD_CMD_DEB_CLOSE_DEVICE:
        SsdDebCloseDevice();
        break;
    case RSSD_CMD_DEB_WAVE_KEYON:
        SsdDebWaveKeyon((struct SsdWavePatch *)packet->data, packet->arguments[1], packet->arguments[2]);
        break;
    case RSSD_CMD_DEB_WAVE_KEYOFF:
        SsdDebWaveKeyoff(packet->arguments[0]);
        break;
    case RSSD_CMD_MIDI_DEV_SET_MIDI_EVENT:
        SsdMidiDevSetMidiEvent(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_MIDI_DEV_SET_HW_EFFECT:
        SsdMidiDevSetHWEffect(packet->arguments[0], packet->arguments[1]);
        break;
    case RSSD_CMD_DEB_ADD_EFFECT:
        SsdDebAddEffect(packet->data, packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case RSSD_CMD_DEB_PLAY_EFFECT:
        SsdDebPlayEffect();
        break;
    case RSSD_CMD_COMPELLING_KEYOFF:
        SsdCompellingKeyoff();
        break;
    default:
        printf("Rssd illegal function No.0x%04x\n", packet->command);
        packet->error = -1;
        break;
    }
    RssdWork.lastPacket = *packet;
    packet->error = SsdGetFuncErrorCode();
}

void RssdGetVoiceEnvX(void)
{
    int envelopes[SSD_VOICE_COUNT];
    RssdRpcPacket request;
    SsdGlobalWork *work = SsdGetGlobalWorkPtr();
    EnvelopeCache *cache = &RssdWork.envelopeCache;
    int voice;
    if (!cache->frames) {
        cache->processingTicks = work->processing_ticks;
        cache->interrupts = work->measured_interrupts;
        work->measured_interrupts = 0;
        work->processing_ticks = 0;
        cache->frames = RSSD_ENVELOPE_CACHE_FRAMES;
    }
    --cache->frames;
    request.arguments[0] = cache->processingTicks;
    request.arguments[1] = cache->interrupts;
    request.arguments[2] = work->timer_interval;
    for (voice = 0; voice < SSD_VOICE_COUNT; ++voice)
        envelopes[voice] = work->voices[voice].allocation.priority; /* whole word: ENVX in the low byte */
    RssdCallFunc(RSSD_CMD_GET_VOICE_ENV, &request, envelopes, sizeof(envelopes));
}

void RssdGetSeqPlayInfo(RssdRpcPacket *packet)
{
    SsdSequence *sequence = SsdGetSequencePtr(packet->arguments[0]);
    packet->arguments[1] = sequence->play_ticks;
    packet->arguments[2] = sequence->play_time >> 8;
    packet->arguments[3] = (sequence->tempo.value & 0xffff0000) + sequence->master_tempo.current.parts.integer;
    /* The 16 bytes from unmodeled_58 through time_numerator go out as one block. */
    RssdCallFunc(RSSD_CMD_GET_SEQ_PLAY_INFO, packet, &sequence->unmodeled_58, RSSD_SEQ_PLAY_INFO_SIZE);
}

int RssdSifClientThread(void *argument)
{
    for (;;) {
        RssdRequestNode *request;
        RssdRpcPacket *packet;
        void *context;
        int receiveSize;
        int sendSize;
        void (*callback)(void *);
        SleepThread();
        request = RssdWork.firstRequest;
        if (request) {
            WaitSema(RssdWork.completionSemaphore);
            CpuDisableIntr();
            RssdWork.firstRequest = request->next;
            CpuEnableIntr();
            packet = &request->packet;
            RssdWork.completionCallback = request->callback;
            RssdWork.completionArgument = request->callbackArgument;
            RssdWork.flags |= RSSD_FLAG_RPC_BUSY;
            context = &RssdWork;
            receiveSize = sizeof(RssdRpcPacket);
            callback = RssdSifRpcCallback;
            sendSize = packet->dataSize + sizeof(RssdRpcPacket);
            if (sceSifCallRpc(&RssdWork.client, packet->command, 0, packet,
                sendSize, packet, receiveSize, callback, context) < 0) {
                RssdWork.flags &= ~RSSD_FLAG_RPC_BUSY;
                SignalSema(RssdWork.completionSemaphore);
            }
            CpuDisableIntr();
            FreeSysMemory(request);
            CpuEnableIntr();
        }
    }
}

void RssdSifRpcCallback(void *argument)
{
    RssdWork.flags &= ~RSSD_FLAG_RPC_BUSY;
    iSignalSema(RssdWork.completionSemaphore);
    if (RssdWork.completionCallback)
        RssdWork.completionCallback(RssdWork.completionArgument);
}

void RssdCallFunc(int command, const void *requestData, const void *data, int size)
{
    const RssdRpcPacket *request = requestData;
    RssdRequestNode *node;
    RssdRequestNode *tail;
    RssdRpcPacket *packet;
    int allocationSize;
    if (!data)
        size = 0;
    allocationSize = size + sizeof(RssdRequestNode);
    CpuDisableIntr();
    node = AllocSysMemory(SMEM_Low, allocationSize, 0);
    CpuEnableIntr();
    packet = &node->packet;
    if (request)
        *packet = *request;
    node->packet.command = command;
    packet->dataSize = size;
    if (size)
        SsdMemoryCopy(node->data, data, size);
    node->next = 0;
    node->callback = RssdWork.nextCallback;
    node->callbackArgument = RssdWork.nextCallbackArgument;
    CpuDisableIntr();
    tail = RssdWork.firstRequest;
    if (tail) {
        while (tail->next)
            tail = tail->next;
        tail->next = node;
    } else {
        RssdWork.firstRequest = node;
    }
    CpuEnableIntr();
    RssdWork.nextCallback = 0;
    RssdWork.nextCallbackArgument = 0;
    WakeupThread(RssdWork.clientThread);
}

void iRssdCallFunc(int command, const void *requestData, const void *data, int size)
{
    const RssdRpcPacket *request = requestData;
    RssdRequestNode *node;
    RssdRequestNode *tail;
    RssdRpcPacket *packet;
    if (!data)
        size = 0;
    node = AllocSysMemory(SMEM_Low, size + sizeof(RssdRequestNode), 0);
    packet = &node->packet;
    if (request)
        *packet = *request;
    node->packet.command = command;
    packet->dataSize = size;
    if (size)
        SsdMemoryCopy(node->data, data, size);
    node->next = 0;
    node->callback = RssdWork.nextCallback;
    node->callbackArgument = RssdWork.nextCallbackArgument;
    tail = RssdWork.firstRequest;
    if (tail) {
        while (tail->next)
            tail = tail->next;
        tail->next = node;
    } else {
        RssdWork.firstRequest = node;
    }
    RssdWork.nextCallback = 0;
    RssdWork.nextCallbackArgument = 0;
    iWakeupThread(RssdWork.clientThread);
}

void RssdSetFuncCallback(RssdFunctionCallback callback, void *argument)
{
    RssdWork.nextCallback = callback;
    RssdWork.nextCallbackArgument = argument;
}
