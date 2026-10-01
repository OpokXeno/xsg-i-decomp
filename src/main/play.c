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

void PLAY_setup(Play *play)
{
    StudioCamera *studio_camera;
    TCamera *camera;
    float end_time;
    float start_time;
    int camera_index;

    if (play->cameraIndex >= 0) {
        for (camera_index = 0; camera_index < 8; camera_index++) {
            xglStudioGetCamera(&studio_camera, camera_index);
            studio_camera->active = 0;
        }

        xglStudioGetCamera(&studio_camera, play->cameraIndex);
        studio_camera->active = 1;

        camera = TCAMERA_get(play->cameraIndex);
        camera->mode[0] = 0x10;
        camera->mode[1] = 0x10;
        camera->mode[2] = 0x10;
    }

    play->state = 0;
    if (play->source != 0) {
        start_time = (float)play->source->startFrame * D_004D7D1C;
        end_time = (float)play->source->endFrame * D_004D7D1C;
        play->endTime = end_time;
        play->startTime = start_time;
        play->currentTime = start_time;
    }
}

void PLAY_setTimeChart(Play *play, void *timeChart)
{
    play->timeChart = timeChart;
}

void PLAY_setObserver(Play *play, int chart_index, int key_index,
                      SceneObject argument, SceneMethod *method)
{
    PlayObserver *observer;
    int i;

    observer = 0;
    for (i = 0; i < 32; i++) {
        if (play->observers[i].argument == 0) {
            observer = &play->observers[i];
            break;
        }
    }

    if (observer != 0) {
        observer->argument = argument;
        observer->method = method;
        observer->keyIndex = key_index;
        observer->chartIndex = chart_index;
    }
}

void PLAY_loadCamera(void)
{
}

void PLAY_ctrl(void)
{
    Play *play;
    PlayPadData *pad_data;
    TCHParams *callback_params;
    StudioCamera *camera;
    PlayObserver *observer;
    PlayTCHCurveSet *curve_set;
    Vector4 stick;
    float frame_step;
    float debug_frame_step;
    float observer_frame_step;
    float current_time;
    float end_time;
    float start_time;
    float sampled_value;
    int current_frame;
    int end_frame;
    int start_frame;
    int observer_index;
    int curve_index;
    int key;

    play = &playControl;
    pad_data = &PadData;

    if (play->flags & PLAY_FLAG_ENABLED) {
        if (play->cameraIndex >= 0) {
            xglStudioGetCamera(&camera, play->cameraIndex);
            if (camera->state != 2 && play->source != 0) {
                if (play->source->formatTag == PLAY_SOURCE_FORMAT_MAC) {
                    FCV2_setStep(play->frameStep);
                    TCAMERA_transMPack2(camera, play->source,
                                        play->currentTime);
                } else {
                    TCAMERA_transMPack(camera, play->source,
                                       play->currentTime);
                }
            }
        }

        if (play->flags & PLAY_FLAG_MANUAL_TIME) {
            frame_step = play->frameStep;
            if (pad_data->buttons.half_28 & PLAY_PAD_FAST) {
                frame_step *= 10.0f;
            }

            stick.z = 0.0f;
            stick.w = 1.0f;
            stick.x = (float)pad_data->axisX;
            stick.y = (float)(-pad_data->axisY);
            xglVectorLength(&stick.w, &stick);

            if (stick.w > 40.0f) {
                if (stick.x > 0.0f) {
                    play->currentTime = play->currentTime + frame_step;
                }
                if (stick.x <= 0.0f) {
                    play->currentTime = play->currentTime - frame_step;
                }
            }

            if (stick.w > 40.0f ||
                (pad_data->buttons.half_28 & PLAY_PAD_SHOW_FRAME)) {
                debug_frame_step = D_004D7D20;
                current_frame = (int)(play->currentTime / debug_frame_step);
                start_frame = (int)(play->startTime / debug_frame_step);
                end_frame = (int)(play->endTime / debug_frame_step);
                DB_reset(4, 8);
                DB_println(D_004C2308, current_frame, start_frame, end_frame);
            }
        } else {
            play->currentTime = play->currentTime + play->frameStep;
        }

        current_time = play->currentTime;
        end_time = play->endTime;
        if (end_time < current_time) {
            play->state = 0xFF;
            if (play->flags & PLAY_FLAG_LOOP) {
                current_time = play->startTime;
                play->currentTime = current_time;
            } else {
                current_time = end_time;
                play->currentTime = current_time;
            }
        }

        start_time = play->startTime;
        if (current_time < start_time) {
            if (play->flags & PLAY_FLAG_LOOP) {
                play->currentTime = end_time;
            } else {
                play->currentTime = start_time;
            }
        }

        observer_frame_step = D_004D7D24;
        for (observer = play->observers, observer_index = 31;
             observer_index >= 0; observer_index--, observer++) {
            if (observer->argument != 0) {
                callback_params = &play->callbackParams;
                play->callbackParams.frame =
                    (int)(play->currentTime / observer_frame_step);
                curve_index = 0;
                curve_set = TCH_getInfo(play->timeChart, observer->keyIndex);
                if (curve_set->count != 0) {
                    do {
                        sampled_value = FCV2_getValueAndKey(
                            &key, curve_set->curves[curve_index], play->currentTime);
                        callback_params->curveIndex = curve_index;
                        curve_index++;
                        callback_params->value = sampled_value;
                        if (observer->chartIndex == 1 || key != 0) {
                            SCRIPT_test(observer->argument, observer->method);
                        }
                    } while (curve_index < curve_set->count);
                }
            }
        }
    }
}

TCHParams *PLAY_getCallBackParams(Play *play)
{
    play->callbackParams.class_ref = classJava_xeno_util_TCHParams->instance_class_ref;
    return &play->callbackParams;
}

int PLAY_checkTCH(TCHParams *result, PlayTCHCurveSet *curve_set,
                  int stop_after_first, float frame)
{
    int curve_index;
    int key;
    float sampled_value;

    curve_index = 0;
    if (curve_set->count != 0) {
        do {
            sampled_value = FCV2_getValueAndKey(&key,
                                                 curve_set->curves[curve_index],
                                                 frame);
            result->curveIndex = curve_index;
            result->value = sampled_value;

            if (stop_after_first == 1 || key != 0) {
                return 1;
            }

            curve_index++;
        } while (curve_index < curve_set->count);
    }

    return 0;
}
