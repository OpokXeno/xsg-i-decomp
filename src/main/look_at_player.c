#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/look_at_player", LookAt_Player);

INCLUDE_ASM("asm/main/nonmatchings/look_at_player", Player_System_Init);

/* Player-movement speed thresholds and vector scaling rate GameCfPlayerMove
 * compares/applies each frame; GameCfPlayerMoveInit restores their defaults. */
extern int WALK_THRESHOLD_I;
extern float WALK_THRESHOLD_F;
extern int RUN_THRESHOLD_I;
extern float RUN_THRESHOLD_F;
extern float VECTOR_RATE;

extern int F2I(float value);

void GameCfPlayerMoveParamSet(float walkThreshold, float runThreshold, float vectorRate)
{
    int walkThresholdInt;
    int runThresholdInt;

    walkThresholdInt = F2I(walkThreshold);
    WALK_THRESHOLD_F = walkThreshold;
    WALK_THRESHOLD_I = walkThresholdInt;
    runThresholdInt = F2I(runThreshold);
    RUN_THRESHOLD_F = runThreshold;
    VECTOR_RATE = vectorRate;
    RUN_THRESHOLD_I = runThresholdInt;
}

extern const float D_004D7C0C;

void GameCfPlayerMoveInit(void)
{
    WALK_THRESHOLD_I = 32;
    WALK_THRESHOLD_F = 32.0f;
    RUN_THRESHOLD_I = 96;
    RUN_THRESHOLD_F = 96.0f;
    VECTOR_RATE = D_004D7C0C;
}

INCLUDE_ASM("asm/main/nonmatchings/look_at_player", GameCfPlayerMove);

INCLUDE_ASM("asm/main/nonmatchings/look_at_player", HitCheckActor);

INCLUDE_ASM("asm/main/nonmatchings/look_at_player", NyuruActor);

INCLUDE_ASM("asm/main/nonmatchings/look_at_player", PlayerLookAtAim);

INCLUDE_ASM("asm/main/nonmatchings/look_at_player", GameCfPlayerLoadResource);
