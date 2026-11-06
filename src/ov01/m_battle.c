/*
 * OV01 original TU 16: 0x00a31348..0x00a314b8 (6 functions)
 */
#include "common.h"

/* batFlags bits: set once MBattleInit has run, set while paused. */
#define MBATTLE_FLAG_INITIALIZED 0x1
#define MBATTLE_FLAG_PAUSED 0x2

static int batFlags = 0;
static const char battleMBattleInitCalledToDoubleText[32] = "MBattleInit: Called to double";

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
    if (batFlags & MBATTLE_FLAG_INITIALIZED) {
        MOutputDebugStringWarn(battleMBattleInitCalledToDoubleText);
        return 0;
    }

    MEfObjInit();
    MCamInit();
    MMvInit();
    MMv2Init();
    batFlags = MBATTLE_FLAG_INITIALIZED;
    return 1;
}

void MBattleCreate(void)
{
    if (!(batFlags & MBATTLE_FLAG_INITIALIZED)) {
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
        batFlags |= MBATTLE_FLAG_PAUSED;
        return;
    }
    /* Clears the pause bit and 0x4, whose role is not established (not a named flag). */
    batFlags &= ~6;
}

int MBattleIsInPause(void)
{
    return (batFlags & MBATTLE_FLAG_PAUSED) != 0;
}
