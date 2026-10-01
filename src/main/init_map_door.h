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
    float direction_x; /* +0x00 */
    unsigned char unmodeled_04[4];
    float direction_z; /* +0x08 */
    unsigned char unmodeled_0c[0x24];
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
    signed char linked_unit; /* +0x05: followed map-unit index, or -1 */
    signed char motion_axis; /* +0x06: selects the motion direction */
    unsigned char unmodeled_07;
    short open_limit; /* +0x08: travel limit compared against DoorUnit.open_timer */
    unsigned char unmodeled_0a[0x1a];
    float trigger_distance; /* +0x24: compared against CheckDoorDist(door) */
    float travel_distance; /* +0x28 */
    unsigned char unmodeled_2c[8];
    int move_sound_channel; /* +0x34: sound channel while the door is moving/open */
    int close_sound_channel; /* +0x38: sound channel for the closing chime */
} DoorParams;

/*
 * A map door's runtime state. InitMapDoor and MAP_updateUnitDoor (main:
 * 0x002c4760, 0x002c4a18, this TU, still assembly) construct and drive it;
 * only the members the AutoDoor/EventDoor/MjDoor/HalfAutoDoor state
 * functions touch are named here.
 */
typedef union DoorPosition {
    Vector4 values;
    u64 words[2];
} DoorPosition;

typedef struct DoorUnit {
    unsigned char unmodeled_000[0x10];
    float model_x; /* +0x10: position copied from the model after movement */
    float model_y; /* +0x14 */
    float model_z; /* +0x18 */
    unsigned char unmodeled_01c[0x64];
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
    unsigned char unmodeled_0ac[4];
    DoorPosition resting_position; /* +0xb0: original ld/sd copies */
    float x; /* +0xc0 */
    float y; /* +0xc4 */
    float z; /* +0xc8 */
    float w; /* +0xcc */
    unsigned char unmodeled_0d0[0xd0];
    DoorParams params; /* +0x1a0 */
} DoorUnit;

/* MapUnit records use the same 0x300-byte stride as the map data. */
typedef struct DoorMapUnit {
    unsigned char unmodeled_000[0xc0];
    Vector4 position; /* +0xc0 */
    unsigned char unmodeled_0d0[0x230];
} DoorMapUnit;

#endif /* SRC_MAIN_INIT_MAP_DOOR_H */
