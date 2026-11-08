#include "rssd/rssd.h"
#include "rssd/rssdring.h"
#include "rssd/rssdstrm.h"
#include "ssd/ssd_mem.h"
#include "ssd/ssd_pcm.h"
#include "ssd/ssd_vag.h"

/* RssdIopStreamWork.mode */
#define RSSD_STREAM_MODE_NONE 0
#define RSSD_STREAM_MODE_PCM 1
#define RSSD_STREAM_MODE_VAG_MONO 2
#define RSSD_STREAM_MODE_VAG_STEREO 3

/* Number of VAG voices (channels) the EE stream can drive. */
#define RSSD_VAG_CHANNEL_MAX 7

/* Empty polls after the last queued block before a channel reports "finished". */
#define RSSD_STREAM_DRAIN_COUNT 2

/* Stream status word: bit 1 = channel active, bits 8 and up = free ring slots. */
#define RSSD_STREAM_STATUS_ACTIVE 2
#define RSSD_STREAM_STATUS_FREE_SHIFT 8

/* Stereo VAG data arrives as interleaved 1024-byte blocks, left then right. */
#define RSSD_VAG_INTERLEAVE_SIZE 1024

typedef struct StreamChannel {
    short format;
    unsigned char active;
    unsigned char drain_count;
    int chunk_size;
    RingBuffer *ring;
} StreamChannel;

typedef struct RssdIopStreamWork {
    short initialized;
    unsigned char mode;
    unsigned char channel_count;
    int chunk_size;
    StreamChannel pcm;
    StreamChannel vag[RSSD_VAG_CHANNEL_MAX];
    unsigned char unmodeled_068[4];
} RssdIopStreamWork;

/* SsdSetVagStreamDataStereo is called with the block size too, which the SSD
 * definition in src/audio/ssd does not declare. */
extern void SsdSetVagStreamDataStereo(void *, int);

void SsdEEVagStreamCallbackStereo(void *argument);
void SsdEEVagStreamCallbackMono(void *argument);
void SsdEEPcmStreamCallback(void *argument);

RssdIopStreamWork RssdStrWork;

/* Creates the PCM ring buffer and starts the IOP-side PCM stream, once. */
void SsdInitEEPcmStream(int chunk_size, int format)
{
    RssdIopStreamWork *work = &RssdStrWork;
    if (!work->initialized) {
        work->pcm.ring = InitRingBuffer(chunk_size, format);
        work->pcm.chunk_size = chunk_size;
        work->pcm.format = format;
        work->pcm.active = 0;
        work->pcm.drain_count = 0;
        work->initialized = 1;
        work->mode = RSSD_STREAM_MODE_PCM;
        work->chunk_size = chunk_size;
        SsdInitPcmStream(chunk_size, SsdEEPcmStreamCallback, &work->pcm);
    }
}

/* Stops the PCM stream and frees its ring buffer. */
void SsdDisposeEEPcmStream(void)
{
    RssdIopStreamWork *work = &RssdStrWork;
    if (work->initialized) {
        SsdDisposePcmStream();
        DisposeRingBuffer(work->pcm.ring);
        work->initialized = 0;
        work->mode = RSSD_STREAM_MODE_NONE;
    }
}

/* Queues as many whole chunks of the request data as the PCM ring accepts, and returns their count. */
int SsdSetEEPcmStreamData(RssdRpcPacket *request)
{
    unsigned char *data = request->data;
    int left = request->arguments[1];
    int chunk_size = RssdStrWork.pcm.chunk_size;
    int count = 0;
    while (left > 0) {
        if (!InRingBuffer(RssdStrWork.pcm.ring, data, chunk_size))
            break;
        left -= chunk_size;
        data += chunk_size;
        count++;
    }
    if (count) {
        RssdStrWork.pcm.active = 1;
        RssdStrWork.pcm.drain_count = RSSD_STREAM_DRAIN_COUNT;
    }
    return count;
}

/* Returns the PCM stream status with the free ring slots from bit 8 up; -1 when not initialized. */
int SsdGetEEPcmStreamStatus(void)
{
    if (!RssdStrWork.initialized) {
        return -1;
    }
    return SsdGetPcmStreamStatus() |
        ((RssdStrWork.pcm.ring->capacity -
          RssdStrWork.pcm.ring->count) << RSSD_STREAM_STATUS_FREE_SHIFT);
}

/*
 * Feeds the next PCM block to the IOP stream. After the drain count of empty polls,
 * it reports the stream as finished.
 */
void SsdEEPcmStreamCallback(void *argument)
{
    StreamChannel *channel = argument;
    RssdRpcPacket request;
    int size;
    int drain_count;
    void *data = OutRingBuffer(channel->ring, &size);
    SsdSetPcmStreamData(data);
    drain_count = channel->drain_count;
    if (!data && drain_count) {
        int remaining = drain_count - 1;
        channel->drain_count = remaining;
        if (!remaining) {
            request.arguments[0] = 1;
            iRssdCallFunc(RSSD_EE_STREAM_FINISHED, &request, 0, 0);
            channel->active = 0;
        }
    }
}

/* Creates the ring buffer of channel 0 and starts the stereo VAG stream, once. */
void SsdInitEEVagStreamStereo(int chunk_size, int format)
{
    RssdIopStreamWork *work = &RssdStrWork;
    if (!work->initialized) {
        StreamChannel *channel = &work->vag[0];
        channel->ring = InitRingBuffer(chunk_size, format);
        channel->chunk_size = chunk_size;
        channel->format = format;
        channel->active = 0;
        channel->drain_count = 0;
        work->initialized = 1;
        work->channel_count = 1;
        work->mode = RSSD_STREAM_MODE_VAG_STEREO;
        work->chunk_size = chunk_size;
        SsdInitVagStreamStereo(chunk_size, SsdEEVagStreamCallbackStereo, work);
    }
}

/* Creates one ring buffer per channel and starts the mono VAG stream, once. */
void SsdInitEEVagStreamMono(int chunk_size, int channels, int format)
{
    int i;
    if (!RssdStrWork.initialized) {
        RssdStrWork.channel_count = channels;
        for (i = 0; i < channels; i++) {
            RssdStrWork.vag[i].ring = InitRingBuffer(chunk_size, format);
            RssdStrWork.vag[i].chunk_size = chunk_size;
            RssdStrWork.vag[i].format = format;
            RssdStrWork.vag[i].active = 0;
            RssdStrWork.vag[i].drain_count = 0;
        }
        RssdStrWork.initialized = 1;
        RssdStrWork.mode = RSSD_STREAM_MODE_VAG_MONO;
        RssdStrWork.chunk_size = chunk_size;
        SsdInitVagStreamMono(chunk_size, channels, SsdEEVagStreamCallbackMono, &RssdStrWork);
        for (i = 0; i < channels; i++) {
        }
    }
}

/* Stops the VAG stream and frees the ring buffers of its channels. */
void SsdDisposeEEVagStream(void)
{
    int i;
    StreamChannel *channel = RssdStrWork.vag;
    if (RssdStrWork.initialized) {
        SsdDisposeVagStream();
        for (i = 0; i < RssdStrWork.channel_count; i++) {
            DisposeRingBuffer(channel->ring);
            channel++;
        }
        RssdStrWork.initialized = 0;
        RssdStrWork.mode = RSSD_STREAM_MODE_NONE;
    }
}

/* Starts a VAG stream from the request arguments and clears the state of each channel. */
void SsdPlayEEVagStream(RssdRpcPacket *request)
{
    int i;
    StreamChannel *channel;
    SsdPlayVagStream(request->arguments[0], request->arguments[1], request->arguments[2]);
    channel = RssdStrWork.vag;
    for (i = 0; i < RssdStrWork.channel_count; i++) {
        channel->active = 0;
        channel->drain_count = 0;
        channel++;
    }
}

/* Stops the VAG channel named by argument 0. Channel 0 also clears every ring buffer. */
void SsdStopEEVagStream(RssdRpcPacket *request)
{
    SsdStopVagStream(request->arguments[0]);
    if (!request->arguments[0])
        SsdEEClearRingBuffer(-1);
}

/*
 * Queues stereo VAG data. In stereo mode the data goes to channel 0. Otherwise the
 * interleaved data is split into two mono channels starting at channel_index.
 */
int SsdSetEEVagStreamDataStereo(const RssdRpcPacket *request)
{
    int channel_index = request->arguments[0];
    const int *chunk_size_field = &RssdStrWork.chunk_size;
    unsigned int mode = RssdStrWork.mode;
    int chunk_size = *chunk_size_field;
    int pair_index;
    unsigned int offset;
    int count;
    int left;
    int length;
    int copied;
    const unsigned char *data;
    unsigned char *output;
    if (mode == RSSD_STREAM_MODE_VAG_STEREO) {
        StreamChannel *channel;
        count = 0;
        left = request->arguments[2];
        data = request->data;
        channel = RssdStrWork.vag;
        for (; left > 0; count++) {
            if (!InRingBuffer(channel->ring, data, chunk_size))
                break;
            left -= chunk_size;
            data += chunk_size;
        }
        if (count) {
            channel->active = 1;
            channel->drain_count = RSSD_STREAM_DRAIN_COUNT;
        }
    } else {
        int active = 1;
        int drain_count = RSSD_STREAM_DRAIN_COUNT;
        StreamChannel *channels;
        pair_index = 0;
        offset = sizeof(RssdRpcPacket);
        channels = RssdStrWork.vag;
        channels += channel_index;
        /* Each pass copies one channel's 1024-byte blocks out of the interleaved data. */
        do {
            StreamChannel *channel = &channels[pair_index];
            count = 0;
            left = request->arguments[2] >> 1;
            data = (const unsigned char *)request + offset;
            while (left > 0) {
                length = chunk_size;
                if (left < length)
                    length = left;
                output = GetRingInBufferPtr(channel->ring);
                if (!output)
                    break;
                for (copied = length; copied > 0;) {
                    SsdMemoryCopy(output, data, RSSD_VAG_INTERLEAVE_SIZE);
                    copied -= RSSD_VAG_INTERLEAVE_SIZE;
                    output += RSSD_VAG_INTERLEAVE_SIZE;
                    data += 2 * RSSD_VAG_INTERLEAVE_SIZE;
                }
                IncRingInBufferPtr(channel->ring, left);
                left -= length;
                count++;
            }
            if (count) {
                channel->active = active;
                channel->drain_count = drain_count;
            }
            pair_index++;
            offset += RSSD_VAG_INTERLEAVE_SIZE;
        } while (pair_index < 2);
    }
    return 0;
}

/* Returns the status of the stereo stream (channel 0), stores it in argument 0; -1 when not initialized. */
int SsdGetEEVagStreamStatusStereo(RssdRpcPacket *request)
{
    StreamChannel *channel;
    int status;
    RingBuffer *ring;
    if (!RssdStrWork.initialized)
        return -1;
    channel = RssdStrWork.vag;
    status = SsdGetVagStreamStatus();
    ring = channel->ring;
    status |= (ring->capacity - ring->count) << RSSD_STREAM_STATUS_FREE_SHIFT;
    if (channel->active)
        status |= RSSD_STREAM_STATUS_ACTIVE;
    request->arguments[0] = status;
    return status;
}

/*
 * Feeds the next stereo block to the IOP stream. After the drain count of empty polls,
 * it reports the stream as finished.
 */
void SsdEEVagStreamCallbackStereo(void *argument)
{
    RssdIopStreamWork *work = argument;
    RssdRpcPacket request;
    int size;
    unsigned char drain_count;
    void *data = OutRingBuffer(work->vag[0].ring, &size);
    SsdSetVagStreamDataStereo(data, size);
    drain_count = work->vag[0].drain_count;
    if (!data && drain_count) {
        work->vag[0].drain_count = --drain_count;
        if (!drain_count) {
            request.arguments[0] = 1;
            iRssdCallFunc(RSSD_EE_STREAM_FINISHED, &request, 0, 0);
            work->vag[0].active = 0;
        }
    }
}

/* Queues as many chunks of mono data as the ring of the channel named by argument 0 accepts. Returns 0. */
int SsdSetEEVagStreamDataMono(RssdRpcPacket *request)
{
    unsigned char *data = request->data;
    int count = 0;
    int left = request->arguments[2];
    StreamChannel *channel = &RssdStrWork.vag[request->arguments[0]];
    while (left > 0) {
        int length = RssdStrWork.chunk_size;
        if (left < length)
            length = left;
        if (!InRingBuffer(channel->ring, data, length))
            break;
        left -= length;
        data += length;
        count++;
    }
    if (count) {
        channel->active = 1;
        channel->drain_count = RSSD_STREAM_DRAIN_COUNT;
    }
    return 0;
}

/* Returns the status of the channel named by argument 0, and stores its volume, pan and pitch in arguments 1 to 3. */
int SsdGetEEVagStreamStatusMono(RssdRpcPacket *request)
{
    int volume;
    int pan;
    int pitch;
    int channel;
    int status;
    RingBuffer *ring;
    if (!RssdStrWork.initialized)
        return -1;
    channel = request->arguments[0];
    status = SsdGetVagStreamParam(channel, &volume, &pan, &pitch);
    if (RssdStrWork.vag[channel].active)
        status |= RSSD_STREAM_STATUS_ACTIVE;
    request->arguments[1] = volume;
    request->arguments[2] = pan;
    request->arguments[3] = pitch;
    ring = RssdStrWork.vag[channel].ring;
    return status | ((ring->capacity - ring->count) << RSSD_STREAM_STATUS_FREE_SHIFT);
}

/* Stores the status of every channel in the request arguments and returns 0; -1 when not initialized. */
int SsdGetEEVagStreamStatusAll(RssdRpcPacket *request)
{
    int i;
    if (!RssdStrWork.initialized)
        return -1;
    for (i = 0; i < RssdStrWork.channel_count; i++) {
        RingBuffer *ring = RssdStrWork.vag[i].ring;
        int status = SsdGetVagStreamStatus();
        if (RssdStrWork.vag[i].active)
            status |= RSSD_STREAM_STATUS_ACTIVE;
        status |= (ring->capacity - ring->count) << RSSD_STREAM_STATUS_FREE_SHIFT;
        request->arguments[i] = status;
    }
    return 0;
}

/* Clears the ring buffer and state of one channel, or of all channels when index is negative. */
void SsdEEClearRingBuffer(int index)
{
    int channels;
    StreamChannel *channel;
    if (index >= 0) {
        channel = &RssdStrWork.vag[index];
        channels = 1;
    } else {
        channel = RssdStrWork.vag;
        channels = RssdStrWork.channel_count;
    }
    for (index = 0; index < channels; index++, channel++) {
        ClearRingBuffer(channel->ring);
        channel->active = 0;
        channel->drain_count = 0;
    }
}

/* Feeds the next block to each mono channel and reports the channels that finished as a bit mask. */
void SsdEEVagStreamCallbackMono(void *argument)
{
    RssdRpcPacket request;
    int size;
    int i = 0;
    RssdIopStreamWork *work = argument;
    int finished = 0;
    StreamChannel *channel = work->vag;
    for (; i < work->channel_count; i++) {
        unsigned char drain_count;
        void *data = OutRingBuffer(channel->ring, &size);
        SsdSetVagStreamDataMono(i, data, size);
        drain_count = channel->drain_count;
        if (!data && drain_count) {
            channel->drain_count = --drain_count;
            if (!drain_count) {
                finished |= 1 << i;
                channel->active = 0;
            }
        }
        channel++;
    }
    if (finished) {
        request.arguments[0] = finished;
        iRssdCallFunc(RSSD_EE_STREAM_FINISHED, &request, 0, 0);
    }
}
