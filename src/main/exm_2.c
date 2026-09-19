#include "common.h"
#include "shared.h"

typedef struct EXM_WindState EXM_WindState;

extern EXM_WindState *wind;
extern float wave;
extern float waverad;
extern EXM_WindState _wind;

void EXM_ResetWind(void);

INCLUDE_ASM("asm/main/nonmatchings/exm_2", EXM_GetWindPower);

INCLUDE_ASM("asm/main/nonmatchings/exm_2", EXM_ResetWind);

/*
 * EXM_GetWindPower/EXM_ResetWind/EXM_SetDirectionalWind/EXM_SetPointWind/
 * EXM_StepShakeWind (this unit, main/tu268) also read or write this struct:
 * +0x00 is the mode EXM_StopWind and EXM_ResetWind clear to 0,
 * EXM_SetDirectionalWind sets to 1 and EXM_SetPointWind sets to 2; +0x10 is a
 * direction/point vector (four floats: EXM_ResetWind zeroes +0x10/+0x14/+0x18/
 * +0x1c individually) that EXM_Set{Directional,Point}Wind load with a 16-byte
 * copy. None of those bytes are written by a function claimed here, so they
 * stay modeled only for their evidenced size and 8-byte alignment (`u64`,
 * not the attribute-qualified float type the ordinary-C admission refuses):
 * EXM_SetWindPara/EXM_GetWindPara move the whole 0x30-byte block in six ld/sd
 * pairs, which needs that alignment -- a struct whose largest member is only
 * 4-byte aligned instead compiles the same `*wind = *para;` assignment to
 * ldl/ldr/sdl/sdr.
 */
struct EXM_WindState {
    char mode;              /* +0x00, read signed (lb) by EXM_GetWindPower */
    u8 unmodeled_01[0xf];
    u64 unmodeled_10[2];    /* +0x10, evidenced size/alignment only */
    float shake_power;      /* +0x20, EXM_SetShakePower */
    float shake_time;       /* +0x24, EXM_SetShakeTime */
    float shake_rad;        /* +0x28, EXM_SetShakeWind/EXM_GetShakeRad */
    u8 unmodeled_2c[4];
};

void EXM_SetWindPara(EXM_WindState *para)
{
    if (para != 0) {
        *wind = *para;
    }
}

void EXM_GetWindPara(EXM_WindState *para)
{
    if (para != 0) {
        *para = *wind;
    }
}

void EXM_InitWind(void)
{
    wind = &_wind;
    EXM_ResetWind();
}

void EXM_StopWind(void)
{
    wind->mode = 0;
    wind->shake_rad = 0.0f;
}

void EXM_SetShakePower(float power)
{
    wind->shake_power = power;
}

/* D_004D8774 = 6.2831855f (2*pi), the upper clamp for the shake phase. */
extern float D_004D8774;
/* D_004D8778 = -6.2831855f (-2*pi), the lower clamp for the shake phase. */
extern float D_004D8778;

void EXM_SetShakeTime(float time)
{
    if (time > D_004D8774) {
        time = D_004D8774;
    } else if (time < D_004D8778) {
        time = D_004D8778;
    }
    wind->shake_time = time;
}

INCLUDE_ASM("asm/main/nonmatchings/exm_2", EXM_SetDirectionalWind);

INCLUDE_ASM("asm/main/nonmatchings/exm_2", EXM_SetPointWind);

void EXM_SetWindStruct(EXM_WindState *state)
{
    wind = state;
}

void EXM_ClearWindStruct(void)
{
    wind = &_wind;
}

INCLUDE_ASM("asm/main/nonmatchings/exm_2", EXM_StepShakeWind);

void EXM_SetShakeWind(float rad)
{
    wind->shake_rad = rad;
}

void EXM_ResetWave(void)
{
    wave = 0.0f;
}

float EXM_GetWaveRad(void)
{
    return waverad;
}

float EXM_GetShakeRad(void)
{
    return wind->shake_rad;
}
