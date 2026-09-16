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

INCLUDE_ASM("asm/main/nonmatchings/exm_2", EXM_SetWindPara);

INCLUDE_ASM("asm/main/nonmatchings/exm_2", EXM_GetWindPara);

void EXM_InitWind(void)
{
    wind = &_wind;
    EXM_ResetWind();
}

INCLUDE_ASM("asm/main/nonmatchings/exm_2", EXM_StopWind);

INCLUDE_ASM("asm/main/nonmatchings/exm_2", EXM_SetShakePower);

INCLUDE_ASM("asm/main/nonmatchings/exm_2", EXM_SetShakeTime);

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

INCLUDE_ASM("asm/main/nonmatchings/exm_2", EXM_SetShakeWind);

void EXM_ResetWave(void)
{
    wave = 0.0f;
}

float EXM_GetWaveRad(void)
{
    return waverad;
}

INCLUDE_ASM("asm/main/nonmatchings/exm_2", EXM_GetShakeRad);
