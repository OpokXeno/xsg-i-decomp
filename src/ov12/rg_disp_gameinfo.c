/*
 * OV12 original TU 38: 0x00a22a78..0x00a239d0 (17 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_disp_gameinfo.h"
#include "ov12/rg_geom_point.h"
#include "ov12/xrg_rand_int.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_error.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_battle_common_data.h"
#include "ov12/rg_robot.h"
#include "ov12/rg_weapon.h"
#include "ov12/rg_bxx.h"

/* RgDispGameInfo.state: how the marker follows the sub robot. */
#define RG_DISP_GAME_INFO_TRACKING 0 /* shown, following the target in 3D */
#define RG_DISP_GAME_INFO_FADE_OUT 1 /* fading out */
#define RG_DISP_GAME_INFO_HIDDEN 2   /* hidden, parked at the 2D offset */
#define RG_DISP_GAME_INFO_FADE_IN 3  /* fading in (also while confused) */

#define RG_DISP_GAME_INFO_FADE_STEP 0.1f
/* Radius in pixels the 3D marker is clamped to around the screen center. */
#define RG_DISP_GAME_INFO_CLAMP_RADIUS 180.0f
/* Distance in world units over which the marker grows to full size. */
#define RG_DISP_GAME_INFO_FAR_DISTANCE 50.0f
/* Weapon indicator brightness, ready or busy. */
#define RG_DISP_GAME_INFO_BRIGHT 0x80
#define RG_DISP_GAME_INFO_DIM 0x20
#define RG_DISP_GAME_INFO_ALPHA_MAX 127
#define RG_DISP_GAME_INFO_WEAPON_SLOTS 3

/*
 * Anchored, rotated, scaled rectangle with per-frame jitter: the anchors in
 * the low four bits are both "half the extent".
 */
#define RG_DISP_GAME_INFO_COLOR_RECT_MODE                                  \
    (XRG_PAINT2D_MODE_JITTER | XRG_PAINT2D_MODE_ANGLE |                     \
     XRG_PAINT2D_MODE_SCALE | XRG_PAINT2D_MODE_PIC_SIZE | 5)


/* Scaffold-owned strings: marker picture names and the state error. */
extern const char dispGameInfoUnknownLockOnStateFormat[];
extern const char dispGameInfoPicture001Name[];
extern const char dispGameInfoPicture002Name[];
extern const char dispGameInfoPicture003Name[];
extern const char dispGameInfoPolygonPictureName[];
extern const char dispGameInfoPicture011Name[];
extern const char dispGameInfoPicture012Name[];
extern const char dispGameInfoPicture013Name[];
extern const char dispGameInfoPicture014Name[];
/* Defined here: dispGameInfoPictureNotFoundFormat ("not exist '%s' picture"), before _paint_one_texture_alpha. */
extern const char dispGameInfoPictureNotFoundFormat[];

const char dispGameInfoAssertInfoNotNil[16] = "pInfo != NIL";
const char dispGameInfoSourcePath[32] = "../rg_disp_gameinfo.euc.c";

void _InitInfo(RgDispGameInfo *pInfo)
{
    RgBxx *dispTex;
    XrgPaint2D *paint;

    if (pInfo == 0) {
        assert_prog(dispGameInfoAssertInfoNotNil, dispGameInfoSourcePath, 73);
    }
    pInfo->hostRobot = 0;
    pInfo->subRobot = 0;
    dispTex = RgBattleCommonDataGetDispTex(InstanceOfRgBattleCommonData());
    pInfo->value1 = 1.0f;
    pInfo->alpha = 1.0f;
    pInfo->state = RG_DISP_GAME_INFO_HIDDEN;
    pInfo->studio = -2;
    pInfo->dispTex = dispTex;
    paint = CreateXrgPaint2D_sub(dispGameInfoSourcePath, 90);
    pInfo->paint = paint;
    XrgPaint2DSetDrawPrio(paint, 4);
    XrgPaint2DSetDrawID(pInfo->paint, pInfo->studio);
    pInfo->swayPhase = 0;
    pInfo->pulsePhase = 0;
}

void _DisposeInfo(RgDispGameInfo *pInfo) {
    if (pInfo == 0) {
        assert_prog(dispGameInfoAssertInfoNotNil, dispGameInfoSourcePath, 100);
    }
    DisposeXrgPaint2D_sub(pInfo->paint, dispGameInfoSourcePath, 101);
}

void _SetHostRobot(RgDispGameInfo *pInfo, RgStatus *hostRobot) {
    if (pInfo == 0) {
        assert_prog(dispGameInfoAssertInfoNotNil, dispGameInfoSourcePath, 108);
    }
    pInfo->hostRobot = hostRobot;
}

void _SetSubRobot(RgDispGameInfo *pInfo, RgStatus *subRobot) {
    if (pInfo == 0) {
        assert_prog(dispGameInfoAssertInfoNotNil, dispGameInfoSourcePath, 114);
    }
    pInfo->subRobot = subRobot;
}

void _PassTimeInfo(RgDispGameInfo *pInfo, float deltaTime) {
    if (pInfo == 0) {
        assert_prog(dispGameInfoAssertInfoNotNil, dispGameInfoSourcePath, 122);
    }
}

const char dispGameInfoPictureNotFoundFormat[24] = "not exist '%s' picture";

void _paint_one_texture_alpha(RgDispGameInfo *pInfo, const char *pictureName,
                              int x, int y, int blendMode) {
    RgBxxPic *pic;

    pic = RgBxxGetPic(pInfo->dispTex, pictureName);
    if (pic == 0) {
        RgError(dispGameInfoPictureNotFoundFormat, dispGameInfoSourcePath, 133, pictureName);
    }
    XrgPaint2DUseTexture(pInfo->paint, pic);
    XrgPaint2DAlpha(pInfo->paint, blendMode);
    XrgPaint2DDrawXYWH(pInfo->paint, XRG_PAINT2D_MODE_PIC_SIZE, x, y,
                       0, 0);
}

void _paint_one_texture(RgDispGameInfo *pInfo, const char *pictureName, int x,
                        int y) {
    _paint_one_texture_alpha(pInfo, pictureName, x, y, 0);
}

void _paint_set_tex_alpha(RgDispGameInfo *pInfo, const char *pictureName,
                          int blendMode) {
    RgBxxPic *pic;

    pic = RgBxxGetPic(pInfo->dispTex, pictureName);
    if (pic == 0) {
        RgError(dispGameInfoPictureNotFoundFormat, dispGameInfoSourcePath, 150, pictureName);
    }
    XrgPaint2DUseTexture(pInfo->paint, pic);
    XrgPaint2DAlpha(pInfo->paint, blendMode);
}

void _paint_xy_color(RgDispGameInfo *pInfo, int x, int y, float angle, float scale,
                     int intensity)
{
    XrgPaint2DRGBA color = {intensity, intensity, intensity,
                            RG_DISP_GAME_INFO_ALPHA_MAX};
    XrgPaint2DRect rectangle;

    InitXrgPaint2DRect(&rectangle, RG_DISP_GAME_INFO_COLOR_RECT_MODE);
    rectangle.x = x;
    rectangle.y = y;
    rectangle.scale = scale;
    rectangle.angle = angle;
    XrgPaint2DColor(pInfo->paint, &color);
    XrgPaint2DDrawRect(pInfo->paint, &rectangle);
}

void _DispInfo(RgDispGameInfo *pInfo)
{
    RgVector hostPos;
    RgVector subPos;
    RgVector delta;
    RgVector anchor;
    RgStatus *hostRobot;
    RgGeomPoint *hostGeom;
    RgGeomPoint *subGeom;
    RgWeapon *weapon;
    XrgPaint2DOffsetOverride *offsetBlock;
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
        const float pi = XRG_PI;

        a4 = pInfo->swayPhase + 0.09817477f;
        offsetBlock = &pInfo->offset;
        while (a4 > pi) {
            a4 -= XRG_TWO_PI;
        }
        if (a4 < -XRG_PI) {
            do {
                a4 += XRG_TWO_PI;
            } while (a4 < -XRG_PI);
        }
        pInfo->swayPhase = a4;
        wrapped5 = pInfo->pulsePhase + 0.33379424f;
        while (wrapped5 > XRG_PI) {
            wrapped5 -= XRG_TWO_PI;
        }
        if (wrapped5 < -XRG_PI) {
            do {
                wrapped5 += XRG_TWO_PI;
            } while (wrapped5 < -XRG_PI);
        }
        pInfo->pulsePhase = wrapped5;

        flags = RgRobotGetStatusFlags(pInfo->hostRobot);
        hostRobot = pInfo->hostRobot;
        hostGeom = RgRobotGetGeom(hostRobot);
        subGeom = RgRobotGetGeom(pInfo->subRobot);
        __RgGeomPointGetPos(hostGeom, hostPos, dispGameInfoSourcePath, 201);
        __RgGeomPointGetPos(subGeom, subPos, dispGameInfoSourcePath, 202);
        confused = RgRobotIsConfused(hostRobot);
        XrgSubVector(delta, hostPos, subPos);
        distance = XrgLengthVector(delta);
        clamped = distance < 0.0f ? 0.0f : (RG_DISP_GAME_INFO_FAR_DISTANCE < distance ? RG_DISP_GAME_INFO_FAR_DISTANCE : distance);
        unit = 1.0f;
        distance = unit - clamped / RG_DISP_GAME_INFO_FAR_DISTANCE;
        alpha = unit;
        unit = distance;
        spread = (distance - 0.5f) * XRG_HALF_PI;
        glow = 0.75f + distance * 0.6f;

        if (confused != 0) {
            anchor[0] = hostPos[0] + XrgRand(-50.0f, 50.0f);
            anchor[1] = hostPos[1] + XrgRand(-10.0f, 10.0f);
            anchor[2] = hostPos[2] + XrgRand(-50.0f, 50.0f);
            anchor[3] = alpha;
            XrgPaint2DOffset3DForce(paint, anchor, RG_DISP_GAME_INFO_CLAMP_RADIUS);
            pInfo->alpha = alpha;
            pInfo->state = RG_DISP_GAME_INFO_FADE_IN;
        } else {
            state = pInfo->state;
            switch (state) {
            case RG_DISP_GAME_INFO_FADE_OUT:
                pInfo->alpha -= RG_DISP_GAME_INFO_FADE_STEP;
                if (pInfo->alpha < 0.0f) {
                    pInfo->alpha = 0.0f;
                    pInfo->state = RG_DISP_GAME_INFO_HIDDEN;
                }
                /* fallthrough */
            case RG_DISP_GAME_INFO_HIDDEN:
                if (flags & 1) {
                    pInfo->state = RG_DISP_GAME_INFO_FADE_IN;
                }
                XrgPaint2DOffset2D(paint, offsetBlock->cx, offsetBlock->cy);
                pInfo->alpha = 0.0f;
                break;
            case RG_DISP_GAME_INFO_FADE_IN:
                pInfo->alpha += RG_DISP_GAME_INFO_FADE_STEP;
                if (pInfo->alpha > alpha) {
                    pInfo->alpha = alpha;
                    pInfo->state = RG_DISP_GAME_INFO_TRACKING;
                }
                /* fallthrough */
            case RG_DISP_GAME_INFO_TRACKING:
                if ((flags & 1) == 0) {
                    pInfo->state = RG_DISP_GAME_INFO_FADE_OUT;
                }
                XrgPaint2DOffset3DForce(paint, subPos, RG_DISP_GAME_INFO_CLAMP_RADIUS);
                break;
            default:
                RgError(dispGameInfoUnknownLockOnStateFormat, dispGameInfoSourcePath, 263, pInfo->state);
                break;
            }
        }
        XrgPaint2DOffsetResult(pInfo->paint, offsetBlock);
        alpha = pInfo->alpha;
        _paint_set_tex_alpha(pInfo, dispGameInfoPicture001Name, XRG_PAINT2D_BLEND_ADD);
        _paint_xy_color(pInfo, 0, 0, spread, alpha, RG_DISP_GAME_INFO_BRIGHT);
        _paint_set_tex_alpha(pInfo, dispGameInfoPicture002Name, XRG_PAINT2D_BLEND_ADD);
        _paint_xy_color(pInfo, 0, 0, 0.0f, alpha, RG_DISP_GAME_INFO_BRIGHT);
        _paint_set_tex_alpha(pInfo, dispGameInfoPicture003Name, XRG_PAINT2D_BLEND_ADD);
        _paint_xy_color(pInfo, 0, 0, 0.0f, alpha * glow, RG_DISP_GAME_INFO_BRIGHT);

        if (pInfo->state == RG_DISP_GAME_INFO_TRACKING) {
            if (confused == 0) {
                if (offsetBlock->flag != 0) {
                    angle = atan2f(offsetBlock->direction[1], offsetBlock->direction[0]);
                    a5 = pInfo->pulsePhase;
                    while (a5 > XRG_PI) {
                        a5 -= XRG_TWO_PI;
                    }
                    if (a5 < -XRG_PI) {
                        do {
                            a5 += XRG_TWO_PI;
                        } while (a5 < -XRG_PI);
                    }
                    radius = 5.0f * sinf(a5);
                    xArm = cosf(angle) * radius;
                    yArm = sinf(angle) * radius;
                    angle += XRG_HALF_PI;
                    cosWeighted = cosf(-angle) * xArm;
                    x = (int)(cosWeighted - sinf(-angle) * yArm);
                    sinWeighted = sinf(-angle) * xArm;
                    y = (int)(sinWeighted + cosf(-angle) * yArm);
                } else {
                    y = 0;
                    x = 0;
                    angle = pInfo->swayPhase;
                }
                _paint_set_tex_alpha(pInfo, dispGameInfoPolygonPictureName, XRG_PAINT2D_BLEND_ADD);
                _paint_xy_color(pInfo, x, y, angle, 1.0f, RG_DISP_GAME_INFO_BRIGHT);
            }
        }

        if (0.3f < pInfo->alpha) {
            sway = (int)(sinf(4.0f * pInfo->swayPhase) * 8.0f);
            weapon = RgRobotGetWeapon(hostRobot, 0);
            if (weapon != 0 && RgWeaponIsTargetting(weapon) != 0
                && 0.0f < RgWeaponGetShotNum(weapon)) {
                    intensity = RG_DISP_GAME_INFO_BRIGHT;
                    if (RgWeaponIsNotBusy(weapon) == 0) {
                        intensity = RG_DISP_GAME_INFO_DIM;
                    }
                    _paint_set_tex_alpha(pInfo, dispGameInfoPicture011Name, XRG_PAINT2D_BLEND_ADD);
                    _paint_xy_color(pInfo, sway - 0x30, 0, 0.0f, 1.0f, intensity);
            }
            weapon = RgRobotGetWeapon(hostRobot, 1);
            if (weapon != 0 && RgWeaponIsTargetting(weapon) != 0
                && 0.0f < RgWeaponGetShotNum(weapon)) {
                    intensity = RG_DISP_GAME_INFO_BRIGHT;
                    if (RgWeaponIsNotBusy(weapon) == 0) {
                        intensity = RG_DISP_GAME_INFO_DIM;
                    }
                    _paint_set_tex_alpha(pInfo, dispGameInfoPicture012Name, XRG_PAINT2D_BLEND_ADD);
                    _paint_xy_color(pInfo, 0x30 - sway, 0, 0.0f, 1.0f, intensity);
            }
            weapon = RgRobotGetWeapon(hostRobot, 2);
            if (weapon != 0 && RgWeaponIsTargetting(weapon) != 0
                && 0.0f < RgWeaponGetShotNum(weapon)) {
                sway -= 0x30;
                intensity = RG_DISP_GAME_INFO_BRIGHT;
                if (RgWeaponIsNotBusy(weapon) == 0) {
                    intensity = RG_DISP_GAME_INFO_DIM;
                }
                _paint_set_tex_alpha(pInfo, dispGameInfoPicture013Name, XRG_PAINT2D_BLEND_ADD);
                _paint_xy_color(pInfo, 0, sway, 0.0f, 1.0f, intensity);
                _paint_set_tex_alpha(pInfo, dispGameInfoPicture014Name, XRG_PAINT2D_BLEND_ADD);
                _paint_xy_color(pInfo, 0, -sway, 0.0f, 1.0f, intensity);
            }
        }
    }
    XrgPaint2DFlush(pInfo->paint);
}

RgDispGameInfo *CreateRgDispGameInfo(RgBxx *dispTex) {
    RgDispGameInfo *pInfo;

    pInfo = RgHeapAlloc(InstanceOfRgHeap(), RG_DISP_GAME_INFO_SIZE,
                        dispGameInfoSourcePath, 360);
    _InitInfo(pInfo);
    return pInfo;
}

void DisposeRgDispGameInfo(RgDispGameInfo *pInfo) {
    if (pInfo == 0) {
        assert_prog(dispGameInfoAssertInfoNotNil, dispGameInfoSourcePath, 367);
    }
    _DisposeInfo(pInfo);
    RgHeapFree(InstanceOfRgHeap(), pInfo, dispGameInfoSourcePath, 369);
}

void RgDispGameInfoSetHostRobot(RgDispGameInfo *pInfo, RgStatus *hostRobot) {
    if (pInfo == 0) {
        assert_prog(dispGameInfoAssertInfoNotNil, dispGameInfoSourcePath, 378);
    }
    _SetHostRobot(pInfo, hostRobot);
}

void RgDispGameInfoSetSubRobot(RgDispGameInfo *pInfo, RgStatus *subRobot) {
    if (pInfo == 0) {
        assert_prog(dispGameInfoAssertInfoNotNil, dispGameInfoSourcePath, 385);
    }
    _SetSubRobot(pInfo, subRobot);
}

void RgDispGameInfoSetStudio(RgDispGameInfo *pInfo, int studio) {
    if (pInfo == 0) {
        assert_prog(dispGameInfoAssertInfoNotNil, dispGameInfoSourcePath, 394);
    }
    pInfo->studio = studio;
    XrgPaint2DSetDrawID(pInfo->paint, studio);
}

void RgDispGameInfoPassTime(RgDispGameInfo *pInfo, float deltaTime) {
    if (pInfo == 0) {
        assert_prog(dispGameInfoAssertInfoNotNil, dispGameInfoSourcePath, 405);
    }
    _PassTimeInfo(pInfo, deltaTime);
}

void RgDispGameInfoDisplay(RgDispGameInfo *pInfo) {
    if (pInfo == 0) {
        assert_prog(dispGameInfoAssertInfoNotNil, dispGameInfoSourcePath, 411);
    }
    _DispInfo(pInfo);
}
