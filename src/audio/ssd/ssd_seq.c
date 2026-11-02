#include "ssd/ssd_seq.h"
#include "ssd/ssd_chan.h"
#include "ssd/ssd_dma.h"
#include "ssd/ssd_err.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_mem.h"
#include "ssd/ssd_musc.h"
#include "ssd/ssd_out.h"

/* Called with two more arguments than the definition in ssd_chan.c takes. */
void SsdSetMidiDevicePriority(SsdMidiDevice *device, int priority, ...);

void SsdGetTimeCode(unsigned int ticks, SsdTimeCode *timeCode);
void SsdStartingSequence(SsdSequence *sequence);
void SsdGetSeqPlayTime(int sequence_id, SsdTimeCode *out);
void SsdSetSeqPanpotFader(int id, int track, signed char fader);
void SsdSetSeqStopCallback(int sequence_id, SsdSequenceStopCallback callback);
SsdSequence *SsdGetSeqLyric(int sequenceId);
SsdSequence *SsdSetSeqABRepeat(int sequenceId);
void SsdGetPlayMeasure(int sequence_id);
void SsdSetSeqNoteFader(int id, int track, short fader);
int SsdGetSeqCuePoint(void);

void SsdInitSequence(SsdSequence *sequence)
{
    SsdMusicData *data = sequence->data;
    sequence->active_tracks = 0;
    sequence->flags = 0;
    if (data != 0) {
        int note;
        int pan;
        sequence->sequence_id = data->file_id;
        sequence->bank = data->bank;
        sequence->track_count = data->track_count;
        note = data->settings.master.note;
        pan = data->settings.master.pan;
        sequence->note = note;
        sequence->pan = pan;
        sequence->reverb_mode = data->reverb_mode;
        sequence->reverb_depth = sequence->reverb_target_depth = data->reverb_depth << 8;
        sequence->reverb_delay = data->reverb_parameters[0];
        sequence->reverb_feedback = data->reverb_parameters[1];
        sequence->options = data->options;
        SsdSetMidiDevicePriority(sequence->device, data->channel_priority, note, pan);
    } else {
        sequence->sequence_id = 0;
        sequence->track_count = 0;
        sequence->bank = 0;
    }
    sequence->tempo.value = SSD_TEMPO_DEFAULT << 16;
    sequence->noise_clock = 0;
    sequence->tempo_duration = 0;
    sequence->clock = 0;
    sequence->clock_step = 25165;
    sequence->interrupts = 0;
    sequence->play_ticks = 0;
    sequence->play_time = 0;
    sequence->unmodeled_58 = 0;
    sequence->measure = 0;
    sequence->beat = 0;
    sequence->beat_tick = 0;
    sequence->beats_per_measure = 4;
    sequence->ticks_per_beat = 96;
    sequence->time_denominator = 4;
    sequence->time_numerator = 4;
    sequence->timing_mode = 0;
    sequence->master_tempo.current.value = SSD_MASTER_TEMPO_NORMAL << 16;
    sequence->master_tempo.remaining = 0;
    sequence->master_note.current.value = 0;
    sequence->master_note.remaining = 0;
    sequence->master_volume.current.value = SSD_MIDI_VALUE_MAX << 16;
    sequence->master_volume.remaining = 0;
    sequence->master_pan.current.value = 0;
    sequence->master_pan.remaining = 0;
    SsdResetMidiDeviceParam(sequence->device, sequence->bank);
    SsdResetChannelAll(sequence->device, sequence->bank);
    sequence->random_state = SsdGetRSRandom();
}

void SsdLinkSequence(SsdSequence *sequence)
{
    CpuDisableIntr();
    sequence->next = SsdWork.sequences;
    SsdWork.sequences = sequence;
    CpuEnableIntr();
}

int SsdUnlinkSequence(SsdSequence *sequence)
{
    SsdSequence *current;
    SsdSequence *previous;

    CpuDisableIntr();
    current = SsdWork.sequences;
    previous = 0;
    while (current != 0) {
        if (current == sequence) {
            break;
        }
        previous = current;
        current = previous->next;
    }
    if (current == 0) {
        CpuEnableIntr();
        SsdErrorProc(SSD_ERROR_SEQUENCE_NOT_FOUND, (int)sequence);
        return SSD_ERROR_SEQUENCE_NOT_FOUND;
    }
    if (previous != 0) {
        previous->next = sequence->next;
    } else {
        SsdWork.sequences = sequence->next;
    }
    CpuEnableIntr();
    return 0;
}

void SsdDisposeSequenceBuffer(SsdSequence *sequence)
{
    if (sequence->tracks != 0)
        SsdDisposePtr(sequence->tracks);
    if (sequence->data != 0)
        SsdDisposePtr(sequence->data);
    if (sequence->device != 0)
        SsdDisposeMidiDevice(sequence->device);
    SsdDisposePtr(sequence);
}

int SsdGetSeqPlayStatus(int sequenceId)
{
    SsdSequence *sequence = SsdGetSequencePtr(sequenceId);
    int flags = 0;
    if (sequence == 0) {
        flags = SSD_ERROR_SEQUENCE_NOT_FOUND;
    } else if (sequence->flags & SSD_SEQUENCE_PLAYING) {
        flags = SSD_SEQ_STATUS_PLAYING;
        if (sequence->flags & SSD_SEQUENCE_STOPPED) {
            flags = SSD_SEQ_STATUS_STOPPED;
        }
        if (sequence->master_tempo.remaining != 0) {
            flags |= SSD_SEQ_STATUS_TEMPO_RAMP;
        }
        if (sequence->master_note.remaining != 0) {
            flags |= SSD_SEQ_STATUS_NOTE_RAMP;
        }
        if (sequence->master_volume.remaining != 0) {
            flags |= SSD_SEQ_STATUS_VOLUME_RAMP;
        }
        if (sequence->master_pan.remaining != 0) {
            flags |= SSD_SEQ_STATUS_PAN_RAMP;
        }
    }
    return flags;
}

void SsdSetSeqMasterTempo(int id, int bpm, int ticks)
{
    SsdSequence *sequence = SsdGetSequencePtr(id);
    int difference;
    if (sequence == 0)
        return;
    if (bpm == 0)
        bpm = SSD_MASTER_TEMPO_NORMAL;
    CpuDisableIntr();
    sequence->master_tempo.target = bpm;
    if (ticks == 0) {
        sequence->master_tempo.current.value = bpm << 16;
        sequence->clock_step = (((sequence->tempo.parts.integer * SSD_TEMPO_CLOCK_SCALE) >> 8)
                                * sequence->master_tempo.current.parts.integer) >> 8;
        sequence->master_tempo.remaining = 0;
    } else {
        difference = (bpm << 16) -sequence->master_tempo.current.value;
        if (difference != 0) {
            sequence->master_tempo.remaining = ticks;
            sequence->master_tempo.step = difference / ticks;
        }
    }
    CpuEnableIntr();
}

void SsdSetSeqMasterNote(int sequenceId, int note, int duration)
{
    SsdSequence *sequence = SsdGetSequencePtr(sequenceId);
    int target = note << 16;
    if (sequence != 0) {
        sequence->master_note.target = target;
        if (duration == 0) {
            sequence->master_note.current.value = target;
            sequence->master_note.remaining = 0;
            SsdSetMidiDeviceNote(sequence->device, note);
        } else {
            int difference = target - sequence->master_note.current.value;
            if (difference != 0) {
                sequence->master_note.step = difference / duration;
                sequence->master_note.remaining = duration;
            }
        }
    }
}

void SsdSetSeqMasterVolume(int id, int volume, int ticks)
{
    SsdSequence *sequence = SsdGetSequencePtr(id);
    int target;
    int difference;
    if (sequence == 0)
        return;
    if (!(sequence->flags & (SSD_SEQUENCE_PLAYING | SSD_SEQUENCE_STOPPED)))
        return;
    target = volume << 16;
    sequence->master_volume.target = target;
    if (ticks == 0) {
        sequence->master_volume.current.value = target;
        sequence->master_volume.remaining = 0;
        SsdSetMidiDeviceVolume(sequence->device, volume);
    } else {
        difference = target - sequence->master_volume.current.value;
        if (difference == 0)
            return;
        sequence->master_volume.remaining = ticks;
        sequence->master_volume.step = difference / ticks;
    }
    if (volume != 0) {
        SsdStartingSequence(sequence);
    } else {
        sequence->flags |= SSD_SEQUENCE_STOP_PENDING;
        if (ticks == 0)
            SsdStoppingSequence(sequence);
    }
}

void SsdSetSeqMasterPanpot(int sequenceId, int pan, int duration)
{
    SsdSequence *sequence = SsdGetSequencePtr(sequenceId);
    int target = pan << 16;
    if (sequence != 0) {
        sequence->master_pan.target = target;
        if (duration == 0) {
            sequence->master_pan.current.value = target;
            sequence->master_pan.remaining = 0;
            SsdSetMidiDevicePanpot(sequence->device, pan);
        } else {
            int difference = target - sequence->master_pan.current.value;
            if (difference != 0) {
                sequence->master_pan.step = difference / duration;
                sequence->master_pan.remaining = duration;
            }
        }
    }
}

void SsdSetSeqReverb(SsdSequence *sequence)
{
    int first;
    int second;
    SsdSpuDmaCompleted(1);
    second = 0;
    first = 0;
    if (sequence->options & SSD_SEQUENCE_REVERB_CORE0) {
        first = SsdSetReverbParameter(sequence->reverb_mode, sequence->reverb_depth,
                                     sequence->reverb_delay, sequence->reverb_feedback);
    }
    if (sequence->options & SSD_SEQUENCE_REVERB_CORE1) {
        second = SsdSetReverbParameter(sequence->reverb_mode + SSD_REVERB_MODE_CORE1, sequence->reverb_depth,
                                      sequence->reverb_delay, sequence->reverb_feedback);
    }
    if (first != 0) {
        SsdReverbClearCompleted(0, 1);
        SsdEnableReverbParam(0);
    }
    if (second != 0) {
        SsdReverbClearCompleted(1, 1);
        SsdEnableReverbParam(1);
    }
}

void SsdSetSeqMute(int sequenceId, int muteMask)
{
    SsdSequence *sequence;
    sequence = SsdGetSequencePtr(sequenceId);
    if (sequence != 0) {
        int trackIndex = 0;
        SsdTrack *track = sequence->tracks + 1;
        int remaining = sequence->track_count - 1;
        if (remaining != 0) {
            int firstTrackBit = 1;
            do {
                unsigned short flagBits = track->flags;
                remaining--;
                if (track->flags != 0) {
                    if (muteMask & (firstTrackBit << trackIndex)) {
                        if (!(flagBits & SSD_TRACK_MUTED)) {
                            track->flags = flagBits | SSD_TRACK_MUTED;
                            track->attributes |= SSD_TRACK_MUTED;
                            SsdCutoffChannel(track->channel);
                        }
                    } else if (flagBits & SSD_TRACK_MUTED) {
                        unsigned int unmutedFlags = flagBits & 0xfffd;
                        track->flags = unmutedFlags;
                        track->attributes &= ~SSD_TRACK_MUTED;
                    }
                } else {
                    if (muteMask & (firstTrackBit << trackIndex)) {
                        track->attributes |= SSD_TRACK_MUTED;
                    } else {
                        track->attributes &= ~SSD_TRACK_MUTED;
                    }
                }
                trackIndex++;
                track++;
            } while (remaining != 0);
        }
    }
}

void SsdSetSeqNoteFader(int id, int track, short fader)
{
    SsdSequence *sequence = SsdGetSequencePtr(id);
    SsdTrack *record;
    SsdChannel *channel;
    if (sequence != 0) {
        record = &sequence->tracks[track];
        channel = record->channel;
        channel->fader_note = fader;
        SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_PITCH);
    }
}

void SsdSetSeqVolumeFader(int sequenceId, int trackIndex, int volume)
{
    SsdSequence *sequence = SsdGetSequencePtr(sequenceId);
    if (sequence != 0) {
        SsdTrack *track = sequence->tracks + trackIndex;
        SsdChannel *channel = track->channel;
        int output = channel->channel_volume * channel->expression * volume /
            (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
        channel->fader_volume = volume;
        channel->volume = output;
        SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_VOLUME);
    }
}

void SsdSetSeqPanpotFader(int id, int track, signed char fader)
{
    SsdSequence *sequence = SsdGetSequencePtr(id);
    SsdTrack *record;
    SsdChannel *channel;
    if (sequence != 0) {
        record = &sequence->tracks[track];
        channel = record->channel;
        channel->fader_panpot = fader;
        SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_VOLUME);
    }
}

void SsdSetSeqPatchNumber(int sequenceId, int patchNumber)
{
    SsdSequence *sequence = SsdGetSequencePtr(sequenceId);
    if (sequence != 0) {
        SsdMusicChunk *chunk = sequence->patch_chunk;
        if (chunk != 0) {
            if (chunk->body.patch.patch_count == 0) {
                SsdErrorProc(SSD_ERROR_NO_PATCHES, 0);
            } else if (chunk->body.patch.patch_count - 1 < patchNumber) {
                SsdErrorProc(SSD_ERROR_PATCH_INDEX, 0);
            } else {
                void *patchSet;
                if (patchNumber >= 0) {
                    unsigned short offset = chunk->body.patch.offsets[patchNumber];
                    patchSet = 0;
                    if (offset != 0) {
                        patchSet = (unsigned char *)chunk + offset;
                    }
                } else {
                    patchSet = 0;
                }
                SsdSetTrackPatchSet(sequenceId, patchSet);
            }
        }
    }
}

void SsdSetTrackPatchSet(int id, const SsdTrackPatchSet *patches)
{
    SsdSequence *sequence = SsdGetSequencePtr(id);
    const SsdTrackPatch *entry;
    int index;
    int count;
    if (sequence != 0) {
        SsdResetChannelFader(sequence->device, 1);
        if (patches != 0) {
            count = patches->count;
            entry = (const SsdTrackPatch *)(patches + 1);
            CpuDisableIntr();
            for (index = 0; index < count; index++, entry++) {
                SsdTrack *track = &sequence->tracks[entry->track];
                SsdChannel *channel = track->channel;
                int volume = channel->channel_volume;
                int expression = channel->expression;
                int fader = entry->volume;
                volume *= expression;
                volume *= fader;
                volume /= SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX;
                channel->fader_note = entry->note;
                channel->fader_volume = fader;
                channel->fader_panpot = entry->panpot;
                channel->volume = volume;
                SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_PITCH | SSD_VOICE_EVENT_VOLUME);
            }
            CpuEnableIntr();
        }
    }
}

struct SsdSequence *SsdSetSeqABRepeat(int sequenceId)
{
    return SsdGetSequencePtr(sequenceId);
}

void SsdGetSeqPlayTime(int sequence_id, SsdTimeCode *out)
{
    SsdSequence *sequence = SsdGetSequencePtr(sequence_id);
    if (sequence != 0) {
        SsdGetTimeCode(sequence->play_time >> 8, out);
        out->play_ticks = sequence->play_ticks;
    }
}

void SsdGetTimeCode(unsigned int ticks, SsdTimeCode *timeCode)
{
    unsigned int seconds = ticks / SSD_TICKS_PER_SECOND;
    unsigned int frameUnits;
    unsigned int minutes;
    unsigned int hours;
    ticks %= SSD_TICKS_PER_SECOND;
    frameUnits = ticks * SSD_SMPTE_FRAMES_PER_SECOND;
    minutes = seconds / 60;
    hours = minutes / 60;

    timeCode->subframes = 0;
    timeCode->ticks = ticks;
    timeCode->seconds = seconds - minutes * 60;
    timeCode->frames = frameUnits / SSD_TICKS_PER_SECOND;
    timeCode->hours = hours;
    timeCode->minutes = minutes - hours * 60;
}

void SsdGetPlayMeasure(int sequence_id)
{
    SsdGetSequencePtr(sequence_id);
}

struct SsdSequence *SsdGetSeqLyric(int sequenceId)
{
    return SsdGetSequencePtr(sequenceId);
}

int SsdGetSeqCuePoint(void)
{
    return 0;
}

void SsdSetSeqSignal(int sequenceId, int signal)
{
    SsdSequence *sequence = SsdGetSequencePtr(sequenceId);
    if (sequence != 0) {
        sequence->signal = signal;
    }
}

void SsdSetSeqStopCallback(int sequence_id, SsdSequenceStopCallback callback)
{
    SsdSequence *sequence = SsdGetSequencePtr(sequence_id);
    if (sequence != 0) {
        sequence->stop_callback = callback;
    }
}

SsdSequence *SsdGetSequencePtr(int sequenceId)
{
    SsdSequence *sequence = SsdWork.sequences;
    while (sequence != 0) {
        if (sequence->sequence_id == sequenceId) {
            break;
        }
        sequence = sequence->next;
    }
    return sequence;
}

void SsdStartingSequence(SsdSequence *sequence)
{
    if ((sequence->flags & SSD_SEQUENCE_PLAYING) == 0) {
        SsdSetSeqReverb(sequence);
        sequence->flags = (sequence->flags & ~(SSD_SEQUENCE_STOPPED | SSD_SEQUENCE_STOP_PENDING)) |
                          SSD_SEQUENCE_PLAYING;
    }
}

void SsdStoppingSequence(SsdSequence *sequence)
{
    if (sequence->flags & SSD_SEQUENCE_PLAYING) {
        CpuDisableIntr();
        SsdCutoffDevive(sequence->device);
        sequence->flags = (sequence->flags & 0x7dff) | SSD_SEQUENCE_STOPPED;
        CpuEnableIntr();
    }
}
