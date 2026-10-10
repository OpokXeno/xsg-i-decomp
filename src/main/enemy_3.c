#include "common.h"

#include "shared.h"
enum {
    ENEMY_ACTION_ENABLED = 0x00010000,
    ENEMY_ACTION_COUNT = 16,
    ENEMY_ACTION_INDEX_BIAS = 1,
    ENEMY_ACTION_DEFAULT_MOTION = 0,
    ENEMY_ACTION_PLAY_MOTION_27 = 1,
    ENEMY_ACTION_RANDOM_WALL_DIRECTION = 2,
    ENEMY_ACTION_DEFAULT_MOTION_ALT = 3,
    ENEMY_ACTION_PLAY_MOTION_14 = 4,
    ENEMY_ACTION_RANDOM_MOTION = 5,
    ENEMY_ACTION_STOP_MOVEMENT = 6,
    ENEMY_ACTION_TABLE_MOTION = 8,
    ENEMY_ACTION_REPLACE_EFFECT = 9,
    ENEMY_ACTION_EFFECT_SEQUENCE_A = 10,
    ENEMY_ACTION_EFFECT_SEQUENCE_B = 11,
    ENEMY_ACTION_EFFECT_SEQUENCE_C = 12,
    ENEMY_ACTION_PLAY_MOTION_10 = 15,
    ENEMY_MOTION_BLEND = 9,
    ENEMY_RANDOM_DIRECTION_COUNT = 16,
    ENEMY_ACTION_TIMER_STEP = 300,
    ENEMY_REACTION_CREATE_EFFECT = 1,
    ENEMY_EFFECT_DETACHED_FLAG = 0x00000400,
    ENEMY_ACTOR_RANDOM_MOTION_FLAG = 0x00200000,
    ENEMY_ACTOR_EFFECT_A_FLAG = 0x01000000,
    ENEMY_ACTOR_EFFECT_B_FLAG = 0x02000000,
    ENEMY_ACTOR_EFFECT_C_FLAG = 0x04000000,
    ENEMY_EFFECT_WALL_REACTION = 727,
    ENEMY_EFFECT_SPECIAL_REACTION = 1625,
    ENEMY_EFFECT_REPLACE = 1681,
    ENEMY_EFFECT_SEQUENCE_A_FIRST = 741,
    ENEMY_EFFECT_SEQUENCE_A_SECOND = 736,
    ENEMY_EFFECT_SEQUENCE_B_FIRST = 740,
    ENEMY_EFFECT_SEQUENCE_B_SECOND = 735,
    ENEMY_EFFECT_SEQUENCE_C_FIRST = 742,
    ENEMY_EFFECT_SEQUENCE_C_FINAL = 737,
    ENEMY_SOUND_DEFAULT = 1,
    ENEMY_SOUND_HIT = 2,
    ENEMY_SOUND_RANDOM = 4,
    ENEMY_SOUND_SPECIAL = 6,
    ENEMY_SOUND_ACTION = 7,
    ENEMY_ACTION_SPECIAL_SOUND_ID = 0x2201
};
#define ENEMY_RANDOM_DIRECTION_SCALE 0.0625f
typedef struct EnemyActionMotion {
    unsigned char unmodeled_00[0x18];
    u16 idle_motion_id;
    unsigned char unmodeled_1a[2];
    u16 reaction_motion_id;
    unsigned char unmodeled_1e[2];
} EnemyActionMotion;
typedef struct EnemyActionActor {
    u32 flags;
    unsigned char unmodeled_04[0x0c];
    float position[4];
    unsigned char unmodeled_20[0x60];
    u8 number;
    unsigned char unmodeled_81[5];
    short sound_effect_id;
    unsigned char unmodeled_88[0x38];
    EnemyActionMotion motion;
    unsigned char unmodeled_e0[0x618];
    float speed;
    unsigned char unmodeled_6fc[0x2e8];
    float target_angle;
    float wall_check_parameter;
} EnemyActionActor;
typedef struct EnemyActionEffect {
    unsigned char unmodeled_00[0x6bc];
    EnemyActionActor *owner;
    unsigned char unmodeled_6c0[0x3cc];
    u32 flags;
} EnemyActionEffect;
#include "enemy_3.h"
typedef struct EnemyActionGameLoopState {
    unsigned char unmodeled_00[0x2a014];
    EnemyActionEffect *shared_effect;
    unsigned char unmodeled_2a018[0x18];
} EnemyActionGameLoopState;
extern EnemyActionGameLoopState GameLoopState;
extern const float D_004D8150;
extern short Get_DefaultMotion(EnemyActionActor *actor, short motion_number);
extern void Set_Motion(EnemyActionActor *actor, short motion_id, short blend);
extern void EnemySound(EnemyActionActor *actor, short sound_index,
                       signed char play, signed char ignore_type);
extern void EnemySoundEnd(EnemyActionActor *actor, short sound_offset);
extern void Check_Wall(float distance_scale, float wall_parameter,
                       float *position, signed char wall_status[16],
                       int status_count, int wall_mask, u8 actor_number, int mode);
extern float xglFRand(void);
extern EnemyActionEffect *sefCreateEffectCf(int effect_id, int effect_number,
                                             int value);
extern void sefDeleteEffectCf(EnemyActionEffect *effect);
extern void Vibration_Set_Weak(int duration);

void Enemy_ActionReady(EnemyActionActor *actor, signed char action)
{
    EnemyReadyWork *work;
    EnemyActionMotion *motion;
    EnemyActionEffect *effect;
    int action_index;

    work = &enepc[actor->number];
    motion = &actor->motion;

    if ((actor->flags & ENEMY_ACTION_ENABLED) != 0) {
        work->action = action;
        work->motion_mode = 0;
        action_index = (int)action - ENEMY_ACTION_INDEX_BIAS;

        if ((unsigned int)action_index < ENEMY_ACTION_COUNT) {
            switch (action_index) {
            case ENEMY_ACTION_DEFAULT_MOTION_ALT:
                Set_Motion(actor,
                           Get_DefaultMotion(actor, 1),
                           ENEMY_MOTION_BLEND);
                EnemySound(actor, ENEMY_SOUND_HIT, 1, 0);
                break;

            case ENEMY_ACTION_DEFAULT_MOTION:
                work->motion_id = motion->idle_motion_id;
                Set_Motion(actor,
                           Get_DefaultMotion(actor, ENEMY_ACTION_DEFAULT_MOTION),
                           ENEMY_MOTION_BLEND);
                EnemySound(actor, ENEMY_SOUND_DEFAULT, 1, 0);
                break;

            case ENEMY_ACTION_PLAY_MOTION_10:
                work->motion_id = motion->idle_motion_id;
                Set_Motion(actor,
                           Get_DefaultMotion(actor, 0),
                           8);
                EnemySound(actor, ENEMY_SOUND_DEFAULT, 1, 0);
                work->action = 1;
                break;
            case ENEMY_ACTION_PLAY_MOTION_27:
                work->motion_id = motion->reaction_motion_id;
                Set_Motion(actor, 27, ENEMY_MOTION_BLEND);
                EnemySound(actor, ENEMY_SOUND_ACTION, 1, 1);
                break;

            case ENEMY_ACTION_RANDOM_WALL_DIRECTION: {
                signed char wall_directions[ENEMY_RANDOM_DIRECTION_COUNT];
                short direction;
                short available_count;
                int random_direction;

                Check_Wall(1.0f, actor->wall_check_parameter,
                           actor->position, wall_directions,
                           ENEMY_RANDOM_DIRECTION_COUNT, 772, actor->number, 0);

                available_count = 0;
                for (direction = 0;
                     direction < ENEMY_RANDOM_DIRECTION_COUNT;
                     direction++) {
                    if (wall_directions[direction] == 0) {
                        wall_directions[available_count++] = (signed char)direction;
                    }
                }

                if (available_count == 0) {
                    available_count = ENEMY_RANDOM_DIRECTION_COUNT;
                }
                random_direction = xglSRand() % available_count;
                work->motion_id = work->action_motion_id;
                actor->target_angle = wall_directions[random_direction] *
                                      D_004D8150 * ENEMY_RANDOM_DIRECTION_SCALE;
                Set_Motion(actor, 11, ENEMY_MOTION_BLEND);
                break;
            }

            case ENEMY_ACTION_PLAY_MOTION_14:
                work->motion_id = 14;
                Set_Motion(actor, 10, 1);
                EnemySound(actor, ENEMY_SOUND_SPECIAL, 1, 1);
                if ((work->reaction_flags & ENEMY_REACTION_CREATE_EFFECT) != 0 &&
                    GameLoopState.shared_effect == 0) {
                    GameLoopState.shared_effect =
                        sefCreateEffectCf(ENEMY_EFFECT_WALL_REACTION, 0, 0);
                    Vibration_Set_Weak(30);
                }
                if (actor->sound_effect_id == ENEMY_ACTION_SPECIAL_SOUND_ID) {
                    work->effect = sefCreateEffectCf(
                        ENEMY_EFFECT_SPECIAL_REACTION, 0, 0);
                    if (work->effect != 0) {
                        effect = work->effect;
                        effect->owner = actor;
                        effect->flags &= ~ENEMY_EFFECT_DETACHED_FLAG;
                    }
                }
                break;

            case ENEMY_ACTION_RANDOM_MOTION:
                actor->flags = (actor->flags | ENEMY_ACTOR_RANDOM_MOTION_FLAG) &
                               0x7fffffff;
                work->motion_id = 30;
                work->random_turn_x = (2.0f * xglFRand()) - 1.0f;
                work->random_turn_z = (2.0f * xglFRand()) - 1.0f;
                work->action_counter = 0;
                Set_Motion(actor, Get_DefaultMotion(actor, 2), ENEMY_MOTION_BLEND);
                EnemySoundEnd(actor, 2);
                EnemySound(actor, ENEMY_SOUND_RANDOM, 1, 0);
                break;

            case ENEMY_ACTION_TABLE_MOTION:
                work->motion_id = work->motion_id_table
                                  [work->motion_table_row]
                                  [work->motion_table_column];
                Set_Motion(actor,
                           work->action_motion_table
                           [work->motion_table_row]
                           [work->motion_table_column],
                           ENEMY_MOTION_BLEND);
                break;

            case ENEMY_ACTION_REPLACE_EFFECT: {
                EnemyActionEffect *replacement_effect;

                work->motion_id = work->initial_motion_id;
                Set_Motion(actor, 30, ENEMY_MOTION_BLEND);
                replacement_effect = work->effect;
                if (replacement_effect != 0) {
                    sefDeleteEffectCf(replacement_effect);
                }

                work->effect = sefCreateEffectCf(ENEMY_EFFECT_REPLACE, 0, 0);
                if (work->effect != 0) {
                    replacement_effect = work->effect;
                    replacement_effect->owner = actor;
                    replacement_effect->flags &= ~ENEMY_EFFECT_DETACHED_FLAG;
                    if ((work->reaction_flags & ENEMY_REACTION_CREATE_EFFECT) != 0 &&
                        GameLoopState.shared_effect == 0) {
                        GameLoopState.shared_effect =
                            sefCreateEffectCf(ENEMY_EFFECT_WALL_REACTION, 0, 0);
                        Vibration_Set_Weak(30);
                    }
                }
                break;
            }

            case ENEMY_ACTION_EFFECT_SEQUENCE_A: {
                EnemyActionEffect *prior_effect;

                work->effect_state |= 1;
                work->action_timer_a += ENEMY_ACTION_TIMER_STEP;
                work->motion_id = 30;
                actor->flags &= ~ENEMY_ACTOR_EFFECT_A_FLAG;
                prior_effect = work->effect;
                if (prior_effect != 0) {
                    sefDeleteEffectCf(prior_effect);
                }

                work->effect = sefCreateEffectCf(
                    ENEMY_EFFECT_SEQUENCE_A_FIRST, 0, 0);
                work->effect = sefCreateEffectCf(
                    ENEMY_EFFECT_SEQUENCE_A_SECOND, 0, 0);
                if (work->effect != 0) {
                    effect = work->effect;
                    effect->owner = actor;
                    effect->flags &= ~ENEMY_EFFECT_DETACHED_FLAG;
                }
                break;
            }

            case ENEMY_ACTION_EFFECT_SEQUENCE_B: {
                EnemyActionEffect *prior_effect;

                Set_Motion(actor, 29, ENEMY_MOTION_BLEND);
                work->effect_state |= 2;
                work->action_timer_b += ENEMY_ACTION_TIMER_STEP;
                work->motion_id = 90;
                actor->flags &= ~ENEMY_ACTOR_EFFECT_B_FLAG;
                prior_effect = work->effect;
                if (prior_effect != 0) {
                    sefDeleteEffectCf(prior_effect);
                }

                work->effect = sefCreateEffectCf(
                    ENEMY_EFFECT_SEQUENCE_B_FIRST, 0, 0);
                work->effect = sefCreateEffectCf(
                    ENEMY_EFFECT_SEQUENCE_B_SECOND, 0, 0);
                if (work->effect != 0) {
                    effect = work->effect;
                    effect->owner = actor;
                    effect->flags &= ~ENEMY_EFFECT_DETACHED_FLAG;
                }
                break;
            }

            case ENEMY_ACTION_EFFECT_SEQUENCE_C: {
                EnemyActionEffect *prior_effect;

                Set_Motion(actor, 28, ENEMY_MOTION_BLEND);
                work->effect_state |= 4;
                work->action_timer_c = ENEMY_ACTION_TIMER_STEP;
                work->motion_id = 30;
                actor->flags &= ~ENEMY_ACTOR_EFFECT_C_FLAG;
                prior_effect = work->effect;
                if (prior_effect != 0) {
                    sefDeleteEffectCf(prior_effect);
                }

                work->effect = sefCreateEffectCf(
                    ENEMY_EFFECT_SEQUENCE_C_FIRST, 0, 0);
                work->effect = sefCreateEffectCf(
                    ENEMY_EFFECT_SEQUENCE_C_FINAL, 0, 0);
                if (work->effect != 0) {
                    effect = work->effect;
                    effect->owner = actor;
                    effect->flags &= ~ENEMY_EFFECT_DETACHED_FLAG;
                }
                break;
            }

            case ENEMY_ACTION_STOP_MOVEMENT:
                work->saved_actor_speed = actor->speed;
                actor->speed = 0.0f;
                break;

            }
        }
    }
}

const float D_004D8150 = 6.2831855f;
