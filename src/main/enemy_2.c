#include "common.h"

#include "enemy_2.h"

/* Enemy command queue: count at +0x120, types at +0x140 and five
 * sixteen-word parameter rows at +0x180. */
enum { ENEMY_COMMAND_QUEUE_SIZE = 16 };
typedef struct EnemyCommandActor {
    u32 flags;
    void (*update)(struct Actor *actor);
    void (*draw)(struct Actor *actor);
    u32 quadword_alignment_gap;
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    Vector4 global_position;
    u8 number;
    u8 unmodeled_81[0xec - 0x81];
    int command_code;
    unsigned char unmodeled_f0[0x120 - 0xf0];
    int enemy_command_count;             /* +0x120 */
    unsigned char unmodeled_124[0x140 - 0x124];
    unsigned int enemy_command_type[16]; /* +0x140 */
    int enemy_command_parameter[5][16];  /* +0x180 */
    unsigned char unmodeled_2c0[0x6f4 - 0x2c0];
    float motion_frame;               /* +0x6F4 */
    float motion_speed;               /* +0x6F8 */
    float motion_start;               /* +0x6FC */
    float motion_end;                 /* +0x700 */
    unsigned short motion_number;     /* +0x704 */
    short motion_progress;            /* +0x706 */
    unsigned char unmodeled_708[0x712 - 0x708];
    short motion_state;               /* +0x712 */
    unsigned char unmodeled_714[0x9ee - 0x714];
    short look_at_target;             /* +0x9EE */
    unsigned char unmodeled_9f0[0xa70 - 0x9f0];
} EnemyCommandActor;

#define D_004D8140 0.03333333507f

#define D_004D813C 0.03333333507f

#define sac_turn_pi 3.141592741f

#define sac_turn_two_pi_subtract 6.283185482f

#define sac_turn_two_pi_add 6.283185482f

int Get_ActorNumber(int target)
{
    unsigned char *base;
    int *id;
    short i;

    base = (unsigned char *)enepc;
    id = (int *)(base + ENEMY_ID_OFFSET);
    for (i = 0; i < 64; i++) {
        if (*id == target)
            return i;
        id = (int *)((unsigned char *)id + sizeof(EnemyWork));
    }
    return -1;
}

void Enemy_Command_Motion(Actor *actor, int motion, signed char mode,
                          int motion_start, int motion_end,
                          int update_flags, int speed_percent)
{
    int flags;
    int selected_motion;
    int loop_mode;

    loop_mode = 1;
    selected_motion = motion | 0x8000;
    flags = (mode == loop_mode) ? 8 : 0;
    if (update_flags == 0)
        flags |= 1;
    ACT_setMotion2(actor, selected_motion, flags);
    if (mode != loop_mode) {
        actor->motion_progress = 0;
        actor->motion_state = 9;
    }
    actor->motion_speed = ((float)speed_percent / 100.0f) * D_004D813C;
    if (motion_start != -1)
        actor->motion_start = (float)motion_start * D_004D813C;
    if (motion_end != -1)
        actor->motion_end = (float)motion_end * D_004D813C;
    if (motion_start != -1)
        actor->motion_frame = actor->motion_start;
}

void Enemy_Command_Freeze(Actor *actor, int command)
{
    unsigned char *work;

    work = (unsigned char *)(enepc + actor->number);

    if (ENEMY_FREEZE_STATE(work) == command)
        return;
    if ((unsigned char)(ENEMY_FREEZE_STATE(work) - 1) < 3 &&
        (unsigned int)(command - 1) < 3)
        return;

    ENEMY_FREEZE_STATE(work) = command;
    switch (command) {
    case 0:
        if (ENEMY_FREEZE_SAVED_SPEED(work) == ENEMY_FREEZE_NO_SAVED_SPEED)
            return;
        ACTOR_SPEED(actor) = ENEMY_FREEZE_SAVED_SPEED(work);
        return;
    case 1:
        ENEMY_FREEZE_SAVED_SPEED(work) = ENEMY_FREEZE_NO_SAVED_SPEED;
        return;
    case 2:
        ENEMY_FREEZE_SAVED_SPEED(work) = ACTOR_SPEED(actor);
        ACTOR_SPEED(actor) = 0.0f;
        return;
    case 3:
        ENEMY_FREEZE_SAVED_SPEED(work) = ACTOR_SPEED(actor);
        ACTOR_SPEED(actor) = D_004D8140;
        return;
    }
}

void Enemy_Command_Turn(Actor *actor, signed char command)
{
    unsigned char *work;

    work = (unsigned char *)(enepc + actor->number);

    switch (command) {
    case 0:
        ENEMY_TURN_LOCK(work) = 1;
        break;
    case 1:
        ENEMY_TURN_LOCK(work) = 0;
        break;
    case 2:
    {
        int current_mode;

        current_mode = ENEMY_TURN_LOCK(work);
        ENEMY_TURN_LOCK(work) = (current_mode ^ 1) != 0;
        break;
    }
    }
}

void Enemy_Command_Light(Actor *actor, signed char light)
{
    unsigned char *work;

    work = (unsigned char *)(enepc + actor->number);

    switch (light) {
    case 0:
        ENEMY_LIGHT(work) = 1;
        break;
    case 1:
        ENEMY_LIGHT(work) = 0;
        break;
    case 2:
    {
        short current_light;

        current_light = ENEMY_LIGHT(work);
        ENEMY_LIGHT(work) = (current_light ^ 1) != 0;
        break;
    }
    }
}

void Enemy_Command_Stop_FreeFall(Actor *actor, signed char command)
{
    switch (command) {
    case 0:
        ACTOR_RUNTIME_FLAGS(actor) |= 0xc0000000u;
        break;
    case 1:
        ACTOR_RUNTIME_FLAGS(actor) &= 0x3fffffffu;
        break;
    }
}

/* A command changes the state selector before notifying the corresponding
 * enemy state machine. Capture the actor once for both operations. */
#define ENEMY_SELECT_TYPE(actor_value, type_value) do { \
    Actor *command_actor = (actor_value); \
    unsigned char *type_work = (unsigned char *)(enepc + command_actor->number); \
    ENEMY_TYPE(type_work) = (type_value); \
    Enemy_Pause(command_actor); \
} while (0)

#define ENEMY_SELECT_CODE(actor_value, code_value) do { \
    Actor *command_actor = (actor_value); \
    command_actor->command_code = (code_value); \
    Enemy_Init(command_actor); \
} while (0)

void Enemy_Command_Type(Actor *actor, int type)
{
    ENEMY_SELECT_TYPE(actor, type);
}

void Enemy_Command_Code(Actor *actor, int code)
{
    ENEMY_SELECT_CODE(actor, code);
}

void Enemy_Command_Target(Actor *actor, int target)
{
    EnemyWork *work;

    work = &enepc[actor->number];

    if (target == 100) {
        ENEMY_TARGET(work) = ((Actor *)GameLoopState[1])->number;
        return;
    }
    ENEMY_TARGET(work) = Get_ActorNumber(target);
}

int Enemy_Command_LookAt(Actor *enemy_actor, int target)
{
    int target_number;

    switch (target) {
    case -1:
        target_number = Actor_LookAt_Release(enemy_actor, 2);
        break;
    case 100:
        Actor_LookAt_Set(enemy_actor, 2,
                         &((Actor *)GameLoopState[1])->position);
        target_number = ((Actor *)GameLoopState[1])->number;
        enemy_actor->look_at_target = target_number;
        break;
    default:
        target_number = Get_ActorNumber(target);
        Actor_LookAt_Set(enemy_actor, 2, &actor[target_number].position);
        target_number = Get_ActorNumber(target);
        enemy_actor->look_at_target = target_number;
        break;
    }
    return target_number;
}

void Enemy_Command_Scale(Actor *actor, int scale_percent, int duration)
{
    unsigned char *work;

    work = (unsigned char *)(enepc + ENEMY_ACTOR_NUMBER(actor));
    do {
        if (duration == 0) {
            float scale;
            unsigned short motion;

            scale = (float)scale_percent / 100.0f;
            ENEMY_SCALE(work)->frame = -1;
            motion = ACTOR_MOTION_NUMBER(actor);
            ENEMY_SCALE(work)->current = scale;
            ENEMY_SCALE(work)->target = scale;
            ACT_setMotion(actor, motion);
            break;
        }
        ENEMY_SCALE(work)->duration = duration;
        ENEMY_SCALE(work)->frame = 0;
        ENEMY_SCALE(work)->start = ENEMY_SCALE(work)->current;
        ENEMY_SCALE(work)->target = (float)scale_percent / 100.0f;
    } while (0);
}

void Enemy_Command_Action(Actor *actor, int action_id, int value,
                          int argument)
{
    unsigned char *work;
    ActorAction *ext;
    short *slot;
    short *argument_slot;
    short i;

    ext = (ActorAction *)((unsigned char *)actor + ACTOR_EXT_OFFSET);
    work = (unsigned char *)(enepc + ENEMY_ACTOR_NUMBER(actor));
    slot = (short *)(work + ENEMY_ACTION_VALUE_OFFSET +
                     action_id * ENEMY_ACTION_STRIDE);

    for (i = 0; i < ENEMY_ACTION_SLOT_COUNT; i++) {
        if (*slot == -1) {
            *slot = value;
            argument_slot = (short *)((unsigned char *)slot + ENEMY_ACTION_ARGUMENT_DELTA);
            *argument_slot = argument;
            ext->frame = 0;
            ext->duration = 0;
            return;
        }
        slot++;
    }
}

void Enemy_Command_Encount(Actor *actor, int command)
{
    Actor *self;
    int encount_command;

    self = actor;
    encount_command = command;
    do {
        Check_Encount(self, 1, encount_command);
    } while (0);
}

void Enemy_Command_Sac_Move(Actor *actor, int duration, float target_x,
                            float target_z)
{
    EnemyWork *work;

    work = &enepc[actor->number];
    if (duration == 0) {
        actor->position.x = target_x;
        actor->position.z = target_z;
        ENEMY_SAC_MOVE(work)->frame = -1;
    } else {
        ENEMY_SAC_MOVE(work)->start.x = actor->position.x;
        ENEMY_SAC_MOVE(work)->duration = duration;
        ENEMY_SAC_MOVE(work)->frame = 0;
        ENEMY_SAC_MOVE(work)->start.y = actor->position.y;
        ENEMY_TURN_FLAGS(work) |= ENEMY_TURN_REQUEST;
        ENEMY_SAC_MOVE(work)->start.z = actor->position.z;
        ENEMY_SAC_MOVE(work)->start.w = actor->position.w;
        ENEMY_SAC_MOVE(work)->target.x = target_x;
        ENEMY_SAC_MOVE(work)->target.y = actor->position.y;
        ENEMY_SAC_MOVE(work)->target.z = target_z;
        ENEMY_SAC_MOVE(work)->target.w = actor->position.w;
    }
}

void Enemy_Command_Sac_Turn(Actor *actor, int duration,
                            float angle_degrees, float angle_mode,
                            short axis)
{
    unsigned char *work;

    work = (unsigned char *)(enepc + actor->number);

    /* `axis` selects one lane of Actor.rotation at run time, so the lane is
     * indexed from the first one instead of named. That is what the original
     * computes: it forms actor + axis * 4 once (sll v1,a2,16 / sra v1,a2,16 /
     * sll v1,a2,2 at 0x002d3408..0x002d342c) and addresses the rotation lane
     * at +0x50 of it; the instantaneous branch reuses the untouched actor for
     * the +0x9e4 target angle (0x002d34a4). A `float *rotation =
     * &actor->rotation.x;` local instead was compiled (form 01,
     * .work/attempts/style-enemy-actor-family/form01-enemy_2.c): it makes
     * ee-gcc 2.96 materialise actor + 0x50 in its own register, grows the
     * function by 8 bytes and the main link refuses it with "cannot move
     * location counter backwards (from 002d3830 to 002d3828)". */

    angle_degrees = angle_degrees / 180.0f;
    angle_degrees = angle_degrees * sac_turn_pi;

    if (angle_mode <= 0.0f) {
        if ((&actor->rotation.x)[axis] < angle_degrees) {
            angle_degrees = angle_degrees - sac_turn_two_pi_subtract;
        }
    } else {
        if (angle_degrees < (&actor->rotation.x)[axis]) {
            angle_degrees = angle_degrees + sac_turn_two_pi_add;
        }
    }

    if (duration == 0) {
        (&actor->rotation.x)[axis] = angle_degrees;
        ENEMY_TURN(work)->frame = -1;
        ACTOR_TARGET_ANGLE(actor) = angle_degrees;
    } else {
        ENEMY_TURN(work)->duration = duration;
        ENEMY_TURN(work)->frame = 0;
        ENEMY_TURN(work)->start_angle = (&actor->rotation.x)[axis];
        ENEMY_TURN(work)->target_angle = angle_degrees;
        ENEMY_TURN_FLAGS(work) |= ENEMY_TURN_REQUEST;
    }
}

void Map_Command_Ladder(int state_index, int ladder_type, int scale_percent,
                        int ladder_mode)
{
    GameLoopMapState *state;

    state = (GameLoopMapState *)GameLoopState;
    state->ladder_type[state_index - 1] = ladder_type;
    state->ladder_scale[state_index - 1] = (float)scale_percent / 100.0f;
    state->ladder_mode[state_index - 1] = ladder_mode;
}

/* The command queue Start_Enemy_Command drains, viewed from the actor's
 * extension block at ACTOR_EXT_OFFSET (the original keeps actor + 0xc0 in a
 * register and loads lw 96/128/192.. from it). */
typedef struct EnemyCommandQueue {
    unsigned char unmodeled_00[0x60];
    int count;                                       /* actor +0x120 */
    unsigned char unmodeled_64[0x80 - 0x64];
    unsigned int type[ENEMY_COMMAND_QUEUE_SIZE];     /* actor +0x140 */
    int parameter[5][ENEMY_COMMAND_QUEUE_SIZE];      /* actor +0x180 */
} EnemyCommandQueue;

void Start_Enemy_Command(Actor *actor)
{
    EnemyCommandQueue *queue;
    unsigned int type;
    int first;
    int second;
    int third;
    int fourth;
    int fifth;
    short i;

    queue = (EnemyCommandQueue *)((unsigned char *)actor + ACTOR_EXT_OFFSET);
    for (i = 0; i < queue->count && i < ENEMY_COMMAND_QUEUE_SIZE; i++) {
        type = queue->type[i];
        first = queue->parameter[0][i];
        second = queue->parameter[1][i];
        third = queue->parameter[2][i];
        fourth = queue->parameter[3][i];
        fifth = queue->parameter[4][i];
        /* A type-15 (Sac_Move) test that leaves the values unchanged: the
         * fifth column is read again through a plain int pointer. The
         * original carries it - the compare constant survives without a
         * user (li v0,15 at 0x002d3594) and the bounds check (sltiu at
         * 0x002d3598) is scheduled after all six column loads, which the
         * statement-macro block reproduces. */
        do {
            if (type == 15)
                fifth = *(queue->parameter[4] + i);
        } while (0);

        switch (type) {
        case 0:
            Enemy_Command_Motion(actor, first, 0, -1, -1, 0, 100);
            break;
        case 1:
            Enemy_Command_Motion(actor, first, 1, -1, -1, 0, 100);
            break;
        case 2:
            Enemy_Command_Motion(actor, first, 0, second, third, fourth, fifth);
            break;
        case 3:
            Enemy_Command_Motion(actor, first, 1, second, third, fourth, fifth);
            break;
        case 4:
            Enemy_Command_Freeze(actor, first);
            break;
        case 5:
            Enemy_Command_Turn(actor, first);
            break;
        case 6:
            Enemy_Command_Type(actor, first);
            break;
        case 7:
            Enemy_Command_Code(actor, first);
            break;
        case 8:
            Enemy_Command_Target(actor, first);
            break;
        case 9:
            Enemy_Command_LookAt(actor, first);
            break;
        case 10:
            Enemy_Command_Scale(actor, first, second);
            break;
        case 11:
            Enemy_Command_Action(actor, first, second, third);
            break;
        case 12:
            Enemy_Command_Light(actor, first);
            break;
        case 13:
            Enemy_Command_Stop_FreeFall(actor, first);
            break;
        case 14:
            Enemy_Command_Encount(actor, first);
            break;
        case 15:
            Enemy_Command_Sac_Move(actor, third,
                                   *(float *)&queue->parameter[0][i],
                                   *(float *)&queue->parameter[1][i]);
            break;
        case 16:
            Enemy_Command_Sac_Turn(actor, third,
                                   *(float *)&queue->parameter[0][i],
                                   *(float *)&queue->parameter[1][i], 0);
            break;
        case 17:
            Enemy_Command_Sac_Turn(actor, third,
                                   *(float *)&queue->parameter[0][i],
                                   *(float *)&queue->parameter[1][i], 1);
            break;
        case 18:
            Enemy_Command_Sac_Turn(actor, third,
                                   *(float *)&queue->parameter[0][i],
                                   *(float *)&queue->parameter[1][i], 2);
            break;
        case 19:
            Map_Command_Ladder(first, second, third, fourth);
            break;
        }
    }
    queue->count = 0;
}


