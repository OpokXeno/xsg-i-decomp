/* Original ssd_main.c translation unit. Shared data is defined by its owning FDR. */





typedef struct SsdVoice SsdVoice;
typedef struct SsdChannel SsdChannel;
typedef struct SsdDevice SsdDevice;
typedef struct SsdGlobalWork SsdGlobalWork;
typedef struct SsdRamp SsdRamp;
typedef struct SsdPart SsdPart;
typedef struct SsdSequence SsdSequence;
typedef struct SsdNote SsdNote;
typedef struct SsdLoop SsdLoop;
typedef struct SsdMidiQueue SsdMidiQueue;
typedef struct SsdSequenceCallback SsdSequenceCallback;
typedef struct SsdStereoVolume SsdStereoVolume;

struct SsdVoice {
    unsigned char unmodeled_00[4];
    unsigned short hardware_changes;
    unsigned char unmodeled_06[2];
    unsigned short left, right;
    unsigned char unmodeled_0c[8];
    unsigned short pitch;
    unsigned char unmodeled_16[2];
    unsigned char envelope;
    unsigned char unmodeled_19[19];
    unsigned short flags, events;
    unsigned char unmodeled_30[4];
    unsigned int gate;
    short note, velocity;
    unsigned char unmodeled_3c[8];
    short pan;
    unsigned char unmodeled_46[4];
    short pitch_modulation, volume_modulation, pan_modulation;
    unsigned char unmodeled_50[168];
    struct SsdVoice *next;
    unsigned char unmodeled_fc[4];
};

struct SsdChannel {
    unsigned int flags;
    unsigned char unmodeled_04[16];
    int pitch, volume, pan;
    unsigned char unmodeled_20[2];
    signed char volume_factor, pan_offset;
    short transpose;
    unsigned char unmodeled_26[3];
    signed char volume_control, expression;
    unsigned char unmodeled_2b[189];
    SsdVoice *voices;
    unsigned char unmodeled_ec[16];
};

struct SsdDevice {
    unsigned int flags;
    unsigned char unmodeled_04, channel_count;
    unsigned char unmodeled_06[6];
    int transpose, volume, pan;
    unsigned char unmodeled_18[4];
    struct SsdDevice *next;
    SsdChannel channels[0];
};

struct SsdRamp { int value, step, target; short remaining; };

struct SsdSequence {
    unsigned char unmodeled_00[4];
    struct SsdSequence *next;
    unsigned char unmodeled_08[4];
    SsdPart *parts;
    unsigned char unmodeled_10[4];
    SsdDevice *device;
    unsigned char unmodeled_18[8];
    short flags;
    short active_parts;
    unsigned char unmodeled_24[2];
    unsigned char part_count;
    unsigned char unmodeled_27[3];
    unsigned char mode;
    unsigned char unmodeled_2b[7];
    short base_tempo;
    unsigned char unmodeled_34[12];
    int clock, clock_step, interrupts, ticks, tempo_accumulator;
    unsigned char unmodeled_54[6];
    unsigned short measure, beat, beat_tick, beats_per_measure, ticks_per_beat;
    unsigned char unmodeled_64[24];
    SsdRamp tempo, note, volume, pan;
};

struct SsdNote {
    unsigned char trigger;
    unsigned char unmodeled_01;
    unsigned char velocity;
    unsigned char pitch;
    int duration;
};

struct SsdLoop {
    unsigned char remaining;
    unsigned char unmodeled_01[3];
    unsigned char *begin;
    unsigned char *end;
};

struct SsdMidiQueue {
    unsigned char unmodeled_00[4];
    struct SsdDevice *device;
    unsigned short count;
    unsigned int messages[64];
};

struct SsdSequenceCallback {
    unsigned char unmodeled_00[4];
    void (*process)(SsdGlobalWork *work, void *argument);
    void *argument;
    struct SsdSequenceCallback *next;
};

struct SsdStereoVolume {
    short left;
    short right;
};

struct SsdGlobalWork {
    unsigned int flags;
    unsigned char unmodeled_004[332];
    int timer;
    unsigned char unmodeled_154[4];
    unsigned int interrupt_count;
    unsigned char unmodeled_15c[12];
    SsdSequence *sequences;
    unsigned char unmodeled_16c[12];
    SsdDevice *devices;
    unsigned char unmodeled_17c[48];
    unsigned int output_changes;
    SsdStereoVolume master_output;
    unsigned char unmodeled_1b4[8];
    SsdStereoVolume reverb_output;
    unsigned char unmodeled_1c0[20];
    short master_volume, reverb_volume;
    unsigned char unmodeled_1d8[7];
    unsigned char stereo_mode;
    SsdRamp master_ramp, reverb_ramp;
    unsigned char unmodeled_200[108];
    SsdVoice voices[48];
    unsigned char unmodeled_326c[20];
    SsdSequenceCallback *callbacks;
    unsigned char unmodeled_3284[20];
    unsigned int processing_ticks, measured_interrupts;
};

struct SsdPart {
    short flags;
    unsigned short effects;
    unsigned char unmodeled_04[5];
    unsigned char loop_depth;
    unsigned char unmodeled_0a[18];
    unsigned char *stream, *jump;
    SsdChannel *channel;
    unsigned char unmodeled_28[8];
    int wait;
    unsigned char unmodeled_34[4];
    unsigned int duration;
    signed char gate_ratio;
    unsigned char octave, note, last_note;
    SsdRamp fade, pitch, volume, pan;
    SsdLoop loops[4];
};

extern SsdGlobalWork SsdWork;
extern int SsdNoteToPitch(short note);
extern void SsdKeyonVoice(SsdVoice *voice);
extern void SsdKeyoffVoice(SsdVoice *voice);
void SsdDeviceVoiceEvent(SsdDevice *device);
extern void SsdCutoffChannel(SsdChannel *channel);
extern void SsdSetChannelVoiceVelocity(SsdChannel *channel, int velocity);
extern void SsdSetChannelVoiceEvent(SsdChannel *channel, unsigned short events);
extern void SsdKeyoffChannelVoice(SsdChannel *channel, SsdVoice *voice);
extern unsigned short sceSdGetParam(unsigned short entry);
extern unsigned char *(*SsdSeqFuncTrap[128])(SsdSequence *, SsdPart *, unsigned char *);
extern void SsdKeyonChannel(struct SsdChannel *channel, SsdNote *note);
extern unsigned char SsdSeqFuncLength[128];
extern SsdMidiQueue SsdMidi;
extern void SsdMidiEventDecode(struct SsdDevice *device, unsigned int message);
extern unsigned int GetTimerCounter(int timer);
void SsdCommonControl(SsdGlobalWork *work);
void SsdMainSequenceProcess(SsdGlobalWork *work);
void SsdMidiTrackSequence(void);
void SsdGetEnvx(void);
extern void SsdLfoControlProc(SsdGlobalWork *work);
void SsdVoiceEvent(void);
extern void SsdSetVoiceKeyon(void);
extern void SsdSetVoiceKeyoff(void);
void SsdCalculateModulate(SsdRamp *ramp);
extern void SsdSetMidiDeviceNote(struct SsdDevice *device, int note);
extern void SsdSetMidiDeviceVolume(struct SsdDevice *device, int volume);
extern void SsdSetMidiDevicePanpot(struct SsdDevice *device, int pan);
void SsdNormalTrackEffect(SsdSequence *sequence);
void SsdNormalTrackGate(SsdSequence *track);
void SsdNormalTrackSequence(SsdSequence *sequence);
void SsdEffectTrackSequence(SsdSequence *sequence);
extern void SsdCutoffDevive(struct SsdDevice *device);
extern void SsdSetVolumeRec(int volume, SsdStereoVolume *output, int channel);
extern void SsdSetMasterVolume(int core, int left, int right);
void SsdSeqInterruptProcess(SsdGlobalWork *work);

void SsdSeqInterruptProcess(SsdGlobalWork *work)
{
    unsigned int start;
    unsigned int end;
    SsdSequenceCallback *callback;

    if (work->flags & 0x80) {
        return;
    }
    start = GetTimerCounter(SsdWork.timer);
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

    callback = SsdWork.callbacks;
    while (callback != 0) {
        void (*process)(SsdGlobalWork *, void *) = callback->process;
        void *argument = callback->argument;
        callback = callback->next;
        process(work, argument);
    }
    end = GetTimerCounter(SsdWork.timer);
    if (end >= start) {
        work->processing_ticks += end - start;
        ++work->measured_interrupts;
    }
}

void SsdCommonControl(SsdGlobalWork *work)
{
    if (work->master_ramp.remaining != 0) {
        SsdCalculateModulate(&work->master_ramp);
        work->master_volume = work->master_ramp.value >> 16;
        SsdSetVolumeRec(work->master_volume, &work->master_output, 0);
        work->output_changes |= 3;
    }
    if (work->reverb_ramp.remaining != 0) {
        SsdCalculateModulate(&work->reverb_ramp);
        work->reverb_volume = work->reverb_ramp.value >> 16;
        SsdSetVolumeRec(work->reverb_volume, &work->reverb_output, 0);
        work->output_changes |= 0xc0;
    }
    if (work->output_changes != 0) {
        if (work->output_changes & 3) {
            SsdSetMasterVolume(0, work->master_output.left, work->master_output.right);
        }
        work->output_changes = 0;
    }
}

void SsdMainSequenceProcess(SsdGlobalWork *work)
{
    SsdSequence *sequence = work->sequences;

    while (sequence != 0) {
        if (sequence->flags < 0) {
            int clock;
            int tempo;
            if (sequence->tempo.remaining != 0) {
                SsdCalculateModulate(&sequence->tempo);
                sequence->clock_step = ((sequence->base_tempo * 53687 >> 8) * (sequence->tempo.value >> 16)) >> 8;
            }
            if (sequence->note.remaining != 0) {
                SsdCalculateModulate(&sequence->note);
                SsdSetMidiDeviceNote(sequence->device, sequence->note.value >> 16);
            }
            if (sequence->volume.remaining != 0) {
                SsdCalculateModulate(&sequence->volume);
                SsdSetMidiDeviceVolume(sequence->device, sequence->volume.value >> 16);
            }
            if (sequence->pan.remaining != 0) {
                SsdCalculateModulate(&sequence->pan);
                SsdSetMidiDevicePanpot(sequence->device, sequence->pan.value >> 16);
            }
            ++sequence->interrupts;
            tempo = sequence->tempo.value >> 16;
            clock = sequence->clock - sequence->clock_step;
            sequence->clock = clock;
            sequence->tempo_accumulator += tempo;
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
                    sequence->active_parts = 0;
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
                    if (sequence->active_parts == 0) {
                        sequence->flags &= 0x7fff;
                        break;
                    }
                    ++sequence->ticks;
                    if (sequence->volume.value == 0) {
                        sequence->flags = (sequence->flags | 0x100) & 0x7dff;
                        SsdCutoffDevive(sequence->device);
                    }
                } while (sequence->clock < 0);
            }
        }
        sequence = sequence->next;
    }
}

void SsdMidiTrackSequence(void)
{
    int count = SsdMidi.count;
    struct SsdDevice *device = SsdMidi.device;
    unsigned int *message = SsdMidi.messages;

    SsdMidi.count = 0;
    while (count != 0) {
        --count;
        SsdMidiEventDecode(device, *message++);
    }
}

void SsdNormalTrackEffect(SsdSequence *sequence)
{
    int remaining = sequence->part_count;
    SsdPart *part = sequence->parts;

    while (remaining != 0) {
        --remaining;
        if (part->flags < 0) {
            unsigned short effects = part->effects;
            unsigned short events = 0;
            SsdChannel *channel = part->channel;

            if (effects & 1) {
                if (!(effects & 2)) {
                    if (--part->pitch.remaining == 0) {
                        part->pitch.value = part->pitch.target;
                        effects &= ~1;
                    } else {
                        part->pitch.value += part->pitch.step;
                    }
                }
                channel->pitch = part->pitch.value;
                events |= 0x10;
            }
            if (effects & 8) {
                int volume;
                if (--part->volume.remaining == 0) {
                    part->volume.value = part->volume.target;
                    effects &= ~8;
                } else {
                    part->volume.value += part->volume.step;
                }
                volume = part->volume.value >> 16;
                channel->volume_control = volume;
                channel->volume = (signed char)volume * channel->expression * channel->volume_factor / 16129;
                events |= 0x20;
            }
            if (effects & 0x10) {
                if (--part->pan.remaining == 0) {
                    part->pan.value = part->pan.target;
                    effects &= ~0x10;
                } else {
                    part->pan.value += part->pan.step;
                }
                channel->pan = part->pan.value;
                events |= 0x20;
            }
            if (effects & 0x20) {
                if (--part->fade.remaining == 0) {
                    SsdCutoffChannel(part->channel);
                    events = 0;
                    effects = 0;
                    part->flags = 0;
                } else {
                    part->fade.value += part->fade.step;
                    SsdSetChannelVoiceVelocity(part->channel, (signed char)(part->fade.value >> 16));
                }
            }
            part->effects = effects;
            if (events != 0) {
                SsdSetChannelVoiceEvent(part->channel, events);
            }
        }
        ++part;
    }
}

void SsdNormalTrackGate(SsdSequence *track)
{
    SsdDevice *device = track->device;
    int remaining = device->channel_count;
    SsdChannel *channel = device->channels;

    while (remaining != 0) {
        SsdVoice *voice = channel->voices;
        --remaining;
        while (voice != 0) {
            unsigned int gate = voice->gate;
            if (gate != 0) {
                if (--gate == 0) {
                    SsdKeyoffChannelVoice(channel, voice);
                }
                voice->gate = gate;
            }
            voice = voice->next;
        }
        ++channel;
    }
}

void SsdNormalTrackSequence(SsdSequence *sequence)
{
    int remaining = sequence->part_count;
    SsdPart *part = sequence->parts;

    while (remaining != 0) {
        --remaining;
        if (part->flags < 0) {
            unsigned char *stream;
            ++sequence->active_parts;
            stream = part->stream;
            if (stream != 0) {
                do {
                    unsigned char opcode;
                    if (part->wait != 0) {
                        break;
                    }
                    opcode = *stream++;
                    if ((signed char)opcode < 0) {
                        stream = SsdSeqFuncTrap[opcode - 128](sequence, part, stream);
                    } else {
                        SsdNote note;
                        unsigned char attributes;
                        int bytes;
                        unsigned int duration;

                        note.velocity = opcode;
                        part->fade.value = opcode << 16;
                        attributes = *stream++;
                        bytes = attributes >> 6;
                        part->octave += (((attributes >> 4) & 3) - 1) * 12;
                        note.pitch = part->octave + (attributes & 15);
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
                        if (!(part->flags & 2)) {
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

void SsdEffectTrackSequence(SsdSequence *sequence)
{
    SsdPart *part = sequence->parts;
    int remaining = sequence->part_count;
    SsdNote note;
    int duration;
    unsigned char *stream;
    unsigned char opcode;
    unsigned char trigger = 1;
    int jump_command = 144;

    sequence->active_parts = 0;
    do {
        if ((unsigned short)part->flags != 0) {

            ++sequence->active_parts;
            note.trigger = 0;
            stream = part->stream;
            while (part->wait == 0) {
                part->flags &= 0xf8ff;
                note.velocity = part->fade.value >> 16;
                note.pitch = part->note;
                do {
                    opcode = *stream++;
                    if ((signed char)opcode >= 0) {
                        int velocity = part->fade.value >> 16;
                        int pitch;
                        duration = stream[0] + (stream[1] << 8);
                        note.velocity = velocity;
                        pitch = part->octave + (opcode & 15);
                        note.duration = duration;
                        note.pitch = pitch;
                        part->wait = duration;
                        stream += 2;
                        part->flags |= 0x100;
                        note.trigger = trigger;
                    } else {
                        stream = SsdSeqFuncTrap[opcode - 128](sequence, part, stream);
                        if ((short)part->flags >= 0) {
                            break;
                        }
                        if (part->flags & 0x100) {
                            note.duration = part->wait;
                            note.trigger = trigger;
                        }
                    }
                } while (!(part->flags & 0x500));
                part->stream = stream;
                if ((short)part->flags & 0x8000) {
                    SsdLoop *loop;
                    if (part->flags & 0x800) {
                        part->flags |= 0x200;
                    }
                    loop = &part->loops[part->loop_depth];
                    while ((signed char)(opcode = *stream) < 0) {
                        int hold_command = 137;
                        int repeat_command = 153;
                        int exit_command = 154;
                        if ((unsigned char)opcode == jump_command) {
                            stream = part->jump;
                            if (stream != 0) {
                                continue;
                            } else {
                                break;
                            }
                        } else if ((unsigned char)opcode == 136 || (unsigned char)opcode == 193) {
                            part->flags &= 0xfdff;
                            break;
                        } else if ((unsigned char)opcode == hold_command) {
                            part->flags |= 0x200;
                            break;
                        } else {
                            if ((unsigned char)opcode == repeat_command) {
                                if (loop->remaining != 0) {
                                    stream = loop->begin;
                                    continue;
                                }
                                --loop;
                            }
                            if (opcode == exit_command && loop->remaining == 0) {
                                stream = loop->end;
                                --loop;
                            } else {
                                stream += SsdSeqFuncLength[opcode - 128];
                            }
                        }
                    }
                } else {
                    break;
                }
            }
            if ((short)part->flags >= 0) {
                --part->wait;
                ++part;
                continue;
            }
            if (note.trigger != 0) {
                SsdChannel *channel = part->channel;
                duration = note.duration;
                if (duration != -1) {
                    int ratio = part->gate_ratio;
                    switch (ratio) {
                    case 16:
                        break;
                    case 15:
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
                part->last_note = part->note = note.pitch;
                SsdKeyonChannel(channel, &note);
                if (part->flags & 0x200) {
                    channel->flags |= 2;
                } else {
                    channel->flags &= ~2;
                }
            }
            --part->wait;
        }
        ++part;
    } while (--remaining != 0);
}

void SsdVoiceEvent(void)
{
    SsdDevice *device = SsdWork.devices;

    while (device != 0) {
        SsdDeviceVoiceEvent(device);
        device = device->next;
    }
}

void SsdDeviceVoiceEvent(SsdDevice *device)
{
    int left;
    int right;
    int volume;
    int pan;
    if (!(device->flags & 1)) {
        int remaining = device->channel_count;
        SsdChannel *channel;
        short unity_volume;

        if (remaining == 0) {
            return;
        }
        channel = device->channels;

        do {
            SsdVoice *voice = channel->voices;
            unity_volume = 32512;
            --remaining;
            while (voice != 0) {
                unsigned short events = voice->events;
                if (events & 0x20) {
                    int gain;
                    volume = channel->volume * voice->velocity / 63;

                    volume -= (volume * voice->volume_modulation) >> 14;
                    if (volume > 32767) {
                        volume = 32767;
                    }
                    if (volume < -32767) {
                        volume = -32767;
                    }
                    volume = ((device->volume >> 8) * volume) >> 23;
                    gain = volume * volume;
                    if (volume < 0) {
                        gain = -gain;
                    }
                    pan = voice->pan_modulation + ((channel->pan + device->pan) >> 8);
                    pan += channel->pan_offset * 256 - 16384;
                    pan += voice->pan * 256 - 16384;
                    volume = gain;
                    if (pan > 32256) {
                        pan = 32256;
                    }
                    if (pan < 0) {
                        pan = 0;
                    }
                    if (SsdWork.stereo_mode != 0) {
                        if (pan < 16128) {
                            right = pan * 23040 / 16128;
                            left = unity_volume - pan * 9472 / 16128;
                        } else {
                            pan = 32256 - pan;
                            left = pan * 23040 / 16128;
                            right = unity_volume - pan * 9472 / 16128;
                        }
                        left = (left * volume) >> 15;
                        right = (right * volume) >> 15;
                    } else {
                        left = right = (volume * 23040) >> 15;
                    }
                    voice->left = left & 0x7fff;
                    voice->right = right & 0x7fff;
                    voice->hardware_changes |= 2;
                }
                if (events & 0x10) {
                    int note = voice->note + channel->transpose + voice->pitch_modulation;
                    note += (channel->pitch + device->transpose) >> 16;
                    voice->pitch = SsdNoteToPitch((short)note) & 0x3fff;
                    voice->hardware_changes |= 1;
                }
                if ((events & 1) && !(channel->flags & 1)) {
                    SsdKeyonVoice(voice);
                    voice->flags |= 9;
                }
                if (events & 2) {
                    SsdKeyoffVoice(voice);
                    events = 0;
                    voice->flags &= 0xfffe;
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

void SsdGetEnvx(void)
{
    SsdVoice *record = SsdWork.voices;
    int core;
    int voice;

    for (core = 0; core < 2; ++core) {
        for (voice = 0; voice < 24; ++voice) {
            record->envelope = sceSdGetParam((unsigned short)(core | (voice << 1) | 0x500)) >> 8;
            ++record;
        }
    }
}

void SsdCalculateModulate(SsdRamp *ramp)
{
    if (--ramp->remaining != 0) {
        ramp->value = (unsigned int)ramp->value + (unsigned int)ramp->step;
    } else {
        ramp->value = ramp->target;
    }
}
