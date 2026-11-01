/* Original SSD ssd_dbg.c translation unit. Unmodeled spans retain original storage. */

typedef struct SsdSpuMemoryBlock {
    unsigned char next_index;
    unsigned char flags;
    unsigned short type;
    unsigned int address;
    unsigned int size;
    unsigned char unmodeled_00c[4];
} SsdSpuMemoryBlock;

typedef struct SsdMidiDevice {
    unsigned int state;
    unsigned char id;
    unsigned char channel_count;
    unsigned short reset_flags;
    unsigned char output_volume;
    unsigned char volume;
    int master_note;
    int master_volume;
    int master_panpot;
    unsigned char first_voice;
    unsigned char last_voice;
    struct SsdMidiDevice *next;
} SsdMidiDevice;

typedef struct SsdVoiceAllocationParams {
    unsigned char unmodeled_000[12];
    unsigned short adsr1, adsr2;
} SsdVoiceAllocationParams;

typedef struct SsdLfoParameters {
    unsigned char unmodeled_000[64];
} SsdLfoParameters;

typedef struct SsdLfoState {
    unsigned char unmodeled_000[176];
} SsdLfoState;

typedef struct SsdWavePatch {
    unsigned short bank_id;
    unsigned char wave_index, hardware_effect;
    unsigned short note;
    unsigned char base_key, transpose;
    signed char velocity_offset;
    unsigned char velocity, pan;
    unsigned char unmodeled_00b;
    SsdVoiceAllocationParams allocation;
    SsdLfoParameters lfo;
} SsdWavePatch;

typedef struct SsdVoice {
    unsigned char unmodeled_000[4];
    unsigned short events;
    unsigned char hardware_effect;
    unsigned char unmodeled_007[21];
    unsigned int start_address, loop_address;
    unsigned short adsr1, adsr2;
    unsigned char unmodeled_028[6];
    unsigned short control_events;
    int user_id;
    unsigned char unmodeled_034[4];
    unsigned short note;
    short velocity_adjustment;
    unsigned char release_rate, unmodeled_03d, base_key, wave_index;
    unsigned short base_note, base_velocity, base_pan;
    unsigned char unmodeled_046[2];
    SsdLfoState lfo;
    struct SsdVoice *next;
} SsdVoice;

typedef struct SsdMidiChannel {
    unsigned char unmodeled_000[4];
    unsigned char hardware_effect;
    unsigned char unmodeled_005[51];
    SsdLfoState lfo;
    SsdVoice *voice_head;
} SsdMidiChannel;

typedef struct SsdWaveEntry {
    unsigned int start_offset;
    unsigned int loop_offset;
    unsigned char unmodeled_008[24];
} SsdWaveEntry;

typedef struct SsdWaveBank {
    unsigned char unmodeled_000[4];
    unsigned short bank_id;
    unsigned char unmodeled_006[6];
    unsigned int spu_address;
    unsigned char unmodeled_010[8];
    SsdWaveEntry *waves;
} SsdWaveBank;

typedef struct SsdEffectData {
    unsigned char flags;
    unsigned char unmodeled_001[2];
    unsigned char track_count;
} SsdEffectData;

typedef struct SsdDebugState {
    SsdEffectData *effect_data;
    int effect_id;
    union {
        short parameter;
        unsigned short bank_id;
    } selection;
    int data_size;
    SsdMidiDevice *device;
    SsdMidiChannel *channel;
    char text[1024];
} SsdDebugState;

typedef struct SsdEffectSequence {
    unsigned char unmodeled_000[38];
    unsigned char track_count;
} SsdEffectSequence;

typedef struct SsdGlobalWork {
    unsigned char unmodeled_000[36];
    SsdSpuMemoryBlock spu_blocks[16];
    unsigned char unmodeled_124[72];
    SsdEffectSequence *effect_sequence;
} SsdGlobalWork;

typedef struct SsdEffectPlayParameters {
    int effect_id;
    int mode;
    unsigned short bank_id;
    signed char start_track;
    unsigned char track_count;
    signed char first_track;
    signed char priority;
    signed char volume;
    signed char pan;
    unsigned char flags;
    SsdEffectData *data;
} SsdEffectPlayParameters;

extern SsdGlobalWork SsdWork;

SsdDebugState SsdDebWork;

extern const char *SsdSpuMemTypeTextTable[13];

extern int sprintf(char *, const char *, ...);

extern int printf(const char *, ...);

extern void CpuDisableIntr(void);

extern void CpuEnableIntr(void);

extern SsdWaveBank *SsdGetWSPtrSpuAdrs(unsigned int);

extern int SsdGetSpuMemoryFree(void);

extern int SsdGetMemoryFreeSize(void);

extern int QueryMaxFreeMemSize(void);

extern SsdMidiDevice *SsdNewMidiDevice(int, int, int);

extern void SsdResetMidiDeviceParam(SsdMidiDevice *, int);

extern void SsdResetChannelAll(SsdMidiDevice *, int);

extern void SsdDisposeMidiDevice(SsdMidiDevice *);

extern SsdWaveBank *SsdGetWSPtrBank(int);

extern SsdVoice *SsdGetKeyonVoice(SsdMidiChannel *, SsdVoiceAllocationParams *);

extern void SsdInitLfoParam(SsdLfoState *, SsdLfoParameters *);

extern void SsdResetLfoProc(SsdLfoState *, SsdLfoState *);

extern void SsdKeyonChannelVoice(SsdMidiChannel *, SsdVoice *);

extern void SsdKeyoffChannelVoice(SsdMidiChannel *, SsdVoice *);

extern void SsdStopEffect(int, int);

extern void SsdDisposePtr(void *);

extern void *SsdNewPtr(int);

extern void SsdMemoryCopy(void *, const void *, int);

extern int SsdSaechSeqFreeTrack(SsdEffectPlayParameters *);

extern void SsdPlaySeqEffectNormal(SsdEffectPlayParameters *);

int SsdCheckSpuMemory(int verbose);

void SsdDebOpenDevice(void);

void SsdDebCloseDevice(void);

void SsdDebWaveKeyon(SsdWavePatch *patch, unsigned int count, int user_id);

void SsdDebSetWaveToVoiceControl(SsdMidiChannel *channel, SsdVoice *voice,
                               SsdWavePatch *patch, SsdWaveBank *bank);

void SsdDebWaveKeyoff(int user_id);

void SsdMidiDevSetMidiEvent(void);

void SsdMidiDevSetHWEffect(int device_id, int hardware_effect);

void SsdDebAddEffect(const void *data, int size, int effect_id, short parameter);

void SsdDebPlayEffect(void);

char *NextTextPtr(char *text);

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
        block = SsdWork.spu_blocks;
        ordinal = 0;
        CpuDisableIntr();
        wave_type = 9;
        line_end = '\n';
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
                SsdSpuMemoryBlock *blocks = SsdWork.spu_blocks;
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

void SsdDebOpenDevice(void)
{
    SsdMidiDevice *device = SsdNewMidiDevice(1, 0, 0);
    SsdDebWork.device = device;
    /* Channels follow the evidenced 32-byte device header in one allocation. */
    SsdDebWork.channel = (SsdMidiChannel *)(device + 1);
    SsdResetMidiDeviceParam(device, 0);
    SsdResetChannelAll(device, 0);
}

void SsdDebCloseDevice(void)
{
    SsdDisposeMidiDevice(SsdDebWork.device);
    SsdDebWork.device = 0;
    SsdDebWork.channel = 0;
}

void SsdDebWaveKeyon(SsdWavePatch *patch, unsigned int count, int user_id)
{
    SsdWaveBank *bank = SsdGetWSPtrBank(patch->bank_id);
    if (bank) {
        SsdMidiChannel *channel = SsdDebWork.channel;
        CpuDisableIntr();
        while (count) {
            SsdVoice *voice = SsdGetKeyonVoice(channel, &patch->allocation);
            SsdDebSetWaveToVoiceControl(channel, voice, patch, bank);
            SsdInitLfoParam(&channel->lfo, &patch->lfo);
            SsdResetLfoProc(&voice->lfo, &channel->lfo);
            voice->user_id = user_id;
            SsdKeyonChannelVoice(channel, voice);
            --count;
            ++patch;
        }
        CpuEnableIntr();
    }
}

void SsdDebSetWaveToVoiceControl(SsdMidiChannel *channel, SsdVoice *voice,
                               SsdWavePatch *patch, SsdWaveBank *bank)
{
    SsdWaveEntry *wave = &bank->waves[patch->wave_index];
    voice->start_address = wave->start_offset + bank->spu_address;
    voice->loop_address = voice->start_address + wave->loop_offset;
    voice->base_key = patch->base_key;
    voice->note = patch->note + ((unsigned int)(patch->base_key + (signed char)patch->transpose) << 8);
    voice->hardware_effect = channel->hardware_effect & patch->hardware_effect;
    voice->adsr1 = patch->allocation.adsr1;
    voice->adsr2 = patch->allocation.adsr2;
    voice->release_rate = (unsigned char)patch->allocation.adsr2 & 31;
    voice->wave_index = patch->wave_index;
    voice->base_velocity = patch->velocity;
    voice->base_pan = patch->pan;
    voice->base_note = patch->note;
    voice->velocity_adjustment = patch->velocity * patch->velocity_offset;
    voice->control_events = 0xfff0;
    voice->events = 0xffff;
}

void SsdDebWaveKeyoff(int user_id)
{
    SsdMidiChannel *channel = SsdDebWork.channel;
    SsdVoice *voice = channel->voice_head;
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

void SsdMidiDevSetHWEffect(int device_id, int hardware_effect)
{
    SsdMidiChannel *channel = SsdDebWork.channel;
    SsdVoice *voice = channel->voice_head;
    channel->hardware_effect = hardware_effect;
    while (voice) {
        voice->hardware_effect = hardware_effect;
        voice->events |= 255;
        voice = voice->next;
    }
}

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

void SsdDebPlayEffect(void)
{
    SsdEffectPlayParameters parameters[1];
    SsdEffectData *data = SsdDebWork.effect_data;
    if (data) {
        SsdEffectSequence *sequence = SsdWork.effect_sequence;
        unsigned char track_count;
        parameters->mode = 0;
        parameters->start_track = 0;
        parameters->effect_id = SsdDebWork.effect_id;
        parameters->bank_id = SsdDebWork.selection.bank_id;
        do {
            parameters->first_track = sequence->track_count - data->track_count;
        } while (0);
        track_count = data->track_count;
        parameters->priority = 127;
        parameters->volume = 127;
        parameters->pan = 63;
        parameters->track_count = track_count;
        parameters->flags = data->flags;
        parameters->data = data;
        SsdSaechSeqFreeTrack(parameters);
        SsdPlaySeqEffectNormal(parameters);
    }
}

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

const char *SsdSpuMemTypeTextTable[13] = {
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
