#ifndef SRC_MAIN_GAME_INIT_CAMERA_H
#define SRC_MAIN_GAME_INIT_CAMERA_H

#include "shared.h"

struct UnitActorStorage;

typedef struct GameInitCameraRecord {
    unsigned char unmodeled_00[0x0c];
    int channel_mode[2];
    unsigned char unmodeled_14[0x1c];
    Vector4 translation_offset;
    unsigned char unmodeled_40[4];
    struct UnitActorStorage *translation_constraint;
    unsigned char unmodeled_48[0x488];
    Vector4 view_offset;
    unsigned char unmodeled_4e0[4];
    struct UnitActorStorage *view_constraint;
    unsigned char unmodeled_4e8[0xdd8];
} GameInitCameraRecord;

extern GameInitCameraRecord tcamera[];
extern struct UnitActorStorage actor;
extern void TCAMERA_init(void);
extern void xglStudioGetCamera(StudioCamera **camera_out, int camera_index);

#endif
