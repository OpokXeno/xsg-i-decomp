/*
 * OV12 original TU 42: 0x00a251a8..0x00a25d30 (17 functions)
 */
#include "common.h"
#include "rg_disp_wpn2p.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern RgHeap *InstanceOfRgHeap(void);

extern float RgRobotGetDashTime(RgStatus *pRobot);
extern void RgGaugeSetValue(void *gauge, float value);
extern void RgGaugePassTime(void *gauge, float deltaTime);

static void _InitDisp(RgDispWpn2P *pDisp);
static void _DestructRobInfo(RobInfo *pInfo);
static void _SetRobInfo(RobInfo *pInfo, RgStatus *pRobot, int side);

/* ov12:0x00a54780 "../rg_disp_wpn2p.euc.c" (source filename, scaffold-owned
 * per config/tu-build.json data_ownership: this .rodata window is still
 * owner "asm"). */
extern const char D_00A54780[];
/* ov12:0x00a54770 "pWep != NIL" */
extern const char D_00A54770[];
/* ov12:0x00a547a0 "pInfo != NIL" */
extern const char D_00A547A0[];
/* ov12:0x00a547f0 "pDisp != NIL" */
extern const char D_00A547F0[];

/*
 * _InitRobInfo (ov12:0x00a254a0) and _SetRobInfo (ov12:0x00a255e8) are still
 * INCLUDE_ASM below; this TU's own single-underscore helpers are all LOCAL,
 * so their forward prototypes stay static like the ones above.
 */
static void _InitRobInfo(RobInfo *pInfo);

extern void *RgRobotGetWeapon(RgStatus *pRobot, unsigned int eSide);
extern void *InstanceOfRgBattleCommonData(void);
extern int RgBattleCommonDataDispWeaponFont(void *env);
extern int RgBattleCommonDataBulletFont(void *env);
extern int RgBattleCommonDataGetDispTex(void *env);
extern int RgBxxGetPic(int archive, const char *pictureName);
extern char *RgWeaponGetEss(void *weapon);
extern void RgFontStr(void *paint, int fontId, const char *text, int x,
                      int y, int color);
extern void RgDispWpnDat_CreateRestNumStr(void *weapon, char *buffer);
extern void DisposeXrgPaint2D_sub(void *paint, const char *source_file,
                                  int line);
extern void DisposeRgGauge(void *gauge);
extern void RgGaugeDraw(void *gauge, void *paint);
extern void XrgPaint2DAlpha(void *paint, int blendMode);
extern void XrgPaint2DDrawXYWH(void *paint, int mode, int x, int y,
                               int width, int height);
extern void XrgPaint2DFlush(void *paint);
extern void XrgPaint2DSetUVOffset(void *paint, int u, int v);
extern void XrgPaint2DSetUVSize(void *paint, int width, int height);
extern void XrgPaint2DUseTexture(void *paint, int texture);

/* ov12:0x00a4fb18 "anYPos_0", size 0xc: the on-screen Y position of each
 * weapon slot's text, indexed by WepInfo.index. */
extern int anYPos_0[3];

/* ov12:0x00a54798 "UNARMED" */
extern const char D_00A54798[];
/* ov12:0x00a54800 "wpn_all.bmp" */
extern const char D_00A54800[];

/*
 * Name field offset within the weapon-essence record RgWeaponGetEss
 * returns; only this field is evidenced here (docs/style.md, "Struct
 * fields, not offset casts" -- narrowly evidenced scalar access).
 */
#define WEP_ESSENCE_NAME_OFFSET 0x330

static void _InitWep(WepInfo *pWep)
{
    if (pWep == 0) {
        assert_prog(D_00A54770, D_00A54780, 0x28);
    }
    pWep->weapon = 0;
    pWep->index = -1;
}

static void _DestructWep(WepInfo *pWep)
{
    if (pWep == 0) {
        assert_prog(D_00A54770, D_00A54780, 0x31);
    }
}

static void _SetWep(WepInfo *pWep, void *weapon, int index)
{
    if (pWep == 0) {
        assert_prog(D_00A54770, D_00A54780, 0x37);
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
    void *weapon;

    if (pWep == 0) {
        assert_prog(D_00A54770, D_00A54780, 75);
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

static void _DispWep(WepInfo *pWep, void *paint, int weaponPic,
                     int bulletFont, int x)
{
    void *env;
    int font;
    void *weapon;
    char *ess;
    char buf[0x80];

    if (pWep == 0) {
        assert_prog(D_00A54770, D_00A54780, 97);
    }
    env = InstanceOfRgBattleCommonData();
    font = RgBattleCommonDataDispWeaponFont(env);
    weapon = pWep->weapon;
    if (paint != 0) {
        if (weapon != 0) {
            if (font != 0) {
                ess = RgWeaponGetEss(weapon);
                if (ess != 0) {
                    RgFontStr(paint, font, ess + WEP_ESSENCE_NAME_OFFSET, x,
                             anYPos_0[pWep->index], 0);
                }
            }
            if (bulletFont != 0) {
                RgDispWpnDat_CreateRestNumStr(weapon, buf);
                RgFontStr(paint, bulletFont, buf, x + 0x40,
                         anYPos_0[pWep->index], 0);
            }
        } else if (font != 0) {
            RgFontStr(paint, font, D_00A54798, x, anYPos_0[pWep->index], 0);
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn2p", _InitRobInfo);

static void _DestructRobInfo(RobInfo *pInfo)
{
    unsigned int i;

    if (pInfo == 0) {
        assert_prog(D_00A547A0, D_00A54780, 188);
    }
    DisposeXrgPaint2D_sub(pInfo->paint, D_00A54780, 189);
    DisposeRgGauge(pInfo->gauge);
    for (i = 0; i < 3; i++) {
        _DestructWep(&pInfo->wep[i]);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn2p", _SetRobInfo);

static void _PassTimeRobInfo(RobInfo *pInfo, float deltaTime)
{
    RgStatus *robot;

    if (pInfo == 0) {
        assert_prog(D_00A547A0, D_00A54780, 0xFB);
    }
    robot = pInfo->robot;
    if (robot == 0) {
        return;
    }
    RgGaugeSetValue(pInfo->gauge, RgRobotGetDashTime(robot));
    RgGaugePassTime(pInfo->gauge, deltaTime);
}

static void _DispRobInfo(RobInfo *pInfo, int weaponPic, int bulletFont)
{
    unsigned int i;
    void *paint;

    if (pInfo == 0) {
        assert_prog(D_00A547A0, D_00A54780, 262);
    }
    if (pInfo->robot != 0) {
        paint = pInfo->paint;
        for (i = 0; i < 3; i++) {
            _UpdateWep(&pInfo->wep[i], pInfo->robot);
            _DispWep(&pInfo->wep[i], paint, weaponPic, bulletFont,
                    pInfo->posX);
        }
        RgGaugeDraw(pInfo->gauge, paint);
        if (pInfo->boardSbPic != 0) {
            XrgPaint2DUseTexture(paint, pInfo->boardSbPic);
            XrgPaint2DSetUVOffset(paint, 0x7A, 0x42);
            XrgPaint2DSetUVSize(paint, 0xEA, 0x1A);
            XrgPaint2DAlpha(paint, 0);
            XrgPaint2DDrawXYWH(paint, 0, pInfo->posX + 2, 0x1A6, 0xE8, 0x18);
        }
        if (pInfo->boardSaPic != 0) {
            XrgPaint2DUseTexture(paint, pInfo->boardSaPic);
            XrgPaint2DSetUVOffset(paint, 0x78, 0x40);
            XrgPaint2DSetUVSize(paint, 0xE8, 0x18);
            XrgPaint2DAlpha(paint, 0);
            XrgPaint2DDrawXYWH(paint, 0, pInfo->posX, 0x1A4, 0xE8, 0x18);
        }
        if (pInfo->boostPic != 0) {
            XrgPaint2DUseTexture(paint, pInfo->boostPic);
            XrgPaint2DAlpha(paint, 0);
            XrgPaint2DDrawXYWH(paint, 0x20, pInfo->posX + 0xA0, 0x190, 0, 0);
        }
        XrgPaint2DFlush(paint);
    }
}

static void _InitDisp(RgDispWpn2P *pDisp)
{
    void *env;
    int dispTex;

    if (pDisp == 0) {
        assert_prog(D_00A547F0, D_00A54780, 326);
    }
    _InitRobInfo(&pDisp->rob[0]);
    _InitRobInfo(&pDisp->rob[1]);
    env = InstanceOfRgBattleCommonData();
    pDisp->bulletFont = RgBattleCommonDataBulletFont(env);
    dispTex = RgBattleCommonDataGetDispTex(env);
    if (pDisp != 0) {
        pDisp->weaponPic = RgBxxGetPic(dispTex, D_00A54800);
    } else {
        pDisp->weaponPic = 0;
    }
}

static void _DestructDisp(RgDispWpn2P *pDisp)
{
    if (pDisp == 0) {
        assert_prog(D_00A547F0, D_00A54780, 0x158);
    }
    _DestructRobInfo(&pDisp->rob[0]);
    _DestructRobInfo(&pDisp->rob[1]);
}

RgDispWpn2P *CreateRgDispWpn2P(void)
{
    RgDispWpn2P *pDisp;

    pDisp = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgDispWpn2P), D_00A54780,
                        0x161);
    _InitDisp(pDisp);
    return pDisp;
}

void DisposeRgDispWpn2P(RgDispWpn2P *pDisp)
{
    if (pDisp == 0) {
        assert_prog(D_00A547F0, D_00A54780, 0x169);
    }
    _DestructDisp(pDisp);
    RgHeapFree(InstanceOfRgHeap(), pDisp, D_00A54780, 0x16B);
}

void RgDispWpn2PSetRobot(RgDispWpn2P *pDisp, RgStatus *pRobot1,
                         RgStatus *pRobot2)
{
    if (pDisp == 0) {
        assert_prog(D_00A547F0, D_00A54780, 0x175);
    }
    _SetRobInfo(&pDisp->rob[0], pRobot1, 0);
    _SetRobInfo(&pDisp->rob[1], pRobot2, 1);
}

void RgDispWpn2PPassTime(RgDispWpn2P *pDisp, float deltaTime)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(D_00A547F0, D_00A54780, 387);
    }
    for (i = 0; i < 2; i++) {
        _PassTimeRobInfo(&pDisp->rob[i], deltaTime);
    }
}

void RgDispWpn2PDisp(RgDispWpn2P *pDisp)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(D_00A547F0, D_00A54780, 399);
    }
    for (i = 0; i < 2; i++) {
        _DispRobInfo(&pDisp->rob[i], pDisp->weaponPic, pDisp->bulletFont);
    }
}
