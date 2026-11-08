#include "ssd/ssd_main.h"
#include "ssd/ssd_chan.h"
#include "ssd/ssd_data.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_lfo.h"
#include "ssd/ssd_midi.h"
#include "ssd/ssd_out.h"
#include "ssd/ssd_seq.h"
#include "ssd/ssd_trap.h"
#include "ssd/ssd_voic.h"

/* Sequence stream opcodes (commands start at 0x80; SsdSeqFuncTrap is indexed by opcode - 0x80). */
#define SSD_SEQ_COMMAND_BASE 0x80
#define SSD_SEQ_REST 0x88
#define SSD_SEQ_TIE 0x89
#define SSD_SEQ_STOP 0x90
#define SSD_SEQ_LOOP_END 0x99
#define SSD_SEQ_LOOP_ESCAPE 0x9a
#define SSD_SEQ_SLUR_OFF 0xc1

/* Gate ratio (sixteenths of the note length) that plays the whole note. */
#define SSD_TRACK_GATE_RATIO_FULL 16

#define SSD_NOTES_PER_OCTAVE 12

void SsdDeviceVoiceEvent(SsdMidiDevice *device);
void SsdCommonControl(SsdGlobalWork *work);
void SsdMainSequenceProcess(SsdGlobalWork *work);
void SsdMidiTrackSequence(void);
void SsdGetEnvx(void);
void SsdVoiceEvent(void);
void SsdNormalTrackEffect(SsdSequence *sequence);
void SsdNormalTrackGate(SsdSequence *sequence);
void SsdNormalTrackSequence(SsdSequence *sequence);
void SsdEffectTrackSequence(SsdSequence *sequence);

/* Sequencer interrupt handler: runs the sequence, MIDI, LFO and voice processing and the registered processes, and records the ticks used. */
void SsdSeqInterruptProcess(SsdGlobalWork *work)
{
    unsigned int start;
    unsigned int end;
    SsdProc *callback;

    if (work->flags & SSD_FLAG_SUSPENDED) {
        return;
    }
    start = GetTimerCounter(SsdWork.timer_id);
    if (++work->interrupt_count & 1) {
        SsdCommonControl(work);
    }
    SsdMainSequenceProcess(work);
    SsdMidiTrackSequence();
    SsdGetEnvx();
    SsdLfoControlProc(work);
    SsdVoiceEvent();
    SsdSetVoiceKeyon();
    SsdSetVoiceKeyoff();

    callback = SsdWork.processes;
    while (callback != 0) {
        SsdProcCallback process = callback->callback;
        void *argument = callback->argument;
        callback = callback->next;
        process(work, argument);
    }
    end = GetTimerCounter(SsdWork.timer_id);
    if (end >= start) {
        work->processing_ticks += end - start;
        ++work->measured_interrupts;
    }
}

/* Advances the master and CD fades and applies the output volumes that changed. */
void SsdCommonControl(SsdGlobalWork *work)
{
    if (work->master_fade.remaining != 0) {
        SsdCalculateModulate(&work->master_fade);
        work->master_depth = work->master_fade.current.value >> 16;
        SsdSetVolumeRec(work->master_depth, &work->master_volume, 0);
        work->output_flags |= SSD_OUTPUT_MASTER_VOLUME;
    }
    if (work->cd_fade.remaining != 0) {
        SsdCalculateModulate(&work->cd_fade);
        work->cd_depth = work->cd_fade.current.value >> 16;
        SsdSetVolumeRec(work->cd_depth, &work->cd_volume, 0);
        work->output_flags |= SSD_OUTPUT_CD_VOLUME;
    }
    if (work->output_flags != 0) {
        if (work->output_flags & SSD_OUTPUT_MASTER_VOLUME) {
            SsdSetMasterVolume(0, work->master_volume.left, work->master_volume.right);
        }
        work->output_flags = 0;
    }
}

/* Advances each active sequence: applies its master ramps, then runs its track passes for each elapsed clock tick. */
void SsdMainSequenceProcess(SsdGlobalWork *work)
{
    SsdSequence *sequence = work->sequences;

    while (sequence != 0) {
        if (sequence->flags < 0) {
            int clock;
            int tempo;
            if (sequence->master_tempo.remaining != 0) {
                SsdCalculateModulate(&sequence->master_tempo);
                sequence->clock_step = ((sequence->tempo.parts.integer * SSD_TEMPO_CLOCK_SCALE >> 8) * (sequence->master_tempo.current.value >> 16)) >> 8;
            }
            if (sequence->master_note.remaining != 0) {
                SsdCalculateModulate(&sequence->master_note);
                SsdSetMidiDeviceNote(sequence->device, sequence->master_note.current.value >> 16);
            }
            if (sequence->master_volume.remaining != 0) {
                SsdCalculateModulate(&sequence->master_volume);
                SsdSetMidiDeviceVolume(sequence->device, sequence->master_volume.current.value >> 16);
            }
            if (sequence->master_pan.remaining != 0) {
                SsdCalculateModulate(&sequence->master_pan);
                SsdSetMidiDevicePanpot(sequence->device, sequence->master_pan.current.value >> 16);
            }
            ++sequence->interrupts;
            tempo = sequence->master_tempo.current.value >> 16;
            clock = sequence->clock - sequence->clock_step;
            sequence->clock = clock;
            sequence->play_time += tempo;
            /* Run one track pass per elapsed clock tick. */
            if (clock < 0) {
                do {
                    sequence->clock += 65536;
                    if (++sequence->beat_tick == sequence->ticks_per_beat) {
                        sequence->beat_tick = 0;
                        if (++sequence->beat >= sequence->beats_per_measure) {
                            sequence->beat = 0;
                            ++sequence->measure;
                        }
                    }
                    sequence->active_tracks = 0;
                    SsdNormalTrackEffect(sequence);
                    SsdNormalTrackGate(sequence);
                    switch (sequence->mode) {
                    case 0:
                        SsdNormalTrackSequence(sequence);
                        break;
                    case 1:
                        SsdEffectTrackSequence(sequence);
                        break;
                    }
                    if (sequence->active_tracks == 0) {
                        sequence->flags &= ~SSD_SEQUENCE_PLAYING;
                        break;
                    }
                    ++sequence->play_ticks;
                    if (sequence->master_volume.current.value == 0) {
                        sequence->flags = (sequence->flags | SSD_SEQUENCE_STOPPED) &
                                          ~(SSD_SEQUENCE_PLAYING | SSD_SEQUENCE_STOP_PENDING);
                        SsdCutoffDevive(sequence->device);
                    }
                } while (sequence->clock < 0);
            }
        }
        sequence = sequence->next;
    }
}

/* Decodes and clears the queued MIDI events of the first queue. */
void SsdMidiTrackSequence(void)
{
    int count = SsdMidi.queues[0].count;
    SsdMidiDevice *device = SsdMidi.device;
    unsigned int *message = SsdMidi.queues[0].events;

    SsdMidi.queues[0].count = 0;
    while (count != 0) {
        --count;
        SsdMidiEventDecode(device, *message++);
    }
}

/* Advances the pitch, volume, pan and fade-out ramps of each active track and updates its channel. */
void SsdNormalTrackEffect(SsdSequence *sequence)
{
    int remaining = sequence->track_count;
    SsdTrack *part = sequence->tracks;

    while (remaining != 0) {
        --remaining;
        if (part->flags < 0) {
            unsigned short effects = part->control_flags;
            unsigned short events = 0;
            SsdChannel *channel = part->channel;

            if (effects & SSD_TRACK_CONTROL_PITCH_RAMP) {
                if (!(effects & 2)) {
                    if (--part->pitch.remaining == 0) {
                        part->pitch.current.value = part->pitch.target;
                        effects &= ~SSD_TRACK_CONTROL_PITCH_RAMP;
                    } else {
                        part->pitch.current.value += part->pitch.step;
                    }
                }
                channel->pitch_offset = part->pitch.current.value;
                events |= SSD_VOICE_EVENT_PITCH;
            }
            if (effects & SSD_TRACK_CONTROL_VOLUME_RAMP) {
                signed char volume;
                if (--part->volume.remaining == 0) {
                    part->volume.current.value = part->volume.target;
                    effects &= ~SSD_TRACK_CONTROL_VOLUME_RAMP;
                } else {
                    part->volume.current.value += part->volume.step;
                }
                volume = part->volume.current.value >> 16;
                channel->channel_volume = volume;
                channel->volume = volume * channel->expression * channel->fader_volume /
                                  (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
                events |= SSD_VOICE_EVENT_VOLUME;
            }
            if (effects & SSD_TRACK_CONTROL_PAN_RAMP) {
                if (--part->pan.remaining == 0) {
                    part->pan.current.value = part->pan.target;
                    effects &= ~SSD_TRACK_CONTROL_PAN_RAMP;
                } else {
                    part->pan.current.value += part->pan.step;
                }
                channel->panpot = part->pan.current.value;
                events |= SSD_VOICE_EVENT_VOLUME;
            }
            if (effects & SSD_TRACK_CONTROL_FADE_OUT) {
                if (--part->velocity.remaining == 0) {
                    SsdCutoffChannel(part->channel);
                    events = 0;
                    effects = 0;
                    part->flags = 0;
                } else {
                    part->velocity.current.value += part->velocity.step;
                    SsdSetChannelVoiceVelocity(part->channel, (signed char)(part->velocity.current.value >> 16));
                }
            }
            part->control_flags = effects;
            if (events != 0) {
                SsdSetChannelVoiceEvent(part->channel, events);
            }
        }
        ++part;
    }
}

/* Counts down the duration of each sounding voice and keys it off when the count reaches zero. */
void SsdNormalTrackGate(SsdSequence *track)
{
    SsdMidiDevice *device = track->device;
    int remaining = device->channel_count;
    SsdChannel *channel = device->channels;

    while (remaining != 0) {
        SsdVoice *voice = channel->voices;
        --remaining;
        while (voice != 0) {
            unsigned int gate = voice->duration;
            if (gate != 0) {
                if (--gate == 0) {
                    SsdKeyoffChannelVoice(channel, voice);
                }
                voice->duration = gate;
            }
            voice = voice->next;
        }
        ++channel;
    }
}

/* Reads the stream of each active track: notes are keyed on, other opcodes go through SsdSeqFuncTrap. */
void SsdNormalTrackSequence(SsdSequence *sequence)
{
    int remaining = sequence->track_count;
    SsdTrack *part = sequence->tracks;

    while (remaining != 0) {
        --remaining;
        if (part->flags < 0) {
            unsigned char *stream;
            ++sequence->active_tracks;
            stream = part->stream;
            if (stream != 0) {
                do {
                    unsigned char opcode;
                    if (part->wait != 0) {
                        break;
                    }
                    opcode = *stream++;
                    if ((signed char)opcode < 0) {
                        stream = SsdSeqFuncTrap[opcode - SSD_SEQ_COMMAND_BASE](sequence, part, stream);
                    } else {
                        SsdMidiEvent note;
                        unsigned char attributes;
                        int bytes;
                        unsigned int duration;

                        note.velocity = opcode;
                        part->velocity.current.value = opcode << 16;
                        attributes = *stream++;
                        bytes = attributes >> 6;
                        part->octave += (((attributes >> 4) & 3) - 1) * SSD_NOTES_PER_OCTAVE;
                        note.key = part->octave + (attributes & 15);
                        if (bytes != 0) {
                            duration = 0;
                            while (bytes-- != 0) {
                                duration <<= 8;
                                duration += *stream++;
                            }
                            part->duration = duration;
                        } else {
                            duration = part->duration;
                        }
                        note.duration = duration;
                        if (!(part->flags & SSD_TRACK_MUTED)) {
                            SsdKeyonChannel(part->channel, &note);
                        }
                    }
                } while (part->flags < 0);
            }
            --part->wait;
            part->stream = stream;
        }
        ++part;
    }
}

/* Plays an effect sequence: reads each track's notes, following jumps and loops, and keys them on. */
void SsdEffectTrackSequence(SsdSequence *sequence)
{
    SsdTrack *part = sequence->tracks;
    int remaining = sequence->track_count;
    SsdMidiEvent note;
    int duration;
    unsigned char *stream;
    unsigned char opcode;
    unsigned char trigger = 1;
    int jump_command = SSD_SEQ_STOP;

    sequence->active_tracks = 0;
    do {
        if ((unsigned short)part->flags != 0) {

            ++sequence->active_tracks;
            note.trigger = 0;
            stream = part->stream;
            while (part->wait == 0) {
                part->flags &= ~(SSD_TRACK_NOTE | SSD_TRACK_SLURRED | SSD_TRACK_REST);
                note.velocity = part->velocity.current.value >> 16;
                note.key = part->note;
                do {
                    opcode = *stream++;
                    if ((signed char)opcode >= 0) {
                        int velocity = part->velocity.current.value >> 16;
                        int pitch;
                        duration = stream[0] + (stream[1] << 8);
                        note.velocity = velocity;
                        pitch = part->octave + (opcode & 15);
                        note.duration = duration;
                        note.key = pitch;
                        part->wait = duration;
                        stream += 2;
                        part->flags |= SSD_TRACK_NOTE;
                        note.trigger = trigger;
                    } else {
                        stream = SsdSeqFuncTrap[opcode - SSD_SEQ_COMMAND_BASE](sequence, part, stream);
                        if (part->flags >= 0) {
                            break;
                        }
                        if (part->flags & SSD_TRACK_NOTE) {
                            note.duration = part->wait;
                            note.trigger = trigger;
                        }
                    }
                } while (!(part->flags & (SSD_TRACK_NOTE | SSD_TRACK_REST)));
                part->stream = stream;
                if (part->flags & SSD_TRACK_ACTIVE) {
                    SsdTrackLoop *loop;
                    if (part->flags & SSD_TRACK_SLUR_MODE) {
                        part->flags |= SSD_TRACK_SLURRED;
                    }
                    loop = &part->loops[part->loop_depth];
                    while ((signed char)(opcode = *stream) < 0) {
                        int hold_command = SSD_SEQ_TIE;
                        int repeat_command = SSD_SEQ_LOOP_END;
                        int exit_command = SSD_SEQ_LOOP_ESCAPE;
                        if (opcode == jump_command) {
                            stream = part->jump;
                            if (stream != 0) {
                                continue;
                            } else {
                                break;
                            }
                        } else if (opcode == SSD_SEQ_REST || opcode == SSD_SEQ_SLUR_OFF) {
                            part->flags &= ~SSD_TRACK_SLURRED;
                            break;
                        } else if (opcode == hold_command) {
                            part->flags |= SSD_TRACK_SLURRED;
                            break;
                        } else {
                            if (opcode == repeat_command) {
                                if (loop->remaining != 0) {
                                    stream = loop->start;
                                    continue;
                                }
                                --loop;
                            }
                            if (opcode == exit_command && loop->remaining == 0) {
                                stream = loop->escape;
                                --loop;
                            } else {
                                stream += SsdSeqFuncLength[opcode - SSD_SEQ_COMMAND_BASE];
                            }
                        }
                    }
                } else {
                    break;
                }
            }
            if (part->flags >= 0) {
                --part->wait;
                ++part;
                continue;
            }
            /* Key on the note that was read, with the gate ratio applied to its duration. */
            if (note.trigger != 0) {
                SsdChannel *channel = part->channel;
                duration = note.duration;
                if (duration != -1) {
                    int ratio = part->gate_ratio;
                    switch (ratio) {
                    case SSD_TRACK_GATE_RATIO_FULL:
                        break;
                    case SSD_TRACK_GATE_RATIO_DEFAULT:
                        duration -= 2;
                        duration = duration > 0 ? duration : 1;
                        break;
                    default:
                        duration = (duration * ratio) >> 4;
                        duration = duration > 0 ? duration : 1;
                        break;
                    }
                }
                note.duration = duration;
                part->last_note = part->note = note.key;
                SsdKeyonChannel(channel, &note);
                if (part->flags & SSD_TRACK_SLURRED) {
                    channel->flags |= SSD_CHANNEL_SLUR;
                } else {
                    channel->flags &= ~SSD_CHANNEL_SLUR;
                }
            }
            --part->wait;
        }
        ++part;
    } while (--remaining != 0);
}

void SsdVoiceEvent(void)
{
    SsdMidiDevice *device = SsdWork.devices;

    while (device != 0) {
        SsdDeviceVoiceEvent(device);
        device = device->next;
    }
}

/* Applies the pending events of each voice of a device: volume, pitch, key on and key off. */
void SsdDeviceVoiceEvent(SsdMidiDevice *device)
{
    int left;
    int right;
    int volume;
    int pan;
    if (!(device->state & SSD_DEVICE_SKIP)) {
        int remaining = device->channel_count;
        SsdChannel *channel;
        short unity_volume;

        if (remaining == 0) {
            return;
        }
        channel = device->channels;

        do {
            SsdVoice *voice = channel->voices;
            unity_volume = SSD_PAN_UNITY_GAIN;
            --remaining;
            while (voice != 0) {
                unsigned short events = voice->events;
                if (events & SSD_VOICE_EVENT_VOLUME) {
                    int gain;
                    volume = channel->volume * voice->velocity / 63;

                    volume -= (volume * voice->lfo.volume) >> 14;
                    if (volume > SSD_SPU_VOLUME_MAX) {
                        volume = SSD_SPU_VOLUME_MAX;
                    }
                    if (volume < -SSD_SPU_VOLUME_MAX) {
                        volume = -SSD_SPU_VOLUME_MAX;
                    }
                    volume = ((device->master_volume >> 8) * volume) >> 23;
                    gain = volume * volume;
                    if (volume < 0) {
                        gain = -gain;
                    }
                    pan = voice->lfo.pan + ((channel->panpot + device->master_panpot) >> 8);
                    pan += channel->fader_panpot * 256 - 16384;
                    pan += voice->base_panpot * 256 - 16384;
                    volume = gain;
                    if (pan > SSD_PAN_RANGE) {
                        pan = SSD_PAN_RANGE;
                    }
                    if (pan < 0) {
                        pan = 0;
                    }
                    if (SsdWork.output_mode != SSD_OUTPUT_MONO) {
                        if (pan < SSD_PAN_CENTER) {
                            right = pan * SSD_PAN_CENTER_GAIN / SSD_PAN_CENTER;
                            left = unity_volume - pan * SSD_PAN_ATTENUATION / SSD_PAN_CENTER;
                        } else {
                            pan = SSD_PAN_RANGE - pan;
                            left = pan * SSD_PAN_CENTER_GAIN / SSD_PAN_CENTER;
                            right = unity_volume - pan * SSD_PAN_ATTENUATION / SSD_PAN_CENTER;
                        }
                        left = (left * volume) >> 15;
                        right = (right * volume) >> 15;
                    } else {
                        left = right = (volume * SSD_PAN_CENTER_GAIN) >> 15;
                    }
                    voice->left_volume = left & 0x7fff;
                    voice->right_volume = right & 0x7fff;
                    voice->flags |= SSD_VOICE_FLAG_VOLUME;
                }
                /* The pitch is the note plus transpose, LFO and master note offsets. */
                if (events & SSD_VOICE_EVENT_PITCH) {
                    int note = voice->note + channel->transpose + voice->lfo.pitch;
                    note += (channel->pitch_offset + device->master_note) >> 16;
                    voice->pitch = SsdNoteToPitch((short)note) & SSD_VOICE_PITCH_MAX;
                    voice->flags |= SSD_VOICE_FLAG_PITCH;
                }
                if ((events & SSD_VOICE_EVENT_KEYON) && !(channel->flags & 1)) {
                    SsdKeyonVoice(voice);
                    voice->key_state |= 9;
                }
                if (events & SSD_VOICE_EVENT_KEYOFF) {
                    SsdKeyoffVoice(voice);
                    events = 0;
                    voice->key_state &= 0xfffe;
                } else {
                    events = 0;
                }
                voice->events = events;
                voice = voice->next;
            }
            ++channel;
        } while (remaining != 0);
    }
}

/* Reads the ENVX value of every SPU voice into its record. */
void SsdGetEnvx(void)
{
    SsdVoice *record = SsdWork.voices;
    int core;
    int voice;

    for (core = 0; core < SSD_CORE_COUNT; ++core) {
        for (voice = 0; voice < SSD_CORE_VOICE_COUNT; ++voice) {
            record->allocation.envx = sceSdGetParam(core | (voice << 1) | SD_VP_ENVX) >> 8;
            ++record;
        }
    }
}

void SsdCalculateModulate(SsdRamp *ramp)
{
    if (--ramp->remaining != 0) {
        ramp->current.value += ramp->step;
    } else {
        ramp->current.value = ramp->target;
    }
}
