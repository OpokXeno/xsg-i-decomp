#include "common.h"
#include "play.h"

Play *PLAY_getCurrent(void)
{
    return &playControl;
}

void PLAY_setupDefault(Play *play)
{
    int i;

    play->cameraIndex = -1;
    play->endTime = D_004D7D14;
    play->frameStep = D_004D7D18;
    play->state = 0;
    play->startTime = 0.0f;
    play->currentTime = 0.0f;
    play->flags = 0;
    play->source = 0;

    for (i = 0; i < 32; i++) {
        play->observers[i].argument = 0;
        play->observers[i].method = 0;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/play", PLAY_setup);

void PLAY_setTimeChart(Play *play, void *timeChart)
{
    play->timeChart = timeChart;
}

INCLUDE_ASM("asm/main/nonmatchings/play", PLAY_setObserver);

void PLAY_loadCamera(void)
{
}

INCLUDE_ASM("asm/main/nonmatchings/play", PLAY_ctrl);

TCHParams *PLAY_getCallBackParams(Play *play)
{
    play->callbackParams.class_ref = classJava_xeno_util_TCHParams->instance_class_ref;
    return &play->callbackParams;
}

INCLUDE_ASM("asm/main/nonmatchings/play", PLAY_checkTCH);
