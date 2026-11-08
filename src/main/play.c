#include "common.h"
#include "main/play.h"
#include "play.h"
#include "main/jni.h"

#define PLAY_CAMERA_COUNT 8
#define PLAY_CAMERA_MODE_DEFAULT 0x10
#define PLAY_SIGNAL_FINISHED 0xFF
#define PLAY_DEFAULT_END_TIME 3.3333335f
#define PLAY_FAST_STEP_MULTIPLIER 10.0f
#define PLAY_STICK_THRESHOLD 40.0f

const char playFrameFormat[24] = "FRAME %6d (%6d, %6d)\n";

Play playControl;

Play *PLAY_getCurrent(void)
{
    return &playControl;
}

/* Resets `play` to its defaults: no camera, the default end time and frame step, and empty observers. */
void PLAY_setupDefault(Play *play)
{
    int i;

    play->cameraIndex = -1;
    play->endTime = PLAY_DEFAULT_END_TIME;
    play->frameStep = PLAY_SECONDS_PER_FRAME;
    play->signal = 0;
    play->startTime = 0.0f;
    play->currentTime = 0.0f;
    play->flags = 0;
    play->source = 0;

    for (i = 0; i < PLAY_OBSERVER_COUNT; i++) {
        play->observers[i].argument = 0;
        play->observers[i].method = 0;
    }
}

/*
 * Prepares the play for a run. With a camera selected, every studio camera is
 * deactivated except the selected one, whose mode is set. With a source, the
 * start and end times are taken from its first and last frames.
 */
void PLAY_setup(Play *play)
{
    StudioCamera *studio_camera;
    TCamera *camera;
    float end_time;
    float start_time;
    int camera_index;

    if (play->cameraIndex >= 0) {
        for (camera_index = 0; camera_index < PLAY_CAMERA_COUNT; camera_index++) {
            xglStudioGetCamera(&studio_camera, camera_index);
            studio_camera->active = 0;
        }

        xglStudioGetCamera(&studio_camera, play->cameraIndex);
        studio_camera->active = 1;

        camera = TCAMERA_get(play->cameraIndex);
        camera->mode[0] = PLAY_CAMERA_MODE_DEFAULT;
        camera->mode[1] = PLAY_CAMERA_MODE_DEFAULT;
        camera->mode[2] = PLAY_CAMERA_MODE_DEFAULT;
    }

    play->signal = 0;
    if (play->source != 0) {
        start_time = (float)play->source->startFrame * PLAY_SECONDS_PER_FRAME;
        end_time = (float)play->source->endFrame * PLAY_SECONDS_PER_FRAME;
        play->endTime = end_time;
        play->startTime = start_time;
        play->currentTime = start_time;
    }
}

void PLAY_setTimeChart(Play *play, void *timeChart)
{
    play->timeChart = timeChart;
}

/*
 * Stores an observer (argument, method, key index and chart index) in the first
 * free observer slot. Does nothing when every slot is in use.
 */
void PLAY_setObserver(Play *play, int chart_index, int key_index,
                      SceneObject argument, JavaMethod *method)
{
    PlayObserver *observer;
    int i;

    observer = 0;
    for (i = 0; i < PLAY_OBSERVER_COUNT; i++) {
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

/* Empty. */
void PLAY_loadCamera(void)
{
}

/*
 * Advances the current play while it is enabled: moves the camera along its
 * source, steps the time (from the pad in manual mode, otherwise by frameStep),
 * wraps or clamps the time at the ends, and calls the observers for each curve
 * that reports a key at the current time (curves of chart 1 always call).
 */
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

        /* Manual time: the stick scrubs the time, and the frame readout is printed while it is pushed or the show-frame button is held. */
        if (play->flags & PLAY_FLAG_MANUAL_TIME) {
            frame_step = play->frameStep;
            if (pad_data->buttons.half_28 & PLAY_PAD_FAST) {
                frame_step *= PLAY_FAST_STEP_MULTIPLIER;
            }

            stick.z = 0.0f;
            stick.w = 1.0f;
            stick.x = (float)pad_data->axisX;
            stick.y = (float)(-pad_data->axisY);
            xglVectorLength(&stick.w, &stick);

            if (stick.w > PLAY_STICK_THRESHOLD) {
                if (stick.x > 0.0f) {
                    play->currentTime = play->currentTime + frame_step;
                }
                if (stick.x <= 0.0f) {
                    play->currentTime = play->currentTime - frame_step;
                }
            }

            if (stick.w > PLAY_STICK_THRESHOLD ||
                (pad_data->buttons.half_28 & PLAY_PAD_SHOW_FRAME)) {
                debug_frame_step = PLAY_SECONDS_PER_FRAME;
                current_frame = (int)(play->currentTime / debug_frame_step);
                start_frame = (int)(play->startTime / debug_frame_step);
                end_frame = (int)(play->endTime / debug_frame_step);
                DB_reset(4, 8);
                DB_println(playFrameFormat, current_frame, start_frame, end_frame);
            }
        } else {
            play->currentTime = play->currentTime + play->frameStep;
        }

        /* Wrap or clamp the time at the end, then at the start. */
        current_time = play->currentTime;
        end_time = play->endTime;
        if (end_time < current_time) {
            play->signal = PLAY_SIGNAL_FINISHED;
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

        /* Sample each observer's curves at the current time. */
        observer_frame_step = PLAY_SECONDS_PER_FRAME;
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

/* Returns the play's callback parameter block, with its class reference set to xeno.util.TCHParams. */
TCHParams *PLAY_getCallBackParams(Play *play)
{
    play->callbackParams.class_ref = classJava_xeno_util_TCHParams->classPointer;
    return &play->callbackParams;
}

/*
 * Samples the curves of `curve_set` at `frame`, storing the curve index and
 * value in `result`. Returns 1 at the first curve that reports a key (or at the
 * first curve when stop_after_first is 1), otherwise 0.
 */
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
