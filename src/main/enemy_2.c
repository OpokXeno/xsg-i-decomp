#include "common.h"
#include "enemy_2.h"
#include "main/enemy_2.h"

#define ENEMY_FREEZE_SPEED 0.03333333507f
#define FRAME_TIME_30FPS 0.03333333507f
#define sac_turn_pi 3.141592741f
#define sac_turn_two_pi_subtract 6.283185482f
#define sac_turn_two_pi_add 6.283185482f

/* A command changes the state selector before notifying the corresponding
 * enemy state machine. Capture the actor once for both operations. */
#define ENEMY_SELECT_TYPE(actor_value, type_value) do { \
    Actor *command_actor = (actor_value); \
    EnemyWork *type_work = enepc + command_actor->number; \
    ENEMY_TYPE(type_work) = (type_value); \
    Enemy_Pause(command_actor); \
} while (0)

#define ENEMY_SELECT_CODE(actor_value, code_value) do { \
    Actor *command_actor = (actor_value); \
    command_actor->args.command_code = (code_value); \
    Enemy_Init(command_actor); \
} while (0)

/* Enemy command queue in the actor's Java argument area (ActorJavaArgs):
 * count at +0x120, types at +0x140 and five sixteen-word parameter rows at
 * +0x180. */
enum { ENEMY_COMMAND_QUEUE_SIZE = 16 };

/* Command types Start_Enemy_Command dispatches. */
enum {
    ENEMY_COMMAND_MOTION = 0,
    ENEMY_COMMAND_MOTION_LOOP = 1,
    ENEMY_COMMAND_MOTION_RANGE = 2,
    ENEMY_COMMAND_MOTION_LOOP_RANGE = 3,
    ENEMY_COMMAND_FREEZE = 4,
    ENEMY_COMMAND_TURN = 5,
    ENEMY_COMMAND_TYPE = 6,
    ENEMY_COMMAND_CODE = 7,
    ENEMY_COMMAND_TARGET = 8,
    ENEMY_COMMAND_LOOK_AT = 9,
    ENEMY_COMMAND_SCALE = 10,
    ENEMY_COMMAND_ACTION = 11,
    ENEMY_COMMAND_LIGHT = 12,
    ENEMY_COMMAND_STOP_FREE_FALL = 13,
    ENEMY_COMMAND_ENCOUNT = 14,
    ENEMY_COMMAND_SAC_MOVE = 15,
    ENEMY_COMMAND_SAC_TURN_X = 16,
    ENEMY_COMMAND_SAC_TURN_Y = 17,
    ENEMY_COMMAND_SAC_TURN_Z = 18,
    ENEMY_COMMAND_MAP_LADDER = 19
};

/* Argument of the turn-lock and light commands. */
enum {
    ENEMY_SWITCH_SET = 0,
    ENEMY_SWITCH_CLEAR = 1,
    ENEMY_SWITCH_TOGGLE = 2
};

/* animation.slot.flags bits Enemy_Command_Stop_FreeFall sets / clears. */
#define ANIM_SLOT_FLAG_STOP_FREE_FALL 0xc0000000u


/* Returns the index of the enemy entry whose entry_id is target, or -1 when none matches. */
int Get_ActorNumber(int target)
{
    EnemyWork *base;
    unsigned int *id;
    short i;

    base = enepc;
    id = &base->entry_id;
    for (i = 0; i < ACTOR_COUNT; i++) {
        if (*id == target)
            return i;
        id += sizeof(EnemyWork) / sizeof(*id);
    }
    return -1;
}

/*
 * Starts motion on actor, looping when mode is 1. speed_percent sets the playback
 * speed. motion_start and motion_end set the frame limits, where -1 keeps the current limit.
 */
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
        actor->animation.slot.nextDataId = 0;
        actor->animation.slot.animQueuedFlags = 9;
    }
    actor->animation.slot.speed = ((float)speed_percent / 100.0f) * FRAME_TIME_30FPS;
    if (motion_start != -1)
        actor->animation.slot.reverseLimit = (float)motion_start * FRAME_TIME_30FPS;
    if (motion_end != -1)
        actor->animation.slot.forwardLimit = (float)motion_end * FRAME_TIME_30FPS;
    if (motion_start != -1)
        actor->animation.slot.frame = actor->animation.slot.reverseLimit;
}

/*
 * Sets the freeze state of the enemy. Release restores the saved speed, stop
 * saves it and sets speed to 0, and slow saves it and sets the slow speed.
 */
void Enemy_Command_Freeze(Actor *actor, int command)
{
    EnemyWork *work;

    work = enepc + actor->number;

    if (ENEMY_FREEZE_STATE(work) == command)
        return;
    if ((unsigned char)(ENEMY_FREEZE_STATE(work) - 1) < 3 &&
        (unsigned int)(command - 1) < 3)
        return;

    ENEMY_FREEZE_STATE(work) = command;
    switch (command) {
    case ENEMY_FREEZE_RELEASE:
        if (ENEMY_FREEZE_SAVED_SPEED(work) == ENEMY_FREEZE_NO_SAVED_SPEED)
            return;
        actor->animation.slot.speed = ENEMY_FREEZE_SAVED_SPEED(work);
        return;
    case ENEMY_FREEZE_SCRIPTED:
        ENEMY_FREEZE_SAVED_SPEED(work) = ENEMY_FREEZE_NO_SAVED_SPEED;
        return;
    case ENEMY_FREEZE_STOP:
        ENEMY_FREEZE_SAVED_SPEED(work) = actor->animation.slot.speed;
        actor->animation.slot.speed = 0.0f;
        return;
    case ENEMY_FREEZE_SLOW:
        ENEMY_FREEZE_SAVED_SPEED(work) = actor->animation.slot.speed;
        actor->animation.slot.speed = ENEMY_FREEZE_SPEED;
        return;
    }
}

/* Sets, clears or toggles the turn lock of the enemy. */
void Enemy_Command_Turn(Actor *actor, signed char command)
{
    EnemyWork *work;

    work = enepc + actor->number;

    switch (command) {
    case ENEMY_SWITCH_SET:
        ENEMY_TURN_LOCK(work) = 1;
        break;
    case ENEMY_SWITCH_CLEAR:
        ENEMY_TURN_LOCK(work) = 0;
        break;
    case ENEMY_SWITCH_TOGGLE:
    {
        int current_mode;

        current_mode = ENEMY_TURN_LOCK(work);
        ENEMY_TURN_LOCK(work) = (current_mode ^ 1) != 0;
        break;
    }
    }
}

/* Sets, clears or toggles the light flag of the enemy. */
void Enemy_Command_Light(Actor *actor, signed char light)
{
    EnemyWork *work;

    work = enepc + actor->number;

    switch (light) {
    case ENEMY_SWITCH_SET:
        ENEMY_LIGHT(work) = 1;
        break;
    case ENEMY_SWITCH_CLEAR:
        ENEMY_LIGHT(work) = 0;
        break;
    case ENEMY_SWITCH_TOGGLE:
    {
        short current_light;

        current_light = ENEMY_LIGHT(work);
        ENEMY_LIGHT(work) = (current_light ^ 1) != 0;
        break;
    }
    }
}

/* Sets or clears the free-fall stop bits in the actor's animation slot flags. */
void Enemy_Command_Stop_FreeFall(Actor *actor, signed char command)
{
    switch (command) {
    case ENEMY_SWITCH_SET:
        actor->animation.slot.flags |= ANIM_SLOT_FLAG_STOP_FREE_FALL;
        break;
    case ENEMY_SWITCH_CLEAR:
        actor->animation.slot.flags &= ~ANIM_SLOT_FLAG_STOP_FREE_FALL;
        break;
    }
}

void Enemy_Command_Type(Actor *actor, int type)
{
    ENEMY_SELECT_TYPE(actor, type);
}

void Enemy_Command_Code(Actor *actor, int code)
{
    ENEMY_SELECT_CODE(actor, code);
}

/* Sets the enemy's target to the player for ENEMY_TARGET_PLAYER, otherwise to the entry with id target. */
void Enemy_Command_Target(Actor *actor, int target)
{
    EnemyWork *work;

    work = &enepc[actor->number];

    if (target == ENEMY_TARGET_PLAYER) {
        ENEMY_TARGET(work) = ((Actor *)GameLoopState[1])->number;
        return;
    }
    ENEMY_TARGET(work) = Get_ActorNumber(target);
}

/*
 * Points the enemy's look-at at the player, at the actor with entry id target,
 * or releases it for ENEMY_LOOK_AT_RELEASE. Returns the target's actor number.
 */
int Enemy_Command_LookAt(Actor *enemy_actor, int target)
{
    int target_number;

    switch (target) {
    case ENEMY_LOOK_AT_RELEASE:
        target_number = Actor_LookAt_Release(enemy_actor, 2);
        break;
    case ENEMY_TARGET_PLAYER:
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

/*
 * Scales the enemy to scale_percent. A duration of 0 applies the scale at once
 * and resets the motion; otherwise the scale is eased over duration frames.
 */
void Enemy_Command_Scale(Actor *actor, int scale_percent, int duration)
{
    EnemyWork *work;

    work = enepc + actor->number;
    do {
        if (duration == 0) {
            float scale;
            unsigned short motion;

            scale = (float)scale_percent / 100.0f;
            ENEMY_SCALE(work)->frame = -1;
            motion = actor->animation.slot.currentDataId;
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

/*
 * Stores value and argument in the first free slot of the enemy's action table
 * for action_id, and resets the action frame and duration.
 */
void Enemy_Command_Action(Actor *actor, int action_id, int value,
                          int argument)
{
    EnemyWork *work;
    ActorJavaArgs *args;
    short *slot;
    short *argument_slot;
    short i;

    args = &actor->args;
    work = enepc + actor->number;
    slot = work->motion_tables[0][action_id];

    for (i = 0; i < ENEMY_ACTION_SLOT_COUNT; i++) {
        if (*slot == -1) {
            *slot = value;
            argument_slot = slot + ENEMY_ACTION_ARGUMENT_DELTA / sizeof(*slot);
            *argument_slot = argument;
            args->action_frame = 0;
            args->action_duration = 0;
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

/*
 * Moves the actor to (target_x, target_z) at once when duration is 0. Otherwise
 * it starts a timed move over duration frames and sets the turn-request flag.
 */
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

/*
 * Turns the rotation lane axis (0 x, 1 y, 2 z) of the actor toward angle_degrees,
 * converted to radians. angle_mode at or below 0 picks the wrap direction for
 * the turn. A duration of 0 sets the angle at once; otherwise it starts a timed turn.
 */
void Enemy_Command_Sac_Turn(Actor *actor, int duration,
                            float angle_degrees, float angle_mode,
                            short axis)
{
    EnemyWork *work;

    work = enepc + actor->number;

    /* `axis` selects one lane of Actor.rotation at run time, so the lane is
     * indexed from the first one instead of named. A named float * local for
     * the lane would change the generated code. */

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
        actor->target_angle = angle_degrees;
    } else {
        ENEMY_TURN(work)->duration = duration;
        ENEMY_TURN(work)->frame = 0;
        ENEMY_TURN(work)->start_angle = (&actor->rotation.x)[axis];
        ENEMY_TURN(work)->target_angle = angle_degrees;
        ENEMY_TURN_FLAGS(work) |= ENEMY_TURN_REQUEST;
    }
}

/*
 * Stores the type, scale (percent) and mode of the ladder at state_index, which
 * is 1-based in the map state's ladder arrays.
 */
void Map_Command_Ladder(int state_index, int ladder_type, int scale_percent,
                        int ladder_mode)
{
    GameLoopMapState *state;

    state = (GameLoopMapState *)GameLoopState;
    state->ladder_type[state_index - 1] = ladder_type;
    state->ladder_scale[state_index - 1] = (float)scale_percent / 100.0f;
    state->ladder_mode[state_index - 1] = ladder_mode;
}

/* Runs the actor's queued enemy commands in order, then empties the queue. */
void Start_Enemy_Command(Actor *actor)
{
    ActorJavaArgs *queue;
    unsigned int type;
    int first;
    int second;
    int third;
    int fourth;
    int fifth;
    short i;

    queue = &actor->args;
    /* Each entry loads its five parameter columns, then the switch dispatches on its type. */
    for (i = 0; i < queue->command_count && i < ENEMY_COMMAND_QUEUE_SIZE; i++) {
        type = queue->command_type[i];
        first = queue->command_parameter[0][i];
        second = queue->command_parameter[1][i];
        third = queue->command_parameter[2][i];
        fourth = queue->command_parameter[3][i];
        fifth = queue->command_parameter[4][i];
        /* For Sac_Move entries the fifth column is read again through a plain
         * int pointer. This keeps the original's load order: the bounds check
         * follows all six column loads. */
        do {
            if (type == ENEMY_COMMAND_SAC_MOVE)
                fifth = *(queue->command_parameter[4] + i);
        } while (0);

        /* Each type calls its Enemy_Command_* or Map_Command_* function. */
        switch (type) {
        case ENEMY_COMMAND_MOTION:
            Enemy_Command_Motion(actor, first, 0, -1, -1, 0, 100);
            break;
        case ENEMY_COMMAND_MOTION_LOOP:
            Enemy_Command_Motion(actor, first, 1, -1, -1, 0, 100);
            break;
        case ENEMY_COMMAND_MOTION_RANGE:
            Enemy_Command_Motion(actor, first, 0, second, third, fourth, fifth);
            break;
        case ENEMY_COMMAND_MOTION_LOOP_RANGE:
            Enemy_Command_Motion(actor, first, 1, second, third, fourth, fifth);
            break;
        case ENEMY_COMMAND_FREEZE:
            Enemy_Command_Freeze(actor, first);
            break;
        case ENEMY_COMMAND_TURN:
            Enemy_Command_Turn(actor, first);
            break;
        case ENEMY_COMMAND_TYPE:
            Enemy_Command_Type(actor, first);
            break;
        case ENEMY_COMMAND_CODE:
            Enemy_Command_Code(actor, first);
            break;
        case ENEMY_COMMAND_TARGET:
            Enemy_Command_Target(actor, first);
            break;
        case ENEMY_COMMAND_LOOK_AT:
            Enemy_Command_LookAt(actor, first);
            break;
        case ENEMY_COMMAND_SCALE:
            Enemy_Command_Scale(actor, first, second);
            break;
        case ENEMY_COMMAND_ACTION:
            Enemy_Command_Action(actor, first, second, third);
            break;
        case ENEMY_COMMAND_LIGHT:
            Enemy_Command_Light(actor, first);
            break;
        case ENEMY_COMMAND_STOP_FREE_FALL:
            Enemy_Command_Stop_FreeFall(actor, first);
            break;
        case ENEMY_COMMAND_ENCOUNT:
            Enemy_Command_Encount(actor, first);
            break;
        case ENEMY_COMMAND_SAC_MOVE:
            Enemy_Command_Sac_Move(actor, third,
                                   *(float *)&queue->command_parameter[0][i],
                                   *(float *)&queue->command_parameter[1][i]);
            break;
        case ENEMY_COMMAND_SAC_TURN_X:
            Enemy_Command_Sac_Turn(actor, third,
                                   *(float *)&queue->command_parameter[0][i],
                                   *(float *)&queue->command_parameter[1][i], 0);
            break;
        case ENEMY_COMMAND_SAC_TURN_Y:
            Enemy_Command_Sac_Turn(actor, third,
                                   *(float *)&queue->command_parameter[0][i],
                                   *(float *)&queue->command_parameter[1][i], 1);
            break;
        case ENEMY_COMMAND_SAC_TURN_Z:
            Enemy_Command_Sac_Turn(actor, third,
                                   *(float *)&queue->command_parameter[0][i],
                                   *(float *)&queue->command_parameter[1][i], 2);
            break;
        case ENEMY_COMMAND_MAP_LADDER:
            Map_Command_Ladder(first, second, third, fourth);
            break;
        }
    }
    queue->command_count = 0;
}
