#include "ssd/ssd_lfo.h"
#include "ssd/ssd_init.h"
#include "ssd/ssd_voic.h"

int SsdLfoOffProc(SsdLfoState *state, SsdGlobalWork *work);
int SsdLfoRectangularSingleProc(SsdLfoState *state, SsdGlobalWork *work);
int SsdLfoRectangularDoubleProc(SsdLfoState *state, SsdGlobalWork *work);
int SsdLfoSawtoothSingleProc(SsdLfoState *state, SsdGlobalWork *work);
int SsdLfoNoiseSingleProc(SsdLfoState *state, SsdGlobalWork *work);
int SsdLfoNoiseDoubleProc(SsdLfoState *state, SsdGlobalWork *work);
int SsdGetRSLFORandom(SsdGlobalWork *work);

void SsdInitLfoWork(SsdLfoWork *work)
{
    int i;
    work->active = 0;
    work->pitch = 0;
    work->volume = 0;
    work->pan = 0;
    work->unmodeled_08 = 0;
    work->first = 0;
    for (i = SSD_LFO_STATE_COUNT - 1; i >= 0; --i)
        work->states[i].param.flags = 0;
}

/*
 * Loads the LFO parameter entries. An entry without flags is cleared, and active
 * takes the bit of the last enabled entry.
 */
void SsdInitLfoParam(SsdLfoWork *work, const SsdLfoParam *params)
{
    int i;
    int active;
    work->first = 0;
    active = 0;
    for (i = 0; i < SSD_LFO_STATE_COUNT; ++i) {
        if (params->flags) {
            work->states[i].param = *params;
            work->states[i].process = WaveformTable[params->waveform];
            active = 1 << i;
        } else {
            work->states[i].param.flags = 0;
        }
        ++params;
    }
    work->active = active;
}

/*
 * Rebuilds the work's running LFO list from source: each enabled state is copied,
 * restarted from zero and linked into the list, with its phase flags cleared.
 */
void SsdResetLfoProc(SsdLfoWork *work, SsdLfoWork *source)
{
    int count;
    SsdLfoState *state;
    SsdLfoState *initial;
    work->first = 0;
    work->pan = 0;
    work->volume = 0;
    work->pitch = 0;
    if (source->active) {
        initial = source->states;
        state = work->states;
        for (count = SSD_LFO_STATE_COUNT; count != 0; ) {
            unsigned short flags = initial->param.flags;
            --count;
            if (flags & SSD_LFO_ENABLED) {
                state->param = initial->param;
                state->process = initial->process;
                state->value = 0;
                state->countdown = 1;
                state->delay = initial->param.delay;
                state->fade_level = initial->param.fade_step;
                state->param.flags = flags & ~(SSD_LFO_PHASE_STARTED | SSD_LFO_PHASE_NEGATIVE);
                state->next = work->first;
                work->first = state;
            }
            ++initial;
            ++state;
        }
    }
}

/*
 * Scales a depth for the period: divided by period for the triangular waveforms,
 * and by period - 1 for waveforms 4 and 5 (when period is not 1).
 */
int SsdSeqGetLFODepth(int depth, int period, int waveform)
{
    if (depth && period) {
        switch (waveform) {
        case SSD_LFO_WAVEFORM_TRIANGULAR_SINGLE:
        case SSD_LFO_WAVEFORM_TRIANGULAR_DOUBLE:
            depth /= period;
            break;
        case 4:
            if (period != 1)
                depth /= period - 1;
            break;
        case 5:
            if (period != 1)
                depth /= period - 1;
            break;
        }
    }
    return depth;
}

/*
 * Runs the LFO states of every active voice and adds each output to the voice's
 * volume, pan or pitch total, according to the state's target. Sets the matching event bits.
 */
void SsdLfoControlProc(SsdGlobalWork *work)
{
    int remaining;
    int value;
    int fade;
    int target;
    int events;
    int volumeTarget;
    int panTarget;
    SsdLfoState *state;
    SsdVoice *voice = work->voices;
    remaining = SSD_VOICE_COUNT - 1;
    volumeTarget = SSD_LFO_TARGET_VOLUME;
    panTarget = SSD_LFO_TARGET_PAN;
    do {
        events = 0;
        if (voice->allocation.priority & SSD_VOICE_PRIORITY_ACTIVE) {
            state = voice->lfo.first;
            voice->lfo.pan = 0;
            voice->lfo.volume = 0;
            voice->lfo.pitch = 0;
            /* Each state is evaluated in turn; its value goes to the total for its target. */
            for (; state; state = state->next) {
                if (state->delay) {
                    --state->delay;
                    continue;
                }
                    value = state->process(state, work);
                    fade = state->fade_level;
                    if (fade < SSD_LFO_FADE_FULL) {
                        value = (value >> 10) * fade;
                        state->fade_level = state->param.fade_step + fade;
                    }
                    value >>= 16;
                    target = state->param.target;
                    if (target == volumeTarget)
                        goto accumulate_volume;
                    if (target < SSD_LFO_TARGET_PAN) {
                        if (target == SSD_LFO_TARGET_PITCH)
                            goto accumulate_pitch;
                        continue;
                    }
                    if (target == panTarget)
                        goto accumulate_pan;
                    continue;
accumulate_pitch:
                        events |= SSD_VOICE_EVENT_PITCH;
                        voice->lfo.pitch += value;
                        continue;
accumulate_volume:
                        events |= SSD_VOICE_EVENT_VOLUME;
                        voice->lfo.volume += value;
                        continue;
accumulate_pan:
                        events |= SSD_VOICE_EVENT_VOLUME;
                        voice->lfo.pan += value;
            }
            voice->events |= events;
        }
        --remaining;
        ++voice;
    } while (remaining != -1);
}

int SsdLfoOffProc(SsdLfoState *state, SsdGlobalWork *work)
{
    state->param.flags &= ~SSD_LFO_ENABLED;
    return 0;
}

/* Square wave that toggles between 0 and depth every period. */
int SsdLfoRectangularSingleProc(SsdLfoState *state, SsdGlobalWork *work)
{
    int value = 0;
    if (--state->countdown == 0) {
        state->countdown = state->param.period;
        if (state->value == 0)
            value = state->param.depth;
        state->value = value;
    }
    return state->value;
}

/* Square wave that alternates between +depth and -depth every period. */
int SsdLfoRectangularDoubleProc(SsdLfoState *state, SsdGlobalWork *work)
{
    if (--state->countdown == 0) {
        int depth = state->param.depth;
        state->countdown = state->param.period;
        if (state->param.flags & SSD_LFO_PHASE_NEGATIVE)
            depth = -depth;
        state->value = depth;
        state->param.flags ^= SSD_LFO_PHASE_NEGATIVE;
    }
    return state->value;
}

/* Triangle wave: the value moves by a step that reverses every period. */
int SsdLfoTriangularSingleProc(SsdLfoState *state, SsdGlobalWork *work)
{
    if (--state->countdown == 0) {
        int depth = state->param.depth;
        state->countdown = state->param.period;
        if (state->param.flags & SSD_LFO_PHASE_NEGATIVE)
            depth = -depth;
        state->step = depth;
        state->param.flags ^= SSD_LFO_PHASE_NEGATIVE;
    }
    return state->value += state->step;
}

/*
 * Triangle wave whose step reverses at each change. After the first segment,
 * each segment lasts twice the period.
 */
int SsdLfoTriangularDoubleProc(SsdLfoState *state, SsdGlobalWork *work)
{
    int countdown = state->countdown;
    --countdown;
    if (countdown == 0) {
        unsigned short flags = state->param.flags;
        int depth;
        countdown = state->param.period;
        if (flags & SSD_LFO_PHASE_STARTED)
            countdown *= 2;
        depth = state->param.depth;
        state->step = depth;
        if (flags & SSD_LFO_PHASE_NEGATIVE)
            state->step = -depth;
        state->param.flags = (flags | SSD_LFO_PHASE_STARTED) ^ SSD_LFO_PHASE_NEGATIVE;
    }
    state->countdown = countdown;
    return state->value += state->step;
}

/* Sawtooth wave: adds depth each step and resets to zero every period. */
int SsdLfoSawtoothSingleProc(SsdLfoState *state, SsdGlobalWork *work)
{
    if (--state->countdown == 0) {
        state->value = 0;
        state->countdown = state->param.period;
    } else {
        state->value += state->param.depth;
    }
    return state->value;
}

/* Noise: picks a new random level, scaled by depth, every period. */
int SsdLfoNoiseSingleProc(SsdLfoState *state, SsdGlobalWork *work)
{
    if (--state->countdown == 0) {
        int random;
        state->countdown = state->param.period;
        random = SsdGetRSLFORandom(work);
        state->value = (state->param.depth >> 15) * random;
    }
    return state->value;
}

/* Noise centred on zero: picks a new random level between -depth and +depth every period. */
int SsdLfoNoiseDoubleProc(SsdLfoState *state, SsdGlobalWork *work)
{
    if (--state->countdown == 0) {
        int random;
        state->countdown = state->param.period;
        random = SsdGetRSLFORandom(work);
        state->value = (state->param.depth >> 14) * random - state->param.depth;
    }
    return state->value;
}

/* Advances the work's xorshift random state and returns a 15-bit value. */
int SsdGetRSLFORandom(SsdGlobalWork *work)
{
    int random = work->random_state;
    random ^= random << 17;
    random ^= random >> 15;
    work->random_state = random;
    return random & 0x7fff;
}

/* LFO waveform procedures by waveform number; unused numbers are off. */
SsdLfoProcedure WaveformTable[16] = {
    /*  0 */ SsdLfoRectangularSingleProc,
    /*  1 */ SsdLfoRectangularDoubleProc,
    /*  2 */ SsdLfoTriangularSingleProc,
    /*  3 */ SsdLfoTriangularDoubleProc,
    /*  4 */ SsdLfoSawtoothSingleProc,
    /*  5 */ SsdLfoSawtoothSingleProc,
    /*  6 */ SsdLfoNoiseSingleProc,
    /*  7 */ SsdLfoNoiseDoubleProc,
    /*  8 */ SsdLfoOffProc,
    /*  9 */ SsdLfoOffProc,
    /* 10 */ SsdLfoOffProc,
    /* 11 */ SsdLfoOffProc,
    /* 12 */ SsdLfoOffProc,
    /* 13 */ SsdLfoOffProc,
    /* 14 */ SsdLfoOffProc,
    /* 15 */ SsdLfoOffProc,
};
