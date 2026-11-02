#ifndef INCLUDE_MAIN_SSD_3_H
#define INCLUDE_MAIN_SSD_3_H

#include "shared.h"

/* Functions and data used by other translation units. */
void SsdInitVagStreamStereo(int channel, int volume);
void SsdDisposeVagStream(void);
void SsdStopVagStream(int channel);
void SsdPlayVagStream(int channel, int start, int loop);
int SsdGetVagStreamStatusStereo(void);
void SsdSetVagStreamDataStereo(int channel, int address, int size);

#endif /* INCLUDE_MAIN_SSD_3_H */
