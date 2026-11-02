#ifndef RSSD_RSSDSTRM_H
#define RSSD_RSSDSTRM_H

#include "rssd/rssdinit.h"

/* Functions and data used by other translation units. */
extern void SsdInitEEPcmStream(int chunk_size, int format);
extern void SsdDisposeEEPcmStream(void);
extern int SsdSetEEPcmStreamData(RssdRpcPacket *request);
extern int SsdGetEEPcmStreamStatus(void);
extern void SsdEEClearRingBuffer(int index);
extern void SsdInitEEVagStreamStereo(int chunk_size, int format);
extern void SsdInitEEVagStreamMono(int chunk_size, int channels, int format);
extern void SsdDisposeEEVagStream(void);
extern void SsdPlayEEVagStream(RssdRpcPacket *request);
extern void SsdStopEEVagStream(RssdRpcPacket *request);
extern int SsdSetEEVagStreamDataStereo(const RssdRpcPacket *request);
extern int SsdSetEEVagStreamDataMono(RssdRpcPacket *request);
extern int SsdGetEEVagStreamStatusStereo(RssdRpcPacket *request);
extern int SsdGetEEVagStreamStatusMono(RssdRpcPacket *request);
extern int SsdGetEEVagStreamStatusAll(RssdRpcPacket *request);

#endif
