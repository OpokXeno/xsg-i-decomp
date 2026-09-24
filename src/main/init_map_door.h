/*
 * TU-local declarations of main/tu183 (src/main/init_map_door.c).
 */

#ifndef SRC_MAIN_INIT_MAP_DOOR_H
#define SRC_MAIN_INIT_MAP_DOOR_H

/*
 * Object referenced by DoorUnit.model. AutoDoorStanbyFunc, EventDoorStanbyFunc
 * and MjDoorStanbyFunc (main:0x002c4d28, 0x002c5088, 0x002c52f0) mirror the
 * door's own position (DoorUnit.x/y/z) into it every standby tick; no other
 * member is evidenced.
 */
typedef struct DoorModel {
    unsigned char unmodeled_00[0x30];
    float x; /* +0x30 */
    float y; /* +0x34 */
    float z; /* +0x38 */
} DoorModel;

/*
 * Per-door configuration block, embedded at DoorUnit +0x1a0. InitMapDoor
 * (this TU, still assembly) builds it from the map's door data; only the
 * members the AutoDoor/EventDoor/MjDoor/HalfAutoDoor state functions touch
 * are named here.
 */
typedef struct DoorParams {
    unsigned char unmodeled_00[4];
    signed char event_signal; /* +0x04: EventDoorStanbyFunc's open trigger; HalfAutoDoorOpenNowFunc clears it once armed */
    unsigned char unmodeled_05[3];
    short open_limit; /* +0x08: travel limit compared against DoorUnit.open_timer */
    unsigned char unmodeled_0a[0x1a];
    float trigger_distance; /* +0x24: compared against CheckDoorDist(door) */
    unsigned char unmodeled_28[0x0c];
    int move_sound_channel; /* +0x34: sound channel while the door is moving/open */
    int close_sound_channel; /* +0x38: sound channel for the closing chime */
} DoorParams;

/*
 * A map door's runtime state. InitMapDoor and MAP_updateUnitDoor (main:
 * 0x002c4760, 0x002c4a18, this TU, still assembly) construct and drive it;
 * only the members the AutoDoor/EventDoor/MjDoor/HalfAutoDoor state
 * functions touch are named here.
 */
typedef struct DoorUnit {
    unsigned char unmodeled_000[0x80];
    DoorModel *model; /* +0x80 */
    unsigned char unmodeled_084[0x1c];
    unsigned char sound_channel; /* +0xa0: xglSoundEffect*ID channel base (calls add 1) */
    unsigned char unmodeled_0a1;
    signed char open_phase; /* +0xa2: 1 opening, 2 open, 3 closing (this allocation's assigned values) */
    unsigned char unmodeled_0a3;
    short door_number; /* +0xa4: printed by the event-door debug logs */
    unsigned char unmodeled_0a6[2];
    unsigned short open_timer; /* +0xa8 */
    short close_delay; /* +0xaa */
    unsigned char unmodeled_0ac[0x14];
    float x; /* +0xc0 */
    float y; /* +0xc4 */
    float z; /* +0xc8 */
    unsigned char unmodeled_0cc[0xd4];
    DoorParams params; /* +0x1a0 */
} DoorUnit;

#endif /* SRC_MAIN_INIT_MAP_DOOR_H */
