/*
 * OV12 original TU 41: 0x00a245d8..0x00a251a8 (13 functions)
 */
#include "common.h"
#include "rg_disp_wpn1p.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);

extern void *RgHeapAlloc(RgHeap *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern RgHeap *InstanceOfRgHeap(void);

extern RgGauge *CreateRgGauge(void);
extern void DisposeRgGauge(RgGauge *gauge);
extern void RgGaugeSetValue(RgGauge *gauge, float value);
extern void RgGaugePassTime(RgGauge *gauge, float deltaTime);

extern float RgWeaponGetShotNum(RgWeapon *weapon);

static void _InitDisp(RgDispWpn1P *pDisp);
static void _DestructDisp(RgDispWpn1P *pDisp);

/* ov12:0x00a54678 "pDisp != NIL" */
extern const char D_00A54678[];
/* ov12:0x00a54688 "../rg_disp_wpn1p.euc.c" (source filename, scaffold-owned
 * per config/tu-build.json data_ownership: this .rodata window is still
 * owner "asm"). */
extern const char D_00A54688[];
/* ov12:0x00a546a0 "pBxx != NIL" */
extern const char D_00A546A0[];

static void _InitWepDisp(WepDisp *pWepDisp, RgBxx *pBxx)
{
    if (pWepDisp == 0) {
        assert_prog(D_00A54678, D_00A54688, 41);
    }
    if (pBxx == 0) {
        assert_prog(D_00A546A0, D_00A54688, 42);
    }
    pWepDisp->gauge = CreateRgGauge();
    pWepDisp->bxx = pBxx;
}

static void _DestructWepDisp(WepDisp *pWepDisp)
{
    DisposeRgGauge(pWepDisp->gauge);
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn1p", _SetWepDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn1p", _UpdateWepDisp);

static void _PassTimeWepDisp(WepDisp *pWepDisp, float deltaTime)
{
    RgWeapon *weapon;

    weapon = pWepDisp->weapon;
    if (weapon != 0) {
        RgGaugeSetValue(pWepDisp->gauge, RgWeaponGetShotNum(weapon));
        RgGaugePassTime(pWepDisp->gauge, deltaTime);
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn1p", _DispWepDisp);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn1p", _InitDisp_00A24BC0);

extern void DisposeXrgPaint2D_sub(void *paint, const char *source_file,
                                  int line);

static void _SetWepDisp(WepDisp *pWepDisp, RgWeapon *weapon,
                        unsigned int index);

static void _DestructDisp(RgDispWpn1P *pDisp)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(D_00A54678, D_00A54688, 404);
    }
    DisposeXrgPaint2D_sub(pDisp->paint, D_00A54688, 406);
    DisposeRgGauge(pDisp->gauge);
    for (i = 0; i < 3; i++) {
        _DestructWepDisp(&pDisp->wep[i]);
    }
}

RgDispWpn1P *CreateRgDispWpn1P(void)
{
    RgDispWpn1P *pDisp;

    pDisp = RgHeapAlloc(InstanceOfRgHeap(), 0x58, D_00A54688, 417);
    _InitDisp(pDisp);
    return pDisp;
}

void DisposeRgDispWpn1P(RgDispWpn1P *pDisp)
{
    if (pDisp == 0) {
        assert_prog(D_00A54678, D_00A54688, 425);
    }
    _DestructDisp(pDisp);
    RgHeapFree(InstanceOfRgHeap(), pDisp, D_00A54688, 427);
}

extern void *RgRobotGetWeapon(RgStatus *pRobot, unsigned int eSide);

void RgDispWpn1PSetRobot(RgDispWpn1P *pDisp, RgStatus *pRobot)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(D_00A54678, D_00A54688, 439);
    }
    pDisp->robot = pRobot;
    if (pRobot != 0) {
        for (i = 0; i < 3; i++) {
            _SetWepDisp(&pDisp->wep[i], RgRobotGetWeapon(pRobot, i), i);
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_wpn1p", RgDispWpn1PPassTime);

extern void RgDispWpnDat_CreateSpeedStr(RgStatus *robot, char *buffer);
extern void RgFontStr(void *paint, int fontId, const char *text, int x,
                      int y, int color);
extern void RgGaugeDraw(RgGauge *gauge, void *paint);
extern void XrgPaint2DAlpha(void *paint, int blendMode);
extern void XrgPaint2DDrawXYWH(void *paint, int mode, int x, int y,
                               int width, int height);
extern void XrgPaint2DFlush(void *paint);
extern void XrgPaint2DUseTexture(void *paint, int texture);

static void _UpdateWepDisp(WepDisp *pWepDisp, RgStatus *pRobot);
static void _DispWepDisp(WepDisp *pWepDisp, void *paint, int bulletFont,
                         int weaponFont);

void RgDispWpn1PDisp(RgDispWpn1P *pDisp)
{
    unsigned int i;
    void *paint;
    WepDisp *wep;
    char speedStr[0x40];

    if (pDisp == 0) {
        assert_prog(D_00A54678, D_00A54688, 483);
    }
    paint = pDisp->paint;
    if (pDisp->robot != 0) {
        if (pDisp->boardSbPic != 0) {
            XrgPaint2DUseTexture(paint, pDisp->boardSbPic);
            XrgPaint2DAlpha(paint, 0);
            XrgPaint2DDrawXYWH(paint, 0x20, 0, 0x160, 0, 0);
        }
        if (pDisp->boardSaPic != 0) {
            XrgPaint2DUseTexture(paint, pDisp->boardSaPic);
            XrgPaint2DAlpha(paint, 0);
            XrgPaint2DDrawXYWH(paint, 0x20, 0, 0x160, 0, 0);
        }
        if (pDisp->boostPic != 0) {
            XrgPaint2DUseTexture(paint, pDisp->boostPic);
            XrgPaint2DAlpha(paint, 0);
            XrgPaint2DDrawXYWH(paint, 0x20, 0xA0, 0x190, 0, 0);
        }
        RgGaugeDraw(pDisp->gauge, paint);
        RgDispWpnDat_CreateSpeedStr(pDisp->robot, speedStr);
        RgFontStr(paint, pDisp->timeFont, speedStr, 0x5C, 0x19F, 1);
        for (i = 0; i < 3; i++) {
            wep = &pDisp->wep[i];
            _UpdateWepDisp(wep, pDisp->robot);
            _DispWepDisp(wep, paint, pDisp->bulletFont, pDisp->weaponFont);
        }
        XrgPaint2DFlush(paint);
    }
}
