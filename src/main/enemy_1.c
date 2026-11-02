#include "common.h"
#include "main/enemy_1.h"
#include "enemy_1.h"

/* Floor attribute bits that select an entry of EfTab_* (footstep effects). */
#define GROUND_EFFECT_COUNT 15
#define GROUND_EFFECT_ATTR_MASK 0x8f00

#define ENEMY_ROUTE_MARK_COUNT 8
#define ENEMY_WALL_DIRECTION_COUNT 16 /* compass directions Check_Wall reports */
#define ENEMY_RANGE_CIRCLE_SEGMENTS 64
#define ENEMY_CONTACT_PADDING 0.3f

#define ENEMY_CONTACT_HEIGHT 1.9f
#define ENEMY_FALL_ACCEL -0.02f
#define INSIDE_FAN_WOOO_ANGLE_OFFSET 3.1415927f
#define INSIDE_FAN_ANGLE_OFFSET 3.1415927f
#define ENEMY_HIT_PADDING 0.3f
#define ENEMY_HIT_HEIGHT 1.9f
#define ENEMY_AIR_FALL_ACCEL -0.02f
#define ENEMY_CHASE_MIN_DISTANCE 0.1f
#define ESCAPE_TARGET_POSITION_EPSILON 0.01f
#define ESCAPE_HEIGHT_FOLLOW_RATE 0.01f
#define CHASE_LOST_DISTANCE 0.01f
#define LAST_PLAYER_REACHED_DISTANCE 0.01f

/* Distance thresholds and retreat speeds used by the two look-back paths.
 * The original has no named objects for them: cc1 emits each use as an
 * anonymous .lit4 literal (0x004D8034..0x004D8040). */
#define ESCAPE_RESET_DISTANCE 0.012f

#define ESCAPE_SPEED 0.06666667f
#define ROUTE_CHASE_RESET_DISTANCE 0.012f
#define ROUTE_CHASE_SPEED 0.06666667f

/* Distance thresholds and retreat speeds used by the two look-back paths. */
#define ROUTE_NEAR_LOOKBACK_DISTANCE 0.01f

#define ROUTE_HALF_TURN_RADIANS 3.1415927f
#define ROUTE_DIRECTION_FULL_TURN 6.2831855f
#define ROUTE_FAR_LOOKBACK_DISTANCE 0.12f
#define ROUTE_RANDOM_HALF_TURN_RADIANS 3.1415927f
#define ROUTE_RETREAT_FULL_TURN 6.2831855f
#define ENEMY_DRIFT_DAMPING 0.9f

DoublewordVector4 EnemyTarget ={ .vector = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 0.0f } };

/* Per-actor work records are cleared before use. */
EnemyWorkEntry enepc[16] = {{0}};

/* JP: 目撃
 * EN: Witnessed */
const char witnessedText[5] = "\314\334\267\342";

/* JP: 上物
 * EN: Field object */
const char fieldObjectText[5] = "\276\345\312\252";

/* JP: 新規
 * EN: New */
const char newText[5] = "\277\267\265\254";

/* JP: 直進
 * EN: Go straight */
const char goStraightText[5] = "\304\276\277\312";

/* JP: 追加
 * EN: Add */
const char addText[5] = "\304\311\262\303";

static unsigned short CoolDown_0 = 0;

static unsigned short CoolDown_1 = 0;

void *AdrsEnemyPreset = 0;

void *AdrsEnemySpline = 0;

void *AdrsEnemyExclamation = 0;

void *AdrsEnemyQuestion = 0;

void *AdrsEnemySphere = 0;

void *AdrsEnemySquare = 0;

EnemyPreset * AdrsEnemy = 0;

/* Partial representation of the original two-byte object. No original consumer
 * establishes its signedness or semantic type; this scalar preserves zero bits. */
u16 MostNearActor = 0;

/* Partial representation of the original four-byte object. Its semantic type
 * remains unknown; no numeric, floating-point or pointer interpretation is claimed. */
u32 MostNear = 0;

int Get_EffectCode(int attr)
{
    int effect[GROUND_EFFECT_COUNT] = {
        0x8100, 0x8200, 0x8300, 0x8400, 0x8500, 0x8600, 0x8700, 0x8800,
        0x8900, 0x8a00, 0x8b00, 0x8c00, 0x8d00, 0x8e00, 0x8f00,
    };
    short i;

    attr &= GROUND_EFFECT_ATTR_MASK;
    for (i = 0; i < GROUND_EFFECT_COUNT; i++) {
        if (attr == effect[i]) {
            return i;
        }
    }
    return -1;
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char finalInitMethodName[11] = "Final_init";

void Enemy_Init(Actor *self)
{
    EnemyParams *params = &self->enemy;
    EnemyWorkEntry *work = &enepc[self->number];
    short i;
    short j;
    unsigned char kind;

    if (params->preset_index == -1) {
        work->initial_motion_id = 30;
        work->locator_start_frame = 5;
        work->locator_end_frame = 25;
        {
            float distance = params->locator_distance;

            work->pause_kind = params->move_kind;
            work->locator_distance = distance;
        }
        self->body_radius = params->body_radius;
        work->value_0c = params->value_108;
    } else {
        params->stiff_duration = AdrsEnemy[params->preset_index].value_d8;
        params->route_step_length = AdrsEnemy[params->preset_index].route_step_length;
        params->pause_interval = AdrsEnemy[params->preset_index].pause_interval;
        params->chase_distance = AdrsEnemy[params->preset_index].chase_distance;
        params->threat_duration = AdrsEnemy[params->preset_index].value_dc;
        params->move_speed = AdrsEnemy[params->preset_index].value_c8;
        params->threat_interval = AdrsEnemy[params->preset_index].threat_interval;
        params->chase_speed = AdrsEnemy[params->preset_index].chase_speed;
        work->value_4c = AdrsEnemy[params->preset_index].value_2c;
        params->sight_angle = AdrsEnemy[params->preset_index].sight_angle;
        work->initial_motion_id = AdrsEnemy[params->preset_index].initial_motion_id;
        params->sight_distance = AdrsEnemy[params->preset_index].sight_distance;
        work->locator_start_frame = AdrsEnemy[params->preset_index].locator_start_frame;
        params->hearing_distance = AdrsEnemy[params->preset_index].hearing_distance;
        work->locator_end_frame = AdrsEnemy[params->preset_index].locator_end_frame;
        work->locator_distance = AdrsEnemy[params->preset_index].locator_distance;
        work->pause_kind = AdrsEnemy[params->preset_index].move_kind;
        self->body_radius = AdrsEnemy[params->preset_index].body_radius;
        work->value_0c = AdrsEnemy[params->preset_index].value_108;
        work->state_flags = AdrsEnemy[params->preset_index].state_flags;
        work->chase_frame_limit = AdrsEnemy[params->preset_index].chase_frame_limit;
        work->value_3858 = AdrsEnemy[params->preset_index].value_3858;
    }
    work->wall_attribute = params->wall_attribute;
    work->event_type = params->event_type;
    self->target_angle = self->rotation.y;
    if (work->route_point_count == 0) {
        work->route_point_count = params->route_value_count / 3;
        for (i = 0; i < work->route_point_count; i++) {
            Vector4 *point = &work->route_points[i];
            float *dest = &point->z;

            dest[-2] = params->route_values[i * 3];
            dest[-1] = params->route_values[i * 3 + 1];
            dest[0] = params->route_values[i * 3 + 2];
        }
    }
    if (work->pause_kind == ENEMY_MOVE_ROUTE_BACK_AND_FORTH_B ||
        work->pause_kind == ENEMY_MOVE_ROUTE_BACK_AND_FORTH_A) {
        for (i = 0; i < work->route_point_count - 2; i++) {
            work->route_points[work->route_point_count + i].x =
                work->route_points[work->route_point_count - i - 2].x;
            *(&work->route_points[work->route_point_count + i].z) =
                *(&work->route_points[work->route_point_count - i - 2].z);
        }
        work->route_point_count = work->route_point_count * 2 - 2;
        if (work->pause_kind == ENEMY_MOVE_ROUTE_BACK_AND_FORTH_B)
            work->pause_kind = ENEMY_MOVE_ROUTE_B;
        if (work->pause_kind == ENEMY_MOVE_ROUTE_BACK_AND_FORTH_A)
            work->pause_kind = ENEMY_MOVE_ROUTE_A;
    }
    for (j = 0; j < work->route_point_count; j++) {
        for (i = 0; i < 4; i++) {
            work->action_motion_table[j][i] = -1;
            work->motion_id_table[j][i] = 0;
        }
    }
    if (work->homing_count == 0) {
        if (params->homing_value_count != 0) {
            work->homing_count = 0;
            for (i = 0; i < params->homing_value_count / 4; i++) {
                Homing_Add(self->number, params->homing_values[i].x,
                           params->homing_values[i].y,
                           params->homing_values[i].z,
                           (short)params->homing_values[i].w);
            }
        }
    }
    kind = work->pause_kind;
    if (kind != ENEMY_MOVE_ROUTE_B && kind != ENEMY_MOVE_ROUTE_A &&
        kind != ENEMY_MOVE_ROUTE_THEN_ESCAPE && kind != ENEMY_MOVE_ROUTE_THEN_ROUTE_CHASE)
        Set_Spline_By_Random(self);
    else
        Set_Spline_By_Route(self, 0);
    for (i = 0; i < ENEMY_ROUTE_MARK_COUNT; i++) {
        work->route_marks[1][i] = 0;
        work->route_marks[0][i] = 0;
    }
    work->route_cursor[0] = 0;
    work->route_cursor[1] = 0;
    if (params->pause_interval != 0)
        work->route_marks[0][Get_Rnd(0, params->pause_interval) % ENEMY_ROUTE_MARK_COUNT] = 1;
    if (params->threat_interval != 0)
        work->route_marks[1][Get_Rnd(0, params->threat_interval) % ENEMY_ROUTE_MARK_COUNT] = 1;
    if (self->inUseId >= 0x4000 && self->inUseId <= 0x4f22)
        work->map_index = 772;
    else
        work->map_index = 768;
    if (params->ex_motion_resource != -1) {
        ACT_initExMotion(self, 0,
                         GameResource[params->ex_motion_resource].motion);
    }
    Enemy_ActionReady(self, ENEMY_ACT_READY);
    work->hit_limit = 0;
    if (GameLoopState.player == self)
        CallMethod_I(finalInitMethodName, 100);
    else
        CallMethod_I(finalInitMethodName, work->event_type);
    UnduParamInit(&self->undulation);
    self->status_flags = 2;
    UnduCheck(&self->position, 0, &self->undulation);
    self->status_flags |= 0x1000;
}

void Enemy_Damage_Explosion(Actor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    unsigned short count;
    short limit;
    int stiff;

    work->goback_x = 0.0f;
    work->goback_z = 0.0f;
    TM_Enemy_Move_Step(actor->number, &work->step_limit, work->goback_x);
    actor->target_angle = Get_Angle(&actor->position,
                                    &GameLoopState.player->position);

    count = work->hit_count;
    limit = work->hit_limit;
    work->hit_count = count + 1;
    if ((short)count >= limit) {
        actor->flags &= ~ACTOR_FLAG_AILMENT_ACTIVE;
        stiff = work->stiff_flag;
        if (stiff & ENEMY_STIFF_ACTIVE_BIT)
            Enemy_ActionReady(actor, ENEMY_ACT_STIFF);
        else
            Enemy_ActionReady(actor, ENEMY_ACT_CHASE);
    }
}

void Enemy_Damage_Electric(Actor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    int count = work->hit_count + 1;
    short limit = work->hit_limit;

    work->hit_count = count;
    if ((short)count >= limit) {
        u32 flags = actor->flags;

        work->spline_parameter = 0;
        actor->flags = flags & ~ACTOR_FLAG_AILMENT_ACTIVE;
        Homing_Search(actor);
        Enemy_ActionReady(actor, ENEMY_ACT_THREAT);
    }
}

void Enemy_Damage_Seal(Actor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    int count = work->hit_count + 1;
    short limit = work->hit_limit;

    work->hit_count = count;
    if ((short)count >= limit) {
        u32 flags = actor->flags;

        work->spline_parameter = 0;
        actor->flags = flags & ~ACTOR_FLAG_AILMENT_ACTIVE;
        Homing_Search(actor);
        Enemy_ActionReady(actor, ENEMY_ACT_PAUSE);
    }
}

void Enemy_Stiff(Actor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    int timer = work->stiff_timer - 1;

    work->stiff_timer = timer;
    if ((short)timer <= 0) {
        work->stiff_flag &= ~ENEMY_STIFF_ACTIVE_BIT;
        work->stiff_timer = 0;
        actor->animation.slot.speed = work->stiff_saved_speed;
        actor->flags &= ~ACTOR_FLAG_STIFF_ACTIVE;
        sefDeleteEffectCf(work->stiff_effect);
        work->stiff_effect = 0;
        Homing_Search(actor);
        Enemy_ActionReady(actor, ENEMY_ACT_THREAT);
    } else {
        actor->animation.slot.speed = 0.0f;
    }
}

void Enemy_Found(Actor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    unsigned short count = work->hit_count;

    work->hit_count = count + 1;
    if ((short)count >= work->hit_limit)
        Enemy_ActionReady(actor, ENEMY_ACT_CHASE);

    if (actor->inUseId != 0x2201) {
        Actor *player = GameLoopState.player;
        actor->target_angle = Get_Angle(&actor->position, &player->position);
    }
}

void Enemy_Tentacle(Actor *self)
{
    EnemyWorkEntry *work = &enepc[self->number];
    short count;
    Vector4 point;
    Vector4 mark;
    float distance;
    float radius;
    int unit;
    short i;

    if (work->locator_start_frame <= (short)work->hit_count &&
        (short)work->hit_count <= work->locator_end_frame) {
        distance = Get_Distance(&self->position, &GameLoopState.player->position);
        radius = self->body_radius +
                 (work->locator_distance - self->body_radius) *
                     ((short)work->hit_count - work->locator_start_frame) /
                     (work->locator_end_frame - work->locator_start_frame);
        self->target_angle =
            Get_Angle(&self->position, &GameLoopState.player->position);

        if (FLAG_FRAME_60) {
            for (i = 0; i < ENEMY_RANGE_CIRCLE_SEGMENTS; i++) {
                float angle = i * 6.2831855f * 0.015625f;

                mark.x = self->position.x + sinf(angle) * radius;
                mark.y = self->position.y + 1.0f;
                mark.z = self->position.z + cosf(angle) * radius;
                mark.w = 1.0f;
                Debug_Mark(&mark, (int)AdrsEnemySphere);
            }
        }

        Get_Point_By_AngleLength(&self->position, &point, self->rotation.y, radius);
        unit = CrossCheckMapUnit(&self->position, &point);
        if (unit != -1)
            SendBrokenSignal(&MapUnit[unit]);

        if (GameLoopState.player->flash_timer == 0 && distance < radius)
            Check_Encount(self, 0, 0);
        else
            GameLoopState.encount_reset = -1;
    }

    count = work->hit_count;
    work->hit_count++;
    if (count >= work->hit_limit)
        Enemy_ActionReady(self, 1);
}

void Enemy_Chase(Actor *self)
{
    EnemyWorkEntry *work = &enepc[self->number];
    EnemyParams *params = &self->enemy;
    AlignedVector4 start;
    Vector4 hit;
    Vector4 cross_point;
    int color[4];
    DoublewordVector4 square;
    float distance;
    float speed;
    int crossed;

    distance = Get_Distance(&self->position, &work->escape_target.position);
    if (ENEMY_CHASE_MIN_DISTANCE <= distance && !(work->stiff_flag & 2)) {
        speed = params->chase_speed;
        Set_Motion(self, Get_DefaultMotion(self, ENEMY_DEFAULT_MOTION_RUN), MOTION_BLEND_DEFAULT);
    } else {
        speed = params->chase_speed / 1.5f;
        Set_Motion(self, Get_DefaultMotion(self, ENEMY_DEFAULT_MOTION_WALK), MOTION_BLEND_DEFAULT);
    }

    start = self->position;
    Check_Undu(&self->position, &work->escape_target.position, work->map_index,
               &hit, 1, 0, 0);
    crossed = CrossPointUwamono(&self->position,
                                &work->escape_target.position,
                                self->body_radius, &cross_point);
    if (__builtin_fabsf(hit.x - work->escape_target.position.x) < ESCAPE_TARGET_POSITION_EPSILON &&
        __builtin_fabsf(hit.z - work->escape_target.position.z) < ESCAPE_TARGET_POSITION_EPSILON &&
        crossed == 0) {
        work->last_player_position = work->escape_target.position_pair;
        xglFontDebugPrintf(8, 100, witnessedText);
    } else if (crossed == 1) {
        work->last_player_position.vector = cross_point;
        xglFontDebugPrintf(8, 112, fieldObjectText);
    } else {
        if (Check_MoveSquare(self->body_radius, &work->escape_target.position,
                             &self->position, &square.vector,
                             work->map_index) == 1) {
            work->last_player_position = square;
            xglFontDebugPrintf(8, 112, newText);
        }
    }

    color[0] = 255;
    color[1] = 255;
    color[2] = 255;
    color[3] = 128;
    TM_Enemy_Move_Step(self->number, &work->last_player_position.vector, speed);
    if ((work->state_flags & ENEMY_STATE_FLAG_AIRBORNE) != 0) {
        self->position.y += (work->escape_target.position.y - self->position.y) *
                            ESCAPE_HEIGHT_FOLLOW_RATE;
    }

    if (work->move_result == ENEMY_MOVE_RESULT_MAP_UNIT) {
        if (Check_MoveSquare(self->body_radius, &cross_point, &self->position,
                             &square.vector, work->map_index) == 1) {
            work->last_player_position = square;
            xglFontDebugPrintf(8, 124, fieldObjectText);
        } else {
            work->last_player_position.vector = cross_point;
            xglFontDebugPrintf(8, 124, goStraightText);
        }
    }

    if (distance >= params->chase_distance || work->move_result == ENEMY_MOVE_RESULT_TERRITORY_B ||
        (work->chase_frame++ >=
             (work->chase_frame_limit < 0 ? -work->chase_frame_limit : work->chase_frame_limit) &&
         work->chase_frame_limit != 0) ||
        (Get_Distance(&self->position, &work->last_player_position.vector) <
             CHASE_LOST_DISTANCE &&
         work->move_result != ENEMY_MOVE_RESULT_MAP_UNIT && work->move_result != ENEMY_MOVE_RESULT_FREE)) {
        work->spline_parameter = 0;
        Homing_Search(self);
        Enemy_ActionReady(self, ENEMY_ACT_THREAT);
    } else if (Get_Distance(&self->position,
                            &work->last_player_position.vector) < LAST_PLAYER_REACHED_DISTANCE) {
        if (Check_MoveSquare(self->body_radius, &work->escape_target.position,
                             &self->position, &square.vector,
                             work->map_index) == 1) {
            work->last_player_position = square;
            xglFontDebugPrintf(8, 112, addText);
            TM_Enemy_Move_Step(self->number, &work->last_player_position.vector,
                               speed);
        }
        self->target_angle = Get_Angle(&self->position,
                                       &actor[work->target_number].position);
    }
}

void Enemy_Route(Actor *self)
{
    EnemyWorkEntry *work = &enepc[self->number];
    EnemyParams *params = &self->enemy;
    Vector4 point;
    Vector4 origin;
    Vector4 target;
    Vector4 hit;
    signed char directions[ENEMY_WALL_DIRECTION_COUNT];
    int saved_parameter = work->spline_parameter;
    short length = 0;
    short i;
    short available;
    short index;
    float distance;
    float angle;
    short motion;
    signed char moved;

    if (work->spline_point_count <= 0)
        return;

    work->unique_pattern = saved_parameter / 128;
    if (work->spline_point_count * 128 - 384 <= saved_parameter &&
        (self->flags & ACTOR_FLAG_HOMING) != 0) {
        if (work->pause_kind != ENEMY_MOVE_ROUTE_B) {
            Set_Spline_By_Random();
        } else {
            Set_Spline_By_Route(self, 0);
        }
        self->flags &= 0x7fffffff;
    }

    if ((work->spline_parameter % 128) == 0 &&
        (self->flags & ACTOR_FLAG_HOMING) == 0) {
        for (i = 0; i < 2; i++) {
            if (work->action_motion_table[work->spline_parameter / 128][0] != -1) {
                work->unique_step = 0;
                Enemy_ActionReady(self, ENEMY_ACT_UNIQUE);
                return;
            }
            if ((i != 0 || params->pause_interval != 0) &&
                (i != 1 || params->threat_interval != 0)) {
                switch (i) {
                case 0:
                    length = params->pause_interval;
                    break;
                case 1:
                    length = params->threat_interval;
                    break;
                }
                work->route_cursor[i] = (work->route_cursor[i] + 1) % length;
                if (work->route_marks[i][work->route_cursor[i]] == 1) {
                    switch (i) {
                    case 0:
                        Enemy_ActionReady(self, ENEMY_ACT_PAUSE);
                        break;
                    case 1:
                        Enemy_ActionReady(self, ENEMY_ACT_THREAT);
                        break;
                    }
                    work->route_marks[0][work->route_cursor[i]] = work->route_marks[1][work->route_cursor[i]] = 0;
                    return;
                }
                if (work->route_cursor[i] == 0) {
                    short mark;

                    for (mark = 0; mark < length; mark++)
                        work->route_marks[i][mark] = 0;
                    work->route_marks[i][Get_Rnd(0, length - 1)] = 1;
                }
                        }
        }
    }

    if (work->state_flags & ENEMY_STATE_FLAG_ALERT_RAISE) {
        GetBSplineLoop(work->spline_parameter, work->spline_point_count, 128,
                       work->spline_points, &point);
        if (work->fast_point_count == 0)
            work->spline_parameter +=
                work->spline_parameter % 2 + 2;
        else
            work->spline_parameter +=
                work->spline_parameter % 8 + 16;
    } else {
        GetBSplineLoopDummy(work->spline_parameter, work->spline_point_count, 128,
                            work->spline_points, &point, self);
    }

    distance = Get_Distance(&self->position, &point);
    if (work->spline_point_count << 7 <= work->spline_parameter)
        work->spline_parameter = 0;
    if (ROUTE_NEAR_LOOKBACK_DISTANCE <= distance)
        self->target_angle = Get_Angle(&self->position, &point);

    {
        int airborne = work->state_flags & ENEMY_STATE_FLAG_AIRBORNE;

        work->goback_x = point.x - self->position.x;
        work->goback_z = point.z - self->position.z;
        self->position.x = point.x;
        self->position.z = point.z;
        if (airborne)
            self->position.y = point.y;
    }

    moved = TM_Check_Move(self);
    if (!moved) {
        if ((int)self->flags < 0) {
            Homing_Search(self);
        } else if (work->pause_kind == ENEMY_MOVE_ROUTE_B || work->pause_kind == ENEMY_MOVE_ROUTE_A) {
            work->spline_parameter = saved_parameter;
        } else {
            available = 0;
            Check_Wall(1.0f, self->body_radius, &origin, directions, -16,
                       work->map_index, self->number, work->wall_attribute);
            for (i = 0; i < ENEMY_WALL_DIRECTION_COUNT; i++) {
                if (directions[i] == 0)
                    directions[available++] = i;
            }
            if (available == 0)
                angle = xglSRand() % 360 / 180.0f * ROUTE_HALF_TURN_RADIANS;
            else
                angle = directions[xglSRand() % available] * ROUTE_DIRECTION_FULL_TURN *
                        0.0625f;
            BSpline_Init(work->spline_points, &self->position, angle);
            work->spline_parameter = 0;
        }
        Set_Motion(self, Get_DefaultMotion(self, ENEMY_DEFAULT_MOTION_IDLE), MOTION_BLEND_DEFAULT);
    } else {
        if (ROUTE_FAR_LOOKBACK_DISTANCE <= distance)
            motion = Get_DefaultMotion(self, ENEMY_DEFAULT_MOTION_RUN);
        else
            motion = Get_DefaultMotion(self, ENEMY_DEFAULT_MOTION_WALK);
        Set_Motion(self, motion, MOTION_BLEND_DEFAULT);
    }

    if ((work->pause_kind == ENEMY_MOVE_WANDER_A || work->pause_kind == ENEMY_MOVE_WANDER_B) &&
        (int)self->flags >= 0 && (work->spline_parameter % 128) == 0) {
        if (work->fast_point_count > 0)
            work->fast_point_count--;
        index = (work->spline_parameter / 128 + 2) %
                work->spline_point_count;
        origin.x = work->spline_coordinates[index][0];
        origin.y = work->spline_coordinates[index][1];
        origin.z = work->spline_coordinates[index][2];
        available = 0;
        Check_Wall(1.0f, self->body_radius, &origin, directions, -16,
                   work->map_index, self->number, work->wall_attribute);
        for (i = 0; i < ENEMY_WALL_DIRECTION_COUNT; i++) {
            if (directions[i] == 0)
                directions[available++] = i;
        }
        if (available == 0)
            angle = xglSRand() % 360 / 180.0f * ROUTE_RANDOM_HALF_TURN_RADIANS;
        else
            angle = directions[xglSRand() % available] * ROUTE_RETREAT_FULL_TURN * 0.0625f;
        target.x = origin.x + sinf(angle) * params->route_step_length;
        target.y = self->position.y;
        target.z = origin.z + cosf(angle) * params->route_step_length;
        target.w = 1.0f;
        Check_Straight(self->body_radius, &origin, &target, 16, &hit,
                       work->map_index, self->number, work->wall_attribute);
        if (work->state_flags & ENEMY_STATE_FLAG_AIRBORNE)
            hit.y = Get_Height(&hit, work->map_index, 0) +
                    Get_Rnd(30, 50) / 10.0f;
        BSpline_Add(work->spline_parameter, work->spline_point_count,
                    128, work->spline_points, &hit, 3);
    }
}

void Enemy_Escape(Actor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    Vector4 point;
    float distance[3];
    short i;
    int length;
    int cooldown;
    short motion;

    if (work->spline_point_count <= 0)
        return;

    for (i = 0; i < 3; i++) {
        length = work->spline_point_count * 32;
        GetBSplineLoop((work->spline_parameter + length + i - 1) % length,
                       work->spline_point_count, 32, work->spline_points,
                       &point);
        distance[i] = Get_Distance(&work->escape_target.position, &point);
    }

    if (distance[0] < distance[1])
        i = distance[1] < distance[2] ? 2 : 1;
    else
        i = distance[0] < distance[2] ? 2 : 0;

    work->spline_parameter =
        (work->spline_parameter + work->spline_point_count * 32 + i - 1) %
        (work->spline_point_count * 32);
    GetBSplineLoop(work->spline_parameter, work->spline_point_count, 32,
                   work->spline_points, &point);

    if (i == 1) {
        actor->target_angle =
            Get_Angle(&actor->position, &work->escape_target.position);
        cooldown = CoolDown_0;
        CoolDown_0 = cooldown - 1;
    } else {
        CoolDown_0 = 16;
        actor->target_angle = Get_Angle(&actor->position, &point);
    }

    if (ESCAPE_RESET_DISTANCE <= Get_Distance(&actor->position, &point) ||
        (short)CoolDown_0 > 0) {
        motion = Get_DefaultMotion(actor, ENEMY_DEFAULT_MOTION_RUN);
    } else {
        motion = Get_DefaultMotion(actor, ENEMY_DEFAULT_MOTION_WALK);
    }
    Set_Motion(actor, motion, MOTION_BLEND_DEFAULT);

    actor->animation.slot.speed = ESCAPE_SPEED;
    work->goback_x = point.x - actor->position.x;
    work->goback_z = point.z - actor->position.z;
    actor->position.x = point.x;
    actor->position.z = point.z;
}

void Enemy_Route_Chase(Actor *self)
{
    EnemyWorkEntry *work = &enepc[self->number];
    Vector4 point;
    float distance[3];
    short i;
    int length;
    int cooldown;
    short motion;

    if (work->spline_point_count <= 0)
        return;

    for (i = 0; i < 3; i++) {
        length = work->spline_point_count * 32;
        GetBSplineLoop((work->spline_parameter + length + i - 1) % length,
                       work->spline_point_count, 32, work->spline_points,
                       &point);
        distance[i] = Get_Distance(&work->escape_target.position, &point);
    }

    if (distance[0] < distance[1])
        i = distance[0] < distance[2] ? 0 : 2;
    else
        i = distance[1] < distance[2] ? 1 : 2;

    work->spline_parameter =
        (work->spline_parameter + work->spline_point_count * 32 + i - 1) %
        (work->spline_point_count * 32);
    GetBSplineLoop(work->spline_parameter, work->spline_point_count, 32,
                   work->spline_points, &point);

    if (i == 1) {
        self->target_angle =
            Get_Angle(&self->position, &work->escape_target.position);
        cooldown = CoolDown_1;
        CoolDown_1 = cooldown - 1;
    } else {
        CoolDown_1 = 16;
        self->target_angle = Get_Angle(&self->position, &point);
    }

    if (ROUTE_CHASE_RESET_DISTANCE <= Get_Distance(&self->position, &point) ||
        (short)CoolDown_1 > 0) {
        motion = Get_DefaultMotion(self, ENEMY_DEFAULT_MOTION_RUN);
    } else {
        motion = Get_DefaultMotion(self, ENEMY_DEFAULT_MOTION_WALK);
    }
    Set_Motion(self, motion, MOTION_BLEND_DEFAULT);

    self->animation.slot.speed = ROUTE_CHASE_SPEED;
    work->goback_x = point.x - self->position.x;
    work->goback_z = point.z - self->position.z;
    self->position.x = point.x;
    self->position.z = point.z;
}

void Enemy_Pause(Actor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    unsigned short count;
    int limit;

    if (work->pause_kind == ENEMY_MOVE_STAND_A || work->pause_kind == ENEMY_MOVE_STAND_B) {
        if (actor->flags & ACTOR_FLAG_FACE_PLAYER) {
            actor->target_angle =
                Get_Angle(&actor->position, &GameLoopState.player->position);
        }
    }

    count = work->hit_count;
    limit = work->hit_limit - 1;
    work->hit_count = count + 1;
    if ((short)count >= limit) {
        switch (work->pause_kind) {
        case ENEMY_MOVE_STAND_A:
        case ENEMY_MOVE_STAND_B:
            Enemy_ActionReady(actor, ENEMY_ACT_PAUSE);
            break;
        case ENEMY_MOVE_WANDER_A:
        case ENEMY_MOVE_WANDER_B:
        case ENEMY_MOVE_ROUTE_A:
        case ENEMY_MOVE_ROUTE_B:
        case ENEMY_MOVE_ROUTE_BACK_AND_FORTH_B:
            Enemy_ActionReady(actor, ENEMY_ACT_ROUTE);
            if (work->state_flags & ENEMY_STATE_FLAG_ALERT_RAISE)
                work->spline_parameter += 2;
            break;
        case ENEMY_MOVE_ROUTE_THEN_ESCAPE:
            Enemy_ActionReady(actor, ENEMY_ACT_ESCAPE);
            break;
        case ENEMY_MOVE_ROUTE_THEN_ROUTE_CHASE:
            Enemy_ActionReady(actor, ENEMY_ACT_ROUTE_CHASE);
            break;
        default: {
            unsigned short direction = xglSRand() & 1;

            switch (direction) {
            case 0:
                Enemy_ActionReady(actor, ENEMY_ACT_ROUTE);
                break;
            case 1:
                Enemy_ActionReady(actor, ENEMY_ACT_THREAT);
                break;
            }
            break;
        }
        }
    }
}

void Enemy_Threat(Actor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    unsigned short count = work->hit_count;
    int limit = work->hit_limit - 1;

    work->hit_count = count + 1;
    if ((short)count >= limit) {
        Enemy_ActionReady(actor, ENEMY_ACT_ROUTE);
        if (work->state_flags & ENEMY_STATE_FLAG_ALERT_RAISE)
            work->spline_parameter += 2;
    }
}

void Enemy_Unique(Actor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    unsigned short count = work->hit_count;
    int limit = work->hit_limit - 1;

    work->hit_count = count + 1;
    if ((short)count >= limit) {
        int step = work->unique_step + 1;

        work->unique_step = step;
        if (work->action_motion_table[work->unique_pattern][(short)step] != -1) {
            Enemy_ActionReady(actor, ENEMY_ACT_UNIQUE);
        } else {
            Enemy_ActionReady(actor, ENEMY_ACT_ROUTE);
            if (work->state_flags & ENEMY_STATE_FLAG_ALERT_RAISE)
                work->spline_parameter += 2;
        }
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char animTimeFormat[20] = "%3d AnimTime  = %8f";

const char animStartFormat[20] = "%3d AnimStart = %8f";

const char animFinalFormat[20] = "%3d AnimFinal = %8f";

const char animSpeedFormat[20] = "%3d AnimSped  = %8f";

const char animNumberFormat[20] = "%3d AnimNumb  = %3d";

const char animBlendFormat[20] = "%3d Hokan     = %8f";

void ACT_updateEnemy(Actor *self)
{
    Vector4 marker;
    int color[4];
    AlignedVector4 pillar;
    EnemyParams *params = &self->enemy;
    EnemyWorkEntry *work = &enepc[self->number];
    int effect_code;
    float angle;
    short i;
    int type;
    unsigned short id;

    color[2] = 255;
    color[3] = 128;
    marker.y = 2.0f;
    marker.w = 1.0f;
    color[0] = 0;
    color[1] = 0;
    marker.x = 0.0f;
    marker.z = 0.0f;
    if (self->number == DebugActor) {
        xglFontDebugPrintf(8, 52, animTimeFormat, self->number, (double)self->animation.slot.frame);
        if (self->number == DebugActor) {
            xglFontDebugPrintf(8, 62, animStartFormat, self->number, (double)self->animation.slot.reverseLimit);
            if (self->number == DebugActor) {
                xglFontDebugPrintf(8, 72, animFinalFormat, self->number, (double)self->animation.slot.forwardLimit);
                if (self->number == DebugActor) {
                    xglFontDebugPrintf(8, 82, animSpeedFormat, self->number, (double)self->animation.slot.speed);
                    if (self->number == DebugActor) {
                        xglFontDebugPrintf(8, 92, animNumberFormat, self->number, self->animation.slot.currentDataId);
                        if (self->number == DebugActor)
                            xglFontDebugPrintf(8, 102, animBlendFormat, self->number, (double)self->animation.blendDuration);
                    }
                }
            }
        }
    }
    if (FLAG_FRAME_60) {
        pillar = self->position;
        pillar.x = self->body_radius;
        pillar.y = 0.0f;
        DispPillar(&self->position, &pillar, color);
    }
    work->move_result = ENEMY_MOVE_RESULT_FREE;
    if (params->gravity_mode != 1) {
        if (Get_Height(&self->position, work->map_index, 16) != GROUND_HEIGHT_NONE)
            self->saved_position = self->position;
    }
    if (work->sense_block_timer > 0)
        work->sense_block_timer--;
    if (!(self->flags & ACTOR_FLAG_COLLIDABLE) || self->inUseId <= 0 ||
        self == GameLoopState.player)
        return;

    if (work->applied_light_level != params->light_level) {
        if (params->light_level == ENEMY_LIGHT_LEVEL_FULL) {
            self->flags &= ~ACTOR_FLAG_TRANSLUCENT;
        } else if (work->applied_light_level == ENEMY_LIGHT_LEVEL_FULL) {
            self->flags |= ACTOR_FLAG_TRANSLUCENT;
            self->drawEnv.flags |= 3;
            self->drawEnv.alpha = ACTOR_TRANSLUCENT_ALPHA;
        }
        self->drawEnv.transparency = (float)params->light_level / 1000.0f;
    }
    work->applied_light_level = params->light_level;
    GameIdLightSet(self, 0);

    if (self->inUseId != 16646 && self->inUseId != 20594) {
        effect_code = Get_EffectCode(Get_Attr(&self->position, 0, 0));
        if (work->action == ENEMY_ACT_CHASE || work->action == 3 || work->action == ENEMY_ACT_ROUTE ||
            work->action == 8 || work->action == ENEMY_ACT_TENTACLE || work->action == ENEMY_ACT_ESCAPE ||
            work->action == ENEMY_ACT_ROUTE_CHASE || EfTab_Code[effect_code] == 1) {
            if (effect_code != -1) {
                SchedulerState *effect;

                if (EfTab_Intr[effect_code] != -1)
                    self->effect_timer++;
                if ((EfTab_Intr[effect_code] != -1 &&
                     self->effect_timer >= EfTab_Intr[effect_code]) ||
                    self->effect_timer == -1) {
                    effect = sefCreateEffectCf(EfTab_Code[effect_code], 0, 0);
                    self->effects[self->effect_slot] = effect;
                    if (effect != 0) {
                        float size = params->body_radius * EfTab_Scal[effect_code];

                        effect->caster = self;
                        self->effect_timer = 0;
                        self->effect_slot = (self->effect_slot + 1) % ACTOR_EFFECT_SLOT_COUNT;
                        effect->scale.z = size;
                        effect->scale.y = size;
                        effect->scale.x = size;
                    }
                }
            } else {
                for (i = 0; i < ACTOR_EFFECT_SLOT_COUNT; i++) {
                    if (self->effects[i])
                        sefDeleteEffectCf(self->effects[i]);
                }
                self->effect_timer = -1;
            }
        }
    }

    if (work->target_number == 0)
        work->escape_target.position_pair = EnemyTarget;
    else
        Get_EnemyIDPos(&actor[work->target_number].position,
                       &work->escape_target.position);
    Start_Enemy_Command(self);
    if (work->freeze_state == ENEMY_FREEZE_SLOW) {
        ACT_updateSequence(self);
        return;
    }
    if (work->escape_target.scale_frame != -1 &&
        (unsigned char)work->freeze_state < ENEMY_FREEZE_STOP) {
        short frame = work->escape_target.scale_frame;
        short count = work->escape_target.scale_frame_count;
        float scale;

        work->escape_target.scale =
            work->escape_target.scale_to -
            (work->escape_target.scale_to - work->escape_target.scale_from) *
                (float)(count * count - frame * frame) / (float)(count * count);
        ACT_setMotion(self, self->animation.slot.currentDataId);
        work->escape_target.scale_frame++;
        if (work->escape_target.scale_frame >= work->escape_target.scale_frame_count)
            work->escape_target.scale_frame = -1;
        scale = work->escape_target.scale;
        self->scale.x = self->scale.y = self->scale.z = scale;
        self->scale.w = 1.0f;
    }
    if (self->look_at_timer == 2)
        Actor_LookAt(self);
    if (self->inUseId == 8705)
        Check_Locater(self);

    if (work->freeze_state != ENEMY_FREEZE_RELEASE) {
        if (work->freeze_state < ENEMY_FREEZE_STOP)
            Script_Action(self);
        if (Get_Distance(&self->position, &GameLoopState.player->position) <=
                self->body_radius + GameLoopState.player->body_radius + ENEMY_CONTACT_PADDING &&
            __builtin_fabsf(self->position.y - GameLoopState.player->position.y) <
                ENEMY_CONTACT_HEIGHT &&
            GameLoopState.player->flash_timer == 0 &&
            enepc[GameLoopState.player->number].encount_lock == 0 &&
            FLAG_FRAME_60 == 0)
            Check_Encount(self, 0, 0);
        {
            float scale = work->escape_target.scale;

            self->scale.x = self->scale.y = self->scale.z = scale;
            self->scale.w = 1.0f;
        }
        ACT_updateMotion(self);
        if (work->freeze_state == ENEMY_FREEZE_SCRIPTED && params->gravity_mode == 0) {
            float height;

            self->data_header = UnduDataGetHeader(work->map_index, 0x8000);
            height = Get_Height(&self->position, work->map_index, 16);
            self->translate_y = height;
            work->fall_speed += ENEMY_FALL_ACCEL;
            self->position.y += work->fall_speed;
            if (self->position.y < height) {
                self->position.y = height;
                work->fall_speed = 0.0f;
            }
        }
        if (work->mark != 0)
            Disp_EnemyMark(self);
        self->data_header = UnduDataGetHeader(0, 0x8000);
        return;
    }

    if (self->inUseId != 8705)
        Check_Locater(self);
    switch (work->action) {
    case ENEMY_ACT_INIT:
        Enemy_Init(self);
    case ENEMY_ACT_PAUSE:
        Enemy_Pause(self);
        break;
    case ENEMY_ACT_THREAT:
        Enemy_Threat(self);
        break;
    case ENEMY_ACT_ROUTE:
        Enemy_Route(self);
        break;
    case ENEMY_ACT_FOUND:
        Enemy_Found(self);
        break;
    case ENEMY_ACT_CHASE:
        Enemy_Chase(self);
        break;
    case ENEMY_ACT_TENTACLE:
        Enemy_Tentacle(self);
        break;
    case ENEMY_ACT_STIFF:
        Enemy_Stiff(self);
        break;
    case ENEMY_ACT_UNIQUE:
        Enemy_Unique(self);
        break;
    case ENEMY_ACT_DAMAGE_EXPLOSION:
        Enemy_Damage_Explosion(self);
        break;
    case ENEMY_ACT_DAMAGE_ELECTRIC:
        Enemy_Damage_Electric(self);
        break;
    case ENEMY_ACT_DAMAGE_SEAL:
        Enemy_Damage_Seal(self);
        break;
    case ENEMY_ACT_ESCAPE:
        Enemy_Escape(self);
        break;
    case ENEMY_ACT_ROUTE_CHASE:
        Enemy_Route_Chase(self);
        break;
    }

    if (params->sight_distance != 0.0f) {
        if (!(self->flags & ACTOR_FLAG_HIDDEN) && work->sense_block_timer <= 0 &&
            enepc[0].encount_lock == 0) {
            if (params->sight_angle < 0.0f) {
                if (self->inUseId == 8705 || self->inUseId == 16646) {
                    if (Check_InsideFan_Wooo(self->rotation.y + INSIDE_FAN_WOOO_ANGLE_OFFSET,
                                             params->sight_distance,
                                             __builtin_fabsf(params->sight_angle),
                                             self->body_radius, &self->position,
                                             &work->escape_target.position,
                                             work->map_index) == 1)
                        Check_Discovery(self);
                } else if (Check_InsideFan(self->rotation.y + INSIDE_FAN_ANGLE_OFFSET,
                                           params->sight_distance,
                                           __builtin_fabsf(params->sight_angle),
                                           self->body_radius, &self->position,
                                           &work->escape_target.position,
                                           work->map_index) == 1) {
                    Check_Discovery(self);
                }
            } else {
                if (self->inUseId == 8705 || self->inUseId == 16646) {
                    if (Check_InsideFan_Wooo(self->rotation.y, params->sight_distance,
                                             __builtin_fabsf(params->sight_angle),
                                             self->body_radius, &self->position,
                                             &work->escape_target.position,
                                             work->map_index) == 1)
                        Check_Discovery(self);
                } else if (Check_InsideFan(self->rotation.y, params->sight_distance,
                                           __builtin_fabsf(params->sight_angle),
                                           self->body_radius, &self->position,
                                           &work->escape_target.position,
                                           work->map_index) == 1) {
                    Check_Discovery(self);
                }
            }
        }
    }
    if (FLAG_FRAME_60 && (PadData.pressed & 1)) {
        angle = self->target_angle;
        work->spline_parameter = 0;
        BSpline_Init(work->spline_points, &self->position, angle);
        Enemy_ActionReady(self, ENEMY_ACT_THREAT);
        Homing_Search(self);
    }

    if ((unsigned short)self->inUseId - 0x4000 < 0xf23u) {
        if (self->flags & ACTOR_FLAG_AILMENT_ACTIVE) {
            if (self->flags & ACTOR_FLAG_STIFF_ACTIVE) {
                if (work->action != ENEMY_ACT_DAMAGE_EXPLOSION && work->action != ENEMY_ACT_STIFF)
                    Enemy_ActionReady(self, ENEMY_ACT_DAMAGE_EXPLOSION);
            }
            if (self->flags & ACTOR_FLAG_ELECTRIC_ACTIVE) {
                if (work->action != ENEMY_ACT_DAMAGE_ELECTRIC)
                    Enemy_ActionReady(self, ENEMY_ACT_DAMAGE_ELECTRIC);
            }
            if (self->flags & ACTOR_FLAG_SEAL_ACTIVE) {
                if (work->action != ENEMY_ACT_DAMAGE_SEAL)
                    Enemy_ActionReady(self, ENEMY_ACT_DAMAGE_SEAL);
            }
        }
    }
    id = self->inUseId;
    type = RES_GetEnemySeType(id);
    if (type == 1) {
        if (self->animation.slot.currentDataId == 1 || self->animation.slot.currentDataId == 3)
            Sound_FootStep(self);
    } else {
        if (work->action != ENEMY_ACT_ROUTE) {
            xglSoundEffectStopID(RES_GetEnemySeBank(self->inUseId) + 2,
                                 self->number + 1);
        } else if (SCRIPT_getCfTime() >= 3) {
            xglSoundEffectPosID(RES_GetEnemySeBank(self->inUseId) + 2,
                                &self->position.x, 1, self->number + 1);
        }
        if (work->action != ENEMY_ACT_CHASE) {
            xglSoundEffectStopID(RES_GetEnemySeBank(self->inUseId) + 4,
                                 self->number + 1);
        } else {
            xglSoundEffectPosID(RES_GetEnemySeBank(self->inUseId) + 4,
                                &self->position.x, 1, self->number + 1);
        }
    }
    if (work->elec_timer > 0) {
        if (--work->elec_timer <= 0) {
            work->elec_timer = 0;
            work->stiff_flag &= ~ENEMY_STIFF_ELECTRIC;
            sefDeleteEffectCf(work->stiff_effect);
            work->stiff_effect = 0;
            xglSoundEffectStopID(SE_ID_DAMAGE_ELECTRIC, 0);
        }
    }
    if (work->status_4_timer > 0) {
        if (--work->status_4_timer == 0) {
            work->stiff_flag &= ~ENEMY_STIFF_SEAL;
            xglSoundEffectStopID(SE_ID_DAMAGE_SEAL, 0);
        }
    }
    if (FLAG_FRAME_60) {
        if (PadData.held & 1)
            GameLoopState.player = &actor[NowTester];
        else
            GameLoopState.player = &actor[0];
    }
    if (Get_Distance(&self->position, &GameLoopState.player->position) <=
            self->body_radius + GameLoopState.player->body_radius + ENEMY_HIT_PADDING &&
        __builtin_fabsf(self->position.y - GameLoopState.player->position.y) <
            ENEMY_HIT_HEIGHT &&
        GameLoopState.player->flash_timer == 0 &&
        enepc[GameLoopState.player->number].encount_lock == 0 &&
        FLAG_FRAME_60 == 0)
        Check_Encount(self, 0, 0);
    Script_Action(self);
    if (work->state_flags & ENEMY_STATE_FLAG_BELT)
        Move_BeltConveyer(self);
    if (work->turn_lock != 1 && work->action != ENEMY_ACT_STIFF) {
        if (work->stiff_flag & ENEMY_STIFF_ELECTRIC)
            self->rotation.y = Get_Cursol_by_Reduce_Speed_Angle_Loop(self->rotation.y, self->target_angle, 36.0f);
        else
            self->rotation.y = Get_Cursol_by_Reduce_Speed_Angle_Loop(self->rotation.y, self->target_angle, 6.0f);
    }
    {
        float scale = work->escape_target.scale;

        self->scale.x = self->scale.y = self->scale.z = scale;
        self->scale.w = 1.0f;
    }
    if (params->gravity_mode != 1) {
        if (Get_Height(&self->position, work->map_index, 16) == GROUND_HEIGHT_NONE)
            self->position = self->saved_position;
    }
    ACT_updateMotion(self);
    if (params->gravity_mode == 0) {
        float height = Get_Height(&self->position, work->map_index, 16);

        self->translate_y = height;
        work->fall_speed += ENEMY_AIR_FALL_ACCEL;
        if (!(work->state_flags & ENEMY_STATE_FLAG_AIRBORNE))
            self->position.y += work->fall_speed;
        if (self->position.y < height) {
            self->position.y = height;
            work->fall_speed = 0.0f;
        }
    }
    if (work->mark != 0)
        Disp_EnemyMark(self);
}

unsigned char TM_Enemy_Move_Step(int actor_number, Vector4 *step_limit,
                                 float goback_x)
{
    EnemyActorHead *self = &actor[actor_number];
    DoublewordVector4 old = self->position_pair;

    Get_One_Step(goback_x, &actor[actor_number].position, step_limit,
                 &enepc[actor_number].goback[0]);
    enepc[actor_number].goback[0] += (&enepc[actor_number])->drift[0];
    enepc[actor_number].goback[2] += enepc[actor_number].drift[2];

    if (old.vector.x < step_limit->x) {
        if (step_limit->x < self->position.x + enepc[actor_number].goback[0])
            enepc[actor_number].goback[0] = step_limit->x - self->position.x;
    }
    if (old.vector.z < step_limit->z) {
        if (step_limit->z < self->position.z + enepc[actor_number].goback[2])
            enepc[actor_number].goback[2] = step_limit->z - self->position.z;
    }

    enepc[actor_number].drift[0] *= ENEMY_DRIFT_DAMPING;
    enepc[actor_number].drift[2] *= ENEMY_DRIFT_DAMPING;
    actor[actor_number].position.x += enepc[actor_number].goback[0];
    actor[actor_number].position_f[2] += enepc[actor_number].goback[2];
    actor[actor_number].target_angle =
        atan2f(enepc[actor_number].goback[0], enepc[actor_number].goback[2]);
    return TM_Check_Move(self);
}

void TM_Check_Move_at_GoBack(Actor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];

    actor->position.x -= work->goback_x;
    actor->position.z -= work->goback_z;
}

int TM_Check_Move(Actor *self)
{
    EnemyWorkEntry *work = &enepc[self->number];
    float height = Get_Height(&self->position, work->map_index, 0x10);
    int attr = Get_Attr(&self->position, work->map_index, 0x10);
    AlignedVector4 start;
    AlignedVector4 end;
    Vector4 hit;
    signed char blocker;

    if (height == GROUND_HEIGHT_NONE) {
        TM_Check_Move_at_GoBack(self);
        work->move_result = ENEMY_MOVE_RESULT_NO_GROUND;
        return 0;
    }

    end = self->position;
    start = self->position;
    end.y = height;
    start.x -= work->goback_x;
    start.z -= work->goback_z;
    if (!Check_Undu(&start, &end, work->map_index, &hit, 1, 0,
                    0x10)) {
        TM_Check_Move_at_GoBack(self);
        work->move_result = ENEMY_MOVE_RESULT_NO_GROUND;
        return 0;
    }

    if (attr != EneTerritoryID[work->wall_attribute] && work->wall_attribute != 0 &&
        work->action != 8 && (int)self->flags >= 0) {
        if (work->state_flags & ENEMY_STATE_FLAG_TERRITORY_LOCK) {
            if (work->action != ENEMY_ACT_CHASE) {
                TM_Check_Move_at_GoBack(self);
                work->move_result = ENEMY_MOVE_RESULT_TERRITORY_A;
                return 0;
            }
        } else {
            TM_Check_Move_at_GoBack(self);
            work->move_result = ENEMY_MOVE_RESULT_TERRITORY_B;
            return 0;
        }
    }

    blocker = Check_Actor(&self->position, self->number);
    if (blocker != -1) {
        TM_Check_Move_at_GoBack(self);
        work->move_result = blocker;
        return 0;
    }

    if ((int)self->flags >= 0 &&
        HitCheckMapUnitPosSize(&self->position, self->body_radius) != -1) {
        if (CheckBrokenMapUnit(&MapUnit[HitCheckMapUnitPosSize(
                &self->position, self->body_radius)]) & 2) {
            SendBrokenSignal(&MapUnit[HitCheckMapUnitPosSize(
                &self->position, self->body_radius)]);
        }
        TM_Check_Move_at_GoBack(self);
        work->move_result = ENEMY_MOVE_RESULT_MAP_UNIT;
        return 0;
    }

    work->move_result = ENEMY_MOVE_RESULT_FREE;
    return 1;
}

char TM_Check_Before_Eyes(Actor *self, float length)
{
    EnemyWorkEntry *work = &enepc[self->number];
    Vector4 point;
    short i;

    Get_Point_By_AngleLength(&self->position, &point, self->rotation.y, length);

    for (i = 0; i < ACTOR_COUNT; i++) {
        if (actor[i].inUseId <= 0)
            continue;
        if (actor[i].kind == 1)
            continue;
        if (!(actor[i].flags & ACTOR_FLAG_COLLIDABLE))
            continue;
        if (self->number == i)
            continue;
        if (Get_Distance(&point, &actor[i].position) <=
            self->body_radius + actor[i].body_radius) {
            return i;
        }
    }

    if (Get_Height(&self->position, work->map_index, 0) == GROUND_HEIGHT_NONE)
        return ENEMY_MOVE_RESULT_NO_GROUND;
    return ENEMY_MOVE_RESULT_FREE;
}

void Set_EnemyMark(Actor *actor, short mark)
{
    EnemyWorkEntry *work = &enepc[actor->number];

    work->mark = mark;
    work->mark_timer = 0;
}

void Disp_EnemyMark(Actor *actor)
{
    EnemyMarkSet mark[16];

    mark[15].none = 0;
    mark[15].exclamation = AdrsEnemyExclamation;
    mark[15].question = AdrsEnemyQuestion;
    mark[15].sphere = AdrsEnemySphere;
    mark[14] = mark[15];
}
