#include "common.h"

#include "enemy_2.h"

#define D_004D8140 0.03333333507f
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

INCLUDE_ASM("asm/main/nonmatchings/enemy_2", Enemy_Command_Motion);

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

void Enemy_Command_Type(Actor *actor, signed char type)
{
    unsigned char *work;

    work = (unsigned char *)(enepc + actor->number);
    ENEMY_TYPE(work) = type;
    /* Statement-macro shape: written as a bare final call, ee-gcc 2.96 turns
     * it into a sibling jump; the original keeps jal plus the epilogue. */
    do {
        Enemy_Pause(actor);
    } while (0);
}

void Enemy_Command_Code(Actor *actor, int code)
{
    actor->command_code = code;
    /* Same statement-macro shape as Enemy_Command_Type above. */
    do {
        Enemy_Init(actor);
    } while (0);
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

INCLUDE_ASM("asm/main/nonmatchings/enemy_2", Enemy_Command_LookAt);

void Enemy_Command_Scale(Actor *actor, int scale_percent, int duration)
{
    unsigned char *work;

    work = (unsigned char *)(enepc + ENEMY_ACTOR_NUMBER(actor));

    if (duration == 0) {
        float scale;
        unsigned short motion;

        scale = (float)scale_percent / 100.0f;
        ENEMY_SCALE(work)->frame = -1;
        motion = ACTOR_MOTION_NUMBER(actor);
        ENEMY_SCALE(work)->current = scale;
        ENEMY_SCALE(work)->target = scale;
        do {
            ACT_setMotion(actor, motion);
        } while (0);
    } else {
        ENEMY_SCALE(work)->duration = duration;
        ENEMY_SCALE(work)->frame = 0;
        ENEMY_SCALE(work)->start = ENEMY_SCALE(work)->current;
        ENEMY_SCALE(work)->target = (float)scale_percent / 100.0f;
    }
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

void Enemy_Command_Encount(Actor *actor, signed char command)
{
    Actor *self;
    int encount_command;

    self = actor;
    encount_command = command;
    do {
        Check_Encount(self, 1, encount_command);
    } while (0);
}

INCLUDE_ASM("asm/main/nonmatchings/enemy_2", Enemy_Command_Sac_Move);

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

INCLUDE_ASM("asm/main/nonmatchings/enemy_2", Map_Command_Ladder);

INCLUDE_ASM("asm/main/nonmatchings/enemy_2", Start_Enemy_Command);
#include "common.h"
