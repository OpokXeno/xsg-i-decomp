#ifndef SRC_MAIN_PLAYCONTROL_H
#define SRC_MAIN_PLAYCONTROL_H

#include "main/play.h"

typedef struct PlayControlInitArguments {
    Play *control;
    unsigned short cameraIndex;
    unsigned char unmodeled_06[2];
    int flags;
} PlayControlInitArguments;

typedef struct PlayControlInitTimingArguments {
    Play *control;
    /* These reads surround the native camera store in observable order. */
    volatile unsigned short cameraIndex;
    unsigned char unmodeled_06[2];
    /* Flags is read after the camera selection has been stored. */
    volatile int flags;
    int frameRange[2];
    float frameStep;
} PlayControlInitTimingArguments;

void PLAY_setup(Play *play);

#endif
