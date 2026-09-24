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

/*
 * ov12:0x00a514d8 contains the format string "over stage-serial ID for
 * enemy (%d)"; ov12:0x00a51450 contains the source filename
 * "../rg_main.euc.c".
 */
extern const char D_00A514D8[];
extern const char D_00A51450[];

/* Stage id per serial, indexed 0..3 (ov12:0x00a4f4c8, size 0x10). */
extern int s_aeBgTbl_0[4];

static void _GetStageData(int stageId, void *data);

static void _GetStageDataBySerial(u32 serial, void *data)
{
    if (serial < 4U) {
        _GetStageData(s_aeBgTbl_0[serial], data);
        return;
    }
    RgWarn(D_00A514D8, D_00A51450, 180, serial);
    _GetStageData(2, data);
}

/* data: the caller's output record (asserted non-null; layout not yet recovered). */
static void _GetStageData(int stageId, void *data);

/* Same (serial, data) interface as _GetStageDataBySerial; the hard data is
 * always stage 3 and the caller's record is forwarded untouched. */
static void _GetStageHardDataBySerial(int serial, void *data)
{
    _GetStageData(3, data);
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

/* ov12:0x00a51500 contains "over stage-serial ID for (hard) enemy (%d)". */
extern const char D_00A51500[];

/* Enemy level per serial for hard mode, indexed 0..3 (ov12:0x00a4f4e8, size 0x10). */
extern int s_anEnemyLevel_2[4];

static int _GetEnemyHardLevelBySerial(u32 serial)
{
    int level;

    level = 0;
    if (serial < 4U) {
        level = s_anEnemyLevel_2[serial];
    } else {
        RgWarn(D_00A51500, D_00A51450, 325, serial);
    }
    return level;
}

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

/* RgAnnounce is defined by its owner, ov12/tu077 rg_announce.c. */
typedef struct RgAnnounce RgAnnounce;
void RgAnnounceDispInit(RgAnnounce *pAnn, int kind);

static void _AnnSetRound(RgAnnounce *pAnn, u32 round)
{
    switch (round) {
    case 0:
        RgAnnounceDispInit(pAnn, 0);
        return;
    case 1:
        RgAnnounceDispInit(pAnn, round);
        return;
    case 2:
        RgAnnounceDispInit(pAnn, round);
        return;
    }
}

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

/*
 * The pause banner's blend color, read by XrgPaint2DColor with one lqc2
 * quadword (src/ov12/xrg_paint2d.c); r/g/b/a in the GS's 0-0x80 scale, same
 * per-component layout as src/ov12/rg_title.c's XrgColor. Copying it here as
 * two 64-bit halves keeps the local copy 8-byte aligned for that quadword
 * read (a 4-member int record copies through ldl/ldr instead).
 */
typedef struct XrgColorQuad {
    u64 lo;
    u64 hi;
} XrgColorQuad;

void XrgPaint2DAlpha(XrgPaint2D *paint, int blendMode);
void XrgPaint2DColor(XrgPaint2D *paint, const void *color);
void XrgPaint2DDrawXYWH(XrgPaint2D *paint, int mode, int x, int y, int width,
                        int height);

/* ov12:0x00a51640 (0x5a,0x5a,0x5a,0x7f): the pause banner's blend color. */
extern const XrgColorQuad D_00A51640;

/* ov12:0x00a51650 contains "PAUSE". */
extern const char D_00A51650[];

static void _DispPause(void)
{
    XrgPaint2D *paint;
    int fontId;
    XrgColorQuad color;

    paint = InstanceOfXrgPaint2D();
    fontId = RgBattleCommonDataDispWeaponFont(InstanceOfRgBattleCommonData());
    color = D_00A51640;
    if (fontId != 0 && paint != 0) {
        XrgPaint2DAlpha(paint, 1);
        XrgPaint2DColor(paint, &color);
        XrgPaint2DDrawXYWH(paint, 0, 0xE2, 0xCC, 0x3C, 0x1C);
        XrgPaint2DAlpha(paint, 2);
        XrgPaint2DColor(paint, &color);
        XrgPaint2DDrawXYWH(paint, 0, 0xE4, 0xCE, 0x38, 0x18);
        RgFontStr(paint, fontId, D_00A51650, 0xEC, 0xD2, 0);
    }
}

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
