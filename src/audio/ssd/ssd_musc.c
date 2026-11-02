#include "ssd/ssd_musc.h"
#include "ssd/ssd_chan.h"
#include "ssd/ssd_err.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_mem.h"
#include "ssd/ssd_seq.h"
#include "ssd/ssd_wave.h"

void SsdInitMusicTrackRec(SsdSequence *sequence);
int SsdGetSeqWorkSize(SsdMusicData *data);
int SsdCheckMidiChannel(SsdSequence *sequence);
void SsdSetDataAddress(SsdSequence *sequence);

int SsdPlaySequence(void *data, int volume, int fade)
{
    int sequenceId = SsdAddSequenceData(data);
    if (sequenceId >= 0) {
        SsdStartSequence(sequenceId, volume, fade);
    }
    return sequenceId;
}

int SsdAddSeqEffectData(struct SsdMusicData *data)
{
    int sequenceId = SsdAddSequenceData(data);
    if (sequenceId >= 0) {
        SsdSequence *sequence = SsdGetSequencePtr(sequenceId);
        sequence->output_volume = SsdWork.effect_volume;
        sequence->is_effect = 1;
        SsdSetMidiDeviceOutputVolume(sequence->device, sequence->output_volume);
    }
    return sequenceId;
}

int SsdAddSequenceData(SsdMusicData *data)
{
    SsdMusicData *source;
    SsdSequence *sequence;
    int sequenceId;
    int size;
    int channels;
    short error = SsdCheckData(data, SSD_MUSIC_DATA_SIGNATURE);

    source = data;
    if (error != 0) {
        SsdErrorProc(error, (int)data);
        return error;
    }
    sequenceId = data->file_id;
    if (SsdGetSequencePtr(sequenceId) != 0) {
        SsdWarringProc(SSD_WARNING_SEQUENCE_EXISTS, sequenceId);
        return SSD_WARNING_SEQUENCE_EXISTS;
    }
    size = data->size;
    data = SsdNewPtr(size);
    SsdMemoryCopy(data, source, size);
    size = SsdGetSeqWorkSize(data);
    if (SsdGetMemoryFreeSize() < size) {
        SsdErrorProc(SSD_ERROR_NO_MEMORY, (int)data);
        return SSD_ERROR_NO_MEMORY;
    }
    sequence = SsdNewPtr(sizeof(SsdSequence));
    if (sequence == 0) {
        SsdErrorProc(SSD_ERROR_NO_SEQUENCE_MEMORY, (int)data);
        return SSD_ERROR_NO_SEQUENCE_MEMORY;
    }
    sequence->signature = SSD_MUSIC_SEQUENCE_SIGNATURE;
    sequence->next = 0;
    sequence->data = data;
    sequence->tracks = 0;
    sequence->tracks = SsdNewPtr(data->track_count * sizeof(SsdTrack));
    if (sequence->tracks == 0) {
        SsdErrorProc(SSD_ERROR_NO_TRACK_MEMORY, (int)data);
        SsdDisposeSequenceBuffer(sequence);
        return SSD_ERROR_NO_TRACK_MEMORY;
    }
    channels = data->midi_channel_count;
    if (channels == 0) {
        channels = SsdCheckMidiChannel(sequence);
    }
    sequence->device = SsdNewMidiDevice(channels, SSD_ALLOCATION_SEARCH, &data->settings.voice_range);
    if (sequence->device == 0) {
        SsdErrorProc(SSD_ERROR_NO_MIDI_DEVICE, (int)data);
        SsdDisposeSequenceBuffer(sequence);
        return SSD_ERROR_NO_MIDI_DEVICE;
    }
    SsdSetDataAddress(sequence);
    SsdInitSequence(sequence);
    SsdSetSeqReverb(sequence);
    sequence->priority = data->priority;
    sequence->mode = 0;
    sequence->output_volume = SsdWork.sequence_volume;
    sequence->is_effect = 0;
    SsdSetMidiDeviceOutputVolume(sequence->device, sequence->output_volume);
    sequence->stop_callback = 0;
    sequence->signal = 0;
    sequence->unmodeled_c0 = 0;
    SsdLinkSequence(sequence);
    return sequenceId;
}

int SsdDisposeSequence(int sequenceId)
{
    SsdSequence *sequence = SsdGetSequencePtr(sequenceId);
    short error;

    if (sequence == 0) {
        SsdErrorProc(SSD_ERROR_SEQUENCE_NOT_FOUND, sequenceId);
        return SSD_ERROR_SEQUENCE_NOT_FOUND;
    }
    error = SsdCheckData(sequence->data, SSD_MUSIC_DATA_SIGNATURE);
    if (error != 0) {
        SsdErrorProc(error, sequenceId);
        return error;
    }
    if (sequence->flags & SSD_SEQUENCE_PLAYING) {
        SsdStopSequence(sequenceId);
    }
    SsdUnlinkSequence(sequence);
    SsdDisposeSequenceBuffer(sequence);
    return 0;
}

int SsdCheckSequence(int sequenceId)
{
    SsdSequence *sequence = SsdWork.sequences;
    while (sequence != 0) {
        if (sequence->sequence_id == sequenceId) {
            return sequenceId;
        }
        sequence = sequence->next;
    }
    return SSD_ERROR_SEQUENCE_NOT_FOUND;
}

void SsdStartSequence(int sequenceId, int volume, int fade)
{
    SsdSequence *sequence = SsdGetSequencePtr(sequenceId);
    short error;

    if (sequence == 0) {
        SsdErrorProc(SSD_ERROR_NO_SEQUENCE, 0);
        return;
    }
    if (sequence->flags & SSD_SEQUENCE_PLAYING) {
        SsdStopSequence(sequenceId);
    }
    error = SsdCheckSumMusicData(sequence->data);
    if (error != 0) {
        SsdErrorProc(error, (int)sequence->data);
        return;
    }
    SsdInitSequence(sequence);
    SsdInitMusicTrackRec(sequence);
    sequence->master_volume.current.value = 0;
    sequence->flags |= SSD_SEQUENCE_STOPPED;
    SsdSetSeqMasterVolume(sequenceId, volume, fade);
}

void SsdResumeSequence(int sequenceId, int volume, int fade)
{
    if (SsdGetSequencePtr(sequenceId) == 0) {
        SsdErrorProc(SSD_ERROR_NO_SEQUENCE, 0);
    } else {
        SsdSetSeqMasterVolume(sequenceId, volume, fade);
    }
}

void SsdStopSequence(int sequenceId)
{
    struct SsdSequence *sequence = SsdGetSequencePtr(sequenceId);
    if (sequence == 0) {
        SsdErrorProc(SSD_ERROR_NO_SEQUENCE, 0);
    } else {
        SsdStoppingSequence(sequence);
    }
}

void SsdPauseSequence(int sequenceId, int fade)
{
    SsdSequence *sequence = SsdGetSequencePtr(sequenceId);
    if (sequence == 0) {
        SsdErrorProc(SSD_ERROR_NO_SEQUENCE, 0);
    } else if (!(sequence->flags & SSD_SEQUENCE_STOPPED)) {
        SsdSetSeqMasterVolume(sequenceId, 0, fade);
    }
}

void SsdInitMusicTrackRec(SsdSequence *sequence)
{
    SsdTrack *track = sequence->tracks;
    int index;
    int *timestamp;
    int initialGateRatio;
    int initialKey;
    int initialVelocity;
    int initialVolume;
    int initialPan;
    int initialLoopDepth;
    SsdGetWSPtrBank(sequence->bank);
    index = 0;
    timestamp = &SsdWork.interrupt_count;
    initialGateRatio = SSD_TRACK_GATE_RATIO_DEFAULT;
    initialKey = SSD_NOTE_MIDDLE_C;
    initialVelocity = SSD_TRACK_MUSIC_VELOCITY_DEFAULT;
    initialVolume = SSD_FIXED_VOLUME_DEFAULT;
    initialPan = SSD_PANPOT_CENTER_FIXED;
    initialLoopDepth = SSD_TRACK_LOOP_NONE;

    do {
        unsigned short flags = track->flags;
        track->timestamp = *timestamp;
        track->effect_id = 0;
        track->priority = 0;
        flags |= SSD_TRACK_ACTIVE | SSD_TRACK_REST | track->attributes;
        track->flags = flags;
        track->control_flags = 0;
        track->bank_id = sequence->bank;
        track->stream = track->begin;
        track->jump = 0;
        track->gate = 0;
        track->gate_ticks = 0;
        track->gate_state = 0;
        track->wait = 0;
        track->delta = 0;
        track->duration = 0;
        track->gate_ratio = initialGateRatio;
        track->octave = initialKey;
        track->note = initialKey;
        track->last_note = initialKey;
        track->velocity.remaining = 0;
        track->velocity.current.value = initialVelocity;
        track->pitch.remaining = 0;
        track->pitch.current.value = 0;
        track->volume.remaining = 0;
        track->volume.current.value = initialVolume;
        track->pan.remaining = 0;
        track->pan.current.value = initialPan;
        track->loop_depth = initialLoopDepth;
        track++;
        index++;
    } while (index < sequence->track_count);
}

int SsdCheckMidiChannel(SsdSequence *sequence)
{
    int scanning = 1;
    int highestChannel = 0;
    SsdMusicChunk *chunk = sequence->data->chunks;

    do {
        if (chunk->type != SSD_CHUNK_END) {
            if (chunk->type == SSD_CHUNK_TRACK) {
                if (highestChannel < chunk->body.track.midi_channel) {
                    highestChannel = chunk->body.track.midi_channel;
                }
            }
        } else {
            scanning = 0;
        }
        chunk = (SsdMusicChunk *)((unsigned char *)chunk + chunk->size);
    } while (scanning);
    return highestChannel + 1;
}

void SsdSetDataAddress(SsdSequence *sequence)
{
    int scanning = 1;
    int trackIndex = 0;
    SsdMusicChunk *chunk = sequence->data->chunks;
    SsdTrack *track = sequence->tracks;

    do {
        switch (chunk->type) {
        case SSD_CHUNK_METADATA:
            sequence->metadata = (char *)chunk->body.track.parameters;
            break;
        case SSD_CHUNK_TRACK:
            track->index = trackIndex;
            track->begin = chunk->body.track.events;
            track->midi_channel = chunk->body.track.midi_channel;
            track->channel = &sequence->device->channels[chunk->body.track.midi_channel];
            track->attributes = chunk->body.track.attributes;
            trackIndex++;
            track->flags = 0;
            track++;
            break;
        case SSD_CHUNK_PATCH:
            sequence->patch_chunk = chunk;
            break;
        case SSD_CHUNK_END:
            scanning = 0;
            break;
        }
        chunk = (SsdMusicChunk *)((unsigned char *)chunk + chunk->size);
    } while (scanning);
}

int SsdGetSeqWorkSize(SsdMusicData *data)
{
    if (data->signature != SSD_MUSIC_DATA_SIGNATURE) {
        return -1;
    }
    return data->track_count * sizeof(SsdTrack) + 292;
}
