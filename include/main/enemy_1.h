#ifndef INCLUDE_MAIN_ENEMY_1_H
#define INCLUDE_MAIN_ENEMY_1_H

#include "common.h"
#include "shared.h"

/* Number of enemy work records (enepc) and of the actor slots they drive. */
#define ENEMY_COUNT 16

/* Size of the homing graph an enemy follows back to its route: positions and
 * the neighbours each one links to. */
#define ENEMY_HOMING_MAX 64
#define ENEMY_HOMING_NEIGHBOURS 4

/* map_index values of EnemyWorkEntry: ACT_updateEnemy picks the enemy one for
 * inUseId 0x4000..0x4f22 and the other for the remaining actors. */
#define ENEMY_MAP_INDEX_ENEMY 772
#define ENEMY_MAP_INDEX_NPC 768

/* EnemyWorkEntry.action: the behaviour ACT_updateEnemy dispatches each frame
 * (Enemy_ActionReady switches to one). */
#define ENEMY_ACT_INIT 0
#define ENEMY_ACT_PAUSE 1
#define ENEMY_ACT_THREAT 2
#define ENEMY_ACT_ROUTE 4
#define ENEMY_ACT_FOUND 5
#define ENEMY_ACT_CHASE 6
#define ENEMY_ACT_STIFF 7
#define ENEMY_ACT_UNIQUE 9
#define ENEMY_ACT_TENTACLE 10
#define ENEMY_ACT_DAMAGE_EXPLOSION 11
#define ENEMY_ACT_DAMAGE_ELECTRIC 12
#define ENEMY_ACT_DAMAGE_SEAL 13
#define ENEMY_ACT_ESCAPE 14
#define ENEMY_ACT_ROUTE_CHASE 15
#define ENEMY_ACT_READY 16 /* Enemy_Init's request, resolves to ENEMY_ACT_PAUSE */

/* EnemyWorkEntry.pause_kind (EnemyParams.move_kind): how the enemy moves
 * between pauses. */
#define ENEMY_MOVE_WANDER_A 1
#define ENEMY_MOVE_WANDER_B 2
#define ENEMY_MOVE_ROUTE_A 3
#define ENEMY_MOVE_ROUTE_BACK_AND_FORTH_A 4 /* becomes ENEMY_MOVE_ROUTE_A with a mirrored route */
#define ENEMY_MOVE_STAND_A 5
#define ENEMY_MOVE_STAND_B 6 /* both stand and may face the player */
#define ENEMY_MOVE_ROUTE_B 8
#define ENEMY_MOVE_ROUTE_BACK_AND_FORTH_B 9 /* becomes ENEMY_MOVE_ROUTE_B */
#define ENEMY_MOVE_ROUTE_THEN_ESCAPE 12     /* pause ends in ENEMY_ACT_ESCAPE */
#define ENEMY_MOVE_ROUTE_THEN_ROUTE_CHASE 13 /* pause ends in ENEMY_ACT_ROUTE_CHASE */

/* EnemyWorkEntry.freeze_state (Enemy_Command_Freeze). */
#define ENEMY_FREEZE_RELEASE 0 /* normal behaviour */
#define ENEMY_FREEZE_SCRIPTED 1 /* script drives the enemy, animation keeps its speed */
#define ENEMY_FREEZE_STOP 2    /* animation halted */
#define ENEMY_FREEZE_SLOW 3    /* animation at a fixed slow speed, sequence only */

/* EnemyWorkEntry.move_result: why TM_Check_Move refused a step. Values below
 * ACTOR_COUNT are the number of the actor in the way. */
#define ENEMY_MOVE_RESULT_FREE 99
#define ENEMY_MOVE_RESULT_NO_GROUND 0x40
#define ENEMY_MOVE_RESULT_TERRITORY_A 0x50 /* left the wall_attribute territory (state flag 8) */
#define ENEMY_MOVE_RESULT_TERRITORY_B 0x58
#define ENEMY_MOVE_RESULT_MAP_UNIT 0x60

/* Arguments of Enemy_Command_Target / Enemy_Command_LookAt: the player, or
 * (look-at only) no target. */
#define ENEMY_TARGET_PLAYER 100
#define ENEMY_LOOK_AT_RELEASE (-1)

/* Default motion kinds Get_DefaultMotion maps to an enemy's motion ids. */
#define ENEMY_DEFAULT_MOTION_IDLE 0
#define ENEMY_DEFAULT_MOTION_WALK 1
#define ENEMY_DEFAULT_MOTION_RUN 2
#define ENEMY_DEFAULT_MOTION_TALK 3 /* Before_Talk's motion */

/* EnemyWorkEntry.stiff_flag bits; Check_Encount turns them into the battle's
 * trigger flags. */
#define ENEMY_STIFF_ACTIVE_BIT 0x01
#define ENEMY_STIFF_ELECTRIC 0x2
#define ENEMY_STIFF_SEAL 0x4

/* EnemyWorkEntry.state_flags bits. */
#define ENEMY_STATE_FLAG_ENCOUNTER 0x1 /* touching the enemy can start a battle */
#define ENEMY_STATE_FLAG_BELT 0x4      /* carried by belt conveyors */
#define ENEMY_STATE_FLAG_TERRITORY_LOCK 0x8 /* chasing is the only way out of wall_attribute's territory */
#define ENEMY_STATE_FLAG_ALERT_RAISE 0x10   /* walks the route faster while alerted */
#define ENEMY_STATE_FLAG_AIRBORNE 0x20      /* follows the route's height instead of the ground */
#define ENEMY_STATE_FLAG_CUSTOM_BLEND 0x100 /* Set_Motion applies value_3858 as blend work */
#define ENEMY_STATE_FLAG_SCRIPT_ACTIVE 0x10000 /* a scripted move or turn is running */

/* Offsets added to RES_GetEnemySeBank(): the enemy's own sound effects. The
 * walk and run offsets also play as the looping sound of ENEMY_ACT_ROUTE and
 * ENEMY_ACT_CHASE; the _ALT ones are the second footstep of a stride. */
#define ENEMY_SE_IDLE 1
#define ENEMY_SE_WALK 2
#define ENEMY_SE_WALK_ALT 3
#define ENEMY_SE_RUN 4
#define ENEMY_SE_RUN_ALT 5

/* Looping sound effects of the ENEMY_ACT_DAMAGE_* states. */
#define SE_ID_DAMAGE_ELECTRIC 0x10008
#define SE_ID_DAMAGE_EXPLOSION 0x10009
#define SE_ID_DAMAGE_SEAL 0x1000b

#define ENEMY_LIGHT_LEVEL_FULL 1000 /* params->light_level of an opaque enemy */

/* Motion blend length Set_Motion / ACT_setMotion2 take for a normal change. */
#define MOTION_BLEND_DEFAULT 9

/* The doubleword-copied vectors of an enepc entry are DoublewordVector4, not
 * AlignedVector4: enepc and EnemyTarget (enemy_1.c) live in an 8-aligned .data
 * section, so their vectors are only 8-aligned, and the two-ld/sd copy comes
 * from the u64 pair. */
typedef struct EnemyEscapeTarget {
    union {
        Vector4 position;
        DoublewordVector4 position_pair;
    };
    float scale;
    float scale_from;
    float scale_to;
    short scale_frame;
    short scale_frame_count;
} EnemyEscapeTarget;

/* Enemy_Command_Sac_Move's timed move at +0x3800 of each entry: the original
 * stores two four-float vectors at +0x3800/+0x3810 and frame/duration
 * halfwords at +0x3820/+0x3822 (0x002d33b0..0x002d33f4); set_motion.c's
 * Script_Action walks it. */
typedef struct EnemySacMove {
    Vector4 start;
    Vector4 target;
    short frame;
    short duration;
} EnemySacMove;

/* Enemy_Command_Turn's timed rotation at +0x3824 of each entry: start and
 * target angle, elapsed frame count and the frame count the turn takes (see
 * Enemy_Command_Sac_Turn in src/main/enemy_2.c). */
typedef struct EnemyTurn {
    float start_angle;
    float target_angle;
    short frame;
    short duration;
} EnemyTurn;

/* The look-at target at +0x3860 that ACT_initMotion clears (control, point)
 * and look.c reads (its LookEnemyTargetSlot); Actor_LookAt points `point` at
 * the entry's lookat_position. */
typedef struct EnemyLookTarget {
    unsigned char unmodeled_00[8];
    float control;                          /* +0x08 */
    float *lookat_point;                    /* +0x0c */
} EnemyLookTarget;

/* The current head-turn angles at +0x3870 (one per axis, bounded by the
 * look_limits triples), cleared by ACT_initMotion. ACT_initMotion addresses
 * this record, look_target and look_limits as three separate sub-records. */
typedef struct EnemyLookAngles {
    float x;
    float y;
    float z;
} EnemyLookAngles;

/* The head-turn limits at +0x3880: ACT_initMotion points them at its static
 * minimum and maximum angle triples. */
typedef struct EnemyLookLimits {
    float *minimum;
    float *maximum;
} EnemyLookLimits;

/* One homing-graph position: Disp_Homing copies and marks it as a vector,
 * Homing_Search and Refresh_Homing read its coordinates. */
typedef union HomingPosition {
    Vector4 vector;
    float elements[4];
} HomingPosition;

typedef struct EnemyWorkEntry {
    float locator_distance;
    short initial_motion_id;
    short locator_start_frame;
    short locator_end_frame;
    short wall_attribute;
    float value_0c;
    union {
        struct {
            float goback_x;
            unsigned char unmodeled_014[4];
            float goback_z;
            unsigned char unmodeled_01c[4];
        };
        float goback[4];
    };
    Vector4 step_limit;
    union {
        struct {
            float drift_x;
            float fall_speed;
            float drift_z;
            unsigned char unmodeled_03c[4];
        };
        float drift[4];
    };
    short spline_parameter;
    short fast_point_count;
    unsigned short hit_count;
    short hit_limit;
    signed char action;
    signed char move_result;
    unsigned char pause_kind;
    unsigned char unmodeled_04b[1];
    unsigned short value_4c;
    unsigned short stiff_timer;
    /* Countdowns of stiff_flag bits 2 (electrified, Check_EnemyElec; its
     * sound is 0x10008) and 4 (sound 0x1000b), see ACT_updateEnemy. */
    short elec_timer;
    short status_4_timer;
    signed char route_marks[2][8];
    short route_cursor[2];
    signed char encounter_side;     /* +0x68: Check_Encount's side, set_motion.c */
    signed char encounter_value;    /* +0x69 */
    unsigned char stiff_flag;
    unsigned char unmodeled_06b;
    short chase_frame_limit;
    short chase_frame;
    int map_index;
    short mark;
    short mark_timer;
    unsigned char unmodeled_078[8];
    union {
        Vector4 spline_points[256];
        float spline_coordinates[256][4];
    };
    short spline_point_count;
    unsigned char unmodeled_1082[0xe];
    Vector4 route_points[256];
    short action_motion_table[256][4];
    short motion_id_table[256][4];
    short unique_pattern;
    unsigned short unique_step;
    short route_point_count;
    unsigned char unmodeled_3096[0xa];
    /* The homing graph Homing_Search walks (set_motion.c): positions,
     * up to four neighbours per position, their count and a linked target. */
    HomingPosition homing_positions[ENEMY_HOMING_MAX];    /* +0x30a0 */
    short homing_neighbours[ENEMY_HOMING_MAX][ENEMY_HOMING_NEIGHBOURS]; /* +0x34a0 */
    short homing_neighbour_count[ENEMY_HOMING_MAX];       /* +0x36a0 */
    short homing_linked_target[ENEMY_HOMING_MAX];         /* +0x3720 */
    short homing_count;
    signed char freeze_state;
    signed char turn_lock;
    short target_number;
    unsigned char unmodeled_37a6[2];
    int saved_motion;                       /* +0x37a8: Before_Talk keeps the talk motion here */
    unsigned char unmodeled_37ac[0xc];
    int event_type;
    unsigned char unmodeled_37bc[4];
    EnemyEscapeTarget escape_target;
    u32 state_flags;
    unsigned char unmodeled_37e4[0xc];
    DoublewordVector4 last_player_position;
    EnemySacMove sac_move;                  /* +0x3800 */
    EnemyTurn turn;                         /* +0x3824 */
    short encount_lock;
    unsigned char unmodeled_3832[0x22];
    float stiff_saved_speed;
    float value_3858;
    unsigned char unmodeled_385c[4];
    EnemyLookTarget look_target;            /* +0x3860 */
    EnemyLookAngles look_angles;            /* +0x3870 */
    unsigned char unmodeled_387c[4];
    EnemyLookLimits look_limits;            /* +0x3880 */
    unsigned char unmodeled_3888[8];
    Vector4 lookat_position;                /* +0x3890 */
    void *stiff_effect;
    unsigned char unmodeled_38a4[4];
    short applied_light_level;
    short casts_shadow;                     /* +0x38aa: Set_Shadow's test, set_motion.c */
    short sense_block_timer;
    unsigned char unmodeled_38ae[2];
} EnemyWorkEntry;

/* Enemy parameter record at Actor +0xc0. Member offsets are relative to +0xc0
 * (actor offset = +0xc0 + member offset); the record ends at +0x228, the end of
 * the largest view (EnemyParamsInitView in enemy_system_init.h). Embedded by the
 * TU-local Actor views of the same bytes: ActorEnemyWorkView (enemy_1.c),
 * ActorEnemyDetectView (set_motion.c, hearing), EncountActor (set_motion.c) and
 * ActorPauseView (pause_menu_page_enemy.h, parameters). Views that keep their own
 * declaration: EnemyParamsInitView (ACT_createEnemy, move_kind stored as a
 * halfword) and EnemyParamsActionView (enemy_3.c, idle/reaction motion ids).
 * A member whose readers disagree on width or signedness is not a union here:
 * GCC 2.96 schedules union-member accesses differently (measured on enemy_1.c),
 * so the TU keeps its own EnemyParams*View declaration instead. */
typedef struct EnemyParams {
    float route_step_length;                /* +0x00 */
    float chase_distance;                   /* +0x04 */
    float move_speed;                       /* +0x08: pause menu label "movement speed" */
    float chase_speed;                      /* +0x0c */
    float sight_angle;                      /* +0x10 */
    float sight_distance;                   /* +0x14 */
    short stiff_duration;                   /* +0x18: pause menu stiffness row */
    short pause_interval;                   /* +0x1a */
    short threat_duration;                  /* +0x1c: pause menu threat row */
    short threat_interval;                  /* +0x1e */
    short turn_duration;                    /* +0x20 */
    short gravity_mode;                     /* +0x22 */
    float locator_distance;                 /* +0x24 */
    short wall_attribute;                   /* +0x28 */
    unsigned char move_kind;                /* +0x2a: Enemy_Init loads one byte */
    unsigned char unmodeled_2b;
    int preset_index;                       /* +0x2c: -1 when no AdrsEnemy preset */
    float *route_values;                    /* +0x30 */
    int route_value_count;                  /* +0x34 */
    Vector4 *homing_values;                 /* +0x38 */
    int homing_value_count;                 /* +0x3c */
    float body_radius;                      /* +0x40 */
    int ex_motion_resource;                 /* +0x44: -1 means no ACT_initExMotion */
    float value_108;                        /* +0x48 */
    int event_type;                         /* +0x4c */
    short monster_set_numbers[8];           /* +0x50: Check_Encount picks one, -1 is empty */
    int value_120;                          /* +0x60 */
    int locator_flags;                      /* +0x64 */
    float hearing_distance;                 /* +0x68: Enemy_FindByEar radius */
    short encounter_event_flags;            /* +0x6c: low byte copied to cfEvent */
    short light_level;                      /* +0x6e */
    short default_motion_table[7];          /* +0x70: Get_DefaultMotion */
    unsigned char unmodeled_7e[0x182];
    int value_2c0;                          /* +0x200 */
    int value_2c4;                          /* +0x204 */
    unsigned char unmodeled_208[0x18];
    short linked_actors[4];                 /* +0x220: Enemy_After_Battle */
} EnemyParams;

/* Functions and data used by other translation units. */
extern void *AdrsEnemyExclamation;
extern void *AdrsEnemyQuestion;
extern void *AdrsEnemySphere;
extern DoublewordVector4 EnemyTarget;
/* Buffer addresses Enemy_SystemInit fills with xglCdReadFile, one per
 * enemy resource file (data\matumoto\enemy.dat, spline.dat, bikkuri.lex,
 * hatena.lex, maru.lex and sikaku.lex in that order). res_get_path.c reads
 * AdrsEnemyPreset back as the buffer Enemy_LoadPreset fills; this TU only
 * ever writes the six, so their pointee type stays unmodelled. */
extern void *AdrsEnemyPreset;
extern void *AdrsEnemySquare;
int Get_EffectCode(int attr);

#endif /* INCLUDE_MAIN_ENEMY_1_H */
