#ifndef INCLUDE_MAIN_ENEMY_2_H
#define INCLUDE_MAIN_ENEMY_2_H

#include "shared.h"
#include "main/enemy_1.h"

/* Sibling of this TU, still original bytes (INCLUDE_ASM above): converts a
 * command's target selector into the enepc/Actor index Enemy_Command_Target
 * below stores, the same numbering ACT_create hands out into Actor.number
 * (+0x80). */
extern int Get_ActorNumber(int target);

/* Enemy_Command_Scale's per-entry blend, stored in each enemy-work entry at
 * +0x37d0 (ENEMY_SCALE_OFFSET in src/main/enemy_2.h). */
typedef struct EnemyScale {
    float current;
    float start;
    float target;
    short frame;
    short duration;
} EnemyScale;

/* One entry of the 16-entry enemy-work array `enepc` (main .data
 * 0x00379110, 0x38b0 bytes per entry); the offsets are the ones every TU
 * agrees on, see src/main/enemy_2.h and src/main/set_motion.h. */
typedef struct EnemyWork {
    float tentacle_range;
    short tentacle_duration;
    short locator_start_frame;
    short locator_end_frame;
    short territory_index;
    float height;
    Vector4 goback_position;
    Vector4 step_limit;
    Vector4 stop_cause;
    short spline_parameter;
    short fast_point_count;
    short hit_count;
    short hit_limit;
    signed char action;
    unsigned char unmodeled_49;
    unsigned char pause_kind;
    unsigned char hidden_unit;
    short turn_count;
    short explosion_count;
    short electric_count;
    short fall_count;
    signed char page_status[2][8];
    short page_count[2];
    unsigned char unmodeled_68[4];
    short chase_frame_limit;
    short chase_frame;
    unsigned int map_index;
    short mark;
    short mark_timer;
    unsigned char unmodeled_78[8];
    float spline_coordinates[256 * 4];
    short spline_point_count;
    unsigned char unmodeled_1082[0x2090 - 0x1082];
    short motion_tables[2][256][4];
    short route_index;
    short action_index;
    unsigned char unmodeled_3094[0x30a0 - 0x3094];
    float route_coordinates[64 * 4];
    short route_values[64][4];
    short route_metadata[2 * 64];
    short route_point_count;
    signed char freeze_state;
    signed char turn_lock;
    short target_number;
    unsigned char unmodeled_37a6[2];
    unsigned int previous_motion;
    unsigned char unmodeled_37ac[0x37b8 - 0x37ac];
    unsigned int entry_id;
    unsigned char unmodeled_37bc[4];
    Vector4 target_position;
    EnemyScale scale;
    int state_flags;
    unsigned char unmodeled_37e4[0x37f0 - 0x37e4];
    /* Enemy_Chase copies target_position or a collision-result position here
     * and passes the complete position to TM_Enemy_Move_Step. */
    DoublewordAlignedVector4 final_target;
    EnemySacMove sac_move;
    EnemyTurn turn;
    unsigned char unmodeled_3830[0x3858 - 0x3830];
    float freeze_saved_speed;
    unsigned char unmodeled_385c[0x3890 - 0x385c];
    float sight_position[3];
    unsigned char unmodeled_389c[0x38a8 - 0x389c];
    short applied_light_level;
    short light;
    unsigned char unmodeled_38ac[4];
} EnemyWork;

#endif /* INCLUDE_MAIN_ENEMY_2_H */
