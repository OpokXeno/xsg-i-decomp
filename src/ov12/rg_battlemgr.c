/*
 * OV12 original TU 47: 0x00a28858..0x00a2ab70 (53 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_battle_common_data.h"
#include "ov12/rg_camera.h"
#include "ov12/rg_charmgr.h"
#include "ov12/rg_disp_gameinfo.h"
#include "ov12/rg_disp_life.h"
#include "ov12/rg_disp_wpn1p.h"
#include "ov12/rg_disp_wpn2p.h"
#include "ov12/rg_draw.h"
#include "ov12/rg_enemy.h"
#include "ov12/rg_filesys.h"
#include "ov12/rg_game_collision.h"
#include "ov12/rg_geom.h"
#include "ov12/rg_geom_ball.h"
#include "ov12/rg_geom_group.h"
#include "ov12/rg_geom_point.h"
#include "ov12/rg_handler.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_player.h"
#include "ov12/rg_read_text.h"
#include "ov12/rg_robot.h"
#include "ov12/rg_robot_control.h"
#include "ov12/xrg_paint2d.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/xrg_v_log.h"
#include "rg_battlemgr.h"

/* libgcc float-to-double helper (shared.h cannot declare it: main/matrix_print.c uses another result type). */
extern double fptodp(float value);

/* The battle's fixed time limit; RgBattleMgrGetPlayTime returns
 * the time played so far as the limit minus the remaining-time field. */
#define RG_BATTLE_TIME_LIMIT 180.0f

/* The split-screen divider: a translucent black strip down the middle of the screen. */
#define RG_BATTLE_DIVIDER_X 254
#define RG_BATTLE_DIVIDER_WIDTH 4
#define RG_BATTLE_DIVIDER_ALPHA 80
#define RG_BATTLE_SCREEN_HEIGHT 448

/* DispInfo.mode before _SetDispInfo has run. */
#define DISP_INFO_NO_MODE 0xFFFF

/* Assertion expressions and the source file name, in this TU's .rodata. */
extern const char battleMgrSourcePath[]; /* "../rg_battlemgr.euc.c" */
extern const char battleMgrCameraInfoFileName[]; /* "rg_camera.info": the battle camera's text file */
extern const char battleMgrAssertStudioNotNil[]; /* "pStudio != NIL" */
extern const char battleMgrCameraParagraphTag[]; /* "CAM": paragraph tag _CreateBattleCamera looks up */
extern const char battleMgrCameraParagraphSubTag[]; /* "2": its sub-tag */
extern const char battleMgrAssertListNotNil[]; /* "pList != NIL" */
extern const char battleMgrAssertFieldNotNil[]; /* "pField != NIL" */
extern const char battleMgrAssertDispNotNil[]; /* "pDisp != NIL" */
extern const char battleMgrAssertInfoNotNil[]; /* "pInfo != NIL" */
extern const char battleMgrAssertMgrNotNil[]; /* "pMgr != NIL" */
extern const char battleMgrAssertIndexBelowFieldCount[];
extern const char battleMgrAssertIndexBelowPlayerMax[];
extern const char battleMgrAssertIndexBelowListCount[];
extern const char battleMgrAssertPlayerNotNil[]; /* "pPlayer != NIL" */
extern const char battleMgrAssertListCountBelowPlayerMax[]; /* "pList->m_uNum < PLAYER_MAX" */
extern const char battleMgrAssertFieldCountBelowStudioMax[];
extern const char battleMgrAssertHostIndexInRange[];
extern const char battleMgrAssertSubIndexInRange[];
extern const char battleMgrPlayerDamageLogFormat[]; /* "*** max = %f life = %f ***\n" */







static void _ClearPlayerList(PlayerList *pList);
static void _InitBattleField(BattleField *pField);
static void _InitDispInfo(DispInfo *pInfo);
static void _DestructDispInfo(DispInfo *pInfo);


static void _ControlPlayerList(PlayerList *pList);
static void _DispPlayerList(PlayerList *pList);
static void _DrawDispInfo(DispInfo *pInfo);
static int _GetDeadList(PlayerList *pList);
static void _PassTimePlayerList(PlayerList *pList, float deltaTime);
static void _PassTimeBattleField(BattleField *pField, float deltaTime);
static void _InitBattleMgr(RgBattleMgr *pMgr);
static void _DisposeBattleMgr(RgBattleMgr *pMgr);
static void _BattleMgrPlayerControl(RgBattleMgr *pMgr, int enable);
static void _BattleMgrActivateTime(RgBattleMgr *pMgr, int active);
static RgPlayer *_GetInPlayerList(PlayerList *pList, u32 slotIndex);
static void _InitBattle(RgBattleMgr *pMgr, RgBattleInit *pInfo);
static void _ControlBattle(RgBattleMgr *pMgr);
static void _PassTimeBattle(RgBattleMgr *pMgr, float deltaTime);
static void _DispBattle(RgBattleMgr *pMgr);

const char battleMgrCameraInfoFileName[16] = "rg_camera.info";

const char battleMgrAssertStudioNotNil[16] = "pStudio != NIL";

const char battleMgrSourcePath[24] = "../rg_battlemgr.euc.c";

const char battleMgrCameraParagraphTag[8] = "CAM";

const char battleMgrCameraParagraphSubTag[8] = "2";

const char battleMgrAssertListNotNil[16] = "pList != NIL";

const char battleMgrAssertIndexBelowPlayerMax[16] = "i < PLAYER_MAX";

const char battleMgrAssertIndexBelowListCount[24] = "i < pList->m_uNum";

const char battleMgrAssertFieldNotNil[16] = "pField != NIL";

const char battleMgrAssertIndexBelowFieldCount[24] = "uIndex < pField->m_uNum";

const char battleMgrAssertFieldCountBelowStudioMax[32] = "pField->m_uNum < BTL_STUDIO_MAX";

const char battleMgrAssertDispNotNil[16] = "pDisp != NIL";

const char battleMgrAssertInfoNotNil[16] = "pInfo != NIL";

const char battleMgrAssertHostIndexInRange[24] = "0 <= nHost && nHost < 2";

const char battleMgrAssertSubIndexInRange[24] = "0 <= nSub && nSub < 2";

const char battleMgrAssertMgrNotNil[16] = "pMgr != NIL";

const char battleMgrPlayerDamageLogFormat[32] = "*** max = %f life = %f ***\n";

/*
 * Creates a battle camera on pStudio and loads its settings from the CAM paragraph of
 * rg_camera.info when that paragraph exists.
 */
static RgCamera *_CreateBattleCamera(RgDrawStudio *pStudio)
{
    RgReadText *pReader = CreateRgReadText(battleMgrCameraInfoFileName);
    RgCamera *camera;

    if (pStudio == 0) {
        assert_prog(battleMgrAssertStudioNotNil, battleMgrSourcePath, 0x41);
    }
    camera = CreateRgCamera(2, pStudio);
    RgReadTextRewind(pReader);
    if (RgReadTextFindParagraph(pReader, battleMgrCameraParagraphTag, battleMgrCameraParagraphSubTag) != 0) {
        RgCameraLoadText(camera, pReader);
    }
    DisposeRgReadText(pReader);
    return camera;
}

/* Returns a bit mask with bit i set for each player slot whose robot is dead. */
static int _GetDeadList(PlayerList *pList)
{
    extern RgStatus *RgPlayerGetRobot(RgPlayer *pPlayer);
    RgPlayer *pPlayer;
    int deadList;
    u32 i;

    deadList = 0;
    if (pList == 0) {
        assert_prog(battleMgrAssertListNotNil, battleMgrSourcePath, 0x55);
    }
    for (i = 0; i < RG_BATTLE_PLAYER_MAX; i++) {
        pPlayer = pList->players[i];
        if (pPlayer != 0 && RgRobotIsDead(RgPlayerGetRobot(pPlayer))) {
            deadList |= 1 << i;
        }
    }
    return deadList;
}

/* Returns 1 when slot 1 has more life, 0 when slot 0 has more, and -1 when they are equal. */
static int _GetBigLifePlayer(PlayerList *pList)
{
    extern RgStatus *RgPlayerGetRobot(RgPlayer *pPlayer);
    extern float RgRobotGetLife(RgStatus *pRobot);
    float life[2];
    u32 i;

    if (pList == 0) {
        assert_prog(battleMgrAssertListNotNil, battleMgrSourcePath, 103);
    }
    for (i = 0; i < RG_BATTLE_PLAYER_MAX; i++) {
        life[i] = RgRobotGetLife(RgPlayerGetRobot(pList->players[i]));
    }
    if (life[0] < life[1]) {
        return 1;
    }
    if (life[1] < life[0]) {
        return 0;
    }
    return -1;
}

static int _GetNumOfPlayerList(PlayerList *pList)
{
    if (pList == 0) {
        assert_prog(battleMgrAssertListNotNil, battleMgrSourcePath, 0x79);
    }
    return pList->count;
}

static RgPlayer *_GetInPlayerList(PlayerList *pList, u32 slotIndex)
{

    if (slotIndex >= 2U) {
        assert_prog(battleMgrAssertIndexBelowPlayerMax, battleMgrSourcePath, 128);
    }
    if (slotIndex >= (u32)pList->count) {
        assert_prog(battleMgrAssertIndexBelowListCount, battleMgrSourcePath, 129);
    }
    return pList->players[slotIndex];
}

/* Adds pPlayer to the list unless it is already in it. */
static void _AddPlayerList(PlayerList *pList, RgPlayer *pPlayer)
{
    RgPlayer **players;
    RgPlayer **pSlot;
    RgPlayer **pNextSlot;
    u32 i;

    if (pList == 0) {
        assert_prog(battleMgrAssertListNotNil, battleMgrSourcePath, 140);
    }
    if (pPlayer == 0) {
        assert_prog(battleMgrAssertPlayerNotNil, battleMgrSourcePath, 141);
    }
    if ((u32)pList->count >= 2U) {
        assert_prog(battleMgrAssertListCountBelowPlayerMax, battleMgrSourcePath, 142);
    }
    pSlot = pList->players;
    for (i = 0; i < 2; i++) {
        if (*pSlot++ == pPlayer) {
            return;
        }
    }
    i = pList->count;
    players = pList->players;
    pNextSlot = players + i;
    *pNextSlot = pPlayer;
    pList->count = i + 1;
}

/* Disposes the players in both slots and empties the list. */
static void _ClearPlayerList(PlayerList *pList)
{
    RgPlayer **pSlot;
    RgPlayer *player;
    u32 slotIndex;

    if (pList == 0) {
        assert_prog(battleMgrAssertListNotNil, battleMgrSourcePath, 0x9C);
    }
    slotIndex = 0;
    pSlot = pList->players;
    do {
        player = *pSlot;
        slotIndex += 1;
        if (player != 0) {
            DisposeRgPlayer(player);
            *pSlot = 0;
        }
        pSlot += 1;
    } while (slotIndex < 2U);
    pList->count = 0;
}

static void _ControlPlayerList(PlayerList *pList)
{
    RgPlayer *pPlayer;
    u32 slotIndex;

    if (pList == 0) {
        assert_prog(battleMgrAssertListNotNil, battleMgrSourcePath, 0xAA);
    }
    for (slotIndex = 0; slotIndex < RG_BATTLE_PLAYER_MAX; slotIndex++) {
        pPlayer = pList->players[slotIndex];
        if (pPlayer != 0) {
            RgPlayerControl(pPlayer);
        }
    }
}

static void _PassTimePlayerList(PlayerList *pList, float deltaTime)
{
    RgPlayer *pPlayer;
    u32 slotIndex;

    if (pList == 0) {
        assert_prog(battleMgrAssertListNotNil, battleMgrSourcePath, 180);
    }
    for (slotIndex = 0; slotIndex < RG_BATTLE_PLAYER_MAX; slotIndex++) {
        pPlayer = pList->players[slotIndex];
        if (pPlayer != 0) {
            RgPlayerPassTime(pPlayer, deltaTime);
        }
    }
}

static void _DispPlayerList(PlayerList *pList)
{
    RgPlayer *pPlayer;
    u32 slotIndex;

    if (pList == 0) {
        assert_prog(battleMgrAssertListNotNil, battleMgrSourcePath, 190);
    }
    for (slotIndex = 0; slotIndex < RG_BATTLE_PLAYER_MAX; slotIndex++) {
        pPlayer = pList->players[slotIndex];
        if (pPlayer != 0) {
            RgPlayerDisp(pPlayer);
        }
    }
}

static void _InitPlayerList(PlayerList *pList)
{
    if (pList == 0) {
        assert_prog(battleMgrAssertListNotNil, battleMgrSourcePath, 0xCC);
    }
    pList->player1P = 0;
    pList->player2P = 0;
    pList->count = 0;
}

static void _DestructPlayerList(PlayerList *pList)
{
    _ClearPlayerList(pList);
}

static PlayerList *_CreatePlayerList(void)
{
    PlayerList *pList = RgHeapAlloc(InstanceOfRgHeap(), sizeof(PlayerList),
                                    battleMgrSourcePath, 0xDC);

    _InitPlayerList(pList);
    return pList;
}

static void _DisposePlayerList(PlayerList *pList)
{
    if (pList == 0) {
        assert_prog(battleMgrAssertListNotNil, battleMgrSourcePath, 0xE3);
    }
    _DestructPlayerList(pList);
    RgHeapFree(InstanceOfRgHeap(), pList, battleMgrSourcePath, 0xE5);
}

static RgDrawStudio *_GetStudioBattleField(BattleField *pField, u32 index)
{
    if (pField == 0) {
        assert_prog(battleMgrAssertFieldNotNil, battleMgrSourcePath, 246);
    }
    if (index >= (u32)pField->count) {
        assert_prog(battleMgrAssertIndexBelowFieldCount, battleMgrSourcePath, 247);
    }
    return pField->studio[index];
}

static RgCamera *_GetCameraBattleField(BattleField *pField, u32 index)
{
    if (pField == 0) {
        assert_prog(battleMgrAssertFieldNotNil, battleMgrSourcePath, 254);
    }
    if (index >= (u32)pField->count) {
        assert_prog(battleMgrAssertIndexBelowFieldCount, battleMgrSourcePath, 255);
    }
    return pField->camera[index];
}

/*
 * Disposes the cameras and screen balls of the field and resets the draw studio to
 * full screen.
 */
static void _ClearBattleField(BattleField *pField)
{
    u32 i;

    if (pField == 0) {
        assert_prog(battleMgrAssertFieldNotNil, battleMgrSourcePath, 0x107);
    }
    for (i = 0; i < (u32)pField->count; i++) {
        if (pField->camera[i] != 0) {
            DisposeRgCamera(pField->camera[i]);
            pField->camera[i] = 0;
        }
        if (pField->screenBall[i] != 0) {
            RgGeomFree(&pField->screenBall[i]->point.geom);
            pField->screenBall[i] = 0;
        }
    }
    pField->count = 0;
    RgDrawCreateDrawStudioFullScreen(InstanceOfRgDraw());
}

/* Creates the field's geometry group and clears its camera and screen ball slots. */
static void _InitBattleField(BattleField *pField)
{
    RgGeomGroup *pGroup;
    u32 i;

    if (pField == 0) {
        assert_prog(battleMgrAssertFieldNotNil, battleMgrSourcePath, 0x11C);
    }
    pField->count = 0;
    pGroup = CreateRgGeomGroup();
    i = 0;
    pField->geomGroup = pGroup;
    do {
        pField->camera[i] = 0;
        pField->screenBall[i] = 0;
        i++;
    } while (i < BTL_STUDIO_MAX);
    RgDrawCreateDrawStudioFullScreen(InstanceOfRgDraw());
}

static void _DestructBattleField(BattleField *pField)
{
    if (pField == 0) {
        assert_prog(battleMgrAssertFieldNotNil, battleMgrSourcePath, 0x129);
    }
    _ClearBattleField(pField);
    DisposeRgGeomGroup(pField->geomGroup);
}

static BattleField *_CreateBattleField(void)
{
    BattleField *pField = RgHeapAlloc(InstanceOfRgHeap(), sizeof(BattleField),
                                      battleMgrSourcePath, 0x131);

    _InitBattleField(pField);
    return pField;
}

static void _DisposeBattleField(BattleField *pField)
{
    if (pField == 0) {
        assert_prog(battleMgrAssertFieldNotNil, battleMgrSourcePath, 0x139);
    }
    _DestructBattleField(pField);
    RgHeapFree(InstanceOfRgHeap(), pField, battleMgrSourcePath, 0x13B);
}

/* Sets the field to one full-screen camera on the first studio. Returns 1. */
static u32 _FullScreenBattleField(BattleField *pField)
{
    RgCamera *camera;

    if (pField == 0) {
        assert_prog(battleMgrAssertFieldNotNil, battleMgrSourcePath, 0x145);
    }
    if ((u32)pField->count >= 2U) {
        assert_prog(battleMgrAssertFieldCountBelowStudioMax, battleMgrSourcePath, 0x146);
    }
    _ClearBattleField(pField);
    pField->count = 1;
    RgDrawCreateDrawStudioFullScreen(InstanceOfRgDraw());
    pField->studio[0] = RgDrawGetStudio(InstanceOfRgDraw(), 0);
    camera = _CreateBattleCamera(pField->studio[0]);
    pField->camera[0] = camera;
    pField->screenBall[0] = RgGeomGroupCreateBall(pField->geomGroup, camera);
    RgGeomBallSetRadius(pField->screenBall[0], 1.5f);
    RgCameraSetFarMode(camera, 0);
    return 1;
}

/*
 * Sets the field to two studios, each with a camera selected for its player. Returns the
 * studio count.
 */
static u32 _DoubleScreenBattleField(BattleField *pField)
{
    u32 playerIndex;
    RgCamera *camera;
    RgGeomBall *screenBall;

    if (pField == 0) {
        assert_prog(battleMgrAssertFieldNotNil, battleMgrSourcePath, 0x164);
    }
    if ((u32)pField->count >= 2U) {
        assert_prog(battleMgrAssertFieldCountBelowStudioMax, battleMgrSourcePath, 0x165);
    }
    _ClearBattleField(pField);
    pField->count = 2;
    RgDrawCreateDrawStudioDouble(InstanceOfRgDraw());
    pField->studio[0] = RgDrawGetStudio(InstanceOfRgDraw(), 0);
    pField->studio[1] = RgDrawGetStudio(InstanceOfRgDraw(), 1);
    for (playerIndex = 0; playerIndex < (u32)pField->count; playerIndex++) {
        camera = _CreateBattleCamera(pField->studio[playerIndex]);
        pField->camera[playerIndex] = camera;
        screenBall = RgGeomGroupCreateBall(pField->geomGroup, camera);
        pField->screenBall[playerIndex] = screenBall;
        RgGeomBallSetRadius(screenBall, 1.0f);
        RgCameraSetFarMode(camera, 1);
        RgCameraSelectableFar(camera, playerIndex);
    }
    return pField->count;
}

static RgGeomGroup *_GetGeomGroupBattleField(BattleField *pField)
{
    if (pField == 0) {
        assert_prog(battleMgrAssertFieldNotNil, battleMgrSourcePath, 0x181);
    }
    return pField->geomGroup;
}

/*
 * Advances the field's cameras, then places each screen ball behind its camera and moves
 * it to a point just short of the camera target.
 */
static void _PassTimeBattleField(BattleField *pField, float deltaTime)
{
    u32 cameraIndex;
    RgCamera *pCamera;
    RgGeomBall *pBall;
    RgMatrix localMatrix;
    RgVector target;
    RgVector direction;
    RgVector offset;
    RgVector setPosition;
    RgVector movePosition;

    if (pField == 0) {
        assert_prog(battleMgrAssertFieldNotNil, battleMgrSourcePath, 0x189);
    }
    for (cameraIndex = 0; cameraIndex < (u32)pField->count; cameraIndex++) {
        if (pField->camera[cameraIndex] != 0) {
            RgCameraControl(pField->camera[cameraIndex], deltaTime);
        }
    }
    for (cameraIndex = 0; cameraIndex < (u32)pField->count; cameraIndex++) {
        pCamera = pField->camera[cameraIndex];
        pBall = pField->screenBall[cameraIndex];
        RgCameraLocal(pCamera, localMatrix);
        RgCameraGetTarget(pCamera, target);
        target[1] = 5.0f;
        localMatrix[13] = 5.0f;
        XrgSubVector(direction, target, &localMatrix[12]);
        direction[1] = 0.0f;
        XrgNormalizeVector(direction, direction);
        XrgScaleVector(offset, direction, 3.0f);
        XrgSubVectorXYZ(movePosition, target, offset);
        XrgScaleVector(offset, direction, 10.0f);
        XrgSubVectorXYZ(setPosition, &localMatrix[12], offset);
        RgGeomPointSetPos(&pBall->point, setPosition);
        RgGeomPointMovePos(&pBall->point, movePosition);
    }
}

/* Creates a game-info display for each player and clears the player slots and mode. */
static void _InitDispInfo(DispInfo *pInfo)
{
    u32 playerIndex;

    if (pInfo == 0) {
        assert_prog(battleMgrAssertDispNotNil, battleMgrSourcePath, 0x1CA);
    }
    for (playerIndex = 0; playerIndex < 2; playerIndex++) {
        pInfo->gameInfo[playerIndex] =
            CreateRgDispGameInfo(RgBattleCommonDataGetDispTex(
                InstanceOfRgBattleCommonData()));
        pInfo->players[playerIndex] = 0;
    }
    pInfo->mode = DISP_INFO_NO_MODE;
}

static void _DestructDispInfo(DispInfo *pInfo)
{
    u32 slot;

    if (pInfo == 0) {
        assert_prog(battleMgrAssertDispNotNil, battleMgrSourcePath, 471);
    }
    slot = 0;
    do {
        DisposeRgDispGameInfo(pInfo->gameInfo[slot]);
        slot += 1;
    } while (slot < 2U);
}

static DispInfo *_CreateDispInfo(void)
{
    DispInfo *pInfo = RgHeapAlloc(InstanceOfRgHeap(), sizeof(DispInfo), battleMgrSourcePath, 479);

    _InitDispInfo(pInfo);
    return pInfo;
}

static void _DisposeDispInfo(DispInfo *pInfo)
{
    if (pInfo == 0) {
        assert_prog(battleMgrAssertInfoNotNil, battleMgrSourcePath, 487);
    }
    _DestructDispInfo(pInfo);
    RgHeapFree(InstanceOfRgHeap(), pInfo, battleMgrSourcePath, 489);
}

static void _SetDispInfo(DispInfo *pDisp, int mode, RgPlayer *player1P,
                         RgPlayer *player2P)
{
    if (pDisp == 0) {
        assert_prog(battleMgrAssertDispNotNil, battleMgrSourcePath, 495);
    }
    pDisp->mode = mode;
    pDisp->player1P = player1P;
    pDisp->player2P = player2P;
}

/* Gives a game-info display its host and sub robots, then advances and draws it. */
static void _DrawDispInfoOnePlayer(RgDispGameInfo *pInfo, RgPlayer **players,
                                   u32 nHost, u32 nSub)
{
    RgPlayer *hostPlayer;
    RgPlayer *subPlayer;

    if (nHost >= 2U) {
        assert_prog(battleMgrAssertHostIndexInRange, battleMgrSourcePath, 0x1F8);
    }
    if (nSub >= 2U) {
        assert_prog(battleMgrAssertSubIndexInRange, battleMgrSourcePath, 0x1F9);
    }
    hostPlayer = players[nHost];
    if (hostPlayer != 0) {
        RgDispGameInfoSetHostRobot(pInfo, RgPlayerGetRobot(hostPlayer));
        subPlayer = players[nSub];
        if (subPlayer != 0) {
            RgDispGameInfoSetSubRobot(pInfo, RgPlayerGetRobot(subPlayer));
        }
    }
    RgDispGameInfoPassTime(pInfo, RG_FRAME_TIME);
    RgDispGameInfoDisplay(pInfo);
}

/*
 * Draws the display info for the current mode: both panels in the two-human modes, or
 * the panel of the one human player otherwise.
 */
static void _DrawDispInfo(DispInfo *pInfo)
{
    u32 mode;
    u32 i;

    if (pInfo == 0) {
        assert_prog(battleMgrAssertDispNotNil, battleMgrSourcePath, 519);
    }
    mode = pInfo->mode;
    switch (mode) {
    case RG_BATTLE_MODE_NO_HUMANS:
    case RG_BATTLE_MODE_BOTH_HUMANS:
        RgDispGameInfoSetStudio(pInfo->gameInfo[0], 0);
        RgDispGameInfoSetStudio(pInfo->gameInfo[1], 1);
        for (i = 0; i < 2; i++) {
            _DrawDispInfoOnePlayer(pInfo->gameInfo[i], pInfo->players, i, 1 - i);
        }
        break;
    case RG_BATTLE_MODE_PLAYER1_ONLY:
        RgDispGameInfoSetStudio(pInfo->gameInfo[0], -2);
        _DrawDispInfoOnePlayer(pInfo->gameInfo[0], pInfo->players, 0, 1);
        return;
    case RG_BATTLE_MODE_PLAYER2_ONLY:
        RgDispGameInfoSetStudio(pInfo->gameInfo[1], -2);
        _DrawDispInfoOnePlayer(pInfo->gameInfo[1], pInfo->players, 1, 0);
        break;
    }
}

/*
 * Creates the player list, collision, geometry, field, display info and display widgets
 * of a new battle manager.
 */
static void _InitBattleMgr(RgBattleMgr *pMgr)
{
    RgBattleCommonDataEnv *pData;
    RgBxx *dispTex;

    pData = InstanceOfRgBattleCommonData();
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 0x250);
    }
    pMgr->playerList = _CreatePlayerList();
    pMgr->gameCollision = CreateRgGameCollision();
    pMgr->geomGroup = CreateRgGeomGroup();
    pMgr->battleField = _CreateBattleField();
    pMgr->mode = 0;
    pMgr->dispInfo = _CreateDispInfo();
    dispTex = RgBattleCommonDataGetDispTex(pData);
    pMgr->dispLife = CreateRgDispLife(dispTex, RgBattleCommonDataTimeFont(pData));
    pMgr->dispWpn1P = CreateRgDispWpn1P(RgBattleCommonDataGetDispTex(pData));
    pMgr->dispWpn2P = CreateRgDispWpn2P();
    pMgr->fileSysData = 0;
    pMgr->timerActive = 1;
    pMgr->playerControl = 0;
    pMgr->result = 0;
}

/* Disposes the parts of a battle manager and clears the character manager. */
static void _DisposeBattleMgr(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 611);
    }
    _DisposePlayerList(pMgr->playerList);
    DisposeRgGameCollision(pMgr->gameCollision);
    DisposeRgGeomGroup(pMgr->geomGroup);
    _DisposeBattleField(pMgr->battleField);
    _DisposeDispInfo(pMgr->dispInfo);
    DisposeRgDispLife(pMgr->dispLife);
    DisposeRgDispWpn1P(pMgr->dispWpn1P);
    DisposeRgDispWpn2P(pMgr->dispWpn2P);
    if (pMgr->fileSysData != 0) {
        DisposeRgFileSysData_sub(pMgr->fileSysData, battleMgrSourcePath, 622);
    }
    pMgr->playerList = 0;
    pMgr->gameCollision = 0;
    pMgr->geomGroup = 0;
    pMgr->dispInfo = 0;
    pMgr->battleField = 0;
    pMgr->fileSysData = 0;
    InstanceOfRgCharMgrClear();
}

static void _BattleMgrPlayerControl(RgBattleMgr *pMgr, int enable)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 640);
    }
    pMgr->playerControl = enable;
}

static void _BattleMgrActivateTime(RgBattleMgr *pMgr, int active)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 647);
    }
    pMgr->timerActive = active;
}

/*
 * Sets up a battle from pInfo: the screens and cameras, the player and enemy controls,
 * the robot targets and the collision pairs.
 */
static void _InitBattle(RgBattleMgr *pMgr, RgBattleInit *pInfo)
{
    RgPlayer *players[4];
    RgStatus *robots[4];
    RgGeomPoint *geoms[2];
    RgGameColiArg collArg;
    RgCamera *camera;
    u32 playerCount;
    int screenCount;
    u32 playerBit;
    u32 nextIndex;
    u32 i;
    int mode;

    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 663);
    }
    if (pInfo == 0) {
        assert_prog(battleMgrAssertInfoNotNil, battleMgrSourcePath, 664);
    }
    pMgr->remainingTime = RG_BATTLE_TIME_LIMIT;
    mode = pInfo->mode;
    pMgr->mode = mode;
    screenCount = (mode == RG_BATTLE_MODE_NO_HUMANS || mode == RG_BATTLE_MODE_BOTH_HUMANS) ? _DoubleScreenBattleField(pMgr->battleField) : _FullScreenBattleField(pMgr->battleField);
    pMgr->fileSysData = RgBattleInitCreateBg(pInfo, pMgr->geomGroup);
    playerCount = RgBattleInitCreatePlayers(pInfo, players, 4);
    for (i = 0; i < playerCount; i++) {
        _AddPlayerList(pMgr->playerList, players[i]);
        robots[i] = RgPlayerGetRobot(players[i]);
    }
    if (mode == RG_BATTLE_MODE_NO_HUMANS || mode == RG_BATTLE_MODE_BOTH_HUMANS) {
        for (i = 0; i < playerCount; i++) {
            RgPlayerSetLightCost(players[i]);
        }
    }
    _SetDispInfo(pMgr->dispInfo, mode, players[0], players[1]);
    RgDispLifeSetRobot(pMgr->dispLife, robots[0], robots[1]);
    if (mode == RG_BATTLE_MODE_NO_HUMANS || mode == RG_BATTLE_MODE_BOTH_HUMANS) {
        RgDispWpn2PSetRobot(pMgr->dispWpn2P, robots[0], robots[1]);
        RgDispLifeSetVsMode(pMgr->dispLife, 1);
    } else {
        RgDispWpn1PSetRobot(pMgr->dispWpn1P, robots[0]);
        RgDispLifeSetVsMode(pMgr->dispLife, 0);
    }
    /* One camera per screen, aimed at its player's robot and the next player's robot. */
    for (i = 0; i < screenCount; i++) {
        nextIndex = i + 1;
        camera = _GetCameraBattleField(pMgr->battleField, i);
        geoms[0] = RgRobotGetGeom(RgPlayerGetRobot(players[i]));
        geoms[1] = RgRobotGetGeom(
            RgPlayerGetRobot(players[nextIndex % playerCount]));
        RgCameraSetting(camera, geoms);
        playerBit = 1U << i;
        /* Human players get an input control and trace the camera of their screen. */
        if (i < playerCount && (pInfo->mode & playerBit)) {
            RgRobotControl *control;
            RgPlayer *player;

            player = players[i];
            control = CreateRgRobotControlInput(RgPlayerGetRobot(player),
                                                camera, i);
            RgPlayerSetTraceCamera(player, camera);
            RgPlayerSetControl(player, control);
        }
    }
    for (i = 0; i < playerCount; i++) {
        playerBit = 1U << i;
        /* Players that are not human get an enemy control. */
        if (!(pInfo->mode & playerBit)) {
            RgStatus *robotA;
            RgStatus *robotB;
            RgRobotControl *control;
            RgPlayer *player;

            player = players[i];
            robotA = RgPlayerGetRobot(player);
            robotB = RgPlayerGetRobot(players[(i + 1) % playerCount]);
            control = &CreateRgEnemyControl(robotA, robotB, pInfo->enemyType)->base;
            RgPlayerSetControl(player, control);
        }
    }
    for (i = 0; i < playerCount; i++) {
        u32 j;

        for (j = i + 1; j < playerCount; j++) {
            RgStatus *robotA;
            RgStatus *robotB;
            RgGeomPoint *geomA;
            RgGeomPoint *geomB;

            /* Each pair of players targets the other robot and its geometry. */
            robotA = RgPlayerGetRobot(players[i]);
            robotB = RgPlayerGetRobot(players[j]);
            geomA = RgRobotGetGeom(robotA);
            geomB = RgRobotGetGeom(robotB);
            RgRobotSetTarget(robotA, geomB);
            RgRobotSetTarget(robotB, geomA);
        }
    }
    for (i = 0; i < playerCount; i++) {
        u32 j;

        InitRgGeomGroupColiArg(&collArg);
        collArg.m_pGroup1 = RgPlayerGetShotGeoms(players[i]);
        collArg.m_pGroup2 = pMgr->geomGroup;
        collArg.hitCallback1 = RgHandlerShotVsBG;
        RgGameCollisionAdd(pMgr->gameCollision, &collArg);

        InitRgGeomGroupColiArg(&collArg);
        collArg.m_pGroup1 = RgPlayerGetAtkGeoms(players[i]);
        collArg.m_pGroup2 = pMgr->geomGroup;
        collArg.hitCallback1 = RgHandlerWeaponVsBG;
        RgGameCollisionAdd(pMgr->gameCollision, &collArg);

        for (j = i + 1; j < playerCount; j++) {
            InitRgGeomGroupColiArg(&collArg);
            collArg.m_pGroup1 = RgPlayerGetBodyGeoms(players[i]);
            collArg.m_pGroup2 = RgPlayerGetBodyGeoms(players[j]);
            collArg.hitCallback1 = RgHandlerRobotVsRobot;
            collArg.hitCallback2 = RgHandlerRobotVsRobot;
            RgGameCollisionAdd(pMgr->gameCollision, &collArg);

            InitRgGeomGroupColiArg(&collArg);
            collArg.m_pGroup1 = RgPlayerGetShotGeoms(players[j]);
            collArg.m_pGroup2 = RgPlayerGetBodyGeoms(players[i]);
            collArg.hitCallback1 = RgHandlerShotVsRobot;
            RgGameCollisionAdd(pMgr->gameCollision, &collArg);

            InitRgGeomGroupColiArg(&collArg);
            collArg.m_pGroup1 = RgPlayerGetShotGeoms(players[i]);
            collArg.m_pGroup2 = RgPlayerGetBodyGeoms(players[j]);
            collArg.hitCallback1 = RgHandlerShotVsRobot;
            RgGameCollisionAdd(pMgr->gameCollision, &collArg);

            InitRgGeomGroupColiArg(&collArg);
            collArg.m_pGroup1 = RgPlayerGetAtkGeoms(players[j]);
            collArg.m_pGroup2 = RgPlayerGetBodyGeoms(players[i]);
            collArg.hitCallback1 = RgHandlerWeaponVsRobot;
            RgGameCollisionAdd(pMgr->gameCollision, &collArg);

            InitRgGeomGroupColiArg(&collArg);
            collArg.m_pGroup1 = RgPlayerGetAtkGeoms(players[i]);
            collArg.m_pGroup2 = RgPlayerGetBodyGeoms(players[j]);
            collArg.hitCallback1 = RgHandlerWeaponVsRobot;
            RgGameCollisionAdd(pMgr->gameCollision, &collArg);
        }

        InitRgGeomGroupColiArg(&collArg);
        collArg.m_pGroup1 = RgPlayerGetAdvGeoms(players[i]);
        collArg.m_pGroup2 = pMgr->geomGroup;
        RgGameCollisionAdd(pMgr->gameCollision, &collArg);

        InitRgGeomGroupColiArg(&collArg);
        collArg.m_pGroup1 = RgPlayerGetEyeGeoms(players[i]);
        collArg.m_pGroup2 = pMgr->geomGroup;
        RgGameCollisionAdd(pMgr->gameCollision, &collArg);

        InitRgGeomGroupColiArg(&collArg);
        collArg.m_pGroup1 = RgPlayerGetBodyGeoms(players[i]);
        collArg.m_pGroup2 = pMgr->geomGroup;
        collArg.hitCallback1 = RgHandlerRobotVsBG;
        RgGameCollisionAdd(pMgr->gameCollision, &collArg);
    }
    {
        RgGameColiArg cameraArg;

        InitRgGeomGroupColiArg(&cameraArg);
        cameraArg.m_pGroup1 = _GetGeomGroupBattleField(pMgr->battleField);
        cameraArg.m_pGroup2 = pMgr->geomGroup;
        cameraArg.hitCallback1 = RgHandlerCameraVsBG;
        RgGameCollisionAdd(pMgr->gameCollision, &cameraArg);
    }
}

/*
 * Runs player control when it is enabled, then the character manager's control and
 * garbage collection.
 */
static void _ControlBattle(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 0x359);
    }
    if (pMgr->playerControl != 0) {
        _ControlPlayerList(pMgr->playerList);
    }
    RgCharMgrControl(InstanceOfRgCharMgr());
    RgCharMgrGC(InstanceOfRgCharMgr());
}

/*
 * Draws the battle: the split-screen divider in the two-human modes, then the players,
 * characters, display info and life and weapon displays.
 */
static void _DispBattle(RgBattleMgr *pMgr)
{
    XrgPaint2DRGBA color;
    XrgPaint2D *pPaint;
    int mode;

    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 0x365);
    }
    mode = pMgr->mode;
    if ((mode == RG_BATTLE_MODE_NO_HUMANS) || (mode == RG_BATTLE_MODE_BOTH_HUMANS)) {
        memset(&color, 0, sizeof(color));
        color.a = RG_BATTLE_DIVIDER_ALPHA;
        pPaint = InstanceOfXrgPaint2D();
        XrgPaint2DAlpha(pPaint, 0);
        XrgPaint2DColor(pPaint, &color);
        XrgPaint2DDrawXYWH(pPaint, 0, RG_BATTLE_DIVIDER_X, 0, RG_BATTLE_DIVIDER_WIDTH, RG_BATTLE_SCREEN_HEIGHT);
    }
    _DispPlayerList(pMgr->playerList);
    RgCharMgrDisp(InstanceOfRgCharMgr());
    _DrawDispInfo(pMgr->dispInfo);
    RgDispLifeDisp(pMgr->dispLife);
    RgDispWpn1PDisp(pMgr->dispWpn1P);
    RgDispWpn2PDisp(pMgr->dispWpn2P);
}

/*
 * Advances the battle by deltaTime. Sets the result when a player is knocked out or the
 * time runs out.
 */
static void _PassTimeBattle(RgBattleMgr *pMgr, float deltaTime)
{
    int timeExpired;

    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 0x383);
    }
    _PassTimePlayerList(pMgr->playerList, deltaTime);
    timeExpired = 0;
    RgCharMgrPassTime(InstanceOfRgCharMgr(), deltaTime);
    RgGameCollisionJob(pMgr->gameCollision);
    _PassTimeBattleField(pMgr->battleField, deltaTime);
    if (pMgr->playerControl != 0 && pMgr->timerActive != 0) {
        pMgr->remainingTime -= deltaTime;
        if (pMgr->remainingTime <= 0.0f) {
            pMgr->remainingTime = 0.0f;
            timeExpired = 1;
        }
    }
    if (pMgr->result == 0) {
        int deadPlayers = _GetDeadList(pMgr->playerList);

        if ((deadPlayers & 1) != 0) {
            pMgr->result = RG_BATTLE_RESULT_KNOCKOUT | RG_BATTLE_RESULT_PLAYER2_WINS;
        } else if ((deadPlayers & 2) != 0) {
            pMgr->result = RG_BATTLE_RESULT_KNOCKOUT | RG_BATTLE_RESULT_PLAYER1_WINS;
        } else if (timeExpired != 0) {
            float playerDamage[2];

            pMgr->result = RG_BATTLE_RESULT_TIME_OVER;
            playerDamage[0] = RgBattleMgrGetPlayerDamage(pMgr, 0);
            playerDamage[1] = RgBattleMgrGetPlayerDamage(pMgr, 1);
            if (playerDamage[0] < playerDamage[1]) {
                pMgr->result |= RG_BATTLE_RESULT_PLAYER1_WINS;
            } else if (playerDamage[1] < playerDamage[0]) {
                pMgr->result |= RG_BATTLE_RESULT_PLAYER2_WINS;
            } else {
                pMgr->result |= RG_BATTLE_RESULT_DRAW;
            }
        }
    }
    RgDispLifeSetTimer(pMgr->dispLife, pMgr->remainingTime);
    RgDispLifePassTime(pMgr->dispLife, deltaTime);
    RgDispWpn1PPassTime(pMgr->dispWpn1P, deltaTime);
    RgDispWpn2PPassTime(pMgr->dispWpn2P, deltaTime);
}

RgBattleMgr *CreateRgBattleMgr(void)
{
    RgBattleMgr *pMgr = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgBattleMgr), battleMgrSourcePath, 1015);

    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 1016);
    }
    _InitBattleMgr(pMgr);
    return pMgr;
}

void DisposeRgBattleMgr(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 1023);
    }
    _DisposeBattleMgr(pMgr);
    RgHeapFree(InstanceOfRgHeap(), pMgr, battleMgrSourcePath, 1025);
}

void RgBattleMgrSetPlayerControl(RgBattleMgr *pMgr, int enable)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 0x40A);
    }
    _BattleMgrPlayerControl(pMgr, enable);
}

void RgBattleMgrActivateTimer(RgBattleMgr *pMgr, int active)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 0x411);
    }
    _BattleMgrActivateTime(pMgr, active);
}

float RgBattleMgrGetPlayTime(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 1052);
    }
    return RG_BATTLE_TIME_LIMIT - pMgr->remainingTime;
}

int RgBattleMgrGetResult(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 1059);
    }
    return pMgr->result;
}

int RgBattleMgrGetMode(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 1067);
    }
    return pMgr->mode;
}

RgDispLife *RgBattleMgrGetDispLife(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 1074);
    }
    return pMgr->dispLife;
}

/*
 * Returns the damage taken by a player (maximum life minus life), logging it, or 0 when
 * the player has no robot.
 */
float RgBattleMgrGetPlayerDamage(RgBattleMgr *pMgr, int playerIndex)
{
    RgPlayer *player;
    RgStatus *playerRobot;

    playerRobot = 0;
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 1085);
    }
    player = _GetInPlayerList(pMgr->playerList, playerIndex);
    if (player != 0) {
        playerRobot = RgPlayerGetRobot(player);
    }
    if (playerRobot != 0) {
        XrgLog(battleMgrPlayerDamageLogFormat, battleMgrSourcePath, 1090,
              fptodp(RgRobotGetLifeMax(playerRobot)),
              fptodp(RgRobotGetLife(playerRobot)));
        return RgRobotGetLifeMax(playerRobot) -
               RgRobotGetLife(playerRobot);
    }
    return 0.0f;
}

void RgBattleMgrInitBattle(RgBattleMgr *pMgr, RgBattleInit *pInfo)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 1103);
    }
    _InitBattle(pMgr, pInfo);
}

void RgBattleMgrControl(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 1110);
    }
    _ControlBattle(pMgr);
}

void RgBattleMgrPassTime(RgBattleMgr *pMgr, float deltaTime)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 1116);
    }
    _PassTimeBattle(pMgr, deltaTime);
}

void RgBattleMgrDisp(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(battleMgrAssertMgrNotNil, battleMgrSourcePath, 1122);
    }
    _DispBattle(pMgr);
}



const char battleMgrAssertPlayerNotNil[16] = "pPlayer != NIL";

const char battleMgrAssertListCountBelowPlayerMax[32] = "pList->m_uNum < PLAYER_MAX";
