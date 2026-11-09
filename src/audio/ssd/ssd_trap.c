#include "ssd/ssd_trap.h"
#include "ssd/ssd_chan.h"
#include "ssd/ssd_err.h"
#include "ssd/ssd_lfo.h"
#include "ssd/ssd_out.h"
#include "ssd/ssd_seq.h"

typedef struct SsdMeasureEvent {
    unsigned char denominator;
    unsigned char numerator;
} SsdMeasureEvent;

void SsdLfoOn(SsdTrack *track, int index);
void SsdLfoOff(SsdTrack *track, int index);

/*
 * Handles an unrecognized sequence opcode: reports it, deactivates the track,
 * cuts its channel and steps back one byte.
 */
unsigned char *SsdSeqDummy(SsdSequence *sequence, SsdTrack *track,
                          unsigned char *events)
{
    SsdSeqErrorProc(SSD_SEQ_ERROR_DUMMY_TRAP, (int)events);
    track->flags &= ~SSD_TRACK_ACTIVE;
    SsdCutoffChannel(track->channel);
    events--;
    Kprintf("Ssd exec dummy trap\n");
    return events;
}

unsigned char *SsdSeqDeltaForAfterDelta(SsdSequence *sequence,
                                      SsdTrack *track,
                                      unsigned char *events)
{
    track->wait = track->delta;
    return events;
}

unsigned char *SsdSeqDeltaForGate(SsdSequence *sequence,
                                SsdTrack *track,
                                unsigned char *events)
{
    unsigned int duration = track->duration;
    track->delta = duration;
    track->wait = duration;
    return events;
}

/*
 * 0x82: the next wait is the previous delta plus the signed byte that follows.
 * The names of 0x82 and 0x83 look swapped against the 0x80/0x81 pair: this one
 * adds to the delta, and 0x83 adds to the gate (the last note's duration).
 */
unsigned char *SsdSeqDeltaAddGate(SsdSequence *sequence, SsdTrack *track,
                                unsigned char *events)
{
    int delta = (signed char)events[0] + track->delta;
    track->delta = delta;
    track->wait = delta;
    return events + 1;
}

unsigned char *SsdSeqDeltaAddAfterDelta(SsdSequence *sequence,
                                     SsdTrack *track,
                                     unsigned char *events)
{
    unsigned int duration = (signed char)events[0] + track->duration;
    track->delta = duration;
    track->wait = duration;
    return events + 1;
}

unsigned char *SsdSeqDelta1(SsdSequence *sequence, SsdTrack *track,
                          unsigned char *events)
{
    int delta = events[0];
    track->delta = delta;
    track->wait = delta;
    return events + 1;
}

unsigned char *SsdSeqDelta2(SsdSequence *sequence, SsdTrack *track,
                           unsigned char *events)
{
    unsigned int duration = events[0] + (events[1] << 8);
    track->delta = duration;
    track->wait = duration;
    return events + 2;
}

unsigned char *SsdSeqDelta3(SsdSequence *sequence, SsdTrack *track,
                          unsigned char *events)
{
    int delta = events[0];
    delta += events[1] << 8;
    delta += events[2] << 16;
    track->delta = delta;
    track->wait = delta;
    return events + 3;
}

/*
 * Sets the track to rest for the 16-bit duration that follows, and clears the
 * channel's slur and sustain flags.
 */
unsigned char *SsdSeqRest(SsdSequence *sequence, SsdTrack *track,
                         unsigned char *events)
{
    unsigned int duration = events[0] + (events[1] << 8);
    unsigned short flags = track->flags;
    SsdChannel *channel = track->channel;

    flags = (flags | SSD_TRACK_REST) & 0xe4ff;
    track->wait = duration;
    track->flags = flags;
    channel->flags &= ~(SSD_CHANNEL_SLUR | SSD_CHANNEL_SUSTAIN);
    return events + 2;
}

/* Sets the track's wait from the 16-bit value that follows and marks it as a note. */
unsigned char *SsdSeqTie(SsdSequence *sequence, SsdTrack *track,
                       unsigned char *events)
{
    track->wait = events[0] + (events[1] << 8);
    track->flags |= SSD_TRACK_NOTE;
    return events + 2;
}

/*
 * Ends the track: returns to the saved repeat point when one is set, otherwise
 * cuts the channel and clears the track flags.
 */
unsigned char *SsdSeqStop(SsdSequence *sequence, SsdTrack *track,
                         unsigned char *events)
{
    unsigned char *repeat = track->jump;
    track->delta = 0;
    track->duration = 0;
    if (repeat) {
        events = repeat;
        track->gate_ticks++;
        track->octave = track->repeat_octave;
    } else {
        SsdCutoffChannel(track->channel);
        events--;
        track->flags = 0;
    }
    return events;
}

/* Saves the current position and octave as the track's repeat point. */
unsigned char *SsdSeqRepeat(SsdSequence *sequence, SsdTrack *track,
                          unsigned char *events)
{
    track->jump = events;
    track->delta = 0;
    track->duration = 0;
    track->repeat_octave = track->octave;
    return events;
}

/* Moves the event pointer by the signed 16-bit displacement that follows. */
unsigned char *SsdSeqJump(SsdSequence *sequence, SsdTrack *track,
                         unsigned char *events)
{
    int displacement = events[0] | (events[1] << 8);
    return events + (short)(displacement - 1);
}

/*
 * Compares the sequence signal with the byte that follows the opcode. On a
 * match it jumps by the 16-bit displacement after that byte; otherwise it
 * skips the whole operand.
 */
unsigned char *SsdSeqIf(SsdSequence *sequence, SsdTrack *track,
                      unsigned char *events)
{
    if (sequence->signal == events[0]) {
        int displacement = events[1] | (events[2] << 8);
        events += (short)(displacement - 1);
    } else {
        events += 3;
    }
    return events;
}

unsigned char *SsdSeqOctaveAbsolute(SsdSequence *sequence,
                                  SsdTrack *track,
                                  unsigned char *events)
{
    track->octave = events[0] * 12;
    return events + 1;
}

unsigned char *SsdSeqOctaveRelative(SsdSequence *sequence,
                                  SsdTrack *track,
                                  unsigned char *events)
{
    track->octave = (track->octave + (signed char)events[0] * 12) & 0x7f;
    return events + 1;
}

unsigned char *SsdSeqOctaveUp(SsdSequence *sequence, SsdTrack *track,
                             unsigned char *events)
{
    track->octave += 12;
    return events;
}

unsigned char *SsdSeqOctaveDown(SsdSequence *sequence, SsdTrack *track,
                              unsigned char *events)
{
    track->octave -= 12;
    return events;
}

/* Opens a loop on the next nesting level; the byte that follows is its count. */
unsigned char *SsdSeqLoopTop(SsdSequence *sequence, SsdTrack *track,
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

/*
 * Ends a loop pass: while repeats remain, records this point as the escape
 * target and jumps back to the loop start; otherwise pops the loop.
 */
unsigned char *SsdSeqLoopEnd(SsdSequence *sequence, SsdTrack *track,
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

/* On the last pass of the innermost loop, jumps to its escape point and pops it. */
unsigned char *SsdSeqLoopEscape(SsdSequence *sequence, SsdTrack *track,
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
                                 SsdTrack *track,
                                 unsigned char *events)
{
    int tempo = events[0];
    int step = (tempo * SSD_TEMPO_CLOCK_SCALE) >> 8;
    sequence->tempo.value = tempo << 16;
    sequence->clock_step = (step * sequence->master_tempo.current.parts.integer) >> 8;
    return events + 1;
}

unsigned char *SsdSeqTempoRelative(SsdSequence *sequence,
                                  SsdTrack *track,
                                  unsigned char *events)
{
    int tempo = sequence->tempo.value + (*events << 16);
    int scaled_tempo = ((tempo >> 16) * SSD_TEMPO_CLOCK_SCALE) >> 8;
    sequence->tempo.value = tempo;
    sequence->clock_step = (scaled_tempo * sequence->master_tempo.current.parts.integer) >> 8;
    return events + 1;
}

/*
 * Changes the tempo toward the target over the given duration, or sets it at
 * once when the duration is 0.
 */
unsigned char *SsdSeqTempoChange(SsdSequence *sequence,
                               SsdTrack *track,
                               unsigned char *events)
{
    int target = events[1];
    int duration = events[0];
    sequence->tempo_target = target << 8;
    if (duration == 0) {
        int step = (target * SSD_TEMPO_CLOCK_SCALE) >> 8;
        sequence->tempo_duration = 0;
        sequence->tempo.value = target << 16;
        sequence->clock_step = (step * sequence->master_tempo.current.parts.integer) >> 8;
    } else {
        int difference = (target << 16) - sequence->tempo.value;
        if (difference != 0) {
            sequence->tempo_increment = difference / duration;
            sequence->tempo_duration = duration;
        }
    }
    return events + 2;
}

unsigned char *SsdSeqMeasure(SsdSequence *sequence, SsdTrack *track,
                           unsigned char *events)
{
    unsigned short denominator = events[0];
    int ticks_per_beat = SSD_TICKS_PER_WHOLE_NOTE / denominator;
    int numerator = events[1];
    do {
        sequence->time_denominator = denominator;
    } while (0);
    sequence->beats_per_measure = numerator;
    sequence->time_numerator = numerator;
    do {
        do {
            sequence->ticks_per_beat = ticks_per_beat;
        } while (0);
    } while (0);
    return events + 2;
}

unsigned char *SsdSeqPosition(SsdSequence *sequence, SsdTrack *track,
                            unsigned char *events)
{
    sequence->measure = events[0] - 1;
    sequence->beat = events[1] - 1;
    sequence->beat_tick = 0;
    return events + 2;
}

/*
 * Fades the master volume to the target over the given number of seconds, or
 * sets it at once when that is 0.
 */
unsigned char *SsdSeqMasterFader(SsdSequence *sequence,
                                SsdTrack *track,
                                unsigned char *events)
{
    int target = events[0] << 16;
    int duration = events[1] * SSD_TICKS_PER_SECOND;

    sequence->master_volume.target = target;
    if (duration == 0) {
        sequence->master_volume.current.value = target;
        sequence->master_volume.remaining = 0;
    } else {
        int difference = target - sequence->master_volume.current.value;
        if (difference != 0) {
            sequence->master_volume.step = difference / duration;
            sequence->master_volume.remaining = duration;
        }
    }
    return events + 2;
}

/*
 * 0xA8 sets the low byte of the channel's bank id and 0xA9 the high byte, the
 * reverse of what the MSB/LSB names suggest: BATTLE2.SMD selects bank 0x0201
 * with a8 01 a9 02.
 */
unsigned char *SsdSeqBankMSB(SsdSequence *sequence, SsdTrack *track,
                           unsigned char *events)
{
    SsdChannel *channel = track->channel;
    channel->bank = events[0] + (channel->bank & 0xff00);
    return events + 1;
}

unsigned char *SsdSeqBankLSB(SsdSequence *sequence, SsdTrack *track,
                           unsigned char *events)
{
    SsdChannel *channel = track->channel;
    channel->bank = (channel->bank & 0xff) + (events[0] << 8);
    return events + 1;
}

unsigned char *SsdSeqWaveChange(SsdSequence *sequence, SsdTrack *track,
                              unsigned char *events)
{
    SsdSetChannelWave(track->channel, events[0]);
    return events + 1;
}

unsigned char *SsdSeqWaveBankType(SsdSequence *sequence,
                                 SsdTrack *track,
                                 unsigned char *events)
{
    SsdChannel *channel = track->channel;
    channel->bank = events[2] + (events[1] << 8);
    SsdSetChannelWave(channel, events[0]);
    return events + 3;
}

unsigned char *SsdSeqProgramChange(SsdSequence *sequence,
                                 SsdTrack *track,
                                 unsigned char *events)
{
    SsdSetChannelProgram(track->channel, events[0]);
    return events + 1;
}

/*
 * Turns random-key note selection on between the two note bytes that follow,
 * or off when the first byte is zero.
 */
unsigned char *SsdSeqRandomNote(SsdSequence *sequence, SsdTrack *track,
                              unsigned char *events)
{
    SsdChannel *channel = track->channel;
    if (events[0]) {
        channel->flags |= SSD_CHANNEL_FIXED_PITCH | SSD_CHANNEL_RANDOM_KEY;
        channel->random_note_low = events[1];
        channel->random_note_high = events[2];
    } else {
        channel->flags &= ~(SSD_CHANNEL_FIXED_PITCH | SSD_CHANNEL_RANDOM_KEY);
    }
    return events + 3;
}

/* No-op handler: the opcode takes no operands. */
unsigned char *SsdSeqADSRReset(SsdSequence *sequence, SsdTrack *track,
                             unsigned char *events)
{
    return events;
}

/* No-op handler: skips the three operand bytes. */
unsigned char *SsdSeqADSRMode(SsdSequence *sequence, SsdTrack *track,
                            unsigned char *events)
{
    return events + 3;
}

unsigned char *SsdSeqAR(SsdSequence *sequence, SsdTrack *track,
                      unsigned char *events)
{
    SsdChannel *channel = track->channel;
    int adsr = (events[0] << 8) | (channel->adsr1 & 0x80ff);
    SsdVoice *voice = channel->voices;
    channel->adsr1 = adsr;
    while (voice != 0) {
        voice->adsr1 = adsr;
        voice->flags |= SSD_VOICE_FLAG_ADSR1;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqDR(SsdSequence *sequence, SsdTrack *track,
                       unsigned char *events)
{
    SsdChannel *channel = track->channel;
    unsigned int adsr1 = (events[0] << 4) | (channel->adsr1 & 0xff0f);
    SsdVoice *voice;
    voice = channel->voices;
    channel->adsr1 = adsr1;
    while (voice) {
        voice->adsr1 = adsr1;
        voice->flags |= SSD_VOICE_FLAG_ADSR1;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqSL(SsdSequence *sequence, SsdTrack *track,
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
        voice->flags |= SSD_VOICE_FLAG_ADSR1;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqSR(SsdSequence *sequence, SsdTrack *track,
                       unsigned char *events)
{
    SsdChannel *channel = track->channel;
    unsigned int adsr2 = (events[0] << 6) | (channel->adsr2 & 0xe03f);
    SsdVoice *voice = channel->voices;
    channel->adsr2 = adsr2;
    while (voice) {
        voice->adsr2 = adsr2;
        voice->flags |= SSD_VOICE_FLAG_ADSR2;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqRR(SsdSequence *sequence, SsdTrack *track,
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
        voice->flags |= SSD_VOICE_FLAG_ADSR2;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqDRSL(SsdSequence *sequence, SsdTrack *track,
                         unsigned char *events)
{
    SsdChannel *channel = track->channel;
    unsigned int adsr1 = events[1] | (events[0] << 4) |
                         (channel->adsr1 & 0xff00);
    SsdVoice *voice = channel->voices;
    channel->adsr1 = adsr1;
    while (voice) {
        voice->adsr1 = adsr1;
        voice->flags |= SSD_VOICE_FLAG_ADSR1;
        voice = voice->next;
    }
    return events + 2;
}

unsigned char *SsdSeqAMode(SsdSequence *sequence, SsdTrack *track,
                         unsigned char *events)
{
    SsdChannel *channel = track->channel;
    unsigned int adsr = (events[0] << 15) | (channel->adsr1 & 0x7fff);
    SsdVoice *voice = channel->voices;
    channel->adsr1 = adsr;
    while (voice != 0) {
        voice->adsr1 = adsr;
        voice->flags |= SSD_VOICE_FLAG_ADSR1;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqSMode(SsdSequence *sequence, SsdTrack *track,
                       unsigned char *events)
{
    SsdChannel *channel = track->channel;
    unsigned int adsr2 = (events[0] << 13) | (channel->adsr2 & 0x3fff);
    SsdVoice *voice = channel->voices;
    channel->adsr2 = adsr2;
    while (voice) {
        voice->adsr2 = adsr2;
        voice->flags |= SSD_VOICE_FLAG_ADSR2;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqRMode(SsdSequence *sequence, SsdTrack *track,
                          unsigned char *events)
{
    int mode = events[0] << 5;
    SsdChannel *channel = track->channel;
    int adsr = mode | (channel->adsr2 & 0xffdf);
    SsdVoice *voice = channel->voices;
    channel->adsr2 = adsr;
    while (voice) {
        voice->adsr2 = adsr;
        voice->flags |= SSD_VOICE_FLAG_ADSR2;
        voice = voice->next;
    }
    return events + 1;
}

unsigned char *SsdSeqPriority(SsdSequence *sequence, SsdTrack *track,
                            unsigned char *events)
{
    track->priority = events[0] & 15;
    return events + 1;
}

unsigned char *SsdSeqGateTime(SsdSequence *sequence, SsdTrack *track,
                            unsigned char *events)
{
    track->gate_ratio = events[0];
    return events + 1;
}

/*
 * Turns the channel's sustain on for values at or above the MIDI switch-on
 * value. Turning it off sends a key-off to the channel.
 */
unsigned char *SsdSeqSustaion(SsdSequence *sequence, SsdTrack *track,
                             unsigned char *events)
{
    SsdChannel *channel = track->channel;
    if (events[0] >= SSD_MIDI_SWITCH_ON) {
        channel->flags |= SSD_CHANNEL_SUSTAIN;
    } else {
        channel->flags &= ~SSD_CHANNEL_SUSTAIN;
        SsdSendChannelKeyoffEvent(channel);
    }
    return events + 1;
}

unsigned char *SsdSeqSulrOn(SsdSequence *sequence, SsdTrack *track,
                           unsigned char *events)
{
    track->flags |= SSD_TRACK_SLUR_MODE;
    return events;
}

unsigned char *SsdSeqSulrOff(SsdSequence *sequence, SsdTrack *track,
                           unsigned char *events)
{
    track->flags &= ~SSD_TRACK_SLUR_MODE;
    return events;
}

unsigned char *SsdSeqPMOn(SsdSequence *sequence, SsdTrack *track,
                        unsigned char *events)
{
    SsdSetChannelHWEffectOn(track->channel, SSD_VOICE_EFFECT_PITCH_MOD, SSD_VOICE_FLAG_PITCH_MOD);
    return events;
}

unsigned char *SsdSeqPMOff(SsdSequence *sequence, SsdTrack *track,
                          unsigned char *events)
{
    SsdSetChannelHWEffectOff(track->channel, SSD_VOICE_EFFECT_PITCH_MOD, SSD_VOICE_FLAG_PITCH_MOD);
    return events;
}

unsigned char *SsdSeqNoiseAbsolute(SsdSequence *sequence, SsdTrack *track,
                                  unsigned char *events)
{
    sequence->noise_clock = events[0];
    SsdSetChannelHWEffectOn(track->channel, SSD_VOICE_EFFECT_NOISE, SSD_VOICE_FLAG_NOISE);
    return events + 1;
}

unsigned char *SsdSeqNoiseRelative(SsdSequence *sequence,
                                 SsdTrack *track,
                                 unsigned char *events)
{
    short noise = sequence->noise_clock;
    int delta = (signed char)events[0];
    noise += delta;
    sequence->noise_clock = noise & 0x3f;
    SsdSetChannelHWEffectOn(track->channel, SSD_VOICE_EFFECT_NOISE, SSD_VOICE_FLAG_NOISE);
    return events + 1;
}

unsigned char *SsdSeqNoiseOn(SsdSequence *sequence, SsdTrack *track,
                           unsigned char *events)
{
    SsdSetChannelHWEffectOn(track->channel, SSD_VOICE_EFFECT_NOISE, SSD_VOICE_FLAG_NOISE);
    return events;
}

unsigned char *SsdSeqNoiseOff(SsdSequence *sequence, SsdTrack *track,
                          unsigned char *events)
{
    SsdSetChannelHWEffectOff(track->channel, SSD_VOICE_EFFECT_NOISE, SSD_VOICE_FLAG_NOISE);
    return events;
}

/* Stores the reverb depth, delay and feedback from the three operand bytes and applies them. */
unsigned char *SsdSeqReverbParameter(SsdSequence *sequence, SsdTrack *track,
                                  unsigned char *events)
{
    int parameter = events[0];
    int left;
    int right;
    sequence->reverb_depth = parameter << 8;
    left = (signed char)events[1];
    sequence->reverb_delay = left;
    right = (signed char)events[2];
    sequence->reverb_feedback = right;
    SsdSetReverbParameter(-1, (short)(parameter << 8), left, right);
    return events + 3;
}

/* Turns the channel's dry and reverb effects on or off from the two flag bytes. */
unsigned char *SsdSeqReverbSet(SsdSequence *sequence, SsdTrack *track,
                             unsigned char *events)
{
    SsdChannel *channel = track->channel;
    if (events[0])
        SsdSetChannelHWEffectOn(channel, SSD_VOICE_EFFECT_DRY, SSD_VOICE_FLAG_DRY);
    else
        SsdSetChannelHWEffectOff(channel, SSD_VOICE_EFFECT_DRY, SSD_VOICE_FLAG_DRY);
    if (events[1])
        SsdSetChannelHWEffectOn(channel, SSD_VOICE_EFFECT_REVERB, SSD_VOICE_FLAG_REVERB);
    else
        SsdSetChannelHWEffectOff(channel, SSD_VOICE_EFFECT_REVERB, SSD_VOICE_FLAG_REVERB);
    return events + 2;
}

unsigned char *SsdSeqReverbOn(SsdSequence *sequence, SsdTrack *track,
                            unsigned char *events)
{
    SsdSetChannelHWEffectOn(track->channel, SSD_VOICE_EFFECT_REVERB, SSD_VOICE_FLAG_REVERB);
    return events;
}

unsigned char *SsdSeqReverbOff(SsdSequence *sequence, SsdTrack *track,
                          unsigned char *events)
{
    SsdSetChannelHWEffectOff(track->channel, SSD_VOICE_EFFECT_REVERB, SSD_VOICE_FLAG_REVERB);
    return events;
}

unsigned char *SsdSeqKeyTransposeAbsolute(SsdSequence *sequence, SsdTrack *track,
                                         unsigned char *events)
{
    int key = events[0];
    SsdChannel *channel = track->channel;
    channel->transpose = key << 8;
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_PITCH);
    return events + 1;
}

unsigned char *SsdSeqKeyTransposeRelative(SsdSequence *sequence,
                                        SsdTrack *track,
                                        unsigned char *events)
{
    int offset = (short)(events[0] << 8);
    SsdChannel *channel = track->channel;
    channel->transpose += offset;
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_PITCH);
    return events + 1;
}

unsigned char *SsdSeqTune(SsdSequence *sequence, SsdTrack *track,
                        unsigned char *events)
{
    short operand = events[0] << 8;
    SsdChannel *channel = track->channel;
    int adjustment = operand >> 5;
    channel->transpose += adjustment;
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_PITCH);
    return events + 1;
}

unsigned char *SsdSeqDetune(SsdSequence *sequence, SsdTrack *track,
                           unsigned char *events)
{
    SsdChannel *channel = track->channel;
    int delta = events[0] + (short)(events[1] << 8);
    channel->transpose += delta;
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_PITCH);
    return events + 2;
}

/* Sets the channel's pitch bend from the two operand bytes and cancels any pitch ramp. */
unsigned char *SsdSeqBender(SsdSequence *sequence, SsdTrack *track,
                          unsigned char *events)
{
    int low = events[0];
    int high = (signed char)events[1];
    SsdChannel *channel = track->channel;
    int bend;
    track->pitch.remaining = 0;
    bend = low * 65536 + high * 16777216;
    track->pitch.current.value = bend;
    channel->pitch_offset = bend;
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_PITCH);
    track->control_flags &= ~SSD_TRACK_CONTROL_PITCH_RAMP;
    return events + 2;
}

/*
 * Starts a pitch ramp over the duration. A nonzero operand is an offset from
 * the current pitch; zero ramps to no pitch. Does nothing when the duration or
 * the difference is zero.
 */
unsigned char *SsdSeqSweep(SsdSequence *sequence, SsdTrack *track,
                         unsigned char *events)
{
    unsigned short duration = events[0] | (events[1] << 8);
    int target = (signed char)events[2] * 16777216;
    int difference;
    if (target) {
        target += track->pitch.current.value;
        difference = target - track->pitch.current.value;
    } else {
        difference = -track->pitch.current.value;
    }
    if (duration && difference) {
        int step = difference / duration;
        unsigned short flags = track->control_flags;
        SsdChannel *channel = track->channel;
        track->pitch.target = target;
        track->pitch.remaining = duration;
        track->control_flags = flags | SSD_TRACK_CONTROL_PITCH_RAMP;
        track->pitch.step = step;
        channel->pitch_offset = track->pitch.current.value;
        SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_PITCH);
    }
    return events + 3;
}

/* Sets the vibrato fade step from the duration byte; a zero duration gives the full step. */
unsigned char *SsdSeqVibrateFade(SsdSequence *sequence, SsdTrack *track,
                               unsigned char *events)
{
    int duration = events[0];
    SsdChannel *channel = track->channel;
    int step = SSD_LFO_FADE_FULL;
    if (duration != 0) {
        step /= duration * 4;
    }
    channel->lfo.states[SSD_LFO_SLOT_VIBRATO].param.fade_step = step;
    return events + 1;
}

/*
 * Enables the vibrato LFO as a triangle wave from the depth, rate and delay
 * bytes. Does nothing when depth or rate is zero.
 */
unsigned char *SsdSeqVibrate1(SsdSequence *sequence, SsdTrack *track,
                             unsigned char *events)
{
    int depth = (signed char)events[1];
    int rate = events[0];
    if (depth && rate) {
        int delay;
        SsdChannel *channel;
        SsdLfoState *vibrato;

        depth = depth < 0 ? -(depth * depth) * 16384
                          : (depth * depth) * 16384;
        rate = rate > SSD_LFO_RATE_LINEAR_LIMIT ?
            rate * rate / SSD_LFO_RATE_LINEAR_LIMIT : rate;
        channel = track->channel;
        vibrato = &channel->lfo.states[SSD_LFO_SLOT_VIBRATO];
        delay = events[2] * 10;
        vibrato->param.flags = SSD_LFO_ENABLED | SSD_LFO_SINGLE_CYCLE;
        vibrato->param.target = SSD_LFO_TARGET_PITCH;
        vibrato->param.waveform = SSD_LFO_WAVEFORM_TRIANGULAR_DOUBLE;
        vibrato->param.depth = SsdSeqGetLFODepth(depth, rate, SSD_LFO_WAVEFORM_TRIANGULAR_DOUBLE);
        vibrato->param.fade_step = SSD_LFO_FADE_FULL;
        vibrato->param.period = rate;
        vibrato->param.delay = delay;
        vibrato->process = SsdLfoTriangularDoubleProc;
        channel->lfo.active |= 1 << SSD_LFO_SLOT_VIBRATO;
    }
    return events + 3;
}

/*
 * Enables the vibrato LFO with the waveform from the operand bytes; the
 * continuous flag in the waveform byte selects a looping cycle. Does nothing
 * when depth or speed is zero.
 */
unsigned char *SsdSeqVibrate2(SsdSequence *sequence, SsdTrack *track,
                           unsigned char *events)
{
    unsigned short speed = events[0];
    int depth = (signed char)events[1];
    unsigned char type = events[2];
    if (depth && speed) {
        short originalSpeed;
        short adjustedSpeed;
        unsigned short flags;
        SsdChannel *channel;
        SsdLfoState *vibrato;
        if (depth < 0)
            depth = -(depth * depth) * 16384;
        else
            depth = depth * depth * 16384;
        originalSpeed = speed;
        if (originalSpeed < SSD_LFO_RATE_LINEAR_LIMIT + 1)
            adjustedSpeed = speed;
        else
            adjustedSpeed = originalSpeed * originalSpeed / SSD_LFO_RATE_LINEAR_LIMIT;
        speed = adjustedSpeed;
        flags = type & SSD_LFO_OPERAND_CONTINUOUS;
        flags = !flags;
        flags = (flags * SSD_LFO_SINGLE_CYCLE) | SSD_LFO_ENABLED;
        type &= SSD_LFO_WAVEFORM_MASK;
        channel = track->channel;
        vibrato = &channel->lfo.states[SSD_LFO_SLOT_VIBRATO];
        vibrato->param.flags = flags;
        vibrato->param.target = SSD_LFO_TARGET_PITCH;
        vibrato->param.waveform = type;
        vibrato->param.depth = SsdSeqGetLFODepth(depth, adjustedSpeed, type);
        vibrato->param.period = speed;
        vibrato->param.delay = 0;
        vibrato->param.fade_step = SSD_LFO_FADE_FULL;
        vibrato->process = WaveformTable[type];
        channel->lfo.active |= 1 << SSD_LFO_SLOT_VIBRATO;
    }
    return events + 3;
}

unsigned char *SsdSeqVibrateOn(SsdSequence *sequence, SsdTrack *track,
                             unsigned char *events)
{
    return events;
}

unsigned char *SsdSeqVibrateOff(SsdSequence *sequence, SsdTrack *track,
                              unsigned char *events)
{
    return events;
}

unsigned char *SsdSeqBenderOff(SsdSequence *sequence, SsdTrack *track,
                              unsigned char *events)
{
    SsdChannel *channel = track->channel;
    track->pitch.current.value = 0;
    track->pitch.remaining = 0;
    channel->pitch_offset = 0;
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_PITCH);
    return events;
}

/* Sets the channel's expression and recomputes its volume from the channel, expression and fader volumes. */
unsigned char *SsdSeqExpression(SsdSequence *sequence, SsdTrack *track,
                               unsigned char *events)
{
    SsdChannel *channel = track->channel;
    int expression = events[0];
    channel->expression = expression;
    channel->volume = channel->channel_volume * expression * channel->fader_volume /
        (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_VOLUME);
    return events + 1;
}

unsigned char *SsdSeqVolumeAbsolute(SsdSequence *sequence,
                                  SsdTrack *track,
                                  unsigned char *events)
{
    int volume = (signed char)events[0];
    SsdChannel *channel = track->channel;
    int scaled_volume;
    track->volume.current.value = volume * 65536;
    scaled_volume = volume * channel->expression * channel->fader_volume;
    channel->channel_volume = volume;
    channel->volume = scaled_volume / (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
    track->control_flags &= 0xfef7;
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_VOLUME);
    return events + 1;
}

unsigned char *SsdSeqVolumeRelative(SsdSequence *sequence,
                                  SsdTrack *track,
                                  unsigned char *events)
{
    SsdChannel *channel = track->channel;
    int volume = channel->channel_volume = channel->channel_volume + events[0];
    track->volume.current.value = volume * 65536;
    channel->volume =
        volume * channel->expression * channel->fader_volume /
        (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_VOLUME);
    track->control_flags &= 0xfef7;
    return events + 1;
}

/*
 * Ramps the track volume toward the signed target over the duration, from the
 * current volume. Does nothing when the duration or the difference is zero.
 */
unsigned char *SsdSeqVolumeFade(SsdSequence *sequence, SsdTrack *track,
                               unsigned char *events)
{
    int target = (signed char)events[2] << 16;
    int duration = events[0] + (events[1] << 8);
    int difference;
    difference = target - track->volume.current.value;
    if (duration && difference) {
        int step = difference / duration;
        signed char volume;
        int output_scale;
        SsdChannel *channel;
        track->volume.remaining = duration;
        volume = track->volume.current.value >> 16;
        channel = track->channel;
        track->volume.target = target;
        track->volume.step = step;
        output_scale = channel->fader_volume;
        target = volume;
        channel->channel_volume = volume;
        channel->volume = target *
            channel->expression * output_scale /
            (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
        SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_VOLUME);
        track->control_flags = (track->control_flags | SSD_TRACK_CONTROL_VOLUME_RAMP) & 0xfeff;
    }
    return events + 3;
}

/* Sets the tremolo fade step from the duration byte; a zero duration gives the full step. */
unsigned char *SsdSeqTremoloFade(SsdSequence *sequence, SsdTrack *track,
                                unsigned char *events)
{
    int duration = events[0];
    SsdChannel *channel = track->channel;
    int step = SSD_LFO_FADE_FULL;
    if (duration)
        step = SSD_LFO_FADE_FULL / (duration * 4);
    channel->lfo.states[SSD_LFO_SLOT_TREMOLO].param.fade_step = step;
    return events + 1;
}

/*
 * Enables the tremolo LFO as a single triangle from the depth, period and fade
 * bytes. Does nothing when depth or period is zero.
 */
unsigned char *SsdSeqTremolo1(SsdSequence *sequence, SsdTrack *track,
                            unsigned char *events)
{
    int depth = (signed char)events[1];
    int period = events[0];
    if (depth && period) {
        int fade;
        SsdChannel *channel;
        SsdLfoState *lfo;
        period = period < SSD_LFO_RATE_LINEAR_LIMIT + 1 ?
            period : period * period / SSD_LFO_RATE_LINEAR_LIMIT;
        channel = track->channel;
        fade = events[2] * 10;
        lfo = &channel->lfo.states[SSD_LFO_SLOT_TREMOLO];
        lfo->param.flags = SSD_LFO_ENABLED | SSD_LFO_SINGLE_CYCLE;
        lfo->param.target = SSD_LFO_TARGET_VOLUME;
        lfo->param.waveform = SSD_LFO_WAVEFORM_TRIANGULAR_SINGLE;
        lfo->param.depth = SsdSeqGetLFODepth(depth * 8388608, period, SSD_LFO_WAVEFORM_TRIANGULAR_SINGLE);
        lfo->param.fade_step = SSD_LFO_FADE_FULL;
        lfo->param.period = period;
        lfo->param.delay = fade;
        lfo->process = SsdLfoTriangularSingleProc;
        channel->lfo.active |= 1 << SSD_LFO_SLOT_TREMOLO;
    }
    return events + 3;
}

/*
 * Enables the tremolo LFO with the waveform from the operand bytes; the
 * continuous flag in the waveform byte selects a looping cycle. Does nothing
 * when depth or period is zero.
 */
unsigned char *SsdSeqTremolo2(SsdSequence *sequence, SsdTrack *track,
                            unsigned char *events)
{
    unsigned short period = events[0];
    int depth = (signed char)events[1];
    unsigned char waveform = events[2];
    if (depth != 0) {
        short original_period = period;
        if (original_period != 0) {
            SsdChannel *channel;
            SsdLfoState *lfo;
            short adjusted_period;
            unsigned short flags;
            depth *= 8388608;
            if (original_period < SSD_LFO_RATE_LINEAR_LIMIT + 1) {
                adjusted_period = period;
            } else {
                adjusted_period = original_period * original_period / SSD_LFO_RATE_LINEAR_LIMIT;
            }
            period = adjusted_period;
            flags = waveform & SSD_LFO_OPERAND_CONTINUOUS;
            flags = !flags;
            flags = (flags * SSD_LFO_SINGLE_CYCLE) | SSD_LFO_ENABLED;
            waveform &= SSD_LFO_WAVEFORM_MASK;
            channel = track->channel;
            lfo = &channel->lfo.states[SSD_LFO_SLOT_TREMOLO];
            lfo->param.flags = flags;
            lfo->param.target = SSD_LFO_TARGET_VOLUME;
            lfo->param.waveform = waveform;
            lfo->param.depth = SsdSeqGetLFODepth(depth, adjusted_period, waveform);
            lfo->param.period = period;
            lfo->param.delay = 0;
            lfo->param.fade_step = SSD_LFO_FADE_FULL;
            lfo->process = WaveformTable[waveform];
            channel->lfo.active |= 1 << SSD_LFO_SLOT_TREMOLO;
        }
    }
    return events + 3;
}

unsigned char *SsdSeqTremoloOn(SsdSequence *sequence, SsdTrack *track,
                             unsigned char *events)
{
    return events;
}

unsigned char *SsdSeqTremoloOff(SsdSequence *sequence, SsdTrack *track,
                               unsigned char *events)
{
    return events;
}

unsigned char *SsdSeqPanpotAbsolute(SsdSequence *sequence,
                                  SsdTrack *track,
                                  unsigned char *events)
{
    int pan = ((signed char)events[0] - 1) * 65536;
    SsdChannel *channel;
    if (pan < 0)
        pan = 0;
    channel = track->channel;
    track->pan.current.value = pan;
    channel->panpot = pan;
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_VOLUME);
    track->control_flags &= ~SSD_TRACK_CONTROL_PAN_RAMP;
    return events + 1;
}

unsigned char *SsdSeqPanpotRelative(SsdSequence *sequence,
                                  SsdTrack *track,
                                  unsigned char *events)
{
    int pan = track->pan.current.value + (signed char)events[0] * 65536;
    SsdChannel *channel;
    if (pan < 0) {
        pan = 0;
    }
    if (pan > 0x7e0000) {
        pan = 0x7e0000;
    }
    channel = track->channel;
    track->pan.current.value = pan;
    channel->panpot = pan;
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_VOLUME);
    track->control_flags &= ~SSD_TRACK_CONTROL_PAN_RAMP;
    return events + 1;
}

/*
 * Starts a pan ramp toward the target over the duration. Does nothing when the
 * duration or the difference is zero.
 */
unsigned char *SsdSeqPanpoMove(SsdSequence *sequence, SsdTrack *track,
                              unsigned char *events)
{
    int target = (signed char)events[2] - 1;
    int duration = events[0] + (events[1] << 8);
    int difference;
    if (target < 0)
        target = 0;
    target *= 65536;
    difference = target - track->pan.current.value;
    if (duration && difference) {
        int step = difference / duration;
        unsigned short state = track->control_flags;
        SsdChannel *channel;
        int pan;
        track->pan.target = target;
        channel = track->channel;
        track->pan.remaining = duration;
        pan = track->pan.current.value;
        track->control_flags = state | SSD_TRACK_CONTROL_PAN_RAMP;
        track->pan.step = step;
        channel->panpot = pan;
        SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_VOLUME);
    }
    return events + 3;
}

/* Sets the shake fade step from the duration byte; a zero duration gives the full step. */
unsigned char *SsdSeqShakeFade(SsdSequence *sequence, SsdTrack *track,
                              unsigned char *events)
{
    int duration = events[0];
    SsdChannel *channel = track->channel;
    int step = SSD_LFO_FADE_FULL;
    if (duration)
        step = SSD_LFO_FADE_FULL / (duration * 4);
    channel->lfo.states[SSD_LFO_SLOT_SHAKE].param.fade_step = step;
    return events + 1;
}

/*
 * Enables the shake LFO as a pan triangle from the depth, period and fade
 * bytes. Does nothing when depth or period is zero.
 */
unsigned char *SsdSeqShake1(SsdSequence *sequence, SsdTrack *track,
                          unsigned char *events)
{
    int depth = (signed char)events[1];
    int period = events[0];
    if (depth && period) {
        int fade;
        SsdChannel *channel;
        SsdLfoState *lfo;
        period = period < SSD_LFO_RATE_LINEAR_LIMIT + 1 ?
            period : period * period / SSD_LFO_RATE_LINEAR_LIMIT;
        channel = track->channel;
        fade = events[2] * 10;
        lfo = &channel->lfo.states[SSD_LFO_SLOT_SHAKE];
        lfo->param.flags = SSD_LFO_ENABLED | SSD_LFO_SINGLE_CYCLE;
        lfo->param.target = SSD_LFO_TARGET_PAN;
        lfo->param.waveform = SSD_LFO_WAVEFORM_TRIANGULAR_DOUBLE;
        lfo->param.depth = SsdSeqGetLFODepth(depth * 16777216, period, SSD_LFO_WAVEFORM_TRIANGULAR_DOUBLE);
        lfo->param.fade_step = SSD_LFO_FADE_FULL;
        lfo->param.period = period;
        lfo->param.delay = fade;
        lfo->process = SsdLfoTriangularDoubleProc;
        channel->lfo.active |= 1 << SSD_LFO_SLOT_SHAKE;
    }
    return events + 3;
}

/*
 * Enables the shake LFO with the waveform from the operand bytes; the
 * continuous flag in the waveform byte selects a looping cycle. Does nothing
 * when depth or period is zero.
 */
unsigned char *SsdSeqShake2(SsdSequence *sequence, SsdTrack *track,
                          unsigned char *events)
{
    unsigned short period = events[0];
    int depth = (signed char)events[1];
    unsigned char waveform = events[2];
    if (depth != 0) {
        short original_period = period;
        if (original_period != 0) {
            SsdChannel *channel;
            SsdLfoState *lfo;
            short adjusted_period;
            unsigned short flags;
            depth *= 16777216;
            if (original_period < SSD_LFO_RATE_LINEAR_LIMIT + 1) {
                adjusted_period = period;
            } else {
                adjusted_period = original_period * original_period / SSD_LFO_RATE_LINEAR_LIMIT;
            }
            period = adjusted_period;
            flags = waveform & SSD_LFO_OPERAND_CONTINUOUS;
            flags = !flags;
            flags = (flags * SSD_LFO_SINGLE_CYCLE) | SSD_LFO_ENABLED;
            waveform &= SSD_LFO_WAVEFORM_MASK;
            channel = track->channel;
            lfo = &channel->lfo.states[SSD_LFO_SLOT_SHAKE];
            lfo->param.flags = flags;
            lfo->param.target = SSD_LFO_TARGET_PAN;
            lfo->param.waveform = waveform;
            lfo->param.depth = SsdSeqGetLFODepth(depth, adjusted_period, waveform);
            lfo->param.period = period;
            lfo->param.delay = 0;
            lfo->param.fade_step = SSD_LFO_FADE_FULL;
            lfo->process = WaveformTable[waveform];
            channel->lfo.active |= 1 << SSD_LFO_SLOT_SHAKE;
        }
    }
    return events + 3;
}

unsigned char *SsdSeqShakeOn(SsdSequence *sequence, SsdTrack *track,
                            unsigned char *events)
{
    SsdLfoOn(track, 2);
    return events;
}

unsigned char *SsdSeqShakeOff(SsdSequence *sequence, SsdTrack *track,
                             unsigned char *events)
{
    SsdLfoOff(track, 2);
    return events;
}

static __inline__ int SsdLfoSingleCycleFlag(int waveform)
{
    return !(waveform & SSD_LFO_OPERAND_CONTINUOUS) << 4;
}

/*
 * Selects the LFO slot from the first operand byte and sets its waveform,
 * target and destination. Its depth, period and delay are cleared.
 */
unsigned char *SsdSeqLFOProcess(SsdSequence *sequence, SsdTrack *track,
                              unsigned char *events)
{
    unsigned char index = events[0];
    SsdChannel *channel = track->channel;
    unsigned char waveform = events[1];
    unsigned char destination = events[2];
    unsigned short mode_flags = SsdLfoSingleCycleFlag(waveform);
    int waveform_index;
    SsdLfoState *lfo;
    channel->lfo.selected = index;
    index = channel->lfo.selected;
    lfo = &channel->lfo.states[index];
    waveform_index = waveform & SSD_LFO_WAVEFORM_MASK;
    lfo->param.flags = mode_flags | SSD_LFO_ENABLED;
    lfo->param.waveform = waveform_index;
    lfo->param.target = destination;
    lfo->param.depth = 0;
    lfo->param.period = 0;
    lfo->param.delay = 0;
    lfo->param.fade_step = SSD_LFO_FADE_FULL;
    lfo->process = WaveformTable[waveform_index];
    return events + 3;
}

/* Sets the depth and period of the selected LFO from the operand bytes. */
unsigned char *SsdSeqLFOParameter(SsdSequence *sequence,
                                SsdTrack *track,
                                unsigned char *events)
{
    int period;
    unsigned int depth;
    SsdChannel *channel;
    SsdLfoState *lfo;
    if (events[0] < SSD_LFO_RATE_LINEAR_LIMIT + 1) {
        period = events[0];
    } else {
        period = events[0] * events[0] / SSD_LFO_RATE_LINEAR_LIMIT;
    }
    depth = events[1] * 0x1000000U | events[2] * 0x10000U;
    channel = track->channel;
    lfo = &channel->lfo.states[channel->lfo.selected];
    lfo->param.depth = SsdSeqGetLFODepth(depth, period, lfo->param.waveform);
    lfo->param.period = period;
    return events + 3;
}

/* Sets the selected LFO's delay and fade step from the two operand bytes. */
unsigned char *SsdSeqLFODelay(SsdSequence *sequence, SsdTrack *track,
                             unsigned char *events)
{
    int fade_step = SSD_LFO_FADE_FULL;
    int delay = events[0] * 10;
    SsdChannel *channel = track->channel;
    if (events[1])
        fade_step /= events[1] * 4;
    channel->lfo.states[channel->lfo.selected].param.delay = delay;
    channel->lfo.states[channel->lfo.selected].param.fade_step = fade_step;
    return events + 2;
}

unsigned char *SsdSeqLFOOn(SsdSequence *sequence, SsdTrack *track,
                           unsigned char *events)
{
    return events + 1;
}

unsigned char *SsdSeqLFOOff(SsdSequence *sequence, SsdTrack *track,
                          unsigned char *events)
{
    return events + 1;
}

void SsdLfoOn(SsdTrack *track, int index)
{
}

void SsdLfoOff(SsdTrack *track, int index)
{
}

/*
 * Resets the track and its channel to their defaults, cuts the channel and
 * clears its LFO state.
 */
unsigned char *SsdSeqAllReset(SsdSequence *sequence, SsdTrack *track,
                             unsigned char *events)
{
    SsdChannel *channel;
    int masterVolume;
    track->volume.current.value = SSD_FIXED_VOLUME_DEFAULT;
    channel = track->channel;
    track->pan.current.value = SSD_PANPOT_CENTER_FIXED;
    track->pitch.current.value = 0;
    track->control_flags = 0;
    track->flags &= 0xf4ff;
    masterVolume = channel->fader_volume;
    channel->panpot = SSD_PANPOT_CENTER_FIXED;
    channel->channel_volume = SSD_CHANNEL_VOLUME_DEFAULT;
    channel->pitch_offset = 0;
    channel->flags &= ~0x0e;
    channel->volume = masterVolume * (SSD_CHANNEL_VOLUME_DEFAULT * SSD_MIDI_VALUE_MAX) /
        (SSD_MIDI_VALUE_MAX * SSD_MIDI_VALUE_MAX);
    channel->bank = track->bank_id;
    SsdCutoffChannel(channel);
    SsdSetChannelHWEffectOff(channel,
                             SSD_VOICE_EFFECT_PITCH_MOD | SSD_VOICE_EFFECT_NOISE | SSD_VOICE_EFFECT_REVERB,
                             SSD_VOICE_FLAG_PITCH_MOD | SSD_VOICE_FLAG_NOISE | SSD_VOICE_FLAG_REVERB);
    SsdSetChannelHWEffectOn(channel, SSD_VOICE_EFFECT_DRY, SSD_VOICE_FLAG_DRY);
    SsdSetChannelVoiceEvent(channel, SSD_VOICE_EVENT_PITCH | SSD_VOICE_EVENT_VOLUME);
    SsdInitLfoWork(&channel->lfo);
    return events;
}

unsigned char *SsdSeqLabel(SsdSequence *sequence, SsdTrack *track,
                         unsigned char *events)
{
    return events + 1;
}

unsigned char *SsdSeqSMPTETime(SsdSequence *sequence, SsdTrack *track,
                             unsigned char *events)
{
    return events + 3;
}

unsigned char *SsdSeqSMPTEFrame(SsdSequence *sequence, SsdTrack *track,
                              unsigned char *events)
{
    return events + 2;
}

unsigned char *SsdSeqSMPTEOffset(SsdSequence *sequence, SsdTrack *track,
                                unsigned char *events)
{
    int ticks = events[0] * (SSD_TICKS_PER_SECOND * 60 * 60) +
                events[1] * (SSD_TICKS_PER_SECOND * 60) +
                events[2] * SSD_TICKS_PER_SECOND +
                events[3] * SSD_TICKS_PER_SECOND / SSD_SMPTE_FRAMES_PER_SECOND;
    sequence->play_time = ticks << 8;
    return events + 5;
}

/*
 * Turns skip mode on or off from the operand byte, and sets the master tempo
 * to the skip or normal speed to match.
 */
unsigned char *SsdSeqSkip(SsdSequence *sequence, SsdTrack *track,
                         unsigned char *events)
{
    int speed;
    SsdMidiDevice *device = sequence->device;
    if (events[0]) {
        speed = SSD_MASTER_TEMPO_SKIP;
        device->state |= SSD_DEVICE_SKIP;
    } else {
        speed = SSD_MASTER_TEMPO_NORMAL;
        device->state &= ~SSD_DEVICE_SKIP;
    }
    sequence->master_tempo.current.value = speed * 65536;
    sequence->master_tempo.remaining = 0;
    sequence->clock_step = ((sequence->tempo.parts.integer * SSD_TEMPO_CLOCK_SCALE >> 8) *
                            (sequence->master_tempo.current.value >> 16)) >> 8;
    return events + 1;
}

/* Handlers of sequence opcodes 0x80-0xff, indexed by opcode - 0x80. */
unsigned char *(*SsdSeqFuncTrap[128])(SsdSequence *, SsdTrack *, unsigned char *) = {
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
