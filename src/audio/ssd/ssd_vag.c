/* Original SSD ssd_vag.c translation unit. Unmodeled spans retain original storage. */

typedef struct SsdVoice {
    unsigned char index, core, voice, state;
    unsigned short dirty;
    unsigned char effect, mode;
    short volume[2];
    unsigned short parameter_0c, parameter_0e;
    unsigned char envelope_state[4];
    unsigned short pitch, modulation;
    unsigned int flags, sample_start;
    unsigned char sample_state[4];
    unsigned short adsr1, adsr2;
    unsigned char unmodeled_028[216];
} SsdVoice;

typedef struct SsdModulator {
    union {
        int value;
        struct {
            unsigned short fraction;
            short integer;
        } parts;
    } current;
    int delta, target;
    short ticks;
} SsdModulator;

typedef struct SsdVagChannel {
    SsdVoice *voice;
    unsigned char voice_state[4];
    unsigned char data_flags, channel_state;
    short pitch;
    SsdModulator controls[2];
    unsigned char effect, flags;
} SsdVagChannel;

typedef struct SsdProc SsdProc;
struct SsdProc {
    unsigned short kind;
    unsigned short flags;
    void (*callback)(SsdProc *, void *);
    void *arg;
    SsdProc *next;
} ;

typedef struct SsdVagWork {
    unsigned char flags;
    unsigned char status;
    unsigned char buffer_selector;
    unsigned char dma_channel;
    unsigned char mode;
    signed char channel_count;
    signed char dma_channel_index;
    unsigned char process_state[4];
    SsdModulator master_volume;
    unsigned char transfer_state[4];
    unsigned char *buffers[2];
    int buffer_bytes;
    int half_buffer_bytes;
    int transfer_stride;
    void (*callback)(void *);
    void *callback_arg;
    unsigned char *transfer_source;
    unsigned char total_blocks;
    unsigned char half_blocks;
    unsigned char completed_blocks;
    unsigned char buffer_index;
    SsdVagChannel channels[4];
    SsdProc update_proc;
} SsdVagWork;

typedef struct SsdVagBlock {
    unsigned int header;
    unsigned int samples[3];
} SsdVagBlock;

typedef void (*SsdVagTransferCallback)(int dmaChannel, void *argument);

typedef struct SsdGlobalWork {
    unsigned char unmodeled_000[412];
    SsdVagTransferCallback vagTransferCallback;
    void *vagTransferArgument;
    unsigned char unmodeled_1a4[200];
    SsdVoice voices[48];
} SsdGlobalWork;

typedef struct SsdVagPacket {
    unsigned char coding, flags;
    unsigned char samples[14];
} SsdVagPacket;

SsdVagWork SsdVagStrWork;


extern int printf(const char *format, ...);

extern SsdGlobalWork SsdWork;

extern void CpuDisableIntr(void);

extern void CpuEnableIntr(void);

extern unsigned char *SsdNewPtr(int bytes);

extern void SsdSetVagStreamCallback(void (*callback)(void));

extern void SsdStreamInterruptProcess(void);

extern void sceSdSetTransIntrHandler(int channel, void (*handler)(void), void *arg);

extern SsdProc *SsdLinkProc(SsdProc *proc, void (*callback)(SsdProc *, void *), void *arg);

extern void SsdUnlinkProc(SsdProc *proc);

extern void SsdDisposePtr(void *ptr);

extern void SsdStreamCopyMemory(void *destination, const void *source, int bytes);

extern void sceSdSetCoreAttr(int attribute, int value);

extern void sceSdSetAddr(int attribute, unsigned int address);

extern void SsdKeyonVoice(SsdVoice *voice);

extern void SsdSetIrqCallback(void (*)(void));

extern void SsdSetPcmStreamCallback(void (*)(int, void *));

extern void SsdCutoffUnlinkVoice(SsdVoice *voice);

extern SsdGlobalWork *SsdGetGlobalWorkPtr(void);

extern int Kprintf(const char *format, ...);

extern int sceSdVoiceTransStatus(int, int);

extern int sceSdVoiceTrans(int, int, void *, unsigned int, int);

extern void SsdCalcDirectVoiceVolume(short *volume, int level, int pan);

extern void SsdCalculateModulate(SsdModulator *modulator);

extern void SsdUnlinkChannelVoice(SsdVoice *voice);

#define SSD_VAG_PREPARE_TRANSFER(out_packet, out_spu_base, out_bank, block) \
    do { \
        int stride = SsdVagStrWork.transfer_stride; \
        int block_byte_offset = stride * (block); \
        unsigned char *block_source = SsdVagStrWork.transfer_source; \
        int channel = SsdVagStrWork.dma_channel_index; \
        int channel_offset = channel * 1024; \
        (out_bank) = SsdVagStrWork.buffer_index; \
        block_source += block_byte_offset; \
        (out_spu_base) = channel * 2048 + 0x6000; \
        (out_packet) = (SsdVagPacket *)&block_source[channel_offset]; \
    } while (0)

void SsdInitVagStreamStereo(int bytes, void (*callback)(void *), void *arg);

void SsdInitVagStreamMono(int bytes, int channels, void (*callback)(void *), void *arg);

void SsdInitVagStream(int bytes_per_channel, int channels,
                     void (*callback)(void *), void *arg);

void SsdDisposeVagStream(void);

void SsdResumeVagStream(void);

void SsdSuspendVagStream(void);

void SsdPlayVagStream(int volume, int ticks, int pitch);

void SsdStopVagStream(int ticks);

int SsdGetVagStreamStatus(void);

int SsdGetVagStreamParam(int channel, int *pitch, int *volume, int *pan);

void SsdSetVagStreamDataStereo(const void *source);

void SsdSetVagStreamDataMono(int channel, const unsigned char *source, int bytes);

void SsdVagStreamCallback(void);

void SsdVagStreamClearMemory(void *buffer, int bytes);

void SsdVagStreamIrqCallback(void);

void SsdVagStreamDmaCallback(int first, void *context);

void SsdVagStreamInitTrans(void *source, int bytes);

void SsdVagStreamBlockTrans(int channel, int start, void *buffer, int bytes);

void SsdVagStreamSpuTransfer(void);

void SsdVagStreamDmaTransfer(int first);

int SsdVagStreamTransferStatus(int channel, int wait);

void SsdInitVagStreamVoice(int channel);

void SsdSetVagStreamPitch(int channel, int pitch);

void SsdSetVagStreamVolume(int channel, int volume, int ticks);

void SsdSetVagStreamPanpot(int channel, int pan);

void SsdSetVagStreamParam(int channel, int pitch, int volume, int pan);

void SsdSetVagStreamHWEffect(int channel, int control, int dirty);

void SsdSetVagStreamMasterVolume(int volume, int ticks);

void SsdVagStreamVolumeFadeProc(SsdProc *proc, void *context);

void SsdAllocVoiceVagStream(void);

void SsdFreeVoiceVagStream(void);

void SsdInitVagStreamStereo(int bytes, void (*callback)(void *), void *arg)
{
    SsdInitVagStream(bytes / 2, 2, callback, arg);
    SsdVagStrWork.channels[0].pitch = 4096;
    SsdVagStrWork.channels[0].controls[0].current.value = 127 * 65536;
    SsdVagStrWork.channels[0].controls[1].current.value = 0;
    SsdVagStrWork.channels[0].effect = 8;
    SsdVagStrWork.channels[0].flags = 0;
    SsdVagStrWork.channels[1].pitch = 4096;
    SsdVagStrWork.channels[1].controls[0].current.value = 127 * 65536;
    SsdVagStrWork.channels[1].controls[1].current.value = 127 * 65536;
    SsdVagStrWork.channels[1].effect = 8;
    SsdVagStrWork.channels[1].flags = 0;
}

void SsdInitVagStreamMono(int bytes, int channels, void (*callback)(void *), void *arg)
{
    int channel;

    if (channels > 4) {
        printf("Ssd init vag stream  Error NumChannel=%d\n", channels);
        return;
    }
    SsdInitVagStream(bytes, channels, callback, arg);
    for (channel = 0; channel < channels; channel++) {
        SsdVagStrWork.channels[channel].pitch = 4096;
        SsdVagStrWork.channels[channel].controls[0].current.value = 127 * 65536;
        SsdVagStrWork.channels[channel].controls[1].current.value = 64 * 65536;
        SsdVagStrWork.channels[channel].effect = 8;
        SsdVagStrWork.channels[channel].flags = 0;
    }
}

void SsdInitVagStream(int bytes_per_channel, int channels,
                     void (*callback)(void *), void *arg)
{
    int half_bytes = bytes_per_channel * channels;
    int total_bytes = half_bytes * 2;

    SsdVagStrWork.flags = 1;
    SsdVagStrWork.status = 0;
    SsdVagStrWork.buffer_selector = 0;
    SsdVagStrWork.dma_channel = 1;
    SsdVagStrWork.mode = 1;
    SsdVagStrWork.channel_count = channels;
    CpuDisableIntr();
    SsdVagStrWork.buffers[0] = SsdNewPtr(total_bytes);
    CpuEnableIntr();
    if (!SsdVagStrWork.buffers[0]) {
        printf("Ssd lack of memory  vag stream dma buffer!\n");
        return;
    }
    SsdVagStrWork.buffers[1] = SsdVagStrWork.buffers[0] + half_bytes;
    SsdVagStrWork.buffer_bytes = total_bytes;
    SsdVagStrWork.half_buffer_bytes = half_bytes;
    SsdVagStrWork.callback = callback;
    SsdVagStrWork.callback_arg = arg;
    SsdVagStrWork.transfer_stride = channels * 1024;
    SsdVagStreamClearMemory(SsdVagStrWork.buffers[0], total_bytes);
    SsdVagStreamInitTrans(SsdVagStrWork.buffers[0], bytes_per_channel);
    SsdVagStrWork.master_volume.current.value = 0;
    SsdAllocVoiceVagStream();
    CpuDisableIntr();
    SsdSetVagStreamCallback(SsdVagStreamCallback);
    sceSdSetTransIntrHandler(SsdVagStrWork.dma_channel, SsdStreamInterruptProcess, &SsdWork);
    SsdLinkProc(&SsdVagStrWork.update_proc, SsdVagStreamVolumeFadeProc, &SsdVagStrWork);
    CpuEnableIntr();
}

void SsdDisposeVagStream(void)
{
    if (SsdVagStrWork.status == 1)
        SsdStopVagStream(0);
    CpuDisableIntr();
    sceSdSetTransIntrHandler(SsdVagStrWork.dma_channel, 0, 0);
    SsdSetVagStreamCallback(0);
    SsdUnlinkProc(&SsdVagStrWork.update_proc);
    CpuEnableIntr();
    if (SsdVagStrWork.buffers[0]) {
        SsdDisposePtr(SsdVagStrWork.buffers[0]);
        SsdVagStrWork.buffers[0] = 0;
    }
    SsdVagStrWork.flags = 0;
}

void SsdResumeVagStream(void)
{
    if (SsdVagStrWork.status == 1) {
        SsdVagStrWork.flags &= ~2;
        CpuDisableIntr();
        SsdVagStreamBlockTrans(SsdVagStrWork.dma_channel, 1, 0, 0);
        CpuEnableIntr();
    }
}

void SsdSuspendVagStream(void)
{
    if (SsdVagStrWork.status == 1) {
        SsdVagStrWork.flags |= 2;
        CpuDisableIntr();
        SsdVagStreamBlockTrans(SsdVagStrWork.dma_channel, 0, 0, 0);
        CpuEnableIntr();
    }
}

void SsdPlayVagStream(int volume, int ticks, int pitch)
{
    int channels = SsdVagStrWork.channel_count;
    int channel;

    if (SsdVagStrWork.status != 1) {
        SsdVagStreamClearMemory(SsdVagStrWork.buffers[0], SsdVagStrWork.buffer_bytes);
        SsdSetVagStreamMasterVolume(0, 0);
        for (channel = 0; channel < channels; channel++) {
            SsdVagStrWork.channels[channel].data_flags &= ~16;
            SsdVagStrWork.channels[channel].pitch = pitch;
        }
        SsdVagStrWork.status = 1;
        SsdSetVagStreamMasterVolume(volume, ticks);
        CpuDisableIntr();
        SsdVagStreamBlockTrans(SsdVagStrWork.dma_channel, 1, 0, 0);
        CpuEnableIntr();
    } else {
        SsdSetVagStreamMasterVolume(volume, ticks);
    }
}

void SsdStopVagStream(int ticks)
{
    if (SsdVagStrWork.status) {
        SsdSetVagStreamMasterVolume(0, ticks);
        if (ticks == 0) {
            SsdVagStrWork.status = 0;
            SsdVagStreamClearMemory(SsdVagStrWork.buffers[0], SsdVagStrWork.buffer_bytes);
            CpuDisableIntr();
            SsdVagStreamBlockTrans(SsdVagStrWork.dma_channel, 0, 0, 0);
            CpuEnableIntr();
        }
    }
}

int SsdGetVagStreamStatus(void)
{
    return SsdVagStrWork.status;
}

int SsdGetVagStreamParam(int channel, int *pitch, int *volume, int *pan)
{
    SsdVagChannel *stream = &SsdVagStrWork.channels[channel];

    if (pitch)
        *pitch = stream->pitch;
    if (volume)
        *volume = stream->controls[0].current.parts.integer;
    if (pan)
        *pan = stream->controls[1].current.parts.integer;
    return SsdVagStrWork.status;
}

void SsdSetVagStreamDataStereo(const void *source)
{
    unsigned char *buffer;
    int selector;

    selector = SsdVagStreamTransferStatus(SsdVagStrWork.dma_channel, 0) >> 24;
    SsdVagStrWork.buffer_selector = selector;
    buffer = SsdVagStrWork.buffers[selector ^ 1];
    if (source) {
        SsdStreamCopyMemory(buffer, source, SsdVagStrWork.half_buffer_bytes);
        SsdVagStrWork.channels[0].data_flags |= 16;
        SsdVagStrWork.channels[1].data_flags |= 16;
    } else {
        SsdVagStreamClearMemory(buffer, SsdVagStrWork.half_buffer_bytes);
        SsdVagStrWork.channels[0].data_flags &= ~16;
        SsdVagStrWork.channels[1].data_flags &= ~16;
    }
}

void SsdSetVagStreamDataMono(int channel, const unsigned char *source, int bytes)
{
    int block;
    unsigned char *buffer;

    block = SsdVagStreamTransferStatus(SsdVagStrWork.dma_channel, 0) >> 24;
    SsdVagStrWork.buffer_selector = block;
    block ^= 1;
    buffer = SsdVagStrWork.buffers[block] + channel * 1024;
    if (source) {
        for (block = 0; block < SsdVagStrWork.half_blocks; block++) {
            int chunk = 1024;

            if (bytes < chunk) {
                chunk = bytes;
                SsdVagStreamClearMemory(buffer, 1024);
            }
            if (chunk) {
                SsdStreamCopyMemory(buffer, source, chunk);
                bytes -= chunk;
                source += 1024;
            }
            buffer += SsdVagStrWork.transfer_stride;
        }
        SsdVagStrWork.channels[channel].data_flags |= 16;
    } else {
        for (block = 0; block < SsdVagStrWork.half_blocks; block++) {
            SsdVagStreamClearMemory(buffer, 1024);
            buffer += SsdVagStrWork.transfer_stride;
        }
        SsdVagStrWork.channels[channel].data_flags &= ~16;
    }
}

void SsdVagStreamCallback(void)
{
    if (SsdVagStrWork.flags & 2)
        return;
    if (SsdVagStrWork.status != 1)
        return;
    if (SsdVagStrWork.callback)
        SsdVagStrWork.callback(SsdVagStrWork.callback_arg);
}

void SsdVagStreamClearMemory(void *buffer, int bytes)
{
    SsdVagBlock *blocks = buffer;
    int index;
    int count = bytes >> 4;

    for (index = 0; index < count; index++) {
        blocks->header = 524;
        blocks->samples[0] = blocks->samples[1] = blocks->samples[2] = 0;
        blocks++;
    }
}

void SsdVagStreamIrqCallback(void)
{
    SsdVagStreamSpuTransfer();
}

void SsdVagStreamDmaCallback(int first, void *context)
{
    if (SsdVagStrWork.flags & 0x20) {
        int channel;
        int start_voices;

        first = (SsdVagStrWork.flags >> 4) & 1;
        start_voices = first;
        if (++SsdVagStrWork.dma_channel_index < SsdVagStrWork.channel_count) {
            SsdVagStreamDmaTransfer(first);
            return;
        }
        sceSdSetCoreAttr(5, 0);
        SsdVagStrWork.flags &= ~0x10;
        sceSdSetAddr(0x1f01, 0x6010 + (SsdVagStrWork.buffer_index << 10));
        sceSdSetCoreAttr(5, 1);
        if (start_voices) {
            for (channel = 0; channel < SsdVagStrWork.channel_count; channel++)
                SsdKeyonVoice(SsdVagStrWork.channels[channel].voice);
        }
    }
}

void SsdVagStreamInitTrans(void *source, int bytes)
{
    int blocks;

    SsdVagStrWork.transfer_source = source;
    blocks = bytes / 1024;
    SsdVagStrWork.total_blocks = blocks * 2;
    SsdVagStrWork.half_blocks = blocks;
    SsdVagStrWork.completed_blocks = 0;
}

void SsdVagStreamBlockTrans(int channel, int start, void *buffer, int bytes)
{
    int voice;
    if (start == 1) {
        SsdVagStrWork.flags |= 0x30;
        SsdVagStrWork.buffer_index = 0;
        sceSdSetCoreAttr(5, 0);
        SsdSetIrqCallback(SsdVagStreamIrqCallback);
        SsdSetPcmStreamCallback(SsdVagStreamDmaCallback);
        for (voice = 0; voice < SsdVagStrWork.channel_count; voice++)
            SsdInitVagStreamVoice(voice);
        SsdVagStrWork.dma_channel_index = 0;
        SsdVagStreamDmaTransfer(1);
    } else {
        SsdVagStrWork.flags &= ~0x30;
        SsdSetPcmStreamCallback(0);
        for (voice = 0; voice < SsdVagStrWork.channel_count; voice++)
            SsdCutoffUnlinkVoice(SsdVagStrWork.channels[voice].voice);
        sceSdSetCoreAttr(5, 0);
    }
}

void SsdVagStreamSpuTransfer(void)
{
    int completedIndex;
    int nextIndex;

    sceSdSetCoreAttr(5, 0);
    SsdVagStrWork.dma_channel_index = 0;
    completedIndex = SsdVagStrWork.completed_blocks;
    nextIndex = completedIndex + 1;
    if (nextIndex == SsdVagStrWork.total_blocks) {
        nextIndex = 0;
    }
    SsdVagStrWork.completed_blocks = nextIndex;
    SsdVagStrWork.buffer_index = !SsdVagStrWork.buffer_index;
    if (completedIndex == 0 || completedIndex == SsdVagStrWork.half_blocks) {
        SsdGlobalWork *work = SsdGetGlobalWorkPtr();
        if (work->vagTransferCallback != 0) {
            work->vagTransferCallback(SsdVagStrWork.dma_channel, work->vagTransferArgument);
        }
    }
    SsdVagStreamDmaTransfer(0);
}

void SsdVagStreamDmaTransfer(int first)
{
    SsdVagPacket *packet;
    SsdVagPacket *last_packet;
    unsigned int destination;
    int status;
    int buffer_index;
    unsigned int spu_start;
    {
        int block = SsdVagStrWork.completed_blocks;
        SSD_VAG_PREPARE_TRANSFER(packet, spu_start, buffer_index, block);
        destination = buffer_index * 1024 + spu_start;
    }
    if (first)
        packet->flags = 6;
    last_packet = &packet[63];
    if (buffer_index)
        last_packet->flags = 3;
    else
        last_packet->flags = 2;
    status = sceSdVoiceTransStatus(SsdVagStrWork.dma_channel, 0);
    if (status < 0)
        Kprintf("sceSdVoiceTransStatus Error=%d", status);
    status = sceSdVoiceTrans(SsdVagStrWork.dma_channel, 0, packet, destination, 1024);
    if (status < 0)
        Kprintf("sceSdVoiceTrans Error=%d", status);
}

int SsdVagStreamTransferStatus(int channel, int wait)
{
    return (SsdVagStrWork.completed_blocks >= SsdVagStrWork.half_blocks) << 24;
}

void SsdInitVagStreamVoice(int channel)
{
    SsdVagChannel *stream = &SsdVagStrWork.channels[channel];
    SsdVoice *voice = stream->voice;

    voice->pitch = stream->pitch;
    SsdCalcDirectVoiceVolume(voice->volume,
        (stream->controls[0].current.value >> 16) * (SsdVagStrWork.master_volume.current.value >> 16) / 127,
        stream->controls[1].current.value >> 16);
    voice->dirty = 0;
    voice->effect = stream->effect;
    voice->mode = stream->flags;
    voice->adsr1 = 0x20ff;
    voice->parameter_0c = 0;
    voice->parameter_0e = 0;
    voice->modulation = 0;
    voice->sample_start = 0x6000 + channel * 2048;
    voice->adsr2 = 0x5fc6;
}

void SsdSetVagStreamPitch(int channel, int pitch)
{
    if (SsdVagStrWork.channel_count < 2) {
        SsdVagChannel *stream = &SsdVagStrWork.channels[channel];
        SsdVoice *voice = stream->voice;

        stream->pitch = pitch;
        voice->pitch = pitch;
        voice->dirty |= 1;
    }
}

void SsdSetVagStreamVolume(int channel, int volume, int ticks)
{
    int end;
    int fixed_volume;
    SsdVagChannel *stream;

    if (channel < 0) {
        end = SsdVagStrWork.channel_count;
        channel = 0;
    } else {
        end = channel + 1;
    }
    fixed_volume = volume << 16;
    while (channel < end) {
        stream = &SsdVagStrWork.channels[channel];
        stream->controls[0].target = fixed_volume;
        stream->controls[0].ticks = ticks;
        if (ticks == 0) {
            int level;
            SsdVoice *voice;
            stream->controls[0].current.value = fixed_volume;
            level = volume * (SsdVagStrWork.master_volume.current.value >> 16) / 127;
            voice = stream->voice;
            SsdCalcDirectVoiceVolume(voice->volume, level, stream->controls[1].current.value >> 16);
            voice->dirty |= 2;
        } else {
            stream->controls[0].delta = (fixed_volume - stream->controls[0].current.value) / ticks;
        }
        channel++;
    }
}

void SsdSetVagStreamPanpot(int channel, int pan)
{
    int end;
    int fixed_pan;
    int level;
    SsdVagChannel *stream;
    SsdVoice *voice;

    if (channel < 0) {
        end = SsdVagStrWork.channel_count;
        channel = 0;
    } else {
        end = channel + 1;
    }
    if (channel >= end)
        return;
    fixed_pan = (unsigned int)pan << 16;
    do {
        int channel_volume;
        int master_volume;

        stream = &SsdVagStrWork.channels[channel];
        channel_volume = stream->controls[0].current.value >> 16;
        stream->controls[1].current.value = fixed_pan;
        master_volume = SsdVagStrWork.master_volume.current.value >> 16;
        level = channel_volume * master_volume / 127;
        voice = stream->voice;
        SsdCalcDirectVoiceVolume(voice->volume, level, pan);
        voice->dirty |= 2;
        ++channel;
    } while (channel < end);
}

void SsdSetVagStreamParam(int channel, int pitch, int volume, int pan)
{
    SsdVagChannel *stream = &SsdVagStrWork.channels[channel];
    SsdVoice *voice = stream->voice;

    stream->controls[0].current.value = volume * 65536;
    stream->pitch = pitch;
    stream->controls[1].current.value = pan * 65536;
    voice->pitch = pitch;
    SsdCalcDirectVoiceVolume(voice->volume,
        volume * (SsdVagStrWork.master_volume.current.value >> 16) / 127, pan);
    voice->dirty |= 3;
}

void SsdSetVagStreamHWEffect(int channel, int control, int dirty)
{
    SsdVagChannel *stream = &SsdVagStrWork.channels[channel];
    SsdVoice *voice = stream->voice;

    voice->effect = stream->effect;
    voice->dirty |= dirty;
}

void SsdSetVagStreamMasterVolume(int volume, int ticks)
{
    int fixed_volume;
    int channel;
    SsdVagChannel *stream;
    SsdVoice *voice;
    int level;

    SsdVagStrWork.master_volume.target = fixed_volume = volume << 16;
    if (ticks == 0) {
        SsdVagStrWork.master_volume.current.value = fixed_volume;
        stream = &SsdVagStrWork.channels[0];
        channel = 0;
        if (channel < SsdVagStrWork.channel_count) {
            do {
                level = (stream->controls[0].current.value >> 16) * volume / 127;
                voice = stream->voice;
                SsdCalcDirectVoiceVolume(voice->volume,
                    level,
                    stream->controls[1].current.value >> 16);
                voice->dirty |= 2;
                stream++;
                channel++;
            } while (channel < SsdVagStrWork.channel_count);
        }
    } else {
        SsdVagStrWork.master_volume.delta =
            (fixed_volume - SsdVagStrWork.master_volume.current.value) / ticks;
    }
    SsdVagStrWork.master_volume.ticks = ticks;
}

void SsdVagStreamVolumeFadeProc(SsdProc *proc, void *context)
{
    SsdVagWork *work = context;
    unsigned char changed[4];
    int channel;
    if (work->status == 1) {
    if (work->master_volume.ticks != 0) {
        SsdCalculateModulate(&work->master_volume);
        if (work->master_volume.current.value == 0) {
            SsdVagStreamBlockTrans(work->dma_channel, 0, 0, 0);
            work->status = 0;
            Kprintf("Stream stop !\n");
        }
        changed[0] = changed[1] = changed[2] = changed[3] = 1;
    } else {
        changed[0] = changed[1] = changed[2] = changed[3] = 0;
    }
    channel = 0;
    if (channel < work->channel_count) {
        unsigned char *modulated_channels = changed;
        do {
            SsdVagChannel *fading_channel = &work->channels[channel];
            if (fading_channel->controls[0].ticks != 0) {
                SsdCalculateModulate(&fading_channel->controls[0]);
                modulated_channels[channel] = 1;
            }
            channel++;
        } while (channel < work->channel_count);
    }
    channel = 0;
    if (channel < work->channel_count) {
        unsigned char *changed_channels = changed;
        do {
            if (changed_channels[channel]) {
                SsdVagChannel *stream = &work->channels[channel];
                SsdVoice *voice = stream->voice;
                SsdCalcDirectVoiceVolume(voice->volume,
                    (stream->controls[0].current.value >> 16) *
                        (work->master_volume.current.value >> 16) / 127,
                    stream->controls[1].current.value >> 16);
                voice->dirty |= 2;
            }
            channel++;
        } while (channel < work->channel_count);
    }
    }
}

void SsdAllocVoiceVagStream(void)
{
    int channelIndex;
    int firstVoice;

    CpuDisableIntr();
    channelIndex = 0;
    firstVoice = 44;
    if (SsdVagStrWork.channel_count > 0) {
        do {
            SsdVoice *voice = &SsdWork.voices[firstVoice + channelIndex];
            unsigned int streamFlags = 0xa0000;
            SsdUnlinkChannelVoice(voice);
            SsdVagStrWork.channels[channelIndex].voice = voice;
            voice->flags = (voice->flags & 0xffff) | streamFlags;
            channelIndex++;
        } while (channelIndex < SsdVagStrWork.channel_count);
    }
    CpuEnableIntr();
}

void SsdFreeVoiceVagStream(void)
{
    int channel;

    for (channel = 0; channel < SsdVagStrWork.channel_count; channel++) {
        SsdCutoffUnlinkVoice(SsdVagStrWork.channels[channel].voice);
        SsdVagStrWork.channels[channel].voice = 0;
    }
}
