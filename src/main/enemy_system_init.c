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
extern const EnemyPresetPathPrefix D_004CAEE0;
extern const char D_004DB858[];
extern char *strcat(char *destination, const char *source);

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

int Enemy_LoadPreset(void *buffer, const char *preset_name)
{
    EnemyPresetPathStorage path;

    path.parts.prefix = D_004CAEE0;
    memset(path.parts.suffix, 0, sizeof(path.parts.suffix));
    strcat(path.bytes, preset_name);
    strcat(path.bytes, D_004DB858);
    AdrsEnemyPreset = buffer;
    return xglCdReadFile(path.bytes, buffer, 0, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/enemy_system_init", TM_Script_Spline_Add);

INCLUDE_ASM("asm/main/nonmatchings/enemy_system_init", ACT_createEnemy);
