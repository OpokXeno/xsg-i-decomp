#include "ssd/ssd_chan.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_lfo.h"
#include "ssd/ssd_mem.h"
#include "ssd/ssd_voic.h"
#include "ssd/ssd_wave.h"

int SsdGetNewDeivceID(void);
void SsdLinkMidiDevice(SsdMidiDevice *device);
void SsdUnlinkMidiDevice(SsdMidiDevice *device);
void SsdResumeDeviceChannel(SsdMidiDevice *device);
void SsdResumeChannel(SsdChannel *channel);
void SsdSuspendDeviceChannel(SsdMidiDevice *device);
void SsdSuspendChannel(SsdChannel *channel);
void SsdSetMidiDeviceMasterValue(SsdMidiDevice *device, unsigned int note,
                               unsigned int volume, unsigned int panpot);
void SsdEnvxKeyoff(SsdChannel *channel);
void SsdLinkChannelVoice(SsdChannel *channel, SsdVoice *voice);
void SsdKeyonChannelWave(SsdChannel *channel, SsdMidiEvent *event);
void SsdKeyonChannelProgram(SsdChannel *channel, SsdMidiEvent *event);
SsdProgramSplit *SsdGetProgramSplitPtr(SsdProgram *program, int key);
void SsdForceKeyoffChannel(SsdChannel *channel, const SsdMidiEvent *event);
void SsdForceKeyoffChannelVoice(SsdChannel *channel, SsdVoice *voice);
void SsdSetChannelAdsr(SsdChannel *channel, int adsr1, int adsr2);

SsdMidiDevice *SsdNewMidiDevice(int channel_count, int allocation_mode,
                              const SsdVoiceRange *voice_range)
{
    SsdMidiDevice *device = SsdNewPtr(sizeof(*device) - sizeof(SsdChannel) +
        channel_count * sizeof(SsdChannel));
    if (device == 0)
        return 0;
    device->state = 0;
    device->id = SsdGetNewDeivceID();
    device->channel_count = channel_count;
    device->output_volume = SSD_MIDI_VALUE_MAX;
    if (voice_range != 0) {
        device->voice_range = *voice_range;
        if (device->voice_range.first >= SSD_VOICE_COUNT)
            device->voice_range.first = 0;
        if (device->voice_range.last >= SSD_VOICE_COUNT)
            device->voice_range.last = SSD_VOICE_COUNT - 1;
    } else {
        device->voice_range.first = 0;
        device->voice_range.last = SSD_VOICE_COUNT - 1;
    }
    {
        int index;
        SsdChannel *channel = device->channels;
        for (index = 0; index < channel_count;) {
            channel->flags = 0;
            channel->ordinal = index++;
            channel->priority = 0;
            channel->allocation_mode = allocation_mode;
            channel->voice_range = device->voice_range;
            channel->voices = 0;
            channel->device = device;
            ++channel;
        }
    }
    SsdLinkMidiDevice(device);
    SsdResetChannelFader(device, 0);
    return device;
}

void SsdDisposeMidiDevice(SsdMidiDevice *device)
{
    SsdResetChannelAll(device, 0);
    SsdUnlinkMidiDevice(device);
    SsdDisposePtr(device);
}

int SsdGetNewDeivceID(void)
{
    int id = 1;
    SsdMidiDevice *device;
    for (;;) {
        device = SsdWork.devices;
        while (device != 0) {
            if (device->id == id)
                break;
            device = device->next;
        }
        if (device == 0)
            break;
        ++id;
        if (id >= 255) {
            id = -1;
            break;
        }
    }
    return id;
}

void SsdLinkMidiDevice(SsdMidiDevice *device)
{
    SsdMidiDevice *tail = SsdWork.devices;
    if (tail != 0) {
        while (tail->next != 0)
            tail = tail->next;
        tail->next = device;
    } else {
        SsdWork.devices = device;
    }
}

void SsdUnlinkMidiDevice(SsdMidiDevice *device)
{
    SsdMidiDevice *previous = SsdWork.devices;
    if (previous == device) {
        SsdWork.devices = previous->next;
    } else {
        while (previous->next != device)
            previous = previous->next;
        previous->next = device->next;
    }
}

void SsdResumeDeviceAll(void)
{
    SsdMidiDevice *device = SsdWork.devices;
    while (device != 0) {
        SsdResumeDeviceChannel(device);
        device = device->next;
    }
}

void SsdResumeDeviceChannel(SsdMidiDevice *device)
{
    int remaining = device->channel_count;
    SsdChannel *channel = device->channels;
    while (remaining != 0) {
        SsdResumeChannel(channel);
        --remaining;
        ++channel;
    }
}

void SsdResumeChannel(SsdChannel *channel)
{
    SsdVoice *voice = channel->voices;
    while (voice != 0) {
        voice->flags |= SSD_VOICE_FLAG_VOLUME | SSD_VOICE_FLAG_PITCH |
                        SSD_VOICE_FLAG_ADSR1 | SSD_VOICE_FLAG_ADSR2;
        voice = voice->next;
    }
}

void SsdSuspendDeviceAll(void)
{
    SsdMidiDevice *device = SsdWork.devices;
    while (device != 0) {
        SsdSuspendDeviceChannel(device);
        device = device->next;
    }
}

void SsdSuspendDeviceChannel(SsdMidiDevice *device)
{
    int remaining = device->channel_count;
    SsdChannel *channel = device->channels;
    while (remaining != 0) {
        SsdSuspendChannel(channel);
        --remaining;
        ++channel;
    }
}

void SsdSuspendChannel(SsdChannel *channel)
{
    SsdVoice *voice = channel->voices;
    while (voice != 0) {
        unsigned char entry = voice->core | (voice->number << 1);
        unsigned short adsr_entry;
        unsigned short adsr;
        sceSdSetParam(entry | SD_VP_VOLL, 0);
        sceSdSetParam(entry | SD_VP_VOLR, 0);
        sceSdSetParam(entry | SD_VP_PITCH, 0);
        adsr_entry = entry | SD_VP_ADSR1;
        adsr = sceSdGetParam(adsr_entry);
        adsr |= 0x7f00;
        sceSdSetParam(adsr_entry, adsr);
        adsr_entry = entry | SD_VP_ADSR2;
        sceSdSetParam(adsr_entry, (sceSdGetParam(adsr_entry) & 0xe000) | 0x1fe6);
        voice = voice->next;
    }
}

void SsdResetMidiDeviceParam(SsdMidiDevice *device, int flags)
{
    int volume = (device->output_volume + 1) * 128;
    device->reset_flags = flags;
    device->volume = SSD_MIDI_VALUE_MAX;
    if (volume != 0)
        --volume;
    device->master_volume = volume * 512;
    device->master_note = 0;
    device->master_panpot = 0;
}

void SsdResetChannelAll(SsdMidiDevice *device, int flags)
{
    int remaining = device->channel_count;
    SsdChannel *channel = device->channels;
    if (flags < 0)
        flags = device->reset_flags;
    while (remaining != 0) {
        SsdCutoffChannel(channel);
        SsdResetChannel(channel, flags);
        --remaining;
        ++channel;
    }
}

void SsdResetChannel(SsdChannel *channel, int bank)
{
    channel->effects = SSD_VOICE_EFFECT_DRY;
    channel->program_type = SSD_PROGRAM_TYPE_WAVE;
    channel->program_index = -1;
    channel->flags = 0;
    channel->bank = bank;
    channel->wave_index = 0;
    channel->transpose = 0;
    channel->pitch_bend = 0;
    channel->channel_volume = SSD_CHANNEL_VOLUME_DEFAULT;
    channel->pitch_offset = 0;
    channel->voices = 0;
    channel->bank_data = 0;
    channel->wave_data = 0;
    channel->program_data = 0;
    channel->expression = SSD_MIDI_VALUE_MAX;
    channel->panpot = SSD_PANPOT_CENTER_FIXED;
    channel->pitch_bend_sensitivity = SSD_PITCH_BEND_SENSITIVITY_DEFAULT;
    channel->volume = channel->fader_volume * SSD_CHANNEL_VOLUME_DEFAULT *
        SSD_MIDI_VALUE_MAX / (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
    SsdInitLfoWork(&channel->lfo);
}

void SsdResetChannelFader(SsdMidiDevice *device, int propagate)
{
    int remaining = device->channel_count;
    SsdChannel *channel = device->channels;
    int reset_volume;
    int reset_panpot;
    if (remaining != 0) {
        reset_volume = SSD_MIDI_VALUE_MAX;
        reset_panpot = SSD_MIDI_PAN_CENTER;
        do {
        channel->fader_note = 0;
        channel->fader_volume = reset_volume;
        channel->fader_panpot = reset_panpot;
        if (propagate != 0) {
            int volume = channel->channel_volume;
            int expression = channel->expression;
            channel->volume = volume * expression / SSD_MIDI_VALUE_MAX;
            SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_PITCH | SSD_VOICE_EVENT_VOLUME);
        }
        --remaining;
        ++channel;
        } while (remaining != 0);
    }
}

void SsdSetMidiDevicePriority(SsdMidiDevice *device, unsigned char priority)
{
    SsdChannel *channel = device->channels;
    int remaining = device->channel_count;
    while (remaining != 0) {
        channel->priority = priority;
        --remaining;
        ++channel;
    }
}

void SsdSetMidiDeviceMasterValue(SsdMidiDevice *device, unsigned int note,
                               unsigned int volume, unsigned int panpot)
{
    device->master_note = note << 16;
    device->master_volume = volume << 16;
    device->master_panpot = panpot << 16;
    SsdSetChannelAllVoiceEvent(device, SSD_VOICE_EVENT_PITCH | SSD_VOICE_EVENT_VOLUME);
}

void SsdSetMidiDeviceNote(SsdMidiDevice *device, unsigned int note)
{
    device->master_note = note << 16;
    SsdSetChannelAllVoiceEvent(device, SSD_VOICE_EVENT_PITCH);
}

void SsdSetMidiDeviceVolume(SsdMidiDevice *device, int volume)
{
    device->volume = volume;
    volume = (volume + 1) * (device->output_volume + 1);
    if (volume != 0)
        --volume;
    device->master_volume = volume * 512;
    SsdSetChannelAllVoiceEvent(device, SSD_VOICE_EVENT_VOLUME);
}

void SsdSetMidiDevicePanpot(SsdMidiDevice *device, int panpot)
{
    device->master_panpot = (panpot - SSD_MIDI_PAN_CENTER) * 131072;
    SsdSetChannelAllVoiceEvent(device, SSD_VOICE_EVENT_VOLUME);
}

void SsdSetMidiDeviceOutputVolume(SsdMidiDevice *device, int volume)
{
    device->output_volume = volume;
    volume = (device->volume + 1) * (volume + 1);
    if (volume != 0)
        --volume;
    device->master_volume = volume * 512;
    SsdSetChannelAllVoiceEvent(device, SSD_VOICE_EVENT_VOLUME);
}

void SsdCutoffDevive(SsdMidiDevice *device)
{
    int remaining = device->channel_count;
    SsdChannel *channel = device->channels;
    while (remaining != 0) {
        SsdCutoffChannel(channel);
        --remaining;
        ++channel;
    }
}

void SsdCutoffChannel(SsdChannel *channel)
{
    while (channel->voices != 0)
        SsdUnlinkChannelVoice(channel->voices);
}

void SsdEnvxKeyoff(SsdChannel *channel)
{
    SsdVoice *voice = channel->voices;

    while (voice != 0) {
        SsdVoice *next = voice->next;

        if (voice->allocation.envx == 0)
            SsdUnlinkChannelVoice(channel->voices);
        voice = next;
    }
}

void SsdLinkChannelVoice(SsdChannel *channel, SsdVoice *voice)
{
    if (voice->channel != 0)
        SsdUnlinkChannelVoice(voice);
    voice->next = channel->voices;
    channel->voices = voice;
    voice->channel = channel;
}

void SsdUnlinkChannelVoice(SsdVoice *voice)
{
    SsdChannel *channel = voice->channel;

    if (channel != 0) {
        SsdVoice *previous = channel->voices;

        if (previous == voice) {
            channel->voices = voice->next;
        } else {
            while (previous->next != voice)
                previous = previous->next;
            previous->next = voice->next;
        }
        voice->channel = 0;
    }
    voice->key_state = 0;
    voice->next = 0;
    SsdCutoffUnlinkVoice(voice);
}

void SsdSetChannelAllVoiceEvent(SsdMidiDevice *device, unsigned short events)
{
    int channels_remaining = device->channel_count;
    SsdChannel *channel = device->channels;

    while (channels_remaining != 0) {
        --channels_remaining;
        SsdSetChannelVoiceEvent(channel, events);
        ++channel;
    }
}

void SsdSetChannelVoiceEvent(SsdChannel *channel, unsigned short events)
{
    SsdVoice *voice = channel->voices;

    while (voice != 0) {
        voice->events |= events;
        voice = voice->next;
    }
}

void SsdSetChannelVoiceVelocity(SsdChannel *channel, int velocity)
{
    SsdVoice *voice = channel->voices;
    velocity = velocity << 24;
    if (voice == 0)
        return;
process_voice:
    voice->velocity = (velocity >> 24) * voice->base_velocity;
    voice->events |= SSD_VOICE_EVENT_VOLUME;
    voice = voice->next;
    if (voice != 0)
        goto process_voice;
}

void SsdSetChannelHWEffectOn(SsdChannel *channel, unsigned char effects,
                           unsigned short flags)
{
    SsdVoice *voice = channel->voices;

    channel->effects = effects | channel->effects;
    while (voice != 0) {
        voice->effects |= effects;
        voice->flags |= flags;
        voice = voice->next;
    }
}

void SsdSetChannelHWEffectOff(SsdChannel *channel, unsigned char effects,
                            unsigned short flags)
{
    SsdVoice *voice = channel->voices;

    effects = ~effects;
    channel->effects = effects & channel->effects;
    while (voice != 0) {
        voice->effects &= effects;
        voice->flags |= flags;
        voice = voice->next;
    }
}

void SsdKeyonChannel(SsdChannel *channel, SsdMidiEvent *event)
{
    if ((channel->flags & SSD_CHANNEL_SLUR) == 0)
        SsdForceKeyoffChannel(channel, event);
    switch (channel->program_type) {
    case SSD_PROGRAM_TYPE_WAVE:
        SsdKeyonChannelWave(channel, event);
        break;
    case SSD_PROGRAM_TYPE_PROGRAM:
        SsdKeyonChannelProgram(channel, event);
        break;
    }
}

void SsdKeyonChannelWave(SsdChannel *channel, SsdMidiEvent *event)
{
    SsdWaveSample *wave = channel->wave_data;
    SsdWaveBank *bank = channel->bank_data;
    unsigned int sample_address = wave->body_offset;
    SsdVoice *voice = SsdGetKeyonVoice(channel, &channel->voice_range);

    if (voice != 0) {
        if ((channel->flags & SSD_CHANNEL_SLUR) == 0) {
            voice->sample_start = sample_address + bank->spu_address;
            voice->loop_start = sample_address + wave->loop_offset;
        }
        voice->key = event->key;
        voice->effects = channel->effects;
        voice->adsr1 = channel->adsr1;
        voice->adsr2 = channel->adsr2;
        voice->release_rate = channel->adsr2 & SSD_ADSR2_RELEASE_RATE_MASK;
        voice->wave_index = channel->wave_index;
        voice->base_velocity = wave->volume;
        voice->base_panpot = wave->pan;
        voice->base_note = wave->base_pitch;
        voice->transpose = 0;
        voice->duration = event->duration;
        voice->note = voice->base_note + (event->key << 8);
        voice->velocity = event->velocity * wave->volume;
        voice->events |= SSD_VOICE_EVENT_PARAMETERS;
        SsdResetLfoProc(&voice->lfo, &channel->lfo);
        SsdKeyonChannelVoice(channel, voice);
    }
}

void SsdKeyonChannelProgram(SsdChannel *channel, SsdMidiEvent *event)
{
    if ((channel->flags & SSD_CHANNEL_SLUR) == 0) {
        SsdProgram *program = channel->program_data;
        SsdProgramSplit *split = program->splits;
        unsigned short key;
        SsdWaveBank *bank;
        int splits_remaining;

        if ((channel->flags & SSD_CHANNEL_RANDOM_KEY) != 0) {
            key = SsdGetRSRandom() %
                (channel->random_note_high - channel->random_note_low) +
                channel->random_note_low;
        } else {
            key = event->key;
        }
        bank = channel->bank_data;
        splits_remaining = program->split_count;

        {
        short range_key = key;
        while (splits_remaining != 0) {
            --splits_remaining;
            if (range_key >= split->lower_key && range_key <= split->upper_key) {
                SsdWaveSample *wave = &bank->samples[split->wave_index];
                unsigned int sample_address = wave->body_offset;
                SsdVoiceRange range;
                SsdVoice *voice;

                range.first = program->voice_range.first;
                if (range.first < channel->voice_range.first)
                    range.first = channel->voice_range.first;
                range.last = program->voice_range.last;
                if (range.last > channel->voice_range.last)
                    range.last = channel->voice_range.last;
                voice = SsdGetKeyonVoice(channel, &range);
                if (voice != 0) {
                    unsigned short adsr2;
                    unsigned short adsr1;
                    int base_velocity;
                    signed char transpose;
                    unsigned short note;

                    voice->sample_start = sample_address + bank->spu_address;
                    voice->loop_start = sample_address + wave->loop_offset;
                    voice->key = key;
                    voice->effects = channel->effects & split->effects;
                    adsr2 = split->adsr2;
                    adsr1 = split->adsr1;
                    voice->adsr2 = adsr2;
                    voice->adsr1 = adsr1;
                    voice->release_rate = adsr2 & SSD_ADSR2_RELEASE_RATE_MASK;
                    voice->wave_index = split->wave_index;
                    base_velocity = wave->volume * split->volume / SSD_MIDI_VALUE_MAX;
                    voice->base_velocity = base_velocity;
                    voice->base_panpot = split->panpot;
                    voice->base_note = wave->base_pitch;
                    transpose = SSD_NOTE_MIDDLE_C - split->root_key;
                    if ((channel->flags & SSD_CHANNEL_FIXED_PITCH) != 0) {
                        voice->transpose = 0;
                        note = voice->base_note + (SSD_NOTE_MIDDLE_C + transpose) * 256;
                    } else {
                        voice->transpose = transpose;
                        note = voice->base_note +
                            ((key + transpose) << 8);
                    }
                    voice->note = note;
                    voice->duration = event->duration;
                    voice->velocity = event->velocity * base_velocity;
                    voice->events |= SSD_VOICE_EVENT_PARAMETERS;
                    SsdResetLfoProc(&voice->lfo, &channel->lfo);
                    SsdKeyonChannelVoice(channel, voice);
                }
            }
            ++split;
        }
        }
    }
}

SsdProgramSplit *SsdGetProgramSplitPtr(SsdProgram *program, int key)
{
    SsdProgramSplit *split = program->splits;

    while (split->upper_key < key)
        ++split;
    return split;
}

void SsdKeyonChannelVoice(SsdChannel *channel, SsdVoice *voice)
{
    unsigned short events = voice->events;

    if ((channel->flags & SSD_CHANNEL_SLUR) == 0)
        events |= SSD_VOICE_EVENT_KEYON;
    voice->events = events & ~SSD_VOICE_EVENT_KEYOFF;
    voice->allocation.priority |= (channel->priority << 16) + SSD_VOICE_PRIORITY_ACTIVE;
    if (voice->channel != channel) {
        voice->next = channel->voices;
        channel->voices = voice;
        voice->channel = channel;
    }
}

void SsdSendChannelKeyoffEvent(SsdChannel *channel)
{
    if ((channel->flags & (SSD_CHANNEL_SLUR | SSD_CHANNEL_SUSTAIN)) == 0) {
        SsdVoice *voice = channel->voices;

        while (voice != 0) {
            if ((voice->key_state & SSD_VOICE_KEY_RELEASED) != 0)
                SsdKeyoffChannelVoice(channel, voice);
            voice = voice->next;
        }
    }
}

void SsdKeyoffChannel(SsdChannel *channel, const SsdMidiEvent *event)
{
    SsdVoice *voice = channel->voices;

    while (voice != 0) {
        if (voice->key == event->key)
            SsdKeyoffChannelVoice(channel, voice);
        voice = voice->next;
    }
}

void SsdKeyoffChannelVoice(SsdChannel *channel, SsdVoice *voice)
{
    voice->key_state |= SSD_VOICE_KEY_RELEASED;
    if ((channel->flags & (SSD_CHANNEL_SLUR | SSD_CHANNEL_SUSTAIN)) == 0) {
        if (channel->allocation_mode == SSD_ALLOCATION_FIXED)
            voice->allocation.priority &= SSD_VOICE_PRIORITY_KEYOFF_MASK;
        else
            voice->allocation.priority &= SSD_VOICE_PRIORITY_RELEASE;
        voice->adsr2 = (voice->adsr2 & ~SSD_ADSR2_RELEASE_RATE_MASK) | voice->release_rate;
        voice->flags |= SSD_VOICE_FLAG_ADSR2;
        voice->events = (voice->events & SSD_VOICE_EVENT_KEYON) | SSD_VOICE_EVENT_KEYOFF;
    }
}

void SsdForceKeyoffChannel(SsdChannel *channel, const SsdMidiEvent *event)
{
    SsdVoice *voice = channel->voices;

    while (voice != 0) {
        if (voice->key == event->key)
            SsdForceKeyoffChannelVoice(channel, voice);
        voice = voice->next;
    }
}

void SsdForceKeyoffChannelVoice(SsdChannel *channel, SsdVoice *voice)
{
    voice->key_state |= SSD_VOICE_KEY_RELEASED;
    if (channel->allocation_mode == SSD_ALLOCATION_FIXED)
        voice->allocation.priority &= SSD_VOICE_PRIORITY_KEYOFF_MASK;
    else
        voice->allocation.priority &= SSD_VOICE_PRIORITY_RELEASE;
    voice->adsr2 = (voice->adsr2 & ~SSD_ADSR2_RELEASE_RATE_MASK) | SSD_FORCED_RELEASE_RATE;
    voice->events = (voice->events & SSD_VOICE_EVENT_KEYON) | SSD_VOICE_EVENT_KEYOFF;
    voice->flags |= SSD_VOICE_FLAG_ADSR2;
}

void SsdSetChannelWave(SsdChannel *channel, unsigned char wave_index)
{
    SsdWaveBank *bank = SsdGetWSPtrBank(channel->bank);
    SsdWaveSample *wave = 0;
    int program_type = 0;
    unsigned char index = wave_index;

    if (bank != 0) {
        channel->wave_index = wave_index;
        if (index < bank->sample_count) {
            wave = &bank->samples[index];
            if (wave->volume != 0) {
                unsigned short adsr1 = wave->adsr1;
                unsigned short adsr2 = wave->adsr2;
                channel->adsr1 = adsr1;
                channel->adsr2 = adsr2;
                program_type = SSD_PROGRAM_TYPE_WAVE;
            }
        }
    }
    channel->program_type = program_type;
    channel->bank_data = bank;
    channel->wave_data = wave;
    channel->program_index = -1;
    channel->program_data = 0;
}

SsdProgram *SsdSetChannelProgram(SsdChannel *channel, unsigned char program_index)
{
    SsdProgram *program = 0;
    int selected_index = -1;
    SsdWaveBank *bank = SsdGetWSPtrBank(channel->bank);
    int program_type = 0;

    if (bank != 0 && program_index < bank->program_count) {
        unsigned short *offsets = bank->programs;

        if (offsets != 0) {
            unsigned short offset = offsets[program_index];

            if (offset != 0) {
                unsigned short adsr1;
                unsigned short adsr2;

                selected_index = program_index;
                /* Program-table entries are byte offsets from the table base. */
                program = (SsdProgram *)((unsigned char *)offsets + offset);
                adsr1 = program->adsr1;
                adsr2 = program->adsr2;
                channel->adsr1 = adsr1;
                channel->adsr2 = adsr2;
                SsdInitLfoParam(&channel->lfo, program->lfo);
                program_type = SSD_PROGRAM_TYPE_PROGRAM;
            }
        }
    }
    channel->program_index = selected_index;
    channel->program_type = program_type;
    channel->bank_data = bank;
    channel->program_data = program;
    return program;
}

void SsdSetChannelAdsr(SsdChannel *channel, int adsr1, int adsr2)
{
    channel->adsr1 = adsr1;
    channel->adsr2 = adsr2;
}
