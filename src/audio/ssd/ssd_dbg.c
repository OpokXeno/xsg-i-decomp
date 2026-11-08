#include "ssd/ssd_dbg.h"
#include "ssd/ssd_chan.h"
#include "ssd/ssd_data.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_mem.h"
#include "ssd/ssd_sec.h"
#include "ssd/ssd_seq.h"
#include "ssd/ssd_voic.h"
#include "ssd/ssd_wave.h"

int QueryMaxFreeMemSize(void);

/* Wave keyon request of the debug interface. */
typedef struct SsdWavePatch {
    unsigned short bank_id;
    unsigned char wave_index, hardware_effect;
    unsigned short note;
    unsigned char base_key;
    signed char transpose;
    signed char velocity_offset;
    unsigned char velocity, pan;
    unsigned char unmodeled_0b;
    SsdVoiceRange voice_range;
    unsigned char unmodeled_0e[10];
    unsigned short adsr1, adsr2;
    SsdLfoParam lfo[4];
} SsdWavePatch;

typedef struct SsdDebugState {
    SsdEffectEntry *effect_data;
    int effect_id;
    union {
        short parameter;
        unsigned short bank_id;
    } selection;
    int data_size;
    SsdMidiDevice *device;
    SsdChannel *channel;
    char text[1024];
} SsdDebugState;

SsdDebugState SsdDebWork;

extern const char *SsdSpuMemTypeTextTable[SSD_SPU_BLOCK_TYPE_COUNT];

void SsdDebSetWaveToVoiceControl(SsdChannel *channel, SsdVoice *voice, SsdWavePatch *patch,
                                 SsdWaveBank *bank);
void SsdMidiDevSetMidiEvent(void);
void SsdMidiDevSetHWEffect(int device_id, int hardware_effect);
void SsdDebAddEffect(const void *data, int size, int effect_id, short parameter);
char *NextTextPtr(char *text);

/*
 * Returns the free SPU memory. When verbose, also prints a report of every SPU
 * memory block and the free sizes.
 */
int SsdCheckSpuMemory(int verbose)
{
    int spu_free;
    if (verbose) {
        char *text = SsdDebWork.text;
        int next_index;
        SsdSpuMemoryBlock *block;
        unsigned int ordinal;
        int wave_type;
        int line_end;
        int main_free;
        int system_free;
        sprintf(text, "Ssd spu memory check  Ver 1.0.1\n");
        text = NextTextPtr(text);
        block = SsdWork.spu_memory.blocks;
        ordinal = 0;
        CpuDisableIntr();
        wave_type = SSD_SPU_BLOCK_WAVE;
        line_end = '\n';
        /* One line per SPU block, following the block links from the first block. */
        do {
            sprintf(text, "  Block %02d : Adrs=0x%06x-0x%06x   Size=0x%06x  %s", ordinal, block->address,
                    block->address + block->size, block->size,
                    SsdSpuMemTypeTextTable[block->type]);
            text = NextTextPtr(text);
            if (block->type == wave_type) {
                SsdWaveBank *bank = SsdGetWSPtrSpuAdrs(block->address);
                sprintf(text, "  Bank=0x%04x", bank->bank_id);
                text = NextTextPtr(text);
            }
            *text++ = line_end;
            next_index = block->next_index;
            if (next_index) {
                SsdSpuMemoryBlock *blocks = SsdWork.spu_memory.blocks;
                block = &blocks[next_index];
                ++ordinal;
            }
        } while (next_index);
        spu_free = SsdGetSpuMemoryFree();
        main_free = SsdGetMemoryFreeSize();
        system_free = QueryMaxFreeMemSize();
        CpuEnableIntr();
        sprintf(text, "  Spu free size=0x%06x  Ssd free size=0x%06x  Iop free size=0x%06x\n", spu_free, main_free, system_free);
        printf(SsdDebWork.text);
    } else {
        spu_free = SsdGetSpuMemoryFree();
    }
    return spu_free;
}

/* Creates the debug MIDI device and resets its parameters and channels. */
void SsdDebOpenDevice(void)
{
    SsdMidiDevice *device = SsdNewMidiDevice(1, 0, 0);
    SsdDebWork.device = device;
    SsdDebWork.channel = device->channels;
    SsdResetMidiDeviceParam(device, 0);
    SsdResetChannelAll(device, 0);
}

/* Disposes the debug MIDI device. */
void SsdDebCloseDevice(void)
{
    SsdDisposeMidiDevice(SsdDebWork.device);
    SsdDebWork.device = 0;
    SsdDebWork.channel = 0;
}

/* Keys on count voices, one per patch entry, all tagged with user_id. */
void SsdDebWaveKeyon(SsdWavePatch *patch, unsigned int count, int user_id)
{
    SsdWaveBank *bank = SsdGetWSPtrBank(patch->bank_id);
    if (bank) {
        SsdChannel *channel = SsdDebWork.channel;
        CpuDisableIntr();
        while (count) {
            SsdVoice *voice = SsdGetKeyonVoice(channel, &patch->voice_range);
            SsdDebSetWaveToVoiceControl(channel, voice, patch, bank);
            SsdInitLfoParam(&channel->lfo, patch->lfo);
            SsdResetLfoProc(&voice->lfo, &channel->lfo);
            voice->user_id = user_id;
            SsdKeyonChannelVoice(channel, voice);
            --count;
            ++patch;
        }
        CpuEnableIntr();
    }
}

/* Copies a wave patch's sample addresses, pitch, envelope and velocity into voice. */
void SsdDebSetWaveToVoiceControl(SsdChannel *channel, SsdVoice *voice,
                               SsdWavePatch *patch, SsdWaveBank *bank)
{
    SsdWaveSample *wave = &bank->samples[patch->wave_index];
    voice->sample_start = wave->body_offset + bank->spu_address;
    voice->loop_start = voice->sample_start + wave->loop_offset;
    voice->key = patch->base_key;
    voice->note = patch->note + ((patch->base_key + patch->transpose) << 8);
    voice->effects = channel->effects & patch->hardware_effect;
    voice->adsr1 = patch->adsr1;
    voice->adsr2 = patch->adsr2;
    voice->release_rate = patch->adsr2 & SSD_ADSR2_RELEASE_RATE_MASK;
    voice->wave_index = patch->wave_index;
    voice->base_velocity = patch->velocity;
    voice->base_panpot = patch->pan;
    voice->base_note = patch->note;
    voice->velocity = patch->velocity * patch->velocity_offset;
    voice->events = SSD_VOICE_EVENT_PARAMETERS;
    voice->flags = SSD_VOICE_FLAG_ALL;
}

/* Keys off the debug channel's voices tagged with user_id. */
void SsdDebWaveKeyoff(int user_id)
{
    SsdChannel *channel = SsdDebWork.channel;
    SsdVoice *voice = channel->voices;
    CpuDisableIntr();
    while (voice) {
        if (voice->user_id == user_id) {
            SsdKeyoffChannelVoice(channel, voice);
            voice->user_id = 0;
        }
        voice = voice->next;
    }
    CpuEnableIntr();
}

void SsdMidiDevSetMidiEvent(void)
{
}

/* Sets the hardware effect mask on the debug channel and on each of its voices. */
void SsdMidiDevSetHWEffect(int device_id, int hardware_effect)
{
    SsdChannel *channel = SsdDebWork.channel;
    SsdVoice *voice = channel->voices;
    channel->effects = hardware_effect;
    while (voice) {
        voice->effects = hardware_effect;
        voice->flags |= 255;
        voice = voice->next;
    }
}

/* Replaces the stored effect with a copy of data and plays it. */
void SsdDebAddEffect(const void *data, int size, int effect_id, short parameter)
{
    if (SsdDebWork.effect_data) {
        SsdStopEffect(SsdDebWork.effect_id, 0);
        SsdDisposePtr(SsdDebWork.effect_data);
    }
    SsdDebWork.effect_data = SsdNewPtr(size);
    SsdMemoryCopy(SsdDebWork.effect_data, data, size);
    SsdDebWork.effect_id = effect_id;
    SsdDebWork.selection.parameter = parameter;
    SsdDebWork.data_size = size;
    SsdDebPlayEffect();
}

/* Plays the stored effect as a normal sequence effect in the selected bank. */
void SsdDebPlayEffect(void)
{
    SsdEffectPlayParam parameters[1];
    SsdEffectEntry *data = SsdDebWork.effect_data;
    if (data) {
        SsdSequence *sequence = SsdWork.effect_sequence;
        unsigned char track_count;
        parameters->mode = 0;
        parameters->start_track = 0;
        parameters->effect_id = SsdDebWork.effect_id;
        parameters->bank_id = SsdDebWork.selection.bank_id;
        do {
            parameters->first_track = sequence->track_count - data->track_count;
        } while (0);
        track_count = data->track_count;
        parameters->priority = SSD_MIDI_VALUE_MAX;
        parameters->volume = SSD_MIDI_VALUE_MAX;
        parameters->pan = 63;
        parameters->track_count = track_count;
        parameters->flags = data->flags;
        parameters->effect_data = data;
        SsdSaechSeqFreeTrack(parameters);
        SsdPlaySeqEffectNormal(parameters);
    }
}

/* Returns a pointer to the terminating NUL of text. */
char *NextTextPtr(char *text)
{
    while (*text++) {
    }
    return text - 1;
}

/* Names of the SPU memory block types printed by the memory report. */
char SsdSpuMemUnuseText[] = "Unuse";
char SsdSpuMemSystemText[] = "System";
char SsdSpuMemDriverText[] = "Driver";
char SsdSpuMemStreamText[] = "Stream";
char SsdSpuMemRevCore0Text[] = "RevCore0";
char SsdSpuMemRevCore1Text[] = "RevCore1";
char SsdSpuMemRevCore2Text[] = "RevCore2";
char SsdSpuMemRevCore3Text[] = "RevCore3";
char SsdSpuMemOWBlockText[] = "OWBlock";
char SsdSpuMemWaveText[] = "Wave";
char SsdSpuMemSampleText[] = "Sampling";

const char *SsdSpuMemTypeTextTable[SSD_SPU_BLOCK_TYPE_COUNT] = {
    SsdSpuMemUnuseText,
    SsdSpuMemSystemText,
    SsdSpuMemDriverText,
    SsdSpuMemStreamText,
    SsdSpuMemRevCore0Text,
    SsdSpuMemRevCore1Text,
    SsdSpuMemRevCore2Text,
    SsdSpuMemRevCore3Text,
    SsdSpuMemOWBlockText,
    SsdSpuMemWaveText,
    SsdSpuMemSampleText,
    0,
    0,
};
