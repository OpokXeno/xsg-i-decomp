#ifndef ENEMY_SYSTEM_INIT_H
#define ENEMY_SYSTEM_INIT_H

#include "shared.h"

/* GameLoopState is opaque here; only this scalar offset is evidenced
 * (sw zero at 0x002d3e44, absolute-addressed through a lui/sw pair rather
 * than gp-relative). */
typedef unsigned int GameLoopStateWords[];
extern GameLoopStateWords GameLoopState;

/* Buffer addresses Enemy_SystemInit fills with xglCdReadFile, one per
 * enemy resource file (data\matumoto\enemy.dat, spline.dat, bikkuri.lex,
 * hatena.lex, maru.lex and sikaku.lex in that order). res_get_path.c reads
 * AdrsEnemyPreset back as the buffer Enemy_LoadPreset fills; this TU only
 * ever writes the six, so their pointee type stays unmodelled. */
extern void *AdrsEnemyPreset;
extern void *AdrsEnemySpline;
extern void *AdrsEnemyExclamation;
extern void *AdrsEnemyQuestion;
extern void *AdrsEnemySphere;
extern void *AdrsEnemySquare;

/* The enemy record ACT_createEnemy builds: one 0xa70-byte slot of `actor`
 * and its 0x38b0-byte `enepc` entry, modelled only as far as this TU writes
 * them. Member names come from the readers:
 *   - EnemyInitParams (actor + 0xc0): Enemy_Init copies a preset record into
 *     it when preset_index is not -1 and otherwise copies locator_distance,
 *     move_kind, body_radius and event_type into the work entry; it divides
 *     route_value_count by 3 and homing_value_count by 4 for the point counts;
 *     ex_motion_resource -1 means no ACT_initExMotion. ACT_updateEnemy passes
 *     sight_angle and sight_distance to Check_InsideFan, skips the ground clamp
 *     when gravity_mode is 1 and compares light_level with the entry's
 *     applied_light_level. Enemy_FindByEar compares the player distance with
 *     hearing_distance; Enemy_Chase moves at chase_speed while the target is
 *     nearer than chase_distance; Enemy_Route steps route_step_length and draws
 *     its pause/threat chances from pause_interval and threat_interval, and
 *     hands wall_attribute (copied to the entry) to Check_Wall.
 *     src/main/call_java_method.h names actor + 0x124 locator_flags, and
 *     Get_DefaultMotion reads default_motion_table[mode] (lh 0x130 at
 *     0x002d0a10, the table src/main/set_motion.h names at actor + 0x130).
 *   - EnemyInitWork: names shared with src/main/enemy_1.h, enemy_2.h,
 *     enemy_3.h, set_motion.h and call_java_method.h at the same offsets;
 *     scale_frame is the -1-terminated frame of the scale tween ACT_updateEnemy
 *     runs; sense_block_timer counts down and blocks sight and hearing;
 *     encount_lock is tested on the player's entry before Check_Encount.
 *   - EnemyInitActor: lookat_timer and lookat_target are the look-at
 *     countdown and target slot src/main/set_motion.h names at +0x9ec/+0x9ee
 *     (Actor_LookAt_Init writes 0 and -1, as here); effects holds the
 *     sefCreateEffectCf results, effect_next the ring index into it and
 *     effect_interval the frame count between them.
 * The value_<actor offset> members have no identified reader yet; only their
 * offsets, widths and the values written here are evidenced. */
typedef struct EnemyInitParams {
    float route_step_length;
    float chase_distance;
    float value_c8;
    float chase_speed;
    float sight_angle;
    float sight_distance;
    short value_d8;
    short pause_interval;
    short value_dc;
    short threat_interval;
    short value_e0;
    short gravity_mode;
    float locator_distance;
    short wall_attribute;
    short move_kind;
    int preset_index;
    unsigned char unmodeled_30[0x4];
    int route_value_count;
    unsigned char unmodeled_38[0x4];
    int homing_value_count;
    float body_radius;
    int ex_motion_resource;
    float value_108;
    int event_type;
    short value_110[8];
    int value_120;
    int locator_flags;
    float hearing_distance;
    short value_12c;
    short light_level;
    short default_motion_table[7];
    unsigned char unmodeled_7e[0x182];
    int value_2c0;
    int value_2c4;
    unsigned char unmodeled_208[0x18];
    short value_2e0[4];
} EnemyInitParams;

typedef struct EnemyInitWork {
    unsigned char unmodeled_00[0x4];
    short initial_motion_id;
    short locator_start_frame;
    short locator_end_frame;
    unsigned char unmodeled_0a[0x36];
    short spline_parameter;
    short fast_point_count;
    unsigned char unmodeled_44[0x4];
    signed char action;
    unsigned char unmodeled_49[0x1f];
    signed char marker_68;
    unsigned char unmodeled_69[0x1];
    unsigned char effect_state;
    unsigned char unmodeled_6b[0x1];
    short chase_frame_limit;
    short chase_frame;
    unsigned char unmodeled_70[0x4];
    short mark;
    short mark_timer;
    unsigned char unmodeled_78[0x8];
    float spline_points[256][4];
    short spline_point_count;
    unsigned char unmodeled_1082[0x100e];
    short action_motion_table[256][4];
    short motion_id_table[256][4];
    unsigned char unmodeled_3090[0x4];
    short route_point_count;
    unsigned char unmodeled_3096[0x40a];
    short route_value[64][4];
    short route_state[64];
    unsigned char unmodeled_3720[0x80];
    short state_37a0;
    unsigned char freeze_state;
    unsigned char turn_lock;
    short target_number;
    unsigned char unmodeled_37a6[0xe];
    int state_37b4;
    unsigned char unmodeled_37b8[0x18];
    float scale;
    unsigned char unmodeled_37d4[0x8];
    short scale_frame;
    unsigned char unmodeled_37de[0x2];
    int turn_flags;
    unsigned char unmodeled_37e4[0x3c];
    short turn_state_3820;
    short turn_state_3822;
    unsigned char unmodeled_3824[0x8];
    short turn_frame;
    short turn_duration;
    short encount_lock;
    unsigned char unmodeled_3832[0x6e];
    void *effect;
    unsigned char unmodeled_38a4[0x4];
    short applied_light_level;
    short light;
    short sense_block_timer;
    unsigned char unmodeled_38ae[0x2];
} EnemyInitWork;

typedef struct EnemyInitActor {
    unsigned int flags;
    void (*update)(struct EnemyInitActor *actor);
    unsigned char unmodeled_08[0x78];
    unsigned char number;
    unsigned char unmodeled_81[0x5];
    short id;
    unsigned char unmodeled_88[0x38];
    EnemyInitParams params;
    unsigned char unmodeled_2e8[0x1e0];
    unsigned char undulation[0x524];
    short lookat_timer;
    short lookat_target;
    unsigned char unmodeled_9f0[0x20];
    void *effects[8];
    short effect_interval;
    short effect_next;
    unsigned char unmodeled_a34[0x3c];
} EnemyInitActor;

extern EnemyInitActor actor[64];
/* GameLoopState[1] is the player's actor: ACT_createEnemy reads its number
 * through it (lw at 0x002d4280, lbu 0x80 at 0x002d4284). */
extern EnemyInitWork enepc[16];

EnemyInitActor *ACT_createEnemy(int number, int id);

#endif
