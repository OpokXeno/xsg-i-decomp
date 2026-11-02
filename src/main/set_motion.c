#include "common.h"
#include "set_motion.h"
#include "main/enemy_1.h"
#include "main/get.h"
#include "main/Undulate.h"
#include "main/pause_menu_page_enemy.h"
#include "main/act_1.h"
#include "main/act_2.h"
#include "main/init_drill.h"
#include "main/res_get_path.h"
#include "main/xgl_2.h"
#include "main/xgl_dma.h"

typedef struct SetMotionPad {
    unsigned char pad_00[0x28];
    unsigned short half_28;
    unsigned short half_2a;
    unsigned char unmodeled_2c[0x43 - 0x2c];
    unsigned char analog_level;
} SetMotionPad;

typedef union AlignedHomingPosition {
    Vector4 coordinates;
    float elements[4];
    unsigned long long alignment[2];
} AlignedHomingPosition;

typedef struct EnemyDetectHead {
    unsigned int flags;
    unsigned char unmodeled_04[0x10 - 4];
    AlignedHomingPosition position;
    unsigned char unmodeled_20[0x70 - 0x20];
} EnemyDetectHead;

/* Partial view of Actor (include/main/act_1.h): the hearing sub-record at +0xc0 is the
 * canonical EnemyParams (include/main/enemy_1.h), and the enemy-detect words at
 * +0x630..+0x9ec are not in the canonical record. */
typedef struct ActorEnemyDetectView {
    EnemyDetectHead head;
    unsigned char unmodeled_70[0x10];
    unsigned char slot_number;
    unsigned char unmodeled_81;
    unsigned char status;
    unsigned char has_head_position;
    unsigned char unmodeled_84[2];
    short active_state;
    unsigned char unmodeled_88[0xc0 - 0x88];
    EnemyParams hearing;
    unsigned char unmodeled_2e8[0x630 - 0x2e8];
    float head_position[4];
    unsigned char unmodeled_640[0x696 - 0x640];
    short battle_state;
    unsigned char unmodeled_698[0x9e4 - 0x698];
    float target_angle;
    float interaction_radius;
    unsigned char unmodeled_9ec[0xa70 - 0x9ec];
} ActorEnemyDetectView;

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

typedef struct LayoutHeader LayoutHeader;

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

typedef struct EncountActor EncountActor;

struct EncountActor {
    ActorHead head;
    unsigned char unmodeled_70[0x10];
    unsigned char number;
    unsigned char unmodeled_81[0xc0 - 0x81];
    EnemyParams params;
};

typedef struct EncountRecord {
    short monster_set_number;
    unsigned char unmodeled_02[2];
    short encounter_side;
    unsigned char trigger_flags;
    unsigned char event_flags;
    short entry_value;
    unsigned char unmodeled_0a[2];
    EncountActor *enemy;
} EncountRecord;

typedef struct DiscoveryGameLoopState {
    unsigned char unmodeled_00[4];
    struct Actor *player;
    unsigned char unmodeled_08[8];
    unsigned int pause_flags; /* +0x10 */
    unsigned char unmodeled_14[8]; /* +0x14 */
    unsigned int event_flags; /* +0x1c */
    unsigned char unmodeled_20[0x29f90 - 0x20]; /* +0x20 */
    EncountRecord encount; /* +0x29f90 */
} DiscoveryGameLoopState;


/* Homing_Add's view of EnemyWorkEntry's homing graph: it stores the position
 * floats with the float alias set, so the homing_count load is scheduled
 * above the stores; every access through the canonical HomingPosition union
 * has alias set 0 and keeps that load below them. See the R09 rematch log. */
typedef struct EnemyHomingAddState {
    unsigned char unmodeled_00[0x30a0];
    float homing_positions[64][4];
    short homing_neighbours[64][4];
    short homing_neighbour_count[64];
    short homing_linked_target[64];
    short homing_count;
} EnemyHomingAddState;

extern short SelNum;
extern ActorEnemyDetectView actor[64];
extern EnemyWorkEntry enepc[16];
extern SetMotionSoundWork SoundWork;
extern UnduParam UnduTemp;
extern UnduTestStorage UnduTest;
extern DiscoveryGameLoopState GameLoopState;
extern float PlayHis[64][4];
extern float BackPos[4];
extern float LocaterAngle[16];
extern short PhCunt[1];
extern const float SHADOW_NO_NEAREST_DISTANCE;
extern short EfTab_Code[15];
extern short EfTab_Intr[15];
extern float EfTab_Scal[15];
extern short EfTab_Loop[15];
extern float TabEnemy_Ang[8];
extern long EneTerritoryID[20];

void Homing_Search(ActorEnemyDetectView *enemy);
extern void EnemySound_Stop(struct Actor *actor, signed char which);
extern void xglSoundEffectStopDirect(int sound_id);
extern void Check_Discovery(struct Actor *enemy);
extern void BSpline_Init(float *points, const float *origin, float angle);

extern void xglSoundEffectPosID();

extern void xglSoundEffectStopID(int sound_id, int flags);

extern void SsdFadeoutEffect(int effect_id, int source_id, int fade_time);
extern void Set_Motion(struct Actor *actor, int motion, unsigned int mode);
extern void Enemy_Command_LookAt(struct Actor *actor, int target);
extern void Enemy_Command_Freeze(struct Actor *actor, int frozen);
extern void UnduParamInit(UnduParam *param);
extern void UnduCheck(const Vector4 *position, void *exclude, UnduParam *param);
extern void Enemy_ActionReady(struct Actor *actor, int action);

extern int Check_Straight_ID(float heading, float *previous,
                             void *enemy_state, int limit,
                             float *result, int stride);

extern void Homing_Add(int actor_index, float x, float y, float z,
                       short target_index);

extern int Check_InsideFan(const Vector4 *origin, const Vector4 *target,
                           int flag, float facing, float min_degrees,
                           float max_degrees, float radius);

extern int RES_GetEnemySeFoot(int sound_id);
/*
 * Sound_FootStep and Enemy_ActionReady always pass 1 for `play`; when it is
 * not 1 the positional call below is skipped and the function has no other
 * effect. `ignore_se_type` is 0 at Enemy_ActionReady's one sound_index == 1
 * call and 1 everywhere else (main:0x002d3930, 0x002d396c, 0x002d3a84,
 * 0x002d06d8, 0x002d0710, 0x002d0744); when it is 0 and sound_index is 2 or
 * 4, a bank whose RES_GetEnemySeType is 1 is skipped entirely.
 */
extern void EnemySound(struct Actor *actor, short sound_index, signed char play,
                       signed char ignore_se_type);

extern float sinf(float angle);
extern float cosf(float angle);
extern void TM_Script_Spline_Add(int number, float x, float y, float z);

static unsigned char idx_0[16];

static float vec_1[9][2];

static float rate_2_003B2008[4];

static unsigned char FootStep[0x180];

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

static unsigned char idx_0[16] = {0, 1, 3, 2, 5, 0, 4, 0, 7, 8, 0, 0, 6, 0, 0, 0};

static float vec_1[9][2] = {
    {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f},
    {1.0f, -1.0f}, {0.0f, -1.0f}, {-1.0f, -1.0f}, {-1.0f, 0.0f},
    {-1.0f, 1.0f},
};

static float rate_2_003B2008[4] = {1.0f, 2.0f, 0.5f, 3.0f};

float PlayHis[64][4] = {{0.0f}};

float BackPos[4] = {0.0f, 0.0f, 0.0f, 0.0f};

float TabEnemy_Ang[8] = { 0.0f, 0.78539818525314331f, 1.5707963705062866f, 2.3561944961547852f, 3.1415927410125732f, 3.9269909858703613f, 4.7123889923095703f, 5.4977874755859375f };

float LocaterAngle[16] = {
    -3.2f, -3.0f, -2.4f, -2.2f, -1.6f, -1.4f, -0.8f, -0.6f,
    -0.1f, 0.1f, 0.6f, 0.8f, 1.4f, 1.6f, 2.2f, 2.3f,
};

long EneTerritoryID[20] = { 0LL, 134217728LL, 150994944LL, 167772160LL, 184549376LL, 201326592LL, 218103808LL, 234881024LL, 251658240LL, 0LL, 0LL, 402653184LL, 419430400LL, 436207616LL, 452984832LL, 469762048LL, 486539264LL, 503316480LL, 520093696LL, 0LL };

short EfTab_Code[15] = { 1492, 1493, 1494, 1555, 1416, 1554, 1627, 1724, 1746, 1658, 1726, 0, 0, 0, 0 };

short EfTab_Intr[15] = { 5, 5, 13, 13, 5, 15, 5, 5, 5, 10, 5, 0, 0, 0, 0 };

float EfTab_Scal[15] = { 4.0f, 4.0f, 8.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f };

short EfTab_Loop[15] = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/* The original scratch blocks are 0x40 bytes; only this prefix is modeled. */
UnduParam UnduTemp = {0};

UnduTestStorage UnduTest = {0};

/* Declared here, not at the top: moving it up changes the original code. */
#define SHADOW_NO_NEAREST_DISTANCE 5000.0f

void Set_Motion(struct Actor *actor, int motion, unsigned int mode)
{
    struct Actor *motion_actor = actor;
    EnemyWorkEntry *enemy_entry = &enepc[ACTOR_NUMBER(actor)];
    unsigned int flags;
    unsigned int motion_mode;
    unsigned int enemy_mode;
    int is_enemy_actor;

    if ((enemy_entry->state_flags & 0x100) != 0) {
        motion_mode = mode | 0x10000000;
        enemy_mode = motion_mode | 0x20;
        if (motion_actor != GameLoopState.player) {
            motion_mode = enemy_mode;
        }
        ACT_setMotion2(actor, motion, motion_mode);
        motion_actor->animation.blendWork[3] =
            enemy_entry->value_3858;
    } else {
        flags = mode | 0x20;
        if (motion_actor == GameLoopState.player) {
            flags = mode;
        }
        ACT_setMotion2(actor, motion, flags);
    }
    is_enemy_actor = actor != GameLoopState.player;
    if (is_enemy_actor) {
        motion_actor->animation.blendDuration = 0.53333336f;
    }
}

void Sound_FootStep(struct Actor *actor)
{
    EnemyWorkEntry *enemy_entry = &enepc[ACTOR_NUMBER(actor)];
    const unsigned char *thresholds;
    short foot_step_index;
    unsigned short motion;
    short previous_step;
    int current_step;
    int sound_index;
    int second_step;

    motion = actor->animation.slot.currentDataId;
    second_step = 0;
    if (motion != 1 && motion != 3) {
        return;
    }
    if ((actor->animation.slot.flags & 2) != 0) {
        return;
    }

    foot_step_index =
        (short)RES_GetEnemySeFoot(*ACTOR_SOUND_EFFECT_ID(actor));
    sound_index = 0;
    thresholds = &FootStep[foot_step_index * 4];
    if (actor->animation.slot.currentDataId == 3) {
        thresholds += 2;
    }
    previous_step = actor->last_step_distance;
    current_step = F2I(actor->animation.slot.frame / 0.033333335f);

    if ((previous_step < (short)thresholds[0] &&
         (short)thresholds[0] <= current_step) ||
        (current_step < previous_step &&
         previous_step < (short)thresholds[0])) {
        sound_index = 0x20001;
        if (actor->animation.slot.currentDataId == 3) {
            sound_index = 0x20003;
        }
        second_step = 0;
    }

    if ((previous_step < (short)thresholds[1] &&
         (short)thresholds[1] <= current_step) ||
        (current_step < previous_step &&
         previous_step < (short)thresholds[1])) {
        sound_index = 0x20002;
        if (actor->animation.slot.currentDataId == 3) {
            sound_index = 0x20004;
        }
        second_step = 1;
    }
    actor->last_step_distance = current_step;
    if (sound_index == 0) {
        return;
    }

    if (actor->animation.slot.currentDataId == 1 &&
        enemy_entry->action == 4) {
        int sound_number = 3;
        if (!second_step) {
            sound_number = 2;
        }
        EnemySound(actor, sound_number, 1, 1);
    }
    if (actor->animation.slot.currentDataId == 3) {
        if (enemy_entry->action == 4) {
            EnemySound(actor, second_step ? 5 : 4, 1, 1);
        }
        if (actor->animation.slot.currentDataId == 3 &&
            enemy_entry->action == 6) {
            EnemySound(actor, second_step ? 5 : 4, 1, 1);
        }
    }
}

void EnemySound(struct Actor *actor, short sound_index, signed char play,
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

void EnemySoundEnd(struct Actor *actor, short sound_offset)
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
        EnemySound_Stop((struct Actor *)&actor[actor_index], which);
    }
    xglSoundEffectStopDirect(0x10008);
    xglSoundEffectStopDirect(0x10009);
    xglSoundEffectStopDirect(0x1000b);
}

void EnemySound_Stop(struct Actor *actor, signed char which)
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

int Get_JAVAReaction(struct Actor *actor)
{
    return enepc[ACTOR_NUMBER(actor)].state_flags;
}

short Get_DefaultMotion(struct Actor *actor, short motion_number)
{
    short *motion_table;

    motion_table = ACTOR_DEFAULT_MOTION_TABLE(actor);
    return motion_table[motion_number];
}

void Before_Talk(struct Actor *actor)
{
    ActorTalk *talk_actor = (ActorTalk *)actor;
    ActorTalkControls *controls = &talk_actor->controls;
    EnemyWorkEntry *enemy_state = &enepc[ACTOR_NUMBER(actor)];

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

void After_Talk(struct Actor *actor)
{
    ActorTalk *talk_actor = (ActorTalk *)actor;
    ActorTalkControls *controls = &talk_actor->controls;
    EnemyWorkEntry *enemy_state = &enepc[ACTOR_NUMBER(actor)];

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
    short enemy_index;

    for (enemy_index = 0; enemy_index < 16; enemy_index++) {
        if (actor[enemy_index].active_state > 0 &&
            actor[enemy_index].status != 1 &&
            (enepc[enemy_index].state_flags & 1) &&
            (enepc[enemy_index].action == 6 ||
             enepc[enemy_index].action == 10 ||
             enepc[enemy_index].action == 5)) {
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
            EnemyWorkEntry *work = &enepc[enemy_index];

            if (work->action == 11 || work->action == 7) {
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
            (enepc[enemy_index].action == 12 ||
             (enepc[enemy_index].stiff_flag & 2))) {
            return 1;
        }
    }
    return 0;
}

void Enemy_FindByEar(ActorEnemyDetectView *enemy, const Vector4 *sound_position)
{
    short index;

    if (PadData.half_28 & 0x200) {
        return;
    }
    for (index = 0; index < 64; index++) {
        if (enemy != &actor[index]) {
            EnemyParams *hearing = &actor[index].hearing;
            if (hearing->hearing_distance != 0.0f) {
                float distance = Get_Distance3D(sound_position, &actor[index].head.position.coordinates);
                if (enepc[index].freeze_state == 0 &&
                    enepc[index].sense_block_timer <= 0 &&
                    distance < hearing->hearing_distance) {
                    Check_Discovery((struct Actor *)&actor[index]);
                }
            }
        }
    }
}

void Script_Action(struct Actor *actor)
{
    EnemyWorkEntry *script = &enepc[ACTOR_NUMBER(actor)];

    if (script->state_flags & 0x10000) {
        if (script->sac_move.frame != -1) {
            Get_MiddlePoint(&script->sac_move.start.x, &script->sac_move.target.x,
                            script->sac_move.frame, script->sac_move.duration,
                            &actor->position.x);
            script->sac_move.frame++;
            if (script->sac_move.frame >= script->sac_move.duration) {
                script->sac_move.frame = -1;
                BSpline_Init(script->spline_coordinates[0],
                             &actor->position.x, actor->target_angle);
            }
        }
        if (script->turn.frame != -1) {
            float heading = script->turn.start_angle +
                (script->turn.target_angle - script->turn.start_angle) *
                (float)script->turn.frame / (float)script->turn.duration;
            script->turn.frame++;
            actor->rotation.y = heading;
            actor->target_angle = heading;
            if (script->turn.frame >= script->turn.duration) {
                script->turn.frame = -1;
            }
        }
    }
}

void Move_BeltConveyer(struct Actor *actor)
{
    unsigned long long flags;
    int surface_flags;
    int direction_mask;

    UnduParamInit(&UnduTemp);
    UnduTemp.header = UnduDataGetHeader(0, 0x8000);
    UnduCheck(&actor->position, 0, &UnduTemp);
    if (ACTOR_NUMBER(actor) == 8) {
        flags = UnduTemp.attribute;
        surface_flags = flags & 0x3f000000;
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

void Check_Discovery(struct Actor *enemy)
{
    EnemyWorkEntry *state = &enepc[ACTOR_NUMBER(enemy)];
    struct Actor *player;
    signed char detection_state;

    if (*ACTOR_SOUND_EFFECT_ID(enemy) == 0) {
        return;
    }
    if ((int)enemy->flags < 0 && state->chase_frame_limit < 0) {
        return;
    }
    if (enemy->flags & 8) {
        return;
    }
    player = GameLoopState.player;
    if (player->flash_timer) {
        return;
    }
    if ((unsigned char)state->action - 5U < 2U) {
        return;
    }
    detection_state = state->action;
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
    switch (state->pause_kind) {
    case 2: case 3: case 4: case 7:
        Enemy_ActionReady(enemy, 10);
        break;
    case 6: case 13:
        break;
    case 5: case 8: case 9: case 10: case 11: case 12:
    default:
        Enemy_ActionReady(enemy, 5);
        state->last_player_position.vector = GameLoopState.player->position;
        break;
    }
}

void Check_Encount(EncountActor *enemy, signed char enable, signed char command)
{
    short facing[2];
    unsigned char sides[9] = {0, 3, 4, 1, 3, 4, 2, 3, 4};
    EnemyWorkEntry *state = &enepc[enemy->number];
    EnemyParams *params = &enemy->params;
    short i;
    short count = 0;
    float angle;
    unsigned short monster_set;
    unsigned char entry_value;
    int options;

    if (!enable) {
        if (!(state->state_flags & 1)) {
            return;
        }
        if (GameLoopState.pause_flags & 0x108000) {
            return;
        }
        angle = Get_Angle_Relative(&GameLoopState.player->position,
                                   &enemy->head.position,
                                   GameLoopState.player->rotation.y);
        for (;;) {
            if (Check_Angle(angle, -1.2217306f, 1.2217306f) == 1) {
                facing[0] = 0;
                break;
            }
            if (Check_Angle(angle, 1.2217306f, 2.6179938f) == 1) {
                facing[0] = 1;
                break;
            }
            if (Check_Angle(angle, -2.6179938f, 1.2217306f) == 1) {
                facing[0] = 1;
            } else {
                facing[0] = 2;
            }
            break;
        }
        angle = Get_Angle_Relative(&enemy->head.position,
                                   &GameLoopState.player->position,
                                   enemy->head.rotation.y);
        for (;;) {
            if (Check_Angle(angle, -1.2217306f, 1.2217306f) == 1) {
                facing[1] = 0;
                break;
            }
            if (Check_Angle(angle, 1.2217306f, 2.6179938f) == 1) {
                facing[1] = 1;
                break;
            }
            if (Check_Angle(angle, -2.6179938f, 1.2217306f) == 1) {
                facing[1] = 1;
            } else {
                facing[1] = 2;
            }
            break;
        }
        state->encounter_side = sides[facing[1] + facing[0] * 3];
        GameLoopState.encount.encounter_side = state->encounter_side;
    } else {
        GameLoopState.encount.encounter_side = command;
    }
    for (i = 0; i < 8; i++) {
        if (params->monster_set_numbers[i] != -1) {
            count++;
        }
    }
    monster_set = params->monster_set_numbers[Get_Rnd(0, count - 1)];
    GameLoopState.encount.monster_set_number = monster_set;
    entry_value = state->encounter_value;
    GameLoopState.encount.entry_value = (signed char)entry_value;
    options = state->stiff_flag;
    GameLoopState.event_flags |= 1;
    GameLoopState.encount.event_flags = params->encounter_event_flags;
    GameLoopState.encount.enemy = enemy;
    GameLoopState.encount.trigger_flags = 0;
    if (options & 1) {
        GameLoopState.encount.trigger_flags = 2;
    }
    if (options & 2) {
        GameLoopState.encount.trigger_flags |= 1;
    }
    if (options & 4) {
        GameLoopState.encount.trigger_flags |= 4;
    }
}

void Enemy_After_Battle(int victory)
{
    EnemyParams *params = &actor[GameLoopState.encount.enemy->number].hearing;
    short i;

    if (victory == 1) {
        if (params->linked_actors[0] != -2) {
            actor[GameLoopState.encount.enemy->number].active_state = 0;
        }
        for (i = 0; i < 4; i++) {
            if (params->linked_actors[i] >= 0) {
                if (actor[Get_ActorNumber(params->linked_actors[i])].active_state != 0) {
                    actor[Get_ActorNumber(params->linked_actors[i])].active_state = 0;
                }
            }
        }
    } else {
        actor[GameLoopState.encount.enemy->number].battle_state = 16;
    }
    for (i = 0; i < 16; i++) {
        if (actor[i].active_state > 0 && actor[i].status != 1) {
            if (enepc[i].state_flags & 1) {
                if (enepc[i].elec_timer != 0) {
                    enepc[i].elec_timer = 1;
                }
                if (enepc[i].action == 7) {
                    enepc[i].stiff_timer = 0;
                }
                if (enepc[i].status_4_timer != 0) {
                    enepc[i].status_4_timer = 1;
                }
                if (enepc[i].action == 6) {
                    enepc[i].spline_parameter = 0;
                    BSpline_Init(enepc[i].spline_coordinates[0],
                                 actor[i].head.position.elements,
                                 actor[i].target_angle);
                    Enemy_ActionReady((struct Actor *)&actor[i], 2);
                    Homing_Search(&actor[i]);
                }
            }
        }
    }
}

void Set_Spline_By_Route(struct Actor *actor, short route_offset)
{
    EnemyWorkEntry *state;
    float *point_data;
    short point;

    state = &enepc[actor->number];
    point = 0;

    if (state->route_point_count > 0) {
        /* One walking pointer covers both arrays, as the original does: it
         * addresses the spline point's z at 0(v1) and the route point's x
         * 1026 floats further on, which is route_points minus
         * spline_points. Only the three coordinates of each record are
         * copied. Initialized inside the positive-count branch so
         * ee-gcc2_96 -O2 -G8 emits addiu v1,a0,0x88 after blez, matching the
         * original. */
        point_data = &state->spline_coordinates[0][2];
        do {
            point_data[-2] = point_data[1026];
            point_data[-1] = point_data[1027];
            point_data[0] = point_data[1028];
            point++;
            point_data += 4;
        } while (point < state->route_point_count);
    }

    state->spline_point_count = state->route_point_count;
    state->spline_parameter =
        (state->route_point_count + route_offset) *
                    ENEMY_SPLINE_UNITS_PER_POINT + 2;
}

void Set_Spline_By_Random(struct Actor *actor)
{
    EnemyWorkEntry *state;
    float (*spline_points)[4];

    state = &enepc[actor->number];
    spline_points = state->spline_coordinates;
    BSpline_Init(spline_points[0], &actor->position.x,
                 actor->target_angle);
    state->spline_point_count = 4;
    state->fast_point_count = 1;
    state->spline_parameter = 0;
}

void Set_PlayerHistory(struct Actor *actor)
{
    float result[4];

    if (Check_Straight_ID(actor->body_radius,
                          PlayHis[PhCunt[0] - 1],
                          &enepc[ACTOR_NUMBER(actor)].escape_target,
                          32, result, 772) < 32) {
        PlayHis[PhCunt[0]][0] = BackPos[0];
        PlayHis[PhCunt[0]][1] = BackPos[1];
        PlayHis[PhCunt[0]][2] = BackPos[2];
        PlayHis[PhCunt[0]][3] = BackPos[3];
        PhCunt[0] = (PhCunt[0] + 1) % 64;
    }
}

void Homing_Search(ActorEnemyDetectView *enemy)
{
    EnemyWorkEntry *state = &enepc[enemy->slot_number];
    float distances[64];
    short order[64];
    float result[4];
    short i;
    short j;
    short parent = 0;
    short path_length;
    int steps_walked;
    float closest;
    float previous_x;
    float previous_y;
    float previous_z;
    float heading;
    float angle;
    short *order_base;

    state->sense_block_timer = 60;
    if (state->homing_count > 0) {
        for (i = 0; i < state->homing_count; i++) {
            distances[i] = Get_Distance(&enemy->head.position.coordinates, &state->homing_positions[i].vector);
        }
        for (i = 0; i < state->homing_count; i++) {
            closest = 1000.0f;
            path_length = -1;
            for (j = 0; j < state->homing_count; j++) {
                if (distances[j] < closest && 0.0f < distances[j]) {
                    closest = distances[j];
                    path_length = j;
                }
            }
            distances[path_length] = 0.0f;
            order[i] = path_length;
        }
        for (i = 0; i < state->homing_count; i++) {
            if (Check_Straight_ID(enemy->interaction_radius, enemy->head.position.elements,
                                  state->homing_positions[order[i]].elements, 32, result, 772) == 32) {
                parent = order[i];
                break;
            }
        }
        order_base = order;
        order_base[0] = parent;
        steps_walked = 0;
        path_length = 1;
        if (parent != 0) {
            for (;;) {
                for (i = 0; i < state->homing_neighbour_count[parent]; i++) {
                    if (state->homing_neighbours[parent][i] < parent) {
                        parent = state->homing_neighbours[parent][i];
                    }
                }
                steps_walked++;
                if (parent == 0) {
                    order[path_length++] = 0;
                    break;
                }
                if (steps_walked > 0xfffe) {
                    break;
                }
                order[path_length++] = parent;
            }
        }
        heading = Get_Angle(&enemy->head.position.coordinates, &state->homing_positions[order[i]].vector);
        enemy->target_angle = heading;
        BSpline_Init(state->spline_coordinates[0], enemy->head.position.elements, heading);
        state->spline_point_count = 3;
        state->spline_parameter = 16;
        previous_x = enemy->head.position.elements[0];
        previous_y = enemy->head.position.elements[1];
        previous_z = enemy->head.position.elements[2];
        for (i = 0; i < path_length; i++) {
            float x = state->homing_positions[order[i]].elements[0];
            float y = state->homing_positions[order[i]].elements[1];
            float z = state->homing_positions[order[i]].elements[2];
            short steps = (short)(__builtin_sqrtf((previous_x - x) * (previous_x - x) +
                                                  (previous_z - z) * (previous_z - z)) / 5.0f);

            if (steps != 0) {
                steps++;
                for (j = 0; j < steps; j++) {
                    TM_Script_Spline_Add(enemy->slot_number,
                                         previous_x + (x - previous_x) * (j + 1) / steps,
                                         previous_y + (y - previous_y) * (j + 1) / steps,
                                         previous_z + (z - previous_z) * (j + 1) / steps);
                }
            } else {
                TM_Script_Spline_Add(enemy->slot_number, x, y, z);
            }
            previous_x = x;
            previous_y = y;
            previous_z = z;
        }
        angle = Get_Angle(&state->spline_points[state->spline_point_count - 2],
                          &state->spline_points[state->spline_point_count - 1]);
        for (i = 0; i < 2; i++) {
            /* The original forms these two addresses as the byte offset
             * index * 16 + field added to state (addiu then addu); the
             * typed homing_positions[last].elements[0] forms tried (see the
             * R09 rematch log) add state first. */
            short last = order[path_length - 1];
            int x_offset = 0x30a0 + last * 16;
            float *x_pointer = (float *)((char *)state + x_offset);
            float tail_x = *x_pointer + sinf(angle) * 0.005f * i;
            short tail = order[path_length - 1];
            int z_offset = 0x30a8 + tail * 16;
            float *z_pointer = (float *)((char *)state + z_offset);
            float tail_z = *z_pointer + cosf(angle) * 0.005f * i;

            TM_Script_Spline_Add(enemy->slot_number, tail_x,
                                 state->homing_positions[order[path_length - 1]].elements[1], tail_z);
        }
        enemy->head.flags |= 0x80000000;
        Enemy_ActionReady((struct Actor *)enemy, 4);
    }
}

void Homing_Add(int actor_index, float x, float y, float z, short target_index)
{
    EnemyHomingAddState *state = (EnemyHomingAddState *)&enepc[actor[actor_index].slot_number];
    short position_index = state->homing_count;

    state->homing_positions[position_index][0] = x;
    state->homing_positions[position_index][1] = y;
    state->homing_positions[position_index][2] = z;

    if (position_index != target_index) {
        state->homing_linked_target[position_index] = target_index;
        if (target_index != -1) {
            state->homing_neighbours[position_index][state->homing_neighbour_count[position_index]] = target_index;
            state->homing_neighbours[target_index][state->homing_neighbour_count[target_index]] = position_index;
            state->homing_neighbour_count[position_index]++;
            state->homing_neighbour_count[target_index]++;
        }
    }
    state->homing_count++;
}

void Refresh_Homing(struct Actor *actor)
{
    EnemyWorkEntry *state;
    short positionCount;
    short positionIndex;
    int neighbourIndex;
    state = &enepc[ACTOR_NUMBER(actor)];

    for (positionIndex = 0; positionIndex < 64; positionIndex++) {
        state->homing_neighbour_count[positionIndex] = 0;
        for (neighbourIndex = 3; neighbourIndex >= 0; neighbourIndex--) {
            state->homing_neighbours[positionIndex][neighbourIndex] = -1;
        }
    }

    positionCount = state->homing_count;
    state->homing_count = 0;
    for (positionIndex = 0; positionIndex < positionCount; positionIndex++) {
        float x;
        float y;
        float z;
        short targetIndex;

        x = state->homing_positions[positionIndex].elements[0];
        y = state->homing_positions[positionIndex].elements[1];
        z = state->homing_positions[positionIndex].elements[2];
        targetIndex = state->homing_linked_target[positionIndex];
        Homing_Add(ACTOR_NUMBER(actor), x, y, z, targetIndex);
    }
}

void Disp_Homing(void)
{
    ActorEnemyDetectView *debug_actor = &actor[DebugActor];
    EnemyWorkEntry *state = &enepc[debug_actor->slot_number];
    short nearest = -1;
    float nearest_distance;
    short d;
    short i;
    short k;

    if (GameLoopState.player->number != DebugActor) {
        nearest_distance = 1000.0f;
        for (i = 0; i < state->homing_count; i++) {
            float distance = Get_Distance(&GameLoopState.player->position, &state->homing_positions[i].vector);

            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = i;
            }
        }
        if (PadData.half_2a & 0x8) {
            SelNum = -1;
        }
        if (PadData.half_2a & 0x80) {
            if (SelNum == nearest) {
                SelNum = -1;
            } else if (SelNum != -1) {
                state->homing_linked_target[SelNum] = nearest;
                Refresh_Homing((struct Actor *)debug_actor);
            } else {
                SelNum = nearest;
            }
        }
        if (((SetMotionPad *)&PadData)->analog_level == 255) {
            state->homing_positions[SelNum].vector = GameLoopState.player->position;
        }
        if ((PadData.half_2a & 0x40) && SelNum != -1) {
            for (i = SelNum; i < state->homing_count - 1; i++) {
                for (k = 0; k < 4; k++) {
                    state->homing_neighbours[i][k] = state->homing_neighbours[i + 1][k];
                }
                state->homing_neighbour_count[i] = state->homing_neighbour_count[i + 1];
                state->homing_linked_target[i] = state->homing_linked_target[i + 1];
                state->homing_positions[i] = state->homing_positions[i + 1];
            }
            state->homing_count--;
            for (d = 0; d < state->homing_count; d++) {
                short kept = 0;

                for (k = 0; k < state->homing_neighbour_count[d]; k++) {
                    if (SelNum != state->homing_neighbours[d][k]) {
                        if (SelNum < state->homing_neighbours[d][k]) {
                            state->homing_neighbours[d][kept++] = state->homing_neighbours[d][k] - 1;
                        } else {
                            state->homing_neighbours[d][kept++] = state->homing_neighbours[d][k];
                        }
                    }
                }
                state->homing_neighbour_count[d] = kept;
                if (SelNum == state->homing_linked_target[d]) {
                    state->homing_linked_target[d] = -1;
                }
                if (SelNum < state->homing_linked_target[d]) {
                    state->homing_linked_target[d]--;
                }
            }
            SelNum = -1;
        }
        if ((PadData.half_2a & 0x20) && SelNum != -1) {
            for (d = state->homing_count; d > SelNum; d--) {
                for (k = 0; k < 4; k++) {
                    state->homing_neighbours[d][k] = state->homing_neighbours[d - 1][k];
                }
                state->homing_neighbour_count[d] = state->homing_neighbour_count[d - 1];
                state->homing_linked_target[d] = state->homing_linked_target[d - 1];
                state->homing_positions[d] = state->homing_positions[d - 1];
            }
            state->homing_count = state->homing_count + 1;
            state->homing_positions[SelNum].vector = GameLoopState.player->position;
            state->homing_linked_target[SelNum] = -1;
            state->homing_neighbour_count[SelNum] = 0;
            for (i = 0; i < state->homing_count; i++) {
                for (k = 0; k < state->homing_neighbour_count[i]; k++) {
                    if (state->homing_neighbours[i][k] >= SelNum) {
                        state->homing_neighbours[i][k]++;
                    }
                }
                if (state->homing_linked_target[i] >= SelNum) {
                    state->homing_linked_target[i]++;
                }
            }
        }
        if (PadData.half_2a & 0x4) {
            for (d = 0; d < state->homing_count; d++) {
                for (k = 0; k < state->homing_neighbour_count[d]; k++) {
                }
            }
        }
        for (d = 0; d < state->homing_count; d++) {
            if (d == SelNum) {
                Debug_Mark(&state->homing_positions[d].vector, (int)AdrsEnemyExclamation);
            } else if (d == nearest) {
                Debug_Mark(&state->homing_positions[d].vector, (int)AdrsEnemyQuestion);
            } else if (state->homing_linked_target[d] == -1) {
                Debug_Mark(&state->homing_positions[d].vector, (int)AdrsEnemySquare);
            } else {
                Debug_Mark(&state->homing_positions[d].vector, (int)AdrsEnemySphere);
            }
        }
        for (d = 0; d < state->homing_count; d++) {
            for (k = 0; k < state->homing_neighbour_count[d]; k++) {
                float point[3];
                short from;
                short to;

                if (d < state->homing_neighbours[d][k]) {
                    to = state->homing_neighbours[d][k];
                    from = d;
                } else {
                    to = d;
                    from = state->homing_neighbours[d][k];
                }

                point[0] = state->homing_positions[to].elements[0] + (state->homing_positions[from].elements[0] - state->homing_positions[to].elements[0]) * Times * 0.015625f;
                point[1] = state->homing_positions[to].elements[1] + (state->homing_positions[from].elements[1] - state->homing_positions[to].elements[1]) * Times * 0.015625f;
                point[2] = state->homing_positions[to].elements[2] + (state->homing_positions[from].elements[2] - state->homing_positions[to].elements[2]) * Times * 0.015625f;
                Debug_Mark((const Vector4 *)point, (int)AdrsEnemySphere);
            }
        }
        Times = (short)((Times + 1) % 64);
    }
}

void Set_Shadow(struct Actor *reference_actor)
{
    float nearest_y;
    float nearest_distance = 5000.0f;
    AlignedHomingPosition nearest_position;
    short actor_index;

    for (actor_index = 0; actor_index < 16; actor_index++) {
        EnemyWorkEntry *state = &enepc[actor_index];

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

    if (nearest_distance != SHADOW_NO_NEAREST_DISTANCE) {
        struct Actor *shadow_actor = reference_actor;
        shadow_actor->shadow_offset.x = nearest_position.coordinates.x - reference_actor->position.x;
        nearest_y = nearest_position.coordinates.y;
        shadow_actor->shadow_offset.y = reference_actor->position.y - nearest_y;
        shadow_actor->shadow_offset.z = nearest_position.coordinates.z - reference_actor->position.z;
        shadow_actor->shadow_offset.w = 1.0f;
    }
}

int Get_MostNear_Actor(struct Actor *reference_actor)
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
                            reference_actor->body_radius)) {
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

void Actor_LookAt(struct Actor *viewer)
{
    EnemyWorkEntry *state = &enepc[ACTOR_NUMBER(viewer)];
    unsigned int remaining = (unsigned short)*ACTOR_LOOKAT_TIMER(viewer);
    int target_index;

    if (remaining - 1U >= 2U) {
        return;
    }
    target_index = ACTOR_LOOKAT_TARGET(viewer)[0];
    if (target_index == -1) {
        return;
    }
    state->look_target.lookat_point = &state->lookat_position.x;
    if (actor[target_index].has_head_position != 0) {
        state->lookat_position.x = actor[target_index].head_position[0];
        state->lookat_position.y = actor[target_index].head_position[1];
        state->lookat_position.z = actor[target_index].head_position[2];
    } else {
        state->lookat_position.x = actor[target_index].head.position.elements[0];
        state->lookat_position.y = actor[target_index].head.position.elements[1] +
                                   enepc[target_index].value_0c;
        state->lookat_position.z = actor[target_index].head.position.elements[2];
    }
}

int Actor_LookAt_Set(struct Actor *actor, signed char duration,
                     const Vector4 *target_position)
{
    EnemyWorkEntry *state;
    float *look_at_position;

    state = &enepc[ACTOR_NUMBER(actor)];
    if (duration < *ACTOR_LOOKAT_TIMER(actor))
    {
        return 0;
    }
    look_at_position = &state->lookat_position.x;
    look_at_position[0] = target_position->x;
    state->look_target.lookat_point = look_at_position;
    *ACTOR_LOOKAT_TIMER(actor) = duration;
    state->lookat_position.y = target_position->y;
    state->lookat_position.z = target_position->z;
    return 1;
}

int Actor_LookAt_Release(struct Actor *actor, signed char duration)
{
    EnemyWorkEntry *state;

    state = &enepc[ACTOR_NUMBER(actor)];
    if (duration < *ACTOR_LOOKAT_TIMER(actor))
    {
        return 0;
    }
    if (state->look_target.lookat_point == 0)
    {
        return 1;
    }
    state->look_target.lookat_point = 0;
    *ACTOR_LOOKAT_TIMER(actor) = 0;
    *ACTOR_LOOKAT_TARGET(actor) = -1;
    return 1;
}

void Actor_LookAt_Init(struct Actor *actor)
{
    *ACTOR_LOOKAT_TIMER(actor) = 0;
    *ACTOR_LOOKAT_TARGET(actor) = -1;
    enepc[ACTOR_NUMBER(actor)].look_target.lookat_point = 0;
}
