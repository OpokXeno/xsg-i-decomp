/*
 * OV12 original TU 1: 0x00a00088..0x00a01ff8 (38 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_main.h"

static int _IsEndOfMode(void)
{
    int result;
    u16 pause;   /* PadData[0].prefix.half_2a bit 0x800: this function's debug-shortcut gate */
    u16 buttons; /* PadData[0].prefix.half_28 bit 0x100: the button bit it reports */

    buttons = PadData[0].prefix.half_28;
    pause = PadData[0].prefix.half_2a;
    result = 0;
    if (InstanceOfRgDebugFlags()->modeEnabled != 0 && (pause & 0x800) != 0) {
        if (buttons & 0x100) {
            result = 1;
        }
    }
    return result;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _DoAnyOnePausing);

static int _IsEndOfPause(int controller)
{
    u16 held;

    if (controller < 0) {
        held = PadData[0].prefix.half_2a | PadData[1].prefix.half_2a;
    } else {
        held = PadData[controller].prefix.half_2a;
    }
    return held & 0x800;
}

static void _EndOfFrame(void)
{
    XrgPaint2DFlush(InstanceOfXrgPaint2D());
    XrgParticleDriverDisp(InstanceOfXrgParticleDriver());
    RgDrawJob(InstanceOfRgDraw());
    XrgSleep();
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _GetStageData);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _GetStageDataBySerial);

static void _GetStageData(int stageId);

static void _GetStageHardDataBySerial(void)
{
    _GetStageData(3);
}

extern const char D_00A51450[];
extern const char D_00A514D8[];
extern int s_anEnemyLevel_1[4];

static int _GetEnemyLevelBySerial(u32 serial)
{
    int level;

    level = 0;
    if (serial < 4U) {
        level = s_anEnemyLevel_1[serial];
    } else {
        RgWarn(D_00A514D8, D_00A51450, 0x103, serial);
    }
    return level;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _GetEnemyHardLevelBySerial);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _GetEnemyChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _GetHardEnemyChar);

extern const char D_00A515C8[];
extern const char D_00A515E0[];
extern const char D_00A515F0[];

static void _InitDataBase(void)
{
    RgReadText *robotText;

    robotText = CreateRgReadText(D_00A515C8);
    RgRobotDBRead(InstanceOfRgRobotDB(), robotText);
    DisposeRgReadText(robotText);
    RgWeaponDBClear(InstanceOfRgWeaponDB());
    RgShotDBClear(InstanceOfRgShotDB());
    RgShotDBRead(InstanceOfRgShotDB(), D_00A515E0);
    RgWeaponDBRead(InstanceOfRgWeaponDB(), D_00A515F0);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _Title);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _SelectOneChar);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _AnnSetRound);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _AnnSetResult);

static void _GameInfoInit(RgGameInfo *info)
{
    int stage;

    info->playerResult[0] = 0;
    info->playerResult[1] = 0;
    info->stage = 0;
    info->activePlayer = 0;
    for (stage = 0; stage < 4; stage++) {
        info->playTime[stage] = 0;
        info->damage[stage] = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _GameInfoEndOfBattle);

static int _GameInfoIsPlayerGetGame(RgGameInfo *info, int player)
{
    if ((unsigned int)info->playerResult[player] < 2) {
        return 0;
    }
    return 1;
}

static int _GameInfoGetCurrentPlayTime(RgGameInfo *info)
{
    return info->playTime[info->stage];
}

static float _GameInfoGetCurrentDamage(RgGameInfo *info)
{
    return info->damage[info->stage];
}

static void _GameInfoNextStage(RgGameInfo *info)
{
    info->playerResult[0] = 0;
    info->playerResult[1] = 0;
    info->stage++;
    info->activePlayer = 0;
}

static int _GameInfoGetStage(RgGameInfo *info)
{
    return info->stage;
}

static int _GameInfoIsEndOfGame(RgGameInfo *info)
{
    int result;

    result = 0;
    if ((u32)info->playerResult[0] >= 2U || (u32)info->playerResult[1] >= 2U) {
        result = 1;
    }
    return result;
}

static void _GameInfoSetDispLife(RgGameInfo *info, RgDispLife *dispLife, int winner)
{
    if (winner < 0) {
        RgDispLifeSetWin(dispLife, info->playerResult[0], info->playerResult[1], 0);
        return;
    }
    RgDispLifeSetWin(dispLife, info->playerResult[0], info->playerResult[1], 1 << winner);
}

static void _LoadBattleData(void)
{
    RgEffectEnvLoadData(InstanceOfRgEffectEnv());
    RgBattleCommonDataLoad(InstanceOfRgBattleCommonData());
    RgMotionInfoDBLoad(InstanceOfRgMotionInfoDB());
}

static void _DisposeBattleDatas(void)
{
    RgMotionInfoDBDispose(InstanceOfRgMotionInfoDB());
    RgBattleCommonDataDispose(InstanceOfRgBattleCommonData());
    RgEffectEnvDisposeData(InstanceOfRgEffectEnv());
    XrgSoundSystemDisposeSequence(0);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _DispPause);

extern const char D_00A51658[];

static void _disp_hardmode(void)
{
    int weaponFont;

    weaponFont = RgBattleCommonDataDispWeaponFont(InstanceOfRgBattleCommonData());
    if (weaponFont == 0) {
        return;
    }
    RgFontStr(InstanceOfXrgPaint2D(), weaponFont, D_00A51658, 8, 0x64, 0);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _OneBattle);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _OneGame);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _VsCpuGame);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _2PGame);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _Help);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _push);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", _pop);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_main", RobotGameMain);

void RobotGameMainDebug(void)
{
}
