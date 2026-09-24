#include "common.h"
#include "shared.h"
#include "init_map_door.h"

extern signed char printflg;
extern unsigned char D_004CA3B8[];
extern unsigned char D_004CA3C8[];
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
int DoorCommonFunc(DoorUnit *door);
void DoorOpenStanbyFunc();
int xglSoundEffectCheckID();
void xglSoundEffectPosID();
void xglSoundEffectStopID();

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", InitMapDoor);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", MAP_updateUnitDoor);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", AutoDoorFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", EventDoorFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", HalfAutoDoorFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", MjDoorFunc);

/* MAP_updateUnitDoor's handler for a double door: nothing to do here. */
void DoubleDoorFunc(void)
{
}

/*
 * Opens when the player enters range: mirrors the door's own position into
 * its attached model, then when close enough starts the standby/opening
 * sound (restarting it first if it is already playing).
 */
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

/*
 * Advances the door's open travel counter, keeps the moving sound at the
 * door's position and applies the shared open motion (DoorCommonFunc); once
 * the travel counter reaches the configured limit, enters the open phase.
 */
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

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", AutoDoorOpenFuncSub);

/*
 * Holds the door open for a short delay, then once the player leaves
 * trigger range stops the moving sound, plays the closing sound and enters
 * the closing phase.
 */
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

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", AutoDoorCloseOpeFunc);

/*
 * Starts opening on the event signal, restarting the moving sound if it is
 * already playing and logging the door number when print mode is enabled.
 */
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
            printf((char *) D_004CA3B8, door->door_number);
        }
        if (xglSoundEffectCheckID(params->move_sound_channel, door->sound_channel + 1) != 0) {
            xglSoundEffectStopID(params->move_sound_channel, door->sound_channel + 1);
        }
        CheckDoorPos(door, &position);
        xglSoundEffectPosID(params->move_sound_channel, &position, 1, door->sound_channel + 1);
        door->open_phase = event_signal;
    }
}

/*
 * Advances the door's open travel counter, keeps the moving sound at the
 * door's position and applies the shared open motion (DoorCommonFunc); once
 * the travel counter reaches the configured limit, enters the open phase.
 */
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

/*
 * Holds the door open at the configured travel limit until the event signal
 * clears, then logs the door number when print mode is enabled, plays the
 * closing sound and enters the closing phase.
 */
void EventDoorOpenNowFunc(DoorUnit *door)
{
    int position;
    DoorParams *params;

    params = &door->params;
    door->open_timer = params->open_limit;
    DoorOpenStanbyFunc(door);
    if (params->event_signal == 0) {
        if (printflg != 0) {
            printf((char *) D_004CA3C8, door->door_number);
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

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", EventDoorCloseOpeFunc);

/*
 * Opens only once both the player is close enough and the configured switch
 * test passes, then starts the standby/opening sound as the other stanby
 * functions do.
 */
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

/*
 * Holds the door open for a short delay, arms closing once the player is
 * within trigger range, then closes with the closing sound once the player
 * leaves that range.
 */
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

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", DoorCommonFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", DoorOpenStanbyFunc);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", CheckDoorDist);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", CheckDoorSwitch);

INCLUDE_ASM("asm/main/nonmatchings/init_map_door", CheckDoorPos);
