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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _GetStudioBattleField);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _GetCameraBattleField);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_battlemgr", _PassTimeBattle);

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
