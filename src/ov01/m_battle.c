/*
 * OV01 original TU 16: 0x00a31348..0x00a314b8 (6 functions)
 */
#include "common.h"

extern int batFlags;
extern const char D_00A50EA8[];

extern void MOutputDebugStringWarn(const char *format, ...);
extern void MEfObjInit(void);
extern void MEfObjEnabled(short enabled);
extern void MCamInit(void);
extern void MCamExec(void);
extern void MMvInit(void);
extern void MMvExec(void);
extern int MMvIsPlaying(void);
extern void MMvStop(void);
extern void MMv2Init(void);
extern void MMv2Exec(void);
extern void MMv2Stop(void);

void MBattlePause(short pause);

int MBattleInit(void)
{
    if (batFlags & 1) {
        MOutputDebugStringWarn(D_00A50EA8);
        return 0;
    }

    MEfObjInit();
    MCamInit();
    MMvInit();
    MMv2Init();
    batFlags = 1;
    return 1;
}

void MBattleCreate(void)
{
    if (!(batFlags & 1)) {
        MBattleInit();
    }
    MEfObjInit();
    MEfObjEnabled(1);
}

void MBattleDestroy(void)
{
    MEfObjEnabled(0);
    if (MMvIsPlaying() != 0) {
        MMvStop();
    }
    MMv2Stop();
    MBattlePause(0);
}

void MBattleExec(void)
{
    MCamExec();
    MMvExec();
    MMv2Exec();
}

void MBattlePause(short pause)
{
    if (pause != 0) {
        batFlags |= 2;
        return;
    }
    batFlags &= ~6;
}

int MBattleIsInPause(void)
{
    return (batFlags >> 1) & 1;
}
