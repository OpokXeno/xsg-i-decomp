#ifndef SSD_PCM_H
#define SSD_PCM_H

#include "ssd/ssd.h"

/* Stream status values (SsdGetPcmStreamStatus, SsdGetVagStreamStatus). */
#define SSD_STREAM_STOPPED 0
#define SSD_STREAM_PLAYING 1

/* Flags of the PCM and VAG stream records. */
#define SSD_STREAM_FLAG_OPEN 0x1
#define SSD_STREAM_FLAG_SUSPENDED 0x2

/* Functions and data used by other translation units. */
void SsdInitPcmStream(int buffer_bytes, void (*callback)(void *), void *context);
void SsdDisposePcmStream(void);
void SsdPlayPcmStream(int volume, int duration);
void SsdStopPcmStream(int duration);
int SsdGetPcmStreamStatus(void);
void SsdSetPcmStreamData(const void *source);
void SsdSetPcmStreamVolume(int volume, int duration);
void SsdResumePcmStream(void);
void SsdSuspendPcmStream(void);
void SsdStreamCopyMemory(void *destination_buffer, const void *source_buffer, int bytes);

#endif
