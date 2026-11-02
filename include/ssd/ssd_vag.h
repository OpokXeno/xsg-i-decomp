#ifndef SSD_VAG_H
#define SSD_VAG_H

#include "ssd/ssd.h"

/* Functions and data used by other translation units. */
void SsdInitVagStreamStereo(int bytes, void (*callback)(void *), void *arg);
void SsdInitVagStreamMono(int bytes, int channels, void (*callback)(void *), void *arg);
void SsdDisposeVagStream(void);
void SsdPlayVagStream(int volume, int ticks, int pitch);
void SsdStopVagStream(int ticks);
int SsdGetVagStreamStatus(void);
int SsdGetVagStreamParam(int channel, int *pitch, int *volume, int *pan);
void SsdSetVagStreamDataMono(int channel, const unsigned char *source, int bytes);
void SsdSetVagStreamVolume(int channel, int volume, int ticks);
void SsdSetVagStreamParam(int channel, int pitch, int volume, int pan);
void SsdResumeVagStream(void);
void SsdSuspendVagStream(void);

#endif
