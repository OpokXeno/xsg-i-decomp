#ifndef SSD_MAIN_H
#define SSD_MAIN_H

#include "ssd/ssd.h"

/* 16.16 fixed-point value read whole or as halves. */
typedef union SsdFixed16 {
    int value;
    struct {
        unsigned short fraction;
        short integer;
    } parts;
} SsdFixed16;

/* Value that moves linearly to a target over a number of interrupts. */
typedef struct SsdRamp {
    SsdFixed16 current;
    int step;
    int target;
    short remaining;
} SsdRamp;

/* Functions and data used by other translation units. */
void SsdSeqInterruptProcess(SsdGlobalWork *work);
void SsdCalculateModulate(SsdRamp *ramp);

#endif
