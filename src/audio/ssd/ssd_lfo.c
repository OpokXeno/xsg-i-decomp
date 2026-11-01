/* Original SSD object: ssd_lfo.c. Untouched storage remains partial. */

typedef struct SsdLfoParam {
    unsigned short flags;
    unsigned char target;
    unsigned char waveform;
    int depth;
    unsigned short period;
    unsigned short delay;
    unsigned short fadeStep;
} SsdLfoParam;
typedef struct SsdLfoState SsdLfoState;
/* One LFO waveform step; only the noise steps also read the driver work. */
typedef int (*SsdLfoProcedure)(SsdLfoState *state, void *ssd);
struct SsdLfoState {
    SsdLfoParam param;
    int (*process)(SsdLfoState *, void *);
    SsdLfoState *next;
    int value;
    int step;
    unsigned short countdown;
    unsigned short delay;
    short fadeLevel;
};
typedef struct SsdLfoWork {
    unsigned char active;
    short pitch;
    short volume;
    short pan;
    short field08;
    SsdLfoState *first;
    SsdLfoState states[4];
} SsdLfoWork;

typedef struct SsdVoiceControl {
    unsigned char index;
    unsigned char core;
    unsigned char voice;
    unsigned char state;
    unsigned char unmodeled_004[20];
    unsigned int priority;
    unsigned char unmodeled_01c[18];
    unsigned short events;
    unsigned char unmodeled_030[24];
    SsdLfoWork lfo;
    unsigned char unmodeled_0f8[8];
} SsdVoiceControl;

typedef struct SsdDriverState {
    unsigned char unmodeled_000[0x230];
    unsigned int key_on[2];
    unsigned int key_off[2];
    unsigned int cutoff[2];
    unsigned int noise[2];
    unsigned int pitch_modulation[2];
    unsigned int effect_left[2];
    unsigned int effect_right[2];
    unsigned int active_voices;
    SsdVoiceControl voices[48];
    unsigned char unmodeled_326c[24];
    int lfoRandom;
} SsdDriverState;

extern SsdLfoProcedure WaveformTable[];

void SsdInitLfoWork(SsdLfoWork *work);
void SsdInitLfoParam(SsdLfoWork *work, SsdLfoParam *params);
void SsdResetLfoProc(SsdLfoWork *work, SsdLfoWork *source);
int SsdSeqGetLFODepth(int depth, int period, int waveform);
void SsdLfoControlProc(SsdDriverState *work);
int SsdLfoOffProc(SsdLfoState *state);
int SsdLfoRectangularSingleProc(SsdLfoState *state);
int SsdLfoRectangularDoubleProc(SsdLfoState *state);
int SsdLfoTriangularSingleProc(SsdLfoState *state);
int SsdLfoTriangularDoubleProc(SsdLfoState *state);
int SsdLfoSawtoothSingleProc(SsdLfoState *state);
int SsdLfoNoiseSingleProc(SsdLfoState *state, void *ssd);
int SsdLfoNoiseDoubleProc(SsdLfoState *state, void *ssd);
int SsdGetRSLFORandom(SsdDriverState *work);

void SsdInitLfoWork(SsdLfoWork *work)
{
    int i;
    work->active = 0;
    work->pitch = 0;
    work->volume = 0;
    work->pan = 0;
    work->field08 = 0;
    work->first = 0;
    for (i = 3; i >= 0; --i)
        work->states[i].param.flags = 0;
}

void SsdInitLfoParam(SsdLfoWork *work, SsdLfoParam *params)
{
    int i;
    int active;
    work->first = 0;
    active = 0;
    for (i = 0; i < 4; ++i) {
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
        for (count = 4; count != 0; ) {
            unsigned short flags = initial->param.flags;
            --count;
            if (flags & 1) {
                state->param = initial->param;
                state->process = initial->process;
                state->value = 0;
                state->countdown = 1;
                state->delay = initial->param.delay;
                state->fadeLevel = initial->param.fadeStep;
                state->param.flags = flags & 0xff9f;
                state->next = work->first;
                work->first = state;
            }
            ++initial;
            ++state;
        }
    }
}

int SsdSeqGetLFODepth(int depth, int period, int waveform)
{
    if (depth && period) {
        switch (waveform) {
        case 2:
        case 3:
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

void SsdLfoControlProc(SsdDriverState *work)
{
    int remaining;
    int value;
    int fade;
    int target;
    int events;
    int volumeTarget;
    int panTarget;
    SsdLfoState *state;
    SsdVoiceControl *voice = work->voices;
    remaining = 47;
    volumeTarget = 1;
    panTarget = 2;
    do {
        events = 0;
        if (voice->priority & 0x1ff) {
            state = voice->lfo.first;
            voice->lfo.pan = 0;
            voice->lfo.volume = 0;
            voice->lfo.pitch = 0;
            for (; state; state = state->next) {
                if (state->delay) {
                    --state->delay;
                    continue;
                }
                    value = state->process(state, work);
                    fade = state->fadeLevel;
                    if (fade < 1024) {
                        value = (value >> 10) * fade;
                        state->fadeLevel = state->param.fadeStep + fade;
                    }
                    value >>= 16;
                    target = state->param.target;
                    if (target == volumeTarget)
                        goto accumulate_volume;
                    if (target < 2) {
                        if (target == 0)
                            goto accumulate_pitch;
                        continue;
                    }
                    if (target == panTarget)
                        goto accumulate_pan;
                    continue;
accumulate_pitch:
                        events |= 0x10;
                        voice->lfo.pitch += value;
                        continue;
accumulate_volume:
                        events |= 0x20;
                        voice->lfo.volume += value;
                        continue;
accumulate_pan:
                        events |= 0x20;
                        voice->lfo.pan += value;
            }
            voice->events |= events;
        }
        --remaining;
        ++voice;
    } while (remaining != -1);
}

int SsdLfoOffProc(SsdLfoState *state)
{
    state->param.flags &= 0xfffe;
    return 0;
}

int SsdLfoRectangularSingleProc(SsdLfoState *state)
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

int SsdLfoRectangularDoubleProc(SsdLfoState *state)
{
    if (--state->countdown == 0) {
        int depth = state->param.depth;
        state->countdown = state->param.period;
        if (state->param.flags & 0x40)
            depth = -depth;
        state->value = depth;
        state->param.flags ^= 0x40;
    }
    return state->value;
}

int SsdLfoTriangularSingleProc(SsdLfoState *state)
{
    if (--state->countdown == 0) {
        int depth = state->param.depth;
        state->countdown = state->param.period;
        if (state->param.flags & 0x40)
            depth = -depth;
        state->step = depth;
        state->param.flags ^= 0x40;
    }
    return state->value += state->step;
}

int SsdLfoTriangularDoubleProc(SsdLfoState *state)
{
    int countdown = state->countdown;
    --countdown;
    if (countdown == 0) {
        unsigned short flags = state->param.flags;
        int depth;
        countdown = state->param.period;
        if (flags & 0x20)
            countdown *= 2;
        depth = state->param.depth;
        state->step = depth;
        if (flags & 0x40)
            state->step = -depth;
        state->param.flags = (flags | 0x20) ^ 0x40;
    }
    state->countdown = countdown;
    return state->value += state->step;
}

int SsdLfoSawtoothSingleProc(SsdLfoState *state)
{
    if (--state->countdown == 0) {
        state->value = 0;
        state->countdown = state->param.period;
    } else {
        state->value += state->param.depth;
    }
    return state->value;
}

int SsdLfoNoiseSingleProc(SsdLfoState *state, void *ssd)
{
    if (--state->countdown == 0) {
        int random;
        state->countdown = state->param.period;
        random = SsdGetRSLFORandom(ssd);
        state->value = (state->param.depth >> 15) * random;
    }
    return state->value;
}

int SsdLfoNoiseDoubleProc(SsdLfoState *state, void *ssd)
{
    if (--state->countdown == 0) {
        int random;
        state->countdown = state->param.period;
        random = SsdGetRSLFORandom(ssd);
        state->value = (state->param.depth >> 14) * random - state->param.depth;
    }
    return state->value;
}

int SsdGetRSLFORandom(SsdDriverState *work)
{
    int random = work->lfoRandom;
    random ^= (unsigned int)random << 17;
    random ^= random >> 15;
    work->lfoRandom = random;
    return random & 0x7fff;
}

/* LFO waveform procedures by waveform number; unused numbers are off. */
SsdLfoProcedure WaveformTable[16] = {
    /*  0 */ (SsdLfoProcedure)SsdLfoRectangularSingleProc,
    /*  1 */ (SsdLfoProcedure)SsdLfoRectangularDoubleProc,
    /*  2 */ (SsdLfoProcedure)SsdLfoTriangularSingleProc,
    /*  3 */ (SsdLfoProcedure)SsdLfoTriangularDoubleProc,
    /*  4 */ (SsdLfoProcedure)SsdLfoSawtoothSingleProc,
    /*  5 */ (SsdLfoProcedure)SsdLfoSawtoothSingleProc,
    /*  6 */ SsdLfoNoiseSingleProc,
    /*  7 */ SsdLfoNoiseDoubleProc,
    /*  8 */ (SsdLfoProcedure)SsdLfoOffProc,
    /*  9 */ (SsdLfoProcedure)SsdLfoOffProc,
    /* 10 */ (SsdLfoProcedure)SsdLfoOffProc,
    /* 11 */ (SsdLfoProcedure)SsdLfoOffProc,
    /* 12 */ (SsdLfoProcedure)SsdLfoOffProc,
    /* 13 */ (SsdLfoProcedure)SsdLfoOffProc,
    /* 14 */ (SsdLfoProcedure)SsdLfoOffProc,
    /* 15 */ (SsdLfoProcedure)SsdLfoOffProc,
};
