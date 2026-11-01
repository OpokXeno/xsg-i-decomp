/* Original SSD FDR 10: sequence opcode handlers. Shared partial layouts preserve evidenced field offsets. */

/* The signed/unsigned views retain the original halfword and byte accesses.
 * Fixed-point sequence tempos use a whole-word accumulator and its signed
 * integer half. All unmodeled spans below are untouched by this FDR. */
typedef struct SsdSequence SsdSequence;
typedef struct SsdSequenceTrack SsdSequenceTrack;
typedef struct SsdChannel SsdChannel;
typedef struct SsdVoice SsdVoice;
typedef struct SsdGlobalWork SsdGlobalWork;
typedef struct SsdLfoRecord SsdLfoRecord;
typedef int (*SsdLfoProcedure)(SsdLfoRecord *, SsdGlobalWork *);
typedef union SsdSignedHalf {
    unsigned short bits;
    short value;
} SsdSignedHalf;
typedef union SsdSignedByte {
    unsigned char bits;
    signed char value;
} SsdSignedByte;
typedef union SsdFixedPoint {
    int value;
    struct { unsigned short fraction; short integer; } parts;
} SsdFixedPoint;
typedef struct SsdDevice { unsigned int flags; } SsdDevice;
typedef struct SsdRamp {
    SsdFixedPoint current;
    int step;
    int target;
    short remaining;
} SsdRamp;
typedef struct SsdSequenceMeter {
    unsigned short beats_per_measure;
    unsigned short ticks_per_beat;
    unsigned short meter_denominator;
    unsigned short meter_numerator;
} SsdSequenceMeter;
typedef struct SsdMeasureEvent { unsigned char denominator, numerator; } SsdMeasureEvent;
struct SsdSequence {
    unsigned char unmodeled_000[0x14];
    SsdDevice *device;
    unsigned char unmodeled_018[0xf];
    unsigned char condition;
    unsigned char noise_clock;
    unsigned char unmodeled_029[7];
    SsdFixedPoint tempo;
    int tempo_increment;
    int tempo_target;
    short tempo_duration;
    unsigned char unmodeled_03e[6];
    int tempo_step;
    unsigned char unmodeled_048[8];
    unsigned int time_offset;
    unsigned char unmodeled_054[6];
    short measure, beat, tick;
    SsdSequenceMeter meter;
    unsigned char unmodeled_068[4];
    short reverb_parameter;
    unsigned char unmodeled_06e[2];
    signed char reverb_left, reverb_right;
    unsigned char unmodeled_072[10];
    SsdRamp master_tempo;
    unsigned char unmodeled_08c[16];
    int master_fade_volume, master_fade_step, master_fade_target;
    unsigned short master_fade_time;
};
struct SsdVoice {
    unsigned char unmodeled_000[4];
    unsigned short flags;
    unsigned char unmodeled_006[0x1e];
    unsigned short adsr1, adsr2;
    unsigned char unmodeled_028[0xd0];
    SsdVoice *next;
};
struct SsdLfoRecord {
    unsigned short flags;
    unsigned char destination, waveform;
    int step;
    SsdSignedHalf period, phase, fade_step;
    SsdLfoProcedure process;
    SsdLfoRecord *next;
    int value;
    unsigned char unmodeled_01c[4];
    unsigned short remaining, delay;
    short fade;
};
typedef struct SsdLfoWork { unsigned char flags; } SsdLfoWork;
struct SsdChannel {
    unsigned int flags;
    unsigned char unmodeled_004[7];
    unsigned char random_parameter_1, random_parameter_2;
    unsigned char unmodeled_00d;
    unsigned short bank;
    unsigned char unmodeled_010[4];
    int pitch;
    int output_volume;
    int pan;
    unsigned char unmodeled_020[2];
    signed char volume_scale;
    unsigned char unmodeled_023;
    SsdSignedHalf tuning;
    unsigned char unmodeled_026[3];
    signed char volume;
    SsdSignedByte expression;
    unsigned char unmodeled_02b[9];
    unsigned short adsr1, adsr2;
    SsdLfoWork lfo;
    unsigned char selected_lfo;
    unsigned char unmodeled_03a[14];
    SsdLfoRecord lfos[4];
    SsdVoice *voices;
};
typedef struct SsdTrackLoop {
    unsigned char remaining;
    unsigned char unmodeled_001;
    unsigned char start_octave, escape_octave;
    unsigned char *start, *escape;
} SsdTrackLoop;
struct SsdSequenceTrack {
    unsigned short flags, control_flags;
    unsigned char track_index, midi_channel, effect_parameter, attributes;
    unsigned char priority, loop_depth;
    unsigned short bank_id;
    unsigned int effect_file_id, effect_id, timestamp;
    unsigned char *data_start, *cursor, *repeat_cursor;
    SsdChannel *channel;
    unsigned int state_028;
    unsigned short repeat_count;
    unsigned char state_02e, repeat_octave;
    unsigned int delta, after_delta, gate_delta;
    unsigned char gate_time, octave;
    unsigned char unmodeled_03e[18];
    int pitch, sweep_step, sweep_target;
    SsdSignedHalf sweep_ticks;
    unsigned char unmodeled_05e[2];
    int volume, volume_step, volume_target;
    unsigned short volume_time;
    unsigned char unmodeled_06e[2];
    int pan, pan_step, pan_target;
    unsigned short pan_time;
    unsigned char unmodeled_07e[2];
    SsdTrackLoop loops[4];
};
extern SsdLfoProcedure WaveformTable[];
extern int SsdLfoTriangularDoubleProc(SsdLfoRecord *, SsdGlobalWork *);
extern int SsdLfoTriangularSingleProc(SsdLfoRecord *, SsdGlobalWork *);
extern void SsdInitLfoWork(SsdLfoWork *);

extern void SsdSeqErrorProc(int error, unsigned char *events);
extern void SsdCutoffChannel(SsdChannel *channel);
extern int Kprintf(const char *format, ...);
extern void SsdSetChannelWave(SsdChannel *channel, int wave);
extern void SsdSetChannelProgram(SsdChannel *channel, int program);
extern void SsdSendChannelKeyoffEvent(SsdChannel *channel);
extern void SsdSetChannelHWEffectOn(SsdChannel *channel, int enabled, int effect);
extern void SsdSetChannelHWEffectOff(SsdChannel *, int effect, int mask);
extern void SsdSetReverbParameter(int core, int parameter, int left, int right);
extern void SsdSetChannelVoiceEvent(SsdChannel *channel, int event);
extern int SsdSeqGetLFODepth(int depth, int rate, int type);
#define SSD_SET_U16(field, value) \
    do { (field) = (unsigned short)(value); } while (0)
#define SSD_SET_METER_TICKS(sequence, ticks) \
    do { SSD_SET_U16((sequence)->meter.ticks_per_beat, (ticks)); } while (0)
void SsdLfoOn(SsdSequenceTrack *track, int index);
void SsdLfoOff(SsdSequenceTrack *track, int index);

unsigned char *SsdSeqDummy(SsdSequence *sequence, SsdSequenceTrack *track,
                          unsigned char *events)
{
    SsdSeqErrorProc(-192, events);
    track->flags &= 0x7fff;
    SsdCutoffChannel(track->channel);
    events--;
    Kprintf("Ssd exec dummy trap\n");
    return events;
}

unsigned char *SsdSeqDeltaForAfterDelta(SsdSequence *sequence,
                                      SsdSequenceTrack *channel,
                                      unsigned char *events)
{
    channel->delta = channel->after_delta;
    return events;
}

unsigned char *SsdSeqDeltaForGate(SsdSequence *sequence,
                                SsdSequenceTrack *track,
                                unsigned char *events)
{
    unsigned int duration = track->gate_delta;
    track->after_delta = duration;
    track->delta = duration;
    return events;
}

unsigned char *SsdSeqDeltaAddGate(SsdSequence *sequence, SsdSequenceTrack *channel,
                                unsigned char *events)
{
    int gate = (signed char)events[0] + channel->after_delta;
    channel->after_delta = gate;
    channel->delta = gate;
    return events + 1;
}

unsigned char *SsdSeqDeltaAddAfterDelta(SsdSequence *sequence,
                                     SsdSequenceTrack *track,
                                     unsigned char *events)
{
    unsigned int duration = (signed char)events[0] + track->gate_delta;
    track->after_delta = duration;
    track->delta = duration;
    return events + 1;
}

unsigned char *SsdSeqDelta1(SsdSequence *sequence, SsdSequenceTrack *channel,
                          unsigned char *events)
{
    int delta = events[0];
    channel->after_delta = delta;
    channel->delta = delta;
    return events + 1;
}

unsigned char *SsdSeqDelta2(SsdSequence *sequence, SsdSequenceTrack *track,
                           unsigned char *events)
{
    unsigned int duration = events[0] + (events[1] << 8);
    track->after_delta = duration;
    track->delta = duration;
    return events + 2;
}

unsigned char *SsdSeqDelta3(SsdSequence *sequence, SsdSequenceTrack *channel,
                          unsigned char *events)
{
    int delta = events[0];
    delta += events[1] << 8;
    delta += events[2] << 16;
    channel->after_delta = delta;
    channel->delta = delta;
    return events + 3;
}

unsigned char *SsdSeqRest(SsdSequence *sequence, SsdSequenceTrack *track,
                         unsigned char *events)
{
    unsigned int duration = events[0] + (events[1] << 8);
    unsigned short flags = track->flags;
    SsdChannel *channel = track->channel;

    flags = (flags | 0x400) & 0xe4ff;
    track->delta = duration;
    track->flags = flags;
    channel->flags &= ~6;
    return events + 2;
}

unsigned char *SsdSeqTie(SsdSequence *sequence, SsdSequenceTrack *channel,
                       unsigned char *events)
{
    channel->delta = events[0] + (events[1] << 8);
    channel->flags |= 0x100;
    return events + 2;
}

unsigned char *SsdSeqStop(SsdSequence *sequence, SsdSequenceTrack *track,
                         unsigned char *events)
{
    unsigned char *repeat = track->repeat_cursor;
    track->after_delta = 0;
    track->gate_delta = 0;
    if (repeat) {
        events = repeat;
        track->repeat_count++;
        track->octave = track->repeat_octave;
    } else {
        SsdCutoffChannel(track->channel);
        events--;
        track->flags = 0;
    }
    return events;
}

unsigned char *SsdSeqRepeat(SsdSequence *sequence, SsdSequenceTrack *channel,
                          unsigned char *events)
{
    channel->repeat_cursor = events;
    channel->after_delta = 0;
    channel->gate_delta = 0;
    channel->repeat_octave = channel->octave;
    return events;
}

unsigned char *SsdSeqJump(SsdSequence *sequence, SsdChannel *channel,
                         unsigned char *events)
{
    int displacement = events[0] | (events[1] << 8);
    return events + (short)(displacement - 1);
}

unsigned char *SsdSeqIf(SsdSequence *sequence, SsdChannel *channel,
                      unsigned char *events)
{
    if (sequence->condition == events[0]) {
        int displacement = events[1] | (events[2] << 8);
        events += (short)(displacement - 1);
    } else {
        events += 3;
    }
    return events;
}

unsigned char *SsdSeqOctaveAbsolute(SsdSequence *sequence,
                                  SsdSequenceTrack *track,
                                  unsigned char *events)
{
    track->octave = events[0] * 12;
    return events + 1;
}

unsigned char *SsdSeqOctaveRelative(SsdSequence *sequence,
                                  SsdSequenceTrack *track,
                                  unsigned char *events)
{
    track->octave = (track->octave + (signed char)events[0] * 12) & 0x7f;
    return events + 1;
}

unsigned char *SsdSeqOctaveUp(SsdSequence *sequence, SsdSequenceTrack *track,
                             unsigned char *events)
{
    track->octave += 12;
    return events;
}

unsigned char *SsdSeqOctaveDown(SsdSequence *sequence, SsdSequenceTrack *track,
                              unsigned char *events)
{
    track->octave -= 12;
    return events;
}

unsigned char *SsdSeqLoopTop(SsdSequence *sequence, SsdSequenceTrack *track,
                            unsigned char *events)
{
    SsdTrackLoop *loop;
    track->loop_depth++;
    loop = &track->loops[track->loop_depth];
    loop->remaining = *events++ - 1;
    loop->start = events;
    loop->start_octave = track->octave;
    return events;
}

unsigned char *SsdSeqLoopEnd(SsdSequence *sequence, SsdSequenceTrack *track,
                           unsigned char *events)
{
    SsdTrackLoop *loop = &track->loops[track->loop_depth];
    if (--loop->remaining != 255) {
        loop->escape = events;
        loop->escape_octave = track->octave;
        events = loop->start;
        track->octave = loop->start_octave;
    } else {
        --track->loop_depth;
    }
    return events;
}

unsigned char *SsdSeqLoopEscape(SsdSequence *sequence, SsdSequenceTrack *track,
                              unsigned char *events)
{
    SsdTrackLoop *frame = &track->loops[track->loop_depth];
    if (frame->remaining == 0) {
        events = frame->escape;
        track->octave = frame->escape_octave;
        track->loop_depth--;
    }
    return events;
}

unsigned char *SsdSeqTempoAbsolute(SsdSequence *sequence,
                                 SsdSequenceTrack *track,
                                 unsigned char *events)
{
    int tempo = events[0];
    int step = (tempo * 53687) >> 8;
    sequence->tempo.value = tempo << 16;
    sequence->tempo_step = (step * sequence->master_tempo.current.parts.integer) >> 8;
    return events + 1;
}

unsigned char *SsdSeqTempoRelative(SsdSequence *sequence,
                                  SsdSequenceTrack *track,
                                  unsigned char *events)
{
    int tempo = sequence->tempo.value + (*events << 16);
    int scaled_tempo = ((tempo >> 16) * 53687) >> 8;
    sequence->tempo.value = tempo;
    sequence->tempo_step = (scaled_tempo * sequence->master_tempo.current.parts.integer) >> 8;
    return events + 1;
}

unsigned char *SsdSeqTempoChange(SsdSequence *sequence,
                               SsdSequenceTrack *track,
                               unsigned char *events)
{
    int target = events[1];
    int duration = events[0];
    sequence->tempo_target = target << 8;
    if (duration == 0) {
        int step = (target * 53687) >> 8;
        sequence->tempo_duration = 0;
        sequence->tempo.value = target << 16;
        sequence->tempo_step = (step * sequence->master_tempo.current.parts.integer) >> 8;
    } else {
        int difference = (target << 16) - sequence->tempo.value;
        if (difference != 0) {
            sequence->tempo_increment = difference / duration;
            sequence->tempo_duration = duration;
        }
    }
    return events + 2;
}

unsigned char *SsdSeqMeasure(SsdSequence *sequence, SsdSequenceTrack *track,
                           unsigned char *events)
{
    const SsdMeasureEvent *meter_event = (const SsdMeasureEvent *)events;
    unsigned short denominator = meter_event->denominator;
    int ticks_per_beat = 384 / (int)denominator;
    int numerator = meter_event->numerator;
    SSD_SET_U16(sequence->meter.meter_denominator, denominator);
    sequence->meter.beats_per_measure = numerator;
    sequence->meter.meter_numerator = numerator;
    SSD_SET_METER_TICKS(sequence, ticks_per_beat);
    return events + 2;
}

unsigned char *SsdSeqPosition(SsdSequence *sequence, SsdSequenceTrack *track,
                            unsigned char *events)
{
    sequence->measure = events[0] - 1;
    sequence->beat = events[1] - 1;
    sequence->tick = 0;
    return events + 2;
}

unsigned char *SsdSeqMasterFader(SsdSequence *sequence,
                                SsdSequenceTrack *track,
                                unsigned char *events)
{
    int target = events[0] << 16;
    int duration = events[1] * 500;

    sequence->master_fade_target = target;
    if (duration == 0) {
        sequence->master_fade_volume = target;
        sequence->master_fade_time = 0;
    } else {
        int difference = target - sequence->master_fade_volume;
        if (difference != 0) {
            sequence->master_fade_step = difference / duration;
            sequence->master_fade_time = duration;
        }
    }
    return events + 2;
}

unsigned char *SsdSeqBankMSB(SsdSequence *sequence, SsdSequenceTrack *track,
                           unsigned char *events)
{
    SsdChannel *channel = track->channel;
    channel->bank = events[0] + (channel->bank & 0xff00);
    return events + 1;
}

unsigned char *SsdSeqBankLSB(SsdSequence *sequence, SsdSequenceTrack *track,
                           unsigned char *events)
{
    SsdChannel *channel = track->channel;
    channel->bank = (channel->bank & 0xff) + (events[0] << 8);
    return events + 1;
}

unsigned char *SsdSeqWaveChange(SsdSequence *sequence, SsdSequenceTrack *track,
                              unsigned char *events)
{
    SsdSetChannelWave(track->channel, events[0]);
    return events + 1;
}

unsigned char *SsdSeqWaveBankType(SsdSequence *sequence,
                                 SsdSequenceTrack *track,
                                 unsigned char *events)
{
    SsdChannel *channel = track->channel;
    channel->bank = events[2] + (events[1] << 8);
    SsdSetChannelWave(channel, events[0]);
    return events + 3;
}

unsigned char *SsdSeqProgramChange(SsdSequence *sequence,
                                 SsdSequenceTrack *track,
                                 unsigned char *events)
{
    SsdSetChannelProgram(track->channel, events[0]);
    return events + 1;
}

unsigned char *SsdSeqRandomNote(SsdSequence *sequence, SsdSequenceTrack *track,
                              unsigned char *events)
{
    SsdChannel *channel = track->channel;
    if (events[0]) {
        channel->flags |= 0x30;
        channel->random_parameter_1 = events[1];
        channel->random_parameter_2 = events[2];
    } else {
        channel->flags &= ~0x30;
    }
    return events + 3;
}

unsigned char *SsdSeqADSRReset(SsdSequence *sequence, SsdSequenceTrack *track,
                             unsigned char *events)
{
    return events;
}

unsigned char *SsdSeqADSRMode(SsdSequence *sequence, SsdChannel *channel,
                            unsigned char *events)
{
    return events + 3;
}

unsigned char *SsdSeqAR(SsdSequence *sequence, SsdSequenceTrack *track,
                      unsigned char *events)
{
    SsdChannel *channel = track->channel;
    int adsr = (events[0] << 8) | (channel->adsr1 & 0x80ff);
    SsdVoice *voice = channel->voices;
    channel->adsr1 = adsr;
    while (voice != 0) {
        voice->adsr1 = adsr;
        voice->flags |= 0x10;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqDR(SsdSequence *sequence, SsdSequenceTrack *track,
                       unsigned char *events)
{
    SsdChannel *channel = track->channel;
    unsigned int adsr1 = (events[0] << 4) | (channel->adsr1 & 0xff0f);
    SsdVoice *voice;
    voice = channel->voices;
    channel->adsr1 = adsr1;
    while (voice) {
        voice->adsr1 = adsr1;
        voice->flags |= 0x10;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqSL(SsdSequence *sequence, SsdSequenceTrack *track,
                      unsigned char *events)
{
    SsdChannel *channel = track->channel;
    unsigned short adsr = events[0];
    int merged = adsr | (channel->adsr1 & 0xfff0);
    SsdVoice *voice = channel->voices;
    adsr = merged;
    channel->adsr1 = merged;
    while (voice != 0) {
        voice->adsr1 = adsr;
        voice->flags |= 0x10;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqSR(SsdSequence *sequence, SsdSequenceTrack *track,
                       unsigned char *events)
{
    SsdChannel *channel = track->channel;
    unsigned int adsr2 = (events[0] << 6) | (channel->adsr2 & 0xe03f);
    SsdVoice *voice = channel->voices;
    channel->adsr2 = adsr2;
    while (voice) {
        voice->adsr2 = adsr2;
        voice->flags |= 0x20;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqRR(SsdSequence *sequence, SsdSequenceTrack *track,
                       unsigned char *events)
{
    SsdChannel *channel = track->channel;
    unsigned short adsr = events[0];
    int merged = adsr | (channel->adsr2 & 0xffe0);
    SsdVoice *voice = channel->voices;
    adsr = merged;
    channel->adsr2 = merged;
    while (voice) {
        voice->adsr2 = adsr;
        voice->flags |= 0x20;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqDRSL(SsdSequence *sequence, SsdSequenceTrack *track,
                         unsigned char *events)
{
    SsdChannel *channel = track->channel;
    unsigned int adsr1 = events[1] | (events[0] << 4) |
                         (channel->adsr1 & 0xff00);
    SsdVoice *voice = channel->voices;
    channel->adsr1 = adsr1;
    while (voice) {
        voice->adsr1 = adsr1;
        voice->flags |= 0x10;
        voice = voice->next;
    }
    return events + 2;
}

unsigned char *SsdSeqAMode(SsdSequence *sequence, SsdSequenceTrack *track,
                         unsigned char *events)
{
    SsdChannel *channel = track->channel;
    unsigned int adsr = (events[0] << 15) | (channel->adsr1 & 0x7fff);
    SsdVoice *voice = channel->voices;
    channel->adsr1 = adsr;
    while (voice != 0) {
        voice->adsr1 = adsr;
        voice->flags |= 0x10;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqSMode(SsdSequence *sequence, SsdSequenceTrack *track,
                       unsigned char *events)
{
    SsdChannel *channel = track->channel;
    unsigned int adsr2 = (events[0] << 13) | (channel->adsr2 & 0x3fff);
    SsdVoice *voice = channel->voices;
    channel->adsr2 = adsr2;
    while (voice) {
        voice->adsr2 = adsr2;
        voice->flags |= 0x20;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqRMode(SsdSequence *sequence, SsdSequenceTrack *track,
                          unsigned char *events)
{
    int mode = events[0] << 5;
    SsdChannel *channel = track->channel;
    int adsr = mode | (channel->adsr2 & 0xffdf);
    SsdVoice *voice = channel->voices;
    channel->adsr2 = adsr;
    while (voice) {
        voice->adsr2 = adsr;
        voice->flags |= 0x20;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqPriority(SsdSequence *sequence, SsdSequenceTrack *track,
                            unsigned char *events)
{
    track->priority = events[0] & 15;
    return events + 1;
}

unsigned char *SsdSeqGateTime(SsdSequence *sequence, SsdSequenceTrack *track,
                            unsigned char *events)
{
    track->gate_time = events[0];
    return events + 1;
}

unsigned char *SsdSeqSustaion(SsdSequence *sequence, SsdSequenceTrack *track,
                             unsigned char *events)
{
    SsdChannel *channel = track->channel;
    if (events[0] >= 64) {
        channel->flags |= 4;
    } else {
        channel->flags &= ~4U;
        SsdSendChannelKeyoffEvent(channel);
    }
    return events + 1;
}

unsigned char *SsdSeqSulrOn(SsdSequence *sequence, SsdSequenceTrack *track,
                           unsigned char *events)
{
    track->flags |= 0x800;
    return events;
}

unsigned char *SsdSeqSulrOff(SsdSequence *sequence, SsdSequenceTrack *track,
                           unsigned char *events)
{
    track->flags &= ~0x800;
    return events;
}

unsigned char *SsdSeqPMOn(SsdSequence *sequence, SsdSequenceTrack *track,
                        unsigned char *events)
{
    SsdSetChannelHWEffectOn(track->channel, 1, 0x1000);
    return events;
}

unsigned char *SsdSeqPMOff(SsdSequence *sequence, SsdSequenceTrack *track,
                          unsigned char *events)
{
    SsdSetChannelHWEffectOff(track->channel, 1, 0x1000);
    return events;
}

unsigned char *SsdSeqNoiseAbsolute(SsdSequence *sequence, SsdSequenceTrack *track,
                                  unsigned char *events)
{
    sequence->noise_clock = events[0];
    SsdSetChannelHWEffectOn(track->channel, 2, 0x2000);
    return events + 1;
}

signed char *SsdSeqNoiseRelative(SsdSequence *sequence,
                                 SsdSequenceTrack *track,
                                 signed char *events)
{
    short noise = sequence->noise_clock;
    int delta = events[0];
    noise += delta;
    sequence->noise_clock = noise & 0x3f;
    SsdSetChannelHWEffectOn(track->channel, 2, 0x2000);
    return events + 1;
}

unsigned char *SsdSeqNoiseOn(SsdSequence *sequence, SsdSequenceTrack *track,
                           unsigned char *events)
{
    SsdSetChannelHWEffectOn(track->channel, 2, 0x2000);
    return events;
}

unsigned char *SsdSeqNoiseOff(SsdSequence *sequence, SsdSequenceTrack *track,
                          unsigned char *events)
{
    SsdSetChannelHWEffectOff(track->channel, 2, 0x2000);
    return events;
}

signed char *SsdSeqReverbParameter(SsdSequence *sequence, SsdSequenceTrack *track,
                                  signed char *events)
{
    int parameter = (unsigned char)events[0];
    int left;
    int right;
    sequence->reverb_parameter = parameter << 8;
    left = events[1];
    sequence->reverb_left = left;
    right = events[2];
    sequence->reverb_right = right;
    SsdSetReverbParameter(-1, (short)(parameter << 8), left, right);
    return events + 3;
}

unsigned char *SsdSeqReverbSet(SsdSequence *sequence, SsdSequenceTrack *track,
                             unsigned char *events)
{
    SsdChannel *channel = track->channel;
    if (events[0])
        SsdSetChannelHWEffectOn(channel, 8, 0x8000);
    else
        SsdSetChannelHWEffectOff(channel, 8, 0x8000);
    if (events[1])
        SsdSetChannelHWEffectOn(channel, 4, 0x4000);
    else
        SsdSetChannelHWEffectOff(channel, 4, 0x4000);
    return events + 2;
}

unsigned char *SsdSeqReverbOn(SsdSequence *sequence, SsdSequenceTrack *track,
                            unsigned char *events)
{
    SsdSetChannelHWEffectOn(track->channel, 4, 0x4000);
    return events;
}

unsigned char *SsdSeqReverbOff(SsdSequence *sequence, SsdSequenceTrack *track,
                          unsigned char *events)
{
    SsdSetChannelHWEffectOff(track->channel, 4, 0x4000);
    return events;
}

unsigned char *SsdSeqKeyTransposeAbsolute(SsdSequence *sequence, SsdSequenceTrack *track,
                                         unsigned char *events)
{
    int key = events[0];
    SsdChannel *channel = track->channel;
    channel->tuning.value = key << 8;
    SsdSetChannelVoiceEvent(channel, 16);
    return events + 1;
}

unsigned char *SsdSeqKeyTransposeRelative(SsdSequence *sequence,
                                        SsdSequenceTrack *track,
                                        unsigned char *events)
{
    int offset = (short)(events[0] << 8);
    SsdChannel *channel = track->channel;
    channel->tuning.bits += offset;
    SsdSetChannelVoiceEvent(channel, 16);
    return events + 1;
}

unsigned char *SsdSeqTune(SsdSequence *sequence, SsdSequenceTrack *track,
                        unsigned char *events)
{
    short operand = events[0] << 8;
    SsdChannel *channel = track->channel;
    int adjustment = operand >> 5;
    channel->tuning.bits += adjustment;
    SsdSetChannelVoiceEvent(channel, 0x10);
    return events + 1;
}

unsigned char *SsdSeqDetune(SsdSequence *sequence, SsdSequenceTrack *track,
                           unsigned char *events)
{
    SsdChannel *channel = track->channel;
    int delta = events[0] + (short)(events[1] << 8);
    channel->tuning.bits += delta;
    SsdSetChannelVoiceEvent(channel, 16);
    return events + 2;
}

signed char *SsdSeqBender(SsdSequence *sequence, SsdSequenceTrack *track,
                          signed char *events)
{
    int low = (unsigned char)events[0];
    int high = events[1];
    SsdChannel *channel = track->channel;
    int bend;
    track->sweep_ticks.value = 0;
    bend = low * 65536 + high * 16777216;
    track->pitch = bend;
    channel->pitch = bend;
    SsdSetChannelVoiceEvent(channel, 16);
    track->control_flags &= 0xfffe;
    return events + 2;
}

unsigned char *SsdSeqSweep(SsdSequence *sequence, SsdSequenceTrack *track,
                         unsigned char *events)
{
    unsigned short duration = events[0] | (events[1] << 8);
    int target = (signed char)events[2] * 16777216;
    int difference;
    if (target) {
        target += track->pitch;
        difference = target - track->pitch;
    } else {
        difference = -track->pitch;
    }
    if (duration && difference) {
        int step = difference / duration;
        unsigned short flags = track->control_flags;
        SsdChannel *channel = track->channel;
        track->sweep_target = target;
        track->sweep_ticks.bits = duration;
        track->control_flags = flags | 1;
        track->sweep_step = step;
        channel->pitch = track->pitch;
        SsdSetChannelVoiceEvent(channel, 16);
    }
    return events + 3;
}

unsigned char *SsdSeqVibrateFade(SsdSequence *sequence, SsdSequenceTrack *track,
                               unsigned char *events)
{
    int duration = events[0];
    SsdChannel *channel = track->channel;
    int step = 1024;
    if (duration != 0) {
        step /= duration * 4;
    }
    channel->lfos[0].fade_step.value = step;
    return events + 1;
}

unsigned char *SsdSeqVibrate1(SsdSequence *sequence, SsdSequenceTrack *track,
                             unsigned char *events)
{
    int depth = (signed char)events[1];
    int rate = events[0];
    if (depth && rate) {
        int delay;
        SsdChannel *channel;
        SsdLfoRecord *vibrato;

        depth = depth < 0 ? -(depth * depth) * 16384
                          : (depth * depth) * 16384;
        rate = rate > 20 ? rate * rate / 20 : rate;
        channel = track->channel;
        vibrato = &channel->lfos[0];
        delay = events[2] * 10;
        vibrato->flags = 17;
        vibrato->destination = 0;
        vibrato->waveform = 3;
        vibrato->step = SsdSeqGetLFODepth(depth, rate, 3);
        vibrato->fade_step.bits = 1024;
        vibrato->period.bits = rate;
        vibrato->phase.bits = delay;
        vibrato->process = SsdLfoTriangularDoubleProc;
        channel->lfo.flags |= 1;
    }
    return events + 3;
}

signed char *SsdSeqVibrate2(SsdSequence *sequence, SsdSequenceTrack *track,
                           signed char *events)
{
    unsigned short speed = (unsigned char)events[0];
    int depth = events[1];
    unsigned char type = events[2];
    if (depth && speed) {
        int originalSpeed;
        short adjustedSpeed;
        unsigned short flags;
        SsdChannel *channel;
        SsdLfoRecord *vibrato;
        if (depth < 0)
            depth = -(depth * depth) * 16384;
        else
            depth = depth * depth * 16384;
        originalSpeed = (short)speed;
        if (originalSpeed < 21)
            adjustedSpeed = speed;
        else
            adjustedSpeed = originalSpeed * originalSpeed / 20;
        speed = adjustedSpeed;
        flags = type & 0x10;
        flags = !flags;
        flags = (flags * 0x10) | 1;
        type &= 0x0f;
        channel = track->channel;
        vibrato = &channel->lfos[0];
        vibrato->flags = flags;
        vibrato->destination = 0;
        vibrato->waveform = type;
        vibrato->step = SsdSeqGetLFODepth(depth, (short)speed, type);
        vibrato->period.value = speed;
        vibrato->phase.value = 0;
        vibrato->fade_step.value = 1024;
        vibrato->process = WaveformTable[type];
        channel->lfo.flags |= 1;
    }
    return events + 3;
}

unsigned char *SsdSeqVibrateOn(SsdSequence *sequence, SsdChannel *channel,
                             unsigned char *events)
{
    return events;
}

unsigned char *SsdSeqVibrateOff(SsdSequence *sequence, SsdSequenceTrack *track,
                              unsigned char *events)
{
    return events;
}

unsigned char *SsdSeqBenderOff(SsdSequence *sequence, SsdSequenceTrack *track,
                              unsigned char *events)
{
    SsdChannel *channel = track->channel;
    track->pitch = 0;
    track->sweep_ticks.bits = 0;
    channel->pitch = 0;
    SsdSetChannelVoiceEvent(channel, 16);
    return events;
}

unsigned char *SsdSeqExpression(SsdSequence *sequence, SsdSequenceTrack *track,
                               unsigned char *events)
{
    SsdChannel *channel = track->channel;
    int expression = events[0];
    channel->expression.bits = expression;
    channel->output_volume = channel->volume * expression * channel->volume_scale / 16129;
    SsdSetChannelVoiceEvent(channel, 32);
    return events + 1;
}

unsigned char *SsdSeqVolumeAbsolute(SsdSequence *sequence,
                                  SsdSequenceTrack *track,
                                  unsigned char *events)
{
    int volume = (signed char)events[0];
    SsdChannel *channel = track->channel;
    int scaled_volume;
    track->volume = volume * 65536;
    scaled_volume = volume * channel->expression.value * channel->volume_scale;
    channel->volume = volume;
    channel->output_volume = scaled_volume / (127 * 127);
    track->control_flags &= 0xfef7;
    SsdSetChannelVoiceEvent(channel, 32);
    return events + 1;
}

unsigned char *SsdSeqVolumeRelative(SsdSequence *sequence,
                                  SsdSequenceTrack *track,
                                  unsigned char *events)
{
    SsdChannel *channel = track->channel;
    int volume = channel->volume = channel->volume + events[0];
    track->volume = volume * 65536;
    channel->output_volume =
        volume * channel->expression.value * channel->volume_scale / 16129;
    SsdSetChannelVoiceEvent(channel, 0x20);
    track->control_flags &= 0xfef7;
    return events + 1;
}

unsigned char *SsdSeqVolumeFade(SsdSequence *sequence, SsdSequenceTrack *track,
                               unsigned char *events)
{
    int target = (unsigned int)(signed char)events[2] << 16;
    int duration = events[0] + (events[1] << 8);
    int difference;
    difference = target - track->volume;
    if (duration && difference) {
        int step = difference / duration;
        int volume;
        int output_scale;
        SsdChannel *channel;
        track->volume_time = duration;
        volume = track->volume >> 16;
        channel = track->channel;
        track->volume_target = target;
        track->volume_step = step;
        output_scale = channel->volume_scale;
        target = (signed char)volume;
        channel->volume = volume;
        channel->output_volume = target *
            channel->expression.value * output_scale / (127 * 127);
        SsdSetChannelVoiceEvent(channel, 32);
        track->control_flags = (track->control_flags | 8) & 0xfeff;
    }
    return events + 3;
}

unsigned char *SsdSeqTremoloFade(SsdSequence *sequence, SsdSequenceTrack *track,
                                unsigned char *events)
{
    int duration = events[0];
    SsdChannel *channel = track->channel;
    int step = 1024;
    if (duration)
        step = 1024 / (duration * 4);
    channel->lfos[1].fade_step.value = step;
    return events + 1;
}

unsigned char *SsdSeqTremolo1(SsdSequence *sequence, SsdSequenceTrack *track,
                            unsigned char *events)
{
    int depth = (signed char)events[1];
    int period = events[0];
    if (depth && period) {
        int fade;
        SsdChannel *channel;
        SsdLfoRecord *lfo;
        period = period < 21 ? period : period * period / 20;
        channel = track->channel;
        fade = events[2] * 10;
        lfo = &channel->lfos[1];
        lfo->flags = 17;
        lfo->destination = 1;
        lfo->waveform = 2;
        lfo->step = SsdSeqGetLFODepth(depth * 8388608, period, 2);
        lfo->fade_step.bits = 1024;
        lfo->period.bits = period;
        lfo->phase.bits = fade;
        lfo->process = SsdLfoTriangularSingleProc;
        channel->lfo.flags |= 2;
    }
    return events + 3;
}

unsigned char *SsdSeqTremolo2(SsdSequence *sequence, SsdSequenceTrack *track,
                            unsigned char *events)
{
    unsigned short period = events[0];
    int depth = (signed char)events[1];
    unsigned char waveform = events[2];
    if (depth != 0) {
        int original_period = (short)period;
        if (original_period != 0) {
            SsdChannel *channel;
            SsdLfoRecord *lfo;
            short adjusted_period;
            unsigned short flags;
            depth *= 8388608;
            if (original_period < 21) {
                adjusted_period = period;
            } else {
                adjusted_period = original_period * original_period / 20;
            }
            period = adjusted_period;
            flags = waveform & 0x10;
            flags = !flags;
            flags = (flags * 0x10) | 1;
            waveform &= 0x0f;
            channel = track->channel;
            lfo = &channel->lfos[1];
            lfo->flags = flags;
            lfo->destination = 1;
            lfo->waveform = waveform;
            lfo->step = SsdSeqGetLFODepth(depth, (short)period, waveform);
            lfo->period.value = period;
            lfo->phase.value = 0;
            lfo->fade_step.value = 1024;
            lfo->process = WaveformTable[waveform];
            channel->lfo.flags |= 2;
        }
    }
    return events + 3;
}

unsigned char *SsdSeqTremoloOn(SsdSequence *sequence, SsdChannel *channel,
                             unsigned char *events)
{
    return events;
}

unsigned char *SsdSeqTremoloOff(SsdSequence *sequence, SsdSequenceTrack *track,
                               unsigned char *events)
{
    return events;
}

unsigned char *SsdSeqPanpotAbsolute(SsdSequence *sequence,
                                  SsdSequenceTrack *track,
                                  unsigned char *events)
{
    int pan = ((signed char)events[0] - 1) * 65536;
    SsdChannel *channel;
    if (pan < 0)
        pan = 0;
    channel = track->channel;
    track->pan = pan;
    channel->pan = pan;
    SsdSetChannelVoiceEvent(channel, 32);
    track->control_flags &= ~0x10;
    return events + 1;
}

unsigned char *SsdSeqPanpotRelative(SsdSequence *sequence,
                                  SsdSequenceTrack *track,
                                  unsigned char *events)
{
    int pan = track->pan + (signed char)events[0] * 65536;
    SsdChannel *channel;
    if (pan < 0) {
        pan = 0;
    }
    if (pan > 0x7e0000) {
        pan = 0x7e0000;
    }
    channel = track->channel;
    track->pan = pan;
    channel->pan = pan;
    SsdSetChannelVoiceEvent(channel, 0x20);
    track->control_flags &= 0xffef;
    return events + 1;
}

unsigned char *SsdSeqPanpoMove(SsdSequence *sequence, SsdSequenceTrack *track,
                              unsigned char *events)
{
    int target = (signed char)events[2] - 1;
    int duration = events[0] + (events[1] << 8);
    int difference;
    if (target < 0)
        target = 0;
    target *= 65536;
    difference = target - track->pan;
    if (duration && difference) {
        int step = difference / duration;
        unsigned short state = track->control_flags;
        SsdChannel *channel;
        int pan;
        track->pan_target = target;
        channel = track->channel;
        track->pan_time = duration;
        pan = track->pan;
        track->control_flags = state | 16;
        track->pan_step = step;
        channel->pan = pan;
        SsdSetChannelVoiceEvent(channel, 32);
    }
    return events + 3;
}

unsigned char *SsdSeqShakeFade(SsdSequence *sequence, SsdSequenceTrack *track,
                              unsigned char *events)
{
    int duration = events[0];
    SsdChannel *channel = track->channel;
    int step = 1024;
    if (duration)
        step = 1024 / (duration * 4);
    channel->lfos[2].fade_step.value = step;
    return events + 1;
}

unsigned char *SsdSeqShake1(SsdSequence *sequence, SsdSequenceTrack *track,
                          unsigned char *events)
{
    int depth = (signed char)events[1];
    int period = events[0];
    if (depth && period) {
        int fade;
        SsdChannel *channel;
        SsdLfoRecord *lfo;
        period = period < 21 ? period : period * period / 20;
        channel = track->channel;
        fade = events[2] * 10;
        lfo = &channel->lfos[2];
        lfo->flags = 17;
        lfo->destination = 2;
        lfo->waveform = 3;
        lfo->step = SsdSeqGetLFODepth(depth * 16777216, period, 3);
        lfo->fade_step.bits = 1024;
        lfo->period.bits = period;
        lfo->phase.bits = fade;
        lfo->process = SsdLfoTriangularDoubleProc;
        channel->lfo.flags |= 4;
    }
    return events + 3;
}

unsigned char *SsdSeqShake2(SsdSequence *sequence, SsdSequenceTrack *track,
                          unsigned char *events)
{
    unsigned short period = events[0];
    int depth = (signed char)events[1];
    unsigned char waveform = events[2];
    if (depth != 0) {
        int original_period = (short)period;
        if (original_period != 0) {
            SsdChannel *channel;
            SsdLfoRecord *lfo;
            short adjusted_period;
            unsigned short flags;
            depth *= 16777216;
            if (original_period < 21) {
                adjusted_period = period;
            } else {
                adjusted_period = original_period * original_period / 20;
            }
            period = adjusted_period;
            flags = waveform & 0x10;
            flags = !flags;
            flags = (flags * 0x10) | 1;
            waveform &= 0x0f;
            channel = track->channel;
            lfo = &channel->lfos[2];
            lfo->flags = flags;
            lfo->destination = 2;
            lfo->waveform = waveform;
            lfo->step = SsdSeqGetLFODepth(depth, (short)period, waveform);
            lfo->period.value = period;
            lfo->phase.value = 0;
            lfo->fade_step.value = 1024;
            lfo->process = WaveformTable[waveform];
            channel->lfo.flags |= 4;
        }
    }
    return events + 3;
}

unsigned char *SsdSeqShakeOn(SsdSequence *sequence, SsdSequenceTrack *track,
                            unsigned char *events)
{
    SsdLfoOn(track, 2);
    return events;
}

unsigned char *SsdSeqShakeOff(SsdSequence *sequence, SsdSequenceTrack *track,
                             unsigned char *events)
{
    SsdLfoOff(track, 2);
    return events;
}

static __inline__ int SsdLfoSingleCycleFlag(int waveform)
{
    return !(waveform & 0x10) << 4;
}

unsigned char *SsdSeqLFOProcess(SsdSequence *sequence, SsdSequenceTrack *track,
                              unsigned char *events)
{
    unsigned char index = events[0];
    SsdChannel *channel = track->channel;
    unsigned char waveform = events[1];
    unsigned char destination = events[2];
    unsigned short mode_flags = SsdLfoSingleCycleFlag(waveform);
    int waveform_index;
    SsdLfoRecord *lfo;
    channel->selected_lfo = index;
    index = channel->selected_lfo;
    lfo = &channel->lfos[index];
    waveform_index = waveform & 15;
    lfo->flags = mode_flags | 1;
    lfo->waveform = waveform_index;
    lfo->destination = destination;
    lfo->step = 0;
    lfo->period.bits = 0;
    lfo->phase.bits = 0;
    lfo->fade_step.bits = 1024;
    lfo->process = WaveformTable[waveform_index];
    return events + 3;
}

unsigned char *SsdSeqLFOParameter(SsdSequence *sequence,
                                SsdSequenceTrack *track,
                                unsigned char *events)
{
    int period;
    unsigned int depth;
    SsdChannel *channel;
    SsdLfoRecord *lfo;
    if (events[0] < 21) {
        period = events[0];
    } else {
        period = events[0] * events[0] / 20;
    }
    depth = events[1] * 0x1000000U | events[2] * 0x10000U;
    channel = track->channel;
    lfo = &channel->lfos[channel->selected_lfo];
    lfo->step = SsdSeqGetLFODepth(depth, period, lfo->waveform);
    lfo->period.value = period;
    return events + 3;
}

unsigned char *SsdSeqLFODelay(SsdSequence *sequence, SsdSequenceTrack *track,
                             unsigned char *events)
{
    int fade_step = 1024;
    int delay = events[0] * 10;
    SsdChannel *channel = track->channel;
    if (events[1])
        fade_step /= events[1] * 4;
    channel->lfos[channel->selected_lfo].phase.bits = delay;
    channel->lfos[channel->selected_lfo].fade_step.bits = fade_step;
    return events + 2;
}

unsigned char *SsdSeqLFOOn(SsdSequence *sequence, SsdSequenceTrack *track,
                           unsigned char *events)
{
    return events + 1;
}

unsigned char *SsdSeqLFOOff(SsdSequence *sequence, SsdChannel *channel,
                          unsigned char *events)
{
    return events + 1;
}

void SsdLfoOn(SsdSequenceTrack *track, int index)
{
}

void SsdLfoOff(SsdSequenceTrack *track, int index)
{
}

unsigned char *SsdSeqAllReset(SsdSequence *sequence, SsdSequenceTrack *track,
                             unsigned char *events)
{
    SsdChannel *channel;
    int masterVolume;
    track->volume = 100 * 65536;
    channel = track->channel;
    track->pan = 63 * 65536;
    track->pitch = 0;
    track->control_flags = 0;
    track->flags &= 0xf4ff;
    masterVolume = channel->volume_scale;
    channel->pan = 63 * 65536;
    channel->volume = 100;
    channel->pitch = 0;
    channel->flags &= ~0x0e;
    channel->output_volume = masterVolume * 12700 / 16129;
    channel->bank = track->bank_id;
    SsdCutoffChannel(channel);
    SsdSetChannelHWEffectOff(channel, 7, 0x7000);
    SsdSetChannelHWEffectOn(channel, 8, 0x8000);
    SsdSetChannelVoiceEvent(channel, 48);
    SsdInitLfoWork(&channel->lfo);
    return events;
}

unsigned char *SsdSeqLabel(SsdSequence *sequence, SsdChannel *channel,
                         unsigned char *events)
{
    return events + 1;
}

unsigned char *SsdSeqSMPTETime(SsdSequence *sequence, SsdSequenceTrack *track,
                             unsigned char *events)
{
    return events + 3;
}

unsigned char *SsdSeqSMPTEFrame(SsdSequence *sequence, SsdChannel *channel,
                              unsigned char *events)
{
    return events + 2;
}

unsigned char *SsdSeqSMPTEOffset(SsdSequence *sequence, SsdSequenceTrack *track,
                                unsigned char *events)
{
    int ticks = events[0] * 1800000 + events[1] * 30000
              + events[2] * 500 + events[3] * 500 / 30;
    sequence->time_offset = (unsigned int)ticks << 8;
    return events + 5;
}

unsigned char *SsdSeqSkip(SsdSequence *sequence, SsdSequenceTrack *track,
                         unsigned char *events)
{
    int speed;
    SsdDevice *device = sequence->device;
    if (events[0]) {
        speed = 16384;
        device->flags |= 1;
    } else {
        speed = 256;
        device->flags &= ~1u;
    }
    sequence->master_tempo.current.value = speed * 65536;
    sequence->master_tempo.remaining = 0;
    sequence->tempo_step = ((sequence->tempo.parts.integer * 53687 >> 8) *
                            (sequence->master_tempo.current.value >> 16)) >> 8;
    return events + 1;
}

/* Handlers of sequence opcodes 0x80-0xff, indexed by opcode - 0x80. */
unsigned char *(*SsdSeqFuncTrap[128])(SsdSequence *, SsdSequenceTrack *, unsigned char *) = {
    /* 0x80 */ SsdSeqDeltaForGate,
    /* 0x81 */ SsdSeqDeltaForAfterDelta,
    /* 0x82 */ SsdSeqDeltaAddGate,
    /* 0x83 */ SsdSeqDeltaAddAfterDelta,
    /* 0x84 */ SsdSeqDelta1,
    /* 0x85 */ SsdSeqDelta2,
    /* 0x86 */ SsdSeqDelta3,
    /* 0x87 */ SsdSeqDummy,
    /* 0x88 */ SsdSeqRest,
    /* 0x89 */ SsdSeqTie,
    /* 0x8a */ SsdSeqDummy,
    /* 0x8b */ SsdSeqDummy,
    /* 0x8c */ SsdSeqDummy,
    /* 0x8d */ SsdSeqDummy,
    /* 0x8e */ SsdSeqDummy,
    /* 0x8f */ SsdSeqDummy,
    /* 0x90 */ SsdSeqStop,
    /* 0x91 */ SsdSeqRepeat,
    /* 0x92 */ SsdSeqJump,
    /* 0x93 */ SsdSeqIf,
    /* 0x94 */ SsdSeqOctaveAbsolute,
    /* 0x95 */ SsdSeqOctaveRelative,
    /* 0x96 */ SsdSeqOctaveUp,
    /* 0x97 */ SsdSeqOctaveDown,
    /* 0x98 */ SsdSeqLoopTop,
    /* 0x99 */ SsdSeqLoopEnd,
    /* 0x9a */ SsdSeqLoopEscape,
    /* 0x9b */ SsdSeqDummy,
    /* 0x9c */ SsdSeqTempoAbsolute,
    /* 0x9d */ SsdSeqTempoRelative,
    /* 0x9e */ SsdSeqDummy,
    /* 0x9f */ SsdSeqDummy,
    /* 0xa0 */ SsdSeqMeasure,
    /* 0xa1 */ SsdSeqPosition,
    /* 0xa2 */ SsdSeqDummy,
    /* 0xa3 */ SsdSeqDummy,
    /* 0xa4 */ SsdSeqDummy,
    /* 0xa5 */ SsdSeqDummy,
    /* 0xa6 */ SsdSeqMasterFader,
    /* 0xa7 */ SsdSeqDummy,
    /* 0xa8 */ SsdSeqBankMSB,
    /* 0xa9 */ SsdSeqBankLSB,
    /* 0xaa */ SsdSeqWaveChange,
    /* 0xab */ SsdSeqWaveBankType,
    /* 0xac */ SsdSeqProgramChange,
    /* 0xad */ SsdSeqDummy,
    /* 0xae */ SsdSeqRandomNote,
    /* 0xaf */ SsdSeqDummy,
    /* 0xb0 */ SsdSeqADSRReset,
    /* 0xb1 */ SsdSeqADSRMode,
    /* 0xb2 */ SsdSeqAR,
    /* 0xb3 */ SsdSeqDR,
    /* 0xb4 */ SsdSeqSL,
    /* 0xb5 */ SsdSeqSR,
    /* 0xb6 */ SsdSeqRR,
    /* 0xb7 */ SsdSeqDRSL,
    /* 0xb8 */ SsdSeqAMode,
    /* 0xb9 */ SsdSeqSMode,
    /* 0xba */ SsdSeqRMode,
    /* 0xbb */ SsdSeqPriority,
    /* 0xbc */ SsdSeqGateTime,
    /* 0xbd */ SsdSeqDummy,
    /* 0xbe */ SsdSeqDummy,
    /* 0xbf */ SsdSeqSustaion,
    /* 0xc0 */ SsdSeqSulrOn,
    /* 0xc1 */ SsdSeqSulrOff,
    /* 0xc2 */ SsdSeqPMOn,
    /* 0xc3 */ SsdSeqPMOff,
    /* 0xc4 */ SsdSeqNoiseAbsolute,
    /* 0xc5 */ SsdSeqNoiseRelative,
    /* 0xc6 */ SsdSeqNoiseOn,
    /* 0xc7 */ SsdSeqNoiseOff,
    /* 0xc8 */ SsdSeqReverbParameter,
    /* 0xc9 */ SsdSeqReverbSet,
    /* 0xca */ SsdSeqReverbOn,
    /* 0xcb */ SsdSeqReverbOff,
    /* 0xcc */ SsdSeqDummy,
    /* 0xcd */ SsdSeqDummy,
    /* 0xce */ SsdSeqDummy,
    /* 0xcf */ SsdSeqDummy,
    /* 0xd0 */ SsdSeqKeyTransposeAbsolute,
    /* 0xd1 */ SsdSeqKeyTransposeRelative,
    /* 0xd2 */ SsdSeqTune,
    /* 0xd3 */ SsdSeqDetune,
    /* 0xd4 */ SsdSeqBender,
    /* 0xd5 */ SsdSeqSweep,
    /* 0xd6 */ SsdSeqDummy,
    /* 0xd7 */ SsdSeqVibrateFade,
    /* 0xd8 */ SsdSeqVibrate1,
    /* 0xd9 */ SsdSeqVibrate2,
    /* 0xda */ SsdSeqVibrateOn,
    /* 0xdb */ SsdSeqVibrateOff,
    /* 0xdc */ SsdSeqDummy,
    /* 0xdd */ SsdSeqBenderOff,
    /* 0xde */ SsdSeqDummy,
    /* 0xdf */ SsdSeqExpression,
    /* 0xe0 */ SsdSeqVolumeAbsolute,
    /* 0xe1 */ SsdSeqVolumeRelative,
    /* 0xe2 */ SsdSeqVolumeFade,
    /* 0xe3 */ SsdSeqTremoloFade,
    /* 0xe4 */ SsdSeqTremolo1,
    /* 0xe5 */ SsdSeqTremolo2,
    /* 0xe6 */ SsdSeqTremoloOn,
    /* 0xe7 */ SsdSeqTremoloOff,
    /* 0xe8 */ SsdSeqPanpotAbsolute,
    /* 0xe9 */ SsdSeqPanpotRelative,
    /* 0xea */ SsdSeqPanpoMove,
    /* 0xeb */ SsdSeqShakeFade,
    /* 0xec */ SsdSeqShake1,
    /* 0xed */ SsdSeqShake2,
    /* 0xee */ SsdSeqShakeOn,
    /* 0xef */ SsdSeqShakeOff,
    /* 0xf0 */ SsdSeqLFOProcess,
    /* 0xf1 */ SsdSeqLFOParameter,
    /* 0xf2 */ SsdSeqLFODelay,
    /* 0xf3 */ SsdSeqDummy,
    /* 0xf4 */ SsdSeqDummy,
    /* 0xf5 */ SsdSeqDummy,
    /* 0xf6 */ SsdSeqLFOOn,
    /* 0xf7 */ SsdSeqLFOOff,
    /* 0xf8 */ SsdSeqAllReset,
    /* 0xf9 */ SsdSeqLabel,
    /* 0xfa */ SsdSeqSMPTETime,
    /* 0xfb */ SsdSeqSMPTEFrame,
    /* 0xfc */ SsdSeqDummy,
    /* 0xfd */ SsdSeqSMPTEOffset,
    /* 0xfe */ SsdSeqSkip,
    /* 0xff */ SsdSeqDummy,
};
