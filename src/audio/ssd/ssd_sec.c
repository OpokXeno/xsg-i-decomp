#include "ssd/ssd_sec.h"
#include "ssd/ssd_chan.h"
#include "ssd/ssd_data.h"
#include "ssd/ssd_err.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_mem.h"
#include "ssd/ssd_seq.h"
#include "ssd/ssd_wave.h"

void SsdFadeoutStopEffectTrack(SsdTrack *track, int duration);
SsdTrack *SsdPlayWaveEffectNormal(SsdEffectPlayParam *play);
SsdEffectData *SsdGetFileIDToEffectData(int file_id);
void SsdStopEffectFileID(int file_id);
int SsdUnlinkEffectSet(SsdEffectData *effect);
void SsdLinkEffectSet(SsdEffectData *effect);
void SsdSetMidiDevicePriority(SsdMidiDevice *device, unsigned char priority);

/* Creates the effect sequence: track_count tracks, each on its own fixed voice from first_voice. */
SsdSequence *SsdMakeEffectSequenceRec(int track_count, int first_voice)
{
    SsdSequence *sequence = SsdNewPtr(sizeof(*sequence));
    SsdTrack *track = SsdNewPtr(track_count * sizeof(*track));
    int index;

    sequence->device = SsdNewMidiDevice(track_count, SSD_ALLOCATION_FIXED, 0);
    sequence->data = 0;
    SsdInitSequence(sequence);
    SsdSetMidiDevicePriority(sequence->device, 3);
    sequence->signature = SSD_EFFECT_SEQUENCE_SIGNATURE;
    sequence->metadata = 0;
    sequence->sequence_id = SSD_EFFECT_SEQUENCE_ID;
    sequence->priority = 1;
    sequence->mode = 1;
    sequence->tracks = track;
    sequence->track_count = track_count;
    sequence->output_volume = SsdWork.effect_volume;
    sequence->is_effect = 1;
    SsdSetMidiDeviceOutputVolume(sequence->device, sequence->output_volume);
    for (index = 0; index < track_count; ++first_voice) {
        SsdChannel *channel;

        track->flags = 0;
        track->control_flags = 0;
        track->index = index;
        track->midi_channel = index;
        track->voice_index = first_voice;
        track->begin = 0;
        channel = &sequence->device->channels[index];
        track->channel = channel;
        channel->voice_range.first = channel->voice_range.last = first_voice;
        channel->allocation_mode = SSD_ALLOCATION_FIXED;
        ++track;
        ++index;
    }
    return sequence;
}

/*
 * Registers an effect data set and returns its file ID, or -1 on failure.
 * A set whose file ID is already registered is not added again.
 */
int SsdAddEffectData(SsdEffectData *effect)
{
    int error = (short)SsdCheckData(effect, SSD_EFFECT_DATA_SIGNATURE);
    const SsdEffectData *source;
    unsigned int size;

    source = effect;
    if (error == 0) {
        if (SsdCheckEffectData(effect->file_id) >= 0)
            return effect->file_id;
        size = effect->size;
        effect = SsdNewPtr(size);
        SsdMemoryCopy(effect, source, size);
        error = SSD_ERROR_BAD_EFFECT_DATA;
        if (effect->magic == SSD_EFFECT_DATA_SIGNATURE) {
            SsdLinkEffectSet(effect);
            return effect->file_id;
        }
    }
    SsdErrorProc(error, (int)effect);
    return -1;
}

/* Stops the effects of a registered data set, unlinks it and frees its memory. */
void SsdDisposeEffectData(int file_id)
{
    SsdEffectData *effect = SsdGetFileIDToEffectData(file_id);

    if (effect != 0) {
        if (effect->magic != SSD_EFFECT_DATA_SIGNATURE) {
            SsdErrorProc(SSD_ERROR_BAD_EFFECT_DATA, (int)effect);
        } else {
            SsdStopEffectFileID(file_id);
            SsdUnlinkEffectSet(effect);
            SsdDisposePtr(effect);
        }
    }
}

void SsdLinkEffectSet(SsdEffectData *effect)
{
    CpuDisableIntr();
    effect->next = SsdWork.effect_sets;
    SsdWork.effect_sets = effect;
    CpuEnableIntr();
}

/* Removes an effect data set from the linked list; reports an error when it is not listed. */
int SsdUnlinkEffectSet(SsdEffectData *effect)
{
    SsdEffectData *current;
    SsdEffectData *previous;

    CpuDisableIntr();
    current = SsdWork.effect_sets;
    previous = 0;
    while (current != 0) {
        if (current == effect)
            break;
        previous = current;
        current = previous->next;
    }
    if (current == 0) {
        CpuEnableIntr();
        SsdErrorProc(SSD_ERROR_EFFECT_SET_NOT_FOUND, (int)effect);
        return SSD_ERROR_EFFECT_SET_NOT_FOUND;
    }
    if (previous != 0)
        previous->next = effect->next;
    else
        SsdWork.effect_sets = effect->next;
    CpuEnableIntr();
    return 0;
}

/* Returns file_id when an effect data set with that ID is registered, otherwise -1. */
int SsdCheckEffectData(int file_id)
{
    SsdEffectData *effect = SsdWork.effect_sets;

    while (effect != 0) {
        if (effect->file_id == file_id)
            return file_id;
        effect = effect->next;
    }
    return -1;
}

void SsdPlayEffectNormal(unsigned int effect, int mode)
{
    SsdPlayEffectParam(effect, SSD_MIDI_VALUE_MAX, SSD_PANPOT_CENTER, mode);
}

/* Updates the matching playing effects; starts the effect when none of them changed. */
void SsdSetPlayEffectParam(unsigned int effect, int volume, int pan, int mode)
{
    int result = SsdSetEffectParam(effect, volume, pan, mode);

    if (result == 0)
        SsdPlayEffectParam(effect, volume, pan, mode);
}

/*
 * Starts an effect. effect_id holds the file ID in its high 16 bits and the
 * effect index in its low 16 bits.
 */
void SsdPlayEffectParam(unsigned int effect_id, int volume, int pan, int mode)
{
    SsdEffectData *effect = SsdWork.effect_sets;
    unsigned short file_id = effect_id >> 16;
    unsigned short index;
    unsigned int offset;
    unsigned short *offsets;
    unsigned short *selected_offset;
    SsdEffectEntry *entry;
    SsdEffectPlayParam play;
    if (effect == 0) {
        SsdErrorProc(SSD_ERROR_EFFECT_SET_NOT_FOUND, effect_id);
        return;
    }
    while (effect->file_id != file_id) {
        effect = effect->next;
        if (effect == 0) {
            SsdErrorProc(SSD_ERROR_EFFECT_SET_NOT_FOUND, effect_id);
            return;
        }
    }
    index = effect_id;
    if (index >= effect->effect_count) {
        SsdErrorProc(SSD_ERROR_EFFECT_INDEX, effect_id);
        return;
    }
    offsets = effect->effect_offsets;
    if (effect->magic != SSD_EFFECT_DATA_SIGNATURE)
        return;
    selected_offset = offsets + index;
    offset = *selected_offset;
    if (offset == 0)
        return;
    /* SED offset table entries are byte offsets from the file header. */
    entry = (SsdEffectEntry *)((unsigned char *)effect + offset);
    play.track_count = entry->track_count;
    if (play.track_count == 0)
        return;
    play.effect_id = effect_id;
    play.mode = mode;
    play.bank_id = effect->bank_id;
    play.start_track = 0;
    play.first_track = 0;
    play.priority = entry->priority;
    play.volume = volume;
    play.pan = pan;
    play.flags = entry->flags;
    play.effect_data = entry;
    if (SsdSaechSeqFreeTrack(&play) == 0)
        SsdPlaySeqEffectNormal(&play);
}

/*
 * Initializes the tracks of an effect from first_track and activates them.
 * Returns first_track.
 */
int SsdPlaySeqEffectNormal(SsdEffectPlayParam *play)
{
    SsdSequence *sequence;
    SsdTrack *track;
    SsdEffectEntry *entry;
    unsigned short *offsets;
    unsigned int timestamp;
    int mapped_volume;
    int output_volume;
    int remaining;

    if (SsdGetWSPtrBank(play->bank_id) == 0) {
        SsdErrorProc(SSD_ERROR_NO_EFFECT_BANK, play->effect_id);
        return 0;
    }
    remaining = play->track_count;
    if (remaining == 0)
        return 0;
    sequence = SsdWork.effect_sequence;
    track = &sequence->tracks[play->first_track];
    if (track->flags != 0)
        SsdStopEffect(track->effect_id, track->mode);
    entry = play->effect_data;
    timestamp = SsdWork.interrupt_count;
    mapped_volume = SsdVolumeTable[play->volume];
    CpuDisableIntr();
    output_volume = mapped_volume * SSD_CHANNEL_VOLUME_DEFAULT * SSD_MIDI_VALUE_MAX /
        (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
    offsets = entry->track_offsets;
    /* One pass per track; an offset of 0 leaves that track deactivated. */
    do {
        unsigned short offset = *offsets++;

        if (offset != 0) {
            SsdChannel *channel;
            unsigned char *events;
            unsigned short bank_id;

            track->timestamp = timestamp;
            track->effect_id = play->effect_id;
            track->mode = play->mode;
            track->priority = entry->priority;
            track->flags = SSD_TRACK_ACTIVE | SSD_TRACK_REST;
            track->control_flags = 0;
            bank_id = play->bank_id;
            events = (unsigned char *)entry + offset;
            track->begin = events;
            track->stream = events;
            track->jump = 0;
            track->gate = 0;
            track->gate_ticks = 0;
            track->gate_state = 0;
            track->wait = 0;
            track->delta = 0;
            track->duration = 0;
            track->gate_ratio = SSD_TRACK_GATE_RATIO_DEFAULT;
            track->octave = SSD_NOTE_MIDDLE_C;
            track->note = SSD_NOTE_MIDDLE_C;
            track->last_note = SSD_NOTE_MIDDLE_C;
            track->velocity.remaining = 0;
            track->bank_id = bank_id;
            track->velocity.current.value = entry->volume << 16;
            track->pitch.remaining = 0;
            track->pitch.current.value = 0;
            track->volume.current.value = SSD_FIXED_VOLUME_DEFAULT;
            track->volume.remaining = 0;
            track->pan.current.value = SSD_PANPOT_CENTER_FIXED;
            track->pan.remaining = 0;
            track->loop_depth = SSD_TRACK_LOOP_NONE;
            channel = track->channel;
            SsdResetChannel(channel, play->bank_id);
            SsdSetChannelWave(channel, 0);
            channel->fader_note = 0;
            channel->fader_volume = mapped_volume;
            channel->fader_panpot = play->pan;
            channel->channel_volume = SSD_CHANNEL_VOLUME_DEFAULT;
            channel->volume = output_volume;
        } else {
            track->flags = 0;
        }
        --remaining;
        ++track;
    } while (remaining != 0);
    sequence->flags = SSD_SEQUENCE_PLAYING;
    CpuEnableIntr();
    return play->first_track;
}

void SsdPlayWaveNormal(unsigned int wave, int mode)
{
    SsdPlayWaveParam(wave, SSD_MIDI_VALUE_MAX, SSD_PANPOT_CENTER, mode);
}

void SsdPlayWaveParam(unsigned int wave, int volume, int pan, int mode)
{
    SsdEffectPlayParam play;
    int result;

    play.effect_id = wave;
    play.mode = mode;
    play.bank_id = wave >> 16;
    play.start_track = wave;
    play.track_count = 1;
    play.priority = 0;
    play.volume = volume;
    play.pan = pan;
    play.flags = 0;
    play.effect_data = 0;
    result = SsdSaechSeqFreeTrack(&play);
    if (result == 0)
        SsdPlayWaveEffectNormal(&play);
}

void SsdStopWave(int effect, int mode)
{
    SsdStopEffect(effect, mode);
}

/* Starts one track on sample start_track of the bank and returns it, or 0 on error. */
SsdTrack *SsdPlayWaveEffectNormal(SsdEffectPlayParam *play)
{
    SsdWaveBank *bank = SsdGetWSPtrBank(play->bank_id);
    SsdSequence *sequence;
    SsdTrack *track;
    SsdChannel *channel;
    int sample_index;
    int mapped_volume;
    unsigned short bank_id;
    unsigned int timestamp;
    int output_volume;
    int invalid_sample;

    /* A missing bank, or a sample that is out of range or silent, fails the request. */
    if (bank == 0) {
        SsdErrorProc(SSD_ERROR_NO_EFFECT_BANK, play->effect_id);
        return 0;
    }
    sample_index = play->start_track;
    invalid_sample = 1;
    if (bank->sample_count >= sample_index) {
        SsdWaveSample *sample = bank->samples + sample_index;
        invalid_sample = sample->volume == 0;
    }
    if (invalid_sample) {
        SsdErrorProc(SSD_ERROR_BAD_EFFECT_SAMPLE, play->effect_id);
        return 0;
    }
    if (play->track_count == 0)
        return 0;
    sequence = SsdWork.effect_sequence;
    track = &sequence->tracks[play->first_track];
    if (track->flags != 0)
        SsdStopEffect(track->effect_id, track->mode);
    mapped_volume = SsdVolumeTable[play->volume];
    CpuDisableIntr();
    timestamp = SsdWork.interrupt_count;
    channel = track->channel;
    track->timestamp = timestamp;
    track->effect_id = play->effect_id;
    track->mode = play->mode;
    track->priority = 0;
    track->control_flags = 0;
    bank_id = play->bank_id;
    track->gate_ratio = SSD_TRACK_GATE_RATIO_DEFAULT;
    track->octave = SSD_NOTE_MIDDLE_C;
    track->note = SSD_NOTE_MIDDLE_C;
    track->last_note = SSD_NOTE_MIDDLE_C;
    track->velocity.current.value = SSD_TRACK_VELOCITY_DEFAULT;
    track->volume.current.value = SSD_FIXED_VOLUME_DEFAULT;
    track->pan.current.value = SSD_PANPOT_CENTER_FIXED;
    track->loop_depth = SSD_TRACK_LOOP_NONE;
    track->begin = 0;
    track->stream = 0;
    track->jump = 0;
    track->gate = 0;
    track->gate_ticks = 0;
    track->gate_state = 0;
    track->wait = 0;
    track->delta = 0;
    track->duration = 0;
    track->velocity.remaining = 0;
    track->volume.remaining = 0;
    track->pan.remaining = 0;
    track->pitch.remaining = 0;
    track->pitch.current.value = 0;
    track->flags = SSD_TRACK_ACTIVE | SSD_TRACK_REST | 0x20;
    track->bank_id = bank_id;
    SsdResetChannel(channel, play->bank_id);
    SsdSetChannelWave(channel, sample_index);
    output_volume = mapped_volume * SSD_CHANNEL_VOLUME_DEFAULT * SSD_MIDI_VALUE_MAX /
        (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
    channel->fader_note = 0;
    channel->fader_volume = mapped_volume;
    channel->fader_panpot = play->pan;
    channel->channel_volume = SSD_CHANNEL_VOLUME_DEFAULT;
    channel->volume = output_volume;
    sequence->flags |= SSD_SEQUENCE_PLAYING;
    CpuEnableIntr();
    return track;
}

/* Updates volume and pan on the active tracks of effect_id (and mode, when nonzero). Returns 1 if any changed. */
int SsdSetEffectParam(unsigned int effect_id, int volume, int pan, int mode)
{
    SsdSequence *sequence = SsdWork.effect_sequence;
    int changed = 0;
    SsdTrack *track = sequence->tracks;
    int remaining = sequence->track_count;
    int mapped_volume = SsdVolumeTable[volume];
    SsdChannel *channel;

    CpuDisableIntr();
    if (mode == 0) {
        do {
            if ((track->flags & SSD_TRACK_ACTIVE) && track->effect_id == effect_id) {
                int update_events = SSD_VOICE_EVENT_PITCH | SSD_VOICE_EVENT_VOLUME;
                channel = track->channel;
                changed = 1;
                channel->fader_volume = mapped_volume;
                channel->fader_panpot = pan;
                channel->volume = channel->channel_volume * channel->expression *
                                  mapped_volume /
                                  (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
                SsdSetChannelVoiceEvent(channel, update_events);
            }
            ++track;
        } while (--remaining != 0);
    } else {
        do {
            if ((track->flags & SSD_TRACK_ACTIVE) && track->effect_id == effect_id &&
                track->mode == mode) {
                int update_events = SSD_VOICE_EVENT_PITCH | SSD_VOICE_EVENT_VOLUME;
                channel = track->channel;
                changed = 1;
                channel->fader_volume = mapped_volume;
                channel->fader_panpot = pan;
                channel->volume = channel->channel_volume * channel->expression *
                                  mapped_volume /
                                  (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
                SsdSetChannelVoiceEvent(channel, update_events);
            }
            ++track;
        } while (--remaining != 0);
    }
    CpuEnableIntr();
    return changed;
}

/*
 * Finds play->track_count consecutive free tracks and sets play->first_track.
 * Stops the same effect first. When no run is free, stops the oldest track
 * whose priority does not exceed the request. Returns -1 if none can be stopped.
 */
int SsdSaechSeqFreeTrack(SsdEffectPlayParam *play)
{
    SsdSequence *sequence;
    int needed;
    int search_limit;
    int first_index;
    SsdTrack *first;

    if (play->flags & SSD_EFFECT_FLAG_STOP_ANY_MODE)
        SsdStopEffect(play->effect_id, 0);
    else
        SsdStopEffect(play->effect_id, play->mode);
    sequence = SsdWork.effect_sequence;
    needed = play->track_count;
    if (play->flags & SSD_EFFECT_FLAG_LOW_TRACKS) {
        search_limit = SSD_EFFECT_LOW_TRACK_COUNT - needed;
        first_index = search_limit;
    } else {
        first_index = sequence->track_count - needed;
        search_limit = first_index - SSD_EFFECT_LOW_TRACK_COUNT;
    }
    first = &sequence->tracks[first_index];
    /* Search for a free run; if there is none, stop a victim track and search again. */
    for (;;) {
        int position = search_limit;
        SsdTrack *track = first;
        SsdTrack *victim;
        unsigned int oldest_time;

        while (position >= 0) {
            int remaining = needed;

            if (track->flags == 0) {
                SsdTrack *next = track + 1;

                --remaining;
                while (remaining != 0) {
                    if (next->flags != 0)
                        break;
                    --remaining;
                    ++next;
                }
                if (remaining == 0) {
                    play->first_track = track->index;
                    return 0;
                }
            }
            position -= remaining;
            track -= remaining;
        }
        track = 0;
        oldest_time = ~0U;
        position = sequence->track_count;
        victim = &sequence->tracks[position - 1];
        while (position != 0) {
            if (victim->flags != 0 && victim->timestamp < oldest_time &&
                victim->priority <= play->priority) {
                oldest_time = victim->timestamp;
                track = victim;
            }
            --position;
            --victim;
        }
        if (track == 0)
            return -1;
        SsdStopEffect(track->effect_id, track->mode);
    }
}

void SsdStopAllEffect(void)
{
    SsdSequence *sequence = SsdWork.effect_sequence;
    SsdTrack *track = sequence->tracks;
    int remaining = sequence->track_count;

    CpuDisableIntr();
    while (remaining != 0) {
        --remaining;
        if (track->flags & SSD_TRACK_ACTIVE) {
            SsdCutoffChannel(track->channel);
            track->flags = 0;
        }
        ++track;
    }
    CpuEnableIntr();
}

void SsdStopEffect(int effect, int mode)
{
    SsdFadeoutEffect(effect, 0, mode);
}

/* Fades out every active track that is not playing effect_id (and mode, when nonzero). */
void SsdContinueOneEffect(unsigned int effect_id, int duration, int mode)
{
    SsdSequence *sequence = SsdWork.effect_sequence;
    SsdTrack *track = sequence->tracks;
    int remaining = sequence->track_count;

    CpuDisableIntr();
    if (mode == 0) {
        while (remaining != 0) {
            --remaining;
            if ((track->flags & SSD_TRACK_ACTIVE) && track->effect_id != effect_id)
                SsdFadeoutStopEffectTrack(track, duration);
            ++track;
        }
    } else {
        while (remaining != 0) {
            --remaining;
            if ((track->flags & SSD_TRACK_ACTIVE) &&
                (track->effect_id != effect_id || track->mode != mode))
                SsdFadeoutStopEffectTrack(track, duration);
            ++track;
        }
    }
    CpuEnableIntr();
}

void SsdStopEffectFileID(int file_id)
{
    SsdFadeoutEffectFileID(file_id, 0);
}

/* Fades out the active tracks of effect_id (and mode, when nonzero). */
void SsdFadeoutEffect(unsigned int effect_id, int duration, int mode)
{
    SsdSequence *sequence = SsdWork.effect_sequence;
    SsdTrack *track = sequence->tracks;
    int remaining = sequence->track_count;

    CpuDisableIntr();
    if (mode == 0) {
        while (remaining != 0) {
            --remaining;
            if ((track->flags & SSD_TRACK_ACTIVE) && track->effect_id == effect_id)
                SsdFadeoutStopEffectTrack(track, duration);
            ++track;
        }
    } else {
        while (remaining != 0) {
            --remaining;
            if ((track->flags & SSD_TRACK_ACTIVE) && track->effect_id == effect_id &&
                track->mode == mode)
                SsdFadeoutStopEffectTrack(track, duration);
            ++track;
        }
    }
    CpuEnableIntr();
}

/* Fades out the active tracks that belong to file_id. */
void SsdFadeoutEffectFileID(int file_id, int duration)
{
    SsdSequence *sequence = SsdWork.effect_sequence;
    SsdTrack *track = sequence->tracks;
    int remaining = sequence->track_count;

    CpuDisableIntr();
    while (remaining != 0) {
        --remaining;
        if ((track->flags & SSD_TRACK_ACTIVE) && (track->effect_id >> 16) == file_id)
            SsdFadeoutStopEffectTrack(track, duration);
        ++track;
    }
    CpuEnableIntr();
}

/* Fades a track out over duration, or cuts it off at once when duration is 0. */
void SsdFadeoutStopEffectTrack(SsdTrack *track, int duration)
{
    if (duration != 0) {
        int step = track->velocity.current.value / duration;

        track->control_flags |= SSD_TRACK_CONTROL_FADE_OUT;
        track->velocity.target = 0;
        track->velocity.remaining = duration;
        track->velocity.step = -step;
    } else {
        track->flags = 0;
        SsdCutoffChannel(track->channel);
    }
}

int SsdCheckPlayEffectAll(void)
{
    SsdSequence *sequence = SsdWork.effect_sequence;
    SsdTrack *track = sequence->tracks;
    int remaining = sequence->track_count;

    do {
        if (track->flags & SSD_TRACK_ACTIVE)
            return 1;
        --remaining;
        ++track;
    } while (remaining != 0);
    return 0;
}

/* Returns 1 when effect_id is playing (and mode matches, when nonzero), otherwise 0. */
int SsdCheckPlayEffect(unsigned int effect_id, int mode)
{
    SsdSequence *sequence = SsdWork.effect_sequence;
    SsdTrack *track = sequence->tracks;
    int remaining = sequence->track_count;
    switch (mode) {
    case 0:
        do {
            if ((track->flags & SSD_TRACK_ACTIVE) && track->effect_id == effect_id)
                return 1;
            --remaining;
            ++track;
        } while (remaining != 0);
        break;
    default:
    do {
        if ((track->flags & SSD_TRACK_ACTIVE) &&
            track->effect_id == effect_id && track->mode == mode)
            return 1;
        ++track;
        --remaining;
    } while (remaining != 0);
    break;
    }
    return 0;
}

int SsdCheckPlayEffectFileID(int file_id)
{
    SsdSequence *sequence = SsdWork.effect_sequence;
    SsdTrack *track = sequence->tracks;
    int remaining = sequence->track_count;

    do {
        if ((track->flags & SSD_TRACK_ACTIVE) &&
            (track->effect_id >> 16) == file_id)
            return 1;
        --remaining;
        ++track;
    } while (remaining != 0);
    return 0;
}

SsdEffectData *SsdGetFileIDToEffectData(int file_id)
{
    SsdEffectData *effect = SsdWork.effect_sets;

    while (effect != 0) {
        if (effect->file_id == file_id)
            break;
        effect = effect->next;
    }
    return effect;
}
