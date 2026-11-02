/*
 * TU-local declarations of main/tu150 (src/main/play.c).
 */

#ifndef SRC_MAIN_PLAY_H
#define SRC_MAIN_PLAY_H

#include "shared.h"
#include "main/jni.h"
#include "main/tcamera.h"
#include "main/fcv2.h"
#include "main/game_init_camera.h"

#define PLAY_FLAG_ENABLED 0x01
#define PLAY_FLAG_MANUAL_TIME 0x02
#define PLAY_FLAG_LOOP 0x04
#define PLAY_SOURCE_FORMAT_MAC 0x004D4143
#define PLAY_PAD_FAST 0x0100
#define PLAY_PAD_SHOW_FRAME 0x0200

typedef struct PlaySource PlaySource;

struct PlaySource {
    unsigned int formatTag;
    unsigned char unmodeled_04[4];
    unsigned short startFrame;
    unsigned short endFrame;
};

typedef struct FCV2Value FCV2Value;

typedef struct PlayPadData {
    struct {
        unsigned char pad_00[0x28];
        unsigned short half_28;
        unsigned short half_2a;
    } buttons;
    unsigned char unmodeled_2c[0x0e];
    signed char axisX;
    signed char axisY;
} PlayPadData;

extern const float playRuntimeFloat;

/*
 * The per-function .lit4 float constants of this TU (still scaffold-owned):
 * playSetupDefaultRate/playSetupDefaultFrameTime (10/3 and 1/30) are PLAY_setupDefault's defaults.
 */
extern const float playSetupDefaultRate;

extern const float playSetupDefaultFrameTime;
extern PlayPadData PadData;

/* PLAY_ctrl loads each frame divisor once on its guarded path. */
extern volatile const float playFrameDivisorPrimary;

extern volatile const float playFrameDivisorSecondary;
extern const char playFrameFormat[];
extern Play playControl;

extern void xglStudioGetCamera(StudioCamera **camera_out, int camera_index);
extern TCamera *TCAMERA_get(int camera_id);
extern float FCV2_getValueAndKey(int *key, FCV2Value *curve, float frame);
extern PlayTCHCurveSet *TCH_getInfo(void *time_chart, unsigned short key_index);
extern void TCAMERA_transMPack(StudioCamera *camera, PlaySource *source, float frame);
extern void TCAMERA_transMPack2(StudioCamera *camera, PlaySource *source, float frame);
extern void SCRIPT_test(SceneObject argument, JavaMethod *method);

#endif
