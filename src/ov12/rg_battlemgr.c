/*
 * OV12 original TU 47: 0x00a28858..0x00a2ab70 (53 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_battlemgr.h"

const char D_00A54CA0[16] = "rg_camera.info";

const char D_00A54CB0[16] = "pStudio != NIL";

const char D_00A54CC0[24] = "../rg_battlemgr.euc.c";

const char D_00A54CD8[8] = "CAM";

const char D_00A54CE0[8] = "2";

const char D_00A54CE8[16] = "pList != NIL";

const char D_00A54CF8[16] = "i < PLAYER_MAX";

const char D_00A54D08[24] = "i < pList->m_uNum";

const char D_00A54D50[16] = "pField != NIL";

const char D_00A54D60[24] = "uIndex < pField->m_uNum";

const char D_00A54D78[32] = "pField->m_uNum < BTL_STUDIO_MAX";

const char D_00A54D98[16] = "pDisp != NIL";

const char D_00A54DA8[16] = "pInfo != NIL";

const char D_00A54DB8[24] = "0 <= nHost && nHost < 2";

const char D_00A54DD0[24] = "0 <= nSub && nSub < 2";

const char D_00A54DE8[16] = "pMgr != NIL";

const char D_00A54DF8[32] = "*** max = %f life = %f ***\n";

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

/* Linker witness for the original literal at ov12:0x00a54d20
 * ("pPlayer != NIL"). */
extern const char D_00A54D20[];
/* Linker witness for the original literal at ov12:0x00a54d30
 * ("pList->m_uNum < PLAYER_MAX"). */
extern const char D_00A54D30[];

static void _AddPlayerList(PlayerList *pList, RgPlayer *pPlayer)
{
    RgPlayer **players;
    RgPlayer **pSlot;
    RgPlayer **pNextSlot;
    u32 i;

    if (pList == 0) {
        assert_prog(D_00A54CE8, D_00A54CC0, 140);
    }
    if (pPlayer == 0) {
        assert_prog(D_00A54D20, D_00A54CC0, 141);
    }
    if ((u32)pList->count >= 2U) {
        assert_prog(D_00A54D30, D_00A54CC0, 142);
    }
    pSlot = &pList->player1P;
    for (i = 0; i < 2; i++) {
        if (*pSlot++ == pPlayer) {
            return;
        }
    }
    i = pList->count;
    players = &pList->player1P;
    pNextSlot = players + i;
    *pNextSlot = pPlayer;
    pList->count = i + 1;
}

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

extern void DisposeRgCamera(RgCamera *camera);
extern void RgGeomFree(RgGeom *geom);

static void _ClearBattleField(BattleField *pField)
{
    u32 i;

    if (pField == 0) {
        assert_prog(D_00A54D50, D_00A54CC0, 0x107);
    }
    for (i = 0; i < (u32)pField->count; i++) {
        if (pField->camera[i] != 0) {
            DisposeRgCamera(pField->camera[i]);
            pField->camera[i] = 0;
        }
        if (pField->screenBall[i] != 0) {
            RgGeomFree(pField->screenBall[i]);
            pField->screenBall[i] = 0;
        }
    }
    pField->count = 0;
    RgDrawCreateDrawStudioFullScreen(InstanceOfRgDraw());
}

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

static u32 _FullScreenBattleField(BattleField *pField)
{
    RgCamera *camera;

    if (pField == 0) {
        assert_prog(D_00A54D50, D_00A54CC0, 0x145);
    }
    if ((u32)pField->count >= 2U) {
        assert_prog(D_00A54D78, D_00A54CC0, 0x146);
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

extern void RgDispGameInfoSetStudio(RgDispGameInfo *pInfo, int studio);

static void _DrawDispInfo(DispInfo *pInfo)
{
    u32 mode;
    u32 i;

    if (pInfo == 0) {
        assert_prog(D_00A54D98, D_00A54CC0, 519);
    }
    mode = pInfo->mode;
    switch (mode) {
    case 0:
    case 3:
        RgDispGameInfoSetStudio(pInfo->gameInfo[0], 0);
        RgDispGameInfoSetStudio(pInfo->gameInfo[1], 1);
        for (i = 0; i < 2; i++) {
            _DrawDispInfoOnePlayer(pInfo->gameInfo[i], pInfo->players, i, 1 - i);
        }
        break;
    case 1:
        RgDispGameInfoSetStudio(pInfo->gameInfo[0], -2);
        _DrawDispInfoOnePlayer(pInfo->gameInfo[0], pInfo->players, 0, 1);
        return;
    case 2:
        RgDispGameInfoSetStudio(pInfo->gameInfo[1], -2);
        _DrawDispInfoOnePlayer(pInfo->gameInfo[1], pInfo->players, 1, 0);
        break;
    }
}

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

#include "ov12/rg_game_collision.h"
#include "ov12/rg_robot_control.h"

extern u32 RgBattleInitCreatePlayers(RgBattleInitModes *pInfo,
                                     RgPlayer **players, u32 maxCount);
extern RgFileSysData *RgBattleInitCreateBg(RgBattleInitModes *pInfo,
                                           RgGeomGroup *pGeomGroup);
extern void RgPlayerSetLightCost(RgPlayer *pPlayer);
extern void RgDispLifeSetRobot(RgDispLife *pDisp, RgRobot *pRobot1,
                               RgRobot *pRobot2);
extern void RgDispLifeSetVsMode(RgDispLife *pDisp, int vsMode);
extern void RgDispWpn1PSetRobot(RgDispWpn1P *pDisp, RgRobot *pRobot);
extern void RgDispWpn2PSetRobot(RgDispWpn2P *pDisp, RgRobot *pRobot1,
                                RgRobot *pRobot2);
extern RgGeomPoint *RgRobotGetGeom(RgRobot *pRobot);
extern void RgCameraSetting(RgCamera *pCamera, RgGeomPoint **pGeoms);
extern RgRobotControl *CreateRgRobotControlInput(RgRobot *pRobot,
                                                 RgCamera *pCamera, int padId);
extern void RgPlayerSetTraceCamera(RgPlayer *pPlayer, RgCamera *pCamera);
extern void RgPlayerSetControl(RgPlayer *pPlayer, RgRobotControl *pControl);
extern RgRobotControl *CreateRgEnemyControl(RgRobot *pRobot,
                                            RgRobot *pEnemyRobot,
                                            int enemyType);
extern void RgRobotSetTarget(RgRobot *pRobot, RgGeomPoint *pTarget);
extern RgGeomGroup *RgPlayerGetShotGeoms(RgPlayer *pPlayer);
extern RgGeomGroup *RgPlayerGetAtkGeoms(RgPlayer *pPlayer);
extern RgGeomGroup *RgPlayerGetBodyGeoms(RgPlayer *pPlayer);
extern RgGeomGroup *RgPlayerGetAdvGeoms(RgPlayer *pPlayer);
extern RgGeomGroup *RgPlayerGetEyeGeoms(RgPlayer *pPlayer);
extern void InitRgGeomGroupColiArg(RgGameColiArg *pArg);
extern void RgGameCollisionAdd(RgGameCollision *pGameColi,
                               RgGameColiArg *pArg);
extern void RgHandlerShotVsBG();
extern void RgHandlerWeaponVsBG();
extern void RgHandlerWeaponVsRobot();
extern void RgHandlerShotVsRobot();
extern void RgHandlerRobotVsRobot();
extern void RgHandlerRobotVsBG();
extern void RgHandlerCameraVsBG();

static void _InitBattle(RgBattleMgr *pMgr, RgBattleInitModes *pInfo)
{
    RgPlayer *players[4];
    RgRobot *robots[4];
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
        assert_prog(D_00A54DE8, D_00A54CC0, 663);
    }
    if (pInfo == 0) {
        assert_prog(D_00A54DA8, D_00A54CC0, 664);
    }
    pMgr->remainingTime = 180.0f;
    mode = pInfo->mode;
    pMgr->mode = mode;
    screenCount = (mode == 0 || mode == 3) ? _DoubleScreenBattleField(pMgr->battleField) : _FullScreenBattleField(pMgr->battleField);
    pMgr->fileSysData = RgBattleInitCreateBg(pInfo, pMgr->geomGroup);
    playerCount = RgBattleInitCreatePlayers(pInfo, players, 4);
    for (i = 0; i < playerCount; i++) {
        _AddPlayerList(pMgr->playerList, players[i]);
        robots[i] = RgPlayerGetRobot(players[i]);
    }
    if (mode == 0 || mode == 3) {
        for (i = 0; i < playerCount; i++) {
            RgPlayerSetLightCost(players[i]);
        }
    }
    _SetDispInfo(pMgr->dispInfo, mode, players[0], players[1]);
    RgDispLifeSetRobot(pMgr->dispLife, robots[0], robots[1]);
    if (mode == 0 || mode == 3) {
        RgDispWpn2PSetRobot(pMgr->dispWpn2P, robots[0], robots[1]);
        RgDispLifeSetVsMode(pMgr->dispLife, 1);
    } else {
        RgDispWpn1PSetRobot(pMgr->dispWpn1P, robots[0]);
        RgDispLifeSetVsMode(pMgr->dispLife, 0);
    }
    for (i = 0; i < screenCount; i++) {
        nextIndex = i + 1;
        camera = _GetCameraBattleField(pMgr->battleField, i);
        geoms[0] = RgRobotGetGeom(RgPlayerGetRobot(players[i]));
        geoms[1] = RgRobotGetGeom(
            RgPlayerGetRobot(players[nextIndex % playerCount]));
        RgCameraSetting(camera, geoms);
        playerBit = 1U << i;
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
        if (!(pInfo->mode & playerBit)) {
            RgRobot *robotA;
            RgRobot *robotB;
            RgRobotControl *control;
            RgPlayer *player;

            player = players[i];
            robotA = RgPlayerGetRobot(player);
            robotB = RgPlayerGetRobot(players[(i + 1) % playerCount]);
            control = CreateRgEnemyControl(robotA, robotB, pInfo->enemyType);
            RgPlayerSetControl(player, control);
        }
    }
    for (i = 0; i < playerCount; i++) {
        u32 j;

        for (j = i + 1; j < playerCount; j++) {
            RgRobot *robotA;
            RgRobot *robotB;
            RgGeomPoint *geomA;
            RgGeomPoint *geomB;

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
        collArg.unmodeled_04 = (unsigned int)RgHandlerShotVsBG;
        RgGameCollisionAdd(pMgr->gameCollision, &collArg);

        InitRgGeomGroupColiArg(&collArg);
        collArg.m_pGroup1 = RgPlayerGetAtkGeoms(players[i]);
        collArg.m_pGroup2 = pMgr->geomGroup;
        collArg.unmodeled_04 = (unsigned int)RgHandlerWeaponVsBG;
        RgGameCollisionAdd(pMgr->gameCollision, &collArg);

        for (j = i + 1; j < playerCount; j++) {
            InitRgGeomGroupColiArg(&collArg);
            collArg.m_pGroup1 = RgPlayerGetBodyGeoms(players[i]);
            collArg.m_pGroup2 = RgPlayerGetBodyGeoms(players[j]);
            collArg.unmodeled_04 = (unsigned int)RgHandlerRobotVsRobot;
            collArg.unmodeled_10 = (unsigned int)RgHandlerRobotVsRobot;
            RgGameCollisionAdd(pMgr->gameCollision, &collArg);

            InitRgGeomGroupColiArg(&collArg);
            collArg.m_pGroup1 = RgPlayerGetShotGeoms(players[j]);
            collArg.m_pGroup2 = RgPlayerGetBodyGeoms(players[i]);
            collArg.unmodeled_04 = (unsigned int)RgHandlerShotVsRobot;
            RgGameCollisionAdd(pMgr->gameCollision, &collArg);

            InitRgGeomGroupColiArg(&collArg);
            collArg.m_pGroup1 = RgPlayerGetShotGeoms(players[i]);
            collArg.m_pGroup2 = RgPlayerGetBodyGeoms(players[j]);
            collArg.unmodeled_04 = (unsigned int)RgHandlerShotVsRobot;
            RgGameCollisionAdd(pMgr->gameCollision, &collArg);

            InitRgGeomGroupColiArg(&collArg);
            collArg.m_pGroup1 = RgPlayerGetAtkGeoms(players[j]);
            collArg.m_pGroup2 = RgPlayerGetBodyGeoms(players[i]);
            collArg.unmodeled_04 = (unsigned int)RgHandlerWeaponVsRobot;
            RgGameCollisionAdd(pMgr->gameCollision, &collArg);

            InitRgGeomGroupColiArg(&collArg);
            collArg.m_pGroup1 = RgPlayerGetAtkGeoms(players[i]);
            collArg.m_pGroup2 = RgPlayerGetBodyGeoms(players[j]);
            collArg.unmodeled_04 = (unsigned int)RgHandlerWeaponVsRobot;
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
        collArg.unmodeled_04 = (unsigned int)RgHandlerRobotVsBG;
        RgGameCollisionAdd(pMgr->gameCollision, &collArg);
    }
    {
        RgGameColiArg cameraArg;

        InitRgGeomGroupColiArg(&cameraArg);
        cameraArg.m_pGroup1 = _GetGeomGroupBattleField(pMgr->battleField);
        cameraArg.m_pGroup2 = pMgr->geomGroup;
        cameraArg.unmodeled_04 = (unsigned int)RgHandlerCameraVsBG;
        RgGameCollisionAdd(pMgr->gameCollision, &cameraArg);
    }
}

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

static void _InitBattle(RgBattleMgr *pMgr, RgBattleInitModes *pInfo);

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
