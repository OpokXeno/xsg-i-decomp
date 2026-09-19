/*
 * OV12 original TU 47: 0x00a28858..0x00a2ab70 (53 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_battlemgr.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);
extern void RgHeapFree(void *heap, void *block, const char *source_file,
                       int line);
extern RgHeap *InstanceOfRgHeap(void);

/* ov12/tu016 (src/ov12/rg_camera.h), still INCLUDE_ASM there. */
extern RgCamera *CreateRgCamera(int type, RgDrawStudio *pStudio);
extern void RgCameraLoadText(RgCamera *pCamera, RgReadText *pReader);

/* ov12/tu036 (src/ov12/rg_read_text.h). CreateRgReadText/RgReadTextRewind are
 * already accepted there; DisposeRgReadText and RgReadTextFindParagraph are
 * still INCLUDE_ASM. */
extern RgReadText *CreateRgReadText(const char *pszName);
extern void DisposeRgReadText(RgReadText *pReader);
extern void RgReadTextRewind(RgReadText *pReader);
extern int RgReadTextFindParagraph(RgReadText *pReader, const char *pszTag,
                                   const char *pszSubTag);

/* ov12/tu059 (src/ov12/rg_geom_group.h), already accepted there. */
extern void DisposeRgGeomGroup(RgGeomGroup *pGroup);

/* Linker witness: this TU's own source-file name, used by every assert here. */
extern const char D_00A54CC0[];
/* Linker witness for the original literal at ov12:0x00a54ca0
 * ("rg_camera.info"), the battle camera's read-text source file. */
extern const char D_00A54CA0[];
/* Linker witness for the original literal at ov12:0x00a54cb0
 * ("pStudio != NIL"). */
extern const char D_00A54CB0[];
/* Linker witness for the original literal at ov12:0x00a54cd8 ("CAM"), the
 * paragraph tag _CreateBattleCamera looks up. */
extern const char D_00A54CD8[];
/* Linker witness for the original literal at ov12:0x00a54ce0 ("2"), the
 * sub-tag RgReadTextFindParagraph compares (strcmp) with the string that
 * follows the "CAM" paragraph tag. */
extern const char D_00A54CE0[];
/* Linker witness for the original literal at ov12:0x00a54ce8
 * ("pList != NIL"). */
extern const char D_00A54CE8[];
/* Linker witness for the original literal at ov12:0x00a54d50
 * ("pField != NIL"). */
extern const char D_00A54D50[];
/* Linker witness for the original literal at ov12:0x00a54d98 ("pDisp != NIL"). */
extern const char D_00A54D98[];
/* Linker witness for the original literal at ov12:0x00a54da8 ("pInfo != NIL"). */
extern const char D_00A54DA8[];
/* Linker witness for the original literal at ov12:0x00a54de8 ("pMgr != NIL"). */
extern const char D_00A54DE8[];

static RgCamera *_CreateBattleCamera(RgDrawStudio *pStudio)
{
    RgReadText *pReader = CreateRgReadText(D_00A54CA0);
    RgCamera *camera;

    if (pStudio == 0) {
        assert_prog(D_00A54CB0, D_00A54CC0, 0x41);
    }
    camera = CreateRgCamera(2, pStudio);
    RgReadTextRewind(pReader);
    if (RgReadTextFindParagraph(pReader, D_00A54CD8, D_00A54CE0) != 0) {
        RgCameraLoadText(camera, pReader);
    }
    DisposeRgReadText(pReader);
    return camera;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _GetDeadList);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _GetBigLifePlayer);

static int _GetNumOfPlayerList(PlayerList *pList)
{
    if (pList == 0) {
        assert_prog(D_00A54CE8, D_00A54CC0, 0x79);
    }
    return pList->count;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _GetInPlayerList);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _AddPlayerList);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _ClearPlayerList);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _ControlPlayerList);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _PassTimePlayerList);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _DispPlayerList);

static void _InitPlayerList(PlayerList *pList)
{
    if (pList == 0) {
        assert_prog(D_00A54CE8, D_00A54CC0, 0xCC);
    }
    pList->player1P = 0;
    pList->player2P = 0;
    pList->count = 0;
}

static void _ClearPlayerList(PlayerList *pList);

static void _DestructPlayerList(PlayerList *pList)
{
    _ClearPlayerList(pList);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _CreatePlayerList);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _DisposePlayerList);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _GetStudioBattleField);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _GetCameraBattleField);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _ClearBattleField);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _InitBattleField);

static void _ClearBattleField(BattleField *pField);

static void _DestructBattleField(BattleField *pField)
{
    if (pField == 0) {
        assert_prog(D_00A54D50, D_00A54CC0, 0x129);
    }
    _ClearBattleField(pField);
    DisposeRgGeomGroup(pField->geomGroup);
}

static void _InitBattleField(BattleField *pField);

static BattleField *_CreateBattleField(void)
{
    BattleField *pField = RgHeapAlloc(InstanceOfRgHeap(), sizeof(BattleField),
                                      D_00A54CC0, 0x131);

    _InitBattleField(pField);
    return pField;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _DisposeBattleField);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _FullScreenBattleField);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _DoubleScreenBattleField);

static RgGeomGroup *_GetGeomGroupBattleField(BattleField *pField)
{
    if (pField == 0) {
        assert_prog(D_00A54D50, D_00A54CC0, 0x181);
    }
    return pField->geomGroup;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _PassTimeBattleField);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _InitDispInfo);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _DestructDispInfo);

void _InitDispInfo(DispInfo *pInfo);

static DispInfo *_CreateDispInfo(void)
{
    DispInfo *pInfo = RgHeapAlloc(InstanceOfRgHeap(), sizeof(DispInfo), D_00A54CC0, 479);

    _InitDispInfo(pInfo);
    return pInfo;
}

void _DestructDispInfo(DispInfo *pInfo);

static void _DisposeDispInfo(DispInfo *pInfo)
{
    if (pInfo == 0) {
        assert_prog(D_00A54DA8, D_00A54CC0, 487);
    }
    _DestructDispInfo(pInfo);
    RgHeapFree(InstanceOfRgHeap(), pInfo, D_00A54CC0, 489);
}

static void _SetDispInfo(DispInfo *pDisp, int mode, RgPlayer *player1P,
                         RgPlayer *player2P)
{
    if (pDisp == 0) {
        assert_prog(D_00A54D98, D_00A54CC0, 495);
    }
    pDisp->mode = mode;
    pDisp->player1P = player1P;
    pDisp->player2P = player2P;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _DrawDispInfoOnePlayer);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _DrawDispInfo);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _InitBattleMgr);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _DisposeBattleMgr);

static void _BattleMgrPlayerControl(RgBattleMgr *pMgr, int enable)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 640);
    }
    pMgr->playerControl = enable;
}

static void _BattleMgrActivateTime(RgBattleMgr *pMgr, int active)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 647);
    }
    pMgr->timerActive = active;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _InitBattle);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _ControlBattle);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _DispBattle);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _PassTimeBattle);

void _InitBattleMgr(RgBattleMgr *pMgr);

RgBattleMgr *CreateRgBattleMgr(void)
{
    RgBattleMgr *pMgr = RgHeapAlloc(InstanceOfRgHeap(), 0x38, D_00A54CC0, 1015);

    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 1016);
    }
    _InitBattleMgr(pMgr);
    return pMgr;
}

void _DisposeBattleMgr(RgBattleMgr *pMgr);

void DisposeRgBattleMgr(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 1023);
    }
    _DisposeBattleMgr(pMgr);
    RgHeapFree(InstanceOfRgHeap(), pMgr, D_00A54CC0, 1025);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", RgBattleMgrSetPlayerControl);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", RgBattleMgrActivateTimer);

/* The battle's fixed time limit in seconds; RgBattleMgrGetPlayTime returns
 * the time played so far as the limit minus the remaining-time field. */
#define RG_BATTLE_TIME_LIMIT 180.0f

float RgBattleMgrGetPlayTime(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 1052);
    }
    return RG_BATTLE_TIME_LIMIT - pMgr->remainingTime;
}

int RgBattleMgrGetResult(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 1059);
    }
    return pMgr->result;
}

int RgBattleMgrGetMode(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 1067);
    }
    return pMgr->mode;
}

RgDispLife *RgBattleMgrGetDispLife(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 1074);
    }
    return pMgr->dispLife;
}

extern RgRobot *RgPlayerGetRobot(RgPlayer *pPlayer);
extern float RgRobotGetLife(RgRobot *pRobot);
extern float RgRobotGetLifeMax(RgRobot *pRobot);
extern void XrgLog(const char *format, const char *source_file, int line,
                   ...);
extern double fptodp(float value);

static RgPlayer *_GetInPlayerList(PlayerList *pList, RgRobot *robot);

/* Linker witness for the original literal at ov12:0x00a54df8
 * ("*** max = %f life = %f ***\n"), RgBattleMgrGetPlayerDamage's debug log
 * of a robot's max and current life before returning the difference. */
extern const char D_00A54DF8[];

float RgBattleMgrGetPlayerDamage(RgBattleMgr *pMgr, RgRobot *robot)
{
    RgPlayer *player;
    RgRobot *playerRobot;

    playerRobot = 0;
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 1085);
    }
    player = _GetInPlayerList(pMgr->playerList, robot);
    if (player != 0) {
        playerRobot = RgPlayerGetRobot(player);
    }
    if (playerRobot != 0) {
        XrgLog(D_00A54DF8, D_00A54CC0, 1090,
              fptodp(RgRobotGetLifeMax(playerRobot)),
              fptodp(RgRobotGetLife(playerRobot)));
        return RgRobotGetLifeMax(playerRobot) -
               RgRobotGetLife(playerRobot);
    }
    return 0.0f;
}

static void _InitBattle(RgBattleMgr *pMgr, int mode);

void RgBattleMgrInitBattle(RgBattleMgr *pMgr, int mode)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 1103);
    }
    _InitBattle(pMgr, mode);
}

static void _ControlBattle(RgBattleMgr *pMgr);

void RgBattleMgrControl(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 1110);
    }
    _ControlBattle(pMgr);
}

static void _PassTimeBattle(RgBattleMgr *pMgr, float deltaTime);

void RgBattleMgrPassTime(RgBattleMgr *pMgr, float deltaTime)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 1116);
    }
    _PassTimeBattle(pMgr, deltaTime);
}

static void _DispBattle(RgBattleMgr *pMgr);

void RgBattleMgrDisp(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 1122);
    }
    _DispBattle(pMgr);
}
