#include "common.h"
#include "set_motion.h"

extern EnemyStateBlock enepc[16];

extern void BSpline_Init(float *points, const float *origin, float angle);
extern void Get_MiddlePoint(const float *start, const float *following,
                            short point_index, short point_count,
                            float *result);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Set_Motion);

INCLUDE_ASM("asm/main/nonmatchings/set_motion", Sound_FootStep);

extern int RES_GetEnemySeBank(int sound_id);
extern int RES_GetEnemySeType(int sound_id);
extern void xglSoundEffectPosID();

/*
 * Sound_FootStep and Enemy_ActionReady always pass 1 for `play`; when it is
 * not 1 the positional call below is skipped and the function has no other
 * effect. `ignore_se_type` is 0 at Enemy_ActionReady's one sound_index == 1
 * call and 1 everywhere else (main:0x002d3930, 0x002d396c, 0x002d3a84,
 * 0x002d06d8, 0x002d0710, 0x002d0744); when it is 0 and sound_index is 2 or
 * 4, a bank whose RES_GetEnemySeType is 1 is skipped entirely.
 */
void EnemySound(Actor *actor, short sound_index, signed char play,
                 signed char ignore_se_type)
{
    int bank;

    if ((sound_index == 2 || sound_index == 4) && ignore_se_type == 0 &&
        RES_GetEnemySeType(*ACTOR_SOUND_EFFECT_ID(actor)) == 1)
    {
        return;
    }
    if (actor->flags & 8)
    {
        return;
    }
    bank = RES_GetEnemySeBank(*ACTOR_SOUND_EFFECT_ID(actor));
    if (bank != -1 && play == 1)
    {
        xglSoundEffectPosID(bank + (sound_index & 0xffff), &actor->position, 1,
                             ACTOR_NUMBER(actor) + 1);
    }
}

extern int RES_GetEnemySeBank(int sound_id);
extern void xglSoundEffectStopID(int sound_id, int flags);

void EnemySoundEnd(Actor *actor, short sound_offset)
{
    xglSoundEffectStopID(
        RES_GetEnemySeBank(*ACTOR_SOUND_EFFECT_ID(actor)) + (sound_offset & 0xffff),
        ACTOR_NUMBER(actor) + 1);
    /* The original keeps a real call here (jal xglSoundEffectStopID at
     * 0x002d0868) followed by its own register restores and jr ra
     * (0x002d0870..0x002d087c), instead of folding the call into a sibling
     * jump. This no-op loop keeps the call out of tail position and
     * reproduces that real call plus epilogue. */
    do {
    } while (0);
}

INCLUDE_ASM("asm/main/nonmatchings/set_motion", EnemySound_StopAll);

extern void SsdFadeoutEffect(int effect_id, int source_id, int fade_time);
extern void xglSoundEffectStopDirect(int sound_id);

/*
 * A TU-local view of main/xgl_sound.c's struct SoundWork (main:0x004a8140),
 * not yet reachable through a shared header. effect_banks starts at +0x20
 * and each entry's low halfword is its handle (main/xgl_sound.c's
 * SoundEffectBankEntry.handle), the same field xglSoundEffectStopDirect
 * reads (main:0x00226720).
 */
typedef struct SetMotionSoundWork
{
    unsigned char unmodeled_00[0x20];
    struct
    {
        unsigned short handle;
        unsigned short file_handle;
    } effect_banks[32];
} SetMotionSoundWork;

extern SetMotionSoundWork SoundWork;

void EnemySound_Stop(Actor *actor, signed char which)
{
    int bank;
    short channel;
    int handle;

    bank = RES_GetEnemySeBank(*ACTOR_SOUND_EFFECT_ID(actor));
    for (channel = 1; channel < 8; channel++)
    {
        if (which == 1)
        {
            handle = SoundWork.effect_banks[(bank + (channel & 0xffff)) >> 16].handle;
            SsdFadeoutEffect((handle << 16) + (channel & 0xffff), 300,
                              ACTOR_NUMBER(actor) + 1);
        }
        else
        {
            xglSoundEffectStopDirect(bank + (channel & 0xffff));
        }
    }
}

int Get_JAVAReaction(Actor *actor)
{
    return *ENEMY_JAVA_REACTION(enepc[ACTOR_NUMBER(actor)]);
}

short Get_DefaultMotion(Actor *actor, short motion_number)
{
    short *motion_table;

    motion_table = ACTOR_DEFAULT_MOTION_TABLE(actor);
    return motion_table[motion_number];
}

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

void Actor_LookAt_Init(Actor *actor)
{
    *ACTOR_LOOKAT_TIMER(actor) = 0;
    *ACTOR_LOOKAT_TARGET(actor) = -1;
    *ENEMY_LOOKAT_POINT(enepc[ACTOR_NUMBER(actor)]) = 0;
}
