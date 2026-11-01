#ifndef PAUSE_MENU_PAGE_ENEMY_H
#define PAUSE_MENU_PAGE_ENEMY_H

#define DEBUG_WELCOME_60FRAME_TEXT ("Welcome to the 60-Frame world!")
#define DEBUG_MODE_30FRAME_TEXT ("     30-Frame Debug Mode      ")
#define PAGE_HEADER_FORMAT ("\241\341\241\241Page %3d[%2d:%3d]\241\241\241\341")
#define ENEMY_MOVE_ID_FORMAT ("\241\241\260\334\306\260\243\311\243\304 [%3d] = &H%8x")
#define ENEMY_TYPE_FORMAT ("\245\277\245\244\245\327\241\241\241\241%2d:%s")
#define ENEMY_MODE_FORMAT ("\245\342\241\274\245\311\241\241\241\241%2d:%s (%3d/%3d)")
#define ENEMY_FREEZE_FORMAT ("\245\325\245\352\241\274\245\272\241\241%2d:%s")
#define ENEMY_HIDDEN_UNIT_FORMAT ("\261\243\244\354\244\306\244\244\244\353\245\346\245\313\245\303\245\310\310\326\271\346\241\241[%5d]")
#define ENEMY_HEIGHT_LOOK_AT_FORMAT ("\277\310\304\271\241\241\241\241[%8f]\241\241\273\353\300\376\241\277\301\352\274\352 [%2d,%2d]")
#define ENEMY_LOCATOR_ROW_FORMAT ("%2d:(%8f,%8f)%8f%s\n")
#define ENEMY_SOUND_SLOT_FORMAT ("\244\275\244\316 %1d = %3d\n")
#define ENEMY_MARK_KIND_FORMAT ("\274\357\316\340\241\241\241\241\241\241%2d:%s")
#define ENEMY_SPLINE_POINT_FORMAT ("%3d:(%8f,%8f,%8f)")
#define ENEMY_MOTION_FRAME_FORMAT ("%3d[%1d]:Motion = %3d / Frame = %3d")

#include "main/enemy_1.h"

#include "main/xgl_2.h"
#include "main/xgl_studio.h"

extern short Times;
#define PAUSE_MENU_PAGE_ENEMY_TWO_PI (6.2831854820251465f)

void xglLightCalcMatrix(StudioLight *light);
void nmlModelSetLight(Matrix4 colorMatrix, Matrix4 normalMatrix);
void nmlModelSetPlace(Matrix4 matrix);
void nmlModelEntry(int entry);

typedef struct EnemyParameters {
    float route_step_length;
    float chase_distance;
    float move_speed;
    float chase_speed;
    float sight_angle;
    float sight_distance;
    short stiff_duration;
    short pause_interval;
    short threat_duration;
    short threat_interval;
    short turn_duration;
    short gravity_mode;
    unsigned char unmodeled_24[0x50 - 0x24];
    short sound_effects[8];
    unsigned char unmodeled_60[4];
    int locator_flags;
    float hearing_distance;
    short unmodeled_6c;
    short light_level;
} EnemyParameters;

typedef struct Actor {
    unsigned int flags;
    void (*update)(struct Actor *actor);
    void (*draw)(struct Actor *actor);
    unsigned char unmodeled_0c[4];
    Vector4 position;
    Vector4 previous_position;
    Vector4 velocity;
    Vector4 acceleration;
    Vector4 rotation;
    Vector4 scale;
    Vector4 global_position;
    unsigned char number;
    unsigned char unmodeled_81;
    unsigned char kind;
    unsigned char unmodeled_83[3];
    short in_use_id;
    unsigned char unmodeled_88[0xc0 - 0x88];
    EnemyParameters parameters;
    unsigned char unmodeled_130[0x704 - 0x130];
    unsigned short motion_number;
    unsigned char unmodeled_706[0x9e4 - 0x706];
    float target_angle;
    float body_radius;
    short look_at_state;
    short look_at_target;
    unsigned char unmodeled_9f0[0xa70 - 0x9f0];
} Actor;

typedef struct EnemyScale {
    float current;
    float start;
    float target;
    short frame;
    short duration;
} EnemyScale;

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
    EnemyVector4 final_target;
    unsigned char unmodeled_3800[0x3858 - 0x3800];
    float freeze_saved_speed;
    unsigned char unmodeled_385c[0x3890 - 0x385c];
    float sight_position[3];
    unsigned char unmodeled_389c[0x38a8 - 0x389c];
    short applied_light_level;
    short light;
    unsigned char unmodeled_38ac[4];
} EnemyWork;

typedef struct GameLoopStateRecord {
    unsigned char unmodeled_00000[0x29fa0];
    signed char debug_directions[16];
    unsigned char debug_enabled[16];
    float debug_values[16];
    unsigned char unmodeled_2a000[0x30];
} GameLoopStateRecord;

typedef struct LayoutHeader {
    float components[4];
} LayoutHeader;

typedef struct UnduParam {
    int queryFlags;
    unsigned char unmodeled_04[4];
    short attrMask;
    unsigned char unmodeled_0a[0x18 - 0x0a];
    struct LayoutHeader *header;
    unsigned char unmodeled_1c[4];
    long long attribute;
} UnduParam;

extern Actor actor[64];

extern EnemyWork enepc[16];

extern short DebugActor;

extern short Page_0;

extern short SelNum;

extern signed char FLAG_FRAME_60;

extern PadPrefix PadData;

extern UnduParam UnduTemp;

extern unsigned long long EneTerritoryID[];

int Get_Attr(const Vector4 *position, int map_index, int mask);

LayoutHeader *UnduDataGetHeader(int map_index, int unit_index);

void UnduParamInit(UnduParam *param);

float UnduCheck(const Vector4 *position, void *exclude, UnduParam *param);

char Get_LocaterType(short index);

void Vibration_Set_Strong(int start, int end, int duration);

void xglFontDebugPrintf(int x, int y, const char *format, ...);

extern const char PauseMenuPageEnemyNotInitializedText[];

extern const char PauseMenuPageEnemyAlertPoseText[];

extern const char PauseMenuPageEnemyAlertIntimidationText[];

extern const char PauseMenuPageEnemyAlertTurningText[];

extern const char PauseMenuPageEnemyAlertPatrolText[];

extern const char PauseMenuPageEnemyUniqueActionText[];

extern const char PauseMenuPageEnemyExplosionDamageText[];

extern const char PauseMenuPageEnemyElectricDamageText[];

extern const char PauseMenuPageEnemySealDamageText[];

extern const char PauseMenuPageEnemyFleeRouteText[];

extern const char PauseMenuPageEnemyPursueRouteText[];

#define ENEMY_NORMAL_MODE_LABEL ("\245\316\241\274\245\336\245\353")

#define ENEMY_ROUTE_TENTACLE_MODE_LABEL ("\245\353\241\274\245\310\277\250\274\352")

#define ENEMY_ROUND_TRIP_TENTACLE_MODE_LABEL ("\261\375\311\374\277\250\274\352")

#define ENEMY_TENTACLE_GATE_MODE_LABEL ("\277\250\274\352\314\347\310\326")

#define ENEMY_ROUTE_CHASE_MODE_LABEL ("\245\353\241\274\245\310\304\311\300\327")

#define ENEMY_ALGORITHM_STOP_LABEL ("\245\242\245\353\245\264\245\352\245\272\245\340\304\344\273\337")

#define ENEMY_FULL_STOP_LABEL ("\264\260\301\264\304\344\273\337")

#define ENEMY_FULL_STOP_CHR_LABEL ("\264\260\301\264\304\344\273\337\241\312\243\303\243\310\243\322\267\277\273\310\315\321\262\304\307\275\241\313")

#define ENEMY_SURPRISE_MARK_LABEL ("\244\323\244\303\244\257\244\352\245\336\241\274\245\257")

#define ENEMY_QUESTION_MARK_LABEL ("\244\317\244\306\244\312\245\336\241\274\245\257")

extern const char discoveredText[];

extern const char chasingText[];

extern const char stiffenText[];

extern const char returnHomeText[];

extern const char tentacleActiveText[];

extern const char pauseMenuPageEnemyNullText[];

extern const char tentacleText[];

extern const char guardText[];

extern const char gatekeeperText[];

extern const char routeText[];

extern const char roundTripText[];

extern const char wallText[];

extern const char floatText[];

extern const char fleeText[];

extern const char locatorNullText[];

extern const char locatorSlashBText[];

extern const char locatorSlashTText[];

extern const char locatorParenBText[];

extern const char locatorBracketBActiveText[];

extern const char locatorBracketBText[];

extern const char locatorBracketTText[];

extern const char locatorNoneText[];

extern const char directionNorthUpText[];

extern const char directionEastRightText[];

extern const char directionSouthDownText[];

extern const char directionWestLeftText[];

extern const char freezeNormalText[];

#define BOOLEAN_NO_LABEL ("\244\244\244\244\244\250")

#define BOOLEAN_YES_LABEL ("\244\317\244\244")

#define ENEMY_STRAIGHT_LINE_LABEL ("\304\276\300\376")

#define ENEMY_SUSPECTED_FACE_TEXT ("\264\351\244\310\273\327\244\357\244\354\244\353")

#define ENEMY_MOVE_HEADING_TEXT ("\241\371\260\334\306\260\241\371")

#define ENEMY_CURRENT_POSITION_FORMAT ("\270\275\272\337\303\317(%8f,%8f,%8f)")

#define ENEMY_PLANNED_OFFSET_FORMAT ("\315\275\304\352\316\314(%8f,%8f,%8f)")

#define ENEMY_TARGET_POINT_FORMAT ("\314\334\311\270\305\300(%8f,%8f,%8f)")

#define ENEMY_INERTIA_FORMAT ("\264\267\300\255\241\241(%8f,%8f,%8f)")

#define ENEMY_ROTATION_ANGLE_FORMAT ("\262\363\305\276 [%8f] -> [%8f]")

#define ENEMY_MOVE_SPEED_DISTANCE_FORMAT ("\260\334\306\260\302\256\305\331[%8f] \260\334\306\260\265\367\316\245[%8f]")

#define ENEMY_SIGHT_DISTANCE_ANGLE_FORMAT ("\272\367\305\250\265\367\316\245[%8f] \272\367\305\250\263\321\305\331[%8f]")

#define ENEMY_CHASE_SPEED_HEARING_FORMAT ("\304\311\300\327\302\256\305\331[%8f] \241\241\262\273\265\367\316\245[%8f]")

#define ENEMY_GIVE_UP_DISTANCE_TIME_FORMAT ("\304\374\244\341\265\367\316\245[%8f] \304\374\244\341\273\376\264\326[%5d]")

#define ENEMY_FINAL_TARGET_SIZE_FORMAT ("\272\307\275\252\314\334\311\270[%8f] \245\265\245\244\245\272\241\241[%8f]")

#define ENEMY_AREA_ID_FORMAT ("\245\250\245\352\245\242\243\311\243\304 [%3d] = &H%8x")

#define ENEMY_ATTRIBUTE_ID_FORMAT ("\241\241\241\241\245\242\245\310\245\352 [%3d] = &H%8x")

#define ENEMY_TITLE_TEXT ("\245\250\245\315\245\337\241\274")

#define ENEMY_NPC_LABEL ("\243\316\243\320\243\303")

#define ENEMY_STATE_HEADING_TEXT ("\241\371\276\365\302\326\241\371")

#define ENEMY_STOP_CAUSE_FORMAT ("\304\344\273\337\270\266\260\370\241\241(%8f,%8f,%8f)")

#define ENEMY_COUNTER_HEADER_TEXT ("      \242\247\245\325\245\354\241\274\245\340\277\364\241\241\242\247\311\321\305\331")

#define ENEMY_RIGID_COUNTERS_FORMAT ("\271\305\304\276\241\241\241\241[%4d]\241\241\241\241\241\241[%4d]")

#define ENEMY_THREAT_COUNTERS_FORMAT ("\260\322\263\305\241\241\241\241[%4d]\241\241\241\241\241\241[%4d]")

#define ENEMY_TURN_COUNT_FORMAT ("\262\363\305\276\241\241\241\241     \241\241\241\241\241\241[%4d]")

#define ENEMY_EXPLOSION_COUNT_FORMAT ("\245\253\245\246\245\363\245\277\241\247\307\372\310\257\241\241%5d")

#define ENEMY_ELECTRIC_COUNT_FORMAT ("\241\241\241\241\241\241\241\241\241\241\305\305\267\342\241\241%5d")

#define ENEMY_FALL_COUNT_FORMAT ("\241\241\241\241\241\241\241\241\241\241\315\356\262\274\241\241%5d")

#define ENEMY_SIGHT_POSITION_FORMAT ("\273\353\300\376\241\241(%8f,%8f,%8f)")

#define ENEMY_FLAGS_HEADING_TEXT ("\241\371\245\325\245\351\245\260\241\371")

#define ENEMY_MOTION_FLAG_FORMAT ("\245\342\241\274\245\267\245\347\245\363\241\241\241\241\241\241\241\241%2d:%s")

#define ENEMY_TURN_FLAG_FORMAT ("\262\363\305\276\241\241\241\241\241\241\241\241\241\241\241\241\241\241%2d:%s")

#define ENEMY_HEAD_SWING_FLAG_FORMAT ("\274\363\277\266\244\352\241\241\241\241\241\241\241\241\241\241\241\241%2d:%s")

#define ENEMY_CANCELABLE_FLAG_FORMAT ("\245\255\245\343\245\363\245\273\245\353\262\304\241\241\241\241\241\241%2d:%s")

#define ENEMY_FREEZE_FLAG_FORMAT ("\245\325\245\352\241\274\245\272\241\241\241\241\241\241\241\241\241\241%2d:%s")

#define ENEMY_JAVA_TALK_FLAG_FORMAT ("\243\312\243\301\243\326\243\301\245\310\241\274\245\257\241\241\241\241%2d:%s")

#define ENEMY_TOUCH_FLAG_FORMAT ("\241\241\241\241\241\241\241\241\245\277\245\303\245\301\241\241\241\241%2d:%s")

#define ENEMY_LOCATOR_FLAG_FORMAT ("\241\241\241\241\241\241\241\241\245\355\245\261\241\274\245\277\241\241%2d:%s")

#define ENEMY_TENTACLE_HEADING_TEXT ("\241\371\277\250\274\352\241\371")

#define ENEMY_TENTACLE_RANGE_FORMAT ("\277\250\274\352\274\315\304\370\241\241\241\241\241\241[%8f]")

#define ENEMY_TENTACLE_DURATION_FORMAT ("\277\250\274\352\264\374\264\326\241\241\241\241\241\241[%8d]")

#define ENEMY_CHECK_START_END_FORMAT ("\310\275\304\352\263\253\273\317\241\301\275\252\316\273[%8d-%8d]")

#define ENEMY_SCALE_HEADING_TEXT ("\241\371\245\271\245\261\241\274\245\353\241\277\310\276\306\251\314\300\241\277\244\275\244\316\302\276\241\371")

#define ENEMY_CURRENT_SCALE_FORMAT ("\245\312\245\246\244\312\245\271\245\261\241\274\245\353\241\241%8f")

#define ENEMY_COUNTER_FORMAT ("\245\253\245\246\245\363\245\277\241\241\241\241\241\241\241\241%3d/ %3d")

#define ENEMY_SCALE_TRANSITION_FORMAT ("\245\271\245\261\241\274\245\353\277\344\260\334\241\241\241\241%8f->%8f")

#define ENEMY_ALPHA_TRANSITION_FORMAT ("\306\251\314\300\305\331\241\241\241\241\241\241\241\241\241\241%8d->%8d")

#define ENEMY_TRAITS_HEADING_TEXT ("\241\371\306\303\304\247\241\371")

#define ENEMY_ENCOUNTER_FORMAT ("\245\250\245\363\245\253\245\246\245\363\245\310\241\241%2d:%s")

#define ENEMY_CONVEYOR_FORMAT ("\245\263\245\363\245\331\245\242\241\241\241\241\241\241%2d:%s")

#define ENEMY_OUTING_FORMAT ("\263\260\275\320\241\241\241\241\241\241\241\241\241\241%2d:%s")

#define ENEMY_MOVE_TYPE_FORMAT ("\260\334\306\260\245\277\245\244\245\327\241\241\241\241%2d:%s")

#define ENEMY_FLIGHT_FORMAT ("\310\364\271\324\241\241\241\241\241\241\241\241\241\241%2d:%s")

#define ENEMY_DESTROY_FORMAT ("\307\313\262\365\241\241\241\241\241\241\241\241\241\241%2d:%s")

#define ENEMY_DEFORM_FORMAT ("\312\321\267\301\241\241\241\241\241\241\241\241\241\241%2d:%s")

#define ENEMY_CLIPPING_FORMAT ("\245\257\245\352\245\303\245\324\245\363\245\260\241\241%2d:%s(%8f)")

#define ENEMY_ID_DISABLED_FORMAT ("\243\311\243\304\314\265\270\372\241\241\241\241\241\241%2d:%s")

#define ENEMY_ROTATION_LOCK_FORMAT ("\262\363\305\276\266\330\273\337\241\241\241\241\241\241%2d:%s")

#define ENEMY_MOTION_TRANSITION_FORMAT ("\245\342\241\274\245\267\245\347\245\363\241\241\241\241%2d -> \260\354\300\316\301\260\241\247%2d")

#define ENEMY_CHASE_TARGET_FORMAT ("\304\311\300\327\302\320\276\335[%2d(%6f,%6f,%6f)]")

#define ENEMY_FLAGS_LABEL ("\245\325\245\351\245\260\241\241\241\241\241\241\241\241:")

#define ENEMY_RETURN_HOME_FORMAT ("\265\242\301\343\303\346\241\241\241\241\241\241\241\241%2d:%s")

#define ENEMY_LOCATOR_HEADING_TEXT ("\241\371\245\355\245\261\241\274\245\277\241\371")

#define ENEMY_LADDER_HEADING_TEXT ("\241\371\245\317\245\267\245\264\244\316\310\353\314\251\241\371")

#define ENEMY_COLUMN_DIRECTION_FORMAT ("%2d:%s[%8f m]%s\n")

#define ENEMY_CIRCLE_HEADING_TEXT ("\241\371\244\260\244\353\241\371")

#define ENEMY_SATELLITE_HEADING_TEXT ("\241\371\245\265\245\306\245\351\245\244\245\310\241\371")

#define ENEMY_COUNTER_RANGE_FORMAT ("\245\253\245\246\245\363\245\277\241\241[%5d/%5d]")

#define ENEMY_SPLINE_FIRST_HEADING_TEXT ("\241\371\245\271\245\327\245\351\245\244\245\363\243\261\241\371")

#define ENEMY_SPLINE_SUMMARY_FORMAT ("\245\253\245\246\245\363\245\277\241\241[%5d/%5d]\241\241\303\346\267\321\305\300\277\364 [%5d]")

#define ENEMY_WINDUP_DURATION_FORMAT ("\315\275\310\367\306\260\272\356\264\374\264\326\241\241\241\241%8d")

#define ENEMY_SPLINE_PLAN_HEADING_TEXT ("\241\371\245\271\245\327\245\351\245\244\245\363\243\262\241\277\272\243\270\345\244\316\315\275\304\352\241\371")

#define ENEMY_ROUTE_ORDER_FORMAT ("\245\353\241\274\245\310\241\241\241\241[%5d]    \241\241\271\324\306\260\275\347\275\370 [%5d]")

#define ENEMY_PAUSE_EVENT_HEADING_TEXT ("\241\371\245\244\245\331\245\363\245\310\241\312\245\335\241\274\245\272\241\313\241\371")

#define ENEMY_THREAT_EVENT_HEADING_TEXT ("\241\371\245\244\245\331\245\363\245\310\241\312\260\322\263\305\241\313\241\371")

#define DEBUG_RETURN_TOOL_TITLE_TEXT ("\241\371\265\242\244\352\306\273\300\337\304\352\245\304\241\274\245\353\241\326\244\252\265\242\244\352\244\317\244\263\244\301\244\351\241\327Ver0.1\241\371")

#define ENEMY_ROW_POINT_FORMAT ("%2d:(%4f,%4f)")

#define ENEMY_STATUS_ROW_FORMAT ("%3d[%s]")

#define ENEMY_ROW_INDEX_FORMAT ("%2d:")

#define ENEMY_NONE_LABEL ("--")

extern const char selectCursorText[];

extern const char downText[];

extern GameLoopStateRecord GameLoopState;


#endif
