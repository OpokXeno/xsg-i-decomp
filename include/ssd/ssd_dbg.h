#ifndef SSD_DBG_H
#define SSD_DBG_H

#include "ssd/ssd.h"

struct SsdWavePatch;

/* Functions and data used by other translation units. */
int SsdCheckSpuMemory(int verbose);
void SsdDebOpenDevice(void);
void SsdDebCloseDevice(void);
void SsdDebWaveKeyon(struct SsdWavePatch *patch, unsigned int count, int user_id);
void SsdDebWaveKeyoff(int user_id);
void SsdDebPlayEffect(void);

#endif
