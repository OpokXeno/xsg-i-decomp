#ifndef SSD_OUT_H
#define SSD_OUT_H

#include "ssd/ssd.h"

/* SPU2 volume register values: full scale, and the master volume the driver starts with. */
#define SSD_SPU_VOLUME_MAX 32767
#define SSD_MASTER_VOLUME_DEFAULT 16383

/* SsdGlobalWork.output_mode */
#define SSD_OUTPUT_MONO 0
#define SSD_OUTPUT_STEREO 1
#define SSD_OUTPUT_SURROUND 2

/* SsdGlobalWork.output_flags: output registers to rewrite at the next common-control pass. */
#define SSD_OUTPUT_MASTER_VOLUME 0x3
#define SSD_OUTPUT_CD_VOLUME 0xc0
#define SSD_OUTPUT_CD_REVERB 0x300

/* Reverb mode argument of SsdSetReverbParameter: the core number is in bits 8 and up. */
#define SSD_REVERB_MODE_OFF 0
#define SSD_REVERB_MODE_NONE (-1)
#define SSD_REVERB_MODE_SKIP (-2)

typedef struct SsdStereoVolume {
    short left;
    short right;
} SsdStereoVolume;

typedef struct SsdReverbState {
    unsigned char unmodeled_00[3];
    signed char mode;
    SsdStereoVolume volume;
    signed char delay;
    signed char feedback;
    short depth;
    unsigned int work_end;
    unsigned int work_handle;
    unsigned int unmodeled_14;
} SsdReverbState;

/* Functions and data used by other translation units. */
void SsdInitOutputControl(void);
void SsdSetOutputMode(int mode);
void SsdSetSpuVolume(int volume, int frames);
void SsdSetCDVolume(int volume, int frames);
void SsdSetSequenceOutputVolume(int volume, int unused);
void SsdSetEffectOutputVolume(int volume, int unused);
int SsdSetReverbParameter(int mode, int depth, int delay, int feedback);
void SsdEnableReverbParam(int core);
void SsdSetVolumeRec(int volume, SsdStereoVolume *output, int reverb);
int SsdReverbClearCompleted(int core, int wait);

#endif
