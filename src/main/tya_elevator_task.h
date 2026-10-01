#ifndef SRC_MAIN_TYA_ELEVATOR_TASK_H
#define SRC_MAIN_TYA_ELEVATOR_TASK_H

#define ELEVATOR_UNIT_COUNT 64
#define ELEVATOR_SLOT_COUNT 64

typedef struct ElevatorPeer ElevatorPeer;
typedef struct ElevatorMapEntry ElevatorMapEntry;

/* The Unit native records a generic Java "py" field address here; the task
 * writes that same address as the elevator's float height. */
typedef union ElevatorFieldAddress {
    void *java_field_address;
    float *height;
} ElevatorFieldAddress;

typedef void (*ElevatorTaskCallback)(void *peer, int field_offset);

struct ElevatorPeer {
    unsigned char unmodeled_00[0x0c];
    ElevatorTaskCallback type_update; /* +0x0c */
    unsigned char unmodeled_10[0x94];
    short serial;                     /* +0xa4 */
    unsigned char unmodeled_a6[0xfa];
    unsigned char map_data_index;     /* +0x1a0 */
    unsigned char elevator_slot;      /* +0x1a1 */
    unsigned char unmodeled_1a2[2];
    float movement_speed;             /* +0x1a4 */
    float height;                     /* +0x1a8 */
    float target_height;              /* +0x1ac */
    float height_step;                /* +0x1b0 */
    ElevatorMapEntry *map_entry;      /* +0x1b4 */
    ElevatorFieldAddress elevator_field; /* +0x1b8, Java object's float height field */
    unsigned char unmodeled_1bc[4];
    unsigned char elevator_state;     /* +0x1c0 */
    unsigned char unmodeled_1c1[0x300 - 0x1c1];
};

struct ElevatorMapEntry {
    unsigned char unmodeled_00[0x34];
    float height;                     /* +0x34 */
    unsigned char unmodeled_38[0x40 - 0x38];
};

typedef struct ElevatorMapResource {
    unsigned char unmodeled_00[4];
    struct ElevatorMapScene *scene;   /* +0x04 */
} ElevatorMapResource;

typedef struct ElevatorMapScene {
    unsigned char unmodeled_00[0x50];
    unsigned int elevator_entries_offset; /* +0x50, relative to this scene */
} ElevatorMapScene;

typedef struct ElevatorGameState {
    unsigned char unmodeled_00[0x54];
    ElevatorMapResource *map_resource; /* +0x54 */
    unsigned char unmodeled_58[8];
    float elevator_heights[ELEVATOR_SLOT_COUNT]; /* +0x60 */
} ElevatorGameState;

extern ElevatorPeer MapUnit[ELEVATOR_UNIT_COUNT];
extern ElevatorGameState GameLoopState;

#endif
