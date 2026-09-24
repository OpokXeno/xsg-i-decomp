/*
 * OV12 original TU 38: 0x00a22a78..0x00a239d0 (17 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_disp_gameinfo.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern const char D_00A54560[];
extern const char D_00A54570[];
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

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void DisposeXrgPaint2D_sub(XrgPaint2D *paint, const char *source_file,
                                  int line);

/*
 * External file-backed witnesses, not candidate-emitted data: this window is
 * asm-owned scaffold data (splat names, no config/symbols/ov12.txt entry).
 *
 * ov12:0x00a54560 contains the assertion expression "pInfo != NIL".
 * ov12:0x00a54570 contains the source filename "../rg_disp_gameinfo.euc.c".
 */
extern const char D_00A54560[];
extern const char D_00A54570[];

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
extern const char D_00A54590[];

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_gameinfo", _paint_xy_color);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_disp_gameinfo", _DispInfo);

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
