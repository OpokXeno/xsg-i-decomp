#ifndef ENEMY_1_H
#define ENEMY_1_H

#include "common.h"
#include "shared.h"
#include "main/enemy_1.h"
#include "main/act_1.h"
#include "main/Undulate.h"
#include "main/act_2.h"
#include "main/check.h"
#include "main/init_drill.h"
#include "main/init_uwamono_sys.h"
#include "main/pause_menu_page_enemy.h"
#include "main/pause_menu_page_uwamono.h"
#include "main/sef.h"
#include "main/xgl_2.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"

#define ACTOR_FLAG_AILMENT_ACTIVE 0x00800000
#define ACTOR_FLAG_STIFF_ACTIVE   0x01000000
#define ACTOR_FLAG_FACE_PLAYER    0x00400000
#define ACTOR_FLAG_COLLIDABLE     0x00010000
#define ENEMY_STIFF_ACTIVE_BIT    0x01
#define ENEMY_ALERT_RAISE_FLAG    0x10
#define ENEMY_AIRBORNE_FLAG       0x20

typedef struct EnemyPreset {
    float route_step_length;
    float chase_distance;
    float value_c8;
    float chase_speed;
    float sight_angle;
    float sight_distance;
    float hearing_distance;
    float body_radius;
    float value_108;
    unsigned short value_d8;
    unsigned short pause_interval;
    unsigned short value_dc;
    unsigned short threat_interval;
    unsigned short value_2c;
    unsigned char move_kind;
    unsigned char unmodeled_2f;
    unsigned short chase_frame_limit;
    unsigned short initial_motion_id;
    float locator_distance;
    unsigned short locator_start_frame;
    unsigned short locator_end_frame;
    int state_flags;
    float value_3858;
    unsigned char unmodeled_44[0xc];
} EnemyPreset;

typedef struct EnemyMarkSet {
    void *none;
    void *exclamation;
    void *question;
    void *sphere;
} EnemyMarkSet;

typedef struct EnemyMapUnit {
    unsigned char unmodeled_000[0x300];
} EnemyMapUnit;

typedef struct EnemyGameLoopState {
    unsigned char unmodeled_00[4];
    struct Actor *player;
    unsigned char unmodeled_08[0x29f8c];
    short encount_reset;
    unsigned char unmodeled_29f96[0x9a];
} EnemyGameLoopState;

typedef struct EnemyResourceRecord {
    void *motion;
    unsigned char unmodeled_04[0xc];
} EnemyResourceRecord;

typedef struct EnemyPadView {
    unsigned char unmodeled_00[0x28];
    unsigned short held;
    unsigned short pressed;
} EnemyPadView;

extern EnemyMapUnit MapUnit[64];
extern long EneTerritoryID[20];
/* Head of the actor records as the functions that index actor[] directly
 * read it. Measured: GCC 2.96 gives their position accesses alias set 0 (the
 * original overlays the position vector with a float array and a doubleword
 * pair), which the canonical Actor.position (a plain vector shared with
 * other matched objects) cannot reproduce, so this TU indexes the array
 * through this view and hands Actor pointers to the rest. */
typedef struct EnemyActorHead {
    u32 flags;
    unsigned char unmodeled_04[0x0c];
    union {
        AlignedVector4 position;
        DoublewordVector4 position_pair;
        float position_f[4];
    };
    unsigned char unmodeled_20[0x62];
    unsigned char kind;
    unsigned char unmodeled_83[3];
    short inUseId;
    unsigned char unmodeled_88[0x9e4 - 0x88];
    float target_angle;
    float body_radius;
    unsigned char unmodeled_9ec[0xa70 - 0x9ec];
} EnemyActorHead;

extern EnemyActorHead actor[64];
extern EnemyWorkEntry enepc[16];
extern EnemyGameLoopState GameLoopState;
extern unsigned short CoolDown_0;
extern unsigned short CoolDown_1;
extern const float ESCAPE_RESET_DISTANCE;
extern const float ESCAPE_SPEED;
extern const float ROUTE_CHASE_RESET_DISTANCE;
extern const float ROUTE_CHASE_SPEED;
extern const char witnessedText[];
extern const char fieldObjectText[];
extern const char newText[];
extern const char goStraightText[];
extern const char addText[];
extern EnemyPreset *AdrsEnemy;
extern EnemyResourceRecord GameResource[128];
extern const char finalInitMethodName[];
extern EnemyPadView PadData;
extern short EfTab_Code[];
extern short EfTab_Intr[];
extern float EfTab_Scal[];
extern const char animTimeFormat[];
extern const char animStartFormat[];
extern const char animFinalFormat[];
extern const char animSpeedFormat[];
extern const char animNumberFormat[];
extern const char animBlendFormat[];
extern float ROUTE_NEAR_LOOKBACK_DISTANCE;
extern float ROUTE_HALF_TURN_RADIANS;
extern float ROUTE_DIRECTION_FULL_TURN;
extern const float ROUTE_FAR_LOOKBACK_DISTANCE;
extern float ROUTE_RANDOM_HALF_TURN_RADIANS;
extern float ROUTE_RETREAT_FULL_TURN;
extern const float ENEMY_DRIFT_DAMPING;

float Get_Angle(const Vector4 *from, const Vector4 *to);
float sinf(float x);
float cosf(float x);
int TM_Check_Move(Actor *self);
void Set_Spline_By_Route(Actor *actor, int route);
void Set_Spline_By_Random();
void BSpline_Init(Vector4 *points, const Vector4 *origin, float angle);
int Get_Rnd(int low, int high);

int Check_MoveSquare(float radius, const Vector4 *to, const Vector4 *from,
                     Vector4 *square, int map_index);

void TM_Check_Move_at_GoBack(Actor *actor);
int Get_Attr(const Vector4 *position, int map_index, int attr_mask);

int Check_Undu(Vector4 *start, Vector4 *end, int map_index, Vector4 *hit,
               int a, int b, int attr_mask);

int CheckBrokenMapUnit(EnemyMapUnit *unit);
int CrossCheckMapUnit(const Vector4 *from, const Vector4 *to);
void SendBrokenSignal(EnemyMapUnit *unit);
void Check_Encount(Actor *actor, int enable, signed char command);

unsigned char TM_Enemy_Move_Step(int actor_number,
                                 Vector4 *step_limit, float goback_x);

float Get_Distance(const Vector4 *from, const Vector4 *to);
float Get_Height(const Vector4 *position, int map_index, int attr_mask);

void Get_Point_By_AngleLength(const Vector4 *origin, Vector4 *point,
                              float angle, float length);

void GetBSplineLoop(short parameter, short point_count, int stride,
                    Vector4 *points, Vector4 *result);

short Get_DefaultMotion(Actor *actor, int mode);
void Set_Motion(Actor *actor, short motion, int mode);
void Enemy_ActionReady(Actor *actor, int action);
void Homing_Search(Actor *actor);
void *sefDeleteEffectCf(void *effect);
void Homing_Add(unsigned char number, float x, float y, float z, short value);
void CallMethod_I(const char *method, int value);
void UnduParamInit(void *undulation);

float UnduCheck(const Vector4 *position, void *velocity,
                void *undulation);

void GameIdLightSet(Actor *actor, void *query);
void ACT_updateSequence(Actor *actor);
void Actor_LookAt(Actor *actor);
void Check_Locater(Actor *actor);
void Script_Action(Actor *actor);
void Get_EnemyIDPos(const Vector4 *origin, Vector4 *destination);
void Start_Enemy_Command(Actor *actor);

int Check_InsideFan(float facing, float radius, float fan_width_degrees,
                    float crossing_radius, const Vector4 *origin,
                    const Vector4 *target, int map_index);

int Check_InsideFan_Wooo(float facing, float radius, float fan_width_degrees,
                         float crossing_radius, const Vector4 *origin,
                         const Vector4 *target, int map_index);

void Check_Discovery(Actor *actor);
void Sound_FootStep(Actor *actor);
int RES_GetEnemySeType(short sound_effect_id);
int RES_GetEnemySeBank(short sound_effect_id);
int SCRIPT_getCfTime(void);
void Move_BeltConveyer(Actor *actor);

float Get_Cursol_by_Reduce_Speed_Angle_Loop(float current, float target,
                                            float speed);

void Disp_EnemyMark(Actor *actor);
void Enemy_Init(Actor *self);
void Enemy_Pause(Actor *actor);
void Enemy_Threat(Actor *actor);
void Enemy_Route(Actor *self);
void Enemy_Found(Actor *actor);
void Enemy_Chase(Actor *self);
void Enemy_Tentacle(Actor *self);
void Enemy_Stiff(Actor *actor);
void Enemy_Unique(Actor *actor);
void Enemy_Damage_Explosion(Actor *actor);
void Enemy_Damage_Electric(Actor *actor);
void Enemy_Damage_Seal(Actor *actor);
void Enemy_Escape(Actor *actor);
void Enemy_Route_Chase(Actor *self);

void GetBSplineLoopDummy(short parameter, short point_count, int stride,
                         Vector4 *points, Vector4 *result, Actor *actor);

void Check_Wall(float scale, float radius, const Vector4 *origin,
                signed char directions[16], int status_count, int map_index,
                unsigned char number, int territory);

short Check_Straight(float radius, const Vector4 *from, const Vector4 *to,
                    int mask, Vector4 *hit, int map_index, int number,
                    int territory);

void BSpline_Add(short parameter, short point_count, short stride,
                 Vector4 *points, const Vector4 *point, short offset);

void Get_One_Step(float length, const Vector4 *source,
                  const Vector4 *destination, float *output);

float atan2f(float y, float x);

#endif
