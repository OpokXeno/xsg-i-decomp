/* Original RSSD rssdinit.c translation unit. Uninterpreted storage remains
 * explicitly unmodeled; RPC records retain their original 32-byte stride. */
typedef void (*RssdFunctionCallback)(void *);
typedef struct RssdRpcPacket {
    short command;
    short error;
    int parameter;
    int value;
    int dataSize;
    int arguments[4];
    unsigned char data[0];
} RssdRpcPacket;

typedef struct RssdRequestNode {
    struct RssdRequestNode *next;
    int unmodeled_004;
    RssdFunctionCallback callback;
    void *callbackArgument;
    RssdRpcPacket packet;
    unsigned char data[0];
} RssdRequestNode;

typedef struct SifRpcClientData { unsigned int opaque[9]; } SifRpcClientData;
typedef struct EnvelopeCache {
    int frames;
    int count;
    int total;
} EnvelopeCache;

typedef struct RssdGlobalWork {
    unsigned int flags;
    unsigned char *allocation;
    unsigned char *clientBuffer;
    unsigned char *clientData;
    unsigned char unmodeled_010[0x38];
    void *serverBuffer;
    unsigned char unmodeled_04c[0x14];
    RssdRpcPacket lastPacket;
    SifRpcClientData client;
    unsigned char unmodeled_0a4[4];
    int serverThread;
    int clientThread;
    int completionSemaphore;
    int serverCallCount;
    RssdRequestNode *firstRequest;
    EnvelopeCache envelopeCache;
    unsigned char unmodeled_0c8[0x10];
    RssdFunctionCallback nextCallback;
    void *nextCallbackArgument;
    RssdFunctionCallback completionCallback;
    void *completionArgument;
    unsigned char unmodeled_0e8[0x18];
} RssdGlobalWork;


typedef struct SsdSequence {
    unsigned char unmodeled_000[48];
    unsigned int flags;
    unsigned char unmodeled_034[24];
    int position;
    unsigned int time;
    unsigned char unmodeled_054[4];
    int playInfo[4];
    unsigned char unmodeled_068[22];
    short playValue;
} SsdSequence;

typedef struct IopThreadParameter {
    unsigned int attributes;
    unsigned int option;
    int (*entry)(void *argument);
    int stackSize;
    int priority;
    unsigned int unmodeled_014;
} IopThreadParameter;

typedef struct IopSemaphoreParameter {
    unsigned int attributes;
    unsigned int option;
    int initialCount;
    int maximumCount;
} IopSemaphoreParameter;

typedef struct SifRpcQueueData { unsigned int opaque[6]; } SifRpcQueueData;

typedef struct SifRpcServerData { unsigned int opaque[17]; } SifRpcServerData;

typedef void *(*SifRpcFunction)(int function, void *buffer, int size);

typedef struct SsdVoiceEnvelopeRecord {
    int envelope;
    unsigned char unmodeled_004[0xfc];
} SsdVoiceEnvelopeRecord;

typedef struct SsdGlobalWork {
    unsigned char unmodeled_000[0x154];
    int status;
    unsigned char unmodeled_158[0x12c];
    SsdVoiceEnvelopeRecord voices[48];
    unsigned char unmodeled_3284[0x14];
    int intervalCount;
    int intervalTotal;
} SsdGlobalWork;

/* IRX module identity read by the loader: name and version 2.5. */
typedef struct ModuleInfo {
    const char *name;
    unsigned short version;
} ModuleInfo;

ModuleInfo Module = { "PS2_SSDRemote", 0x0205 };

extern int printf(const char *format, ...);

extern void SsdResetFuncErrorCode(void);

extern signed char SsdGetFuncErrorCode(void);

extern void RssdInit(RssdRpcPacket *);

extern void RssdSpuDirectRead(RssdRpcPacket *);

extern void RssdAddWaveData(RssdRpcPacket *);

extern void RssdNextWaveData(RssdRpcPacket *);

extern void RssdPlaySequenceData(RssdRpcPacket *);

extern void RssdAddSequenceData(RssdRpcPacket *);

extern void RssdAddSeqEffectData(RssdRpcPacket *);

extern void RssdAddEffectData(RssdRpcPacket *);

extern void RssdInitSampling(RssdRpcPacket *);

extern void RssdTransferSampling(RssdRpcPacket *);

extern void RssdTransferSamplingNext(RssdRpcPacket *);

extern void SsdResume(void);

extern void SsdSuspend(void);

extern void SsdSetOutputMode(int);

extern void SsdSetSpuVolume(int, int);

extern void SsdSetCDVolume(int, int);

extern void SsdSetReverbParameter(int, int, int, int);

extern void SsdReverbClearCompleted(int, int);

extern void SsdEnableReverbParam(int);

extern void SsdDisposeWaveBank(unsigned short);

extern int SsdCheckWaveData(int);

extern void SsdSetSegmentAllocMode(int, int);

extern void SsdResetSegmentAllocMode(int);

extern void SsdDisposeSequence(int);

extern int SsdCheckSequence(int);

extern void SsdStartSequence(int, int, int);

extern void SsdResumeSequence(int, int, int);

extern void SsdStopSequence(int);

extern void SsdPauseSequence(int, int);

extern int SsdGetSeqPlayStatus(int);

extern void SsdSetSeqMasterTempo(int, int, int);

extern void SsdSetSeqMasterVolume(int, int, int);

extern void SsdSetSeqMasterNote(int, int, int);

extern void SsdSetSeqMasterPanpot(int, int, int);

extern void SsdSetSeqMute(int, int);

extern void SsdSetSeqNoteFader(int, int, int);

extern void SsdSetSeqVolumeFader(int, int, int);

extern void SsdSetSeqPanpotFader(int, int, int);

extern void SsdSetSequenceOutputVolume(int, int);

extern void SsdSetEffectOutputVolume(int, int);

extern void SsdSetSeqPatchNumber(int, int);

extern void SsdSetTrackPatchSet(int, void *);

extern void SsdSetSeqSignal(int, int);

extern void SsdDisposeEffectData(int);

extern int SsdCheckEffectData(int);

extern void SsdPlayEffectNormal(int, int);

extern void SsdPlayEffectParam(int, int, int, int);

extern void SsdSetEffectParam(int, int, int, int);

extern void SsdSetPlayEffectParam(int, int, int, int);

extern void SsdStopAllEffect(void);

extern void SsdFadeoutEffect(int, int, int);

extern void SsdFadeoutEffectFileID(int, int);

extern void SsdContinueOneEffect(int, int, int);

extern int SsdCheckPlayEffectAll(void);

extern int SsdCheckPlayEffect(int, int);

extern int SsdCheckPlayEffectFileID(int);

extern void SsdPlayWaveNormal(int, int);

extern void SsdPlayWaveParam(int, int, int, int);

extern void SsdStopWave(int, int);

extern void SsdInitEEPcmStream(int, int);

extern void SsdDisposeEEPcmStream(void);

extern void SsdPlayPcmStream(int, int);

extern void SsdStopPcmStream(int);

extern void SsdSetEEPcmStreamData(RssdRpcPacket *);

extern int SsdGetEEPcmStreamStatus(void);

extern void SsdSetPcmStreamVolume(int, int);

extern void SsdEEClearRingBuffer(int);

extern void SsdInitEEVagStreamStereo(int, int);

extern void SsdInitEEVagStreamMono(int, int, int);

extern void SsdDisposeEEVagStream(void);

extern void SsdPlayEEVagStream(RssdRpcPacket *);

extern void SsdStopEEVagStream(RssdRpcPacket *);

extern void SsdSetEEVagStreamDataStereo(RssdRpcPacket *);

extern void SsdSetEEVagStreamDataMono(RssdRpcPacket *);

extern int SsdGetEEVagStreamStatusStereo(RssdRpcPacket *);

extern int SsdGetEEVagStreamStatusMono(RssdRpcPacket *);

extern int SsdGetEEVagStreamStatusAll(RssdRpcPacket *);

extern void SsdSetVagStreamPitch(int, int, int);

extern void SsdSetVagStreamVolume(int, int, int);

extern void SsdSetVagStreamPanpot(int, int, int);

extern void SsdSetVagStreamParam(int, int, int, int);

extern void SsdDisposeSampling(void);

extern void SsdPlaySampling(int, int, int, int);

extern void SsdStopSampling(int);

extern void SsdSetSamplingParam(int, int, int, int);

extern void SsdSetSamplingEffect(int, int, int);

extern int SsdCheckSpuMemory(int);

extern void SsdDebOpenDevice(void);

extern void SsdDebCloseDevice(void);

extern void SsdDebWaveKeyon(void *, int, int);

extern void SsdDebWaveKeyoff(int);

extern void SsdMidiDevSetMidiEvent(int, int);

extern void SsdMidiDevSetHWEffect(int, unsigned char);

extern void SsdDebAddEffect(void *, int, int, unsigned short);

extern void SsdDebPlayEffect(void);

extern void SsdCompellingKeyoff(void);

extern void *AllocSysMemory(int mode, int size, void *address);

extern void SsdMemoryCopy(void *destination, const void *source, int size);

extern int iWakeupThread(int thread);

extern void CpuDisableIntr(void);

extern void CpuEnableIntr(void);

extern int WakeupThread(int thread);

extern int iSignalSema(int semaphore);

extern SsdSequence *SsdGetSequencePtr(int sequenceId);

extern int SleepThread(void);

extern int WaitSema(int semaphore);

extern int SignalSema(int semaphore);

extern int FreeSysMemory(void *memory);

extern int sceSifCallRpc(SifRpcClientData *client, int command, int mode,
    void *send, int sendSize, void *receive, int receiveSize,
    void (*callback)(void *argument), void *argument);

extern int CreateSema(IopSemaphoreParameter *parameter);

extern int CreateThread(IopThreadParameter *parameter);

extern int StartThread(int thread, void *argument);

extern int sceSifCheckInit(void);

extern void sceSifInit(void);

extern void sceSifInitRpc(int mode);

extern int GetThreadId(void);

extern void sceSifSetRpcQueue(SifRpcQueueData *queue, int thread);

extern void sceSifRegisterRpc(SifRpcServerData *server, int id,
    SifRpcFunction function, void *buffer, SifRpcFunction callback,
    void *callbackBuffer, SifRpcQueueData *queue);

extern void sceSifRpcLoop(SifRpcQueueData *queue);

extern SsdGlobalWork *SsdGetGlobalWorkPtr(void);

RssdGlobalWork RssdWork;
void RssdExecFunc(RssdRpcPacket *packet);
void *RssdSifRpcServer(int command, void *buffer, int size);
int RssdSifServerThread(void *argument);
int RssdSifClientThread(void *argument);
void RssdSifRpcCallback(void *argument);
void RssdGetVoiceEnvX(void);
void RssdGetSeqPlayInfo(RssdRpcPacket *packet);
void RssdCallFunc(int command, const void *requestData, const void *data, int size);


int start(void)
{
    IopThreadParameter thread;
    IopSemaphoreParameter semaphore;
    CpuDisableIntr();
    RssdWork.allocation = AllocSysMemory(0, 0x10020, 0);
    RssdWork.serverBuffer = AllocSysMemory(0, 0x10020, 0);
    CpuEnableIntr();
    RssdWork.clientBuffer = RssdWork.allocation;
    RssdWork.clientData = RssdWork.allocation + 32;
    semaphore.initialCount = 1;
    semaphore.maximumCount = 1;
    semaphore.attributes = 0;
    RssdWork.completionSemaphore = CreateSema(&semaphore);
    thread.attributes = 0x02000000;
    thread.entry = RssdSifServerThread;
    thread.stackSize = 4096;
    thread.priority = 10;
    thread.option = 0;
    RssdWork.serverThread = CreateThread(&thread);
    StartThread(RssdWork.serverThread, 0);
    thread.attributes = 0x02000000;
    thread.entry = RssdSifClientThread;
    thread.stackSize = 4096;
    thread.priority = 9;
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
    sceSifRegisterRpc(&server, 0x900, RssdSifRpcServer,
        RssdWork.serverBuffer, 0, 0, &queue);
    sceSifRpcLoop(&queue);
    return 0;
}

void *RssdSifRpcServer(int command, void *bufferData, int size)
{
    RssdRpcPacket *buffer = bufferData;
    if (command == 4) {
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
    case 0x1:
        RssdInit(packet);
        break;
    case 0x5:
        SsdResume();
        break;
    case 0x6:
        SsdSuspend();
        break;
    case 0x10:
        SsdSetOutputMode(packet->arguments[0]);
        break;
    case 0x12:
        SsdSetSpuVolume(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x13:
        SsdSetCDVolume(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x14:
        SsdSetReverbParameter(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        SsdReverbClearCompleted(packet->arguments[0] >> 8, 1);
        SsdEnableReverbParam(packet->arguments[0] >> 8);
        break;
    case 0x1b:
        RssdSpuDirectRead(packet);
        break;
    case 0x20:
        RssdAddWaveData(packet);
        break;
    case 0x21:
        RssdNextWaveData(packet);
        break;
    case 0x22:
        SsdDisposeWaveBank((unsigned short)packet->arguments[0]);
        break;
    case 0x24:
        packet->value = SsdCheckWaveData(packet->arguments[0]);
        break;
    case 0x2c:
        SsdSetSegmentAllocMode(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x2d:
        SsdResetSegmentAllocMode(packet->arguments[0]);
        break;
    case 0x30:
        RssdPlaySequenceData(packet);
        break;
    case 0x31:
        RssdAddSequenceData(packet);
        break;
    case 0x38:
        RssdAddSeqEffectData(packet);
        break;
    case 0x32:
        SsdDisposeSequence(packet->arguments[0]);
        break;
    case 0x33:
        packet->value = SsdCheckSequence(packet->arguments[0]);
        break;
    case 0x34:
        SsdStartSequence(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0x35:
        SsdResumeSequence(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0x36:
        SsdStopSequence(packet->arguments[0]);
        break;
    case 0x37:
        SsdPauseSequence(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x40:
        packet->value = SsdGetSeqPlayStatus(packet->arguments[0]);
        break;
    case 0x41:
        SsdSetSeqMasterTempo(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0x42:
        SsdSetSeqMasterVolume(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0x43:
        SsdSetSeqMasterNote(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0x44:
        SsdSetSeqMasterPanpot(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0x45:
        SsdSetSeqMute(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x48:
        SsdSetSeqNoteFader(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0x49:
        SsdSetSeqVolumeFader(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0x4a:
        SsdSetSeqPanpotFader(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0x4b:
        SsdSetSequenceOutputVolume(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x4c:
        SsdSetEffectOutputVolume(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x50:
        SsdSetSeqPatchNumber(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x51:
        SsdSetTrackPatchSet(packet->arguments[0], packet->data);
        break;
    case 0x58:
        SsdSetSeqSignal(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x60:
        RssdAddEffectData(packet);
        break;
    case 0x61:
        SsdDisposeEffectData(packet->arguments[0]);
        break;
    case 0x62:
        packet->value = SsdCheckEffectData(packet->arguments[0]);
        break;
    case 0x70:
        SsdPlayEffectNormal(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x71:
        SsdPlayEffectParam(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case 0x7b:
        SsdSetEffectParam(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case 0x7c:
        SsdSetPlayEffectParam(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case 0x78:
        SsdStopAllEffect();
        break;
    case 0x79:
        SsdFadeoutEffect(packet->arguments[0], 0, packet->arguments[1]);
        break;
    case 0x7a:
        SsdFadeoutEffectFileID(packet->arguments[0], 0);
        break;
    case 0x7d:
        SsdContinueOneEffect(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0x7e:
        SsdFadeoutEffect(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0x7f:
        SsdFadeoutEffectFileID(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x80:
        packet->value = SsdCheckPlayEffectAll();
        break;
    case 0x81:
        packet->value = SsdCheckPlayEffect(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x82:
        packet->value = SsdCheckPlayEffectFileID(packet->arguments[0]);
        break;
    case 0x90:
        SsdPlayWaveNormal(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x91:
        SsdPlayWaveParam(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case 0x92:
        SsdStopWave(packet->arguments[0], packet->arguments[1]);
        break;
    case 0xa0:
        SsdInitEEPcmStream(packet->arguments[0], packet->arguments[1]);
        break;
    case 0xa1:
        SsdDisposeEEPcmStream();
        break;
    case 0xa2:
        SsdPlayPcmStream(packet->arguments[0], packet->arguments[1]);
        break;
    case 0xa3:
        SsdStopPcmStream(packet->arguments[0]);
        break;
    case 0xa4:
        SsdSetEEPcmStreamData(packet);
        break;
    case 0xa5:
        packet->value = SsdGetEEPcmStreamStatus();
        break;
    case 0xa6:
        SsdSetPcmStreamVolume(packet->arguments[0], packet->arguments[1]);
        break;
    case 0xa8:
        SsdEEClearRingBuffer(packet->arguments[0]);
        break;
    case 0xb0:
        SsdInitEEVagStreamStereo(packet->arguments[0], packet->arguments[1]);
        break;
    case 0xb1:
        SsdInitEEVagStreamMono(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0xb2:
        SsdDisposeEEVagStream();
        break;
    case 0xb3:
        SsdPlayEEVagStream(packet);
        break;
    case 0xb4:
        SsdStopEEVagStream(packet);
        break;
    case 0xb5:
        SsdSetEEVagStreamDataStereo(packet);
        break;
    case 0xb6:
        SsdSetEEVagStreamDataMono(packet);
        break;
    case 0xb7:
        packet->value = SsdGetEEVagStreamStatusStereo(packet);
        break;
    case 0xb8:
        packet->value = SsdGetEEVagStreamStatusMono(packet);
        break;
    case 0xbe:
        packet->value = SsdGetEEVagStreamStatusAll(packet);
        break;
    case 0xba:
        SsdSetVagStreamPitch(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0xbb:
        SsdSetVagStreamVolume(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0xbc:
        SsdSetVagStreamPanpot(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0xbd:
        SsdSetVagStreamParam(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case 0xd0:
        RssdInitSampling(packet);
        break;
    case 0xd1:
        SsdDisposeSampling();
        break;
    case 0xd2:
        RssdTransferSampling(packet);
        break;
    case 0xd3:
        RssdTransferSamplingNext(packet);
        break;
    case 0xd4:
        SsdPlaySampling(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case 0xd5:
        SsdStopSampling(packet->arguments[0]);
        break;
    case 0xd6:
        SsdSetSamplingParam(packet->arguments[0], packet->arguments[1], packet->arguments[2], packet->arguments[3]);
        break;
    case 0xd7:
        SsdSetSamplingEffect(packet->arguments[0], packet->arguments[1], packet->arguments[2]);
        break;
    case 0x100:
        packet->value = SsdCheckSpuMemory(packet->arguments[0]);
        break;
    case 0x102:
        RssdGetVoiceEnvX();
        break;
    case 0x103:
        RssdGetSeqPlayInfo(packet);
        break;
    case 0x200:
        SsdDebOpenDevice();
        break;
    case 0x201:
        SsdDebCloseDevice();
        break;
    case 0x202:
        SsdDebWaveKeyon(packet->data, packet->arguments[1], packet->arguments[2]);
        break;
    case 0x203:
        SsdDebWaveKeyoff(packet->arguments[0]);
        break;
    case 0x204:
        SsdMidiDevSetMidiEvent(packet->arguments[0], packet->arguments[1]);
        break;
    case 0x205:
        SsdMidiDevSetHWEffect(packet->arguments[0], (unsigned char)packet->arguments[1]);
        break;
    case 0x300:
        SsdDebAddEffect(packet->data, packet->arguments[1], packet->arguments[2], (unsigned short)packet->arguments[3]);
        break;
    case 0x301:
        SsdDebPlayEffect();
        break;
    case 0x400:
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
    int envelopes[48];
    RssdRpcPacket request;
    SsdGlobalWork *work = SsdGetGlobalWorkPtr();
    EnvelopeCache *cache = &RssdWork.envelopeCache;
    int voice;
    if (!cache->frames) {
        cache->count = work->intervalCount;
        cache->total = work->intervalTotal;
        work->intervalTotal = 0;
        work->intervalCount = 0;
        cache->frames = 20;
    }
    --cache->frames;
    request.arguments[0] = cache->count;
    request.arguments[1] = cache->total;
    request.arguments[2] = work->status;
    for (voice = 0; voice < 48; ++voice)
        envelopes[voice] = work->voices[voice].envelope;
    RssdCallFunc(0x102, &request, envelopes, sizeof(envelopes));
}

void RssdGetSeqPlayInfo(RssdRpcPacket *packet)
{
    SsdSequence *sequence = SsdGetSequencePtr(packet->arguments[0]);
    packet->arguments[1] = sequence->position;
    packet->arguments[2] = sequence->time >> 8;
    packet->arguments[3] = (sequence->flags & 0xffff0000) + sequence->playValue;
    RssdCallFunc(0x103, packet, sequence->playInfo, 16);
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
        RssdWork.flags |= 1;
        context = &RssdWork;
        receiveSize = sizeof(RssdRpcPacket);
        callback = RssdSifRpcCallback;
        sendSize = packet->dataSize + sizeof(RssdRpcPacket);
        if (sceSifCallRpc(&RssdWork.client, packet->command, 0, packet,
            sendSize, packet, receiveSize, callback, context) < 0) {
            RssdWork.flags &= ~1;
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
    RssdWork.flags &= ~1;
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
    node = AllocSysMemory(0, allocationSize, 0);
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
    node = AllocSysMemory(0, size + sizeof(RssdRequestNode), 0);
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
