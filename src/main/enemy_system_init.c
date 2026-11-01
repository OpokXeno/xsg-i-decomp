#include "common.h"

#include "shared.h"

#include "main/xgl_cd.h"

#include "enemy_system_init.h"

const char enemyDataPath[24] = "data\\matumoto\\enemy.dat";

const char enemySplinePath[32] = "data\\matumoto\\spline.dat";

const char enemyBikkuriLexPath[32] = "data\\matumoto\\bikkuri.lex";

const char enemyHatenaLexPath[32] = "data\\matumoto\\hatena.lex";

const char enemyMaruLexPath[24] = "data\\matumoto\\maru.lex";

const char enemySikakuLexPath[32] = "data\\matumoto\\sikaku.lex";

typedef struct EnemyPresetPathPrefix {
    char bytes[15];
} EnemyPresetPathPrefix;

const EnemyPresetPathPrefix enemyPresetPathPrefix = { "data\\matumoto\\" };

typedef struct EnemyPresetPathParts {
    EnemyPresetPathPrefix prefix;
    char suffix[241];
} EnemyPresetPathParts;

typedef union EnemyPresetPathStorage {
    EnemyPresetPathParts parts;
    char bytes[256];
} EnemyPresetPathStorage;

extern const char enemyDatSuffix[];

extern char *strcat(char *destination, const char *source);

/* GetBSplineLoop in main/tu201 indexes DataSpline from its .sdata slot. */

extern void *DataSpline[1];

extern void *AdrsEnemy;

extern void ACT_updateEnemy(EnemyInitActor *actor);

extern void UnduParamInit(void *undulation);

/* GetBSplineLoop in main/tu201 indexes DataSpline from its .sdata slot. */

void Enemy_SystemInit(void) {
    AdrsEnemyPreset = WorkEnd;
    GameLoopState[0x1c / sizeof(unsigned int)] = 0;
    WorkEnd += xglCdReadFile(enemyDataPath, WorkEnd, 0, 0);

    AdrsEnemySpline = WorkEnd;
    WorkEnd += xglCdReadFile(enemySplinePath, WorkEnd, 0, 0);

    AdrsEnemyExclamation = WorkEnd;
    WorkEnd += xglCdReadFile(enemyBikkuriLexPath, WorkEnd, 0, 0);

    AdrsEnemyQuestion = WorkEnd;
    WorkEnd += xglCdReadFile(enemyHatenaLexPath, WorkEnd, 0, 0);

    AdrsEnemySphere = WorkEnd;
    WorkEnd += xglCdReadFile(enemyMaruLexPath, WorkEnd, 0, 0);

    AdrsEnemySquare = WorkEnd;
    WorkEnd += xglCdReadFile(enemySikakuLexPath, WorkEnd, 0, 0);
}

int Enemy_LoadPreset(void *buffer, const char *preset_name)
{
    EnemyPresetPathStorage path;

    path.parts.prefix = enemyPresetPathPrefix;
    memset(path.parts.suffix, 0, sizeof(path.parts.suffix));
    strcat(path.bytes, preset_name);
    strcat(path.bytes, enemyDatSuffix);
    AdrsEnemyPreset = buffer;
    return xglCdReadFile(path.bytes, buffer, 0, 0);
}

void TM_Script_Spline_Add(int number, float x, float y, float z)
{
    short point = enepc[number].spline_point_count;

    enepc[number].spline_point_count++;
    enepc[number].spline_points[point][0] = x;
    enepc[number].spline_points[point][1] = y;
    enepc[number].spline_points[point][2] = z;
    enepc[number].spline_points[point][3] = 1.0f;
}

/* The enemy-parameter prefix shared by patrol, chase and action setup.
 * PauseMenuPageEnemy labels +0x08 as movement speed and +0x00 as movement
 * distance. Its stiffness and threat rows display the durations at
 * +0x18/+0x1c with their intervals at +0x1a/+0x1e. Enemy_ActionReady copies
 * those durations into the counter limit used by Enemy_Pause/Enemy_Threat.
 * The remaining
 * parameter record is outside this partial type. */
typedef struct EnemyBehaviorDefaults {
    float route_step_length;
    float chase_distance;
    float movement_speed;
    float chase_speed;
    float sight_angle;
    float sight_distance;
    short pause_duration;
    short pause_interval;
    short threat_duration;
    short threat_interval;
} EnemyBehaviorDefaults;

/* Encounter configuration shares the enemy's parameter record.
 * Check_Encount randomly selects one of the eight nonnegative monster-set
 * numbers at +0x50 for monsSetNo. It copies the low byte of +0x6c to
 * cfEvent, whose bit 0x40 GameModeCfMain tests after battle. The halfword
 * remains the original initialization width; its high byte is not yet
 * interpreted. Untouched gaps are outside the recovered field semantics. */
typedef struct EnemyEncounterParameters {
    EnemyBehaviorDefaults behavior;
    unsigned char unmodeled_20[0x50 - 0x20];
    short monster_set_numbers[8];
    unsigned char unmodeled_60[4];
    int locator_flags;
    float hearing_distance;
    short encounter_event_flags;
    short light_level;
} EnemyEncounterParameters;

/* Navigation state inside one enemy-work entry. Check_Encount stores
 * the facing-derived encounter side at +0x68. Homing_Add appends positions
 * at +0x30a0 and increments +0x37a0; Homing_Search walks those positions
 * using that count. Script_Action interpolates between the two movement
 * points using the step/duration pair at +0x3820/+0x3822. The gaps remain
 * uninterpreted storage, not inferred padding or new recovered fields. */
typedef struct EnemyNavigationWork {
    unsigned char unmodeled_00[0x68];
    signed char encounter_side;
    unsigned char unmodeled_69[0x30a0 - 0x69];
    float homing_points[64][4];
    short neighbours[64][4];
    short neighbour_count[64];
    short linked_target[64];
    short homing_point_count;
    unsigned char unmodeled_37a2[0x3800 - 0x37a2];
    float movement_points[2][4];
    short movement_step;
    short movement_duration;
} EnemyNavigationWork;

EnemyInitActor *ACT_createEnemy(int number, int id)
{
    EnemyInitActor *enemy;
    EnemyInitParams *params;
    EnemyBehaviorDefaults *behavior;
    EnemyEncounterParameters *encounter;
    EnemyInitWork *work;
    EnemyNavigationWork *navigation;
    short i;
    short j;
    short k;
    EnemyInitActor **loop_state;

    if (number < 0) {
        for (i = 0; i < 64; i++) {
            if (actor[i].id == 0) {
                number = i;
                break;
            }
        }
        if (number < 0) {
            return 0;
        }
    }

    enemy = &actor[number];
    params = &enemy->params;
    behavior = (void *)params;
    encounter = (void *)params;

    enemy->number = number;
    enemy->id = id;
    work = &enepc[enemy->number];
    navigation = (void *)work;
    behavior->route_step_length = 15.0f;
    params->preset_index = -1;
    params->route_value_count = 0;
    behavior->threat_duration = 20;
    behavior->pause_duration = 30;
    behavior->pause_interval = 4;
    behavior->chase_distance = 10.0f;
    behavior->sight_distance = 4.0f;
    behavior->movement_speed = 0.15f;
    behavior->chase_speed = 0.07f;
    params->value_e0 = 25;
    behavior->threat_interval = 4;
    params->wall_attribute = 0;
    work->route_point_count = 0;
    behavior->sight_angle = 90.0f;
    params->move_kind = 1;
    work->initial_motion_id = 30;
    work->locator_start_frame = 5;
    encounter->hearing_distance = 10.0f;
    params->homing_value_count = 0;
    params->locator_distance = 4.0f;
    params->body_radius = 0.35f;
    navigation->homing_point_count = 0;
    params->value_108 = 1.0f;
    work->locator_end_frame = 25;
    params->ex_motion_resource = -1;
    params->value_120 = 0;
    params->event_type = -1;
    work->scale_frame = -1;
    work->scale = 1.0f;
    for (i = 0; i < 64; i++) {
        work->route_state[i] = 0;
        for (k = 0; k < 4; k++) {
            work->route_value[i][k] = -1;
        }
    }
    navigation->homing_point_count = 0;
    navigation->encounter_side = -1;
    work->spline_parameter = 0;
    work->fast_point_count = 0;
    work->chase_frame = 0;
    work->chase_frame_limit = 0;
    loop_state = (EnemyInitActor **)GameLoopState;
    enemy->flags |= 0x10000;
    work->mark = 0;
    work->mark_timer = 0;
    enemy->flags |= 0x20000;
    enemy->flags |= 0x40000000;
    work->target_number = loop_state[1]->number;
    enemy->lookat_timer = 0;
    enemy->lookat_target = -1;
    params->default_motion_table[1] = 1;
    params->default_motion_table[2] = 3;
    params->default_motion_table[0] = 0;
    work->freeze_state = 0;
    params->default_motion_table[4] = 0;
    params->default_motion_table[5] = 0;
    params->default_motion_table[3] = 9;
    params->default_motion_table[6] = 0;
    encounter->encounter_event_flags = 0;
    work->state_37b4 = 0;
    work->turn_flags = 0;
    work->turn_lock = 0;
    work->effect_state = 0;
    params->value_2c4 = 0;
    work->encount_lock = 0;
    params->value_2c0 = 0;
    encounter->locator_flags = 0;
    if ((unsigned short)enemy->id - 0x4000 >= 0x806U) {
        encounter->locator_flags = 0x1001b;
    }
    work->turn_frame = -1;
    navigation->movement_step = -1;
    work->turn_duration = 0;
    navigation->movement_duration = 0;
    UnduParamInit(enemy->undulation);
    work->applied_light_level = 1000;
    params->gravity_mode = 0;
    enemy->update = ACT_updateEnemy;
    enemy->flags |= 0x20;
    encounter->light_level = 1000;
    for (i = 0; i < 4; i++) {
        params->value_2e0[i] = -1;
    }
    work->effect = 0;
    for (i = 0; i < 8; i++) {
        enemy->effects[i] = 0;
    }
    enemy->effect_interval = -1;
    enemy->effect_next = 0;
    DataSpline[0] = AdrsEnemySpline;
    work->light = 0;
    for (j = 0; j < 256; j++) {
        for (i = 0; i < 4; i++) {
            work->action_motion_table[j][i] = -1;
            work->motion_id_table[j][i] = 0;
        }
    }
    for (i = 0; i < 8; i++) {
        encounter->monster_set_numbers[i] = -1;
    }
    work->sense_block_timer = 0;
    work->action = 0;
    AdrsEnemy = (char *)AdrsEnemyPreset + 4;
    return enemy;
}

const char enemyDatSuffix[8] = ".dat";

/* JP: 初期化していない
 * EN: Not initialized */
const char PauseMenuPageEnemyNotInitializedText[17] = "\275\351\264\374\262\275\244\267\244\306\244\244\244\312\244\244";

/* JP: 警戒中：ポーズ
 * EN: On alert: pose */
const char PauseMenuPageEnemyAlertPoseText[15] = "\267\331\262\374\303\346\241\247\245\335\241\274\245\272";

/* JP: 警戒中：威嚇
 * EN: On alert: intimidation */
const char PauseMenuPageEnemyAlertIntimidationText[13] = "\267\331\262\374\303\346\241\247\260\322\263\305";

/* JP: 警戒中：回転
 * EN: On alert: turning */
const char PauseMenuPageEnemyAlertTurningText[13] = "\267\331\262\374\303\346\241\247\262\363\305\276";

/* JP: 警戒中：ルート移動
 * EN: On alert: moving along route */
const char PauseMenuPageEnemyAlertPatrolText[19] = "\267\331\262\374\303\346\241\247\245\353\241\274\245\310\260\334\306\260";

/* JP: ユニーク動作
 * EN: Unique action */
const char PauseMenuPageEnemyUniqueActionText[13] = "\245\346\245\313\241\274\245\257\306\260\272\356";

/* JP: 爆発ダメージ中
 * EN: Taking explosion damage */
const char PauseMenuPageEnemyExplosionDamageText[15] = "\307\372\310\257\245\300\245\341\241\274\245\270\303\346";

/* JP: 電撃ダメージ中
 * EN: Taking electric shock damage */
const char PauseMenuPageEnemyElectricDamageText[15] = "\305\305\267\342\245\300\245\341\241\274\245\270\303\346";

/* JP: 封印ダメージ中
 * EN: Taking seal damage */
const char PauseMenuPageEnemySealDamageText[15] = "\311\365\260\365\245\300\245\341\241\274\245\270\303\346";

/* JP: ルート上を逃亡
 * EN: Fleeing along route */
const char PauseMenuPageEnemyFleeRouteText[15] = "\245\353\241\274\245\310\276\345\244\362\306\250\313\264";

/* JP: ルート上を追跡
 * EN: Pursuing along route */
const char PauseMenuPageEnemyPursueRouteText[15] = "\245\353\241\274\245\310\276\345\244\362\304\311\300\327";
