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
static void _SetWepDisp(WepDisp *pWepDisp, RgWeapon *weapon,
                        unsigned int index);

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

extern int RgWeaponGetControlFlag(RgWeapon *weapon);
extern char *RgWeaponGetEss(RgWeapon *weapon);
extern int RgBxxGetPic(RgBxx *archive, const char *pictureName);
extern void *RgGaugeGetDisp(RgGauge *gauge);
extern int RgGaugeDispIsDrawable(void *gaugeDisp);
extern void RgGaugeSetActivity(RgGauge *gauge, int activity);
extern void RgGaugeDispSetTex(void *gaugeDisp, int barPic, int restPic);
extern void RgGaugeDispSetBarUVWH(void *gaugeDisp, int x, int y, int width,
                                   int height);
extern void RgGaugeDispSetRestUVWH(void *gaugeDisp, int x, int y, int width,
                                    int height);
extern void RgGaugeDispSetPos(void *gaugeDisp, int x, int y);
extern void RgGaugeDispSetAlpha(void *gaugeDisp, int alpha);
extern const char D_00A546B0[];
extern const char D_00A546C0[];
extern const char D_00A546D0[];
extern const char D_00A546E0[];
extern const char D_00A546F0[];

static void _SetWepDisp(WepDisp *pWepDisp, RgWeapon *weapon,
                        unsigned int index)
{
    RgBxx *archive;
    void *gaugeDisp;
    int restPic;
    int barPic;

    if (pWepDisp == 0) {
        assert_prog(D_00A54678, D_00A54688, 58);
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
        if ((RgWeaponGetControlFlag(weapon) & 7) != 4) {
            RgGaugeSetActivity(pWepDisp->gauge, 1);
            RgGaugeSetValue(pWepDisp->gauge, RgWeaponGetShotNum(weapon));
        } else {
            RgGaugeSetActivity(pWepDisp->gauge, 0);
        }
        restPic = RgBxxGetPic(archive, D_00A546B0);
        switch ((int)index) {
        case 0:
            barPic = RgBxxGetPic(archive, D_00A546C0);
            RgGaugeDispSetBarUVWH(gaugeDisp, 2, 15, 74, 11);
            RgGaugeDispSetPos(gaugeDisp, 114, 383);
            if (barPic == 0) {
                assert_prog(D_00A546D0, D_00A54688, 105);
            }
            break;
        case 1:
            barPic = RgBxxGetPic(archive, D_00A546E0);
            RgGaugeDispSetBarUVWH(gaugeDisp, 2, 6, 74, 11);
            RgGaugeDispSetPos(gaugeDisp, 338, 374);
            if (barPic == 0) {
                assert_prog(D_00A546D0, D_00A54688, 111);
            }
            break;
        case 2:
            barPic = RgBxxGetPic(archive, D_00A546F0);
            RgGaugeDispSetBarUVWH(gaugeDisp, 14, 11, 74, 11);
            RgGaugeDispSetPos(gaugeDisp, 302, 395);
            if (barPic == 0) {
                assert_prog(D_00A546D0, D_00A54688, 117);
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

static void _UpdateWepDisp(WepDisp *pWepDisp, RgStatus *pRobot)
{
    RgWeapon *weapon;
    extern void *RgRobotGetWeapon(RgStatus *pRobot, unsigned int eSide);

    if (pWepDisp == 0) {
        assert_prog(D_00A54678, D_00A54688, 137);
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
extern const WepTextPositions D_00A54700;
extern const char D_00A54718[];
extern const int s_anTextPos_0[3][2];
extern const int s_abOrder_1[3];
extern void RgDispWpnDat_CreateRestNumStr(RgWeapon *weapon, char *buffer);
extern void RgFontStr(void *paint, int fontId, const char *text, int x,
                      int y, int color);
extern void RgGaugeDraw(RgGauge *gauge, void *paint);

/*
 * RgWeaponGetEss returns opaque weapon-essence storage. The name consumed
 * here begins at +0x330; the same tail offset is independently used by the
 * two-player display and weapon-selection code.
 */
#define WEP_ESSENCE_NAME_OFFSET 0x330

static void _DispWepDisp(WepDisp *pWepDisp, void *paint, int bulletFont,
                         int weaponFont)
{
    WepTextPositions textPositions = D_00A54700;
    int index;
    char restNumber[64];
    char *essence;

    if (pWepDisp == 0) {
        assert_prog(D_00A54678, D_00A54688, 186);
    }
    index = pWepDisp->index;
    if (pWepDisp->weapon != 0) {
        RgGaugeDraw(pWepDisp->gauge, paint);
        RgDispWpnDat_CreateRestNumStr(pWepDisp->weapon, restNumber);
        RgFontStr(paint, bulletFont, restNumber,
                  s_anTextPos_0[index][0], s_anTextPos_0[index][1],
                  s_abOrder_1[index]);
        if (weaponFont != 0) {
            essence = RgWeaponGetEss(pWepDisp->weapon);
            if (essence != 0) {
                RgFontStr(paint, weaponFont,
                          &essence[WEP_ESSENCE_NAME_OFFSET],
                          textPositions.xy[index][0], textPositions.xy[index][1], 0);
            }
        }
    } else if (weaponFont != 0) {
        RgFontStr(paint, weaponFont, D_00A54718,
                  textPositions.xy[index][0], textPositions.xy[index][1], 0);
    }
}

extern void *InstanceOfRgBattleCommonData(void);
extern int RgBattleCommonDataGetDispTex(void *env);
extern int RgBattleCommonDataBulletFont(void *env);
extern int RgBattleCommonDataTimeFont(void *env);
extern int RgBattleCommonDataDispWeaponFont(void *env);
extern void *CreateXrgPaint2D_sub(const char *source_file, int line);
extern void XrgPaint2DSetDrawPrio(void *paint, int priority);
extern int RgBxxGetPic(RgBxx *archive, const char *pictureName);
extern void *RgGaugeGetDisp(RgGauge *gauge);
extern void RgGaugeDispSetTex(void *gaugeDisp, int barPic, int restPic);
extern void RgGaugeDispSetBarUVWH(void *gaugeDisp, int x, int y, int width,
                                  int height);
extern void RgGaugeDispSetPos(void *gaugeDisp, int x, int y);
extern void RgGaugeDispSetAlpha(void *gaugeDisp, int alpha);

extern const char D_00A54720[];
extern const char D_00A54730[];
extern const char D_00A54740[];
extern const char D_00A54750[];
extern const char D_00A54760[];

static void _InitDisp(RgDispWpn1P *pDisp)
{
    void *env;
    int dispTexHandle;
    void *gaugeDisp;
    int barPic;
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(D_00A54678, D_00A54688, 361);
    }
    env = InstanceOfRgBattleCommonData();
    pDisp->robot = 0;
    pDisp->paint = CreateXrgPaint2D_sub(D_00A54688, 365);
    XrgPaint2DSetDrawPrio(pDisp->paint, 4);

    dispTexHandle = RgBattleCommonDataGetDispTex(env);
    pDisp->bulletFont = RgBattleCommonDataBulletFont(env);
    pDisp->weaponFont = RgBattleCommonDataDispWeaponFont(env);
    pDisp->timeFont = RgBattleCommonDataTimeFont(env);
    pDisp->boardSaPic = RgBxxGetPic((RgBxx *)dispTexHandle, D_00A54720);
    pDisp->boardSbPic = RgBxxGetPic((RgBxx *)dispTexHandle, D_00A54730);
    pDisp->boostPic = RgBxxGetPic((RgBxx *)dispTexHandle, D_00A54740);
    pDisp->mphPic = RgBxxGetPic((RgBxx *)dispTexHandle, D_00A54750);

    pDisp->gauge = CreateRgGauge();
    gaugeDisp = RgGaugeGetDisp(pDisp->gauge);
    barPic = RgBxxGetPic((RgBxx *)dispTexHandle, D_00A54760);
    RgGaugeDispSetTex(gaugeDisp, barPic, 0);
    RgGaugeDispSetBarUVWH(gaugeDisp, 14, 8, 183, 12);
    RgGaugeDispSetAlpha(gaugeDisp, 1);
    RgGaugeDispSetPos(gaugeDisp, 142, 424);

    for (i = 0; i < 3; i++) {
        _InitWepDisp(&pDisp->wep[i], (RgBxx *)dispTexHandle);
    }
}

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

extern float RgRobotGetDashTime(RgStatus *pRobot);

void RgDispWpn1PPassTime(RgDispWpn1P *pDisp, float deltaTime)
{
    unsigned int i;

    if (pDisp == 0) {
        assert_prog(D_00A54678, D_00A54688, 459);
    }
    if (pDisp->robot != 0) {
        RgGaugeSetValue(pDisp->gauge, RgRobotGetDashTime(pDisp->robot));
        RgGaugePassTime(pDisp->gauge, deltaTime);
    }
    for (i = 0; i < 3; i++) {
        _UpdateWepDisp(&pDisp->wep[i], pDisp->robot);
        _PassTimeWepDisp(&pDisp->wep[i], deltaTime);
    }
}

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
