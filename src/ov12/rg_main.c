/*
 * OV12 original TU 1: 0x00a00088..0x00a01ff8 (38 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_pad.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_actor_charid.h"
#include "ov12/rg_announce.h"
#include "ov12/rg_battle_init.h"
#include "ov12/rg_battlemgr.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_error.h"
#include "ov12/rg_filesys.h"
#include "ov12/rg_get_frame_time.h"
#include "ov12/rg_help.h"
#include "ov12/rg_select_agws.h"
#include "ov12/rg_select_subcon.h"
#include "ov12/rg_title.h"
#include "ov12/rg_weapon_db.h"
#include "ov12/xrg_pad.h"
#include "ov12/xrg_paint2d.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/xrg_sound.h"
#include "ov12/xrg_sysinit.h"
#include "ov12/xrg_v_log.h"
#include "rg_main.h"

/* libgcc float-to-double helper (shared.h cannot declare it: main/matrix_print.c uses another result type). */
extern double fptodp(float value);

extern XglPadRecord PadData[2];

/* Stages of one round, driven by _OneBattle. */
enum {
    RG_BATTLE_PHASE_START,        /* announce the round */
    RG_BATTLE_PHASE_ROUND_CALL,   /* "round N" shown, READY next */
    RG_BATTLE_PHASE_READY,        /* READY shown, FIGHT next */
    RG_BATTLE_PHASE_FIGHT,        /* players in control until the round ends */
    RG_BATTLE_PHASE_PAUSED,
    RG_BATTLE_PHASE_RESULT,       /* result banner, then the next-step choice */
    RG_BATTLE_PHASE_GAME_OVER,    /* GAME OVER banner until START or timeout */
    RG_BATTLE_PHASE_STAGE_CLEAR,  /* stage clear tally */
    RG_BATTLE_PHASE_GAME_CLEAR,   /* final tally */
    RG_BATTLE_PHASE_DONE
};

/* Seconds each phase waits before moving on. */
#define RG_BATTLE_CALL_TIME 1.0f
#define RG_BATTLE_READY_TIME 1.5f
#define RG_BATTLE_RESULT_TIME 3.0f
#define RG_BATTLE_GAME_OVER_TIME 4.0f
#define RG_BATTLE_STAGE_CLEAR_TIME 5.0f
#define RG_BATTLE_GAME_CLEAR_TIME 60.0f


/*
 * The pause banner's blend color, read by XrgPaint2DColor with one lqc2
 * quadword (src/ov12/xrg_paint2d.c); r/g/b/a in the GS's 0-0x80 scale, same
 * per-component layout as src/ov12/rg_title.c's XrgColor. Copying it here as
 * two 64-bit halves keeps the local copy 8-byte aligned for that quadword
 * read (a 4-member int record copies through ldl/ldr instead).
 */
typedef union XrgColorQuad {
    u64 halves[2];
    XrgPaint2DRGBA rgba;
} XrgColorQuad;


extern const char mainAssertStageDataNotNil[];
extern const char mainStageSerialOverEnemyFormat[];
extern int sRender[];
extern const char mainEmptyWeaponName[];

static void _GetStageData(int stageId, RgBattleInit *data);
static void _GetEnemyChar(RgBattleInitPlayer *names, int player);
static int _GetHardEnemyChar(RgBattleInitPlayer *names, int player,
                             unsigned int stage);
static int _OneBattle(RgBattleMgr *battle, RgBattleInit *init, RgGameInfo *info, int difficulty);
static int _SelectOneChar(RgBattleInit *init, int player);
static void _push(void);
static void _pop(void);
static int _Title(int mode);
static void _VsCpuGame(int difficulty);
static void _2PGame(void);
static void _Help(void);

static int _IsEndOfMode(void)
{
    int result;
    u16 pause;   /* PadData[0].pressed: START gates the debug shortcut */
    u16 buttons; /* PadData[0].buttons: SELECT is the button it reports */

    buttons = PadData[0].buttons;
    pause = PadData[0].pressed;
    result = 0;
    if (InstanceOfRgDebugFlags()->modeEnabled != 0 && (pause & PAD_START) != 0) {
        if (buttons & PAD_SELECT) {
            result = 1;
        }
    }
    return result;
}

/* Returns the controller (0 or 1) whose START press pauses in this battle mode, or -1. */
static int _DoAnyOnePausing(unsigned int mode)
{
    switch (mode) {
    case RG_BATTLE_MODE_PLAYER1_ONLY:
        if ((PadData[0].pressed & PAD_START) != 0) {
            return 0;
        }
        break;
    case RG_BATTLE_MODE_NO_HUMANS:
        if ((PadData[0].pressed & PAD_START) != 0) {
            return 0;
        }
        break;
    case RG_BATTLE_MODE_PLAYER2_ONLY:
        if ((PadData[1].pressed & PAD_START) != 0) {
            return 1;
        }
        break;
    case RG_BATTLE_MODE_BOTH_HUMANS:
        if ((PadData[0].pressed & PAD_START) != 0) {
            return 0;
        }
        if ((PadData[1].pressed & PAD_START) != 0) {
            return 1;
        }
        break;
    }
    return -1;
}

/* Returns PAD_START when START was pressed on controller (on either pad when it is negative), else 0. */
static int _IsEndOfPause(int controller)
{
    u16 held;

    if (controller < 0) {
        held = PadData[0].pressed | PadData[1].pressed;
    } else {
        held = PadData[controller].pressed;
    }
    return held & PAD_START;
}

/* Flushes the 2D paint, particle and draw queues, then calls XrgSleep. */
static void _EndOfFrame(void)
{
    XrgPaint2DFlush(InstanceOfXrgPaint2D());
    XrgParticleDriverDisp(InstanceOfXrgParticleDriver());
    RgDrawJob(InstanceOfRgDraw());
    XrgSleep();
}

const char mainSourcePath[] = "../rg_main.euc.c";

/*
 * Fills data with the background name, start positions and facing of stageId.
 * Unknown stages report an error.
 */
static void _GetStageData(int stageId, RgBattleInit *data)
{
    RgBattleInitPlayer *player[2];
    float zero;
    float height;

    if (data == 0) {
        assert_prog(mainAssertStageDataNotNil, mainSourcePath, 123);
    }
    player[0] = &data->players[0];
    player[1] = &data->players[1];
    switch (stageId) {
    case 0:
        zero = 0.0f;
        height = 1.5f;
        __builtin_strcpy(data->bg, "BG_TOWN");
        XrgSetVectorXYZ(player[0]->position, zero, height, -80.0f);
        XrgSetVectorXYZ(player[1]->position, zero, height, 80.0f);
        XrgSetVectorXYZ(player[0]->direction, 0.0f, zero, 1.0f);
        XrgSetVectorXYZ(player[1]->direction, 0.0f, zero, -1.0f);
        break;
    case 1:
        __builtin_strcpy(data->bg, "BG_AIRCAR");
        zero = 0.0f;
        height = 1.5f;
        XrgSetVectorXYZ(player[0]->position, zero, height, -97.5f);
        XrgSetVectorXYZ(player[1]->position, zero, height, 68.0f);
        XrgSetVectorXYZ(player[0]->direction, 0.0f, zero, 1.0f);
        XrgSetVectorXYZ(player[1]->direction, 0.0f, zero, -1.0f);
        break;
    case 2:
        zero = 0.0f;
        height = 1.5f;
        __builtin_strcpy(data->bg, "BG_COL");
        XrgSetVectorXYZ(player[0]->position, zero, height, -10.0f);
        XrgSetVectorXYZ(player[1]->position, zero, height, 10.0f);
        XrgSetVectorXYZ(player[0]->direction, 0.0f, zero, 1.0f);
        XrgSetVectorXYZ(player[1]->direction, 0.0f, zero, -1.0f);
        break;
    case 3:
        height = 1.5f;
        __builtin_strcpy(data->bg, "BG_BLOCK");
        zero = 0.0f;
        XrgSetVectorXYZ(player[0]->position, -90.0f, height, -80.0f);
        XrgSetVectorXYZ(player[1]->position, 90.0f, height, 80.0f);
        XrgSetVectorXYZ(player[0]->direction, 0.0f, zero, 1.0f);
        XrgSetVectorXYZ(player[1]->direction, 0.0f, zero, -1.0f);
        break;
    case 4:
        zero = 0.0f;
        height = 1.5f;
        __builtin_strcpy(data->bg, "BG_VS1");
        XrgSetVectorXYZ(player[0]->position, zero, height, -10.0f);
        XrgSetVectorXYZ(player[1]->position, zero, height, 10.0f);
        XrgSetVectorXYZ(player[0]->direction, 0.0f, zero, 1.0f);
        XrgSetVectorXYZ(player[1]->direction, 0.0f, zero, -1.0f);
        break;
    default:
        RgError("unknown stage ID %d", mainSourcePath, 163, stageId);
        return;
    }
}

const char mainStageSerialOverEnemyFormat[] = "over stage-serial ID for enemy (%d)";

/* Stage id per serial, indexed 0..3. */
static int s_aeBgTbl_0[RG_STAGE_COUNT] = {0, 1, 2, 3};

/* Stage data for serial; an out-of-range serial warns and uses stage 2. */
static void _GetStageDataBySerial(u32 serial, void *data)
{
    if (serial < RG_STAGE_COUNT) {
        _GetStageData(s_aeBgTbl_0[serial], data);
        return;
    }
    RgWarn(mainStageSerialOverEnemyFormat, mainSourcePath, 180, serial);
    _GetStageData(2, data);
}

/*
 * Same (serial, data) interface as _GetStageDataBySerial. The hard data is
 * always stage 3 and the serial is ignored; the caller's record is forwarded
 * untouched.
 */
static void _GetStageHardDataBySerial(int serial, void *data)
{
    _GetStageData(3, data);
}

/* Normal-mode enemy level per serial, indexed 0..3. */
static int s_anEnemyLevel_1[RG_STAGE_COUNT] = {0x1000, 0x0B15, 0x05FE, 0x1DEF};

static int _GetEnemyLevelBySerial(u32 serial)
{
    int level;

    level = 0;
    if (serial < RG_STAGE_COUNT) {
        level = s_anEnemyLevel_1[serial];
    } else {
        RgWarn(mainStageSerialOverEnemyFormat, mainSourcePath, 0x103, serial);
    }
    return level;
}

const char mainStageSerialOverHardEnemyFormat[] = "over stage-serial ID for (hard) enemy (%d)";

/* Enemy level per serial for hard mode, indexed 0..3. */
static int s_anEnemyLevel_2[RG_STAGE_COUNT] = {0x1DEF, 0x1DEF, 0x1DEF, 0x1DEF};

static int _GetEnemyHardLevelBySerial(u32 serial)
{
    int level;

    level = 0;
    if (serial < RG_STAGE_COUNT) {
        level = s_anEnemyLevel_2[serial];
    } else {
        RgWarn(mainStageSerialOverHardEnemyFormat, mainSourcePath, 325, serial);
    }
    return level;
}

static int abInit_3[2]  = {0, 0};

int s_stack[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/* Picks the enemy character for the player slot and writes its name and three weapon names. */
static void _GetEnemyChar(RgBattleInitPlayer *names, int player)
{
    RgWeaponEssence *weapons[3];
    int charId;
    unsigned int i;

    charId = RgSelectGetEnemyChar();
    if (RgSelectGetEnemyWeapons(weapons, charId) != 0) {
        i = 0;
        while (i < 3) {
            strcpy(names[player].weapons.names[i], weapons[i]->databaseName);
            i++;
        }
    }
    strcpy(names[player].actorName, RgActorCharIDToName(charId));
}

const char mainWeaponNameSmg32Vx[] = "SMG32VX";

const char mainWeaponNameBa15Vx[] = "BA15VX";

const char mainWeaponNameSmp53Ag[] = "SMP53AG";

const char mainWeaponNameDlc02Ag4[] = "DLC02AG4";

const char mainWeaponNameUnarmed[] = "UNARMED";

const char mainWeaponNameCb85Vx[] = "CB85VX";

const char mainWeaponNameHmpAg5[] = "HMP-AG5";

const char mainWeaponNameHggAg5[] = "HGG-AG5";

const char mainWeaponNameBmpAg5[] = "BMP-AG5";

const char mainWeaponNameHg75Vx[] = "HG75VX";

const char mainWeaponNameEcm1Vx[] = "ECM1-VX";

const char mainWeaponNameLg10Ag[] = "LG10AG";

const char mainWeaponNameBsw13Ag[] = "BSW13AG";

const char mainWeaponNameAirdAg2[] = "AIRD-AG2";

const char mainWeaponNameFlm64Ag[] = "FLM64AG";

const char mainWeaponNameLg24Vx[] = "LG24VX";

const char mainWeaponNameBmp45Vx[] = "BMP45VX";

/*
 * Fills the player slot with the hard-mode enemy of stage. Stages 9999 and 9998
 * select the special opponents; other stages use a normal enemy.
 */
static int _GetHardEnemyChar(RgBattleInitPlayer *names, int player, unsigned int stage)
{
    switch (stage) {
    case 0:
        strcpy(names[player].actorName, RgActorCharIDToName(2));
        __builtin_memcpy(names[player].weapons.words[0], mainWeaponNameSmg32Vx, 8);
        __builtin_memcpy(names[player].weapons.words[1], mainWeaponNameBa15Vx, 7);
        __builtin_memcpy(names[player].weapons.words[2], mainWeaponNameSmp53Ag, 8);
        break;
    case 1:
        strcpy(names[player].actorName, RgActorCharIDToName(3));
        __builtin_memcpy(names[player].weapons.words[0], mainWeaponNameDlc02Ag4, 9);
        __builtin_memcpy(names[player].weapons.words[1], mainWeaponNameUnarmed, 8);
        __builtin_memcpy(names[player].weapons.words[2], mainWeaponNameCb85Vx, 7);
        break;
    case 2:
        strcpy(names[player].actorName, RgActorCharIDToName(1));
        __builtin_memcpy(names[player].weapons.words[0], mainWeaponNameHmpAg5, 8);
        __builtin_memcpy(names[player].weapons.words[1], mainWeaponNameHggAg5, 8);
        __builtin_memcpy(names[player].weapons.words[2], mainWeaponNameBmpAg5, 8);
        break;
    case 3:
        strcpy(names[player].actorName, RgActorCharIDToName(4));
        __builtin_memcpy(names[player].weapons.words[0], mainWeaponNameHg75Vx, 7);
        __builtin_memcpy(names[player].weapons.words[1], mainWeaponNameHg75Vx, 7);
        __builtin_memcpy(names[player].weapons.words[2], mainWeaponNameEcm1Vx, 8);
        break;
    case 9999:
        strcpy(names[player].actorName, RgActorCharIDToName(5));
        __builtin_memcpy(names[player].weapons.words[0], mainWeaponNameLg10Ag, 7);
        __builtin_memcpy(names[player].weapons.words[1], mainWeaponNameBsw13Ag, 8);
        __builtin_memcpy(names[player].weapons.words[2], mainWeaponNameAirdAg2, 9);
        break;
    case 9998:
        strcpy(names[player].actorName, RgActorCharIDToName(0));
        __builtin_memcpy(names[player].weapons.words[0], mainWeaponNameFlm64Ag, 8);
        __builtin_memcpy(names[player].weapons.words[1], mainWeaponNameLg24Vx, 7);
        __builtin_memcpy(names[player].weapons.words[2], mainWeaponNameBmp45Vx, 8);
        break;
    default:
        _GetEnemyChar(names, player);
        break;
    }
}

const char mainRobotSpecFileName[] = "rg_robot_spec.info";

const char mainShotInfoFileName[] = "rg_shot.info";

const char mainWeaponInfoFileName[] = "rg_weapon.info";

/* Reads the robot, shot and weapon databases from their info files. */
static void _InitDataBase(void)
{
    RgReadText *robotText;

    robotText = CreateRgReadText(mainRobotSpecFileName);
    RgRobotDBRead(InstanceOfRgRobotDB(), robotText);
    DisposeRgReadText(robotText);
    RgWeaponDBClear(InstanceOfRgWeaponDB());
    RgShotDBClear(InstanceOfRgShotDB());
    RgShotDBRead(InstanceOfRgShotDB(), mainShotInfoFileName);
    RgWeaponDBRead(InstanceOfRgWeaponDB(), mainWeaponInfoFileName);
}

/* Runs the title screen for mode until it returns a result or the mode ends; returns the result. */
static int _Title(int mode)
{
    RgTitle *title;
    int result;

    result = 0;
    RgFileSysPrepareFile(InstanceOfRgFileSys(), "title.npr", "data\\nisimori\\");
    title = CreateRgTitle(mode);
    while (!_IsEndOfMode() && result == 0) {
        RgTitlePassTime(title, RgGetFrameTime());
        RgTitleDisp(title);
        result = RgTitleGetResult(title);
        _EndOfFrame();
    }
    DisposeRgTitle(title);
    RgFileSysDisposePrepares(InstanceOfRgFileSys());
    return result;
}

static struct RgSelectAGWSData s_ainData_4[2];

/*
 * Runs the character select for player slot. Returns the select result; the
 * chosen character and its weapon names are copied into init only when it is 1.
 */
static int _SelectOneChar(RgBattleInit *init, int player)
{
    RgWeaponEssence *weapons[3];
    RgSelectAGWS *selector;
    struct RgSelectAGWSData *selection;
    RgBattleInitPlayer *character;
    int result;
    unsigned int i;

    result = 0;
    if (init == 0) {
        assert_prog("pInfo != NIL", mainSourcePath, 473);
    }
    XrgSleep();
    if (abInit_3[player] == 0) {
        selection = &s_ainData_4[player];
        InitRgSelectAGWSData(selection);
        s_ainData_4[player].selectedCharacter = 0;
        s_ainData_4[player].availableCharacters = RgSelectGetPlayerChars();
        abInit_3[player] = 1;
    }
    selector = CreateRgSelectAGWS();
    RgSelectAGWSSetSelectData(selector, &s_ainData_4[player]);
    switch (player) {
    case 0:
        RgSelectAGWSSetMode(selector, 0);
        break;
    case 1:
        RgSelectAGWSSetMode(selector, 1);
        break;
    }
    while (!_IsEndOfMode()) {
        result = RgSelectAGWSIsEnd(selector);
        if (result != 0) {
            break;
        }
        RgSelectAGWSPassTime(selector, RG_FRAME_TIME);
        RgSelectAGWSDisp(selector);
        _EndOfFrame();
    }
    XrgSleep();
    if (result == 1) {
        character = &init->players[player];
        RgSelectAGWSGetSelectData(selector, &s_ainData_4[player]);
        strcpy(character->actorName,
               RgActorCharIDToName(RgSelectAGWSGetCollectData(selector, weapons)));
        for (i = 0; i < 3; i++) {
            if (weapons[i] == 0) {
                character->weapons.names[i][0] = mainEmptyWeaponName[0];
            } else {
                strcpy(character->weapons.names[i], weapons[i]->databaseName);
            }
        }
    }
    DisposeRgSelectAGWS(selector);
    XrgSleep();
    XrgSleep();
    return result;
}

const char mainEmptyWeaponName[16] = {0};

static void _AnnSetRound(RgAnnounce *pAnn, u32 round)
{
    switch (round) {
    case 0:
        RgAnnounceDispInit(pAnn, RG_ANNOUNCE_ROUND1);
        return;
    case 1:
        RgAnnounceDispInit(pAnn, round);
        return;
    case 2:
        RgAnnounceDispInit(pAnn, round);
        return;
    }
}

/* Banner for a finished round, from the viewpoint of the human player(s) in humanMask. */
static void _AnnSetResult(RgAnnounce *announcement, int battleResult, int humanMask,
                          float timer)
{
    int winner;
    int endedKind;
    int humans;

    winner = battleResult & RG_BATTLE_RESULT_WINNER_MASK;
    endedKind = battleResult & RG_BATTLE_RESULT_ENDED_MASK;
    if (winner == RG_BATTLE_RESULT_DRAW) {
        RgAnnounceDispInit(announcement, RG_ANNOUNCE_DRAW);
        return;
    }
    if (endedKind == RG_BATTLE_RESULT_TIME_OVER && timer > 1.5f) {
        RgAnnounceDispInit(announcement, RG_ANNOUNCE_TIME_OVER);
        return;
    }
    humans = humanMask & RG_BATTLE_MODE_BOTH_HUMANS;
    if (humans == RG_BATTLE_MODE_PLAYER1_ONLY) {
        if (winner == RG_BATTLE_RESULT_PLAYER1_WINS) {
            RgAnnounceDispInit(announcement, RG_ANNOUNCE_YOU_WIN);
            return;
        }
        RgAnnounceDispInit(announcement, RG_ANNOUNCE_YOU_LOSE);
        return;
    }
    if (humans == RG_BATTLE_MODE_PLAYER2_ONLY) {
        if (winner == RG_BATTLE_RESULT_PLAYER1_WINS) {
            RgAnnounceDispInit(announcement, RG_ANNOUNCE_YOU_LOSE);
            return;
        }
        RgAnnounceDispInit(announcement, RG_ANNOUNCE_YOU_WIN);
        return;
    }
    if (humans == RG_BATTLE_MODE_BOTH_HUMANS) {
        if (winner == RG_BATTLE_RESULT_PLAYER1_WINS) {
            RgAnnounceDispInit(announcement, RG_ANNOUNCE_YOU_WIN_YOU_LOSE);
            return;
        }
        RgAnnounceDispInit(announcement, RG_ANNOUNCE_YOU_LOSE_YOU_WIN);
        return;
    }
}

static void _GameInfoInit(RgGameInfo *info)
{
    int stage;

    info->playerResult[0] = 0;
    info->playerResult[1] = 0;
    info->stage = 0;
    info->activePlayer = 0;
    for (stage = 0; stage < RG_STAGE_COUNT; stage++) {
        info->playTime[stage] = 0;
        info->damage[stage] = 0;
    }
}

static void _GameInfoEndOfBattle(RgGameInfo *info, int player, RgBattleMgr *battleMgr)
{
    if (player >= 0) {
        info->playerResult[player]++;
        info->activePlayer++;
    }
    info->playTime[info->stage] += (int)(RgBattleMgrGetPlayTime(battleMgr) * 100.0f);
    info->damage[info->stage] += RgBattleMgrGetPlayerDamage(battleMgr, 0);
}

static int _GameInfoIsPlayerGetGame(RgGameInfo *info, int player)
{
    if ((unsigned int)info->playerResult[player] < RG_ROUNDS_TO_WIN) {
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
    if ((u32)info->playerResult[0] >= RG_ROUNDS_TO_WIN || (u32)info->playerResult[1] >= RG_ROUNDS_TO_WIN) {
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

/* The pause banner's blend colour: r, g, b, a = 0x5A, 0x5A, 0x5A, 0x7F. */
const XrgColorQuad mainPauseColor = {{0x0000005A0000005AULL, 0x0000007F0000005AULL}};

const char mainPauseText[] = "PAUSE";

/* Draws the PAUSE banner, its panel and text, when the weapon font and paint handle exist. */
static void _DispPause(void)
{
    XrgPaint2D *paint;
    RgFont *font;
    XrgColorQuad color;

    paint = InstanceOfXrgPaint2D();
    font = RgBattleCommonDataDispWeaponFont(InstanceOfRgBattleCommonData());
    color = mainPauseColor;
    if (font != 0 && paint != 0) {
        XrgPaint2DAlpha(paint, 1);
        XrgPaint2DColor(paint, &color.rgba);
        XrgPaint2DDrawXYWH(paint, 0, 0xE2, 0xCC, 0x3C, 0x1C);
        XrgPaint2DAlpha(paint, 2);
        XrgPaint2DColor(paint, &color.rgba);
        XrgPaint2DDrawXYWH(paint, 0, 0xE4, 0xCE, 0x38, 0x18);
        RgFontStr(paint, font, mainPauseText, 0xEC, 0xD2, 0);
    }
}

const char mainHardModeText[] = "HARD MODE";

static void _disp_hardmode(void)
{
    RgFont *weaponFont;

    weaponFont = RgBattleCommonDataDispWeaponFont(InstanceOfRgBattleCommonData());
    if (weaponFont == 0) {
        return;
    }
    RgFontStr(InstanceOfXrgPaint2D(), weaponFont, mainHardModeText, 8, 0x64, 0);
}

const char mainAssertBattleMgrNotNil[] = "pMgr != NIL";

/*
 * Runs one battle through its phases (round call, ready, fight, pause, result,
 * game over, stage clear, game clear). Returns nonzero when the mode ended, which
 * tells the caller to stop playing.
 */
static int _OneBattle(RgBattleMgr *battle, RgBattleInit *init, RgGameInfo *info, int difficulty)
{
    RgAnnounce *announce;
    RgDispLife *dispLife;
    unsigned int mode;
    float frameTime;
    float phaseTimer;
    unsigned int battlePhase;
    int simulateFrame;
    int drawnOnce;
    int pauseController;
    int battleResult;
    int playTime;
    float damage;
    int modeEnded;

    drawnOnce = 0;
    pauseController = -1;
    if (battle == 0) {
        assert_prog(mainAssertBattleMgrNotNil, mainSourcePath, 755);
    }
    battlePhase = RG_BATTLE_PHASE_START;
    announce = CreateRgAnnounce();
    RgBattleMgrInitBattle(battle, init);
    dispLife = RgBattleMgrGetDispLife(battle);
    phaseTimer = RG_BATTLE_CALL_TIME;
    _GameInfoSetDispLife(info, dispLife, -1);
    battleResult = 0;
    mode = RgBattleMgrGetMode(battle);
    while ((modeEnded = _IsEndOfMode()) == 0 && battlePhase != RG_BATTLE_PHASE_DONE) {
        frameTime = RgGetFrameTime();
        phaseTimer -= frameTime;
        simulateFrame = 1;
        if (battlePhase < RG_BATTLE_PHASE_DONE) {
            /* Each case sets the phase for the next frame. */
            switch (battlePhase) {
            case RG_BATTLE_PHASE_START:
                battlePhase = RG_BATTLE_PHASE_ROUND_CALL;
                _AnnSetRound(announce, info->activePlayer);
                /* fallthrough */
            case RG_BATTLE_PHASE_ROUND_CALL:
                if (phaseTimer < 0.0f) {
                    phaseTimer = RG_BATTLE_READY_TIME;
                    battlePhase = RG_BATTLE_PHASE_READY;
                    RgAnnounceDispInit(announce, RG_ANNOUNCE_READY);
                }
                break;
            case RG_BATTLE_PHASE_READY:
                if (phaseTimer < 0.0f) {
                    battlePhase = RG_BATTLE_PHASE_FIGHT;
                    RgAnnounceDispInitTime(announce, RG_ANNOUNCE_FIGHT, 0.5f);
                    RgBattleMgrSetPlayerControl(battle, 1);
                }
                break;
            case RG_BATTLE_PHASE_FIGHT:
                battleResult = RgBattleMgrGetResult(battle);
                pauseController = _DoAnyOnePausing(mode);
                if (pauseController >= 0) {
                    battlePhase = RG_BATTLE_PHASE_PAUSED;
                }
                if (battleResult != 0) {
                    int winner;

                    RgBattleMgrActivateTimer(battle, 0);
                    phaseTimer = RG_BATTLE_RESULT_TIME;
                    battlePhase = RG_BATTLE_PHASE_RESULT;
                    winner = battleResult & RG_BATTLE_RESULT_WINNER_MASK;
                    switch (winner) {
                    default:
                        _GameInfoEndOfBattle(info, -1, battle);
                        break;
                    case RG_BATTLE_RESULT_PLAYER1_WINS:
                        _GameInfoEndOfBattle(info, 0, battle);
                        _GameInfoSetDispLife(info, dispLife, 0);
                        break;
                    case RG_BATTLE_RESULT_PLAYER2_WINS:
                        _GameInfoEndOfBattle(info, 1, battle);
                        _GameInfoSetDispLife(info, dispLife, 1);
                        break;
                    }
                }
                break;
            case RG_BATTLE_PHASE_RESULT:
                _AnnSetResult(announce, battleResult, RgBattleMgrGetMode(battle), phaseTimer);
                if (!(phaseTimer >= 0.0f)) {
                    int winner;

                    RgBattleMgrSetPlayerControl(battle, 0);
                    winner = battleResult & RG_BATTLE_RESULT_WINNER_MASK;
                    /* After the result banner, the mode picks the next step. */
                    switch (mode) {
                    case RG_BATTLE_MODE_PLAYER1_ONLY:
                        if (winner == mode && _GameInfoIsPlayerGetGame(info, 0) != 0) {
                            battlePhase = RG_BATTLE_PHASE_STAGE_CLEAR;
                            playTime = _GameInfoGetCurrentPlayTime(info);
                            damage = _GameInfoGetCurrentDamage(info);
                            phaseTimer = RG_BATTLE_STAGE_CLEAR_TIME;
                            RgAnnounceDispInit(announce, RG_ANNOUNCE_STAGE_CLEAR);
                            RgAnnounceSetStageClear(announce, playTime, damage);
                        } else if (winner == RG_BATTLE_RESULT_PLAYER2_WINS && _GameInfoIsPlayerGetGame(info, 1) != 0) {
                            phaseTimer = RG_BATTLE_GAME_OVER_TIME;
                            battlePhase = RG_BATTLE_PHASE_GAME_OVER;
                            RgAnnounceDispInit(announce, RG_ANNOUNCE_GAME_OVER);
                        } else {
                            battlePhase = RG_BATTLE_PHASE_DONE;
                        }
                        break;
                    case RG_BATTLE_MODE_BOTH_HUMANS:
                        if (_GameInfoIsEndOfGame(info) != 0) {
                            RgAnnounceDispInit(announce, RG_ANNOUNCE_GAME_OVER);
                            phaseTimer = RG_BATTLE_GAME_OVER_TIME;
                            battlePhase = RG_BATTLE_PHASE_GAME_OVER;
                        } else {
                            battlePhase = RG_BATTLE_PHASE_DONE;
                        }
                        break;
                    case RG_BATTLE_MODE_NO_HUMANS:
                    case RG_BATTLE_MODE_PLAYER2_ONLY:
                    default:
                        battlePhase = RG_BATTLE_PHASE_DONE;
                        break;
                    }
                }
                break;
            case RG_BATTLE_PHASE_GAME_OVER:
                if (phaseTimer <= 0.0f) {
                    battlePhase = RG_BATTLE_PHASE_DONE;
                }
                XrgPadSetID(0);
                if (XrgPadIsStart() != 0) {
                    battlePhase = RG_BATTLE_PHASE_DONE;
                }
                XrgPadSetID(1);
                if (XrgPadIsStart() != 0) {
                    battlePhase = RG_BATTLE_PHASE_DONE;
                }
                break;
            case RG_BATTLE_PHASE_STAGE_CLEAR:
                XrgPadSetID(0);
                if (phaseTimer <= 0.0f || XrgPadIsStart() != 0) {
                    if (_GameInfoGetStage(info) == 3) {
                        phaseTimer = RG_BATTLE_GAME_CLEAR_TIME;
                        battlePhase = RG_BATTLE_PHASE_GAME_CLEAR;
                        RgAnnounceDispInit(announce, RG_ANNOUNCE_GAME_CLEAR);
                        RgAnnounceSetGameClear(announce, info->playTime, info->damage, RG_STAGE_COUNT);
                    } else {
                        battlePhase = RG_BATTLE_PHASE_DONE;
                    }
                }
                if (difficulty != 0) {
                    _disp_hardmode();
                }
                break;
            case RG_BATTLE_PHASE_GAME_CLEAR:
                XrgPadSetID(0);
                if (XrgPadIsStart() != 0 || phaseTimer <= 0.0f) {
                    battlePhase = RG_BATTLE_PHASE_DONE;
                }
                if (difficulty != 0) {
                    _disp_hardmode();
                }
                break;
            case RG_BATTLE_PHASE_PAUSED:
                simulateFrame = 0;
                if (_IsEndOfPause(pauseController) != 0) {
                    battlePhase = RG_BATTLE_PHASE_FIGHT;
                }
                break;
            }
        }
        /* Simulate unless paused; the battle is drawn from the second frame on. */
        if (simulateFrame != 0) {
            RgBattleMgrControl(battle);
            RgBattleMgrPassTime(battle, frameTime);
            RgAnnouncePassTime(announce, frameTime);
        }
        if (drawnOnce != 0) {
            RgBattleMgrDisp(battle);
        } else {
            drawnOnce = 1;
        }
        RgAnnounceDisp(announce);
        if (battlePhase == RG_BATTLE_PHASE_PAUSED) {
            _DispPause();
        }
        _EndOfFrame();
    }
    XrgSleep();
    XrgSleep();
    XrgSoundSystemStop();
    DisposeRgAnnounce(announce);
    return modeEnded;
}

/*
 * Plays battles of one game until it is over or a battle ends the mode. Returns
 * nonzero when the mode was not ended.
 */
static int _OneGame(RgBattleInit *init, RgGameInfo *info, int difficulty)
{
    RgBattleMgr *battle;
    int continuing;

    continuing = 1;
    XrgSoundSystemPlayBGM(0);
    while (continuing && !_GameInfoIsEndOfGame(info)) {
        battle = CreateRgBattleMgr();
        continuing = _OneBattle(battle, init, info, difficulty) == 0;
        RgBattleMgrSetPlayerControl(battle, 0);
        DisposeRgBattleMgr(battle);
    }
    return continuing;
}

const char mainHardModeWarning[] = "This game is 'HARD' mode";

const char mainLogSeparatorFormat[] = "--------------------------------------------------\n";

const char mainLogStageLevelFormat[] = "stage %s : level %d\n";

const char mainLogModeFormat[] = "mode %x\n";

const char mainLogEnemyLevelFormat[] = "enemy lev %x\n";

const char mainLogNewlineFormat[] = "\n";

const char mainLogPlayer1Format[] = "player 1 '%s'\n";

const char mainLogPositionFormat[] = "pos %f %f %f\n";

const char mainLogDirectionFormat[] = "dir %f %f %f\n";

const char mainLogWeaponLeftFormat[] = "weapon left  '%s'\n";

const char mainLogWeaponRightFormat[] = "weapon right '%s'\n";

const char mainLogWeaponBackFormat[] = "weapon back  '%s'\n";

const char mainLogPlayer2Format[] = "player 2 '%s'\n";

/*
 * Plays the vs-CPU game: character select, then the stages in order, each with
 * its enemy, until the game ends early or all stages are played. difficulty
 * selects the hard data.
 */
static void _VsCpuGame(int difficulty)
{
    RgBattleInit init;
    RgGameInfo info;
    int serial;

    InitRgBattleInit(&init);
    _LoadBattleData();
    if (difficulty != 0) {
        RgWarn(mainHardModeWarning, mainSourcePath, 1039);
    }
    if (_SelectOneChar(&init, 0) == 1) {
        _GameInfoInit(&info);
        while ((unsigned int)_GameInfoGetStage(&info) < RG_STAGE_COUNT) {
            serial = _GameInfoGetStage(&info);
            serial %= RG_STAGE_COUNT;
            if (difficulty != 0) {
                init.enemyType = _GetEnemyHardLevelBySerial(serial);
            } else {
                init.enemyType = _GetEnemyLevelBySerial(serial);
            }
            if (difficulty != 0) {
                _GetStageDataBySerial(serial, &init);
            } else {
                _GetStageDataBySerial(serial, &init);
            }
            init.mode = RG_BATTLE_MODE_VS_CPU;
            if (difficulty != 0) {
                _GetHardEnemyChar(init.players, 1, serial);
            } else {
                _GetEnemyChar(init.players, 1);
            }
            /* Logs the battle setup: mode, enemy level and both players. */
            XrgLog(mainLogSeparatorFormat, mainSourcePath, 1091);
            XrgLog(mainLogStageLevelFormat, mainSourcePath, 1092, init.bg, serial);
            XrgLog(mainLogModeFormat, mainSourcePath, 1093, init.mode);
            XrgLog(mainLogEnemyLevelFormat, mainSourcePath, 1094, init.enemyType);
            XrgLog(mainLogNewlineFormat, mainSourcePath, 1095);
            XrgLog(mainLogPlayer1Format, mainSourcePath, 1096, &init);
            XrgLog(mainLogPositionFormat, mainSourcePath, 1097,
                   fptodp(init.players[0].position[0]),
                   fptodp(init.players[0].position[1]),
                   fptodp(init.players[0].position[2]));
            XrgLog(mainLogDirectionFormat, mainSourcePath, 1098,
                   fptodp(init.players[0].direction[0]),
                   fptodp(init.players[0].direction[1]),
                   fptodp(init.players[0].direction[2]));
            XrgLog(mainLogWeaponLeftFormat, mainSourcePath, 1099, init.players[0].weapons.names[0]);
            XrgLog(mainLogWeaponRightFormat, mainSourcePath, 1100, init.players[0].weapons.names[1]);
            XrgLog(mainLogWeaponBackFormat, mainSourcePath, 1101, init.players[0].weapons.names[2]);
            XrgLog(mainLogPlayer2Format, mainSourcePath, 1102, &init.players[1]);
            XrgLog(mainLogPositionFormat, mainSourcePath, 1103,
                   fptodp(init.players[1].position[0]),
                   fptodp(init.players[1].position[1]),
                   fptodp(init.players[1].position[2]));
            XrgLog(mainLogDirectionFormat, mainSourcePath, 1104,
                   fptodp(init.players[1].direction[0]),
                   fptodp(init.players[1].direction[1]),
                   fptodp(init.players[1].direction[2]));
            XrgLog(mainLogWeaponLeftFormat, mainSourcePath, 1105, init.players[1].weapons.names[0]);
            XrgLog(mainLogWeaponRightFormat, mainSourcePath, 1106, init.players[1].weapons.names[1]);
            XrgLog(mainLogWeaponBackFormat, mainSourcePath, 1107, init.players[1].weapons.names[2]);
            XrgLog(mainLogSeparatorFormat, mainSourcePath, 1108);
            if (_OneGame(&init, &info, difficulty) == 0 ||
                !_GameInfoIsPlayerGetGame(&info, 0)) {
                break;
            }
            _GameInfoNextStage(&info);
        }
        XrgSoundSystemStopSequence();
    }
    _DisposeBattleDatas();
}

/* Two-player game: selects both characters, then plays stage 4 when both selections are valid. */
static void _2PGame(void)
{
    int result[2];
    RgBattleInit init;
    RgGameInfo info;

    InitRgBattleInit(&init);
    _LoadBattleData();
    result[0] = 0;
    result[1] = 0;
    do {
        result[0] = _SelectOneChar(&init, 0);
        if (result[0] == 2) {
            break;
        }
        result[1] = _SelectOneChar(&init, 1);
    } while (result[1] != 1);
    _GetStageData(4, &init);
    init.mode = RG_BATTLE_MODE_TWO_PLAYERS;
    if (result[0] == 1 && result[1] == result[0]) {
        _GameInfoInit(&info);
        _OneGame(&init, &info, 0);
    }
    XrgSoundSystemStopSequence();
    _DisposeBattleDatas();
}

const char mainHelpFileName[16] = "help.npr";

const char mainUnknownTitleStatusFormat[24] = "unknown title status";

/* Runs the help screen until it reports the end or the mode ends. */
static void _Help(void)
{
    RgHelp *help;
    int done;

    done = 0;
    RgFileSysPrepareFile(InstanceOfRgFileSys(), mainHelpFileName, "data\\nisimori\\");
    help = CreateRgHelp();
    while (!done && !_IsEndOfMode()) {
        RgHelpPassTime(help, RgGetFrameTime());
        RgHelpDisp(help);
        done = RgHelpIsEnd(help);
        _EndOfFrame();
    }
    DisposeRgHelp(help);
    RgFileSysDisposePrepares(InstanceOfRgFileSys());
}

/* Saves sRender words 9 to 16 into s_stack and clears them. */
static void _push(void)
{
    int i;
    int stateWord;

    for (i = 0; i < 8; i++) {
        stateWord = sRender[i + 9];
        sRender[i + 9] = 0;
        s_stack[i] = stateWord;
    }
}

/* Restores sRender words 9 to 16 from s_stack. */
static void _pop(void)
{
    int i;

    for (i = 0; i < 8; i++) {
        sRender[i + 9] = s_stack[i];
    }
}

/* Entry point of the robot game: runs the title menu and the chosen mode until the player exits. */
void RobotGameMain(void)
{
    int running;
    int titleCount;

    running = 1;
    _push();
    titleCount = 0;
    XrgSleep();
    XrgSystemInit();
    InitXrgSoundSystem();
    while (running) {
        int selection;

        XrgSystemInit();
        XrgSoundSystemPlayBGM(3);
        _InitDataBase();
        selection = _Title(titleCount++);
        XrgSleep();
        switch (selection) {
        case 1:
            _VsCpuGame(0);
            break;
        case 2:
            _VsCpuGame(1);
            break;
        case 3:
            _2PGame();
            break;
        case 4:
            _Help();
            break;
        case 0:
            running = 0;
            break;
        case 5:
            running = 0;
            break;
        default:
            RgError(mainUnknownTitleStatusFormat, mainSourcePath, 1258);
            break;
        }
        RgDrawCreateDrawStudioFullScreen(InstanceOfRgDraw());
        RgFileSysClear(InstanceOfRgFileSys());
        XrgSystemDispose();
        XrgSleep();
        XrgSleep();
    }
    DisposeXrgSoundSystem();
    _pop();
}

/* Does nothing. */
void RobotGameMainDebug(void)
{
}



const char mainAssertStageDataNotNil[16] = "pInitDat != NIL";
