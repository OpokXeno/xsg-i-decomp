/*
 * OV12 original TU 41: 0x00a245d8..0x00a251a8 (13 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_battle_common_data.h"
#include "ov12/rg_disp_wpn1p.h"
#include "ov12/rg_gauge_disp.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_weapon_db.h"
#include "ov12/rg_robot.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_disp.h"
#include "ov12/rg_font.h"

/* Screen layout of the HUD. */
#define RG_DISP_WPN1P_BOARD_Y 0x160
#define RG_DISP_WPN1P_PAINT_PRIO 4

static void _InitDisp(RgDispWpn1P *pDisp);
static void _DestructDisp(RgDispWpn1P *pDisp);
static void _SetWepDisp(WepDisp *pWepDisp, RgWeapon *weapon,
                        unsigned int index);
static void _UpdateWepDisp(WepDisp *pWepDisp, RgStatus *pRobot);
static void _DispWepDisp(WepDisp *pWepDisp, XrgPaint2D *paint,
                         RgFont *bulletFont, RgFont *weaponFont);

/* Assert messages and the source path for the assert_prog calls in this file. */
const char dispWpn1pAssertDispNotNil[] = "pDisp != NIL";
const char dispWpn1pSourcePath[] = "../rg_disp_wpn1p.euc.c";
const char dispWpn1pAssertBxxNotNil[] = "pBxx != NIL";

static void _InitWepDisp(WepDisp *pWepDisp, RgBxx *pBxx)
{
    if (pWepDisp == 0) {
        assert_prog(dispWpn1pAssertDispNotNil, dispWpn1pSourcePath, 41);
    }
    if (pBxx == 0) {
        assert_prog(dispWpn1pAssertBxxNotNil, dispWpn1pSourcePath, 42);
    }
    pWepDisp->gauge = CreateRgGauge();
    pWepDisp->bxx = pBxx;
}

static void _DestructWepDisp(WepDisp *pWepDisp)
{
    DisposeRgGauge(pWepDisp->gauge);
}

const char dispWpn1pBarDisabledPictureName[] = "bar_wepd.bmp";
const char dispWpn1pBarLeftPictureName[] = "bar_wepl.bmp";
const char dispWpn1pAssertBarTexNotNil[] = "pBarTex != NIL";
const char dispWpn1pBarRightPictureName[] = "bar_wepr.bmp";
const char dispWpn1pBarBothPictureName[] = "bar_wepb.bmp";

/*
 * Binds a weapon to a weapon display slot and sets the gauge's bar picture, position
 * and activity for it. A null weapon clears the slot and deactivates its gauge.
 */
static void _SetWepDisp(WepDisp *pWepDisp, RgWeapon *weapon,
                        unsigned int index)
{
    RgBxx *archive;
    RgGaugeDisp *gaugeDisp;
    RgBxxPic *restPic;
    RgBxxPic *barPic;

    if (pWepDisp == 0) {
        assert_prog(dispWpn1pAssertDispNotNil, dispWpn1pSourcePath, 58);
    }
    if (weapon == 0) {
        pWepDisp->index = index;
        pWepDisp->weapon = 0;
        RgGaugeSetActivity(pWepDisp->gauge, 0);
        return;
    }
    RgWeaponGetEss(weapon);
    gaugeDisp = RgGaugeGetDisp(pWepDisp->gauge);
    if (pWepDisp->weapon != weapon || pWepDisp->index != index ||
        !RgGaugeDispIsDrawable(gaugeDisp)) {
        pWepDisp->weapon = weapon;
        pWepDisp->index = index;
        archive = pWepDisp->bxx;
        if ((RgWeaponGetControlFlag(weapon) & RG_WEAPON_CONTROL_KIND_MASK) != RG_WEAPON_CONTROL_KIND_UNARMED) {
            RgGaugeSetActivity(pWepDisp->gauge, 1);
            RgGaugeSetValue(pWepDisp->gauge, RgWeaponGetShotNum(weapon));
        } else {
            RgGaugeSetActivity(pWepDisp->gauge, 0);
        }
        restPic = RgBxxGetPic(archive, dispWpn1pBarDisabledPictureName);
        switch ((int)index) {
        case 0:
            barPic = RgBxxGetPic(archive, dispWpn1pBarLeftPictureName);
            RgGaugeDispSetBarUVWH(gaugeDisp, 2, 15, 74, 11);
            RgGaugeDispSetPos(gaugeDisp, 114, 383);
            if (barPic == 0) {
                assert_prog(dispWpn1pAssertBarTexNotNil, dispWpn1pSourcePath, 105);
            }
            break;
        case 1:
            barPic = RgBxxGetPic(archive, dispWpn1pBarRightPictureName);
            RgGaugeDispSetBarUVWH(gaugeDisp, 2, 6, 74, 11);
            RgGaugeDispSetPos(gaugeDisp, 338, 374);
            if (barPic == 0) {
                assert_prog(dispWpn1pAssertBarTexNotNil, dispWpn1pSourcePath, 111);
            }
            break;
        case 2:
            barPic = RgBxxGetPic(archive, dispWpn1pBarBothPictureName);
            RgGaugeDispSetBarUVWH(gaugeDisp, 14, 11, 74, 11);
            RgGaugeDispSetPos(gaugeDisp, 302, 395);
            if (barPic == 0) {
                assert_prog(dispWpn1pAssertBarTexNotNil, dispWpn1pSourcePath, 117);
            }
            break;
        default:
            barPic = 0;
            break;
        }
        RgGaugeDispSetTex(gaugeDisp, barPic, restPic);
        RgGaugeDispSetRestUVWH(gaugeDisp, 0, 0, 0, 11);
        RgGaugeDispSetAlpha(gaugeDisp, 0);
    }
}

/* Re-reads the weapon of this slot from the robot and rebinds the slot when it changed. */
static void _UpdateWepDisp(WepDisp *pWepDisp, RgStatus *pRobot)
{
    RgWeapon *weapon;

    if (pWepDisp == 0) {
        assert_prog(dispWpn1pAssertDispNotNil, dispWpn1pSourcePath, 137);
    }
    if (pRobot == 0) {
        pWepDisp->weapon = 0;
        return;
    }
    weapon = RgRobotGetWeapon(pRobot, pWepDisp->index);
    if (weapon != pWepDisp->weapon) {
        _SetWepDisp(pWepDisp, weapon, pWepDisp->index);
    }
}

static void _PassTimeWepDisp(WepDisp *pWepDisp, float deltaTime)
{
    RgWeapon *weapon;

    weapon = pWepDisp->weapon;
    if (weapon != 0) {
        RgGaugeSetValue(pWepDisp->gauge, RgWeaponGetShotNum(weapon));
        RgGaugePassTime(pWepDisp->gauge, deltaTime);
    }
}

typedef struct WepTextPositions {
    int xy[3][2];
} WepTextPositions;
const WepTextPositions dispWpn1pTextPositionTable = {{{8, 365}, {452, 366}, {416, 401}}};
const char dispWpn1pUnarmedText[] = "UNARMED";

/* Draws the remaining shot count and the weapon name of a slot, or UNARMED when it has no weapon. */
static void _DispWepDisp(WepDisp *pWepDisp, XrgPaint2D *paint,
                         RgFont *bulletFont, RgFont *weaponFont)
{
    static int s_anTextPos[3][2] = {{183, 359}, {392, 351}, {243, 383}};
    static int s_abOrder[3] = {1, 1, 0};
    WepTextPositions textPositions = dispWpn1pTextPositionTable;
    int index;
    char restNumber[64];
    RgWeaponEssence *essence;

    if (pWepDisp == 0) {
        assert_prog(dispWpn1pAssertDispNotNil, dispWpn1pSourcePath, 186);
    }
    index = pWepDisp->index;
    if (pWepDisp->weapon != 0) {
        RgGaugeDraw(pWepDisp->gauge, paint);
        RgDispWpnDat_CreateRestNumStr(pWepDisp->weapon, restNumber);
        RgFontStr(paint, bulletFont, restNumber,
                  s_anTextPos[index][0], s_anTextPos[index][1],
                  s_abOrder[index]);
        if (weaponFont != 0) {
            essence = RgWeaponGetEss(pWepDisp->weapon);
            if (essence != 0) {
                RgFontStr(paint, weaponFont,
                          essence->databaseName,
                          textPositions.xy[index][0], textPositions.xy[index][1], 0);
            }
        }
    } else if (weaponFont != 0) {
        RgFontStr(paint, weaponFont, dispWpn1pUnarmedText,
                  textPositions.xy[index][0], textPositions.xy[index][1], 0);
    }
}


const char dispWpn1pBoardSaPictureName[] = "board_sa.bmp";
const char dispWpn1pBoardSbPictureName[] = "board_sb.bmp";
const char dispWpn1pBoostPictureName[] = "boost.bmp";
const char dispWpn1pMphPictureName[] = "mph1.bmp";
const char dispWpn1pBarDashPictureName[] = "bar_dash.bmp";

/*
 * Creates the paint, fonts, pictures and dash gauge from the battle common data, then
 * initializes the four weapon slots.
 */
static void _InitDisp(RgDispWpn1P *pDisp)
{
    RgBattleCommonDataEnv *env;
    RgBxx *dispTexArchive;
    RgGaugeDisp *gaugeDisp;
    RgBxxPic *barPic;
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(dispWpn1pAssertDispNotNil, dispWpn1pSourcePath, 361);
    }
    env = InstanceOfRgBattleCommonData();
    pDisp->robot = 0;
    pDisp->paint = CreateXrgPaint2D_sub(dispWpn1pSourcePath, 365);
    XrgPaint2DSetDrawPrio(pDisp->paint, RG_DISP_WPN1P_PAINT_PRIO);

    dispTexArchive = RgBattleCommonDataGetDispTex(env);
    pDisp->bulletFont = RgBattleCommonDataBulletFont(env);
    pDisp->weaponFont = RgBattleCommonDataDispWeaponFont(env);
    pDisp->timeFont = RgBattleCommonDataTimeFont(env);
    pDisp->boardSaPic = RgBxxGetPic(dispTexArchive, dispWpn1pBoardSaPictureName);
    pDisp->boardSbPic = RgBxxGetPic(dispTexArchive, dispWpn1pBoardSbPictureName);
    pDisp->boostPic = RgBxxGetPic(dispTexArchive, dispWpn1pBoostPictureName);
    pDisp->mphPic = RgBxxGetPic(dispTexArchive, dispWpn1pMphPictureName);

    pDisp->gauge = CreateRgGauge();
    gaugeDisp = RgGaugeGetDisp(pDisp->gauge);
    barPic = RgBxxGetPic(dispTexArchive, dispWpn1pBarDashPictureName);
    RgGaugeDispSetTex(gaugeDisp, barPic, 0);
    RgGaugeDispSetBarUVWH(gaugeDisp, 14, 8, 183, 12);
    RgGaugeDispSetAlpha(gaugeDisp, 1);
    RgGaugeDispSetPos(gaugeDisp, 142, 424);

    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        _InitWepDisp(&pDisp->wep[i], dispTexArchive);
    }
}



static void _DestructDisp(RgDispWpn1P *pDisp)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(dispWpn1pAssertDispNotNil, dispWpn1pSourcePath, 404);
    }
    DisposeXrgPaint2D_sub(pDisp->paint, dispWpn1pSourcePath, 406);
    DisposeRgGauge(pDisp->gauge);
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        _DestructWepDisp(&pDisp->wep[i]);
    }
}

RgDispWpn1P *CreateRgDispWpn1P(RgBxx *dispTex)
{
    RgDispWpn1P *pDisp;

    pDisp = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgDispWpn1P), dispWpn1pSourcePath, 417);
    _InitDisp(pDisp);
    return pDisp;
}

void DisposeRgDispWpn1P(RgDispWpn1P *pDisp)
{
    if (pDisp == 0) {
        assert_prog(dispWpn1pAssertDispNotNil, dispWpn1pSourcePath, 425);
    }
    _DestructDisp(pDisp);
    RgHeapFree(InstanceOfRgHeap(), pDisp, dispWpn1pSourcePath, 427);
}


/* Attaches the robot to the display and binds each of its weapons to a slot. */
void RgDispWpn1PSetRobot(RgDispWpn1P *pDisp, RgStatus *pRobot)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(dispWpn1pAssertDispNotNil, dispWpn1pSourcePath, 439);
    }
    pDisp->robot = pRobot;
    if (pRobot != 0) {
        for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
            _SetWepDisp(&pDisp->wep[i], RgRobotGetWeapon(pRobot, i), i);
        }
    }
}


/* Advances the dash gauge and each weapon slot by deltaTime. */
void RgDispWpn1PPassTime(RgDispWpn1P *pDisp, float deltaTime)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(dispWpn1pAssertDispNotNil, dispWpn1pSourcePath, 459);
    }
    if (pDisp->robot != 0) {
        RgGaugeSetValue(pDisp->gauge, RgRobotGetDashTime(pDisp->robot));
        RgGaugePassTime(pDisp->gauge, deltaTime);
    }
    for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
        _UpdateWepDisp(&pDisp->wep[i], pDisp->robot);
        _PassTimeWepDisp(&pDisp->wep[i], deltaTime);
    }
}


/*
 * Draws the HUD for the attached robot: the board pictures, the dash gauge, the speed
 * text and each weapon slot, then flushes the paint.
 */
void RgDispWpn1PDisp(RgDispWpn1P *pDisp)
{
    unsigned int i;
    XrgPaint2D *paint;
    WepDisp *wep;
    char speedStr[0x40];

    if (pDisp == 0) {
        assert_prog(dispWpn1pAssertDispNotNil, dispWpn1pSourcePath, 483);
    }
    paint = pDisp->paint;
    if (pDisp->robot != 0) {
        if (pDisp->boardSbPic != 0) {
            XrgPaint2DUseTexture(paint, pDisp->boardSbPic);
            XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
            XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE, 0, RG_DISP_WPN1P_BOARD_Y, 0, 0);
        }
        if (pDisp->boardSaPic != 0) {
            XrgPaint2DUseTexture(paint, pDisp->boardSaPic);
            XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
            XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE, 0, RG_DISP_WPN1P_BOARD_Y, 0, 0);
        }
        if (pDisp->boostPic != 0) {
            XrgPaint2DUseTexture(paint, pDisp->boostPic);
            XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
            XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE, 0xA0, 0x190, 0, 0);
        }
        RgGaugeDraw(pDisp->gauge, paint);
        RgDispWpnDat_CreateSpeedStr(pDisp->robot, speedStr);
        RgFontStr(paint, pDisp->timeFont, speedStr, 0x5C, 0x19F, 1);
        for (i = 0; i < RG_EQUIP_TYPE_NUM; i++) {
            wep = &pDisp->wep[i];
            _UpdateWepDisp(wep, pDisp->robot);
            _DispWepDisp(wep, paint, pDisp->bulletFont, pDisp->weaponFont);
        }
        XrgPaint2DFlush(paint);
    }
}
