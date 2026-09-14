#include "common.h"
#include "set_motion.h"

extern EnemyStateBlock enepc[16];

extern void BSpline_Init(float *points, const float *origin, float angle);
extern void Get_MiddlePoint(const float *start, const float *following,
                            short point_index, short point_count,
                            float *result);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Set_Motion);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Sound_FootStep);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", EnemySound);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", EnemySoundEnd);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", EnemySound_StopAll);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", EnemySound_Stop);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Get_JAVAReaction);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Get_DefaultMotion);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Before_Talk);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", After_Talk);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Check_EnemyFound);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Check_EnemyBurn);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Check_EnemyElec);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Enemy_FindByEar);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Script_Action);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Move_BeltConveyer);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Check_Discovery);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Check_Encount);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Enemy_After_Battle);

void Set_Spline_By_Route(Actor *actor, short route_offset)
{
    unsigned char *actor_bytes;
    unsigned char *state;
    float *point_data;
    short point;

    actor_bytes = (unsigned char *)actor;
    state = enepc[actor_bytes[0x80]];
    point = 0;

    if (ENEMY_ROUTE_POINT_COUNT(state) > 0) {
        /* One walking pointer covers both arrays, as the original does: it
         * addresses the spline point's z at 0(v1) and the route point's x
         * 1026 floats further on, which is ENEMY_ROUTE_POINTS minus
         * ENEMY_SPLINE_POINTS. Only the three coordinates of each record are
         * copied. Initialized inside the positive-count branch so
         * ee-gcc2_96 -O2 -G8 emits addiu v1,a0,0x88 after blez, matching the
         * original. */
        point_data = &ENEMY_SPLINE_POINTS(state)[0][2];
        do {
            point_data[-2] = point_data[1026];
            point_data[-1] = point_data[1027];
            point_data[0] = point_data[1028];
            point++;
            point_data += 4;
        } while (point < ENEMY_ROUTE_POINT_COUNT(state));
    }

    ENEMY_SPLINE_POINT_COUNT(state) = ENEMY_ROUTE_POINT_COUNT(state);
    ENEMY_SPLINE_PARAMETER(state) =
        (short)((ENEMY_ROUTE_POINT_COUNT(state) + route_offset) *
                    ENEMY_SPLINE_UNITS_PER_POINT + 2);
}

void Set_Spline_By_Random(Actor *actor)
{
    unsigned char *actor_bytes;
    unsigned char *state;
    float (*spline_points)[4];

    actor_bytes = (unsigned char *)actor;
    state = enepc[actor_bytes[0x80]];
    spline_points = ENEMY_SPLINE_POINTS(state);
    BSpline_Init(spline_points[0], &actor->position.x,
                 *(float *)((unsigned char *)actor + ACTOR_TARGET_ANGLE_OFFSET));
    ENEMY_SPLINE_POINT_COUNT(state) = 4;
    ENEMY_SPLINE_FAST_POINTS(state) = 1;
    ENEMY_SPLINE_PARAMETER(state) = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Set_PlayerHistory);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Homing_Search);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Homing_Add);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Refresh_Homing);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Disp_Homing);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Set_Shadow);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Get_MostNear_Actor);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Actor_LookAt);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Actor_LookAt_Set);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Actor_LookAt_Release);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Actor_LookAt_Init);
