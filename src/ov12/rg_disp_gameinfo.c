/*
 * OV12 original TU 38: 0x00a22a78..0x00a239d0 (17 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_disp_gameinfo.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
const char D_00A54560[16] = "pInfo != NIL";
const char D_00A54570[32] = "../rg_disp_gameinfo.euc.c";
extern void *InstanceOfRgBattleCommonData(void);
extern int RgBattleCommonDataGetDispTex(void *env);
extern XrgPaint2D *CreateXrgPaint2D_sub(const char *sourceFile, int line);
extern void XrgPaint2DSetDrawPrio(XrgPaint2D *paint, int prio);
extern void XrgPaint2DSetDrawID(XrgPaint2D *paint, int drawID);

void _InitInfo(RgDispGameInfo *pInfo)
{
    int dispTex;
    XrgPaint2D *paint;

    if (pInfo == 0) {
        assert_prog(D_00A54560, D_00A54570, 73);
    }
    pInfo->hostRobot = 0;
    pInfo->subRobot = 0;
    dispTex = RgBattleCommonDataGetDispTex(InstanceOfRgBattleCommonData());
    pInfo->value1 = 1.0f;
    pInfo->value2 = 1.0f;
    pInfo->value3 = 2;
    pInfo->studio = -2;
    pInfo->dispTex = dispTex;
    paint = CreateXrgPaint2D_sub(D_00A54570, 90);
    pInfo->paint = paint;
    XrgPaint2DSetDrawPrio(paint, 4);
    XrgPaint2DSetDrawID(pInfo->paint, pInfo->studio);
    pInfo->value4 = 0;
    pInfo->value5 = 0;
}


extern void DisposeXrgPaint2D_sub(XrgPaint2D *paint, const char *source_file,
                                  int line);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a54560 contains the assertion expression "pInfo != NIL".
 * ov12:0x00a54570 contains the source filename "../rg_disp_gameinfo.euc.c".
 */

void _DisposeInfo(RgDispGameInfo *pInfo) {
    if (pInfo == 0) {
        assert_prog(D_00A54560, D_00A54570, 100);
    }
    DisposeXrgPaint2D_sub(pInfo->paint, D_00A54570, 101);
}

void _SetHostRobot(RgDispGameInfo *pInfo, void *hostRobot) {
    if (pInfo == 0) {
        assert_prog(D_00A54560, D_00A54570, 108);
    }
    pInfo->hostRobot = hostRobot;
}

void _SetSubRobot(RgDispGameInfo *pInfo, void *subRobot) {
    if (pInfo == 0) {
        assert_prog(D_00A54560, D_00A54570, 114);
    }
    pInfo->subRobot = subRobot;
}

void _PassTimeInfo(RgDispGameInfo *pInfo, float deltaTime) {
    if (pInfo == 0) {
        assert_prog(D_00A54560, D_00A54570, 122);
    }
}

extern void *RgBxxGetPic(int archive, const char *pictureName);
extern void RgError(const char *message, const char *source_file, int line,
                    ...);
/*
 * The second argument selects a blend mode (XrgPaint2DAlpha switches over
 * 0..5; _DispInfo passes 1 to _paint_set_tex_alpha), not an alpha level.
 */
extern void XrgPaint2DAlpha(XrgPaint2D *paint, int blendMode);
extern void XrgPaint2DDrawXYWH(XrgPaint2D *paint, int mode, int x, int y,
                               int width, int height);
extern void XrgPaint2DUseTexture(XrgPaint2D *paint, void *pic);

/*
 * Mode bit 0x20 of XrgPaint2DRect.mode (documented in src/ov12/xrg_paint2d.h)
 * takes the drawn extent from the bound picture instead of width/height; the
 * spelling follows src/ov12/rg_piclist.c.
 */
#define XRG_PAINT2D_MODE_USE_PIC_SIZE 0x20

/*
 * ov12:0x00a54590 contains the error message "not exist '%s' picture".
 */
const char D_00A54590[24] = "not exist '%s' picture";

void _paint_one_texture_alpha(RgDispGameInfo *pInfo, const char *pictureName,
                              int x, int y, int blendMode) {
    void *pic;

    pic = RgBxxGetPic(pInfo->dispTex, pictureName);
    if (pic == 0) {
        RgError(D_00A54590, D_00A54570, 133, pictureName);
    }
    XrgPaint2DUseTexture(pInfo->paint, pic);
    XrgPaint2DAlpha(pInfo->paint, blendMode);
    XrgPaint2DDrawXYWH(pInfo->paint, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y,
                       0, 0);
}

void _paint_one_texture(RgDispGameInfo *pInfo, const char *pictureName, int x,
                        int y) {
    _paint_one_texture_alpha(pInfo, pictureName, x, y, 0);
}

void _paint_set_tex_alpha(RgDispGameInfo *pInfo, const char *pictureName,
                          int blendMode) {
    void *pic;

    pic = RgBxxGetPic(pInfo->dispTex, pictureName);
    if (pic == 0) {
        RgError(D_00A54590, D_00A54570, 150, pictureName);
    }
    XrgPaint2DUseTexture(pInfo->paint, pic);
    XrgPaint2DAlpha(pInfo->paint, blendMode);
}

/* DrawRect consumes the whole rectangle descriptor, including its
 * alignment gap; this caller follows the paint renderer's layout. */
typedef struct RgDispGameInfoColorRect {
    int mode;
    unsigned char quadword_alignment_gap[12];
    int x;
    int y;
    int width;
    int height;
    float scale;
    float angle;
} RgDispGameInfoColorRect;

extern void InitXrgPaint2DRect(RgDispGameInfoColorRect *rectangle, int mode);
extern void XrgPaint2DColor(XrgPaint2D *paint, const unsigned int *color);
extern void XrgPaint2DDrawRect(XrgPaint2D *paint,
                               RgDispGameInfoColorRect *rectangle);

/* Anchored, rotated, scaled rectangle with per-frame jitter. */
#define RG_DISP_GAME_INFO_COLOR_RECT_MODE 0x1E5

void _paint_xy_color(RgDispGameInfo *pInfo, int x, int y, float angle, float scale,
                     int intensity)
{
    unsigned int color[4] = {intensity, intensity, intensity, 127};
    RgDispGameInfoColorRect rectangle;

    InitXrgPaint2DRect(&rectangle, RG_DISP_GAME_INFO_COLOR_RECT_MODE);
    rectangle.x = x;
    rectangle.y = y;
    rectangle.scale = scale;
    rectangle.angle = angle;
    XrgPaint2DColor(pInfo->paint, color);
    XrgPaint2DDrawRect(pInfo->paint, &rectangle);
}

extern float XrgRand(float lower, float upper);
extern float XrgLengthVector(RgVector vector);
extern void XrgSubVector(RgVector result, RgVector a, RgVector b);
extern void __RgGeomPointGetPos(void *point, RgVector destination,
                                const char *source_file, int line);
extern unsigned int RgRobotGetStatusFlags(void *robot);
extern void *RgRobotGetGeom(void *robot);
extern int RgRobotIsConfused(void *robot);
extern void *RgRobotGetWeapon(void *robot, unsigned int slot);
extern int RgWeaponIsTargetting(void *weapon);
extern float RgWeaponGetShotNum(void *weapon);
extern int RgWeaponIsNotBusy(void *weapon);
extern void XrgPaint2DFlush(XrgPaint2D *paint);
extern void XrgPaint2DOffset2D(XrgPaint2D *paint, int x, int y);
extern void XrgPaint2DOffset3DForce(XrgPaint2D *paint, const RgVector anchor,
                                    float force);
extern void XrgPaint2DOffsetResult(XrgPaint2D *paint, const void *anchor);
extern float atan2f(float y, float x);
extern float sinf(float angle);
extern float cosf(float angle);
extern const char D_00A545A8[];
extern const char D_00A545C0[];
extern const char D_00A545C8[];
extern const char D_00A545D0[];
extern const char D_00A545D8[];
extern const char D_00A545E8[];
extern const char D_00A545F0[];
extern const char D_00A545F8[];
extern const char D_00A54600[];

void _DispInfo(RgDispGameInfo *pInfo)
{
    RgVector hostPos;
    RgVector subPos;
    RgVector delta;
    RgVector anchor;
    void *hostRobot;
    void *hostGeom;
    void *subGeom;
    void *weapon;
    RgDispGameOffset *offsetBlock;
    XrgPaint2D *paint;
    unsigned int flags;
    int confused;
    float spread;
    float glow;
    float clamped;
    float distance;
    float alpha;
    float angle;
    float radius;
    float xArm;
    float yArm;
    float cosWeighted;
    float sinWeighted;
    float a4;
    float a5;
    float wrapped5;
    float unit;
    int y;
    int x;
    int sway;
    int intensity;
    unsigned int state;

    paint = pInfo->paint;
    if (pInfo->subRobot != 0) {
        const float pi = 3.1415927f;

        a4 = *(float *)&pInfo->value4 + 0.09817477f;
        offsetBlock = &pInfo->offset;
        while (a4 > pi) {
            a4 -= 6.2831855f;
        }
        if (a4 < -3.1415927f) {
            do {
                a4 += 6.2831855f;
            } while (a4 < -3.1415927f);
        }
        *(float *)&pInfo->value4 = a4;
        wrapped5 = *(float *)&pInfo->value5 + 0.33379424f;
        while (wrapped5 > 3.1415927f) {
            wrapped5 -= 6.2831855f;
        }
        if (wrapped5 < -3.1415927f) {
            do {
                wrapped5 += 6.2831855f;
            } while (wrapped5 < -3.1415927f);
        }
        *(float *)&pInfo->value5 = wrapped5;

        flags = RgRobotGetStatusFlags(pInfo->hostRobot);
        hostRobot = pInfo->hostRobot;
        hostGeom = RgRobotGetGeom(hostRobot);
        subGeom = RgRobotGetGeom(pInfo->subRobot);
        __RgGeomPointGetPos(hostGeom, hostPos, D_00A54570, 201);
        __RgGeomPointGetPos(subGeom, subPos, D_00A54570, 202);
        confused = RgRobotIsConfused(hostRobot);
        XrgSubVector(delta, hostPos, subPos);
        distance = XrgLengthVector(delta);
        clamped = distance < 0.0f ? 0.0f : (50.0f < distance ? 50.0f : distance);
        unit = 1.0f;
        distance = unit - clamped / 50.0f;
        alpha = unit;
        unit = distance;
        spread = (distance - 0.5f) * 1.5707964f;
        glow = 0.75f + distance * 0.6f;

        if (confused != 0) {
            anchor[0] = hostPos[0] + XrgRand(-50.0f, 50.0f);
            anchor[1] = hostPos[1] + XrgRand(-10.0f, 10.0f);
            anchor[2] = hostPos[2] + XrgRand(-50.0f, 50.0f);
            anchor[3] = alpha;
            XrgPaint2DOffset3DForce(paint, anchor, 180.0f);
            pInfo->value2 = alpha;
            pInfo->value3 = 3;
        } else {
            state = pInfo->value3;
            switch (state) {
            case 1:
                pInfo->value2 -= 0.1f;
                if (pInfo->value2 < 0.0f) {
                    pInfo->value2 = 0.0f;
                    pInfo->value3 = 2;
                }
                /* fallthrough */
            case 2:
                if (flags & 1) {
                    pInfo->value3 = 3;
                }
                XrgPaint2DOffset2D(paint, offsetBlock->offsetX,
                                   offsetBlock->offsetY);
                pInfo->value2 = 0.0f;
                break;
            case 3:
                pInfo->value2 += 0.1f;
                if (pInfo->value2 > alpha) {
                    pInfo->value2 = alpha;
                    pInfo->value3 = 0;
                }
                /* fallthrough */
            case 0:
                if ((flags & 1) == 0) {
                    pInfo->value3 = 1;
                }
                XrgPaint2DOffset3DForce(paint, subPos, 180.0f);
                break;
            default:
                RgError(D_00A545A8, D_00A54570, 263, pInfo->value3);
                break;
            }
        }
        XrgPaint2DOffsetResult(pInfo->paint, offsetBlock);
        alpha = pInfo->value2;
        _paint_set_tex_alpha(pInfo, D_00A545C0, 1);
        _paint_xy_color(pInfo, 0, 0, spread, alpha, 0x80);
        _paint_set_tex_alpha(pInfo, D_00A545C8, 1);
        _paint_xy_color(pInfo, 0, 0, 0.0f, alpha, 0x80);
        _paint_set_tex_alpha(pInfo, D_00A545D0, 1);
        _paint_xy_color(pInfo, 0, 0, 0.0f, alpha * glow, 0x80);

        if (pInfo->value3 == 0) {
            if (confused == 0) {
                if (offsetBlock->hasVector != 0) {
                    angle = atan2f(offsetBlock->vectorY, offsetBlock->vectorX);
                    a5 = *(float *)&pInfo->value5;
                    while (a5 > 3.1415927f) {
                        a5 -= 6.2831855f;
                    }
                    if (a5 < -3.1415927f) {
                        do {
                            a5 += 6.2831855f;
                        } while (a5 < -3.1415927f);
                    }
                    radius = 5.0f * sinf(a5);
                    xArm = cosf(angle) * radius;
                    yArm = sinf(angle) * radius;
                    angle += 1.5707964f;
                    cosWeighted = cosf(-angle) * xArm;
                    x = (int)(cosWeighted - sinf(-angle) * yArm);
                    sinWeighted = sinf(-angle) * xArm;
                    y = (int)(sinWeighted + cosf(-angle) * yArm);
                } else {
                    y = 0;
                    x = 0;
                    angle = *(float *)&pInfo->value4;
                }
                _paint_set_tex_alpha(pInfo, D_00A545D8, 1);
                _paint_xy_color(pInfo, x, y, angle, 1.0f, 0x80);
            }
        }

        if (0.3f < pInfo->value2) {
            sway = (int)(sinf(4.0f * *(float *)&pInfo->value4) * 8.0f);
            weapon = RgRobotGetWeapon(hostRobot, 0);
            if (weapon != 0 && RgWeaponIsTargetting(weapon) != 0
                && 0.0f < RgWeaponGetShotNum(weapon)) {
                    intensity = 0x80;
                    if (RgWeaponIsNotBusy(weapon) == 0) {
                        intensity = 0x20;
                    }
                    _paint_set_tex_alpha(pInfo, D_00A545E8, 1);
                    _paint_xy_color(pInfo, sway - 0x30, 0, 0.0f, 1.0f, intensity);
            }
            weapon = RgRobotGetWeapon(hostRobot, 1);
            if (weapon != 0 && RgWeaponIsTargetting(weapon) != 0
                && 0.0f < RgWeaponGetShotNum(weapon)) {
                    intensity = 0x80;
                    if (RgWeaponIsNotBusy(weapon) == 0) {
                        intensity = 0x20;
                    }
                    _paint_set_tex_alpha(pInfo, D_00A545F0, 1);
                    _paint_xy_color(pInfo, 0x30 - sway, 0, 0.0f, 1.0f, intensity);
            }
            weapon = RgRobotGetWeapon(hostRobot, 2);
            if (weapon != 0 && RgWeaponIsTargetting(weapon) != 0
                && 0.0f < RgWeaponGetShotNum(weapon)) {
                sway -= 0x30;
                intensity = 0x80;
                if (RgWeaponIsNotBusy(weapon) == 0) {
                    intensity = 0x20;
                }
                _paint_set_tex_alpha(pInfo, D_00A545F8, 1);
                _paint_xy_color(pInfo, 0, sway, 0.0f, 1.0f, intensity);
                _paint_set_tex_alpha(pInfo, D_00A54600, 1);
                _paint_xy_color(pInfo, 0, -sway, 0.0f, 1.0f, intensity);
            }
        }
    }
    XrgPaint2DFlush(pInfo->paint);
}

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void _InitInfo(RgDispGameInfo *pInfo);

RgDispGameInfo *CreateRgDispGameInfo(void) {
    RgDispGameInfo *pInfo;

    pInfo = RgHeapAlloc(InstanceOfRgHeap(), RG_DISP_GAME_INFO_SIZE,
                        D_00A54570, 360);
    _InitInfo(pInfo);
    return pInfo;
}

extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);

void DisposeRgDispGameInfo(RgDispGameInfo *pInfo) {
    if (pInfo == 0) {
        assert_prog(D_00A54560, D_00A54570, 367);
    }
    _DisposeInfo(pInfo);
    RgHeapFree(InstanceOfRgHeap(), pInfo, D_00A54570, 369);
}

void RgDispGameInfoSetHostRobot(RgDispGameInfo *pInfo, void *hostRobot) {
    if (pInfo == 0) {
        assert_prog(D_00A54560, D_00A54570, 378);
    }
    _SetHostRobot(pInfo, hostRobot);
}

void RgDispGameInfoSetSubRobot(RgDispGameInfo *pInfo, void *subRobot) {
    if (pInfo == 0) {
        assert_prog(D_00A54560, D_00A54570, 385);
    }
    _SetSubRobot(pInfo, subRobot);
}

extern void XrgPaint2DSetDrawID(XrgPaint2D *paint, int drawId);

void RgDispGameInfoSetStudio(RgDispGameInfo *pInfo, int studio) {
    if (pInfo == 0) {
        assert_prog(D_00A54560, D_00A54570, 394);
    }
    pInfo->studio = studio;
    XrgPaint2DSetDrawID(pInfo->paint, studio);
}

void RgDispGameInfoPassTime(RgDispGameInfo *pInfo, float deltaTime) {
    if (pInfo == 0) {
        assert_prog(D_00A54560, D_00A54570, 405);
    }
    _PassTimeInfo(pInfo, deltaTime);
}

extern void _DispInfo(RgDispGameInfo *pInfo);

void RgDispGameInfoDisplay(RgDispGameInfo *pInfo) {
    if (pInfo == 0) {
        assert_prog(D_00A54560, D_00A54570, 411);
    }
    _DispInfo(pInfo);
}
