/*
 * OV12 original TU 74: 0x00a3f230..0x00a40008 (12 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_error.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_title.h"
#include "ov12/xrg_event.h"
#include "ov12/xrg_pad.h"
#include "ov12/xrg_paint2d.h"
#include "ov12/xrg_sound.h"

#define TITLE_BACKGROUND_COUNT 2
#define TITLE_CURSOR_Y 176
#define TITLE_GLOW_AMPLITUDE 64.0f
#define TITLE_GLOW_MIDPOINT 64
#define TITLE_ROTATION_STEP 0.09817477f
#define TITLE_PI 3.1415927f
#define TITLE_TWO_PI 6.2831855f

/* Full brightness of a blend colour component, in the GS 0-0x80 scale. */
#define TITLE_BRIGHTNESS_MAX 128
#define TITLE_BRIGHTNESS_SHIFT 7

/* Values of RgTitle.menu_state. */
enum {
    TITLE_STATE_SELECT,
    TITLE_STATE_DECIDED,
    TITLE_STATE_FADE_OUT,
    TITLE_STATE_DONE
};

/* Values of RgTitle.cursor_id, in menu order. */
enum {
    TITLE_CURSOR_ONE_PLAYER,
    TITLE_CURSOR_VERSUS,
    TITLE_CURSOR_HELP,
    TITLE_CURSOR_EXIT,
    TITLE_CURSOR_COUNT
};

/* Seconds the decided and fade-out states last. */
#define TITLE_DECIDED_TIME 1.0f
#define TITLE_FADE_OUT_TIME 0.5f

/* Slots of RgTitle.bxx, named after the archives loaded. */
enum {
    TITLE_BXX_BASE,
    TITLE_BXX_BASE_AG02,
    TITLE_BXX_BASE_VX01,
    TITLE_BXX_TITLE,
    TITLE_BXX_RED,
    TITLE_BXX_B1H,
    TITLE_BXX_VE,
    TITLE_BXX_CUR_1E,
    TITLE_BXX_CUR_VH
};

/* Slots of RgTitle.pictures, named after the pictures looked up. */
enum {
    TITLE_PIC_BG,
    TITLE_PIC_BG_AG02,
    TITLE_PIC_BG_VX10,
    TITLE_PIC_BLUE_E,
    TITLE_PIC_RED_ECPT,
    TITLE_PIC_SEL_BTN,
    TITLE_PIC_SEL_E1P,
    TITLE_PIC_SEL_EHLP,
    TITLE_PIC_SEL_EVS,
    TITLE_PIC_SEL_EEXT,
    TITLE_PIC_RED_1P,
    TITLE_PIC_RED_EXIT,
    TITLE_PIC_RED_HELP,
    TITLE_PIC_RED_VS
};


static void _InitTitle(RgTitle *pTitle, int count);
static void _DestructTitle(RgTitle *pTitle);
static void _disp_blight(float brightness);
static void _disp_add_blight(XrgPaint2D *paint, RgBxxPic *picture, int x, int y,
                             int brightness);

const char titleAssertPaintNotNil[] = "pPaint != NIL";
const char titleSourcePath[] = "../rg_title.euc.c";
const char titleAssertPicNotNil[] = "pPic != NIL";
const char titleAssertTitleNotNil[] = "pTitle != NIL";
const char titleBaseBxxFileName[] = "top_base.bxx";
const char titleBaseAg02BxxFileName[] = "top_base_ag02.bxx";
const char titleBaseVx01BxxFileName[] = "top_base_vx01.bxx";
const char titleTitleBxxFileName[] = "top_title.bxx";
const char titleRedBxxFileName[] = "top_red.bxx";
const char titleB1hBxxFileName[] = "top_b1h.bxx";
const char titleVeBxxFileName[] = "top_ve.bxx";
const char titleCursor1eBxxFileName[] = "top_cur_1e.bxx";
const char titleCursorVhBxxFileName[] = "top_cur_vh.bxx";
const char titleBackgroundPicName[] = "bg.bmp";
const char titleBackgroundAg02PicName[] = "bg_ag02.bmp";
const char titleBackgroundVx10PicName[] = "bg_vx10.bmp";
const char titleBlueEnglishPicName[] = "blue_e.bmp";
const char titleRedEcptPicName[] = "red_ecpt.bmp";
const char titleRed1pPicName[] = "red_1p.bmp";
const char titleRedExitPicName[] = "red_exit.bmp";
const char titleRedHelpPicName[] = "red_help.bmp";
const char titleRedVsPicName[] = "red_vs.bmp";
const char titleSelectButtonPicName[] = "sel_btn.bmp";
const char titleSelectEnglish1pPicName[] = "sel_e1p.bmp";
const char titleSelectEnglishHelpPicName[] = "sel_ehlp.bmp";
const char titleSelectEnglishVsPicName[] = "sel_evs.bmp";
const char titleSelectEnglishExitPicName[] = "sel_eext.bmp";
const char titleCannotFindPicFormat[] = "cannot find %d pic";

/* The blend colour: full-range red, green and blue in the GS 0-0x80 scale,
 * alpha one unit short of it. */
static XrgPaint2DRGBA s_aCol = {128, 128, 128, 127};

static void _disp_blight(float brightness)
{
    float clamped_brightness;
    int component;

    if (brightness < 0.0f) {
        clamped_brightness = 0.0f;
    } else {
        clamped_brightness = 1.0f;
        if (!(brightness > 1.0f)) {
            clamped_brightness = brightness;
        }
    }

    component = (int) (clamped_brightness * 128.0f);
    s_aCol.b = component;
    s_aCol.g = component;
    s_aCol.r = component;
}

static void _disp_normal(XrgPaint2D *pPaint, RgBxxPic *pPic, int x, int y)
{
    if (pPaint == 0) {
        assert_prog(titleAssertPaintNotNil, titleSourcePath, 101);
    }
    if (pPic == 0) {
        assert_prog(titleAssertPicNotNil, titleSourcePath, 102);
    }
    XrgPaint2DUseTexture(pPaint, pPic);
    XrgPaint2DColor(pPaint, &s_aCol);
    XrgPaint2DDrawXYWH(pPaint, XRG_PAINT2D_MODE_PIC_SIZE, x, y, 0, 0);
}

static void _disp_linear(XrgPaint2D *pPaint, RgBxxPic *pPic, int x, int y)
{
    if (pPaint == 0) {
        assert_prog(titleAssertPaintNotNil, titleSourcePath, 110);
    }
    if (pPic == 0) {
        assert_prog(titleAssertPicNotNil, titleSourcePath, 111);
    }
    XrgPaint2DUseTexture(pPaint, pPic);
    XrgPaint2DColor(pPaint, &s_aCol);
    XrgPaint2DAlpha(pPaint, XRG_PAINT2D_BLEND_LINEAR);
    XrgPaint2DDrawXYWH(pPaint, XRG_PAINT2D_MODE_PIC_SIZE, x, y, 0, 0);
}

static void _disp_add(XrgPaint2D *pPaint, RgBxxPic *pPic, int x, int y)
{
    if (pPaint == 0) {
        assert_prog(titleAssertPaintNotNil, titleSourcePath, 119);
    }
    if (pPic == 0) {
        assert_prog(titleAssertPicNotNil, titleSourcePath, 120);
    }
    XrgPaint2DUseTexture(pPaint, pPic);
    XrgPaint2DColor(pPaint, &s_aCol);
    XrgPaint2DAlpha(pPaint, XRG_PAINT2D_BLEND_ADD);
    XrgPaint2DDrawXYWH(pPaint, XRG_PAINT2D_MODE_PIC_SIZE, x, y, 0, 0);
}

static void _disp_add_blight(XrgPaint2D *pPaint, RgBxxPic *pPic, int x, int y,
                             int brightness)
{
    XrgPaint2DRGBA color;
    int maximum_brightness;

    if (pPaint == 0) {
        assert_prog(titleAssertPaintNotNil, titleSourcePath, 129);
    }
    if (pPic == 0) {
        assert_prog(titleAssertPicNotNil, titleSourcePath, 130);
    }

    maximum_brightness = TITLE_BRIGHTNESS_MAX;
    if (brightness < 0) {
        brightness = 0;
    } else {
        if (maximum_brightness < brightness) {
            brightness = maximum_brightness;
        }
    }

    color.r = (s_aCol.r * brightness) >> TITLE_BRIGHTNESS_SHIFT;
    color.g = (s_aCol.g * brightness) >> TITLE_BRIGHTNESS_SHIFT;
    color.b = (s_aCol.b * brightness) >> TITLE_BRIGHTNESS_SHIFT;
    color.a = (s_aCol.a * brightness) >> TITLE_BRIGHTNESS_SHIFT;

    XrgPaint2DUseTexture(pPaint, pPic);
    XrgPaint2DColor(pPaint, &color);
    XrgPaint2DAlpha(pPaint, XRG_PAINT2D_BLEND_ADD);
    XrgPaint2DDrawXYWH(pPaint, XRG_PAINT2D_MODE_PIC_SIZE, x, y, 0, 0);
}

static void _InitTitle(RgTitle *pTitle, int count)
{
    unsigned int picture_index;

    if (pTitle == 0) {
        assert_prog(titleAssertTitleNotNil, titleSourcePath, 147);
    }
    pTitle->count = count;
    pTitle->cursor_id = 0;
    pTitle->menu_state = 0;
    pTitle->paint = CreateXrgPaint2D_sub(titleSourcePath, 153);
    pTitle->bxx[TITLE_BXX_BASE] = LoadRgBxx_sub(titleBaseBxxFileName, titleSourcePath, 155);
    pTitle->bxx[TITLE_BXX_BASE_AG02] = LoadRgBxx_sub(titleBaseAg02BxxFileName, titleSourcePath, 156);
    pTitle->bxx[TITLE_BXX_BASE_VX01] = LoadRgBxx_sub(titleBaseVx01BxxFileName, titleSourcePath, 157);
    pTitle->bxx[TITLE_BXX_TITLE] = LoadRgBxx_sub(titleTitleBxxFileName, titleSourcePath, 158);
    pTitle->bxx[TITLE_BXX_RED] = LoadRgBxx_sub(titleRedBxxFileName, titleSourcePath, 159);
    pTitle->bxx[TITLE_BXX_B1H] = LoadRgBxx_sub(titleB1hBxxFileName, titleSourcePath, 160);
    pTitle->bxx[TITLE_BXX_VE] = LoadRgBxx_sub(titleVeBxxFileName, titleSourcePath, 161);
    pTitle->bxx[TITLE_BXX_CUR_1E] = LoadRgBxx_sub(titleCursor1eBxxFileName, titleSourcePath, 162);
    pTitle->bxx[TITLE_BXX_CUR_VH] = LoadRgBxx_sub(titleCursorVhBxxFileName, titleSourcePath, 163);

    pTitle->pictures[TITLE_PIC_BG] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_BASE], titleBackgroundPicName);
    pTitle->pictures[TITLE_PIC_BG_AG02] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_BASE_AG02], titleBackgroundAg02PicName);
    pTitle->pictures[TITLE_PIC_BG_VX10] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_BASE_VX01], titleBackgroundVx10PicName);
    pTitle->pictures[TITLE_PIC_BLUE_E] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_TITLE], titleBlueEnglishPicName);
    pTitle->pictures[TITLE_PIC_RED_ECPT] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_RED], titleRedEcptPicName);
    pTitle->pictures[TITLE_PIC_RED_1P] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_CUR_1E], titleRed1pPicName);
    pTitle->pictures[TITLE_PIC_RED_EXIT] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_CUR_1E], titleRedExitPicName);
    pTitle->pictures[TITLE_PIC_RED_HELP] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_CUR_VH], titleRedHelpPicName);
    pTitle->pictures[TITLE_PIC_RED_VS] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_CUR_VH], titleRedVsPicName);
    pTitle->pictures[TITLE_PIC_SEL_BTN] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_B1H], titleSelectButtonPicName);
    pTitle->pictures[TITLE_PIC_SEL_E1P] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_B1H], titleSelectEnglish1pPicName);
    pTitle->pictures[TITLE_PIC_SEL_EHLP] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_B1H], titleSelectEnglishHelpPicName);
    pTitle->pictures[TITLE_PIC_SEL_EVS] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_VE], titleSelectEnglishVsPicName);
    pTitle->pictures[TITLE_PIC_SEL_EEXT] = RgBxxGetPic(pTitle->bxx[TITLE_BXX_VE], titleSelectEnglishExitPicName);

    for (picture_index = 0; picture_index < RG_TITLE_PICTURE_COUNT; picture_index++) {
        if (pTitle->pictures[picture_index] == 0) {
            RgError(titleCannotFindPicFormat, titleSourcePath, 213, picture_index);
        }
    }
}

static void _DestructTitle(RgTitle *pTitle)
{
    if (pTitle == 0) {
        assert_prog(titleAssertTitleNotNil, titleSourcePath, 218);
    }
    DisposeXrgPaint2D_sub(pTitle->paint, titleSourcePath, 219);
    DisposeRgBxx_sub(pTitle->bxx[TITLE_BXX_BASE], titleSourcePath, 221);
    DisposeRgBxx_sub(pTitle->bxx[TITLE_BXX_BASE_AG02], titleSourcePath, 222);
    DisposeRgBxx_sub(pTitle->bxx[TITLE_BXX_BASE_VX01], titleSourcePath, 223);
    DisposeRgBxx_sub(pTitle->bxx[TITLE_BXX_TITLE], titleSourcePath, 224);
    DisposeRgBxx_sub(pTitle->bxx[TITLE_BXX_RED], titleSourcePath, 225);
    DisposeRgBxx_sub(pTitle->bxx[TITLE_BXX_B1H], titleSourcePath, 226);
    DisposeRgBxx_sub(pTitle->bxx[TITLE_BXX_VE], titleSourcePath, 227);
    DisposeRgBxx_sub(pTitle->bxx[TITLE_BXX_CUR_1E], titleSourcePath, 228);
    DisposeRgBxx_sub(pTitle->bxx[TITLE_BXX_CUR_VH], titleSourcePath, 229);
}

RgTitle *CreateRgTitle(int count)
{
    RgTitle *pTitle;

    pTitle = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgTitle), titleSourcePath, 236);
    _InitTitle(pTitle, count);
    return pTitle;
}

void DisposeRgTitle(RgTitle *pTitle)
{
    if (pTitle == 0) {
        assert_prog(titleAssertTitleNotNil, titleSourcePath, 244);
    }
    _DestructTitle(pTitle);
    RgHeapFree(InstanceOfRgHeap(), pTitle, titleSourcePath, 246);
}

const char titleUnknownCursorIdFormat[] = "unknown title cursor id %d";
static float s_fRot_0 = 0.0f;
static int s_nID_1 = 0;

int RgTitleGetResult(RgTitle *pTitle)
{
    int cursor_id;

    if (pTitle == 0) {
        assert_prog(titleAssertTitleNotNil, titleSourcePath, 256);
    }

    if (pTitle->menu_state == TITLE_STATE_DONE) {
        cursor_id = pTitle->cursor_id;
        switch ((unsigned int)cursor_id) {
        case TITLE_CURSOR_VERSUS:
            return RG_TITLE_RESULT_VERSUS;
        case TITLE_CURSOR_ONE_PLAYER:
            if (!XrgPadIsSelectLevelLR() || XrgEventGetLevel() < 2) {
                return RG_TITLE_RESULT_ONE_PLAYER;
            }
            return RG_TITLE_RESULT_ONE_PLAYER_LEVEL_SELECT;
        case TITLE_CURSOR_HELP:
            return RG_TITLE_RESULT_HELP;
        case TITLE_CURSOR_EXIT:
            return RG_TITLE_RESULT_EXIT;
        default:
            RgError(titleUnknownCursorIdFormat, titleSourcePath, 274, cursor_id);
            break;
        }
    }
    return RG_TITLE_RESULT_NONE;
}

void RgTitlePassTime(RgTitle *pTitle, float delta)
{
    if (pTitle == 0) {
        assert_prog(titleAssertTitleNotNil, titleSourcePath, 287);
    }

    XrgPadSetID(0);

    if (pTitle->menu_state != TITLE_STATE_DECIDED) {
        if (pTitle->menu_state != TITLE_STATE_SELECT) {
            if (pTitle->menu_state != TITLE_STATE_FADE_OUT) {
                return;
            }
        } else {
            if (XrgPadIsUp() != 0 && pTitle->cursor_id != TITLE_CURSOR_ONE_PLAYER) {
                pTitle->cursor_id--;
                XrgSoundSystemCursor();
            }
            if (XrgPadIsDown() != 0 && (unsigned int) pTitle->cursor_id < TITLE_CURSOR_EXIT) {
                pTitle->cursor_id++;
                XrgSoundSystemCursor();
            }

            if (XrgPadIsMaru() != 0 || XrgPadIsStart() != 0) {
                pTitle->menu_state = TITLE_STATE_DECIDED;
                pTitle->transition_time = TITLE_DECIDED_TIME;
                if (pTitle->cursor_id == TITLE_CURSOR_EXIT) {
                    XrgSoundSystemCancel();
                } else {
                    XrgSoundSystemOk();
                }
            }
            return;
        }
    } else {
        pTitle->transition_time -= delta;
        if (pTitle->transition_time < 0.0f) {
            pTitle->menu_state = TITLE_STATE_FADE_OUT;
            pTitle->transition_time = TITLE_FADE_OUT_TIME;
        }
        return;
    }

    pTitle->transition_time -= delta;
    if (pTitle->transition_time < 0.0f) {
        pTitle->menu_state = TITLE_STATE_DONE;
    }
}

void RgTitleDisp(RgTitle *pTitle)
{
    float rotation;
    int glow;
    int highlight = 1;

    if (pTitle == 0) {
        assert_prog(titleAssertTitleNotNil, titleSourcePath, 333);
    }
    if (pTitle->menu_state == TITLE_STATE_DONE) {
        return;
    }
    if (pTitle->menu_state == TITLE_STATE_FADE_OUT) {
        _disp_blight(pTitle->transition_time * 2.0f);
    } else {
        _disp_blight(1.0f);
    }

    switch (pTitle->count % TITLE_BACKGROUND_COUNT) {
    case 0:
        _disp_normal(pTitle->paint, pTitle->pictures[TITLE_PIC_BG_VX10], 0, 0);
        break;
    case 1:
        _disp_normal(pTitle->paint, pTitle->pictures[TITLE_PIC_BG_AG02], 0, 0);
        break;
    }

    glow = (int)(sinf(s_fRot_0) * TITLE_GLOW_AMPLITUDE) + TITLE_GLOW_MIDPOINT;
    rotation = s_fRot_0 + TITLE_ROTATION_STEP;
    while (rotation > TITLE_PI) {
        rotation -= TITLE_TWO_PI;
    }
    if (rotation < -TITLE_PI) {
        do {
            rotation += TITLE_TWO_PI;
        } while (rotation < -TITLE_PI);
    }
    s_fRot_0 = rotation;
    _disp_add_blight(pTitle->paint, pTitle->pictures[TITLE_PIC_RED_ECPT], 0, 0, glow);
    _disp_add(pTitle->paint, pTitle->pictures[TITLE_PIC_BLUE_E], 0, 0);

    if ((unsigned int)(pTitle->menu_state - TITLE_STATE_DECIDED) < 2) {
        s_nID_1++;
        highlight = s_nID_1 & 1;
    }

    switch ((unsigned int)pTitle->cursor_id) {
    case TITLE_CURSOR_ONE_PLAYER:
        _disp_linear(pTitle->paint, pTitle->pictures[TITLE_PIC_SEL_E1P], 0, TITLE_CURSOR_Y);
        if (highlight) {
            _disp_add(pTitle->paint, pTitle->pictures[TITLE_PIC_RED_1P], 0, TITLE_CURSOR_Y);
        }
        break;
    case TITLE_CURSOR_VERSUS:
        _disp_linear(pTitle->paint, pTitle->pictures[TITLE_PIC_SEL_EVS], 0, TITLE_CURSOR_Y);
        if (highlight) {
            _disp_add(pTitle->paint, pTitle->pictures[TITLE_PIC_RED_VS], 0, TITLE_CURSOR_Y);
        }
        break;
    case TITLE_CURSOR_HELP:
        _disp_linear(pTitle->paint, pTitle->pictures[TITLE_PIC_SEL_EHLP], 0, TITLE_CURSOR_Y);
        if (highlight) {
            _disp_add(pTitle->paint, pTitle->pictures[TITLE_PIC_RED_HELP], 0, TITLE_CURSOR_Y);
        }
        break;
    case TITLE_CURSOR_EXIT:
        _disp_linear(pTitle->paint, pTitle->pictures[TITLE_PIC_SEL_EEXT], 0, TITLE_CURSOR_Y);
        if (highlight) {
            _disp_add(pTitle->paint, pTitle->pictures[TITLE_PIC_RED_EXIT], 0, TITLE_CURSOR_Y);
        }
        break;
    default:
        RgError(titleUnknownCursorIdFormat, titleSourcePath, 403, pTitle->cursor_id);
        break;
    }
    XrgPaint2DFlush(pTitle->paint);
}
