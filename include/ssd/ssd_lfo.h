#ifndef SSD_LFO_H
#define SSD_LFO_H

#include "ssd/ssd.h"

#define SSD_LFO_STATE_COUNT 4

/* SsdLfoParam.target: the voice value the LFO modulates. */
#define SSD_LFO_TARGET_PITCH 0
#define SSD_LFO_TARGET_VOLUME 1
#define SSD_LFO_TARGET_PAN 2

/* Slots of SsdLfoWork.states used by the vibrato, tremolo and shake opcodes. */
#define SSD_LFO_SLOT_VIBRATO 0
#define SSD_LFO_SLOT_TREMOLO 1
#define SSD_LFO_SLOT_SHAKE 2

/* SsdLfoParam.waveform: indexes WaveformTable (the low four bits of the opcode operand). */
#define SSD_LFO_WAVEFORM_TRIANGULAR_SINGLE 2
#define SSD_LFO_WAVEFORM_TRIANGULAR_DOUBLE 3

/* Waveform operand of the LFO opcodes: waveform number, and a bit that makes it run continuously. */
#define SSD_LFO_WAVEFORM_MASK 0x0f
#define SSD_LFO_OPERAND_CONTINUOUS 0x10

/*
 * SsdLfoParam.flags: enabled, single-cycle (set when operand bit 4 is clear)
 * and the two phase bits the waveform procedures toggle while running.
 */
#define SSD_LFO_ENABLED 0x1
#define SSD_LFO_SINGLE_CYCLE 0x10
#define SSD_LFO_PHASE_STARTED 0x20
#define SSD_LFO_PHASE_NEGATIVE 0x40

/* Fade level at which the fade-in is complete, and rate scaling above 20. */
#define SSD_LFO_FADE_FULL 1024
#define SSD_LFO_RATE_LINEAR_LIMIT 20

/* One LFO waveform step; only the noise steps read the driver work. */
typedef int (*SsdLfoProcedure)(SsdLfoState *state, SsdGlobalWork *work);

typedef struct SsdLfoParam {
    unsigned short flags;
    unsigned char target;
    unsigned char waveform;
    int depth;
    unsigned short period;
    unsigned short delay;
    unsigned short fade_step;
} SsdLfoParam;

struct SsdLfoState {
    SsdLfoParam param;
    SsdLfoProcedure process;
    SsdLfoState *next;
    int value;
    int step;
    unsigned short countdown;
    unsigned short delay;
    short fade_level;
};

typedef struct SsdLfoWork {
    unsigned char active;
    unsigned char selected;
    short pitch;
    short volume;
    short pan;
    short unmodeled_08;
    SsdLfoState *first;
    SsdLfoState states[4];
} SsdLfoWork;

/* Functions and data used by other translation units. */
extern SsdLfoProcedure WaveformTable[];

void SsdInitLfoWork(SsdLfoWork *work);
void SsdInitLfoParam(SsdLfoWork *work, const SsdLfoParam *params);
void SsdResetLfoProc(SsdLfoWork *work, SsdLfoWork *source);
int SsdSeqGetLFODepth(int depth, int period, int waveform);
void SsdLfoControlProc(SsdGlobalWork *work);
int SsdLfoTriangularSingleProc(SsdLfoState *state, SsdGlobalWork *work);
int SsdLfoTriangularDoubleProc(SsdLfoState *state, SsdGlobalWork *work);

#endif
