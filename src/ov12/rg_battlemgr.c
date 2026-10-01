/*
 * OV12 original TU 47: 0x00a28858..0x00a2ab70 (53 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_battlemgr.h"
#include "ov12/rg_draw.h"

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

extern const char D_00A54D60[];
extern int RgRobotIsDead(RgRobot *pRobot);

static int _GetDeadList(PlayerList *pList)
{
    extern RgRobot *RgPlayerGetRobot(RgPlayer *pPlayer);
    RgPlayer **pSlot;
    RgPlayer *pPlayer;
    int deadList;
    u32 i;

    deadList = 0;
    if (pList == 0) {
        assert_prog(D_00A54CE8, D_00A54CC0, 0x55);
    }
    pSlot = &pList->player1P;
    for (i = 0; i < 2; i++) {
        pPlayer = *pSlot++;
        if (pPlayer != 0 && RgRobotIsDead(RgPlayerGetRobot(pPlayer))) {
            deadList |= 1 << i;
        }
    }
    return deadList;
}

static int _GetBigLifePlayer(PlayerList *pList)
{
    extern RgRobot *RgPlayerGetRobot(RgPlayer *pPlayer);
    extern float RgRobotGetLife(RgRobot *pRobot);
    float life[2];
    u32 i;

    if (pList == 0) {
        assert_prog(D_00A54CE8, D_00A54CC0, 103);
    }
    for (i = 0; i < 2; i++) {
        RgPlayer **players = &pList->player1P;
        RgPlayer **playerSlot;

        playerSlot = &players[i];
        life[i] = RgRobotGetLife(RgPlayerGetRobot(*playerSlot));
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
        assert_prog(D_00A54CE8, D_00A54CC0, 0x79);
    }
    return pList->count;
}

extern const char D_00A54CF8[];
extern const char D_00A54D08[];

static RgPlayer *_GetInPlayerList(PlayerList *pList, RgRobot *robot)
{
    u32 slotIndex = (u32)robot;
    RgPlayer **players;

    if (slotIndex >= 2U) {
        assert_prog(D_00A54CF8, D_00A54CC0, 128);
    }
    if (slotIndex >= (u32)pList->count) {
        assert_prog(D_00A54D08, D_00A54CC0, 129);
    }
    players = &pList->player1P;
    return players[slotIndex];
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _AddPlayerList);

extern void DisposeRgPlayer(RgPlayer *pPlayer);

static void _ClearPlayerList(PlayerList *pList)
{
    RgPlayer **pSlot;
    RgPlayer *player;
    u32 slotIndex;

    if (pList == 0) {
        assert_prog(D_00A54CE8, D_00A54CC0, 0x9C);
    }
    slotIndex = 0;
    pSlot = &pList->player1P;
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

extern void RgPlayerControl(RgPlayer *pPlayer);

static void _ControlPlayerList(PlayerList *pList)
{
    RgPlayer **pSlot;
    RgPlayer *pPlayer;
    u32 slotIndex;

    if (pList == 0) {
        assert_prog(D_00A54CE8, D_00A54CC0, 0xAA);
    }
    pSlot = &pList->player1P;
    for (slotIndex = 0; slotIndex < 2; slotIndex++) {
        pPlayer = *pSlot++;
        if (pPlayer != 0) {
            RgPlayerControl(pPlayer);
        }
    }
}

extern void RgPlayerPassTime(RgPlayer *pPlayer, float deltaTime);

static void _PassTimePlayerList(PlayerList *pList, float deltaTime)
{
    RgPlayer **pSlot;
    RgPlayer *pPlayer;
    u32 slotIndex;

    if (pList == 0) {
        assert_prog(D_00A54CE8, D_00A54CC0, 180);
    }
    pSlot = &pList->player1P;
    for (slotIndex = 0; slotIndex < 2; slotIndex++) {
        pPlayer = *pSlot++;
        if (pPlayer != 0) {
            RgPlayerPassTime(pPlayer, deltaTime);
        }
    }
}

extern void RgPlayerDisp(RgPlayer *pPlayer);

static void _DispPlayerList(PlayerList *pList)
{
    RgPlayer **pSlot;
    RgPlayer *pPlayer;
    u32 slotIndex;

    if (pList == 0) {
        assert_prog(D_00A54CE8, D_00A54CC0, 190);
    }
    pSlot = &pList->player1P;
    for (slotIndex = 0; slotIndex < 2; slotIndex++) {
        pPlayer = *pSlot++;
        if (pPlayer != 0) {
            RgPlayerDisp(pPlayer);
        }
    }
}

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

static PlayerList *_CreatePlayerList(void)
{
    PlayerList *pList = RgHeapAlloc(InstanceOfRgHeap(), sizeof(PlayerList),
                                    D_00A54CC0, 0xDC);

    _InitPlayerList(pList);
    return pList;
}

static void _DisposePlayerList(PlayerList *pList)
{
    if (pList == 0) {
        assert_prog(D_00A54CE8, D_00A54CC0, 0xE3);
    }
    _DestructPlayerList(pList);
    RgHeapFree(InstanceOfRgHeap(), pList, D_00A54CC0, 0xE5);
}

static RgDrawStudio *_GetStudioBattleField(BattleField *pField, u32 index)
{
    if (pField == 0) {
        assert_prog(D_00A54D50, D_00A54CC0, 246);
    }
    if (index >= (u32)pField->count) {
        assert_prog(D_00A54D60, D_00A54CC0, 247);
    }
    return pField->studio[index];
}

static RgCamera *_GetCameraBattleField(BattleField *pField, u32 index)
{
    if (pField == 0) {
        assert_prog(D_00A54D50, D_00A54CC0, 254);
    }
    if (index >= (u32)pField->count) {
        assert_prog(D_00A54D60, D_00A54CC0, 255);
    }
    return pField->camera[index];
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _ClearBattleField);

extern RgGeomGroup *CreateRgGeomGroup(void);
extern RgDraw *InstanceOfRgDraw(void);
extern void RgDrawCreateDrawStudioFullScreen(RgDraw *pDraw);

static void _InitBattleField(BattleField *pField)
{
    RgGeomGroup *pGroup;
    u8 *pStudioBase;
    u8 *pCameraSlot;
    u8 *pStudioSlot;
    u32 offset;
    u32 pass;

    if (pField == 0) {
        assert_prog(D_00A54D50, D_00A54CC0, 0x11C);
    }
    pField->count = 0;
    pGroup = CreateRgGeomGroup();
    pass = 0;
    pField->geomGroup = pGroup;
    pStudioBase = (u8 *) pField->studio;
    offset = 0x10;
    do {
        pass += 1;
        pCameraSlot = &pStudioBase[offset];
        pStudioSlot = (u8 *) pField + offset;
        offset += 4;
        *(int *) pCameraSlot = 0;
        *(int *) pStudioSlot = 0;
    } while (pass < 2U);
    RgDrawCreateDrawStudioFullScreen(InstanceOfRgDraw());
}

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

static void _DisposeBattleField(BattleField *pField)
{
    if (pField == 0) {
        assert_prog(D_00A54D50, D_00A54CC0, 0x139);
    }
    _DestructBattleField(pField);
    RgHeapFree(InstanceOfRgHeap(), pField, D_00A54CC0, 0x13B);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _FullScreenBattleField);

#include "ov12/rg_camera.h"
extern RgDispGameInfo *CreateRgDispGameInfo(int dispTex);
extern void RgDrawCreateDrawStudioDouble(RgDraw *pDraw);
extern RgDrawStudio *RgDrawGetStudio(RgDraw *pDraw, int screenIndex);
extern RgGeom *RgGeomGroupCreateBall(RgGeomGroup *pGroup, void *parent);
extern void RgGeomBallSetRadius(void *ball, float radius);
extern void RgCameraSetFarMode(RgCamera *pCamera, int enable);
extern void RgCameraSelectableFar(RgCamera *pCamera, int level);
extern void RgCameraControl(RgCamera *pCamera, float deltaTime);
extern void RgCameraLocal(RgCamera *pCamera, RgMatrix matrix);
extern void RgCameraGetTarget(RgCamera *pCamera, RgVector position);
extern void XrgSubVector(RgVector destination, RgVector first, RgVector second);
extern void XrgNormalizeVector(RgVector destination, RgVector source);
extern void XrgScaleVector(RgVector destination, RgVector source, float scale);
extern void XrgSubVectorXYZ(RgVector destination, RgVector first,
                            RgVector second);
extern void RgGeomPointSetPos(RgGeomPoint *pPoint, RgVector position);
extern void RgGeomPointMovePos(RgGeomPoint *pPoint, RgVector position);
extern const char D_00A54D78[];

static u32 _DoubleScreenBattleField(BattleField *pField)
{
    u32 playerIndex;
    RgCamera *camera;
    RgGeom *screenBall;

    if (pField == 0) {
        assert_prog(D_00A54D50, D_00A54CC0, 0x164);
    }
    if ((u32)pField->count >= 2U) {
        assert_prog(D_00A54D78, D_00A54CC0, 0x165);
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
        assert_prog(D_00A54D50, D_00A54CC0, 0x181);
    }
    return pField->geomGroup;
}

static void _PassTimeBattleField(BattleField *pField, float deltaTime)
{
    u32 cameraIndex;
    RgCamera *pCamera;
    RgGeom *pBall;
    RgMatrix localMatrix;
    RgVector target;
    RgVector direction;
    RgVector offset;
    RgVector setPosition;
    RgVector movePosition;

    if (pField == 0) {
        assert_prog(D_00A54D50, D_00A54CC0, 0x189);
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
        RgGeomPointSetPos((RgGeomPoint *)pBall, setPosition);
        RgGeomPointMovePos((RgGeomPoint *)pBall, movePosition);
    }
}

static void _InitDispInfo(DispInfo *pInfo)
{
    extern RgBattleCommonDataEnv *InstanceOfRgBattleCommonData(void);
    extern int RgBattleCommonDataGetDispTex(RgBattleCommonDataEnv *pEnv);
    u32 playerIndex;

    if (pInfo == 0) {
        assert_prog(D_00A54D98, D_00A54CC0, 0x1CA);
    }
    for (playerIndex = 0; playerIndex < 2; playerIndex++) {
        pInfo->gameInfo[playerIndex] =
            CreateRgDispGameInfo(RgBattleCommonDataGetDispTex(
                InstanceOfRgBattleCommonData()));
        pInfo->players[playerIndex] = 0;
    }
    pInfo->mode = 0xFFFF;
}

extern void DisposeRgDispGameInfo(RgDispGameInfo *pInfo);

static void _DestructDispInfo(DispInfo *pInfo)
{
    u32 slot;

    if (pInfo == 0) {
        assert_prog(D_00A54D98, D_00A54CC0, 471);
    }
    slot = 0;
    do {
        DisposeRgDispGameInfo(pInfo->gameInfo[slot]);
        slot += 1;
    } while (slot < 2U);
}

static void _InitDispInfo(DispInfo *pInfo);

static DispInfo *_CreateDispInfo(void)
{
    DispInfo *pInfo = RgHeapAlloc(InstanceOfRgHeap(), sizeof(DispInfo), D_00A54CC0, 479);

    _InitDispInfo(pInfo);
    return pInfo;
}

static void _DestructDispInfo(DispInfo *pInfo);

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

extern RgRobot *RgPlayerGetRobot(RgPlayer *pPlayer);
extern void RgDispGameInfoSetHostRobot(RgDispGameInfo *pInfo, RgRobot *robot);
extern void RgDispGameInfoSetSubRobot(RgDispGameInfo *pInfo, RgRobot *robot);
extern void RgDispGameInfoPassTime(RgDispGameInfo *pInfo, float deltaTime);
extern void RgDispGameInfoDisplay(RgDispGameInfo *pInfo);
extern const char D_00A54DB8[];
extern const char D_00A54DD0[];

static void _DrawDispInfoOnePlayer(RgDispGameInfo *pInfo, RgPlayer **players,
                                   u32 nHost, u32 nSub)
{
    RgPlayer *hostPlayer;
    RgPlayer *subPlayer;

    if (nHost >= 2U) {
        assert_prog(D_00A54DB8, D_00A54CC0, 0x1F8);
    }
    if (nSub >= 2U) {
        assert_prog(D_00A54DD0, D_00A54CC0, 0x1F9);
    }
    hostPlayer = players[nHost];
    if (hostPlayer != 0) {
        RgDispGameInfoSetHostRobot(pInfo, RgPlayerGetRobot(hostPlayer));
        subPlayer = players[nSub];
        if (subPlayer != 0) {
            RgDispGameInfoSetSubRobot(pInfo, RgPlayerGetRobot(subPlayer));
        }
    }
    RgDispGameInfoPassTime(pInfo, 0.033333335f);
    RgDispGameInfoDisplay(pInfo);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _DrawDispInfo);

extern RgBattleCommonDataEnv *InstanceOfRgBattleCommonData(void);
extern int RgBattleCommonDataGetDispTex(RgBattleCommonDataEnv *pEnv);
extern int RgBattleCommonDataTimeFont(RgBattleCommonDataEnv *pEnv);
extern RgDispLife *CreateRgDispLife(int dispTex, int timeFont);
/*
 * This TU's own local prototype for CreateRgDispWpn1P (ov12/tu041): the
 * accepted definition there takes no argument, but this call site's
 * compiled bytes evaluate and pass RgBattleCommonDataGetDispTex's result
 * anyway, so the argument is declared to match the call.
 */
extern RgDispWpn1P *CreateRgDispWpn1P(int dispTex);
extern RgDispWpn2P *CreateRgDispWpn2P(void);
extern RgGameCollision *CreateRgGameCollision(void);
extern PlayerList *_CreatePlayerList(void);

static void _InitBattleMgr(RgBattleMgr *pMgr)
{
    RgBattleCommonDataEnv *pData;
    int dispTex;

    pData = InstanceOfRgBattleCommonData();
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 0x250);
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

extern void DisposeRgGameCollision(RgGameCollision *pGameColi);
extern void DisposeRgDispLife(RgDispLife *pDisp);
extern void DisposeRgDispWpn1P(RgDispWpn1P *pDisp);
extern void DisposeRgDispWpn2P(RgDispWpn2P *pDisp);
extern void DisposeRgFileSysData_sub(RgFileSysData *pFile, const char *pFileName,
                                     int line);
extern struct RgCharMgr *InstanceOfRgCharMgrClear(void);

static void _DisposeBattleMgr(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 611);
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
        DisposeRgFileSysData_sub(pMgr->fileSysData, D_00A54CC0, 622);
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

extern RgCharMgr *InstanceOfRgCharMgr(void);
extern void RgCharMgrControl(RgCharMgr *pMgr);
extern void RgCharMgrGC(RgCharMgr *pMgr);
static void _ControlPlayerList(PlayerList *pList);

static void _ControlBattle(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 0x359);
    }
    if (pMgr->playerControl != 0) {
        _ControlPlayerList(pMgr->playerList);
    }
    RgCharMgrControl(InstanceOfRgCharMgr());
    RgCharMgrGC(InstanceOfRgCharMgr());
}

extern XrgPaint2D *InstanceOfXrgPaint2D(void);
extern void RgCharMgrDisp(RgCharMgr *pMgr);
extern void RgDispLifeDisp(RgDispLife *pLife);
extern void RgDispWpn1PDisp(RgDispWpn1P *pDisp);
extern void RgDispWpn2PDisp(RgDispWpn2P *pDisp);
extern void XrgPaint2DAlpha(XrgPaint2D *pPaint, int alpha);
extern void XrgPaint2DColor(XrgPaint2D *pPaint, int *color);
extern void XrgPaint2DDrawXYWH(XrgPaint2D *pPaint, int, int, int, int, int);
static void _DispPlayerList(PlayerList *pList);
static void _DrawDispInfo(DispInfo *pInfo);

static void _DispBattle(RgBattleMgr *pMgr)
{
    int color[4];
    XrgPaint2D *pPaint;
    int mode;

    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 0x365);
    }
    mode = pMgr->mode;
    if ((mode == 0) || (mode == 3)) {
        memset(color, 0, sizeof(color));
        color[3] = 0x50;
        pPaint = InstanceOfXrgPaint2D();
        XrgPaint2DAlpha(pPaint, 0);
        XrgPaint2DColor(pPaint, color);
        XrgPaint2DDrawXYWH(pPaint, 0, 0xFE, 0, 4, 0x1C0);
    }
    _DispPlayerList(pMgr->playerList);
    RgCharMgrDisp(InstanceOfRgCharMgr());
    _DrawDispInfo(pMgr->dispInfo);
    RgDispLifeDisp(pMgr->dispLife);
    RgDispWpn1PDisp(pMgr->dispWpn1P);
    RgDispWpn2PDisp(pMgr->dispWpn2P);
}

static int _GetDeadList(PlayerList *pList);
static void _PassTimePlayerList(PlayerList *pList, float deltaTime);
static void _PassTimeBattleField(BattleField *pField, float deltaTime);
extern void RgCharMgrPassTime(RgCharMgr *pMgr, float deltaTime);
extern void RgGameCollisionJob(RgGameCollision *pGameCollision);
extern void RgDispLifeSetTimer(RgDispLife *pLife, float timer);
extern void RgDispLifePassTime(RgDispLife *pLife, float deltaTime);
extern void RgDispWpn1PPassTime(RgDispWpn1P *pDisp, float deltaTime);
extern void RgDispWpn2PPassTime(RgDispWpn2P *pDisp, float deltaTime);
extern float RgBattleMgrGetPlayerDamage(RgBattleMgr *pMgr, RgRobot *robot);

static void _PassTimeBattle(RgBattleMgr *pMgr, float deltaTime)
{
    int timeExpired;

    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 0x383);
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
            pMgr->result = 10;
        } else if ((deadPlayers & 2) != 0) {
            pMgr->result = 9;
        } else if (timeExpired != 0) {
            float playerDamage[2];

            pMgr->result = 4;
            playerDamage[0] = RgBattleMgrGetPlayerDamage(pMgr, 0);
            playerDamage[1] = RgBattleMgrGetPlayerDamage(pMgr, (RgRobot *) 1);
            if (playerDamage[0] < playerDamage[1]) {
                pMgr->result |= 1;
            } else if (playerDamage[1] < playerDamage[0]) {
                pMgr->result |= 2;
            } else {
                pMgr->result |= 3;
            }
        }
    }
    RgDispLifeSetTimer(pMgr->dispLife, pMgr->remainingTime);
    RgDispLifePassTime(pMgr->dispLife, deltaTime);
    RgDispWpn1PPassTime(pMgr->dispWpn1P, deltaTime);
    RgDispWpn2PPassTime(pMgr->dispWpn2P, deltaTime);
}

static void _InitBattleMgr(RgBattleMgr *pMgr);

RgBattleMgr *CreateRgBattleMgr(void)
{
    RgBattleMgr *pMgr = RgHeapAlloc(InstanceOfRgHeap(), 0x38, D_00A54CC0, 1015);

    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 1016);
    }
    _InitBattleMgr(pMgr);
    return pMgr;
}

static void _DisposeBattleMgr(RgBattleMgr *pMgr);

void DisposeRgBattleMgr(RgBattleMgr *pMgr)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 1023);
    }
    _DisposeBattleMgr(pMgr);
    RgHeapFree(InstanceOfRgHeap(), pMgr, D_00A54CC0, 1025);
}

static void _BattleMgrPlayerControl(RgBattleMgr *pMgr, int enable);

void RgBattleMgrSetPlayerControl(RgBattleMgr *pMgr, int enable)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 0x40A);
    }
    _BattleMgrPlayerControl(pMgr, enable);
}

static void _BattleMgrActivateTime(RgBattleMgr *pMgr, int active);

void RgBattleMgrActivateTimer(RgBattleMgr *pMgr, int active)
{
    if (pMgr == 0) {
        assert_prog(D_00A54DE8, D_00A54CC0, 0x411);
    }
    _BattleMgrActivateTime(pMgr, active);
}

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
