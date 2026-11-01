/* Original RSSD rssdstrm.c translation unit. */

typedef struct RingBuffer {
    signed char capacity;
    unsigned char queued;
} RingBuffer;

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
    StreamChannel vag[7];
    unsigned char unmodeled_068[4];
} RssdIopStreamWork;

typedef struct StreamRequest {
    unsigned char _unmodeled_header[16];
    int argument[4];
    unsigned char data[0];
} StreamRequest;

RssdIopStreamWork RssdStrWork;

extern RingBuffer *InitRingBuffer(int, int);

extern void SsdInitVagStreamStereo(int, void (*)(RssdIopStreamWork *), RssdIopStreamWork *);

void SsdEEVagStreamCallbackStereo(RssdIopStreamWork *);

extern void DisposeRingBuffer(RingBuffer *);

extern void SsdDisposeVagStream(void);

extern int SsdGetVagStreamParam(int, int *, int *, int *);

extern void ClearRingBuffer(RingBuffer *);

void SsdEEClearRingBuffer(int);

extern void SsdInitPcmStream(int, void (*)(StreamChannel *), StreamChannel *);

void SsdEEPcmStreamCallback(StreamChannel *);

extern void SsdDisposePcmStream(void);

extern int SsdGetVagStreamStatus(void);

extern int InRingBuffer(RingBuffer *, const void *, int);

extern void SsdPlayVagStream(int, int, int);

extern void SsdStopVagStream(int);

extern void *OutRingBuffer(RingBuffer *, int *);

extern void SsdSetVagStreamDataMono(int, void *, int);

extern int iRssdCallFunc(int, int *, void *, int);

void SsdEEVagStreamCallbackMono(RssdIopStreamWork *);

extern int SsdGetPcmStreamStatus(void);

extern void SsdInitVagStreamMono(int, int, void (*)(RssdIopStreamWork *), RssdIopStreamWork *);

extern void SsdSetVagStreamDataStereo(void *, int);

extern void SsdSetPcmStreamData(void *);

extern void *GetRingInBufferPtr(RingBuffer *);

extern void IncRingInBufferPtr(RingBuffer *, int);

extern void SsdMemoryCopy(void *, const void *, int);

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
        work->mode = 1;
        work->chunk_size = chunk_size;
        SsdInitPcmStream(chunk_size, SsdEEPcmStreamCallback, &work->pcm);
    }
}

void SsdDisposeEEPcmStream(void)
{
    RssdIopStreamWork *work = &RssdStrWork;
    if (work->initialized) {
        SsdDisposePcmStream();
        DisposeRingBuffer(work->pcm.ring);
        work->initialized = 0;
        work->mode = 0;
    }
}

int SsdSetEEPcmStreamData(StreamRequest *request)
{
    unsigned char *data = request->data;
    int left = request->argument[1];
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
        RssdStrWork.pcm.drain_count = 2;
    }
    return count;
}

int SsdGetEEPcmStreamStatus(void)
{
    if (!RssdStrWork.initialized) {
        return -1;
    }
    return SsdGetPcmStreamStatus() |
        ((RssdStrWork.pcm.ring->capacity -
          RssdStrWork.pcm.ring->queued) << 8);
}

void SsdEEPcmStreamCallback(StreamChannel *channel)
{
    int request[8];
    int size;
    int drain_count;
    void *data = OutRingBuffer(channel->ring, &size);
    SsdSetPcmStreamData(data);
    drain_count = channel->drain_count;
    if (!data && drain_count) {
        int remaining = drain_count - 1;
        channel->drain_count = remaining;
        if (!remaining) {
            request[4] = 1;
            iRssdCallFunc(10, request, 0, 0);
            channel->active = 0;
        }
    }
}

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
        work->mode = 3;
        work->chunk_size = chunk_size;
        SsdInitVagStreamStereo(chunk_size, SsdEEVagStreamCallbackStereo, work);
    }
}

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
        RssdStrWork.mode = 2;
        RssdStrWork.chunk_size = chunk_size;
        SsdInitVagStreamMono(chunk_size, channels, SsdEEVagStreamCallbackMono, &RssdStrWork);
        for (i = 0; i < channels; i++) {
        }
    }
}

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
        RssdStrWork.mode = 0;
    }
}

void SsdPlayEEVagStream(StreamRequest *request)
{
    int i;
    StreamChannel *channel;
    SsdPlayVagStream(request->argument[0], request->argument[1], request->argument[2]);
    channel = RssdStrWork.vag;
    for (i = 0; i < RssdStrWork.channel_count; i++) {
        channel->active = 0;
        channel->drain_count = 0;
        channel++;
    }
}

void SsdStopEEVagStream(StreamRequest *request)
{
    SsdStopVagStream(request->argument[0]);
    if (!request->argument[0])
        SsdEEClearRingBuffer(-1);
}

int SsdSetEEVagStreamDataStereo(const StreamRequest *request)
{
    int channel_index = request->argument[0];
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
    if (mode == 3) {
        StreamChannel *channel;
        count = 0;
        left = request->argument[2];
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
            channel->drain_count = 2;
        }
    } else {
        int active = 1;
        int drain_count = 2;
        StreamChannel *channels;
        pair_index = 0;
        offset = sizeof(StreamRequest);
        channels = RssdStrWork.vag;
        channels += channel_index;
        do {
            StreamChannel *channel = &channels[pair_index];
            count = 0;
            left = request->argument[2] >> 1;
            data = (const unsigned char *)request + offset;
            while (left > 0) {
                length = chunk_size;
                if (left < length)
                    length = left;
                output = GetRingInBufferPtr(channel->ring);
                if (!output)
                    break;
                for (copied = length; copied > 0;) {
                    SsdMemoryCopy(output, data, 1024);
                    copied -= 1024;
                    output += 1024;
                    data += 2048;
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
            offset += 1024;
        } while (pair_index < 2);
    }
    return 0;
}

int SsdGetEEVagStreamStatusStereo(StreamRequest *request)
{
    StreamChannel *channel;
    int status;
    RingBuffer *ring;
    if (!RssdStrWork.initialized)
        return -1;
    channel = RssdStrWork.vag;
    status = SsdGetVagStreamStatus();
    ring = channel->ring;
    status |= (ring->capacity - ring->queued) << 8;
    if (channel->active)
        status |= 2;
    request->argument[0] = status;
    return status;
}

void SsdEEVagStreamCallbackStereo(RssdIopStreamWork *work)
{
    int request[8];
    int size;
    unsigned char drain_count;
    void *data = OutRingBuffer(work->vag[0].ring, &size);
    SsdSetVagStreamDataStereo(data, size);
    drain_count = work->vag[0].drain_count;
    if (!data && drain_count) {
        work->vag[0].drain_count = --drain_count;
        if (!drain_count) {
            request[4] = 1;
            iRssdCallFunc(10, request, 0, 0);
            work->vag[0].active = 0;
        }
    }
}

int SsdSetEEVagStreamDataMono(StreamRequest *request)
{
    unsigned char *data = request->data;
    int count = 0;
    int left = request->argument[2];
    StreamChannel *channel = &RssdStrWork.vag[request->argument[0]];
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
        channel->drain_count = 2;
    }
    return 0;
}

int SsdGetEEVagStreamStatusMono(StreamRequest *request)
{
    int volume;
    int pan;
    int pitch;
    int channel;
    int status;
    RingBuffer *ring;
    if (!RssdStrWork.initialized)
        return -1;
    channel = request->argument[0];
    status = SsdGetVagStreamParam(channel, &volume, &pan, &pitch);
    if (RssdStrWork.vag[channel].active)
        status |= 2;
    request->argument[1] = volume;
    request->argument[2] = pan;
    request->argument[3] = pitch;
    ring = RssdStrWork.vag[channel].ring;
    return status | ((ring->capacity - ring->queued) << 8);
}

int SsdGetEEVagStreamStatusAll(StreamRequest *request)
{
    int i;
    if (!RssdStrWork.initialized)
        return -1;
    for (i = 0; i < RssdStrWork.channel_count; i++) {
        RingBuffer *ring = RssdStrWork.vag[i].ring;
        int status = SsdGetVagStreamStatus();
        if (RssdStrWork.vag[i].active)
            status |= 2;
        status |= (ring->capacity - ring->queued) << 8;
        request->argument[i] = status;
    }
    return 0;
}

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

void SsdEEVagStreamCallbackMono(RssdIopStreamWork *argument)
{
    int request[8];
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
        request[4] = finished;
        iRssdCallFunc(10, request, 0, 0);
    }
}
