#include "common.h"
#include "get_ladder.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "main/get_ladder.h"
#include "main/act_1.h"
#include "main/act_2.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

#define FLY_TO_LADDER_SPEED (0.033333335071802139f)
#define FLY_TO_LADDER_RISE_STEP (0.69999998807907104f)
#define GET_OFF_LADDER_SPEED (0.033333335071802139f)

#define ARRIVAL_COUNT 4       /* ladder arrival attribute ids */
#define LADDER_DIRECTION_COUNT 4

/* LadderStep[]: ground height of each rung around the current one. */
#define LADDER_STEP_COUNT 127
#define LADDER_STEP_MIDDLE 63 /* rung the actor starts on */

#define LADDER_ID_SEARCH_MAX 1024 /* map probes along a ladder */
#define ARRIVAL_SEARCH_MAX 256
#define LADDER_FLY_FRAMES 13      /* frames to fly on or off the ladder */
#define LADDER_STEP_FRAMES 15     /* frames of one rung step */
#define LADDER_STEP_FRAMES_F 15.0f
#define LADDER_STICK_DEADZONE 32.0f
#define LADDER_STEP_WAIT_FRAMES 20
#define LADDER_CLIMB_SE 0x20011   /* sound while climbing */

/* LadderWork.state */
enum LadderState {
    LADDER_STATE_IDLE = 0,
    LADDER_STATE_APPROACH = 1,  /* waiting before turning to the ladder */
    LADDER_STATE_FLY_ON = 3,    /* flying onto the ladder */
    LADDER_STATE_HANG = 4,      /* on the ladder, not moving */
    LADDER_STATE_STEP_UP = 5,
    LADDER_STATE_STEP_DOWN = 6,
    LADDER_STATE_GET_OFF = 7,
    LADDER_STATE_AT_EXIT = 8
};

struct LadderAttributeTable {
    int attributes[LADDER_COUNT];
};

struct ArrivalAttributeTable {
    int attributes[ARRIVAL_COUNT];
};

typedef struct LadderWork {
    unsigned char unmodeled_0000[0x3800];
    AlignedVector4 from;
    AlignedVector4 to;
    short fly_frame;
    short fly_frames;
    unsigned char unmodeled_3824[0x3830 - 0x3824];
    short state;
    float limit_low;
    float limit_high;
    float step_from;
    float step_to;
    float ceiling_height;
    short step_frame;
    short step_wait;
    signed char ladder_id;
    signed char from_top;
    unsigned char unmodeled_384e[2];
    float facing_angle;
    unsigned char unmodeled_3854[0x386c - 0x3854];
    float *lookat_point;
    unsigned char unmodeled_3870[0x3890 - 0x3870];
    Vector4 lookat_target;
    unsigned char unmodeled_38a0[0x10];
} LadderWork;

typedef struct LadderPlayer {
    unsigned char unmodeled_00[0x4d0];
    unsigned short flags;
} LadderPlayer;

typedef struct LadderGameState {
    unsigned char unmodeled_00[4];
    LadderPlayer *player;
    unsigned char unmodeled_08[8];
    unsigned int ladder_flags;
    unsigned char unmodeled_14[0xc1 - 0x14];
    unsigned char map_flags;
    unsigned char unmodeled_c2[0x29fa0 - 0xc2];
    signed char ladder_direction[LADDER_COUNT];
    unsigned char ladder_bottom_exit[LADDER_COUNT];
    float ladder_height[LADDER_COUNT];
    unsigned char unmodeled_2a000[0x30];
} LadderGameState;

typedef struct LadderPad {
    unsigned char unmodeled_00[0x28];
    unsigned short buttons;
    unsigned char unmodeled_2a[0x3a];
    signed char axis[4];
} LadderPad;

extern short NowLadderStep;
extern LadderWork enepc[16];
extern LadderGameState GameLoopState;
extern LadderPad PadData[2];
extern const char hashigoBottomName[16];
extern int Get_Attr_NU(const float *position, int mapIndex, int attrMask);

extern void Get_LadderID_Limit(const float *position, float *result,
                               float x_limit, float z_limit);

extern int Get_Attr(const Vector4 *position, int map_index, int attr_mask);

extern void Get_MiddlePoint(const float *start, const float *following,
                            short point_index, short point_count,
                            float *result);

extern void Get_MiddlePoint_Parabora(const Vector4 *start,
                                     const Vector4 *following,
                                     int point_index, int point_count,
                                     float height, Vector4 *result);

extern float Get_Height(const Vector4 *point, int mode, int flags);
extern float Get_Multi_Max_Under(float first, float second, float third);
extern float Get_Cursol_by_Reduce_Speed_Angle_Loop(float current, float target,
                                                   float speed);

extern float *MotTransXYZ(float *xyz);
extern void CallMethod_I(const char *method_name, int argument1);
extern void *memset(void *destination, int value, unsigned int count);

extern void On_Ladder(Actor *actor);

const struct LadderAttributeTable ladderAttributeTable = {{
    0x00100000, 0x00110000, 0x00120000, 0x00130000,
    0x00140000, 0x00150000, 0x00160000, 0x00170000,
    0x00180000, 0x00190000, 0x001A0000, 0x001B0000,
    0x001C0000, 0x001D0000, 0x001E0000, 0x001F0000
}};

const struct ArrivalAttributeTable arrivalAttributeTable = {{
    0x00200000, 0x00210000, 0x00220000, 0x00230000
}};

float AdjTable_X[LADDER_DIRECTION_COUNT] = {0.0f, 0.41f, 0.0f, -0.41f};

float AdjTable_Z[LADDER_DIRECTION_COUNT] = {-0.41f, 0.0f, 0.41f, 0.0f};

float LadderStep[LADDER_STEP_COUNT] = {0.0f};

short Get_Ladder(int attribute)
{
    struct LadderAttributeTable table = ladderAttributeTable;
    short index = 0;

    do {
        if (attribute == table.attributes[index]) {
            return index;
        }

        index++;
    } while (index < LADDER_COUNT);

    return -1;
}

short Get_Arrival(int attribute)
{
    struct ArrivalAttributeTable table = arrivalAttributeTable;
    short index = 0;

    do {
        if (attribute == table.attributes[index]) {
            return index;
        }

        index++;
    } while (index < ARRIVAL_COUNT);

    return -1;
}

void Get_LadderID_Limit(const float *start, float *position,
                        float xStep, float zStep)
{
    short index = 0;
    int found = 0;
    int attribute;

    position[0] = start[0];
    position[1] = start[1];
    position[2] = start[2];
    position[3] = start[3];

    for (;;) {
        if (index >= LADDER_ID_SEARCH_MAX) {
            break;
        }
        attribute = Get_Attr_NU(position, 0, 0);
        if (Get_Ladder(attribute) != -1) {
            found = 1;
        }
        if (Get_Ladder(attribute) == -1 && found == 1) {
            break;
        }
        index++;
        position[0] += xStep;
        position[2] += zStep;
    }

    if (index == LADDER_ID_SEARCH_MAX) {
        position[0] = start[0];
        position[2] = start[2];
    } else {
        position[0] -= xStep;
        position[2] -= zStep;
    }
}

int Get_ArrivalID_First(const float *start, float *position,
                        float xStep, float zStep)
{
    short index = 0;
    int attribute;

    position[0] = start[0];
    position[1] = start[1];
    position[2] = start[2];
    position[3] = start[3];

    for (;;) {
        if (index >= ARRIVAL_SEARCH_MAX) {
            break;
        }

        attribute = Get_Attr_NU(position, 0, 0);

        if (Get_Arrival(attribute) != -1) {
            break;
        }

        position[0] += xStep;
        position[2] += zStep;
        index++;
    }

    return index != ARRIVAL_SEARCH_MAX;
}

void Fly_to_Ladder(Actor *actor)
{
    LadderWork *work = &enepc[actor->number];
    AlignedVector4 point;
    float height;
    short i;
    short state = work->state;

    if (state == LADDER_STATE_APPROACH) {
        work->fly_frame++;
        if (work->fly_frame < LADDER_FLY_FRAMES) {
            return;
        }

        work->fly_frame = 0;
        actor->target_angle = work->facing_angle;

        if (work->from_top == 0) {
            int direction = GameLoopState.ladder_direction[work->ladder_id];
            int motion = 21;

            if (direction == 0 &&
                work->to.x < work->from.x) {
                motion = 19;
            } else if (direction == state &&
                       work->from.y < work->to.y) {
                motion = 19;
            } else if (direction == 3 &&
                       work->from.y < work->to.y) {
                motion = 19;
            }
            ACT_setMotion2(actor, motion, 8);
        } else {
            ACT_setMotion2(actor, 28, 8);
        }
        work->state = LADDER_STATE_FLY_ON;
    }

    Get_MiddlePoint_Parabora(&work->from, &work->to,
                             work->fly_frame, work->fly_frames, 0.0f,
                             &actor->position);

    if (work->fly_frame++ < work->fly_frames) {
        return;
    }

    work->fly_frame = -1;
    work->state = LADDER_STATE_HANG;
    work->step_wait = LADDER_STEP_WAIT_FRAMES;
    ACT_setMotion2(actor, 20, 8);
    actor->animation.slot.speed = FLY_TO_LADDER_SPEED;

    point = actor->position;
    point.x -= AdjTable_X[GameLoopState.ladder_direction[work->ladder_id]];
    point.z -= AdjTable_Z[GameLoopState.ladder_direction[work->ladder_id]];
    height = Get_Height(&point, 0, 0);

    NowLadderStep = LADDER_STEP_MIDDLE;
    if (actor->position.y < height || work->from_top == 1) {
        actor->position.y = height;
    }
    LadderStep[LADDER_STEP_MIDDLE] = actor->position.y;

    for (i = 1; i < LADDER_STEP_MIDDLE; i++) {
        float rise = i * FLY_TO_LADDER_RISE_STEP;
        float base = actor->position.y;

        LadderStep[LADDER_STEP_MIDDLE + i] = base + rise;
        LadderStep[LADDER_STEP_MIDDLE - i] = base - rise;
    }
}

void Get_Off_Ladder(Actor *actor)
{
    LadderWork *work = &enepc[actor->number];

    work->fly_frame++;
    Get_MiddlePoint(&work->from.x, &work->to.x,
                    work->fly_frame, work->fly_frames,
                    &actor->position.x);

    if (work->fly_frame >= work->fly_frames) {
        work->fly_frame = -1;
        work->state = LADDER_STATE_IDLE;
        ACT_setMotion2(actor, 0, 8);
        GameLoopState.player->flags |= 0x1000;
        GameLoopState.map_flags |= 0x20;
    }

    actor->animation.slot.speed = GET_OFF_LADDER_SPEED;
}

short Ready_Ladder(Actor *actor)
{
    AlignedVector4 point;
    AlignedVector4 edge;
    float limit_x[4] = {0.0f, 0.001f, 0.0f, -0.001f};
    float limit_z[4] = {-0.001f, 0.0f, 0.001f, 0.0f};
    float probe_x[4] = {0.0f, 0.79999995f, 0.0f, -0.79999995f};
    float probe_z[4] = {-0.79999995f, 0.0f, 0.79999995f, 0.0f};
    float facing[4] = {3.14159274f, 1.57079637f, 0.0f, -1.57079637f};
    LadderWork *work = &enepc[actor->number];
    int attribute;
    short ladder;
    int direction = -1;
    float lift = 0.0f;
    float height;
    float ceiling;

    Get_Height(&actor->position, 0, 0);
    attribute = Get_Attr(&actor->position, 0, 0);
    ladder = Get_Ladder(attribute);
    if (ladder == -1 && Get_Arrival(attribute) == -1) {
        return 0;
    }

    work->from = actor->position;
    if (ladder != -1) {
        direction = GameLoopState.ladder_direction[ladder];
        work->to = actor->position;
        work->from_top = 1;
        work->fly_frames = LADDER_FLY_FRAMES;
        lift = 0.0f;
    } else {
        actor->target_angle = facing[Get_Arrival(attribute)];
        point = actor->position;
        point.x += probe_x[Get_Arrival(attribute)];
        point.z += probe_z[Get_Arrival(attribute)];
        ladder = Get_Ladder(Get_Attr(&point, 0, 0));
        if (ladder == -1) {
            return 0;
        }
        work->to = point;
        work->fly_frames = LADDER_FLY_FRAMES;
        work->from_top = 0;
        direction = GameLoopState.ladder_direction[ladder];
    }

    work->facing_angle = facing[direction];
    actor->target_angle = work->facing_angle;
    work->state = LADDER_STATE_APPROACH;
    ACT_setMotion2(actor, 0, 9);

    point = work->to;
    point.y += Get_Height(&work->to, 0, 0);
    Get_LadderID_Limit(&point.x, &edge.x,
                       limit_x[direction], limit_z[direction]);
    height = Get_Height(&edge, 0, 0);
    work->to = edge;
    ceiling = Get_Multi_Max_Under(
        height, 0.7f, actor->position.y + lift + 0.35f);
    work->to.y = ceiling;
    work->fly_frame = 0;
    work->ladder_id = ladder;
    work->ceiling_height = ceiling;
    point = edge;

    switch (direction) {
    case 0:
        Get_LadderID_Limit(&point.x, &edge.x, -0.001f,
                           0.0f);
        work->limit_low = edge.x;
        Get_LadderID_Limit(&point.x, &edge.x, 0.001f,
                           0.0f);
        work->limit_high = edge.x;
        work->to.x = work->limit_low + (work->limit_high - work->limit_low) * 0.5f;
        work->to.z -= 0.4f;
        break;
    case 1:
        Get_LadderID_Limit(&point.x, &edge.x, 0.0f,
                           -0.001f);
        work->limit_low = edge.z;
        Get_LadderID_Limit(&point.x, &edge.x, 0.0f,
                           0.001f);
        work->limit_high = edge.z;
        work->to.x += 0.4f;
        work->to.z = work->limit_low + (work->limit_high - work->limit_low) * 0.5f;
        break;
    case 3:
        Get_LadderID_Limit(&point.x, &edge.x, 0.0f,
                           -0.001f);
        work->limit_low = edge.z;
        Get_LadderID_Limit(&point.x, &edge.x, 0.0f,
                           0.001f);
        work->limit_high = edge.z;
        work->to.x -= 0.4f;
        work->to.z = work->limit_low + (work->limit_high - work->limit_low) * 0.5f;
        break;
    }

    actor->animation.slot.speed = 1.0f / 30.0f;
    Fly_to_Ladder(actor);
    return 1;
}

void On_Ladder(register Actor * const actor)
{
    AlignedVector4 destination;
    AlignedVector4 probe;
    float up_step[4][4];
    float down_step[4][4];
    float left_step[4][4];
    float right_step[4][4];
    float move_x = 0.0f;
    float move_y = 0.0f;
    float move_z = 0.0f;
    float stick_x;
    float stick_y;
    float speed_x;
    float speed_y;
    float ground;
    float height;
    float top;
    signed char ladder_id;
    int direction;
    short step;
    int rung;
    int arrived;
    register LadderWork *work;

    work = &enepc[actor->number];
    memset(up_step, 0, sizeof(up_step));
    up_step[0][1] = 0.05f;
    up_step[1][2] = -0.05f;
    up_step[3][2] = -0.05f;
    memset(down_step, 0, sizeof(down_step));
    down_step[0][1] = -0.05f;
    down_step[1][2] = 0.05f;
    down_step[3][2] = 0.05f;
    memset(left_step, 0, sizeof(left_step));
    left_step[0][0] = -0.025f;
    left_step[1][1] = -0.025f;
    left_step[3][1] = 0.025f;
    memset(right_step, 0, sizeof(right_step));
    right_step[0][0] = 0.025f;
    right_step[1][1] = 0.025f;
    right_step[3][1] = -0.025f;

    ladder_id = work->ladder_id;
    direction = GameLoopState.ladder_direction[ladder_id];
    probe = actor->position;
    probe.x -= AdjTable_X[direction];
    probe.z -= AdjTable_Z[direction];
    height = GameLoopState.ladder_height[ladder_id];
    ground = Get_Height(&probe, 0, 0);
    Get_Attr(&probe, 0, 0);
    actor->translate_y = ground;

    if (!(GameLoopState.ladder_flags & 0x8000)) {
        stick_x = PadData[0].axis[2];
        stick_y = PadData[0].axis[3];

        if (stick_x >= LADDER_STICK_DEADZONE || stick_x <= -LADDER_STICK_DEADZONE) {
            speed_x = stick_x * 0.0004f * 1.5f;
        } else {
            speed_x = 0.0f;
        }

        if (stick_y >= LADDER_STICK_DEADZONE || stick_y <= -LADDER_STICK_DEADZONE) {
            speed_y = stick_y * 0.0004f * 1.5f;
        } else {
            speed_y = 0.0f;
        }

        if (speed_x == 0.0f && speed_y == 0.0f) {
            if (PadData[0].buttons & PAD_UP) {
                move_x = up_step[direction][0];
                move_y = up_step[direction][1];
                move_z = up_step[direction][2];
            }
            if (PadData[0].buttons & PAD_DOWN) {
                move_x = down_step[direction][0];
                move_y = down_step[direction][1];
                move_z = down_step[direction][2];
            }
            if (PadData[0].buttons & PAD_LEFT) {
                move_x = left_step[direction][0];
                move_y = left_step[direction][1];
                move_z = left_step[direction][2];
            }
            if (PadData[0].buttons & PAD_RIGHT) {
                move_x = right_step[direction][0];
                move_y = right_step[direction][1];
                move_z = right_step[direction][2];
            }
        } else {
            switch (direction) {
            case 0:
                move_x = speed_x;
                move_z = 0.0f;
                move_y = -speed_y;
                break;
            case 1:
                move_y = speed_x;
                move_z = speed_y;
                move_x = 0.0f;
                break;
            case 2:
            case 3:
                move_y = -speed_x;
                move_z = speed_y;
                move_x = 0.0f;
                break;
            }
        }
    }

    xglFontDebugPrintf(4, 2, "NUM = %3d,%3d,%8f", ladder_id, direction,
                       height);
    xglFontDebugPrintf(4, 10, "XYZ = %4f,%4f,%4f", move_x,
                       move_y, move_z);
    top = ground + height;
    xglFontDebugPrintf(4, 18, "HEI = %4f<%4f<%4f", ground,
                       actor->position.y, top);
    xglFontDebugPrintf(4, 26, "LIM = %4f<%4f<%4f", work->limit_low,
                       actor->position.x,
                       work->limit_high);

    for (step = 0; step < LADDER_STEP_COUNT; step++) {
        rung = LADDER_STEP_MIDDLE + step;
        if (LadderStep[rung] != GROUND_HEIGHT_NONE) {
            xglFontDebugPrintf(176, 120 - step * 8, "%3d:%8f", rung,
                               LadderStep[rung]);
        }
        if (LadderStep[LADDER_STEP_MIDDLE - step] != GROUND_HEIGHT_NONE) {
            xglFontDebugPrintf(176, 120 + step * 8, "%3d:%8f", LADDER_STEP_MIDDLE - step,
                               LadderStep[LADDER_STEP_MIDDLE - step]);
        }
    }

    switch (work->state) {
    case LADDER_STATE_STEP_UP:
        work->step_frame++;
        if (work->step_frame >= LADDER_STEP_FRAMES) {
            work->state = LADDER_STATE_HANG;
        }
        move_y = work->step_from +
                 (work->step_to - work->step_from) * work->step_frame / LADDER_STEP_FRAMES_F -
                 actor->position.y;
        work->lookat_point = &work->lookat_target.x;
        work->lookat_target.x = actor->position.x - AdjTable_X[direction];
        work->lookat_target.y = actor->position.y + 2.0f;
        work->lookat_target.z = actor->position.z - AdjTable_Z[direction];
        break;
    case LADDER_STATE_STEP_DOWN:
        work->step_frame++;
        if (work->step_frame >= LADDER_STEP_FRAMES) {
            work->state = LADDER_STATE_HANG;
        }
        move_y = work->step_from +
                 (work->step_to - work->step_from) * work->step_frame / LADDER_STEP_FRAMES_F -
                 actor->position.y;
        work->lookat_point = &work->lookat_target.x;
        work->lookat_target.x = actor->position.x - AdjTable_X[direction];
        work->lookat_target.y = actor->position.y - 2.0f;
        work->lookat_target.z = actor->position.z - AdjTable_Z[direction];
        break;
    case LADDER_STATE_HANG:
        work->step_frame = 0;
        work->lookat_point = 0;
        if (move_y == 0.0f) {
            xglSoundEffectStopID(LADDER_CLIMB_SE, 0);
        }
        if (work->step_wait >= 18) {
            move_y = 0.0f;
        }
        if (move_y < 0.0f) {
            if (actor->position.y - 0.7f <= ground &&
                GameLoopState.ladder_bottom_exit[ladder_id] == 0) {
                return;
            }
            xglSoundEffectNormalID(LADDER_CLIMB_SE, 0);
            work->step_frame = 0;
            work->step_from = actor->position.y;
            work->state = LADDER_STATE_STEP_DOWN;
            work->step_to = LadderStep[NowLadderStep-- - 1];
            ACT_setMotion(actor, 20);
            move_y = work->step_from +
                     (work->step_to - work->step_from) * work->step_frame / LADDER_STEP_FRAMES_F -
                     actor->position.y;
        }
        if (move_y > 0.0f) {
            if (top <= actor->position.y + 0.7f) {
                CallMethod_I("HashigoTop", work->ladder_id);
                if (GameLoopState.ladder_flags & 0x20) {
                    work->state = LADDER_STATE_AT_EXIT;
                }
                return;
            }
            xglSoundEffectNormalID(LADDER_CLIMB_SE, 0);
            work->step_frame = 0;
            work->step_from = actor->position.y;
            work->state = LADDER_STATE_STEP_UP;
            work->step_to = LadderStep[NowLadderStep++ + 1];
            ACT_setMotion(actor, 16);
            move_y = work->step_from +
                     (work->step_to - work->step_from) * work->step_frame / LADDER_STEP_FRAMES_F -
                     actor->position.y;
        }
        break;
    }

    if (work->step_wait < 0 || --work->step_wait < 0) {
        work->step_wait = 0;
    }

    if (actor->position.y + move_y > top) {
        actor->position.y = top;
        return;
    }

    actor->animation.slot.speed = 0.0f;
    actor->animation.slot.frame =
        (actor->animation.slot.forwardLimit - actor->animation.slot.reverseLimit) *
        work->step_frame / LADDER_STEP_FRAMES_F;

    if (actor->position.y + move_y < ground &&
        GameLoopState.ladder_bottom_exit[ladder_id] == 1) {
        CallMethod_I(hashigoBottomName, work->ladder_id);
        if (GameLoopState.ladder_flags & 0x20) {
            work->state = LADDER_STATE_AT_EXIT;
            return;
        }
        actor->position.y = ground;
        arrived = 1;
        destination = actor->position;
        switch (direction) {
        case 0:
            destination.z = actor->position.z + 0.5f + 0.41f;
            break;
        case 1:
            destination.x = actor->position.x - 0.5f - 0.41f;
            break;
        case 3:
            destination.x = actor->position.x + 0.5f + 0.41f;
            break;
        }
        ACT_setMotion2(actor, 31, 8);
    } else {
        if (direction != 0) {
            actor->position.x += move_x;
        }
        actor->position.y += move_y;
        if (direction != 1 && direction != 3) {
            actor->position.z += move_z;
        }
        arrived = 0;
        destination = actor->position;

        if (work->step_wait == 0) {
            if (direction == 0) {
                if (move_x < 0.0f) {
                    probe = actor->position;
                    probe.x = probe.x - AdjTable_X[direction] - 0.79999995f;
                    probe.z -= AdjTable_Z[direction];
                    if (Get_Arrival(Get_Attr(&probe, 0, 0)) != -1) {
                        ground = Get_Height(&probe, 0, 0x800);
                        if (ground <= actor->position.y &&
                            actor->position.y < ground + 1.0f) {
                            destination = probe;
                            destination.x -= 0.15f;
                            actor->target_angle = 3.1415927f;
                            work->lookat_point = 0;
                            arrived = 1;
                            ACT_setMotion2(actor, 18, 0);
                        }
                    }
                }
                if (move_x > 0.0f) {
                    probe = actor->position;
                    probe.x = probe.x - AdjTable_X[direction] + 0.79999995f;
                    probe.z -= AdjTable_Z[direction];
                    if (Get_Arrival(Get_Attr(&probe, 0, 0)) != -1) {
                        ground = Get_Height(&probe, 0, 0x800);
                        if (ground <= actor->position.y &&
                            actor->position.y < ground + 1.0f) {
                            destination = probe;
                            destination.x += 0.15f;
                            actor->target_angle = 3.1415927f;
                            work->lookat_point = 0;
                            arrived = 1;
                            ACT_setMotion2(actor, 17, 0);
                        }
                    }
                }
            } else {
                if (move_z < 0.0f) {
                    probe = actor->position;
                    probe.x -= AdjTable_X[direction];
                    probe.z = probe.z - AdjTable_Z[direction] - 0.79999995f;
                    if (Get_Arrival(Get_Attr(&probe, 0, 0)) != -1) {
                        ground = Get_Height(&probe, 0, 0x800);
                        if (ground - 0.5f <= actor->position.y &&
                            actor->position.y < ground + 1.0f) {
                            arrived = 1;
                            destination = probe;
                            destination.z -= 0.15f;
                            work->lookat_point = 0;
                            if (direction == 1) {
                                ACT_setMotion2(actor, 18, 8);
                            } else {
                                ACT_setMotion2(actor, 17, 8);
                            }
                        }
                    }
                }
                if (move_z > 0.0f) {
                    probe = actor->position;
                    probe.x -= AdjTable_X[direction];
                    probe.z = probe.z - AdjTable_Z[direction] + 0.79999995f;
                    if (Get_Arrival(Get_Attr(&probe, 0, 0)) != -1) {
                        ground = Get_Height(&probe, 0, 0x800);
                        if (ground <= actor->position.y &&
                            actor->position.y < ground + 1.0f) {
                            arrived = 1;
                            destination = probe;
                            destination.z += 0.15f;
                            work->lookat_point = 0;
                            if (direction == 1) {
                                ACT_setMotion2(actor, 17, 8);
                            } else {
                                ACT_setMotion2(actor, 18, 8);
                            }
                        }
                    }
                }
            }
        }
    }

    if (arrived == 1) {
        work->from = actor->position;
        work->to = destination;
        work->to.y = ground;
        work->fly_frames = LADDER_FLY_FRAMES;
        work->fly_frame = 0;
        work->state = LADDER_STATE_GET_OFF;
        xglSoundEffectStopID(LADDER_CLIMB_SE, 0);
        actor->animation.slot.speed = 0.033333335f;
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char hashigoBottomName[16] = "HashigoBottom";

int Ladder_Main(Actor *actor)
{
    LadderWork *work = &enepc[actor->number];

    actor->rotation.y = Get_Cursol_by_Reduce_Speed_Angle_Loop(
        actor->rotation.y, actor->target_angle, 6.0f);

    switch (work->state) {
    case LADDER_STATE_APPROACH:
    case LADDER_STATE_FLY_ON:
        Fly_to_Ladder(actor);
        break;
    case LADDER_STATE_HANG:
    case LADDER_STATE_STEP_UP:
    case LADDER_STATE_STEP_DOWN:
        On_Ladder(actor);
        break;
    case LADDER_STATE_GET_OFF:
        Get_Off_Ladder(actor);
        break;
    default:
        return Ready_Ladder(actor);
    }

    xglMatrixStackUnit();
    MotTransXYZ(&actor->position.x);
    xglMatrixStackRotZ(actor->rotation.z);
    xglMatrixStackRotY(actor->rotation.y);
    xglMatrixStackRotX(actor->rotation.x);
    ACT_updateMotion(actor);
    return 1;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
short NowLadderStep = 0;

short SelNum = -1;

short PhCunt = 0;
