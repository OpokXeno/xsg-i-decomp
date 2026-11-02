#include "ssd/ssd_vag.h"
#include "ssd/ssd_chan.h"
#include "ssd/ssd_dma.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_mem.h"
#include "ssd/ssd_pcm.h"
#include "ssd/ssd_voic.h"

/* Streams of up to four mono channels, each double-buffered in SPU memory at SSD_SPU_STREAM_BASE. */
#define SSD_VAG_MAX_CHANNELS 4
#define SSD_VAG_CHUNK_BYTES 1024
#define SSD_VAG_VOICE_STRIDE (SSD_VAG_CHUNK_BYTES * 2)
#define SSD_VAG_PACKETS_PER_CHUNK 64
#define SSD_VAG_FIRST_VOICE (SSD_VOICE_COUNT - SSD_VAG_MAX_CHANNELS)
#define SSD_VAG_VOICE_PRIORITY 0xa0000

/* SsdVagWork.flags above SSD_STREAM_FLAG_*: voices wait for the IRQ, a DMA chain is running. */
#define SSD_VAG_FLAG_START_PENDING 0x10
#define SSD_VAG_FLAG_TRANSFERRING 0x20

/* SsdVagChannel.data_flags: the application has supplied data for the next half. */
#define SSD_VAG_DATA_READY 0x10

/* VAG packet flags */
#define SSD_VAG_PACKET_LOOP_END 0x1
#define SSD_VAG_PACKET_LOOP 0x2
#define SSD_VAG_PACKET_LOOP_START 0x4

/* Silent packet: coding byte 0x0c, flags byte 0x02. */
#define SSD_VAG_SILENT_HEADER 0x20c

/* The IRQ address sits on the second packet of the chunk being played. */
#define SSD_VAG_IRQ_OFFSET 0x10

typedef struct SsdVagChannel {
    SsdVoice *voice;
    unsigned char voice_state[4];
    unsigned char data_flags, channel_state;
    short pitch;
    SsdRamp controls[2];
    unsigned char effect, flags;
} SsdVagChannel;

typedef struct SsdVagWork {
    unsigned char flags;
    unsigned char status;
    unsigned char buffer_selector;
    unsigned char dma_channel;
    unsigned char mode;
    signed char channel_count;
    signed char dma_channel_index;
    unsigned char process_state[4];
    SsdRamp master_volume;
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
    SsdVagChannel channels[SSD_VAG_MAX_CHANNELS];
    SsdProc update_proc;
} SsdVagWork;

typedef struct SsdVagBlock {
    unsigned int header;
    unsigned int samples[3];
} SsdVagBlock;

typedef struct SsdVagPacket {
    unsigned char coding, flags;
    unsigned char samples[14];
} SsdVagPacket;

SsdVagWork SsdVagStrWork;

void SsdInitVagStream(int bytes_per_channel, int channels,
                     void (*callback)(void *), void *arg);

void SsdSetVagStreamDataStereo(const void *source);

void SsdVagStreamCallback(int result, void *argument);

void SsdVagStreamClearMemory(void *buffer, int bytes);

void SsdVagStreamIrqCallback(int result, void *argument);

void SsdVagStreamDmaCallback(int first, void *context);

void SsdVagStreamInitTrans(void *source, int bytes);

void SsdVagStreamBlockTrans(int channel, int start, void *buffer, int bytes);

void SsdVagStreamSpuTransfer(void);

void SsdVagStreamDmaTransfer(int first);

int SsdVagStreamTransferStatus(int channel, int wait);

void SsdInitVagStreamVoice(int channel);

void SsdSetVagStreamPitch(int channel, int pitch);

void SsdSetVagStreamPanpot(int channel, int pan);

void SsdSetVagStreamHWEffect(int channel, int control, int dirty);

void SsdSetVagStreamMasterVolume(int volume, int ticks);

void SsdVagStreamVolumeFadeProc(SsdGlobalWork *global_work, void *context);

void SsdAllocVoiceVagStream(void);

void SsdFreeVoiceVagStream(void);

void SsdInitVagStreamStereo(int bytes, void (*callback)(void *), void *arg)
{
    SsdInitVagStream(bytes / 2, 2, callback, arg);
    SsdVagStrWork.channels[0].pitch = SSD_VOICE_PITCH_NORMAL;
    SsdVagStrWork.channels[0].controls[0].current.value = SSD_MIDI_VALUE_MAX * 65536;
    SsdVagStrWork.channels[0].controls[1].current.value = 0;
    SsdVagStrWork.channels[0].effect = SSD_VOICE_EFFECT_DRY;
    SsdVagStrWork.channels[0].flags = 0;
    SsdVagStrWork.channels[1].pitch = SSD_VOICE_PITCH_NORMAL;
    SsdVagStrWork.channels[1].controls[0].current.value = SSD_MIDI_VALUE_MAX * 65536;
    SsdVagStrWork.channels[1].controls[1].current.value = SSD_MIDI_VALUE_MAX * 65536;
    SsdVagStrWork.channels[1].effect = SSD_VOICE_EFFECT_DRY;
    SsdVagStrWork.channels[1].flags = 0;
}

void SsdInitVagStreamMono(int bytes, int channels, void (*callback)(void *), void *arg)
{
    int channel;

    if (channels > SSD_VAG_MAX_CHANNELS) {
        printf("Ssd init vag stream  Error NumChannel=%d\n", channels);
        return;
    }
    SsdInitVagStream(bytes, channels, callback, arg);
    for (channel = 0; channel < channels; channel++) {
        SsdVagStrWork.channels[channel].pitch = SSD_VOICE_PITCH_NORMAL;
        SsdVagStrWork.channels[channel].controls[0].current.value = SSD_MIDI_VALUE_MAX * 65536;
        SsdVagStrWork.channels[channel].controls[1].current.value = SSD_MIDI_PAN_CENTER * 65536;
        SsdVagStrWork.channels[channel].effect = SSD_VOICE_EFFECT_DRY;
        SsdVagStrWork.channels[channel].flags = 0;
    }
}

void SsdInitVagStream(int bytes_per_channel, int channels,
                     void (*callback)(void *), void *arg)
{
    int half_bytes = bytes_per_channel * channels;
    int total_bytes = half_bytes * 2;

    SsdVagStrWork.flags = SSD_STREAM_FLAG_OPEN;
    SsdVagStrWork.status = SSD_STREAM_STOPPED;
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
    SsdVagStrWork.transfer_stride = channels * SSD_VAG_CHUNK_BYTES;
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
    if (SsdVagStrWork.status == SSD_STREAM_PLAYING)
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
    if (SsdVagStrWork.status == SSD_STREAM_PLAYING) {
        SsdVagStrWork.flags &= ~SSD_STREAM_FLAG_SUSPENDED;
        CpuDisableIntr();
        SsdVagStreamBlockTrans(SsdVagStrWork.dma_channel, 1, 0, 0);
        CpuEnableIntr();
    }
}

void SsdSuspendVagStream(void)
{
    if (SsdVagStrWork.status == SSD_STREAM_PLAYING) {
        SsdVagStrWork.flags |= SSD_STREAM_FLAG_SUSPENDED;
        CpuDisableIntr();
        SsdVagStreamBlockTrans(SsdVagStrWork.dma_channel, 0, 0, 0);
        CpuEnableIntr();
    }
}

void SsdPlayVagStream(int volume, int ticks, int pitch)
{
    int channels = SsdVagStrWork.channel_count;
    int channel;

    if (SsdVagStrWork.status != SSD_STREAM_PLAYING) {
        SsdVagStreamClearMemory(SsdVagStrWork.buffers[0], SsdVagStrWork.buffer_bytes);
        SsdSetVagStreamMasterVolume(0, 0);
        for (channel = 0; channel < channels; channel++) {
            SsdVagStrWork.channels[channel].data_flags &= ~SSD_VAG_DATA_READY;
            SsdVagStrWork.channels[channel].pitch = pitch;
        }
        SsdVagStrWork.status = SSD_STREAM_PLAYING;
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
            SsdVagStrWork.status = SSD_STREAM_STOPPED;
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
        SsdVagStrWork.channels[0].data_flags |= SSD_VAG_DATA_READY;
        SsdVagStrWork.channels[1].data_flags |= SSD_VAG_DATA_READY;
    } else {
        SsdVagStreamClearMemory(buffer, SsdVagStrWork.half_buffer_bytes);
        SsdVagStrWork.channels[0].data_flags &= ~SSD_VAG_DATA_READY;
        SsdVagStrWork.channels[1].data_flags &= ~SSD_VAG_DATA_READY;
    }
}

void SsdSetVagStreamDataMono(int channel, const unsigned char *source, int bytes)
{
    int block;
    unsigned char *buffer;

    block = SsdVagStreamTransferStatus(SsdVagStrWork.dma_channel, 0) >> 24;
    SsdVagStrWork.buffer_selector = block;
    block ^= 1;
    buffer = SsdVagStrWork.buffers[block] + channel * SSD_VAG_CHUNK_BYTES;
    if (source) {
        for (block = 0; block < SsdVagStrWork.half_blocks; block++) {
            int chunk = SSD_VAG_CHUNK_BYTES;

            if (bytes < chunk) {
                chunk = bytes;
                SsdVagStreamClearMemory(buffer, SSD_VAG_CHUNK_BYTES);
            }
            if (chunk) {
                SsdStreamCopyMemory(buffer, source, chunk);
                bytes -= chunk;
                source += SSD_VAG_CHUNK_BYTES;
            }
            buffer += SsdVagStrWork.transfer_stride;
        }
        SsdVagStrWork.channels[channel].data_flags |= SSD_VAG_DATA_READY;
    } else {
        for (block = 0; block < SsdVagStrWork.half_blocks; block++) {
            SsdVagStreamClearMemory(buffer, SSD_VAG_CHUNK_BYTES);
            buffer += SsdVagStrWork.transfer_stride;
        }
        SsdVagStrWork.channels[channel].data_flags &= ~SSD_VAG_DATA_READY;
    }
}

void SsdVagStreamCallback(int result, void *argument)
{
    if (SsdVagStrWork.flags & SSD_STREAM_FLAG_SUSPENDED)
        return;
    if (SsdVagStrWork.status != SSD_STREAM_PLAYING)
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
        blocks->header = SSD_VAG_SILENT_HEADER;
        blocks->samples[0] = blocks->samples[1] = blocks->samples[2] = 0;
        blocks++;
    }
}

void SsdVagStreamIrqCallback(int result, void *argument)
{
    SsdVagStreamSpuTransfer();
}

void SsdVagStreamDmaCallback(int first, void *context)
{
    if (SsdVagStrWork.flags & SSD_VAG_FLAG_TRANSFERRING) {
        int channel;
        int start_voices;

        first = (SsdVagStrWork.flags >> 4) & 1;
        start_voices = first;
        if (++SsdVagStrWork.dma_channel_index < SsdVagStrWork.channel_count) {
            SsdVagStreamDmaTransfer(first);
            return;
        }
        sceSdSetCoreAttr(SD_C_IRQ_ENABLE | SD_CORE_1, 0);
        SsdVagStrWork.flags &= ~SSD_VAG_FLAG_START_PENDING;
        sceSdSetAddr(SD_A_IRQA | SD_CORE_1,
                     SSD_SPU_STREAM_BASE + SSD_VAG_IRQ_OFFSET + (SsdVagStrWork.buffer_index << 10));
        sceSdSetCoreAttr(SD_C_IRQ_ENABLE | SD_CORE_1, 1);
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
    blocks = bytes / SSD_VAG_CHUNK_BYTES;
    SsdVagStrWork.total_blocks = blocks * 2;
    SsdVagStrWork.half_blocks = blocks;
    SsdVagStrWork.completed_blocks = 0;
}

void SsdVagStreamBlockTrans(int channel, int start, void *buffer, int bytes)
{
    int voice;
    if (start == 1) {
        SsdVagStrWork.flags |= SSD_VAG_FLAG_TRANSFERRING | SSD_VAG_FLAG_START_PENDING;
        SsdVagStrWork.buffer_index = 0;
        sceSdSetCoreAttr(SD_C_IRQ_ENABLE | SD_CORE_1, 0);
        SsdSetIrqCallback(SsdVagStreamIrqCallback);
        SsdSetPcmStreamCallback(SsdVagStreamDmaCallback);
        for (voice = 0; voice < SsdVagStrWork.channel_count; voice++)
            SsdInitVagStreamVoice(voice);
        SsdVagStrWork.dma_channel_index = 0;
        SsdVagStreamDmaTransfer(1);
    } else {
        SsdVagStrWork.flags &= ~(SSD_VAG_FLAG_TRANSFERRING | SSD_VAG_FLAG_START_PENDING);
        SsdSetPcmStreamCallback(0);
        for (voice = 0; voice < SsdVagStrWork.channel_count; voice++)
            SsdCutoffUnlinkVoice(SsdVagStrWork.channels[voice].voice);
        sceSdSetCoreAttr(SD_C_IRQ_ENABLE | SD_CORE_1, 0);
    }
}

void SsdVagStreamSpuTransfer(void)
{
    int completedIndex;
    int nextIndex;

    sceSdSetCoreAttr(SD_C_IRQ_ENABLE | SD_CORE_1, 0);
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
        if (work->vag_callback != 0) {
            work->vag_callback(SsdVagStrWork.dma_channel, work->vag_argument);
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
        do {
            int stride = SsdVagStrWork.transfer_stride;
            int block_byte_offset = stride * block;
            unsigned char *block_source = SsdVagStrWork.transfer_source;
            int channel = SsdVagStrWork.dma_channel_index;
            int channel_offset = channel * SSD_VAG_CHUNK_BYTES;
            buffer_index = SsdVagStrWork.buffer_index;
            block_source += block_byte_offset;
            spu_start = channel * SSD_VAG_VOICE_STRIDE + SSD_SPU_STREAM_BASE;
            packet = (SsdVagPacket *)&block_source[channel_offset];
        } while (0);
        destination = buffer_index * SSD_VAG_CHUNK_BYTES + spu_start;
    }
    if (first)
        packet->flags = SSD_VAG_PACKET_LOOP | SSD_VAG_PACKET_LOOP_START;
    last_packet = &packet[SSD_VAG_PACKETS_PER_CHUNK - 1];
    if (buffer_index)
        last_packet->flags = SSD_VAG_PACKET_LOOP | SSD_VAG_PACKET_LOOP_END;
    else
        last_packet->flags = SSD_VAG_PACKET_LOOP;
    status = sceSdVoiceTransStatus(SsdVagStrWork.dma_channel, 0);
    if (status < 0)
        Kprintf("sceSdVoiceTransStatus Error=%d", status);
    status = sceSdVoiceTrans(SsdVagStrWork.dma_channel, SD_TRANS_MODE_WRITE, packet, destination,
                             SSD_VAG_CHUNK_BYTES);
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
    SsdCalcDirectVoiceVolume(&voice->left_volume,
        (stream->controls[0].current.value >> 16) * (SsdVagStrWork.master_volume.current.value >> 16) / SSD_MIDI_VALUE_MAX,
        stream->controls[1].current.value >> 16);
    voice->flags = 0;
    voice->effects = stream->effect;
    voice->attributes = stream->flags;
    voice->adsr1 = SSD_VOICE_ADSR1_RAW;
    voice->reserved_volume[0] = 0;
    voice->reserved_volume[1] = 0;
    voice->modulation = 0;
    voice->sample_start = SSD_SPU_STREAM_BASE + channel * SSD_VAG_VOICE_STRIDE;
    voice->adsr2 = SSD_VOICE_ADSR2_RAW;
}

void SsdSetVagStreamPitch(int channel, int pitch)
{
    if (SsdVagStrWork.channel_count < 2) {
        SsdVagChannel *stream = &SsdVagStrWork.channels[channel];
        SsdVoice *voice = stream->voice;

        stream->pitch = pitch;
        voice->pitch = pitch;
        voice->flags |= SSD_VOICE_FLAG_PITCH;
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
        stream->controls[0].remaining = ticks;
        if (ticks == 0) {
            int level;
            SsdVoice *voice;
            stream->controls[0].current.value = fixed_volume;
            level = volume * (SsdVagStrWork.master_volume.current.value >> 16) / SSD_MIDI_VALUE_MAX;
            voice = stream->voice;
            SsdCalcDirectVoiceVolume(&voice->left_volume, level, stream->controls[1].current.value >> 16);
            voice->flags |= SSD_VOICE_FLAG_VOLUME;
        } else {
            stream->controls[0].step = (fixed_volume - stream->controls[0].current.value) / ticks;
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
    fixed_pan = pan << 16;
    do {
        int channel_volume;
        int master_volume;

        stream = &SsdVagStrWork.channels[channel];
        channel_volume = stream->controls[0].current.value >> 16;
        stream->controls[1].current.value = fixed_pan;
        master_volume = SsdVagStrWork.master_volume.current.value >> 16;
        level = channel_volume * master_volume / SSD_MIDI_VALUE_MAX;
        voice = stream->voice;
        SsdCalcDirectVoiceVolume(&voice->left_volume, level, pan);
        voice->flags |= SSD_VOICE_FLAG_VOLUME;
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
    SsdCalcDirectVoiceVolume(&voice->left_volume,
        volume * (SsdVagStrWork.master_volume.current.value >> 16) / SSD_MIDI_VALUE_MAX, pan);
    voice->flags |= SSD_VOICE_FLAG_PITCH | SSD_VOICE_FLAG_VOLUME;
}

void SsdSetVagStreamHWEffect(int channel, int control, int dirty)
{
    SsdVagChannel *stream = &SsdVagStrWork.channels[channel];
    SsdVoice *voice = stream->voice;

    voice->effects = stream->effect;
    voice->flags |= dirty;
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
                level = (stream->controls[0].current.value >> 16) * volume / SSD_MIDI_VALUE_MAX;
                voice = stream->voice;
                SsdCalcDirectVoiceVolume(&voice->left_volume,
                    level,
                    stream->controls[1].current.value >> 16);
                voice->flags |= SSD_VOICE_FLAG_VOLUME;
                stream++;
                channel++;
            } while (channel < SsdVagStrWork.channel_count);
        }
    } else {
        SsdVagStrWork.master_volume.step =
            (fixed_volume - SsdVagStrWork.master_volume.current.value) / ticks;
    }
    SsdVagStrWork.master_volume.remaining = ticks;
}

void SsdVagStreamVolumeFadeProc(SsdGlobalWork *global_work, void *context)
{
    SsdVagWork *work = context;
    unsigned char changed[4];
    int channel;
    if (work->status == SSD_STREAM_PLAYING) {
    if (work->master_volume.remaining != 0) {
        SsdCalculateModulate(&work->master_volume);
        if (work->master_volume.current.value == 0) {
            SsdVagStreamBlockTrans(work->dma_channel, 0, 0, 0);
            work->status = SSD_STREAM_STOPPED;
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
            if (fading_channel->controls[0].remaining != 0) {
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
                SsdCalcDirectVoiceVolume(&voice->left_volume,
                    (stream->controls[0].current.value >> 16) *
                        (work->master_volume.current.value >> 16) / SSD_MIDI_VALUE_MAX,
                    stream->controls[1].current.value >> 16);
                voice->flags |= SSD_VOICE_FLAG_VOLUME;
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
    firstVoice = SSD_VAG_FIRST_VOICE;
    if (SsdVagStrWork.channel_count > 0) {
        do {
            SsdVoice *voice = &SsdWork.voices[firstVoice + channelIndex];
            unsigned int streamFlags = SSD_VAG_VOICE_PRIORITY;
            SsdUnlinkChannelVoice(voice);
            SsdVagStrWork.channels[channelIndex].voice = voice;
            voice->allocation.priority = (voice->allocation.priority & 0xffff) | streamFlags;
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
