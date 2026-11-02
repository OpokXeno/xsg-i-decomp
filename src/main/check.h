#ifndef XENO_MAIN_CHECK_H
#define XENO_MAIN_CHECK_H

#include "shared.h"
#include "main/enemy_1.h"
#include "main/get.h"

typedef struct CheckActorBody {
    float sort_offset;
    float target_angle;
    float radius;
    short lookat_timer;
    unsigned char unmodeled_0e[2];
    short encount_lock;
    unsigned char unmodeled_12[0xe];
    AlignedVector4 saved_position;
} CheckActorBody;

typedef struct CheckActorRecord {
    u32 flags;
    unsigned char unmodeled_04[0x0c];
    AlignedVector4 position;
    unsigned char unmodeled_20[0x62];
    unsigned char kind;
    unsigned char unmodeled_83[3];
    short sound_effect_id;
    unsigned char unmodeled_88[0x9d8 - 0x88];
    long long light_blend_frames;
    CheckActorBody body;
    unsigned char unmodeled_a10[0xa70 - 0xa10];
} CheckActorRecord;

typedef struct CheckActorGameLoopState {
    u32 unmodeled_00;
    CheckActorRecord *player;
} CheckActorGameLoopState;

extern CheckActorRecord actor[64];
extern EnemyWorkEntry enepc[16];
extern CheckActorGameLoopState GameLoopState;

#endif
