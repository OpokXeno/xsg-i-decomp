#include "common.h"

#include "set_motion.h"

typedef union AlignedHomingPosition {
    Vector4 coordinates;
    float elements[4];
    unsigned long long alignment[2];
} AlignedHomingPosition;

typedef struct EnemyDetectHead {
    unsigned char unmodeled_00[0x10];
    AlignedHomingPosition position;
    unsigned char unmodeled_20[0x70 - 0x20];
} EnemyDetectHead;

typedef struct ActorHearing {
    unsigned char unmodeled_00[0x68];
    float radius;
} ActorHearing;

typedef struct EnemyDetectActor {
    EnemyDetectHead head;
    unsigned char unmodeled_70[0x10];
    unsigned char slot_number;
    unsigned char unmodeled_81;
    unsigned char status;
    unsigned char has_head_position;
    unsigned char unmodeled_84[2];
    short active_state;
    unsigned char unmodeled_88[0xc0 - 0x88];
    ActorHearing hearing;
    unsigned char unmodeled_12c[0x630 - 0x12c];
    float head_position[4];
    unsigned char unmodeled_640[0xa70 - 0x640];
} EnemyDetectActor;

extern EnemyDetectActor actor[64];

extern void EnemySound_Stop(Actor *actor, signed char which);

extern void xglSoundEffectStopDirect(int sound_id);

extern float Get_Distance3D(const Vector4 *origin, const Vector4 *target);

extern PadPrefix PadData;

extern void Check_Discovery(Actor *enemy);

extern EnemyStateBlock enepc[16];

extern void BSpline_Init(float *points, const float *origin, float angle);

extern void Get_MiddlePoint(const float *start, const float *following,
                            short point_index, short point_count,
                            float *result);

extern int RES_GetEnemySeBank(int sound_id);

extern int RES_GetEnemySeType(int sound_id);

extern void xglSoundEffectPosID();

/*
 * Sound_FootStep and Enemy_ActionReady always pass 1 for `play`; when it is
 * not 1 the positional call below is skipped and the function has no other
 * effect. `ignore_se_type` is 0 at Enemy_ActionReady's one sound_index == 1
 * call and 1 everywhere else (main:0x002d3930, 0x002d396c, 0x002d3a84,
 * 0x002d06d8, 0x002d0710, 0x002d0744); when it is 0 and sound_index is 2 or
 * 4, a bank whose RES_GetEnemySeType is 1 is skipped entirely.
 */

extern void xglSoundEffectStopID(int sound_id, int flags);

extern void SsdFadeoutEffect(int effect_id, int source_id, int fade_time);

/*
 * A TU-local view of main/xgl_sound.c's struct SoundWork (main:0x004a8140),
 * not yet reachable through a shared header. effect_banks starts at +0x20
 * and each entry's low halfword is its handle (main/xgl_sound.c's
 * SoundEffectBankEntry.handle), the same field xglSoundEffectStopDirect
 * reads (main:0x00226720).
 */

typedef struct SetMotionSoundWork
{
    unsigned char unmodeled_00[0x20];
    struct
    {
        unsigned short handle;
        unsigned short file_handle;
    } effect_banks[32];
} SetMotionSoundWork;

extern SetMotionSoundWork SoundWork;

typedef struct ActorTalkControls {
    unsigned char unmodeled_00[0x64];
    unsigned int talk_flags;
} ActorTalkControls;

typedef struct ActorTalk {
    ActorHead head;
    unsigned char unmodeled_70[0xc0 - sizeof(ActorHead)];
    ActorTalkControls controls;
    unsigned char unmodeled_128[0x704 - 0x128];
    unsigned short saved_talk_motion;
    unsigned short motion_progress;
    unsigned char unmodeled_708[0x712 - 0x708];
    unsigned short talk_state;
    unsigned char unmodeled_714[0x9e4 - 0x714];
    float target_angle;
} ActorTalk;

extern void Set_Motion(Actor *actor, int motion, unsigned int mode);

extern void Enemy_Command_LookAt(Actor *actor, int target);

extern void Enemy_Command_Freeze(Actor *actor, int frozen);

typedef struct EnemyTalkState {
    unsigned char unmodeled_00[0x37a8];
    int saved_motion;
} EnemyTalkState;

#define ENEMY_ACTION_STATE_OFFSET 0x48

#define ENEMY_ELECTRIC_STATUS_OFFSET 0x6a

typedef struct EnemyEarState {
    unsigned char unmodeled_00[0x37a2];
    signed char ear_disabled;
    unsigned char unmodeled_37a3[0x38ac - 0x37a3];
    short discovery_count;
} EnemyEarState;

/* This flag is a signed byte in the original enemy-work record. */

#define ENEMY_EAR_DISABLED_OFFSET 0x37a2

typedef struct EnemyScriptState {
    unsigned char unmodeled_00[0x37e0];
    unsigned int action_flags;
    unsigned char unmodeled_37e4[0x3800 - 0x37e4];
    float movement_points[2][4];
    short movement_step;
    short movement_duration;
    float start_angle;
    float end_angle;
    short turn_step;
    short turn_duration;
} EnemyScriptState;

typedef struct LayoutHeader LayoutHeader;

typedef struct BeltUndulation {
    unsigned char unmodeled_00[0x18];
    LayoutHeader *header;
    unsigned char unmodeled_1c[4];
    unsigned long long surface_flags;
    unsigned char unmodeled_028[0x18];
} BeltUndulation;

/* The original scratch blocks are 0x40 bytes; only this prefix is modeled. */

BeltUndulation UnduTemp = {0};

typedef struct UnduTestStorage {
    int queryFlags;
    unsigned char unmodeled_04[4];
    short attrMask;
    unsigned char unmodeled_0a[0x0e];
    LayoutHeader *header;
    unsigned char unmodeled_1c[4];
    long long attribute;
    unsigned char unmodeled_028[0x18];
} UnduTestStorage;

UnduTestStorage UnduTest = {0};

static unsigned char idx_0[16] = {0, 1, 3, 2, 5, 0, 4, 0, 7, 8, 0, 0, 6, 0, 0, 0};

static float vec_1[9][2] = {
    {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
    {1.0f, -1.0f}, {0.0f, -1.0f}, {-1.0f, -1.0f}, {-1.0f, 0.0f},
    {-1.0f, 1.0f},
};

static float rate_2_003B2008[4] = {1.0f, 2.0f, 0.5f, 3.0f};

extern void UnduParamInit(BeltUndulation *param);

extern LayoutHeader *UnduDataGetHeader(int map_index, int unit_index);

extern void UnduCheck(const Vector4 *position, void *exclude, BeltUndulation *param);

typedef struct EnemyDiscoveryState {
    unsigned char unmodeled_00[0x48];
    unsigned char detection_state;
    unsigned char unmodeled_49;
    unsigned char action_state;
    unsigned char unmodeled_4b[0x6c - 0x4b];
    short discovery_count;
    unsigned char unmodeled_6e[0x37f0 - 0x6e];
    unsigned long long last_player_position_words[2];
} EnemyDiscoveryState;

typedef struct DiscoveryPlayer {
    unsigned char unmodeled_00[16];
    unsigned long long position_words[2];
    unsigned char unmodeled_20[0x9f0 - 0x20];
    short disable_detection;
} DiscoveryPlayer;

typedef struct DiscoveryGameLoopState {
    unsigned char unmodeled_00[4];
    DiscoveryPlayer *player;
    unsigned char unmodeled_08[8];
} DiscoveryGameLoopState;

extern DiscoveryGameLoopState GameLoopState;

extern signed char FLAG_FRAME_60;

extern void Enemy_ActionReady(Actor *actor, int action);

typedef struct PlayerHistoryActor {
    ActorHead head;
    unsigned char unmodeled_70[0x9e8 - sizeof(ActorHead)];
    float interaction_radius;
} PlayerHistoryActor;

float PlayHis[64][4] = {{0.0f}};

float BackPos[4] = {0.0f, 0.0f, 0.0f, 0.0f};

float LocaterAngle[16] = {
    -3.2f, -3.0f, -2.4f, -2.2f, -1.6f, -1.4f, -0.8f, -0.6f,
    -0.1f, 0.1f, 0.6f, 0.8f, 1.4f, 1.6f, 2.2f, 2.3f,
};

extern short PhCunt[1];

extern int Check_Straight_ID(float heading, float *previous,
                             unsigned char *enemy_state, int limit,
                             float *result, int stride);

typedef struct EnemyHomingState {
    unsigned char unmodeled_00[0x30a0];
    float positions[64][4];
    short neighbours[64][4];
    short neighbour_count[64];
    short linked_target[64];
    unsigned short position_count;
} EnemyHomingState;

extern void Homing_Add(int actor_index, float x, float y, float z,
                       short target_index);

typedef struct EnemyShadowState {
    unsigned char unmodeled_00[0x38aa];
    short casts_shadow;
} EnemyShadowState;

typedef struct ActorShadow {
    ActorHead head;
    unsigned char unmodeled_70[0xa0 - sizeof(ActorHead)];
    Vector4 shadow_offset;
} ActorShadow;

extern float Get_Distance(const Vector4 *source, const Vector4 *destination);



extern const float D_004D8138;

#define D_004D8138 5000.0f

typedef struct ActorFan {
    ActorHead head;
    unsigned char unmodeled_70[0x9e8 - sizeof(ActorHead)];
    float interaction_radius;
} ActorFan;

extern int Check_InsideFan(const Vector4 *origin, const Vector4 *target,
                           int flag, float facing, float min_degrees,
                           float max_degrees, float radius);

typedef struct EnemyLookatState {
    unsigned char unmodeled_00[0x0c];
    float target_height;
    unsigned char unmodeled_10[0x386c - 0x10];
    float *lookat_point;
    unsigned char unmodeled_3870[0x3890 - 0x3870];
    Vector4 point;
} EnemyLookatState;

extern void ACT_setMotion2(Actor *actor, int motion, int flags);

extern int RES_GetEnemySeFoot(int sound_id);

extern int F2I(float value);

extern void EnemySound(Actor *actor, short sound_index, signed char play,
                       signed char ignore_se_type);

static unsigned char FootStep[0x180] = {
    0x02, 0x10, 0x01, 0x0c, 0x02, 0x10, 0x01, 0x0c, 0x02, 0x10, 0x01, 0x0c, 0x0f, 0x02, 0x04, 0x10,
    0x02, 0x10, 0x01, 0x0c, 0x02, 0x10, 0x01, 0x0c, 0x02, 0x10, 0x01, 0x0c, 0x02, 0x10, 0x04, 0x10,
    0x02, 0x10, 0x01, 0x0c, 0x02, 0x10, 0x03, 0x0d, 0x02, 0x10, 0x05, 0x10, 0x02, 0x10, 0x04, 0x10,
    0x01, 0x0f, 0x05, 0x10, 0x02, 0x10, 0x03, 0x0d, 0x02, 0x10, 0x04, 0x10, 0x02, 0x10, 0x01, 0x0c,
    0x02, 0x10, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 0x01, 0x0c, 0x02, 0x10, 0x01, 0x0c,
    0x02, 0x10, 0x01, 0x0c, 0x02, 0x10, 0x01, 0x0c, 0x01, 0x0f, 0x01, 0x08, 0x02, 0x10, 0x01, 0x09,
    0x04, 0x12, 0x04, 0x11, 0x02, 0x10, 0x05, 0x10, 0x02, 0x10, 0x05, 0x10, 0x02, 0x0e, 0x04, 0x10,
    0x02, 0x10, 0x04, 0x10, 0x01, 0x10, 0x01, 0x0a, 0x01, 0x10, 0x06, 0x12, 0x02, 0x0d, 0x01, 0x09,
    0x01, 0x10, 0x04, 0x10, 0x02, 0x10, 0x03, 0x0d, 0x02, 0x10, 0x01, 0x0c, 0x01, 0x0f, 0x05, 0x10,
    0x02, 0x10, 0x01, 0x0c, 0x02, 0x0f, 0x01, 0x0c, 0x02, 0x0e, 0x04, 0x10, 0x01, 0x0f, 0x01, 0x08,
    0x01, 0x0f, 0x01, 0x08, 0x02, 0x10, 0x01, 0x0c, 0x02, 0x10, 0x05, 0x10, 0x02, 0x10, 0x05, 0x10,
    0x02, 0x10, 0x05, 0x10, 0x0f, 0x02, 0x04, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x04, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x10, 0x05, 0x10,
    0x02, 0x10, 0x04, 0x10, 0x01, 0x0e, 0x03, 0x0d, 0x01, 0x0e, 0x03, 0x0d, 0x01, 0x0e, 0x03, 0x0d,
    0x01, 0x0e, 0x03, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0e, 0x03, 0x0d, 0x01, 0x0e, 0x03, 0x0d,
    0x01, 0x0e, 0x03, 0x0d, 0x01, 0x0e, 0x03, 0x0d, 0x01, 0x0e, 0x03, 0x0d, 0x01, 0x0e, 0x03, 0x0d,
    0x01, 0x0e, 0x03, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x01, 0x16, 0x01, 0x0d, 0x11, 0x01, 0x05, 0x11,
    0x03, 0x11, 0x04, 0x10, 0x03, 0x10, 0x04, 0x0f, 0x16, 0x09, 0x0d, 0x01, 0x1d, 0x0e, 0x09, 0x05,
    0x13, 0x04, 0x06, 0x12, 0x13, 0x04, 0x06, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x01, 0x08, 0x01, 0x18, 0x08, 0x0c, 0x01, 0x10, 0x01, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x01, 0x0d, 0x01, 0x01, 0x11, 0x02, 0x0b,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x01, 0x03, 0x0c, 0x11, 0x01, 0x05, 0x11,
    0x01, 0x10, 0x02, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x11, 0x01, 0x0b, 0x01, 0x04, 0x11, 0x04, 0x10,
    0x01, 0x17, 0x05, 0x14, 0x01, 0x17, 0x05, 0x14, 0x01, 0x17, 0x05, 0x14, 0x01, 0x17, 0x05, 0x14
};

/*
 * Sound_FootStep and Enemy_ActionReady always pass 1 for `play`; when it is
 * not 1 the positional call below is skipped and the function has no other
 * effect. `ignore_se_type` is 0 at Enemy_ActionReady's one sound_index == 1
 * call and 1 everywhere else (main:0x002d3930, 0x002d396c, 0x002d3a84,
 * 0x002d06d8, 0x002d0710, 0x002d0744); when it is 0 and sound_index is 2 or
 * 4, a bank whose RES_GetEnemySeType is 1 is skipped entirely.
 */

/*
 * A TU-local view of main/xgl_sound.c's struct SoundWork (main:0x004a8140),
 * not yet reachable through a shared header. effect_banks starts at +0x20
 * and each entry's low halfword is its handle (main/xgl_sound.c's
 * SoundEffectBankEntry.handle), the same field xglSoundEffectStopDirect
 * reads (main:0x00226720).
 */

/* This flag is a signed byte in the original enemy-work record. */

/* The original scratch blocks are 0x40 bytes; only this prefix is modeled. */

void Set_Motion(Actor *actor, int motion, unsigned int mode)
{
    Actor *motion_actor = actor;
    unsigned char *enemy_entry = enepc[ACTOR_NUMBER(actor)];
    unsigned int flags;
    unsigned int motion_mode;
    unsigned int enemy_mode;
    int is_enemy_actor;

    if ((*ENEMY_JAVA_REACTION(enemy_entry) & 0x100) != 0) {
        motion_mode = mode | 0x10000000;
        enemy_mode = motion_mode | 0x20;
        if (motion_actor != (Actor *)GameLoopState.player) {
            motion_mode = enemy_mode;
        }
        ACT_setMotion2(actor, motion, motion_mode);
        motion_actor->copied_motion_parameter =
            *ENEMY_MOTION_PARAMETER(enemy_entry);
    } else {
        flags = mode | 0x20;
        if (motion_actor == (Actor *)GameLoopState.player) {
            flags = mode;
        }
        ACT_setMotion2(actor, motion, flags);
    }
    is_enemy_actor = actor != (Actor *)GameLoopState.player;
    if (is_enemy_actor) {
        motion_actor->motion_parameter = 0.53333336f;
    }
}

void Sound_FootStep(Actor *actor)
{
    unsigned char *enemy_entry = enepc[ACTOR_NUMBER(actor)];
    const unsigned char *thresholds;
    short foot_step_index;
    unsigned short motion;
    short previous_step;
    int current_step;
    int sound_index;
    int second_step;

    motion = actor->motion_id;
    second_step = 0;
    if (motion != 1 && motion != 3) {
        return;
    }
    if ((actor->movement_flags & 2) != 0) {
        return;
    }

    foot_step_index =
        (short)RES_GetEnemySeFoot(*ACTOR_SOUND_EFFECT_ID(actor));
    sound_index = 0;
    thresholds = &FootStep[foot_step_index * 4];
    if (actor->motion_id == 3) {
        thresholds += 2;
    }
    previous_step = actor->last_step_distance;
    current_step = F2I(actor->movement_distance / 0.033333335f);

    if ((previous_step < (short)thresholds[0] &&
         (short)thresholds[0] <= current_step) ||
        (current_step < previous_step &&
         previous_step < (short)thresholds[0])) {
        sound_index = 0x20001;
        if (actor->motion_id == 3) {
            sound_index = 0x20003;
        }
        second_step = 0;
    }

    if ((previous_step < (short)thresholds[1] &&
         (short)thresholds[1] <= current_step) ||
        (current_step < previous_step &&
         previous_step < (short)thresholds[1])) {
        sound_index = 0x20002;
        if (actor->motion_id == 3) {
            sound_index = 0x20004;
        }
        second_step = 1;
    }
    actor->last_step_distance = current_step;
    if (sound_index == 0) {
        return;
    }

    if (actor->motion_id == 1 &&
        (signed char)enemy_entry[ENEMY_ACTION_STATE_OFFSET] == 4) {
        int sound_number = 3;
        if (!second_step) {
            sound_number = 2;
        }
        EnemySound(actor, sound_number, 1, 1);
    }
    if (actor->motion_id == 3) {
        if ((signed char)enemy_entry[ENEMY_ACTION_STATE_OFFSET] == 4) {
            EnemySound(actor, second_step ? 5 : 4, 1, 1);
        }
        if (actor->motion_id == 3 &&
            (signed char)enemy_entry[ENEMY_ACTION_STATE_OFFSET] == 6) {
            EnemySound(actor, second_step ? 5 : 4, 1, 1);
        }
    }
}

void EnemySound(Actor *actor, short sound_index, signed char play,
                 signed char ignore_se_type)
{
    int bank;

    if ((sound_index == 2 || sound_index == 4) && ignore_se_type == 0 &&
        RES_GetEnemySeType(*ACTOR_SOUND_EFFECT_ID(actor)) == 1)
    {
        return;
    }
    if (actor->flags & 8)
    {
        return;
    }
    bank = RES_GetEnemySeBank(*ACTOR_SOUND_EFFECT_ID(actor));
    if (bank != -1 && play == 1)
    {
        xglSoundEffectPosID(bank + (sound_index & 0xffff), &actor->position, 1,
                             ACTOR_NUMBER(actor) + 1);
    }
}

void EnemySoundEnd(Actor *actor, short sound_offset)
{
    xglSoundEffectStopID(
        RES_GetEnemySeBank(*ACTOR_SOUND_EFFECT_ID(actor)) + (sound_offset & 0xffff),
        ACTOR_NUMBER(actor) + 1);
    /* The original keeps a real call here (jal xglSoundEffectStopID at
     * 0x002d0868) followed by its own register restores and jr ra
     * (0x002d0870..0x002d087c), instead of folding the call into a sibling
     * jump. This no-op loop keeps the call out of tail position and
     * reproduces that real call plus epilogue. */
    do {
    } while (0);
}

void EnemySound_StopAll(signed char which)
{
    short actor_index;

    for (actor_index = 0; actor_index < 16; actor_index++)
    {
        EnemySound_Stop((Actor *)&actor[actor_index], which);
    }
    xglSoundEffectStopDirect(0x10008);
    xglSoundEffectStopDirect(0x10009);
    xglSoundEffectStopDirect(0x1000b);
}

void EnemySound_Stop(Actor *actor, signed char which)
{
    int bank;
    short channel;
    int handle;

    bank = RES_GetEnemySeBank(*ACTOR_SOUND_EFFECT_ID(actor));
    for (channel = 1; channel < 8; channel++)
    {
        if (which == 1)
        {
            handle = SoundWork.effect_banks[(bank + (channel & 0xffff)) >> 16].handle;
            SsdFadeoutEffect((handle << 16) + (channel & 0xffff), 300,
                              ACTOR_NUMBER(actor) + 1);
        }
        else
        {
            xglSoundEffectStopDirect(bank + (channel & 0xffff));
        }
    }
}

int Get_JAVAReaction(Actor *actor)
{
    return *ENEMY_JAVA_REACTION(enepc[ACTOR_NUMBER(actor)]);
}

short Get_DefaultMotion(Actor *actor, short motion_number)
{
    short *motion_table;

    motion_table = ACTOR_DEFAULT_MOTION_TABLE(actor);
    return motion_table[motion_number];
}

void Before_Talk(Actor *actor)
{
    ActorTalk *talk_actor = (ActorTalk *)actor;
    ActorTalkControls *controls = &talk_actor->controls;
    EnemyTalkState *enemy_state = (EnemyTalkState *)enepc[ACTOR_NUMBER(actor)];

    if (controls->talk_flags & 1) {
        enemy_state->saved_motion = talk_actor->saved_talk_motion;
        Set_Motion(actor, Get_DefaultMotion(actor, 3), 1);
        talk_actor->motion_progress = 0;
        talk_actor->talk_state = 9;
    }
    if (controls->talk_flags & 4) {
        Enemy_Command_LookAt(actor, 100);
    }
    if (controls->talk_flags & 0x10) {
        Enemy_Command_Freeze(actor, 1);
        /* This path ends with a sibling jump in the original, unlike
         * the after-talk path, which returns through its own epilogue. */
    }
}

void After_Talk(Actor *actor)
{
    ActorTalk *talk_actor = (ActorTalk *)actor;
    ActorTalkControls *controls = &talk_actor->controls;
    EnemyTalkState *enemy_state =
        (EnemyTalkState *)enepc[ACTOR_NUMBER(actor)];

    if (controls->talk_flags & 1) {
        Set_Motion(actor, enemy_state->saved_motion, 9);
    }
    if (controls->talk_flags & 4) {
        Enemy_Command_LookAt(actor, -1);
    }
    do {
        if (!(controls->talk_flags & 0x10))
            break;
        Enemy_Command_Freeze(actor, 0);
    } while (0);
}

int Check_EnemyFound(void)
{
    int reaction_offset;
    short enemy_index;

    for (enemy_index = 0; enemy_index < 16; enemy_index++) {
        reaction_offset = ENEMY_JAVA_REACTION_OFFSET;
        if (actor[enemy_index].active_state > 0 &&
            actor[enemy_index].status != 1 &&
            (ENEMY_JAVA_REACTION_AT(enepc[enemy_index], reaction_offset)->value & 1) &&
            ((signed char)enepc[enemy_index][0x48] == 6 ||
             (signed char)enepc[enemy_index][0x48] == 10 ||
             (signed char)enepc[enemy_index][0x48] == 5)) {
            return 1;
        }
    }
    return 0;
}

int Check_EnemyBurn(void)
{
    short enemy_index;

    for (enemy_index = 0; enemy_index < 16; enemy_index++) {
        if (actor[enemy_index].active_state > 0 &&
            actor[enemy_index].status != 1) {
            signed char action = (signed char)enepc[enemy_index][ENEMY_ACTION_STATE_OFFSET];
            if (action == 11 || action == 7) {
                return 1;
            }
        }
    }
    return 0;
}

int Check_EnemyElec(void)
{
    short enemy_index;

    for (enemy_index = 0; enemy_index < 16; enemy_index++) {
        if (actor[enemy_index].active_state > 0 &&
            actor[enemy_index].status != 1 &&
            ((signed char)enepc[enemy_index][ENEMY_ACTION_STATE_OFFSET] == 12 ||
             (enepc[enemy_index][ENEMY_ELECTRIC_STATUS_OFFSET] & 2))) {
            return 1;
        }
    }
    return 0;
}

void Enemy_FindByEar(Actor *enemy, const Vector4 *sound_position)
{
    short index;

    if (PadData.half_28 & 0x200) {
        return;
    }
    for (index = 0; index < 64; index++) {
        if ((void *)enemy != (void *)&actor[index].head) {
            ActorHearing *hearing = &actor[index].hearing;
            if (hearing->radius != 0.0f) {
                float distance = Get_Distance3D(sound_position, &actor[index].head.position.coordinates);
                if (((signed char)enepc[index][ENEMY_EAR_DISABLED_OFFSET]) == 0 &&
                    ((EnemyEarState *)enepc[index])->discovery_count <= 0 &&
                    distance < hearing->radius) {
                    Check_Discovery((Actor *)&actor[index]);
                }
            }
        }
    }
}

void Script_Action(Actor *actor)
{
    EnemyScriptState *script = (EnemyScriptState *)enepc[ACTOR_NUMBER(actor)];

    if (script->action_flags & 0x10000) {
        if (script->movement_step != -1) {
            Get_MiddlePoint(script->movement_points[0], script->movement_points[1],
                            script->movement_step, script->movement_duration,
                            &actor->position.x);
            script->movement_step++;
            if (script->movement_step >= script->movement_duration) {
                script->movement_step = -1;
                BSpline_Init(ENEMY_SPLINE_POINTS(script)[0],
                             &actor->position.x, ((ActorTalk *)actor)->target_angle);
            }
        }
        if (script->turn_step != -1) {
            float heading = script->start_angle +
                (script->end_angle - script->start_angle) *
                (float)script->turn_step / (float)script->turn_duration;
            script->turn_step++;
            actor->rotation.y = heading;
            ((ActorTalk *)actor)->target_angle = heading;
            if (script->turn_step >= script->turn_duration) {
                script->turn_step = -1;
            }
        }
    }
}

void Move_BeltConveyer(Actor *actor)
{
    unsigned long long flags;
    int surface_flags;
    int direction_mask;

    UnduParamInit(&UnduTemp);
    UnduTemp.header = UnduDataGetHeader(0, 0x8000);
    UnduCheck(&actor->position, 0, &UnduTemp);
    if (ACTOR_NUMBER(actor) == 8) {
        flags = UnduTemp.surface_flags;
        surface_flags = (int)(flags & 0x3f000000);
        direction_mask = surface_flags & 0x0f000000;
        if (direction_mask != 0) {
            unsigned char direction = idx_0[(unsigned int)direction_mask >> 24];
            float distance = rate_2_003B2008[(unsigned int)surface_flags >> 28] * 0.1024f;
            float *direction_vector = vec_1[direction];
            actor->position.x += distance * direction_vector[0];
            actor->position.z += distance * direction_vector[1];
        }
    }
}

void Check_Discovery(Actor *enemy)
{
    EnemyDiscoveryState *state = (EnemyDiscoveryState *)enepc[ACTOR_NUMBER(enemy)];
    DiscoveryPlayer *player;
    signed char detection_state;

    if (*ACTOR_SOUND_EFFECT_ID(enemy) == 0) {
        return;
    }
    if ((int)enemy->flags < 0 && state->discovery_count < 0) {
        return;
    }
    if (enemy->flags & 8) {
        return;
    }
    player = GameLoopState.player;
    if (player->disable_detection) {
        return;
    }
    if ((unsigned int)state->detection_state - 5U < 2U) {
        return;
    }
    detection_state = state->detection_state;
    if (detection_state == 11) {
        return;
    }
    if (detection_state == 12) {
        return;
    }
    if (detection_state == 13) {
        return;
    }
    if (detection_state == 7) {
        return;
    }
    if (detection_state == 10) {
        return;
    }
    if (detection_state == 0) {
        return;
    }
    if (detection_state == 14) {
        return;
    }
    enemy->flags |= 0x00400000;
    if (FLAG_FRAME_60 != 0 && (PadData.half_28 & 0x200)) {
        return;
    }
    switch (state->action_state) {
    case 2: case 3: case 4: case 7:
        Enemy_ActionReady(enemy, 10);
        break;
    case 6: case 13:
        break;
    case 5: case 8: case 9: case 10: case 11: case 12:
    default:
        Enemy_ActionReady(enemy, 5);
        state->last_player_position_words[0] = GameLoopState.player->position_words[0];
        state->last_player_position_words[1] = GameLoopState.player->position_words[1];
        break;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Check_Encount);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Enemy_After_Battle);

void Set_Spline_By_Route(Actor *actor, short route_offset)
{
    unsigned char *actor_bytes;
    unsigned char *state;
    float *point_data;
    short point;

    actor_bytes = (unsigned char *)actor;
    state = enepc[actor_bytes[0x80]];
    point = 0;

    if (ENEMY_ROUTE_POINT_COUNT(state) > 0) {
        /* One walking pointer covers both arrays, as the original does: it
         * addresses the spline point's z at 0(v1) and the route point's x
         * 1026 floats further on, which is ENEMY_ROUTE_POINTS minus
         * ENEMY_SPLINE_POINTS. Only the three coordinates of each record are
         * copied. Initialized inside the positive-count branch so
         * ee-gcc2_96 -O2 -G8 emits addiu v1,a0,0x88 after blez, matching the
         * original. */
        point_data = &ENEMY_SPLINE_POINTS(state)[0][2];
        do {
            point_data[-2] = point_data[1026];
            point_data[-1] = point_data[1027];
            point_data[0] = point_data[1028];
            point++;
            point_data += 4;
        } while (point < ENEMY_ROUTE_POINT_COUNT(state));
    }

    ENEMY_SPLINE_POINT_COUNT(state) = ENEMY_ROUTE_POINT_COUNT(state);
    ENEMY_SPLINE_PARAMETER(state) =
        (short)((ENEMY_ROUTE_POINT_COUNT(state) + route_offset) *
                    ENEMY_SPLINE_UNITS_PER_POINT + 2);
}

void Set_Spline_By_Random(Actor *actor)
{
    unsigned char *actor_bytes;
    unsigned char *state;
    float (*spline_points)[4];

    actor_bytes = (unsigned char *)actor;
    state = enepc[actor_bytes[0x80]];
    spline_points = ENEMY_SPLINE_POINTS(state);
    BSpline_Init(spline_points[0], &actor->position.x,
                 ((ActorTalk *)actor)->target_angle);
    ENEMY_SPLINE_POINT_COUNT(state) = 4;
    ENEMY_SPLINE_FAST_POINTS(state) = 1;
    ENEMY_SPLINE_PARAMETER(state) = 0;
}

void Set_PlayerHistory(Actor *actor)
{
    float result[4];

    if (Check_Straight_ID(((PlayerHistoryActor *)actor)->interaction_radius,
                          PlayHis[PhCunt[0] - 1],
                          &enepc[ACTOR_NUMBER(actor)][0x37c0],
                          32, result, 772) < 32) {
        PlayHis[PhCunt[0]][0] = BackPos[0];
        PlayHis[PhCunt[0]][1] = BackPos[1];
        PlayHis[PhCunt[0]][2] = BackPos[2];
        PlayHis[PhCunt[0]][3] = BackPos[3];
        PhCunt[0] = (PhCunt[0] + 1) % 64;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Homing_Search);

void Homing_Add(int actor_index, float x, float y, float z, short target_index)
{
    EnemyHomingState *state = (EnemyHomingState *)enepc[actor[actor_index].slot_number];
    short position_index = state->position_count;

    state->positions[position_index][0] = x;
    state->positions[position_index][1] = y;
    state->positions[position_index][2] = z;

    if (position_index != target_index) {
        state->linked_target[position_index] = target_index;
        if (target_index != -1) {
            state->neighbours[position_index][state->neighbour_count[position_index]] = target_index;
            state->neighbours[target_index][state->neighbour_count[target_index]] = position_index;
            state->neighbour_count[position_index]++;
            state->neighbour_count[target_index]++;
        }
    }
    state->position_count++;
}

void Refresh_Homing(Actor *actor)
{
    EnemyHomingState *state;
    short positionCount;
    short positionIndex;
    int neighbourIndex;
    state = (EnemyHomingState *)enepc[ACTOR_NUMBER(actor)];

    for (positionIndex = 0; positionIndex < 64; positionIndex++) {
        state->neighbour_count[positionIndex] = 0;
        for (neighbourIndex = 3; neighbourIndex >= 0; neighbourIndex--) {
            state->neighbours[positionIndex][neighbourIndex] = -1;
        }
    }

    positionCount = (short)state->position_count;
    state->position_count = 0;
    for (positionIndex = 0; positionIndex < positionCount; positionIndex++) {
        float x;
        float y;
        float z;
        short targetIndex;

        x = state->positions[positionIndex][0];
        y = state->positions[positionIndex][1];
        z = state->positions[positionIndex][2];
        targetIndex = state->linked_target[positionIndex];
        Homing_Add(ACTOR_NUMBER(actor), x, y, z, targetIndex);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Disp_Homing);

void Set_Shadow(Actor *reference_actor)
{
    float nearest_y;
    float nearest_distance = 5000.0f;
    AlignedHomingPosition nearest_position;
    short actor_index;

    for (actor_index = 0; actor_index < 16; actor_index++) {
        EnemyShadowState *state = (EnemyShadowState *)enepc[actor_index];

        if (ACTOR_NUMBER(reference_actor) != actor_index &&
            actor[actor_index].active_state > 0 && actor[actor_index].status != 1 &&
            state->casts_shadow) {
            float distance = Get_Distance(&reference_actor->position,
                                           &actor[actor_index].head.position.coordinates);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest_position = actor[actor_index].head.position;
            }
        }
    }

    if (nearest_distance != D_004D8138) {
        ActorShadow *shadow_actor = (ActorShadow *)reference_actor;
        shadow_actor->shadow_offset.x = nearest_position.coordinates.x - reference_actor->position.x;
        nearest_y = nearest_position.coordinates.y;
        shadow_actor->shadow_offset.y = reference_actor->position.y - nearest_y;
        shadow_actor->shadow_offset.z = nearest_position.coordinates.z - reference_actor->position.z;
        shadow_actor->shadow_offset.w = 1.0f;
    }
}

int Get_MostNear_Actor(Actor *reference_actor)
{
    float nearest_distance = 5.0f;
    short actor_index;
    int nearest_index = -1;

    for (actor_index = 0; actor_index < 64; actor_index++) {
        if (ACTOR_NUMBER(reference_actor) != actor_index &&
            actor[actor_index].active_state > 0 && actor[actor_index].status != 1 &&
            Check_InsideFan(&reference_actor->position,
                            &actor[actor_index].head.position.coordinates,
                            -1, reference_actor->rotation.y, 5.0f, 180.0f,
                            ((ActorFan *)reference_actor)->interaction_radius)) {
            float distance = Get_Distance3D(
                &reference_actor->position,
                &actor[actor_index].head.position.coordinates);
            if (distance < nearest_distance && distance <= 8.0f) {
                nearest_distance = distance;
                nearest_index = actor_index;
            }
        }
    }
    return nearest_index;
}

void Actor_LookAt(Actor *viewer)
{
    EnemyLookatState *state = (EnemyLookatState *)enepc[ACTOR_NUMBER(viewer)];
    unsigned int remaining = (unsigned short)*ACTOR_LOOKAT_TIMER(viewer);
    int target_index;

    if (remaining - 1U >= 2U) {
        return;
    }
    target_index = ACTOR_LOOKAT_TARGET(viewer)[0];
    if (target_index == -1) {
        return;
    }
    state->lookat_point = &state->point.x;
    if (actor[target_index].has_head_position != 0) {
        state->point.x = actor[target_index].head_position[0];
        state->point.y = actor[target_index].head_position[1];
        state->point.z = actor[target_index].head_position[2];
    } else {
        state->point.x = actor[target_index].head.position.elements[0];
        state->point.y = actor[target_index].head.position.elements[1] +
                         ((EnemyLookatState *)enepc[target_index])->target_height;
        state->point.z = actor[target_index].head.position.elements[2];
    }
}

int Actor_LookAt_Set(Actor *actor, signed char duration,
                     const Vector4 *target_position)
{
    EnemyLookatState *state;
    float *look_at_position;

    state = (EnemyLookatState *)enepc[ACTOR_NUMBER(actor)];
    if (duration < *ACTOR_LOOKAT_TIMER(actor))
    {
        return 0;
    }
    look_at_position = &state->point.x;
    look_at_position[0] = target_position->x;
    state->lookat_point = look_at_position;
    *ACTOR_LOOKAT_TIMER(actor) = duration;
    state->point.y = target_position->y;
    state->point.z = target_position->z;
    return 1;
}

int Actor_LookAt_Release(Actor *actor, signed char duration)
{
    unsigned char *state;

    state = enepc[ACTOR_NUMBER(actor)];
    if (duration < *ACTOR_LOOKAT_TIMER(actor))
    {
        return 0;
    }
    if (*ENEMY_LOOKAT_POINT(state) == 0)
    {
        return 1;
    }
    *ENEMY_LOOKAT_POINT(state) = 0;
    *ACTOR_LOOKAT_TIMER(actor) = 0;
    *ACTOR_LOOKAT_TARGET(actor) = -1;
    return 1;
}

void Actor_LookAt_Init(Actor *actor)
{
    *ACTOR_LOOKAT_TIMER(actor) = 0;
    *ACTOR_LOOKAT_TARGET(actor) = -1;
    *ENEMY_LOOKAT_POINT(enepc[ACTOR_NUMBER(actor)]) = 0;
}
