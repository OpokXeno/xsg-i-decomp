#include "ssd/ssd_init.h"
#include "ssd/ssd_chan.h"
#include "ssd/ssd_dma.h"
#include "ssd/ssd_mem.h"
#include "ssd/ssd_voic.h"

/* Up to four sampling voices, taken from the end of the voice table (they overlap the VAG stream's). */
#define SSD_SAMPLING_MAX_CHANNELS 4
#define SSD_SAMPLING_FIRST_VOICE 43
#define SSD_SAMPLING_VOICE_PRIORITY 0x90000

typedef void (*SsdSampleKeyoffCallback)(int result, int channel, int argument);

typedef struct SsdSamplingChannel {
    SsdVoice *voice;
    unsigned int unknown04;
    unsigned char state, status;
    unsigned short pitch;
    int volume[4], pan[4];
    unsigned char effect, attributes;
    unsigned short unknown46;
} SsdSamplingChannel;

typedef struct SsdSamplingWork {
    unsigned char voice_count, unknown01;
    unsigned short pitch, volume, pan;
    unsigned char effect, attributes;
    unsigned short unknown10;
    SsdSamplingChannel channels[SSD_SAMPLING_MAX_CHANNELS];
    unsigned int spu_address, buffer_size, next_address;
    SsdDmaCallback dma_callback;
    SsdSampleKeyoffCallback keyoff_callback;
    void *buffer;
    SsdProc process;
} SsdSamplingWork;

SsdSamplingWork SsdSampleWork;

void SsdSampleKeyoffInterProc(SsdGlobalWork *work, void *argument);
void SsdAllocSamplingVoice(int count);
void SsdInitSamplingVoice(void);

void SsdInitSampling(int voice_count, unsigned int spu_address,
                     unsigned int buffer_size)
{
    if (voice_count <= SSD_SAMPLING_MAX_CHANNELS) {
        SsdSampleWork.pitch = SSD_VOICE_PITCH_NORMAL;
        SsdSampleWork.volume = SSD_MIDI_VALUE_MAX;
        SsdSampleWork.pan = SSD_MIDI_PAN_CENTER;
        SsdSampleWork.effect = SSD_VOICE_EFFECT_REVERB | SSD_VOICE_EFFECT_DRY;
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
    SsdSamplingChannel *sample;
    SsdVoice *voice;
    for (channel = 0; channel < SsdSampleWork.voice_count; channel++) {
        sample = &SsdSampleWork.channels[channel];
        voice = sample->voice;
        SsdCutoffUnlinkVoice(voice);
        SsdSpuMFree(voice->sample_start);
    }
    CpuDisableIntr();
    FreeSysMemory(SsdSampleWork.buffer);
    CpuEnableIntr();
}

void SsdSetSampleDmaCallback(SsdDmaCallback callback)
{
    SsdSampleWork.dma_callback = callback;
}

void SsdSetSampleKeyoffCallback(SsdSampleKeyoffCallback callback)
{
    SsdSampleWork.keyoff_callback = callback;
}

void SsdTransferSampling(int channel, const void *data, unsigned int size)
{
    unsigned int address;
    SsdSamplingChannel *sample;
    SsdMemoryCopy(SsdSampleWork.buffer, data, size);
    sample = &SsdSampleWork.channels[channel];
    address = sample->voice->sample_start;
    SsdSpuDmaWrite(address, SsdSampleWork.buffer, size,
                   SsdSampleWork.dma_callback, &SsdSampleWork);
    SsdSampleWork.next_address = address + size;
}

void SsdTransferSamplingNext(void *data, unsigned int size)
{
    unsigned int address = SsdSampleWork.next_address;
    SsdSpuDmaWrite(address, data, size, SsdSampleWork.dma_callback,
                   &SsdSampleWork);
    SsdSampleWork.next_address = address + size;
}

void SsdPlaySampling(int channel, int pitch, int volume, int pan)
{
    SsdSamplingChannel *sample;
    SsdVoice *voice;
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
    SsdCalcDirectVoiceVolume(&voice->left_volume, volume, pan);
    CpuDisableIntr();
    voice->flags = SSD_VOICE_FLAG_ALL;
    SsdKeyonVoice(voice);
    CpuEnableIntr();
}

void SsdStopSampling(int channel)
{
    SsdSamplingChannel *sample;
    CpuDisableIntr();
    if (SsdSampleWork.process.kind & 1)
        SsdUnlinkProc(&SsdSampleWork.process);
    CpuEnableIntr();
    sample = &SsdSampleWork.channels[channel];
    sample->state = 0;
    sample->status = 0;
    SsdCutoffUnlinkVoice(sample->voice);
}

void SsdSetSamplingParam(int channel, int pitch, int volume, int pan)
{
    SsdSamplingChannel *sample = &SsdSampleWork.channels[channel];
    SsdVoice *voice = sample->voice;
    sample->volume[0] = volume;
    sample->pan[0] = pan;
    voice->pitch = pitch;
    SsdCalcDirectVoiceVolume(&voice->left_volume, volume, pan);
    voice->flags |= SSD_VOICE_FLAG_PITCH | SSD_VOICE_FLAG_VOLUME;
}

void SsdSetSamplingEffect(int channel, int dry, int reverb)
{
    unsigned char effect = 0;
    unsigned short updates = 0;
    SsdSamplingChannel *sample;
    SsdVoice *voice;
    if (dry) {
        effect = SSD_VOICE_EFFECT_DRY;
        updates = SSD_VOICE_FLAG_DRY;
    }
    if (reverb) {
        effect |= SSD_VOICE_EFFECT_REVERB;
        updates = SSD_VOICE_FLAG_REVERB;
    }
    sample = &SsdSampleWork.channels[channel];
    voice = sample->voice;
    sample->effect = effect;
    voice->effects = effect;
    voice->flags |= updates;
}

void SsdAllocSamplingVoice(int count)
{
    int channel;
    int first_voice = SSD_SAMPLING_FIRST_VOICE;
    SsdSampleWork.voice_count = count;
    for (channel = 0; channel < count; channel++) {
        SsdVoice *voices = SsdWork.voices;
        SsdVoice *voice = &voices[first_voice + channel];
        SsdSamplingChannel *sample = &SsdSampleWork.channels[channel];
        SsdUnlinkChannelVoice(voice);
        voice->allocation.priority = (unsigned short)voice->allocation.priority | SSD_SAMPLING_VOICE_PRIORITY;
        sample->voice = voice;
    }
}

void SsdInitSamplingVoice(void)
{
    if (SsdSampleWork.voice_count) {
    int channel = 0;
    short pitch = SSD_VOICE_PITCH_NORMAL;
    int volume = SSD_MIDI_VALUE_MAX;
    int pan = SSD_MIDI_PAN_CENTER;
    unsigned char effect = SSD_VOICE_EFFECT_REVERB | SSD_VOICE_EFFECT_DRY;
    int adsr1 = SSD_VOICE_ADSR1_RAW;
    do {
        SsdSamplingChannel *sample = &SsdSampleWork.channels[channel];
        SsdVoice *voice = sample->voice;
        sample->pitch = pitch;
        sample->state = 0;
        sample->status = 0;
        sample->volume[0] = volume;
        sample->pan[0] = pan;
        sample->effect = effect;
        sample->attributes = 0;
        voice->pitch = sample->pitch;
        SsdCalcDirectVoiceVolume(&voice->left_volume, sample->volume[0],
                                 sample->pan[0]);
        voice->flags = 0;
        voice->effects = sample->effect;
        voice->attributes = sample->attributes;
        voice->reserved_volume[0] = 0;
        voice->reserved_volume[1] = 0;
        voice->modulation = 0;
        voice->sample_start = SsdSpuMallocAdrsDynamic(SsdSampleWork.buffer_size,
                                                    SsdSampleWork.spu_address, SSD_SPU_BLOCK_SAMPLING);
        voice->adsr1 = adsr1;
        voice->adsr2 = SSD_VOICE_ADSR2_RAW;
        channel++;
    } while (channel < SsdSampleWork.voice_count);
    }
}

void SsdSampleKeyoffInterProc(SsdGlobalWork *work, void *argument)
{
    int channel;
    for (channel = 0; channel < SsdSampleWork.voice_count; channel++) {
        SsdSamplingChannel *sample = &SsdSampleWork.channels[channel];
        unsigned char state = sample->state;
        int active = sample->voice->allocation.envx;
        if (state & 2) {
            if (active == 0) {
                SsdSampleWork.keyoff_callback(1, channel, 0);
                if (SsdSampleWork.process.kind & 1)
                    SsdUnlinkProc(&SsdSampleWork.process);
                sample->state = 0;
            }
        } else if (active) {
            sample->state = state | 2;
        }
    }
}
