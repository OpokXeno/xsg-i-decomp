#include "ssd/ssd_pcm.h"
#include "ssd/ssd_dma.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_mem.h"

/* AutoDMA stream data is interleaved in 512-byte runs per channel. */
#define SSD_PCM_INTERLEAVE_BYTES 512
#define SSD_PCM_BLOCK_BYTES 1024

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
    SsdRamp volume;
    SsdProc process;
} SsdPcmStreamState;

SsdPcmStreamState SsdPcmStrWork;

void SsdPcmStreamVolumeFadeProc(SsdGlobalWork *work, void *context);
void SsdPcmStreamCallback(int result, void *argument);
void SsdStreamMonoCopyMemory(unsigned char *destination, const void *source_buffer, int bytes);
void SsdStreamClearMemory(void *buffer, int bytes);

/*
 * Sets up the PCM stream with two buffer halves of buffer_bytes each, from one
 * allocation. The fill callback runs on each DMA transfer completion while playing.
 */
void SsdInitPcmStream(int buffer_bytes, void (*callback)(void *), void *context)
{
    SsdPcmStrWork.flags = SSD_STREAM_FLAG_OPEN;
    SsdPcmStrWork.playing = SSD_STREAM_STOPPED;
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

/* Stops playback, detaches the DMA and callback hooks, and frees the buffer. */
void SsdDisposePcmStream(void)
{
    if (SsdPcmStrWork.playing == SSD_STREAM_PLAYING)
        SsdStopPcmStream(0);
    sceSdSetParam(SsdPcmStrWork.dma_channel | SD_P_BVOLL, 0);
    sceSdSetParam(SsdPcmStrWork.dma_channel | SD_P_BVOLR, 0);
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

/* Clears the suspended flag and restarts the looped DMA transfer while playing. */
void SsdResumePcmStream(void)
{
    if (SsdPcmStrWork.playing == SSD_STREAM_PLAYING) {
        SsdPcmStrWork.transfer_busy = 1;
        CpuDisableIntr();
        SsdPcmStrWork.flags &= ~SSD_STREAM_FLAG_SUSPENDED;
        sceSdBlockTrans(SsdPcmStrWork.dma_channel, SD_BLOCK_LOOP,
                        SsdPcmStrWork.buffers[0], SsdPcmStrWork.buffer_bytes * 2);
        CpuEnableIntr();
        SsdPcmStrWork.transfer_busy = 0;
    }
}

/* Stops the DMA transfer and marks the stream suspended while playing. */
void SsdSuspendPcmStream(void)
{
    if (SsdPcmStrWork.playing == SSD_STREAM_PLAYING) {
        SsdPcmStrWork.transfer_busy = 1;
        CpuDisableIntr();
        SsdPcmStrWork.flags |= SSD_STREAM_FLAG_SUSPENDED;
        sceSdBlockTrans(SsdPcmStrWork.dma_channel, SD_TRANS_MODE_STOP, 0, 0);
        CpuEnableIntr();
        SsdPcmStrWork.transfer_busy = 0;
    }
}

/*
 * Starts playback at volume, ramped over duration. When the stream was not
 * playing, its buffer is cleared and the looped DMA transfer is restarted.
 */
void SsdPlayPcmStream(int volume, int duration)
{
    SsdPcmStrWork.volume.remaining = 0;
    if (SsdPcmStrWork.playing != SSD_STREAM_PLAYING) {
        SsdStreamClearMemory(SsdPcmStrWork.buffers[0], SsdPcmStrWork.buffer_bytes * 2);
        SsdPcmStrWork.transfer_busy = 1;
        SsdPcmStrWork.volume.current.value = 0;
        sceSdSetParam(SsdPcmStrWork.dma_channel | SD_P_BVOLL, 0);
        sceSdSetParam(SsdPcmStrWork.dma_channel | SD_P_BVOLR, 0);
        CpuDisableIntr();
        sceSdBlockTrans(SsdPcmStrWork.dma_channel, SD_BLOCK_LOOP,
                        SsdPcmStrWork.buffers[0], SsdPcmStrWork.buffer_bytes * 2);
        CpuEnableIntr();
        SsdPcmStrWork.transfer_busy = 0;
    }
    SsdSetPcmStreamVolume(volume, duration);
    SsdPcmStrWork.playing = SSD_STREAM_PLAYING;
}

/*
 * Ramps the volume to zero over duration. With duration 0 the DMA transfer is
 * also stopped when the stream is playing.
 */
void SsdStopPcmStream(int duration)
{
    SsdSetPcmStreamVolume(0, duration);
    if (!duration) {
        CpuDisableIntr();
        if (SsdPcmStrWork.playing == SSD_STREAM_PLAYING) {
            SsdPcmStrWork.transfer_busy = 1;
            SsdPcmStrWork.playing = SSD_STREAM_STOPPED;
            sceSdBlockTrans(SsdPcmStrWork.dma_channel, SD_TRANS_MODE_STOP, 0, 0);
            SsdPcmStrWork.transfer_busy = 0;
        }
        CpuEnableIntr();
    }
}

int SsdGetPcmStreamStatus(void)
{
    return SsdPcmStrWork.playing;
}

/*
 * Copies source (silence when null) into the buffer half the DMA is not reading.
 * Mono expansion is used when output_mode is 0.
 */
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
        if (!SsdWork.output_mode)
            SsdStreamMonoCopyMemory(destination, source, bytes);
        else
            SsdStreamCopyMemory(destination, source, bytes);
    } else {
        SsdStreamClearMemory(destination, bytes);
    }
}

/* Sets the volume at once when duration is 0, otherwise ramps to it over duration. */
void SsdSetPcmStreamVolume(int volume, int duration)
{
    int target = volume << 16;
    if (!duration) {
        int level;
        SsdPcmStrWork.volume.current.value = target;
        target >>= 16;
        target = target * target * 2;
        level = target;
        sceSdSetParam(SsdPcmStrWork.dma_channel | SD_P_BVOLL, level);
        sceSdSetParam(SsdPcmStrWork.dma_channel | SD_P_BVOLR, level);
    } else {
        int difference = target - SsdPcmStrWork.volume.current.value;
        SsdPcmStrWork.volume.step = difference / duration;
    }
    /* In the immediate path, target holds the squared amplitude when it is stored below. */
    SsdPcmStrWork.volume.target = target;
    SsdPcmStrWork.volume.remaining = duration;
}

/* Per-frame proc: advances the volume ramp and writes its squared level to both channels. */
void SsdPcmStreamVolumeFadeProc(SsdGlobalWork *work, void *context)
{
    SsdPcmStreamState *stream = context;
    int volume;
    int level;

    if (stream->volume.remaining) {
        SsdCalculateModulate(&stream->volume);
        volume = stream->volume.current.value;
        level = volume >> 16;
        level = (level * level) << 1;
        sceSdSetParam(stream->dma_channel | SD_P_BVOLL, level);
        sceSdSetParam(stream->dma_channel | SD_P_BVOLR, level);
        /*
         * Always false: (volume << 16) has no low bits set. The original keeps
         * the raw volume word live in a callee-saved register across both SDK
         * writes with no visible later use; this guard reproduces that lifetime.
         * Combine folds it and the dead store is deleted after register
         * allocation, so it emits no code.
         */
        if ((volume << 16) & 0xffff) {
            stream->volume.current.value = volume;
        }
    }
}

/* DMA completion callback: calls the fill callback while the stream plays and is not suspended. */
void SsdPcmStreamCallback(int result, void *argument)
{
    if (SsdPcmStrWork.flags & SSD_STREAM_FLAG_SUSPENDED)
        return;
    if (SsdPcmStrWork.playing != SSD_STREAM_PLAYING)
        return;
    if (SsdPcmStrWork.fill_callback)
        SsdPcmStrWork.fill_callback(SsdPcmStrWork.callback_context);
}

/* Writes each 1024-byte source block to destination as two copies of its first 512 bytes. */
void SsdStreamMonoCopyMemory(unsigned char *destination, const void *source_buffer, int bytes)
{
    const unsigned char *source = source_buffer;
    int block;
    for (block = 0; block < bytes / SSD_PCM_BLOCK_BYTES; block++) {
        SsdStreamCopyMemory(destination, source, SSD_PCM_INTERLEAVE_BYTES);
        destination += SSD_PCM_INTERLEAVE_BYTES;
        SsdStreamCopyMemory(destination, source, SSD_PCM_INTERLEAVE_BYTES);
        destination += SSD_PCM_INTERLEAVE_BYTES;
        source += SSD_PCM_BLOCK_BYTES;
    }
}

/* Copies bytes from source to destination 16 bytes at a time. */
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

/* Zeroes bytes of buffer 16 bytes at a time. */
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
