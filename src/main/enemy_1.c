#include "common.h"

#include "enemy_1.h"

/* Per-actor work records are cleared before use. */

EnemyWorkEntry enepc[16] = {{0}};

static unsigned short CoolDown_0 = 0;

static unsigned short CoolDown_1 = 0;

void *AdrsEnemyPreset = 0;

void *AdrsEnemySpline = 0;

void *AdrsEnemyExclamation = 0;

void *AdrsEnemyQuestion = 0;

void *AdrsEnemySphere = 0;

void *AdrsEnemySquare = 0;

/* Distance thresholds and retreat speeds used by the two look-back paths.
 * The original has no named objects for them: cc1 emits each use as an
 * anonymous .lit4 literal (0x004D8034..0x004D8040). */

#define D_004D8034 0.012f

#define D_004D8038 0.06666667f

#define D_004D803C 0.012f

#define D_004D8040 0.06666667f

/* Per-actor work records are cleared before use. */

/* Distance thresholds and retreat speeds used by the two look-back paths. */

int Get_EffectCode(int attr)
{
    int effect[15] = {
        0x8100, 0x8200, 0x8300, 0x8400, 0x8500, 0x8600, 0x8700, 0x8800,
        0x8900, 0x8a00, 0x8b00, 0x8c00, 0x8d00, 0x8e00, 0x8f00,
    };
    short i;

    attr &= 0x8f00;
    for (i = 0; i < 15; i++) {
        if (attr == effect[i]) {
            return i;
        }
    }
    return -1;
}

INCLUDE_ASM("asm/main/nonmatchings/enemy_1", Enemy_Init);

void Enemy_Damage_Explosion(EnemyActor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    unsigned short count;
    short limit;
    int stiff;

    work->goback_x = 0.0f;
    work->goback_z = 0.0f;
    TM_Enemy_Move_Step(actor->number, &work->step_limit, work->goback_x);
    actor->target_angle = Get_Angle(&actor->position,
                                    &GameLoopState[1]->position);

    count = work->hit_count;
    limit = work->hit_limit;
    work->hit_count = count + 1;
    if ((short)count >= limit) {
        actor->flags &= ~ACTOR_FLAG_AILMENT_ACTIVE;
        stiff = work->stiff_flag;
        if (stiff & ENEMY_STIFF_ACTIVE_BIT)
            Enemy_ActionReady(actor, 7);
        else
            Enemy_ActionReady(actor, 6);
    }
}

void Enemy_Damage_Electric(EnemyActor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    int count = work->hit_count + 1;
    short limit = work->hit_limit;

    work->hit_count = count;
    if ((short)count >= limit) {
        u32 flags = actor->flags;

        work->state_40.alert_level = 0;
        actor->flags = flags & ~ACTOR_FLAG_AILMENT_ACTIVE;
        Homing_Search(actor);
        Enemy_ActionReady(actor, 2);
    }
}

void Enemy_Damage_Seal(EnemyActor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    int count = work->hit_count + 1;
    short limit = work->hit_limit;

    work->hit_count = count;
    if ((short)count >= limit) {
        u32 flags = actor->flags;

        work->state_40.alert_level = 0;
        actor->flags = flags & ~ACTOR_FLAG_AILMENT_ACTIVE;
        Homing_Search(actor);
        Enemy_ActionReady(actor, 1);
    }
}

void Enemy_Stiff(EnemyActor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    int timer = work->stiff_timer - 1;

    work->stiff_timer = timer;
    if ((short)timer <= 0) {
        work->stiff_flag &= ~ENEMY_STIFF_ACTIVE_BIT;
        work->stiff_timer = 0;
        actor->speed = work->stiff_saved_speed;
        actor->flags &= ~ACTOR_FLAG_STIFF_ACTIVE;
        sefDeleteEffectCf(work->stiff_effect);
        work->stiff_effect = 0;
        Homing_Search(actor);
        Enemy_ActionReady(actor, 2);
    } else {
        actor->speed = 0.0f;
    }
}

void Enemy_Found(EnemyActor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    unsigned short count = work->hit_count;

    work->hit_count = count + 1;
    if ((short)count >= work->hit_limit)
        Enemy_ActionReady(actor, 6);

    if (actor->sound_effect_id != 0x2201) {
        EnemyActor *player = (EnemyActor *)GameLoopState[1];
        actor->target_angle = Get_Angle(&actor->position, &player->position);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/enemy_1", Enemy_Tentacle);

INCLUDE_ASM("asm/main/nonmatchings/enemy_1", Enemy_Chase);

INCLUDE_ASM("asm/main/nonmatchings/enemy_1", Enemy_Route);

void Enemy_Escape(EnemyActor *actor)
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
        GetBSplineLoop((work->state_40.spline_parameter + length + i - 1) % length,
                       work->spline_point_count, 32, work->spline_points,
                       &point);
        distance[i] = Get_Distance(&work->escape_target.position, &point);
    }

    if (distance[0] < distance[1])
        i = distance[1] < distance[2] ? 2 : 1;
    else
        i = distance[0] < distance[2] ? 2 : 0;

    work->state_40.spline_parameter =
        (work->state_40.spline_parameter + work->spline_point_count * 32 + i - 1) %
        (work->spline_point_count * 32);
    GetBSplineLoop(work->state_40.spline_parameter, work->spline_point_count, 32,
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

    if (D_004D8034 <= Get_Distance(&actor->position, &point) ||
        (short)CoolDown_0 > 0) {
        motion = Get_DefaultMotion(actor, 2);
    } else {
        motion = Get_DefaultMotion(actor, 1);
    }
    Set_Motion(actor, motion, 9);

    actor->speed = D_004D8038;
    work->goback_x = point.x - actor->position.x;
    work->goback_z = point.z - actor->position.z;
    actor->position.x = point.x;
    actor->position.z = point.z;
}

void Enemy_Route_Chase(EnemyActor *self)
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
        GetBSplineLoop((work->state_40.spline_parameter + length + i - 1) % length,
                       work->spline_point_count, 32, work->spline_points,
                       &point);
        distance[i] = Get_Distance(&work->escape_target.position, &point);
    }

    if (distance[0] < distance[1])
        i = distance[0] < distance[2] ? 0 : 2;
    else
        i = distance[1] < distance[2] ? 1 : 2;

    work->state_40.spline_parameter =
        (work->state_40.spline_parameter + work->spline_point_count * 32 + i - 1) %
        (work->spline_point_count * 32);
    GetBSplineLoop(work->state_40.spline_parameter, work->spline_point_count, 32,
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

    if (D_004D803C <= Get_Distance(&self->position, &point) ||
        (short)CoolDown_1 > 0) {
        motion = Get_DefaultMotion(self, 2);
    } else {
        motion = Get_DefaultMotion(self, 1);
    }
    Set_Motion(self, motion, 9);

    self->speed = D_004D8040;
    work->goback_x = point.x - self->position.x;
    work->goback_z = point.z - self->position.z;
    self->position.x = point.x;
    self->position.z = point.z;
}

void Enemy_Pause(EnemyActor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    unsigned short count;
    int limit;

    if (work->pause_kind == 5 || work->pause_kind == 6) {
        if (actor->flags & ACTOR_FLAG_FACE_PLAYER) {
            actor->target_angle =
                Get_Angle(&actor->position, &GameLoopState[1]->position);
        }
    }

    count = work->hit_count;
    limit = work->hit_limit - 1;
    work->hit_count = count + 1;
    if ((short)count >= limit) {
        switch (work->pause_kind) {
        case 5:
        case 6:
            Enemy_ActionReady(actor, 1);
            break;
        case 1:
        case 2:
        case 3:
        case 8:
        case 9:
            Enemy_ActionReady(actor, 4);
            if (work->state_flags & ENEMY_ALERT_RAISE_FLAG)
                work->state_40.spline_parameter += 2;
            break;
        case 12:
            Enemy_ActionReady(actor, 14);
            break;
        case 13:
            Enemy_ActionReady(actor, 15);
            break;
        default: {
            unsigned short direction = xglSRand() & 1;

            switch (direction) {
            case 0:
                Enemy_ActionReady(actor, 4);
                break;
            case 1:
                Enemy_ActionReady(actor, 2);
                break;
            }
            break;
        }
        }
    }
}

void Enemy_Threat(EnemyActor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    unsigned short count = work->hit_count;
    int limit = work->hit_limit - 1;

    work->hit_count = count + 1;
    if ((short)count >= limit) {
        Enemy_ActionReady(actor, 4);
        if (work->state_flags & ENEMY_ALERT_RAISE_FLAG)
            work->state_40.alert_level += 2;
    }
}

void Enemy_Unique(EnemyActor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];
    unsigned short count = work->hit_count;
    int limit = work->hit_limit - 1;

    work->hit_count = count + 1;
    if ((short)count >= limit) {
        int step = work->unique_step + 1;

        work->unique_step = step;
        if (work->unique_pattern_step[work->unique_pattern][(short)step] != -1) {
            Enemy_ActionReady(actor, 9);
        } else {
            Enemy_ActionReady(actor, 4);
            if (work->state_flags & ENEMY_ALERT_RAISE_FLAG)
                work->state_40.spline_parameter += 2;
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/enemy_1", ACT_updateEnemy);

INCLUDE_ASM("asm/main/nonmatchings/enemy_1", TM_Enemy_Move_Step);

void TM_Check_Move_at_GoBack(EnemyActor *actor)
{
    EnemyWorkEntry *work = &enepc[actor->number];

    actor->position.x -= work->goback_x;
    actor->position.z -= work->goback_z;
}

INCLUDE_ASM("asm/main/nonmatchings/enemy_1", TM_Check_Move);

char TM_Check_Before_Eyes(EnemyActor *self, float length)
{
    EnemyWorkEntry *work = &enepc[self->number];
    Vector4 point;
    short i;

    Get_Point_By_AngleLength(&self->position, &point, self->angle, length);

    for (i = 0; i < 64; i++) {
        if (actor[i].sound_effect_id <= 0)
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

    if (Get_Height(&self->position, work->map_index, 0) == -1000.0f)
        return 64;
    return 99;
}

void Set_EnemyMark(EnemyActor *actor, short mark)
{
    EnemyWorkEntry *work = &enepc[actor->number];

    work->mark = mark;
    work->mark_timer = 0;
}

void Disp_EnemyMark(void)
{
    EnemyMarkSet mark[16];

    mark[15].none = 0;
    mark[15].exclamation = AdrsEnemyExclamation;
    mark[15].question = AdrsEnemyQuestion;
    mark[15].sphere = AdrsEnemySphere;
    mark[14] = mark[15];
}
