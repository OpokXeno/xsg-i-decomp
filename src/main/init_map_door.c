#include "common.h"
#include "shared.h"
#include "init_map_door.h"
#include "main/karakama_test.h"
#include "main/near_dir.h"
#include "main/outer_product.h"
#include "main/xgl_dma.h"

typedef struct DoorPlayerState {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
    unsigned char unmodeled_20[0x34];
    float facing_angle;
} DoorPlayerState;

typedef struct DoorGameLoopState {
    unsigned char unmodeled_00[4];
    DoorPlayerState *player;
    unsigned char unmodeled_08[0x2a028];
} DoorGameLoopState;

extern DoorUnit MapUnit[64];
extern DoorGameLoopState GameLoopState;
int printf(const char *, ...);

/*
 * CheckDoorDist is called with no argument from AutoDoorStanbyFunc,
 * EventDoorStanbyFunc and MjDoorStanbyFunc (it is their first call, so $a0
 * still holds their own argument only by coincidence) and with the door
 * pointer from AutoDoorOpenNowFunc/HalfAutoDoorOpenNowFunc, which reload it
 * into $a0 after DoorOpenStanbyFunc clobbers the register. Left unprototyped
 * to admit both call shapes exactly as compiled.
 */
float CheckDoorDist();

void CheckDoorPos();
int CheckDoorSwitch(DoorUnit *door);
void DoorCommonFunc(DoorUnit *door);
void DoorOpenStanbyFunc();
void EventDoorStanbyFunc(DoorUnit *door);
void EventDoorOpenOpeFunc(DoorUnit *door);
void EventDoorOpenNowFunc(DoorUnit *door);
void AutoDoorOpenOpeFunc(DoorUnit *door);
void AutoDoorOpenNowFunc(DoorUnit *door);
void HalfAutoDoorOpenNowFunc(DoorUnit *door);
void MjDoorStanbyFunc(DoorUnit *door);
void AutoDoorStanbyFunc(DoorUnit *door);
int AutoDoorCloseOpeFunc(DoorUnit *door);
void EventDoorCloseOpeFunc(DoorUnit *door);
int xglSoundEffectCheckID();
void xglSoundEffectPosID();
void xglSoundEffectStopID();

float atan2f(float y, float x);
void GetPartsPos();
void GetPartsSize();
void MAP_updateUnitDoor(DoorUnit *door);
void AutoDoorFunc(DoorUnit *door);
int EventDoorFunc(DoorUnit *door);
int HalfAutoDoorFunc(DoorUnit *door);
int MjDoorFunc(DoorUnit *door);

/* MAP_updateUnitDoor passes the door; DoubleDoorFunc itself takes no parameter. */
void DoubleDoorFunc();

const char doorInfoFormat[56] = "InitMapDoor id=%d kind=%d doorType=%d doorRange=%f\n";

const char doorPositionFormat[32] = "doorpos[X]=%f Y=%f Z=%f\n";

const char doorHitSizeFormat[32] = "hitsize[X]=%f Y=%f Z=%f\n";

const char doorRangeFormat[40] = "range=%f scope=%f spd=%d\n";

void InitMapDoor(DoorUnit *door)
{
    DoorParams *params = &door->params;
    int linked_index;
    signed char door_type;

    GetPartsPos(door);
    __builtin_memcpy(&door->x, &door->model_x, 16);
    GetPartsSize(door);
    if (params->travel_distance == 0.0f) {
        params->travel_distance = door->resting_position.x +
                                 door->resting_position.x;
    }
    door->update_func = MAP_updateUnitDoor;
    if (door->open_phase != 4) {
        door->open_phase = 0;
    }
    linked_index = params->linked_unit;
    if (linked_index != -1) {
        DoorParams *linked_params = &MapUnit[linked_index].params;
        linked_params->open_limit = params->open_limit;
        linked_params->part_size_x = params->part_size_x;
        linked_params->part_size_y = params->part_size_y;
        linked_params->part_size_z = params->part_size_z;
        linked_params->part_offset_x = params->part_offset_x;
        linked_params->part_offset_y = params->part_offset_y;
        linked_params->part_offset_z = params->part_offset_z;
        linked_params->travel_distance = params->travel_distance;
        linked_params->trigger_distance = params->trigger_distance;
        MapUnit[linked_index].open_phase = 4;
        GetPartsPos(&MapUnit[linked_index]);
        MapUnit[linked_index].position = MapUnit[linked_index].model_position;
    }
    door_type = params->door_type;
    if (door_type == 1 && CheckDoorDist(door) < params->trigger_distance) {
        params->event_signal = door_type;
        door->open_phase = 2;
    }
    if (params->event_signal == 1) {
        door->open_phase = 2;
    }
    params->initialization_state = 0;
    door->flags |= 0x80000000U;
    if (!printflg) {
        return;
    }
    printf(doorInfoFormat, door->door_number, params->kind,
           params->door_type, params->travel_distance);
    printf(doorPositionFormat, door->x, door->y, door->z);
    printf(doorHitSizeFormat, door->resting_position.x,
           door->resting_position.y, door->resting_position.z);
    printf(doorRangeFormat, params->travel_distance,
           params->trigger_distance, params->open_limit);
}

void MAP_updateUnitDoor(DoorUnit *door)
{
    DoorParams *params = &door->params;

    /* Case 5 is written before case 4: the original emitted the HalfAutoDoorFunc block first. */
    switch (params->door_type) {
    case 1:
        AutoDoorFunc(door);
        break;
    case 2:
        EventDoorFunc(door);
        break;
    case 3:
        DoubleDoorFunc(door);
        break;
    case 5:
        HalfAutoDoorFunc(door);
        break;
    case 4:
        MjDoorFunc(door);
        break;
    }
    door->model_x = door->model->x;
    door->model_y = door->model->y;
    door->model_z = door->model->z;
    if (params->linked_unit != -1) {
        int index = params->linked_unit;
        DoorModel *model;

        MapUnit[index].open_timer = door->open_timer;
        DoorCommonFunc(&MapUnit[index]);
        model = MapUnit[index].model;
        MapUnit[index].model_lanes[0] = model->x;
        MapUnit[index].model_lanes[1] = model->y;
        MapUnit[index].model_lanes[2] = model->z;
    }
}

void AutoDoorFunc(DoorUnit *door)
{
    switch (door->open_phase) {
    case 0:
        AutoDoorStanbyFunc(door);
        return;
    case 1:
        AutoDoorOpenOpeFunc(door);
        return;
    case 2:
        AutoDoorOpenNowFunc(door);
        return;
    case 3:
        AutoDoorCloseOpeFunc(door);
        break;
    case 4:
        break;
    default:
        return;
    }
}

int EventDoorFunc(DoorUnit *door)
{
    switch (door->open_phase) {
    case 0:
        EventDoorStanbyFunc(door);
        return;
    case 1:
        EventDoorOpenOpeFunc(door);
        return;
    case 2:
        EventDoorOpenNowFunc(door);
        return;
    case 3:
        EventDoorCloseOpeFunc(door);
        /* fall through */
    case 4:
        return;
    default:
        return;
    }
}

int HalfAutoDoorFunc(DoorUnit *door)
{
    switch (door->open_phase) {
    case 0:
        EventDoorStanbyFunc(door);
        return;
    case 1:
        AutoDoorOpenOpeFunc(door);
        return;
    case 2:
        HalfAutoDoorOpenNowFunc(door);
        return;
    case 3:
        AutoDoorCloseOpeFunc(door);
        /* fall through */
    case 4:
        return;
    default:
        return;
    }
}

int MjDoorFunc(DoorUnit *door)
{
    switch (door->open_phase) {
    case 0:
        MjDoorStanbyFunc(door);
        return;
    case 1:
        AutoDoorOpenOpeFunc(door);
        return;
    case 2:
        AutoDoorOpenNowFunc(door);
        return;
    case 3:
        AutoDoorCloseOpeFunc(door);
        /* fall through */
    case 4:
        return;
    default:
        return;
    }
}

void DoubleDoorFunc(void)
{
}

void AutoDoorStanbyFunc(DoorUnit *door)
{
    int position;
    DoorParams *params;
    DoorModel *model;

    params = &door->params;
    model = door->model;
    model->x = door->x;
    model->y = door->y;
    model->z = door->z;
    if (CheckDoorDist() < params->trigger_distance) {
        door->open_phase = 1;
        if (xglSoundEffectCheckID(params->move_sound_channel, door->sound_channel + 1) != 0) {
            xglSoundEffectStopID(params->move_sound_channel, door->sound_channel + 1);
        }
        CheckDoorPos(door, &position);
        xglSoundEffectPosID(params->move_sound_channel, &position, 1, door->sound_channel + 1);
    }
}

void AutoDoorOpenOpeFunc(DoorUnit *door)
{
    int position;
    DoorParams *params;

    params = &door->params;
    door->open_timer = door->open_timer + 1;
    CheckDoorPos(door, &position);
    xglSoundEffectPosID(params->move_sound_channel, &position, 1, door->sound_channel + 1);
    DoorCommonFunc(door);
    if ((short) door->open_timer >= params->open_limit) {
        door->open_phase = 2;
    }
}

void AutoDoorOpenFuncSub(DoorUnit *door)
{
    signed char direction;

    direction = door->params.motion_axis;
    if (direction != 2) {
        if (direction < 3) {
            if (direction == 1) {
                door->model->x = door->model->x + door->model->direction_x;
                door->model->z = door->model->z + door->model->direction_z;
            }
        }
    } else {
        door->model->x = door->model->x - door->model->direction_x;
        door->model->z = door->model->z - door->model->direction_z;
    }
}

void AutoDoorOpenNowFunc(DoorUnit *door)
{
    int position[4];
    unsigned short next_close_delay;
    DoorParams *params;

    params = &door->params;
    DoorOpenStanbyFunc(door);
    door->open_timer = params->open_limit;
    /* separate load: the compiler reloads close_delay for the increment below instead of reusing the compare's value */
    next_close_delay = (unsigned short) door->close_delay;
    if (door->close_delay < 0x10) {
        door->close_delay = next_close_delay + 1;
        return;
    }
    if (params->trigger_distance < CheckDoorDist(door)) {
        if (xglSoundEffectCheckID(params->move_sound_channel, door->sound_channel + 1) != 0) {
            xglSoundEffectStopID(params->move_sound_channel, door->sound_channel + 1);
        }
        CheckDoorPos(door, position);
        xglSoundEffectPosID(params->close_sound_channel, position, 1, door->sound_channel + 1);
        door->open_phase = 3;
        door->close_delay = 0;
    }
}

int AutoDoorCloseOpeFunc(DoorUnit *door)
{
    Vector4 closing_position;
    Vector4 opening_position;
    DoorParams *params;
    unsigned int timer;

    params = &door->params;
    timer = door->open_timer;
    timer--;
    door->open_timer = timer;
    DoorCommonFunc(door);
    if (CheckDoorDist(door) < params->trigger_distance) {
        if (xglSoundEffectCheckID(params->close_sound_channel, door->sound_channel + 1) != 0) {
            xglSoundEffectStopID(params->close_sound_channel, door->sound_channel + 1);
        }
        CheckDoorPos(door, &opening_position);
        xglSoundEffectPosID(params->move_sound_channel, &opening_position, 1, door->sound_channel + 1);
        door->open_phase = 1;
    } else {
        CheckDoorPos(door, &closing_position);
        xglSoundEffectPosID(params->close_sound_channel, &closing_position, 1, door->sound_channel + 1);
        if ((short) door->open_timer <= 0) {
            door->open_phase = 0;
        }
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* Defined here, after the tables of the functions above, to keep .rodata in original address order. */
const char doorOpenFormat[16] = "id=%d DoorOpen\n";

const char doorCloseFormat[24] = "id=%d DoorClose\n";

void EventDoorStanbyFunc(DoorUnit *door)
{
    int position;
    signed char event_signal;
    DoorParams *params;
    DoorModel *model;

    params = &door->params;
    model = door->model;
    model->x = door->x;
    model->y = door->y;
    model->z = door->z;
    event_signal = params->event_signal;
    if (event_signal == 1) {
        if (printflg != 0) {
            printf(doorOpenFormat, door->door_number);
        }
        if (xglSoundEffectCheckID(params->move_sound_channel, door->sound_channel + 1) != 0) {
            xglSoundEffectStopID(params->move_sound_channel, door->sound_channel + 1);
        }
        CheckDoorPos(door, &position);
        xglSoundEffectPosID(params->move_sound_channel, &position, 1, door->sound_channel + 1);
        door->open_phase = event_signal;
    }
}

void EventDoorOpenOpeFunc(DoorUnit *door)
{
    int position;
    DoorParams *params;

    params = &door->params;
    door->open_timer = door->open_timer + 1;
    CheckDoorPos(door, &position);
    xglSoundEffectPosID(params->move_sound_channel, &position, 1, door->sound_channel + 1);
    DoorCommonFunc(door);
    if ((short) door->open_timer >= params->open_limit) {
        door->open_phase = 2;
    }
}

void EventDoorOpenNowFunc(DoorUnit *door)
{
    int position;
    DoorParams *params;

    params = &door->params;
    door->open_timer = params->open_limit;
    DoorOpenStanbyFunc(door);
    if (params->event_signal == 0) {
        if (printflg != 0) {
            printf(doorCloseFormat, door->door_number);
        }
        door->open_phase = 3;
        door->open_timer = params->open_limit;
        if (xglSoundEffectCheckID(params->move_sound_channel, door->sound_channel + 1) != 0) {
            xglSoundEffectStopID(params->move_sound_channel, door->sound_channel + 1);
        }
        CheckDoorPos(door, &position);
        xglSoundEffectPosID(params->close_sound_channel, &position, 1, door->sound_channel + 1);
    }
}

void EventDoorCloseOpeFunc(DoorUnit *door)
{
    int position;
    DoorParams *params = &door->params;

    door->open_timer = (short) door->open_timer - 1;
    CheckDoorPos(door, &position);
    xglSoundEffectPosID(params->close_sound_channel, &position, 1, door->sound_channel + 1);
    DoorCommonFunc(door);
    if ((short) door->open_timer <= 0) {
        door->open_phase = 0;
    }
}

void MjDoorStanbyFunc(DoorUnit *door)
{
    int position;
    DoorParams *params;
    DoorModel *model;

    params = &door->params;
    model = door->model;
    model->x = door->x;
    model->y = door->y;
    model->z = door->z;
    if (CheckDoorDist() < params->trigger_distance && CheckDoorSwitch(door) != 0) {
        door->open_phase = 1;
        if (xglSoundEffectCheckID(params->move_sound_channel, door->sound_channel + 1) != 0) {
            xglSoundEffectStopID(params->move_sound_channel, door->sound_channel + 1);
        }
        CheckDoorPos(door, &position);
        xglSoundEffectPosID(params->move_sound_channel, &position, 1, door->sound_channel + 1);
    }
}

void HalfAutoDoorOpenNowFunc(DoorUnit *door)
{
    int position[4];
    float distance;
    float trigger_distance;
    unsigned short next_close_delay;
    DoorParams *params;

    params = &door->params;
    DoorOpenStanbyFunc(door);
    door->open_timer = params->open_limit;
    /* separate load: the compiler reloads close_delay for the increment below instead of reusing the compare's value */
    next_close_delay = (unsigned short) door->close_delay;
    if (door->close_delay < 0xC) {
        door->close_delay = next_close_delay + 1;
        return;
    }
    distance = CheckDoorDist(door);
    trigger_distance = params->trigger_distance;
    if (params->event_signal == 1 && distance < trigger_distance) {
        params->event_signal = 0;
    }
    if (trigger_distance < distance && params->event_signal == 0) {
        door->close_delay = 0;
        door->open_phase = 3;
        CheckDoorPos(door, position);
        xglSoundEffectPosID(params->close_sound_channel, position, 1, door->sound_channel + 1);
    }
}

void DoorCommonFunc(DoorUnit *door)
{
    AlignedVector4 resting = door->resting_position;
    DoorParams *params = &door->params;
    DoorModel *opening_model;
    DoorModel *closing_model;
    short opening_frame;
    short closing_frame;
    float opening_x_distance;
    float closing_x_distance;
    float opening_progress;
    float closing_progress;
    float opening_limit;
    float closing_limit;

    switch (params->motion_axis) {
    case 0:
        break;
    case 1:
        opening_model = door->model;
        opening_x_distance = opening_model->direction_x * params->travel_distance;
        opening_frame = (short) door->open_timer;
        opening_progress = (float) opening_frame;
        opening_limit = (float) params->open_limit;
        opening_model->x = door->x + opening_x_distance * opening_progress / opening_limit;
        opening_model->z = door->z + opening_model->direction_z * params->travel_distance * opening_progress / opening_limit;
        break;
    case 2:
        closing_model = door->model;
        closing_x_distance = closing_model->direction_x * params->travel_distance;
        closing_frame = (short) door->open_timer;
        closing_progress = (float) closing_frame;
        closing_limit = (float) params->open_limit;
        closing_model->x = door->x - closing_x_distance * closing_progress / closing_limit;
        closing_model->z = door->z - closing_model->direction_z * params->travel_distance * closing_progress / closing_limit;
        break;
    case 3:
        door->model->y = door->y + resting.y * (float) (short) door->open_timer / (float) params->open_limit;
        break;
    case 4:
        door->model->y = door->y - resting.y * (float) (short) door->open_timer / (float) params->open_limit;
        break;
    }
    door->model_x = door->model->x;
    door->model_y = door->model->y;
    door->model_z = door->model->z;
}

void DoorOpenStanbyFunc(DoorUnit *door)
{
    AlignedVector4 resting = door->resting_position;
    DoorParams *params = &door->params;
    DoorModel *model;

    switch (params->motion_axis) {
    case 1:
        model = door->model;
        model->x = door->x + model->direction_x * params->travel_distance;
        model->z = door->z + model->direction_z * params->travel_distance;
        break;
    case 2:
        model = door->model;
        model->x = door->x - model->direction_x * params->travel_distance;
        model->z = door->z - model->direction_z * params->travel_distance;
        break;
    case 3:
        model = door->model;
        model->y = door->y + resting.y;
        break;
    case 4:
        model = door->model;
        model->y = door->y - resting.y;
        break;
    }
}

float CheckDoorDist(DoorUnit *door)
{
    DoorPlayerState *player = GameLoopState.player;
    float distance = 65535.0f;

    if (__builtin_fabsf(player->position.y - door->y) > 2.0f) {
        return distance;
    }

    {
        AlignedVector4 position;
        CheckDoorPos(door, &position);
        return CheckDist2D(&player->position, &position);
    }
}

int CheckDoorSwitch(DoorUnit *door)
{
    Vector4 position;
    DoorPlayerState *player = GameLoopState.player;
    float angle;

    CheckDoorPos(door, &position);
    if ((PadData.half_2a & 0x20) != 0) {
        angle = atan2f(position.x - player->position.x, position.z - player->position.z);
        angle = nearDir(angle, player->facing_angle);
        if (__builtin_fabsf(angle) < 1.04719758f) {
            return 1;
        }
    }
    return 0;
}

void CheckDoorPos(DoorUnit *door, AlignedVector4 *position)
{
    signed char linked_index = door->params.linked_unit;
    DoorUnit *target_unit;

    if (linked_index == -1) {
        __builtin_memcpy(position, &door->x, sizeof(*position));
        return;
    }

    target_unit = &MapUnit[linked_index];
    position->x = door->x - (door->x - target_unit->position.x) * 0.5f;
    position->y = door->y;
    position->z = door->z - (door->z - target_unit->position.z) * 0.5f;
}
