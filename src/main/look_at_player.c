#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/look_at_player", LookAt_Player);

INCLUDE_ASM("asm/main/nonmatchings/look_at_player", Player_System_Init);

INCLUDE_ASM("asm/main/nonmatchings/look_at_player", GameCfPlayerMoveParamSet);

/* Player-movement speed thresholds and vector scaling rate GameCfPlayerMove
 * compares/applies each frame; GameCfPlayerMoveInit restores their defaults. */
extern int WALK_THRESHOLD_I;
extern float WALK_THRESHOLD_F;
extern int RUN_THRESHOLD_I;
extern float RUN_THRESHOLD_F;
extern float VECTOR_RATE;

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
