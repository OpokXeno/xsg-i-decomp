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

#define PLAYER_HISTORY_COUNT 64   /* positions PlayHis remembers, a ring indexed by PhCunt */
#define ENEMY_SENSE_BLOCK_FRAMES 60 /* frames an enemy ignores new sightings after Homing_Search */
#define ENEMY_STRAIGHT_CHECK_STEPS 32 /* sample count of the Check_Straight_ID walks */
#define HOMING_STEP_LENGTH 5.0f     /* distance between spline points added along a homing leg */
#define HOMING_SEARCH_RANGE 1000.0f /* farthest homing position considered */

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
 * TU-local view of struct SoundWork from main/xgl_sound.c. effect_banks starts
 * at +0x20, and each entry's low halfword is its handle (SoundEffectBankEntry.handle),
 * the field xglSoundEffectStopDirect reads.
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
 * has alias set 0 and keeps that load below them. */
typedef struct EnemyHomingAddState {
    unsigned char unmodeled_00[0x30a0];
    float homing_positions[ENEMY_HOMING_MAX][4];
    short homing_neighbours[ENEMY_HOMING_MAX][ENEMY_HOMING_NEIGHBOURS];
    short homing_neighbour_count[ENEMY_HOMING_MAX];
    short homing_linked_target[ENEMY_HOMING_MAX];
    short homing_count;
} EnemyHomingAddState;

extern short SelNum;
extern ActorEnemyDetectView actor[ACTOR_COUNT];
extern EnemyWorkEntry enepc[ENEMY_COUNT];
extern SetMotionSoundWork SoundWork;
extern UnduParam UnduTemp;
extern UnduTestStorage UnduTest;
extern DiscoveryGameLoopState GameLoopState;
extern float PlayHis[PLAYER_HISTORY_COUNT][4];
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

float PlayHis[PLAYER_HISTORY_COUNT][4] = {{0.0f}};

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

/* Check_Encount classifies the bearing between the player and an enemy:
 * within 70 degrees of straight ahead is the front, beyond 150 degrees the
 * back, in between the side. */
#define ENCOUNTER_FRONT_ARC 1.2217306f
#define ENCOUNTER_BACK_ARC 2.6179938f
#define ENCOUNTER_MONSTER_SET_COUNT 8

/* ActorTalkControls.talk_flags bits Before_Talk / After_Talk honour. */
#define TALK_FLAG_TALK_MOTION 0x1    /* switch to the talk motion and back */
#define TALK_FLAG_LOOK_AT_PLAYER 0x4 /* turn the head towards the player */
#define TALK_FLAG_FREEZE 0x10        /* hand the enemy to the script */

/*
 * Starts motion on actor through ACT_setMotion2. Enemies get the 0x20 mode bit.
 * With the custom blend flag, the mode also gets 0x10000000 and the enemy
 * entry's blend weight is copied into the actor's blend work.
 */
void Set_Motion(struct Actor *actor, int motion, unsigned int mode)
{
    struct Actor *motion_actor = actor;
    EnemyWorkEntry *enemy_entry = &enepc[ACTOR_NUMBER(actor)];
    unsigned int flags;
    unsigned int motion_mode;
    unsigned int enemy_mode;
    int is_enemy_actor;

    if ((enemy_entry->state_flags & ENEMY_STATE_FLAG_CUSTOM_BLEND) != 0) {
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

/* Plays a walk or run footstep sound when the animation frame crosses a FootStep threshold. */
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
    if (motion != MOTION_ID_WALK && motion != MOTION_ID_RUN) {
        return;
    }
    if ((actor->animation.slot.flags & 2) != 0) {
        return;
    }

    foot_step_index =
        (short)RES_GetEnemySeFoot(*ACTOR_SOUND_EFFECT_ID(actor));
    sound_index = 0;
    thresholds = &FootStep[foot_step_index * 4];
    if (actor->animation.slot.currentDataId == MOTION_ID_RUN) {
        thresholds += 2;
    }
    previous_step = actor->last_step_distance;
    current_step = F2I(actor->animation.slot.frame / 0.033333335f);

    if ((previous_step < (short)thresholds[0] &&
         (short)thresholds[0] <= current_step) ||
        (current_step < previous_step &&
         previous_step < (short)thresholds[0])) {
        sound_index = FOOT_STEP_WALK_A;
        if (actor->animation.slot.currentDataId == MOTION_ID_RUN) {
            sound_index = FOOT_STEP_RUN_A;
        }
        second_step = 0;
    }

    if ((previous_step < (short)thresholds[1] &&
         (short)thresholds[1] <= current_step) ||
        (current_step < previous_step &&
         previous_step < (short)thresholds[1])) {
        sound_index = FOOT_STEP_WALK_B;
        if (actor->animation.slot.currentDataId == MOTION_ID_RUN) {
            sound_index = FOOT_STEP_RUN_B;
        }
        second_step = 1;
    }
    actor->last_step_distance = current_step;
    if (sound_index == 0) {
        return;
    }

    if (actor->animation.slot.currentDataId == MOTION_ID_WALK &&
        enemy_entry->action == ENEMY_ACT_ROUTE) {
        int sound_number = ENEMY_SE_WALK_ALT;
        if (!second_step) {
            sound_number = ENEMY_SE_WALK;
        }
        EnemySound(actor, sound_number, 1, 1);
    }
    if (actor->animation.slot.currentDataId == MOTION_ID_RUN) {
        if (enemy_entry->action == ENEMY_ACT_ROUTE) {
            EnemySound(actor, second_step ? ENEMY_SE_RUN_ALT : ENEMY_SE_RUN, 1, 1);
        }
        if (actor->animation.slot.currentDataId == MOTION_ID_RUN &&
            enemy_entry->action == ENEMY_ACT_CHASE) {
            EnemySound(actor, second_step ? ENEMY_SE_RUN_ALT : ENEMY_SE_RUN, 1, 1);
        }
    }
}

/*
 * Plays sound_index for actor from its enemy sound bank. Nothing plays when
 * the actor is hidden, or when play is not 1. When ignore_se_type is 0 and the
 * sound is a walk or run sound, the call returns early for banks whose
 * RES_GetEnemySeType is 1.
 */
void EnemySound(struct Actor *actor, short sound_index, signed char play,
                 signed char ignore_se_type)
{
    int bank;

    if ((sound_index == ENEMY_SE_WALK || sound_index == ENEMY_SE_RUN) && ignore_se_type == 0 &&
        RES_GetEnemySeType(*ACTOR_SOUND_EFFECT_ID(actor)) == 1)
    {
        return;
    }
    if (actor->flags & ACTOR_FLAG_HIDDEN)
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
    /* The original keeps a real call here, followed by its own register
     * restores and return, instead of folding the call into a sibling jump.
     * This no-op loop keeps the call out of tail position and reproduces that
     * real call plus epilogue. */
    do {
    } while (0);
}

/* Stops the enemy sounds of every enemy actor, then the three damage sound effects. */
void EnemySound_StopAll(signed char which)
{
    short actor_index;

    for (actor_index = 0; actor_index < ENEMY_COUNT; actor_index++)
    {
        EnemySound_Stop((struct Actor *)&actor[actor_index], which);
    }
    xglSoundEffectStopDirect(SE_ID_DAMAGE_ELECTRIC);
    xglSoundEffectStopDirect(SE_ID_DAMAGE_EXPLOSION);
    xglSoundEffectStopDirect(SE_ID_DAMAGE_SEAL);
}

/*
 * Stops channels 1-7 of the actor's enemy sound bank. which == 1 fades them
 * out with SsdFadeoutEffect; any other value stops them directly.
 */
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

/* Applies the talk flags when talk starts: talk motion, look at the player, freeze. */
void Before_Talk(struct Actor *actor)
{
    ActorTalk *talk_actor = (ActorTalk *)actor;
    ActorTalkControls *controls = &talk_actor->controls;
    EnemyWorkEntry *enemy_state = &enepc[ACTOR_NUMBER(actor)];

    if (controls->talk_flags & TALK_FLAG_TALK_MOTION) {
        enemy_state->saved_motion = talk_actor->saved_talk_motion;
        Set_Motion(actor, Get_DefaultMotion(actor, ENEMY_DEFAULT_MOTION_TALK), 1);
        talk_actor->motion_progress = 0;
        talk_actor->talk_state = 9;
    }
    if (controls->talk_flags & TALK_FLAG_LOOK_AT_PLAYER) {
        Enemy_Command_LookAt(actor, ENEMY_TARGET_PLAYER);
    }
    if (controls->talk_flags & TALK_FLAG_FREEZE) {
        Enemy_Command_Freeze(actor, ENEMY_FREEZE_SCRIPTED);
        /* This path ends with a sibling jump in the original, unlike
         * the after-talk path, which returns through its own epilogue. */
    }
}

/* Undoes the talk flags when talk ends: restores the saved motion, releases look-at and freeze. */
void After_Talk(struct Actor *actor)
{
    ActorTalk *talk_actor = (ActorTalk *)actor;
    ActorTalkControls *controls = &talk_actor->controls;
    EnemyWorkEntry *enemy_state = &enepc[ACTOR_NUMBER(actor)];

    if (controls->talk_flags & TALK_FLAG_TALK_MOTION) {
        Set_Motion(actor, enemy_state->saved_motion, MOTION_BLEND_DEFAULT);
    }
    if (controls->talk_flags & TALK_FLAG_LOOK_AT_PLAYER) {
        Enemy_Command_LookAt(actor, ENEMY_LOOK_AT_RELEASE);
    }
    do {
        if (!(controls->talk_flags & TALK_FLAG_FREEZE))
            break;
        Enemy_Command_Freeze(actor, ENEMY_FREEZE_RELEASE);
    } while (0);
}

/* Returns 1 if an active enemy with the encounter flag is chasing, tentacling or found. */
int Check_EnemyFound(void)
{
    short enemy_index;

    for (enemy_index = 0; enemy_index < ENEMY_COUNT; enemy_index++) {
        if (actor[enemy_index].active_state > 0 &&
            actor[enemy_index].status != 1 &&
            (enepc[enemy_index].state_flags & ENEMY_STATE_FLAG_ENCOUNTER) &&
            (enepc[enemy_index].action == ENEMY_ACT_CHASE ||
             enepc[enemy_index].action == ENEMY_ACT_TENTACLE ||
             enepc[enemy_index].action == ENEMY_ACT_FOUND)) {
            return 1;
        }
    }
    return 0;
}

/* Returns 1 if an active enemy is in its explosion or stiff action. */
int Check_EnemyBurn(void)
{
    short enemy_index;

    for (enemy_index = 0; enemy_index < ENEMY_COUNT; enemy_index++) {
        if (actor[enemy_index].active_state > 0 &&
            actor[enemy_index].status != 1) {
            EnemyWorkEntry *work = &enepc[enemy_index];

            if (work->action == ENEMY_ACT_DAMAGE_EXPLOSION || work->action == ENEMY_ACT_STIFF) {
                return 1;
            }
        }
    }
    return 0;
}

/* Returns 1 if an active enemy is in its electric damage action or has the electric stiff flag. */
int Check_EnemyElec(void)
{
    short enemy_index;

    for (enemy_index = 0; enemy_index < ENEMY_COUNT; enemy_index++) {
        if (actor[enemy_index].active_state > 0 &&
            actor[enemy_index].status != 1 &&
            (enepc[enemy_index].action == ENEMY_ACT_DAMAGE_ELECTRIC ||
             (enepc[enemy_index].stiff_flag & ENEMY_STIFF_ELECTRIC))) {
            return 1;
        }
    }
    return 0;
}

/*
 * Runs Check_Discovery for each actor other than enemy that is within its
 * hearing distance of sound_position. Does nothing while PAD_L3 is held.
 */
void Enemy_FindByEar(ActorEnemyDetectView *enemy, const Vector4 *sound_position)
{
    short index;

    if (PadData.half_28 & PAD_L3) {
        return;
    }
    for (index = 0; index < ACTOR_COUNT; index++) {
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

/* Advances the active scripted move and turn of actor by one frame. */
void Script_Action(struct Actor *actor)
{
    EnemyWorkEntry *script = &enepc[ACTOR_NUMBER(actor)];

    if (script->state_flags & ENEMY_STATE_FLAG_SCRIPT_ACTIVE) {
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

/* Moves actor 8 along the belt conveyor floor direction that UnduCheck finds under it. */
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
        surface_flags = flags & ACTOR_ATTR_FLOOR_MASK;
        direction_mask = surface_flags & ACTOR_ATTR_FLOOR_DIR_MASK;
        if (direction_mask != 0) {
            unsigned char direction = idx_0[(unsigned int)direction_mask >> ACTOR_ATTR_FLOOR_DIR_SHIFT];
            float distance = rate_2_003B2008[(unsigned int)surface_flags >> ACTOR_ATTR_FLOOR_SPEED_SHIFT] * 0.1024f;
            float *direction_vector = vec_1[direction];
            actor->position.x += distance * direction_vector[0];
            actor->position.z += distance * direction_vector[1];
        }
    }
}

/*
 * Makes enemy react to spotting the player: it faces the player and starts a
 * tentacle or found action, chosen by pause_kind. Hidden enemies, enemies
 * already in a found, damage, stiff, tentacle, init or escape action, and a
 * flashing player are skipped.
 */
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
    if (enemy->flags & ACTOR_FLAG_HIDDEN) {
        return;
    }
    player = GameLoopState.player;
    if (player->flash_timer) {
        return;
    }
    if ((unsigned char)state->action - ENEMY_ACT_FOUND < 2U) {
        return;
    }
    detection_state = state->action;
    if (detection_state == ENEMY_ACT_DAMAGE_EXPLOSION) {
        return;
    }
    if (detection_state == ENEMY_ACT_DAMAGE_ELECTRIC) {
        return;
    }
    if (detection_state == ENEMY_ACT_DAMAGE_SEAL) {
        return;
    }
    if (detection_state == ENEMY_ACT_STIFF) {
        return;
    }
    if (detection_state == ENEMY_ACT_TENTACLE) {
        return;
    }
    if (detection_state == ENEMY_ACT_INIT) {
        return;
    }
    if (detection_state == ENEMY_ACT_ESCAPE) {
        return;
    }
    enemy->flags |= ACTOR_FLAG_FACE_PLAYER;
    if (FLAG_FRAME_60 != 0 && (PadData.half_28 & PAD_L3)) {
        return;
    }
    switch (state->pause_kind) {
    case 2: case 3: case 4: case 7:
        Enemy_ActionReady(enemy, ENEMY_ACT_TENTACLE);
        break;
    case 6: case 13:
        break;
    case 5: case 8: case 9: case 10: case 11: case 12:
    default:
        Enemy_ActionReady(enemy, ENEMY_ACT_FOUND);
        state->last_player_position.vector = GameLoopState.player->position;
        break;
    }
}

/*
 * Fills GameLoopState.encount for an encounter with enemy. With enable 0 the
 * encounter side comes from the player-enemy bearing (and nothing happens
 * unless the enemy has the encounter flag and no pause flags are set);
 * otherwise the side is command. A monster set is picked at random.
 */
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
        if (!(state->state_flags & ENEMY_STATE_FLAG_ENCOUNTER)) {
            return;
        }
        if (GameLoopState.pause_flags & 0x108000) {
            return;
        }
        /* Bearing of the enemy from the player (facing[0]), then the reverse (facing[1]). */
        angle = Get_Angle_Relative(&GameLoopState.player->position,
                                   &enemy->head.position,
                                   GameLoopState.player->rotation.y);
        for (;;) {
            if (Check_Angle(angle, -ENCOUNTER_FRONT_ARC, ENCOUNTER_FRONT_ARC) == 1) {
                facing[0] = 0;
                break;
            }
            if (Check_Angle(angle, ENCOUNTER_FRONT_ARC, ENCOUNTER_BACK_ARC) == 1) {
                facing[0] = 1;
                break;
            }
            if (Check_Angle(angle, -ENCOUNTER_BACK_ARC, ENCOUNTER_FRONT_ARC) == 1) {
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
            if (Check_Angle(angle, -ENCOUNTER_FRONT_ARC, ENCOUNTER_FRONT_ARC) == 1) {
                facing[1] = 0;
                break;
            }
            if (Check_Angle(angle, ENCOUNTER_FRONT_ARC, ENCOUNTER_BACK_ARC) == 1) {
                facing[1] = 1;
                break;
            }
            if (Check_Angle(angle, -ENCOUNTER_BACK_ARC, ENCOUNTER_FRONT_ARC) == 1) {
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
    /* Counts the monster sets in use, then picks one at random. */
    for (i = 0; i < ENCOUNTER_MONSTER_SET_COUNT; i++) {
        if (params->monster_set_numbers[i] != -1) {
            count++;
        }
    }
    monster_set = params->monster_set_numbers[Get_Rnd(0, count - 1)];
    GameLoopState.encount.monster_set_number = monster_set;
    entry_value = state->encounter_value;
    GameLoopState.encount.entry_value = (signed char)entry_value;
    /* Trigger flags from the enemy's stiff state. */
    options = state->stiff_flag;
    GameLoopState.event_flags |= 1;
    GameLoopState.encount.event_flags = params->encounter_event_flags;
    GameLoopState.encount.enemy = enemy;
    GameLoopState.encount.trigger_flags = 0;
    if (options & ENEMY_STIFF_ACTIVE_BIT) {
        GameLoopState.encount.trigger_flags = 2;
    }
    if (options & ENEMY_STIFF_ELECTRIC) {
        GameLoopState.encount.trigger_flags |= 1;
    }
    if (options & ENEMY_STIFF_SEAL) {
        GameLoopState.encount.trigger_flags |= 4;
    }
}

/*
 * Ends a battle with the encounter enemy. A victory deactivates the enemy and
 * its linked actors; otherwise the enemy's battle_state becomes 16. Every
 * chasing encounter enemy then gets a new spline and a threat action.
 */
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
    for (i = 0; i < ENEMY_COUNT; i++) {
        if (actor[i].active_state > 0 && actor[i].status != 1) {
            if (enepc[i].state_flags & ENEMY_STATE_FLAG_ENCOUNTER) {
                if (enepc[i].elec_timer != 0) {
                    enepc[i].elec_timer = 1;
                }
                if (enepc[i].action == ENEMY_ACT_STIFF) {
                    enepc[i].stiff_timer = 0;
                }
                if (enepc[i].status_4_timer != 0) {
                    enepc[i].status_4_timer = 1;
                }
                if (enepc[i].action == ENEMY_ACT_CHASE) {
                    enepc[i].spline_parameter = 0;
                    BSpline_Init(enepc[i].spline_coordinates[0],
                                 actor[i].head.position.elements,
                                 actor[i].target_angle);
                    Enemy_ActionReady((struct Actor *)&actor[i], ENEMY_ACT_THREAT);
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
        /* One walking pointer covers both arrays: it addresses the spline
         * point's z at 0(v1) and the route point's x 1026 floats further on,
         * which is route_points minus spline_points. Only the three
         * coordinates of each record are copied. The pointer is set up inside
         * this branch, which keeps the original's instruction order. */
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

/* Starts an enemy spline at actor's position, with four spline points. */
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

/*
 * Appends BackPos to the PlayHis ring unless the Check_Straight_ID walk from
 * the last entry completes all its steps.
 */
void Set_PlayerHistory(struct Actor *actor)
{
    float result[4];

    if (Check_Straight_ID(actor->body_radius,
                          PlayHis[PhCunt[0] - 1],
                          &enepc[ACTOR_NUMBER(actor)].escape_target,
                          ENEMY_STRAIGHT_CHECK_STEPS, result, ENEMY_MAP_INDEX_ENEMY) < ENEMY_STRAIGHT_CHECK_STEPS) {
        PlayHis[PhCunt[0]][0] = BackPos[0];
        PlayHis[PhCunt[0]][1] = BackPos[1];
        PlayHis[PhCunt[0]][2] = BackPos[2];
        PlayHis[PhCunt[0]][3] = BackPos[3];
        PhCunt[0] = (PhCunt[0] + 1) % PLAYER_HISTORY_COUNT;
    }
}

/*
 * Starts a homing search for enemy: sets its sense block timer, then takes the
 * nearest homing node it can see straight to and walks the node links back to
 * node 0, adding spline points along that path.
 */
void Homing_Search(ActorEnemyDetectView *enemy)
{
    EnemyWorkEntry *state = &enepc[enemy->slot_number];
    float distances[ENEMY_HOMING_MAX];
    short order[ENEMY_HOMING_MAX];
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

    state->sense_block_timer = ENEMY_SENSE_BLOCK_FRAMES;
    if (state->homing_count > 0) {
        /* Orders the nodes by distance from the enemy, nearest first. */
        for (i = 0; i < state->homing_count; i++) {
            distances[i] = Get_Distance(&enemy->head.position.coordinates, &state->homing_positions[i].vector);
        }
        for (i = 0; i < state->homing_count; i++) {
            closest = HOMING_SEARCH_RANGE;
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
                                  state->homing_positions[order[i]].elements, ENEMY_STRAIGHT_CHECK_STEPS, result, ENEMY_MAP_INDEX_ENEMY) == ENEMY_STRAIGHT_CHECK_STEPS) {
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
        /* Starts the spline at the enemy and adds points along the path. */
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
                                                  (previous_z - z) * (previous_z - z)) / HOMING_STEP_LENGTH);

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
            /* Addresses the position as a byte offset (0x30a0 + index * 16)
             * added to state, as the original does; a typed
             * homing_positions[last] access changes the address arithmetic. */
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
        enemy->head.flags |= ACTOR_FLAG_HOMING;
        Enemy_ActionReady((struct Actor *)enemy, ENEMY_ACT_ROUTE);
    }
}

/*
 * Adds the point (x, y, z) to the actor's homing graph. target_index is the
 * node it links to, or -1 for none.
 */
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

/* Rebuilds the neighbour links of the actor's homing graph from its stored target links. */
void Refresh_Homing(struct Actor *actor)
{
    EnemyWorkEntry *state;
    short positionCount;
    short positionIndex;
    int neighbourIndex;
    state = &enepc[ACTOR_NUMBER(actor)];

    for (positionIndex = 0; positionIndex < ENEMY_HOMING_MAX; positionIndex++) {
        state->homing_neighbour_count[positionIndex] = 0;
        for (neighbourIndex = ENEMY_HOMING_NEIGHBOURS - 1; neighbourIndex >= 0; neighbourIndex--) {
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

/*
 * Debug editor for the homing graph of DebugActor. The pad selects, links,
 * moves, adds and deletes nodes, and the graph is drawn with Debug_Mark.
 * Does nothing while the player is DebugActor.
 */
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
        nearest_distance = HOMING_SEARCH_RANGE;
        for (i = 0; i < state->homing_count; i++) {
            float distance = Get_Distance(&GameLoopState.player->position, &state->homing_positions[i].vector);

            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = i;
            }
        }
        if (PadData.half_2a & PAD_R1) {
            SelNum = -1;
        }
        if (PadData.half_2a & PAD_SQUARE) {
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
        /* Cross deletes the selected node and shifts the arrays down. */
        if ((PadData.half_2a & PAD_CROSS) && SelNum != -1) {
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
        /* Circle inserts a node at the selected index and shifts the arrays up. */
        if ((PadData.half_2a & PAD_CIRCLE) && SelNum != -1) {
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
        if (PadData.half_2a & PAD_L1) {
            for (d = 0; d < state->homing_count; d++) {
                for (k = 0; k < state->homing_neighbour_count[d]; k++) {
                }
            }
        }
        /* Marks each node: selected, nearest, unlinked, or linked. */
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
        /* Draws a marker between each pair of linked nodes. */
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

/*
 * Sets reference_actor's shadow offset toward the nearest other enemy that
 * casts a shadow, within 5000 units. Leaves the offset unchanged when none is found.
 */
void Set_Shadow(struct Actor *reference_actor)
{
    float nearest_y;
    float nearest_distance = 5000.0f;
    AlignedHomingPosition nearest_position;
    short actor_index;

    for (actor_index = 0; actor_index < ENEMY_COUNT; actor_index++) {
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

/*
 * Returns the index of the nearest active actor that is within 8 units of
 * reference_actor and inside its front fan, or -1 if there is none.
 */
int Get_MostNear_Actor(struct Actor *reference_actor)
{
    float nearest_distance = 5.0f;
    short actor_index;
    int nearest_index = -1;

    for (actor_index = 0; actor_index < ACTOR_COUNT; actor_index++) {
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

/*
 * Updates viewer's look-at point from its look target while the look timer is
 * 1 or 2. The target is the head position when the target has one.
 */
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

/*
 * Starts a look-at of target_position lasting duration. Returns 0 and changes
 * nothing when the current look timer is longer than duration; otherwise 1.
 */
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

/*
 * Releases the look-at of actor: clears its look timer and target. Returns 0
 * without releasing when the current look timer is longer than duration.
 */
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

/* Clears the look timer, look target and look-at point of actor. */
void Actor_LookAt_Init(struct Actor *actor)
{
    *ACTOR_LOOKAT_TIMER(actor) = 0;
    *ACTOR_LOOKAT_TARGET(actor) = -1;
    enepc[ACTOR_NUMBER(actor)].look_target.lookat_point = 0;
}
