#ifndef RSSD_RSSDFUNC_H
#define RSSD_RSSDFUNC_H

#include "rssd/rssdinit.h"

/* Functions and data used by other translation units. */
extern void RssdInit(RssdRpcPacket *request);
extern void RssdSpuDirectRead(RssdRpcPacket *request);
extern void RssdAddWaveData(RssdRpcPacket *request);
extern void RssdNextWaveData(RssdRpcPacket *request);
extern void RssdPlaySequenceData(RssdRpcPacket *request);
extern void RssdAddSequenceData(RssdRpcPacket *request);
extern void RssdAddSeqEffectData(RssdRpcPacket *request);
extern void RssdAddEffectData(RssdRpcPacket *request);
extern void RssdInitSampling(RssdRpcPacket *request);
extern void RssdTransferSampling(RssdRpcPacket *request);
extern void RssdTransferSamplingNext(RssdRpcPacket *request);

#endif
