#include "common.h"
#include "shared.h"
#include "main/xgl_cd.h"
#include "enemy_system_init.h"
#include "main/enemy_system_init.h"
#include "main/act_1.h"
#include "main/enemy_1.h"

#define ENEMY_INITIAL_MOTION_ID 30

typedef struct EnemyPresetPathPrefix {
    char bytes[15];
} EnemyPresetPathPrefix;

typedef struct EnemyPresetPathParts {
    EnemyPresetPathPrefix prefix;
    char suffix[241];
} EnemyPresetPathParts;

typedef union EnemyPresetPathStorage {
    EnemyPresetPathParts parts;
    char bytes[256];
} EnemyPresetPathStorage;


extern const char enemyDatSuffix[];

/* GetBSplineLoop in main/tu201 indexes DataSpline from its .sdata slot. */
extern void *DataSpline[1];

extern void *AdrsEnemy;

extern char *strcat(char *destination, const char *source);
extern void ACT_updateEnemy(ActorEnemyInitView *actor);
extern void UnduParamInit(void *undulation);

const char enemyDataPath[24] = "data\\matumoto\\enemy.dat";

const char enemySplinePath[32] = "data\\matumoto\\spline.dat";

const char enemyBikkuriLexPath[32] = "data\\matumoto\\bikkuri.lex";

const char enemyHatenaLexPath[32] = "data\\matumoto\\hatena.lex";

const char enemyMaruLexPath[24] = "data\\matumoto\\maru.lex";

const char enemySikakuLexPath[32] = "data\\matumoto\\sikaku.lex";

const EnemyPresetPathPrefix enemyPresetPathPrefix = { "data\\matumoto\\" };

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

void Enemy_SystemInit(void) {
    AdrsEnemyPreset = WorkEnd;
    GameLoopState[0x1c / sizeof(ActorEnemyInitView *)] = 0;
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

ActorEnemyInitView *ACT_createEnemy(int number, int id)
{
    ActorEnemyInitView *enemy;
    EnemyParamsInitView *params;
    EnemyInitWork *work;
    short i;
    short j;
    short k;
    if (number < 0) {
        for (i = 0; i < ACTOR_COUNT; i++) {
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

    enemy->number = number;
    enemy->id = id;
    work = &enepc[enemy->number];
    params->route_step_length = 15.0f;
    params->preset_index = -1;
    params->route_value_count = 0;
    params->threat_duration = 20;
    params->stiff_duration = 30;
    params->pause_interval = 4;
    params->chase_distance = 10.0f;
    params->sight_distance = 4.0f;
    params->move_speed = 0.15f;
    params->chase_speed = 0.07f;
    params->turn_duration = 25;
    params->threat_interval = 4;
    params->wall_attribute = 0;
    work->route_point_count = 0;
    params->sight_angle = 90.0f;
    params->move_kind = 1;
    work->initial_motion_id = ENEMY_INITIAL_MOTION_ID;
    work->locator_start_frame = 5;
    params->hearing_distance = 10.0f;
    params->homing_value_count = 0;
    params->locator_distance = 4.0f;
    params->body_radius = 0.35f;
    work->homing_point_count = 0;
    params->value_108 = 1.0f;
    work->locator_end_frame = 25;
    params->ex_motion_resource = -1;
    params->value_120 = 0;
    params->event_type = -1;
    work->scale_frame = -1;
    work->scale = 1.0f;
    for (i = 0; i < ENEMY_HOMING_MAX; i++) {
        work->route_state[i] = 0;
        for (k = 0; k < 4; k++) {
            work->route_value[i][k] = -1;
        }
    }
    work->homing_point_count = 0;
    work->encounter_side = -1;
    work->spline_parameter = 0;
    work->fast_point_count = 0;
    work->chase_frame = 0;
    work->chase_frame_limit = 0;
    enemy->flags |= ACTOR_FLAG_COLLIDABLE;
    work->mark = 0;
    work->mark_timer = 0;
    enemy->flags |= 0x20000;
    enemy->flags |= 0x40000000;
    work->target_number = GameLoopState[1]->number;
    enemy->lookat_timer = 0;
    enemy->lookat_target = -1;
    params->default_motion_table[1] = MOTION_ID_WALK;
    params->default_motion_table[2] = MOTION_ID_RUN;
    params->default_motion_table[0] = MOTION_ID_IDLE;
    work->freeze_state = ENEMY_FREEZE_RELEASE;
    params->default_motion_table[4] = 0;
    params->default_motion_table[5] = 0;
    params->default_motion_table[3] = MOTION_BLEND_DEFAULT;
    params->default_motion_table[6] = 0;
    params->encounter_event_flags = 0;
    work->state_37b4 = 0;
    work->turn_flags = 0;
    work->turn_lock = 0;
    work->effect_state = 0;
    params->value_2c4 = 0;
    work->encount_lock = 0;
    params->value_2c0 = 0;
    params->locator_flags = 0;
    if ((unsigned short)enemy->id - 0x4000 >= 0x806U) {
        params->locator_flags = 0x1001b;
    }
    work->turn_frame = -1;
    work->movement_step = -1;
    work->turn_duration = 0;
    work->movement_duration = 0;
    UnduParamInit(enemy->undulation);
    work->applied_light_level = ENEMY_LIGHT_LEVEL_FULL;
    params->gravity_mode = 0;
    enemy->update = ACT_updateEnemy;
    enemy->flags |= 0x20;
    params->light_level = ENEMY_LIGHT_LEVEL_FULL;
    for (i = 0; i < 4; i++) {
        params->linked_actors[i] = -1;
    }
    work->effect = 0;
    for (i = 0; i < ACTOR_EFFECT_SLOT_COUNT; i++) {
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
        params->monster_set_numbers[i] = -1;
    }
    work->sense_block_timer = 0;
    work->action = ENEMY_ACT_INIT;
    AdrsEnemy = (char *)AdrsEnemyPreset + 4;
    return enemy;
}

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char enemyDatSuffix[8] = ".dat";
