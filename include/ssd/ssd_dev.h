#ifndef SSD_DEV_H
#define SSD_DEV_H

#include "ssd/ssd.h"

/* Functions and data used by other translation units. */
void SsdSetVoiceRRAttr(int voice, int rate, int mode);
void SsdSetMasterVolume(int core, int left, int right);
void SsdSetReverb(int core, int enabled);
void SsdSetReverbModeDepth(int core, int left, int right);

#endif
