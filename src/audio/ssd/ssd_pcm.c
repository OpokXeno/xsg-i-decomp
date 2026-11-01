/* Original SSD ssd_pcm.c translation unit. Unmodeled spans retain original storage. */

typedef struct SsdProcess SsdProcess;
struct SsdProcess {
    unsigned short flags;
    unsigned short reset_control;
    void (*callback)(SsdProcess *process, void *context);
    void *context;
    SsdProcess *next;
} ;

typedef struct SsdModulation {
    int current;
    int increment;
    int target;
    short duration;
} SsdModulation;

typedef struct SsdPcmStreamState {
    unsigned char flags;
    unsigned char playing;
    unsigned char transfer_busy;
    unsigned char reset_control;
    unsigned char active_buffer;
    unsigned char dma_channel;
    void *buffers[2];
    int buffer_bytes;
    void (*fill_callback)(void *context);
    void *callback_context;
    SsdModulation volume;
    SsdProcess process;
} SsdPcmStreamState;

typedef struct SsdDriverState {
    unsigned char original_prefix[0x1df];
    unsigned char stereo_enabled;
} SsdDriverState;

extern SsdDriverState SsdWork;

SsdPcmStreamState SsdPcmStrWork;

extern int CpuDisableIntr(void);

extern int CpuEnableIntr(void);

extern void *SsdNewPtr(int bytes);

extern unsigned int SsdGetMemoryFreeSize(void);

extern int printf(const char *format, ...);

extern void SsdSetPcmStreamCallback(void (*callback)(void));

extern int SsdStreamInterruptProcess(int channel, void *context);

extern int sceSdSetTransIntrHandler(int channel, int (*handler)(int, void *), void *context);

extern SsdProcess *SsdLinkProc(SsdProcess *process,
                             void (*callback)(SsdProcess *, void *), void *context);

extern int sceSdSetParam(int entry, int value);

extern void SsdUnlinkProc(SsdProcess *process);

extern void SsdDisposePtr(void *buffer);

extern int sceSdBlockTrans(int channel, int mode, void *buffer, int bytes);

extern int sceSdBlockTransStatus(int channel, int wait);

extern void SsdCalculateModulate(SsdModulation *modulation);

void SsdInitPcmStream(int buffer_bytes, void (*callback)(void *), void *context);

void SsdDisposePcmStream(void);

void SsdResumePcmStream(void);

void SsdSuspendPcmStream(void);

void SsdPlayPcmStream(int volume, int duration);

void SsdStopPcmStream(int duration);

int SsdGetPcmStreamStatus(void);

void SsdSetPcmStreamData(const void *source);

void SsdSetPcmStreamVolume(int volume, int duration);

void SsdPcmStreamVolumeFadeProc(void *process, void *context);

void SsdPcmStreamCallback(void);

void SsdStreamMonoCopyMemory(unsigned char *destination, const void *source_buffer, int bytes);

void SsdStreamCopyMemory(void *destination_buffer, const void *source_buffer, int bytes);

void SsdStreamClearMemory(void *buffer, int bytes);

void SsdInitPcmStream(int buffer_bytes, void (*callback)(void *), void *context)
{
    SsdPcmStrWork.flags = 1;
    SsdPcmStrWork.playing = 0;
    SsdPcmStrWork.transfer_busy = 0;
    SsdPcmStrWork.reset_control = 0;
    SsdPcmStrWork.active_buffer = 0;
    SsdPcmStrWork.dma_channel = 1;
    CpuDisableIntr();
    SsdPcmStrWork.buffers[0] = SsdNewPtr(buffer_bytes * 2);
    CpuEnableIntr();
    if (!SsdPcmStrWork.buffers[0]) {
        printf("Ssd lack of memory  pcm stream dma buffer !  Alloc=%x  Free=%x\n",
               buffer_bytes * 2, SsdGetMemoryFreeSize());
    } else {
        unsigned char *buffer = SsdPcmStrWork.buffers[0];
        SsdPcmStrWork.buffers[1] = buffer + buffer_bytes;
        SsdPcmStrWork.buffer_bytes = buffer_bytes;
        SsdPcmStrWork.fill_callback = callback;
        SsdPcmStrWork.callback_context = context;
        SsdSetPcmStreamVolume(0, 0);
        CpuDisableIntr();
        SsdSetPcmStreamCallback(SsdPcmStreamCallback);
        sceSdSetTransIntrHandler(SsdPcmStrWork.dma_channel,
                                SsdStreamInterruptProcess, &SsdWork);
        SsdLinkProc(&SsdPcmStrWork.process, SsdPcmStreamVolumeFadeProc, &SsdPcmStrWork);
        CpuEnableIntr();
    }
}

void SsdDisposePcmStream(void)
{
    if (SsdPcmStrWork.playing == 1)
        SsdStopPcmStream(0);
    sceSdSetParam(SsdPcmStrWork.dma_channel | 0xf80, 0);
    sceSdSetParam(SsdPcmStrWork.dma_channel | 0x1080, 0);
    CpuDisableIntr();
    SsdUnlinkProc(&SsdPcmStrWork.process);
    sceSdSetTransIntrHandler(SsdPcmStrWork.dma_channel, 0, 0);
    SsdSetPcmStreamCallback(0);
    CpuEnableIntr();
    if (SsdPcmStrWork.buffers[0]) {
        SsdDisposePtr(SsdPcmStrWork.buffers[0]);
        SsdPcmStrWork.buffers[0] = 0;
    }
    SsdPcmStrWork.flags = 0;
}

void SsdResumePcmStream(void)
{
    if (SsdPcmStrWork.playing == 1) {
        SsdPcmStrWork.transfer_busy = 1;
        CpuDisableIntr();
        SsdPcmStrWork.flags &= ~2;
        sceSdBlockTrans(SsdPcmStrWork.dma_channel, 16,
                        SsdPcmStrWork.buffers[0], SsdPcmStrWork.buffer_bytes * 2);
        CpuEnableIntr();
        SsdPcmStrWork.transfer_busy = 0;
    }
}

void SsdSuspendPcmStream(void)
{
    if (SsdPcmStrWork.playing == 1) {
        SsdPcmStrWork.transfer_busy = 1;
        CpuDisableIntr();
        SsdPcmStrWork.flags |= 2;
        sceSdBlockTrans(SsdPcmStrWork.dma_channel, 2, 0, 0);
        CpuEnableIntr();
        SsdPcmStrWork.transfer_busy = 0;
    }
}

void SsdPlayPcmStream(int volume, int duration)
{
    SsdPcmStrWork.volume.duration = 0;
    if (SsdPcmStrWork.playing != 1) {
        SsdStreamClearMemory(SsdPcmStrWork.buffers[0], SsdPcmStrWork.buffer_bytes * 2);
        SsdPcmStrWork.transfer_busy = 1;
        SsdPcmStrWork.volume.current = 0;
        sceSdSetParam(SsdPcmStrWork.dma_channel | 0xf80, 0);
        sceSdSetParam(SsdPcmStrWork.dma_channel | 0x1080, 0);
        CpuDisableIntr();
        sceSdBlockTrans(SsdPcmStrWork.dma_channel, 16,
                        SsdPcmStrWork.buffers[0], SsdPcmStrWork.buffer_bytes * 2);
        CpuEnableIntr();
        SsdPcmStrWork.transfer_busy = 0;
    }
    SsdSetPcmStreamVolume(volume, duration);
    SsdPcmStrWork.playing = 1;
}

void SsdStopPcmStream(int duration)
{
    SsdSetPcmStreamVolume(0, duration);
    if (!duration) {
        CpuDisableIntr();
        if (SsdPcmStrWork.playing == 1) {
            SsdPcmStrWork.transfer_busy = 1;
            SsdPcmStrWork.playing = 0;
            sceSdBlockTrans(SsdPcmStrWork.dma_channel, 2, 0, 0);
            SsdPcmStrWork.transfer_busy = 0;
        }
        CpuEnableIntr();
    }
}

int SsdGetPcmStreamStatus(void)
{
    return SsdPcmStrWork.playing;
}

void SsdSetPcmStreamData(const void *source)
{
    int active;
    void *destination;
    int bytes;
    active = sceSdBlockTransStatus(SsdPcmStrWork.dma_channel, 0) >> 24;
    SsdPcmStrWork.active_buffer = active;
    destination = SsdPcmStrWork.buffers[active ^ 1];
    bytes = SsdPcmStrWork.buffer_bytes;
    if (source) {
        if (!SsdWork.stereo_enabled)
            SsdStreamMonoCopyMemory(destination, source, bytes);
        else
            SsdStreamCopyMemory(destination, source, bytes);
    } else {
        SsdStreamClearMemory(destination, bytes);
    }
}

void SsdSetPcmStreamVolume(int volume, int duration)
{
    int target = (unsigned int)volume << 16;
    if (!duration) {
        int level;
        SsdPcmStrWork.volume.current = target;
        target >>= 16;
        target = (unsigned int)target * target * 2;
        level = target & 0xffff;
        sceSdSetParam(SsdPcmStrWork.dma_channel | 0xf80, level);
        sceSdSetParam(SsdPcmStrWork.dma_channel | 0x1080, level);
    } else {
        int difference = (unsigned int)target - (unsigned int)SsdPcmStrWork.volume.current;
        SsdPcmStrWork.volume.increment = difference / duration;
    }
    /* The immediate path stores its squared amplitude in target in the original. */
    SsdPcmStrWork.volume.target = target;
    SsdPcmStrWork.volume.duration = duration;
}

void SsdPcmStreamVolumeFadeProc(void *process, void *context)
{
    SsdPcmStreamState *stream = context;
    int volume;
    int level;

    if (stream->volume.duration) {
        SsdCalculateModulate(&stream->volume);
        volume = stream->volume.current;
        level = volume >> 16;
        level = (level * level) << 1;
        level &= 0xffff;
        sceSdSetParam(stream->dma_channel | 0xf80, level);
        sceSdSetParam(stream->dma_channel | 0x1080, level);
        /*
         * Always false: (volume << 16) has no low bits set. The original keeps
         * the raw volume word live in a callee-saved register across both SDK
         * writes with no visible later use; this guard reproduces that lifetime.
         * Combine folds it and the dead store is deleted after register
         * allocation, so it emits no code.
         */
        if ((volume << 16) & 0xffff) {
            stream->volume.current = volume;
        }
    }
}

void SsdPcmStreamCallback(void)
{
    if (SsdPcmStrWork.flags & 2)
        return;
    if (SsdPcmStrWork.playing != 1)
        return;
    if (SsdPcmStrWork.fill_callback)
        SsdPcmStrWork.fill_callback(SsdPcmStrWork.callback_context);
}

void SsdStreamMonoCopyMemory(unsigned char *destination, const void *source_buffer, int bytes)
{
    const unsigned char *source = source_buffer;
    int block;
    for (block = 0; block < bytes / 1024; block++) {
        SsdStreamCopyMemory(destination, source, 512);
        destination += 512;
        SsdStreamCopyMemory(destination, source, 512);
        destination += 512;
        source += 1024;
    }
}

void SsdStreamCopyMemory(void *destination_buffer, const void *source_buffer, int bytes)
{
    unsigned int *destination = destination_buffer;
    const unsigned int *source = source_buffer;
    while (bytes > 0) {
        unsigned int first;
        unsigned int second;
        unsigned int third;
        unsigned int fourth;
        bytes -= 16;
        first = source[0];
        second = source[1];
        third = source[2];
        fourth = source[3];
        destination[0] = first;
        destination[1] = second;
        destination[2] = third;
        destination[3] = fourth;
        source += 4;
        destination += 4;
    }
}

void SsdStreamClearMemory(void *buffer, int bytes)
{
    unsigned int *destination = buffer;
    while (bytes > 0) {
        bytes -= 16;
        destination[3] = 0;
        destination[2] = 0;
        destination[1] = 0;
        destination[0] = 0;
        destination += 4;
    }
}
