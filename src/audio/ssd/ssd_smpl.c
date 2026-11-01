/* Original SSD object: ssd_smpl.c. Partial layouts describe evidenced fields. */
typedef struct SsdVoiceControl {
    unsigned char index, core, voice, state;
    unsigned short updates;
    unsigned char effect, attributes;
    short output_volume[4];
    unsigned short unknown16, unknown18;
    unsigned short pitch, release;
    /* Allocation uses the full word; keyoff observes its low activity byte. */
    union {
        unsigned int priority;
        struct {
            unsigned char active;
            unsigned char unmodeled_019[3];
        } parts;
    } allocation;
    unsigned int spu_address, loop_address;
    unsigned short adsr1, adsr2;
    unsigned char unmodeled_028[216];
} SsdVoiceControl;

typedef struct {
    SsdVoiceControl *voice;
    unsigned int unknown04;
    unsigned char state, status;
    unsigned short pitch;
    int volume[4], pan[4];
    unsigned char effect, attributes;
    unsigned short unknown46;
} SamplingChannel;

typedef struct {
    unsigned short flags;
    unsigned short unknown02;
    unsigned int unknown04, unknown08, unknown12;
} SamplingProcess;

typedef struct {
    unsigned char voice_count, unknown01;
    unsigned short pitch, volume, pan;
    unsigned char effect, attributes;
    unsigned short unknown10;
    SamplingChannel channels[4];
    unsigned int spu_address, buffer_size, next_address;
    void (*dma_callback)();
    void (*keyoff_callback)();
    void *buffer;
    SamplingProcess process;
} SamplingWork;

SamplingWork SsdSampleWork;
extern void SsdCalcDirectVoiceVolume(short *, int, int);
extern void SsdUnlinkProc(SamplingProcess *);
extern void CpuDisableIntr(void);
extern void CpuEnableIntr(void);
void SsdSampleKeyoffInterProc(void);
extern void SsdLinkProc(SamplingProcess *, void (*)(void), SamplingWork *);
extern void SsdKeyonVoice(SsdVoiceControl *);
extern void SsdMemoryCopy(void *, const void *, unsigned int);
extern void SsdSpuDmaWrite(unsigned int, const void *, unsigned int,
                           void (*)(), SamplingWork *);
extern unsigned int SsdSpuMallocAdrsDynamic(unsigned int, unsigned int, int);
extern void SsdCutoffUnlinkVoice(SsdVoiceControl *);
extern unsigned int SsdSpuMFree(unsigned int);
extern int FreeSysMemory(void *);
extern void *AllocSysMemory(int, unsigned int, void *);
extern int printf(const char *, ...);
void SsdAllocSamplingVoice(int count);
void SsdInitSamplingVoice(void);
extern struct {
    unsigned char unmodeled_000[0x230];
    unsigned int key_on[2], key_off[2], cutoff[2], noise[2];
    unsigned int pitch_modulation[2], effect_left[2], effect_right[2];
    unsigned int active_voices;
    SsdVoiceControl voices[48];
} SsdWork;
extern void SsdUnlinkChannelVoice(SsdVoiceControl *);
void SsdSetSamplingEffect(int channel, int reverb, int noise);
void SsdSetSamplingParam(int channel, int pitch, int volume, int pan);
void SsdPlaySampling(int channel, int pitch, int volume, int pan);
void SsdTransferSampling(int channel, const void *data, unsigned int size);
void SsdTransferSamplingNext(const void *data, unsigned int size);
void SsdStopSampling(int channel);
void SsdDisposeSampling(void);
void SsdSetSampleDmaCallback(void (*callback)());
void SsdSetSampleKeyoffCallback(void (*callback)());
void SsdInitSampling(int voice_count, unsigned int spu_address,
                     unsigned int buffer_size);

void SsdInitSampling(int voice_count, unsigned int spu_address,
                     unsigned int buffer_size)
{
    if (voice_count < 5) {
        SsdSampleWork.pitch = 4096;
        SsdSampleWork.volume = 127;
        SsdSampleWork.pan = 64;
        SsdSampleWork.effect = 12;
        SsdSampleWork.attributes = 0;
        SsdSampleWork.spu_address = spu_address;
        SsdSampleWork.buffer_size = buffer_size;
        SsdSampleWork.dma_callback = 0;
        SsdSampleWork.keyoff_callback = 0;
        CpuDisableIntr();
        SsdSampleWork.buffer = AllocSysMemory(0, buffer_size, 0);
        CpuEnableIntr();
        if (SsdSampleWork.buffer == 0) {
            printf("Ssd lock of sampling memory\n");
            return;
        }
        SsdAllocSamplingVoice(voice_count);
        SsdInitSamplingVoice();
    }
}

void SsdDisposeSampling(void)
{
    int channel;
    SamplingChannel *sample;
    SsdVoiceControl *voice;
    for (channel = 0; channel < SsdSampleWork.voice_count; channel++) {
        sample = &SsdSampleWork.channels[channel];
        voice = sample->voice;
        SsdCutoffUnlinkVoice(voice);
        SsdSpuMFree(voice->spu_address);
    }
    CpuDisableIntr();
    FreeSysMemory(SsdSampleWork.buffer);
    CpuEnableIntr();
}

void SsdSetSampleDmaCallback(void (*callback)())
{
    SsdSampleWork.dma_callback = callback;
}

void SsdSetSampleKeyoffCallback(void (*callback)())
{
    SsdSampleWork.keyoff_callback = callback;
}

void SsdTransferSampling(int channel, const void *data, unsigned int size)
{
    unsigned int address;
    SamplingChannel *sample;
    SsdMemoryCopy(SsdSampleWork.buffer, data, size);
    sample = &SsdSampleWork.channels[channel];
    address = sample->voice->spu_address;
    SsdSpuDmaWrite(address, SsdSampleWork.buffer, size,
                   SsdSampleWork.dma_callback, &SsdSampleWork);
    SsdSampleWork.next_address = address + size;
}

void SsdTransferSamplingNext(const void *data, unsigned int size)
{
    unsigned int address = SsdSampleWork.next_address;
    SsdSpuDmaWrite(address, data, size, SsdSampleWork.dma_callback,
                   &SsdSampleWork);
    SsdSampleWork.next_address = address + size;
}

void SsdPlaySampling(int channel, int pitch, int volume, int pan)
{
    SamplingChannel *sample;
    SsdVoiceControl *voice;
    CpuDisableIntr();
    SsdLinkProc(&SsdSampleWork.process, SsdSampleKeyoffInterProc,
                &SsdSampleWork);
    CpuEnableIntr();
    sample = &SsdSampleWork.channels[channel];
    voice = sample->voice;
    sample->state = 0;
    sample->status = 0;
    sample->volume[0] = volume;
    sample->pan[0] = pan;
    sample->status = 0;
    voice->pitch = pitch;
    SsdCalcDirectVoiceVolume(voice->output_volume, volume, pan);
    CpuDisableIntr();
    voice->updates = 0xffff;
    SsdKeyonVoice(voice);
    CpuEnableIntr();
}

void SsdStopSampling(int channel)
{
    SamplingChannel *sample;
    CpuDisableIntr();
    if (SsdSampleWork.process.flags & 1)
        SsdUnlinkProc(&SsdSampleWork.process);
    CpuEnableIntr();
    sample = &SsdSampleWork.channels[channel];
    sample->state = 0;
    sample->status = 0;
    SsdCutoffUnlinkVoice(sample->voice);
}

void SsdSetSamplingParam(int channel, int pitch, int volume, int pan)
{
    SamplingChannel *sample = &SsdSampleWork.channels[channel];
    SsdVoiceControl *voice = sample->voice;
    sample->volume[0] = volume;
    sample->pan[0] = pan;
    voice->pitch = pitch;
    SsdCalcDirectVoiceVolume(voice->output_volume, volume, pan);
    voice->updates |= 3;
}

void SsdSetSamplingEffect(int channel, int reverb, int noise)
{
    unsigned char effect = 0;
    unsigned short updates = 0;
    SamplingChannel *sample;
    SsdVoiceControl *voice;
    if (reverb) {
        effect = 8;
        updates = 0x8000;
    }
    if (noise) {
        effect |= 4;
        updates = 0x4000;
    }
    sample = &SsdSampleWork.channels[channel];
    voice = sample->voice;
    sample->effect = effect;
    voice->effect = effect;
    voice->updates |= updates;
}

void SsdAllocSamplingVoice(int count)
{
    int channel;
    int first_voice = 43;
    SsdSampleWork.voice_count = count;
    for (channel = 0; channel < count; channel++) {
        SsdVoiceControl *voices = SsdWork.voices;
        SsdVoiceControl *voice = &voices[first_voice + channel];
        SamplingChannel *sample = &SsdSampleWork.channels[channel];
        SsdUnlinkChannelVoice(voice);
        voice->allocation.priority = (unsigned short)voice->allocation.priority | 0x90000;
        sample->voice = voice;
    }
}

void SsdInitSamplingVoice(void)
{
    if (SsdSampleWork.voice_count) {
    int channel = 0;
    short pitch = 4096;
    int volume = 127;
    int pan = 64;
    unsigned char effect = 12;
    int adsr1 = 0x20ff;
    do {
        SamplingChannel *sample = &SsdSampleWork.channels[channel];
        SsdVoiceControl *voice = sample->voice;
        sample->pitch = pitch;
        sample->state = 0;
        sample->status = 0;
        sample->volume[0] = volume;
        sample->pan[0] = pan;
        sample->effect = effect;
        sample->attributes = 0;
        voice->pitch = sample->pitch;
        SsdCalcDirectVoiceVolume(voice->output_volume, sample->volume[0],
                                 sample->pan[0]);
        voice->updates = 0;
        voice->effect = sample->effect;
        voice->attributes = sample->attributes;
        voice->output_volume[2] = 0;
        voice->output_volume[3] = 0;
        voice->release = 0;
        voice->spu_address = SsdSpuMallocAdrsDynamic(SsdSampleWork.buffer_size,
                                                    SsdSampleWork.spu_address, 10);
        voice->adsr1 = adsr1;
        voice->adsr2 = 0x5fc6;
        channel++;
    } while (channel < SsdSampleWork.voice_count);
    }
}

void SsdSampleKeyoffInterProc(void)
{
    int channel;
    for (channel = 0; channel < SsdSampleWork.voice_count; channel++) {
        SamplingChannel *sample = &SsdSampleWork.channels[channel];
        unsigned char state = sample->state;
        int active = sample->voice->allocation.parts.active;
        if (state & 2) {
            if (active == 0) {
                SsdSampleWork.keyoff_callback(1, channel, 0);
                if (SsdSampleWork.process.flags & 1)
                    SsdUnlinkProc(&SsdSampleWork.process);
                sample->state = 0;
            }
        } else if (active) {
            sample->state = state | 2;
        }
    }
}
