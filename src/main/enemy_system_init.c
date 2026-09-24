#include "common.h"
#include "shared.h"
#include "main/xgl_cd.h"

#include "enemy_system_init.h"

extern const char D_004CAE30[];
extern const char D_004CAE48[];
extern const char D_004CAE68[];
extern const char D_004CAE88[];
extern const char D_004CAEA8[];
extern const char D_004CAEC0[];

void Enemy_SystemInit(void) {
    AdrsEnemyPreset = WorkEnd;
    GameLoopState[0x1c / sizeof(unsigned int)] = 0;
    WorkEnd += xglCdReadFile(D_004CAE30, WorkEnd, 0, 0);

    AdrsEnemySpline = WorkEnd;
    WorkEnd += xglCdReadFile(D_004CAE48, WorkEnd, 0, 0);

    AdrsEnemyExclamation = WorkEnd;
    WorkEnd += xglCdReadFile(D_004CAE68, WorkEnd, 0, 0);

    AdrsEnemyQuestion = WorkEnd;
    WorkEnd += xglCdReadFile(D_004CAE88, WorkEnd, 0, 0);

    AdrsEnemySphere = WorkEnd;
    WorkEnd += xglCdReadFile(D_004CAEA8, WorkEnd, 0, 0);

    AdrsEnemySquare = WorkEnd;
    WorkEnd += xglCdReadFile(D_004CAEC0, WorkEnd, 0, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/enemy_system_init", Enemy_LoadPreset);

INCLUDE_ASM("asm/main/nonmatchings/enemy_system_init", TM_Script_Spline_Add);

INCLUDE_ASM("asm/main/nonmatchings/enemy_system_init", ACT_createEnemy);
