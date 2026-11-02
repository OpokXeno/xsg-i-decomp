#ifndef SSD_TRAP_H
#define SSD_TRAP_H

#include "ssd/ssd.h"

/* Handler of one sequence opcode; it returns the position of the next event. */
typedef unsigned char *(*SsdSeqTrapHandler)(SsdSequence *sequence, SsdTrack *track,
                                            unsigned char *events);

/* Functions and data used by other translation units. */
extern SsdSeqTrapHandler SsdSeqFuncTrap[128];

#endif
