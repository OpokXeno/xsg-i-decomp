/*
 * OV12 original TU 42: 0x00a251a8..0x00a25d30 (17 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_battle_common_data.h"
#include "ov12/rg_disp_wpn2p.h"
#include "ov12/rg_gauge_disp.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_weapon_db.h"
#include "ov12/rg_robot.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_disp.h"
#include "ov12/rg_font.h"

#define RG_DISP_WPN2P_PAINT_PRIO 4

static void _InitDisp(RgDispWpn2P *pDisp);
static void _DestructRobInfo(RobInfo *pInfo);
static void _SetRobInfo(RobInfo *pInfo, RgStatus *pRobot, int side);
static void _InitRobInfo(RobInfo *pInfo);

/* Defined here: the assert and file-name strings and the weapon-slot text rows. */
const char dispWpn2pAssertWeaponNotNil[] = "pWep != NIL";
const char dispWpn2pSourcePath[] = "../rg_disp_wpn2p.euc.c";
const char dispWpn2pUnarmedText[] = "UNARMED";
const char dispWpn2pAssertInfoNotNil[] = "pInfo != NIL";
const char dispWpn2pBarDashPictureName[] = "bar_dash.bmp";
const char dispWpn2pBoardSaPictureName[] = "board_sa.bmp";
const char dispWpn2pBoardSbPictureName[] = "board_sb.bmp";
const char dispWpn2pBoostPictureName[] = "boost.bmp";
const char dispWpn2pAssertDispNotNil[] = "pDisp != NIL";
const char dispWpn2pWeaponAllPictureName[] = "wpn_all.bmp";

/* The on-screen Y position of each weapon slot's text, indexed by WepInfo.index. */
static int anYPos_0[RG_EQUIP_TYPE_NUM] = { 0x168, 0x17C, 0x190 };

static void _InitWep(WepInfo *pWep)
{
    if (pWep == 0) {
        assert_prog(dispWpn2pAssertWeaponNotNil, dispWpn2pSourcePath, 0x28);
    }
    pWep->weapon = 0;
    pWep->index = -1;
}

static void _DestructWep(WepInfo *pWep)
{
    if (pWep == 0) {
        assert_prog(dispWpn2pAssertWeaponNotNil, dispWpn2pSourcePath, 0x31);
    }
}

static void _SetWep(WepInfo *pWep, RgWeapon *weapon, int index)
{
    if (pWep == 0) {
        assert_prog(dispWpn2pAssertWeaponNotNil, dispWpn2pSourcePath, 0x37);
    }
    pWep->weapon = weapon;
    pWep->index = index;
    if (weapon == 0 || index == -1) {
        pWep->index = index;
        pWep->weapon = 0;
    }
}

static void _UpdateWep(WepInfo *pWep, RgStatus *pRobot)
{
    RgWeapon *weapon;

    if (pWep == 0) {
        assert_prog(dispWpn2pAssertWeaponNotNil, dispWpn2pSourcePath, 75);
    }
    if (pRobot == 0) {
        pWep->weapon = 0;
        pWep->index = -1;
    } else if (pWep->index != -1) {
        weapon = RgRobotGetWeapon(pRobot, pWep->index);
        if (pWep->weapon != weapon) {
            _SetWep(pWep, weapon, pWep->index);
        }
    }
}

/* Draws one weapon slot's text row: the weapon name (or UNARMED) and its remaining count. */
static void _DispWep(WepInfo *pWep, XrgPaint2D *paint, RgBxxPic *weaponPic,
                     RgFont *bulletFont, int x)
{
    RgBattleCommonDataEnv *env;
    RgFont *font;
    RgWeapon *weapon;
    RgWeaponEssence *ess;
    char buf[0x80];

    if (pWep == 0) {
        assert_prog(dispWpn2pAssertWeaponNotNil, dispWpn2pSourcePath, 97);
    }
    env = InstanceOfRgBattleCommonData();
    font = RgBattleCommonDataDispWeaponFont(env);
    weapon = pWep->weapon;
    if (paint != 0) {
        if (weapon != 0) {
            if (font != 0) {
                ess = RgWeaponGetEss(weapon);
                if (ess != 0) {
                    RgFontStr(paint, font, ess->databaseName, x,
                             anYPos_0[pWep->index], 0);
                }
            }
            if (bulletFont != 0) {
                RgDispWpnDat_CreateRestNumStr(weapon, buf);
                RgFontStr(paint, bulletFont, buf, x + 0x40,
                         anYPos_0[pWep->index], 0);
            }
        } else if (font != 0) {
            RgFontStr(paint, font, dispWpn2pUnarmedText, x, anYPos_0[pWep->index], 0);
        }
    }
}

/* Creates the paint layer and gauge of one robot info block and clears its slots. */
static void _InitRobInfo(RobInfo *pInfo)
{
    unsigned int i;

    if (pInfo == 0) {
        assert_prog(dispWpn2pAssertInfoNotNil, dispWpn2pSourcePath, 171);
    }
    pInfo->robot = 0;
    pInfo->paint = CreateXrgPaint2D_sub(dispWpn2pSourcePath, 174);
    XrgPaint2DSetDrawPrio(pInfo->paint, RG_DISP_WPN2P_PAINT_PRIO);
    pInfo->posX = 0;
    pInfo->gauge = CreateRgGauge();
    pInfo->boardSaPic = 0;
    pInfo->boardSbPic = 0;
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        _InitWep(&pInfo->wep[i]);
    }
}

static void _DestructRobInfo(RobInfo *pInfo)
{
    unsigned int i;

    if (pInfo == 0) {
        assert_prog(dispWpn2pAssertInfoNotNil, dispWpn2pSourcePath, 188);
    }
    DisposeXrgPaint2D_sub(pInfo->paint, dispWpn2pSourcePath, 189);
    DisposeRgGauge(pInfo->gauge);
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        _DestructWep(&pInfo->wep[i]);
    }
}

/*
 * Binds a robot (or null) to one side's info block. The weapon slots, gauge and
 * board textures are set up only for a non-null robot; side picks the draw ID
 * and the X position.
 */
static void _SetRobInfo(RobInfo *pInfo, RgStatus *pRobot, int side)
{
    unsigned int i;
    RgBattleCommonDataEnv *battleData;
    RgBxx *displayTexture;
    RgBxxPic *barTexture;
    RgGaugeDisp *gaugeDisplay;

    if (pInfo == 0) {
        assert_prog(dispWpn2pAssertInfoNotNil, dispWpn2pSourcePath, 199);
    }
    pInfo->robot = pRobot;
    battleData = InstanceOfRgBattleCommonData();
    displayTexture = RgBattleCommonDataGetDispTex(battleData);
    if (pRobot == 0) {
        return;
    }
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        _SetWep(&pInfo->wep[i], RgRobotGetWeapon(pRobot, i), i);
    }
    barTexture = RgBxxGetPic(displayTexture, dispWpn2pBarDashPictureName);
    gaugeDisplay = RgGaugeGetDisp(pInfo->gauge);
    RgGaugeDispSetBarUVWH(gaugeDisplay, 14, 8, 183, 12);
    RgGaugeDispSetPos(gaugeDisplay, 114, 383);
    RgGaugeDispSetTex(gaugeDisplay, barTexture, 0);
    RgGaugeDispSetAlpha(gaugeDisplay, 1);
    pInfo->boardSaPic = RgBxxGetPic(displayTexture, dispWpn2pBoardSaPictureName);
    pInfo->boardSbPic = RgBxxGetPic(displayTexture, dispWpn2pBoardSbPictureName);
    pInfo->boostPic = RgBxxGetPic(displayTexture, dispWpn2pBoostPictureName);
    switch (side) {
    case 0:
        XrgPaint2DSetDrawID(pInfo->paint, 0);
        pInfo->posX = 8;
        break;
    case 1:
        XrgPaint2DSetDrawID(pInfo->paint, 1);
        pInfo->posX = 264;
        break;
    }
    RgGaugeDispSetPos(gaugeDisplay, pInfo->posX + 24, 426);
}

static void _PassTimeRobInfo(RobInfo *pInfo, float deltaTime)
{
    RgStatus *robot;

    if (pInfo == 0) {
        assert_prog(dispWpn2pAssertInfoNotNil, dispWpn2pSourcePath, 0xFB);
    }
    robot = pInfo->robot;
    if (robot == 0) {
        return;
    }
    RgGaugeSetValue(pInfo->gauge, RgRobotGetDashTime(robot));
    RgGaugePassTime(pInfo->gauge, deltaTime);
}

/* Draws one side's weapon rows, dash gauge and board pictures, then flushes the paint. */
static void _DispRobInfo(RobInfo *pInfo, RgBxxPic *weaponPic, RgFont *bulletFont)
{
    unsigned int i;
    XrgPaint2D *paint;

    if (pInfo == 0) {
        assert_prog(dispWpn2pAssertInfoNotNil, dispWpn2pSourcePath, 262);
    }
    if (pInfo->robot != 0) {
        paint = pInfo->paint;
        for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
            _UpdateWep(&pInfo->wep[i], pInfo->robot);
            _DispWep(&pInfo->wep[i], paint, weaponPic, bulletFont,
                    pInfo->posX);
        }
        RgGaugeDraw(pInfo->gauge, paint);
        if (pInfo->boardSbPic != 0) {
            XrgPaint2DUseTexture(paint, pInfo->boardSbPic);
            XrgPaint2DSetUVOffset(paint, 0x7A, 0x42);
            XrgPaint2DSetUVSize(paint, 0xEA, 0x1A);
            XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
            XrgPaint2DDrawXYWH(paint, 0, pInfo->posX + 2, 0x1A6, 0xE8, 0x18);
        }
        if (pInfo->boardSaPic != 0) {
            XrgPaint2DUseTexture(paint, pInfo->boardSaPic);
            XrgPaint2DSetUVOffset(paint, 0x78, 0x40);
            XrgPaint2DSetUVSize(paint, 0xE8, 0x18);
            XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
            XrgPaint2DDrawXYWH(paint, 0, pInfo->posX, 0x1A4, 0xE8, 0x18);
        }
        if (pInfo->boostPic != 0) {
            XrgPaint2DUseTexture(paint, pInfo->boostPic);
            XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
            XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE, pInfo->posX + 0xA0, 0x190, 0, 0);
        }
        XrgPaint2DFlush(paint);
    }
}

/* Initializes both robot info blocks, the bullet font and the weapon picture. */
static void _InitDisp(RgDispWpn2P *pDisp)
{
    RgBattleCommonDataEnv *env;
    RgBxx *dispTex;

    if (pDisp == 0) {
        assert_prog(dispWpn2pAssertDispNotNil, dispWpn2pSourcePath, 326);
    }
    _InitRobInfo(&pDisp->rob[0]);
    _InitRobInfo(&pDisp->rob[1]);
    env = InstanceOfRgBattleCommonData();
    pDisp->bulletFont = RgBattleCommonDataBulletFont(env);
    dispTex = RgBattleCommonDataGetDispTex(env);
    if (pDisp != 0) {
        pDisp->weaponPic = RgBxxGetPic(dispTex, dispWpn2pWeaponAllPictureName);
    } else {
        pDisp->weaponPic = 0;
    }
}

static void _DestructDisp(RgDispWpn2P *pDisp)
{
    if (pDisp == 0) {
        assert_prog(dispWpn2pAssertDispNotNil, dispWpn2pSourcePath, 0x158);
    }
    _DestructRobInfo(&pDisp->rob[0]);
    _DestructRobInfo(&pDisp->rob[1]);
}

RgDispWpn2P *CreateRgDispWpn2P(void)
{
    RgDispWpn2P *pDisp;

    pDisp = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgDispWpn2P), dispWpn2pSourcePath,
                        0x161);
    _InitDisp(pDisp);
    return pDisp;
}

void DisposeRgDispWpn2P(RgDispWpn2P *pDisp)
{
    if (pDisp == 0) {
        assert_prog(dispWpn2pAssertDispNotNil, dispWpn2pSourcePath, 0x169);
    }
    _DestructDisp(pDisp);
    RgHeapFree(InstanceOfRgHeap(), pDisp, dispWpn2pSourcePath, 0x16B);
}

void RgDispWpn2PSetRobot(RgDispWpn2P *pDisp, RgStatus *pRobot1,
                         RgStatus *pRobot2)
{
    if (pDisp == 0) {
        assert_prog(dispWpn2pAssertDispNotNil, dispWpn2pSourcePath, 0x175);
    }
    _SetRobInfo(&pDisp->rob[0], pRobot1, 0);
    _SetRobInfo(&pDisp->rob[1], pRobot2, 1);
}

void RgDispWpn2PPassTime(RgDispWpn2P *pDisp, float deltaTime)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(dispWpn2pAssertDispNotNil, dispWpn2pSourcePath, 387);
    }
    for (i = 0; i < RG_DISP_WPN2P_SIDE_NUM; i++) {
        _PassTimeRobInfo(&pDisp->rob[i], deltaTime);
    }
}

void RgDispWpn2PDisp(RgDispWpn2P *pDisp)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(dispWpn2pAssertDispNotNil, dispWpn2pSourcePath, 399);
    }
    for (i = 0; i < RG_DISP_WPN2P_SIDE_NUM; i++) {
        _DispRobInfo(&pDisp->rob[i], pDisp->weaponPic, pDisp->bulletFont);
    }
}
