#include "common.h"

struct LadderAttributeTable {
    int attributes[16];
};

struct ArrivalAttributeTable {
    int attributes[4];
};

const struct LadderAttributeTable D_004CAC20 = {{
    0x00100000, 0x00110000, 0x00120000, 0x00130000,
    0x00140000, 0x00150000, 0x00160000, 0x00170000,
    0x00180000, 0x00190000, 0x001A0000, 0x001B0000,
    0x001C0000, 0x001D0000, 0x001E0000, 0x001F0000
}};

const struct ArrivalAttributeTable D_004CAC60 = {{
    0x00200000, 0x00210000, 0x00220000, 0x00230000
}};

short PhCunt = 0;

extern int Get_Attr_NU(const float *position, int mapIndex, int attrMask);

#include "get_ladder.h"

#include "shared.h"

#include "main/xgl_2.h"

float AdjTable_X[4] = {0.0f, 0.41f, 0.0f, -0.41f};

float AdjTable_Z[4] = {-0.41f, 0.0f, 0.41f, 0.0f};

float LadderStep[127] = {0.0f};

extern short NowLadderStep;

/* The original copies these 16-byte points with aligned doubleword loads and stores. */

typedef struct LadderPoint {
    Vector4 coordinates;
    u64 alignment[0];
} LadderPoint;

typedef struct LadderWork {
    unsigned char unmodeled_0000[0x3800];
    LadderPoint from;
    LadderPoint to;
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

typedef struct LadderActor {
    unsigned char unmodeled_00[0x10];
    LadderPoint position;
    unsigned char unmodeled_20[0x30];
    Vector4 rotation;
    unsigned char unmodeled_60[0x20];
    unsigned char number;
    unsigned char unmodeled_81[0x4dc - 0x81];
    float translate_y;
    unsigned char unmodeled_4e0[0x6f4 - 0x4e0];
    float motion_time;
    float speed;
    float motion_start;
    float motion_end;
    unsigned char unmodeled_704[0x9e4 - 0x704];
    float target_angle;
} LadderActor;

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
    signed char ladder_direction[16];
    unsigned char ladder_bottom_exit[16];
    float ladder_height[16];
    unsigned char unmodeled_2a000[0x30];
} LadderGameState;

typedef struct LadderPad {
    unsigned char unmodeled_00[0x28];
    unsigned short buttons;
    unsigned char unmodeled_2a[0x3a];
    signed char axis[4];
} LadderPad;

extern LadderWork enepc[16];

extern LadderGameState GameLoopState;

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

extern void ACT_setMotion2(LadderActor *actor, int motion, int blend);

extern float Get_Cursol_by_Reduce_Speed_Angle_Loop(float current, float target,
                                                   float speed);

extern float *MotTransXYZ(float *xyz);

extern void ACT_updateMotion(LadderActor *actor);

extern const float D_004D8068;

extern const float D_004D806C;

extern const float D_004D8070;

extern LadderPad PadData[2];

extern void ACT_setMotion(LadderActor *actor, unsigned int motion);

extern void CallMethod_I(const char *method_name, int argument1);

extern void xglSoundEffectNormalID(int id, int channel);

extern void xglSoundEffectStopID(int id, int channel);

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

extern void *memset(void *destination, int value, unsigned int count);

extern const char D_004CAD30[16];

/* The original copies these 16-byte points with aligned doubleword loads and stores. */

extern void On_Ladder(LadderActor *actor);

/* The original copies these 16-byte points with aligned doubleword loads and stores. */

short Get_Ladder(int attribute)
{
    struct LadderAttributeTable table = D_004CAC20;
    short index = 0;

    do {
        if (attribute == table.attributes[index]) {
            return index;
        }

        index++;
    } while (index < 16);

    return -1;
}

short Get_Arrival(int attribute)
{
    struct ArrivalAttributeTable table = D_004CAC60;
    short index = 0;

    do {
        if (attribute == table.attributes[index]) {
            return index;
        }

        index++;
    } while (index < 4);

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
        if (index >= 1024) {
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

    if (index == 1024) {
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
        if (index >= 256) {
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

    return index != 256;
}

void Fly_to_Ladder(LadderActor *actor)
{
    LadderWork *work = &enepc[actor->number];
    LadderPoint point;
    float height;
    short i;
    short state = work->state;

    if (state == 1) {
        work->fly_frame++;
        if ((short) work->fly_frame < 13) {
            return;
        }

        work->fly_frame = 0;
        actor->target_angle = work->facing_angle;

        if (work->from_top == 0) {
            int direction = GameLoopState.ladder_direction[work->ladder_id];
            int motion = 21;

            if (direction == 0 &&
                work->to.coordinates.x < work->from.coordinates.x) {
                motion = 19;
            } else if (direction == state &&
                       work->from.coordinates.y < work->to.coordinates.y) {
                motion = 19;
            } else if (direction == 3 &&
                       work->from.coordinates.y < work->to.coordinates.y) {
                motion = 19;
            }
            ACT_setMotion2(actor, motion, 8);
        } else {
            ACT_setMotion2(actor, 28, 8);
        }
        work->state = 3;
    }

    Get_MiddlePoint_Parabora(&work->from.coordinates, &work->to.coordinates,
                             work->fly_frame, work->fly_frames, 0.0f,
                             &actor->position.coordinates);

    if ((short) work->fly_frame++ < work->fly_frames) {
        return;
    }

    work->fly_frame = -1;
    work->state = 4;
    work->step_wait = 20;
    ACT_setMotion2(actor, 20, 8);
    actor->speed = D_004D8068;

    point = actor->position;
    point.coordinates.x -= AdjTable_X[GameLoopState.ladder_direction[work->ladder_id]];
    point.coordinates.z -= AdjTable_Z[GameLoopState.ladder_direction[work->ladder_id]];
    height = Get_Height(&point.coordinates, 0, 0);

    NowLadderStep = 63;
    if (actor->position.coordinates.y < height || work->from_top == 1) {
        actor->position.coordinates.y = height;
    }
    LadderStep[63] = actor->position.coordinates.y;

    for (i = 1; i < 63; i++) {
        float rise = i * D_004D806C;
        float base = actor->position.coordinates.y;

        LadderStep[63 + i] = base + rise;
        LadderStep[63 - i] = base - rise;
    }
}

void Get_Off_Ladder(LadderActor *actor)
{
    LadderWork *work = &enepc[actor->number];

    work->fly_frame++;
    Get_MiddlePoint(&work->from.coordinates.x, &work->to.coordinates.x,
                    work->fly_frame, work->fly_frames,
                    &actor->position.coordinates.x);

    if ((short) work->fly_frame >= work->fly_frames) {
        work->fly_frame = -1;
        work->state = 0;
        ACT_setMotion2(actor, 0, 8);
        GameLoopState.player->flags |= 0x1000;
        GameLoopState.map_flags |= 0x20;
    }

    actor->speed = D_004D8070;
}

short Ready_Ladder(LadderActor *actor)
{
    LadderPoint point;
    LadderPoint edge;
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

    Get_Height(&actor->position.coordinates, 0, 0);
    attribute = Get_Attr(&actor->position.coordinates, 0, 0);
    ladder = Get_Ladder(attribute);
    if (ladder == -1 && Get_Arrival(attribute) == -1) {
        return 0;
    }

    work->from = actor->position;
    if (ladder != -1) {
        direction = GameLoopState.ladder_direction[ladder];
        work->to = actor->position;
        work->from_top = 1;
        work->fly_frames = 13;
        lift = 0.0f;
    } else {
        actor->target_angle = facing[Get_Arrival(attribute)];
        point = actor->position;
        point.coordinates.x += probe_x[Get_Arrival(attribute)];
        point.coordinates.z += probe_z[Get_Arrival(attribute)];
        ladder = Get_Ladder(Get_Attr(&point.coordinates, 0, 0));
        if (ladder == -1) {
            return 0;
        }
        work->to = point;
        work->fly_frames = 13;
        work->from_top = 0;
        direction = GameLoopState.ladder_direction[ladder];
    }

    work->facing_angle = facing[direction];
    actor->target_angle = work->facing_angle;
    work->state = 1;
    ACT_setMotion2(actor, 0, 9);

    point = work->to;
    point.coordinates.y += Get_Height(&work->to.coordinates, 0, 0);
    Get_LadderID_Limit(&point.coordinates.x, &edge.coordinates.x,
                       limit_x[direction], limit_z[direction]);
    height = Get_Height(&edge.coordinates, 0, 0);
    work->to = edge;
    ceiling = Get_Multi_Max_Under(
        height, 0.7f, actor->position.coordinates.y + lift + 0.35f);
    work->to.coordinates.y = ceiling;
    work->fly_frame = 0;
    work->ladder_id = ladder;
    work->ceiling_height = ceiling;
    point = edge;

    switch (direction) {
    case 0:
        Get_LadderID_Limit(&point.coordinates.x, &edge.coordinates.x, -0.001f,
                           0.0f);
        work->limit_low = edge.coordinates.x;
        Get_LadderID_Limit(&point.coordinates.x, &edge.coordinates.x, 0.001f,
                           0.0f);
        work->limit_high = edge.coordinates.x;
        work->to.coordinates.x = work->limit_low + (work->limit_high - work->limit_low) * 0.5f;
        work->to.coordinates.z -= 0.4f;
        break;
    case 1:
        Get_LadderID_Limit(&point.coordinates.x, &edge.coordinates.x, 0.0f,
                           -0.001f);
        work->limit_low = edge.coordinates.z;
        Get_LadderID_Limit(&point.coordinates.x, &edge.coordinates.x, 0.0f,
                           0.001f);
        work->limit_high = edge.coordinates.z;
        work->to.coordinates.x += 0.4f;
        work->to.coordinates.z = work->limit_low + (work->limit_high - work->limit_low) * 0.5f;
        break;
    case 3:
        Get_LadderID_Limit(&point.coordinates.x, &edge.coordinates.x, 0.0f,
                           -0.001f);
        work->limit_low = edge.coordinates.z;
        Get_LadderID_Limit(&point.coordinates.x, &edge.coordinates.x, 0.0f,
                           0.001f);
        work->limit_high = edge.coordinates.z;
        work->to.coordinates.x -= 0.4f;
        work->to.coordinates.z = work->limit_low + (work->limit_high - work->limit_low) * 0.5f;
        break;
    }

    actor->speed = 1.0f / 30.0f;
    Fly_to_Ladder(actor);
    return 1;
}

void On_Ladder(register LadderActor * const actor)
{
    LadderPoint destination;
    LadderPoint probe;
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
    probe.coordinates.x -= AdjTable_X[direction];
    probe.coordinates.z -= AdjTable_Z[direction];
    height = GameLoopState.ladder_height[ladder_id];
    ground = Get_Height(&probe.coordinates, 0, 0);
    Get_Attr(&probe.coordinates, 0, 0);
    actor->translate_y = ground;

    if (!(GameLoopState.ladder_flags & 0x8000)) {
        stick_x = PadData[0].axis[2];
        stick_y = PadData[0].axis[3];

        if (stick_x >= 32.0f || stick_x <= -32.0f) {
            speed_x = stick_x * 0.0004f * 1.5f;
        } else {
            speed_x = 0.0f;
        }

        if (stick_y >= 32.0f || stick_y <= -32.0f) {
            speed_y = stick_y * 0.0004f * 1.5f;
        } else {
            speed_y = 0.0f;
        }

        if (speed_x == 0.0f && speed_y == 0.0f) {
            if (PadData[0].buttons & 0x1000) {
                move_x = up_step[direction][0];
                move_y = up_step[direction][1];
                move_z = up_step[direction][2];
            }
            if (PadData[0].buttons & 0x4000) {
                move_x = down_step[direction][0];
                move_y = down_step[direction][1];
                move_z = down_step[direction][2];
            }
            if (PadData[0].buttons & 0x8000) {
                move_x = left_step[direction][0];
                move_y = left_step[direction][1];
                move_z = left_step[direction][2];
            }
            if (PadData[0].buttons & 0x2000) {
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
                       actor->position.coordinates.y, top);
    xglFontDebugPrintf(4, 26, "LIM = %4f<%4f<%4f", work->limit_low,
                       actor->position.coordinates.x,
                       work->limit_high);

    for (step = 0; step < 127; step++) {
        rung = 63 + step;
        if (LadderStep[rung] != -1000.0f) {
            xglFontDebugPrintf(176, 120 - step * 8, "%3d:%8f", rung,
                               LadderStep[rung]);
        }
        if (LadderStep[63 - step] != -1000.0f) {
            xglFontDebugPrintf(176, 120 + step * 8, "%3d:%8f", 63 - step,
                               LadderStep[63 - step]);
        }
    }

    switch (work->state) {
    case 5:
        work->step_frame++;
        if (work->step_frame >= 15) {
            work->state = 4;
        }
        move_y = work->step_from +
                 (work->step_to - work->step_from) * work->step_frame / 15.0f -
                 actor->position.coordinates.y;
        work->lookat_point = &work->lookat_target.x;
        work->lookat_target.x = actor->position.coordinates.x - AdjTable_X[direction];
        work->lookat_target.y = actor->position.coordinates.y + 2.0f;
        work->lookat_target.z = actor->position.coordinates.z - AdjTable_Z[direction];
        break;
    case 6:
        work->step_frame++;
        if (work->step_frame >= 15) {
            work->state = 4;
        }
        move_y = work->step_from +
                 (work->step_to - work->step_from) * work->step_frame / 15.0f -
                 actor->position.coordinates.y;
        work->lookat_point = &work->lookat_target.x;
        work->lookat_target.x = actor->position.coordinates.x - AdjTable_X[direction];
        work->lookat_target.y = actor->position.coordinates.y - 2.0f;
        work->lookat_target.z = actor->position.coordinates.z - AdjTable_Z[direction];
        break;
    case 4:
        work->step_frame = 0;
        work->lookat_point = 0;
        if (move_y == 0.0f) {
            xglSoundEffectStopID(0x20011, 0);
        }
        if (work->step_wait >= 18) {
            move_y = 0.0f;
        }
        if (move_y < 0.0f) {
            if (actor->position.coordinates.y - 0.7f <= ground &&
                GameLoopState.ladder_bottom_exit[ladder_id] == 0) {
                return;
            }
            xglSoundEffectNormalID(0x20011, 0);
            work->step_frame = 0;
            work->step_from = actor->position.coordinates.y;
            work->state = 6;
            work->step_to = LadderStep[NowLadderStep-- - 1];
            ACT_setMotion(actor, 20);
            move_y = work->step_from +
                     (work->step_to - work->step_from) * work->step_frame / 15.0f -
                     actor->position.coordinates.y;
        }
        if (move_y > 0.0f) {
            if (top <= actor->position.coordinates.y + 0.7f) {
                CallMethod_I("HashigoTop", work->ladder_id);
                if (GameLoopState.ladder_flags & 0x20) {
                    work->state = 8;
                }
                return;
            }
            xglSoundEffectNormalID(0x20011, 0);
            work->step_frame = 0;
            work->step_from = actor->position.coordinates.y;
            work->state = 5;
            work->step_to = LadderStep[NowLadderStep++ + 1];
            ACT_setMotion(actor, 16);
            move_y = work->step_from +
                     (work->step_to - work->step_from) * work->step_frame / 15.0f -
                     actor->position.coordinates.y;
        }
        break;
    }

    if (work->step_wait < 0 || --work->step_wait < 0) {
        work->step_wait = 0;
    }

    if (actor->position.coordinates.y + move_y > top) {
        actor->position.coordinates.y = top;
        return;
    }

    actor->speed = 0.0f;
    actor->motion_time =
        (actor->motion_end - actor->motion_start) * work->step_frame / 15.0f;

    if (actor->position.coordinates.y + move_y < ground &&
        GameLoopState.ladder_bottom_exit[ladder_id] == 1) {
        CallMethod_I(D_004CAD30, work->ladder_id);
        if (GameLoopState.ladder_flags & 0x20) {
            work->state = 8;
            return;
        }
        actor->position.coordinates.y = ground;
        arrived = 1;
        destination = actor->position;
        switch (direction) {
        case 0:
            destination.coordinates.z = actor->position.coordinates.z + 0.5f + 0.41f;
            break;
        case 1:
            destination.coordinates.x = actor->position.coordinates.x - 0.5f - 0.41f;
            break;
        case 3:
            destination.coordinates.x = actor->position.coordinates.x + 0.5f + 0.41f;
            break;
        }
        ACT_setMotion2(actor, 31, 8);
    } else {
        if (direction != 0) {
            actor->position.coordinates.x += move_x;
        }
        actor->position.coordinates.y += move_y;
        if (direction != 1 && direction != 3) {
            actor->position.coordinates.z += move_z;
        }
        arrived = 0;
        destination = actor->position;

        if (work->step_wait == 0) {
            if (direction == 0) {
                if (move_x < 0.0f) {
                    probe = actor->position;
                    probe.coordinates.x = probe.coordinates.x - AdjTable_X[direction] - 0.79999995f;
                    probe.coordinates.z -= AdjTable_Z[direction];
                    if (Get_Arrival(Get_Attr(&probe.coordinates, 0, 0)) != -1) {
                        ground = Get_Height(&probe.coordinates, 0, 0x800);
                        if (ground <= actor->position.coordinates.y &&
                            actor->position.coordinates.y < ground + 1.0f) {
                            destination = probe;
                            destination.coordinates.x -= 0.15f;
                            actor->target_angle = 3.1415927f;
                            work->lookat_point = 0;
                            arrived = 1;
                            ACT_setMotion2(actor, 18, 0);
                        }
                    }
                }
                if (move_x > 0.0f) {
                    probe = actor->position;
                    probe.coordinates.x = probe.coordinates.x - AdjTable_X[direction] + 0.79999995f;
                    probe.coordinates.z -= AdjTable_Z[direction];
                    if (Get_Arrival(Get_Attr(&probe.coordinates, 0, 0)) != -1) {
                        ground = Get_Height(&probe.coordinates, 0, 0x800);
                        if (ground <= actor->position.coordinates.y &&
                            actor->position.coordinates.y < ground + 1.0f) {
                            destination = probe;
                            destination.coordinates.x += 0.15f;
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
                    probe.coordinates.x -= AdjTable_X[direction];
                    probe.coordinates.z = probe.coordinates.z - AdjTable_Z[direction] - 0.79999995f;
                    if (Get_Arrival(Get_Attr(&probe.coordinates, 0, 0)) != -1) {
                        ground = Get_Height(&probe.coordinates, 0, 0x800);
                        if (ground - 0.5f <= actor->position.coordinates.y &&
                            actor->position.coordinates.y < ground + 1.0f) {
                            arrived = 1;
                            destination = probe;
                            destination.coordinates.z -= 0.15f;
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
                    probe.coordinates.x -= AdjTable_X[direction];
                    probe.coordinates.z = probe.coordinates.z - AdjTable_Z[direction] + 0.79999995f;
                    if (Get_Arrival(Get_Attr(&probe.coordinates, 0, 0)) != -1) {
                        ground = Get_Height(&probe.coordinates, 0, 0x800);
                        if (ground <= actor->position.coordinates.y &&
                            actor->position.coordinates.y < ground + 1.0f) {
                            arrived = 1;
                            destination = probe;
                            destination.coordinates.z += 0.15f;
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
        work->to.coordinates.y = ground;
        work->fly_frames = 13;
        work->fly_frame = 0;
        work->state = 7;
        xglSoundEffectStopID(0x20011, 0);
        actor->speed = 0.033333335f;
    }
}

int Ladder_Main(LadderActor *actor)
{
    LadderWork *work = &enepc[actor->number];

    actor->rotation.y = Get_Cursol_by_Reduce_Speed_Angle_Loop(
        actor->rotation.y, actor->target_angle, 6.0f);

    switch (work->state) {
    case 1:
    case 3:
        Fly_to_Ladder(actor);
        break;
    case 4:
    case 5:
    case 6:
        On_Ladder(actor);
        break;
    case 7:
        Get_Off_Ladder(actor);
        break;
    default:
        return Ready_Ladder(actor);
    }

    xglMatrixStackUnit();
    MotTransXYZ(&actor->position.coordinates.x);
    xglMatrixStackRotZ(actor->rotation.z);
    xglMatrixStackRotY(actor->rotation.y);
    xglMatrixStackRotX(actor->rotation.x);
    ACT_updateMotion(actor);
    return 1;
}
