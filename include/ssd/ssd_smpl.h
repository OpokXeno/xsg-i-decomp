#ifndef SSD_SMPL_H
#define SSD_SMPL_H

#include "ssd/ssd.h"
#include "ssd/ssd_dma.h"

/* Functions and data used by other translation units. */
void SsdInitSampling(int voice_count, unsigned int spu_address, unsigned int buffer_size);
void SsdDisposeSampling(void);
void SsdSetSampleDmaCallback(SsdDmaCallback callback);
void SsdSetSampleKeyoffCallback(void (*callback)(int result, int channel, int argument));
void SsdTransferSampling(int channel, const void *data, unsigned int size);
void SsdPlaySampling(int channel, int pitch, int volume, int pan);
void SsdStopSampling(int channel);
void SsdSetSamplingParam(int channel, int pitch, int volume, int pan);
void SsdSetSamplingEffect(int channel, int dry, int reverb);

#endif
