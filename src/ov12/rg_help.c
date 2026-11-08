/*
 * OV12 original TU 76: 0x00a420e0..0x00a434b8 (21 functions)
 * The help screen.
 */
#include "common.h"
#include "shared.h"
#include "rg_help.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"
#include "ov12/rg_bxx.h"
#include "ov12/rg_error.h"
#include "ov12/xrg_pad.h"
#include "ov12/xrg_sound.h"

const char helpAssertHelpNotNil[] = "pHelp != NIL";
const char helpSourcePath[] = "../rg_help.euc.c";
const char helpAssertIndexBelowLoadCapacity[] = "i < LOAD_CAPA";
const char helpPicMissingWarningFormat[] = "'%s' pic is not exist in '%s'";
const char helpBxxNotLoadedWarningFormat[] = "'%s' bxx is not loaded";
/* Full brightness of one colour channel; the fixed-point shift is 7 bits. */
#define RG_HELP_BRIGHTNESS_MAX 128
#define RG_HELP_BRIGHTNESS_SHIFT 7

/* Help phases RgHelp::phase steps through. */
#define RG_HELP_PHASE_WAIT 0
#define RG_HELP_PHASE_FADE 1
#define RG_HELP_PHASE_CLOSED 2

#define RG_HELP_FADE_SECONDS 0.5f
#define RG_HELP_BLINK_PERIOD 2.0f
#define RG_HELP_BLINK_HALF_PERIOD 1.0f

/* Screen lines the help pictures are laid on: bottom edge, body, footer. */
#define RG_HELP_SCREEN_BOTTOM 464
#define RG_HELP_BODY_Y 176
#define RG_HELP_FOOTER_Y 360

#define RG_HELP_LOAD_CAPA 20
#define RG_HELP_ARCHIVE_COUNT 19
#define RG_HELP_PIC_COUNT 62
#define RG_HELP_MODE_COUNT 16

static int s_aColor[4] = {RG_HELP_BRIGHTNESS_MAX, RG_HELP_BRIGHTNESS_MAX,
                          RG_HELP_BRIGHTNESS_MAX, RG_HELP_BRIGHTNESS_MAX};

/* Archive and picture name strings, defined outside this file. */
extern const char helpBgPadArchiveName[];
extern const char helpBgGradArchiveName[];
extern const char helpPage0303LowArchiveName[];
extern const char helpPage0303High2ArchiveName[];
extern const char helpPage0303HighArchiveName[];
extern const char helpPage0302ArchiveName[];
extern const char helpPage0301ArchiveName[];
extern const char helpPage0203Mid2ArchiveName[];
extern const char helpPage0203MidArchiveName[];
extern const char helpPage020309ArchiveName[];
extern const char helpPage0203Low2ArchiveName[];
extern const char helpPage0203LowArchiveName[];
extern const char helpPage0203HighArchiveName[];
extern const char helpPage0103MidArchiveName[];
extern const char helpPage0103LowArchiveName[];
extern const char helpPage0103HighArchiveName[];
extern const char helpPage0103High2ArchiveName[];
extern const char helpPage0102ArchiveName[];
extern const char helpPage0101ArchiveName[];
extern const char helpBgControllerPicName[];
extern const char helpBgGradPicName[];
extern const char helpPic030309Name[];
extern const char helpPic030301Name[];
extern const char helpPic030322Name[];
extern const char helpPic030321Name[];
extern const char helpPic030320Name[];
extern const char helpPic030317Name[];
extern const char helpPic030316Name[];
extern const char helpPic030315Name[];
extern const char helpPic030314Name[];
extern const char helpPic030313Name[];
extern const char helpPic030312Name[];
extern const char helpPic030311Name[];
extern const char helpPic030310Name[];
extern const char helpPic030201Name[];
extern const char helpPic030104Name[];
extern const char helpPic030103Name[];
extern const char helpPic030102Name[];
extern const char helpPic030101Name[];
extern const char helpPic020317Name[];
extern const char helpPic020316Name[];
extern const char helpPic020315Name[];
extern const char helpPic020314Name[];
extern const char helpPic020313Name[];
extern const char helpPic020312Name[];
extern const char helpPic020311Name[];
extern const char helpPic020310Name[];
extern const char helpPic020309UnderscoreName[];
extern const char helpPic020308Name[];
extern const char helpPic020307Name[];
extern const char helpPic020306Name[];
extern const char helpPic020305Name[];
extern const char helpPic020304Name[];
extern const char helpPic020303Name[];
extern const char helpPic020302Name[];
extern const char helpPic020301Name[];
extern const char helpPic020322Name[];
extern const char helpPic020321Name[];
extern const char helpPic020320Name[];
extern const char helpPic010309Name[];
extern const char helpPic010308Name[];
extern const char helpPic010307Name[];
extern const char helpPic010306Name[];
extern const char helpPic010305Name[];
extern const char helpPic010304Name[];
extern const char helpPic010303Name[];
extern const char helpPic010302Name[];
extern const char helpPic010301Name[];
extern const char helpPic010322Name[];
extern const char helpPic010321Name[];
extern const char helpPic010320Name[];
extern const char helpPic010319Name[];
extern const char helpPic010318Name[];
extern const char helpPic010316Name[];
extern const char helpPic010314Name[];
extern const char helpPic010312Name[];
extern const char helpPic010311Name[];
extern const char helpPic010202Name[];
extern const char helpPic010201Name[];
extern const char helpPic010102Name[];
extern const char helpPic010101Name[];

/*
 * One row of the 16-entry cursor move table in _control_mode_left, indexed by
 * the current cursor position; the member for whichever pad direction the
 * player pressed gives the cursor value that press moves to.
 */
typedef struct RgHelpMoveEntry {
    int up;
    int down;
    int left;
    int right;
} RgHelpMoveEntry;


/*
 * XrgPaint2DDrawXYWH's mode word: bit 0x20 takes the drawn extent from the
 * bound picture instead of the width/height arguments (_paint).
 */
#define XRG_PAINT2D_MODE_USE_PIC_SIZE 0x20

typedef union RgHelpColor {
    long long halves[2];
    XrgPaint2DRGBA rgba;
} RgHelpColor;

/*
 * The rectangle _paint_lin_uvwh's caller builds: u,v feed XrgPaint2DSetUVOffset
 * and, added to x,y, the screen position this call draws at; w,h feed both
 * XrgPaint2DSetUVSize and the draw call's own width/height.
 */
typedef struct RgHelpUvRect {
    int u;
    int v;
    int w;
    int h;
} RgHelpUvRect;

typedef union RgHelpAlignedUvRect {
    RgHelpUvRect rect;
    long long halves[2];
} RgHelpAlignedUvRect;


static void _InitHelp(RgHelp *pHelp);
static void _DestructHelp(RgHelp *pHelp);
static void _init_mode_left(RgHelp *pHelp);
static void _set_global_blight(float blight);
static void _control_mode_left(RgHelp *pHelp);
static void _color(XrgPaint2D *paintContext, const XrgPaint2DRGBA *overrideColor);
static void _paint(XrgPaint2D *paintContext, RgBxxPic *pic, int blendMode, int x, int y);
static void _paint_b(XrgPaint2D *paintContext, RgBxxPic *pic, int blendMode, int x,
                     int y, int factor);
static void _paint_lin(XrgPaint2D *paintContext, RgBxxPic *pic, int x, int y);
static void _paint_bg_lower(RgHelp *pHelp);
static void _paint_bg_higher(RgHelp *pHelp);
static void _paint_mode_left(RgHelp *pHelp);

/* Sets all four colour channels to blight scaled to 0..RG_HELP_BRIGHTNESS_MAX, clamped at the top. */
static void _set_global_blight(float blight)
{
    int scaled_brightness;
    int brightness;
    int upper_bound;
    int exceeds_upper_bound;

    upper_bound = RG_HELP_BRIGHTNESS_MAX;
    brightness = 0;
    scaled_brightness = (int)(blight * (float)RG_HELP_BRIGHTNESS_MAX);
    if (scaled_brightness >= 0) {
        exceeds_upper_bound = scaled_brightness > upper_bound;
        brightness = exceeds_upper_bound ? upper_bound : scaled_brightness;
    }

    s_aColor[3] = brightness;
    s_aColor[2] = brightness;
    s_aColor[1] = brightness;
    s_aColor[0] = brightness;
}

/* Creates the paint context, loads every help archive, then looks up each picture in its archive. */
static void _InitHelp(RgHelp *pHelp)
{
    static const char *s_apszFileTbl[RG_HELP_ARCHIVE_COUNT] = {
        helpPage0101ArchiveName, helpPage0102ArchiveName, helpPage0103High2ArchiveName, helpPage0103HighArchiveName, helpPage0103LowArchiveName,
        helpPage0103MidArchiveName, helpPage0203HighArchiveName, helpPage0203LowArchiveName, helpPage0203Low2ArchiveName, helpPage020309ArchiveName,
        helpPage0203MidArchiveName, helpPage0203Mid2ArchiveName, helpPage0301ArchiveName, helpPage0302ArchiveName, helpPage0303HighArchiveName,
        helpPage0303High2ArchiveName, helpPage0303LowArchiveName, helpBgGradArchiveName, helpBgPadArchiveName
    };
    static const char *s_ainPicNameTbl[RG_HELP_PIC_COUNT][2] = {
        {helpPage0101ArchiveName, helpPic010101Name}, {helpPage0101ArchiveName, helpPic010102Name},
        {helpPage0102ArchiveName, helpPic010201Name}, {helpPage0102ArchiveName, helpPic010202Name},
        {helpPage0103HighArchiveName, helpPic010311Name}, {helpPage0103HighArchiveName, helpPic010312Name},
        {helpPage0103HighArchiveName, helpPic010314Name}, {helpPage0103HighArchiveName, helpPic010316Name},
        {helpPage0103HighArchiveName, helpPic010318Name}, {helpPage0103High2ArchiveName, helpPic010319Name},
        {helpPage0103High2ArchiveName, helpPic010320Name}, {helpPage0103High2ArchiveName, helpPic010321Name},
        {helpPage0103High2ArchiveName, helpPic010322Name}, {helpPage0103LowArchiveName, helpPic010301Name},
        {helpPage0103LowArchiveName, helpPic010302Name}, {helpPage0103LowArchiveName, helpPic010303Name},
        {helpPage0103LowArchiveName, helpPic010304Name}, {helpPage0103MidArchiveName, helpPic010305Name},
        {helpPage0103MidArchiveName, helpPic010306Name}, {helpPage0103MidArchiveName, helpPic010307Name},
        {helpPage0103MidArchiveName, helpPic010308Name}, {helpPage0103MidArchiveName, helpPic010309Name},
        {helpPage0203HighArchiveName, helpPic020320Name}, {helpPage0203HighArchiveName, helpPic020321Name},
        {helpPage0203HighArchiveName, helpPic020322Name}, {helpPage0203LowArchiveName, helpPic020301Name},
        {helpPage0203LowArchiveName, helpPic020302Name}, {helpPage0203LowArchiveName, helpPic020303Name},
        {helpPage0203LowArchiveName, helpPic020304Name}, {helpPage0203Low2ArchiveName, helpPic020305Name},
        {helpPage0203Low2ArchiveName, helpPic020306Name}, {helpPage0203Low2ArchiveName, helpPic020307Name},
        {helpPage0203Low2ArchiveName, helpPic020308Name}, {helpPage020309ArchiveName, helpPic020309UnderscoreName},
        {helpPage0203MidArchiveName, helpPic020310Name}, {helpPage0203MidArchiveName, helpPic020311Name},
        {helpPage0203MidArchiveName, helpPic020312Name}, {helpPage0203MidArchiveName, helpPic020313Name},
        {helpPage0203MidArchiveName, helpPic020314Name}, {helpPage0203MidArchiveName, helpPic020315Name},
        {helpPage0203Mid2ArchiveName, helpPic020316Name}, {helpPage0203Mid2ArchiveName, helpPic020317Name},
        {helpPage0301ArchiveName, helpPic030101Name}, {helpPage0301ArchiveName, helpPic030102Name},
        {helpPage0301ArchiveName, helpPic030103Name}, {helpPage0301ArchiveName, helpPic030104Name},
        {helpPage0302ArchiveName, helpPic030201Name}, {helpPage0303HighArchiveName, helpPic030310Name},
        {helpPage0303HighArchiveName, helpPic030311Name}, {helpPage0303HighArchiveName, helpPic030312Name},
        {helpPage0303HighArchiveName, helpPic030313Name}, {helpPage0303HighArchiveName, helpPic030314Name},
        {helpPage0303HighArchiveName, helpPic030315Name}, {helpPage0303HighArchiveName, helpPic030316Name},
        {helpPage0303HighArchiveName, helpPic030317Name}, {helpPage0303High2ArchiveName, helpPic030320Name},
        {helpPage0303High2ArchiveName, helpPic030321Name}, {helpPage0303High2ArchiveName, helpPic030322Name},
        {helpPage0303LowArchiveName, helpPic030301Name}, {helpPage0303LowArchiveName, helpPic030309Name},
        {helpBgGradArchiveName, helpBgGradPicName}, {helpBgPadArchiveName, helpBgControllerPicName}
    };
    unsigned int archive_index;
    unsigned int entry_index;
    const char *archive_name;
    const char *file_name;
    const char *picture_name;
    RgBxx *archive;
    RgBxxPic *picture;

    if (pHelp == 0) {
        assert_prog(helpAssertHelpNotNil, helpSourcePath, 110);
    }

    pHelp->ended = 0;
    pHelp->paintContext = CreateXrgPaint2D_sub(helpSourcePath, 113);
    _init_mode_left(pHelp);
    pHelp->bxxCount = 0;

    /* Load the archives in table order. */
    for (archive_index = 0; archive_index < RG_HELP_ARCHIVE_COUNT;
         archive_index++) {
        file_name = s_apszFileTbl[archive_index];
        if (archive_index >= RG_HELP_LOAD_CAPA) {
            assert_prog(helpAssertIndexBelowLoadCapacity, helpSourcePath, 121);
        }

        strcpy(pHelp->archives[archive_index].archiveName, file_name);
        pHelp->archives[archive_index].archive =
            LoadRgBxx_sub(file_name, helpSourcePath, 123);
        pHelp->bxxCount++;
    }

    /* Resolve each picture in the loaded archive that holds it. */
    for (archive_index = 0; archive_index < RG_HELP_PIC_COUNT;
         archive_index++) {
        archive_name = s_ainPicNameTbl[archive_index][0];
        picture_name = s_ainPicNameTbl[archive_index][1];
        archive = 0;
        picture = 0;

        for (entry_index = 0; entry_index < (unsigned int)pHelp->bxxCount;
             entry_index++) {
            if (strcmp(pHelp->archives[entry_index].archiveName, archive_name) == 0) {
                archive = pHelp->archives[entry_index].archive;
                break;
            }
        }

        if (archive != 0) {
            picture = RgBxxGetPic(archive, picture_name);
            if (picture == 0) {
                RgWarn(helpPicMissingWarningFormat, helpSourcePath, 148, picture_name,
                       archive_name);
            }
        } else {
            RgWarn(helpBxxNotLoadedWarningFormat, helpSourcePath, 151, archive_name);
        }
        pHelp->pics[archive_index] = picture;
    }
}

/* Releases the paint context and every archive loaded by _InitHelp. */
static void _DestructHelp(RgHelp *pHelp)
{
    unsigned int archive_index;

    if (pHelp == 0) {
        assert_prog(helpAssertHelpNotNil, helpSourcePath, 160);
    }

    DisposeXrgPaint2D_sub(pHelp->paintContext, helpSourcePath, 162);
    for (archive_index = 0; archive_index < (unsigned int)pHelp->bxxCount;
         archive_index++) {
        DisposeRgBxx_sub(pHelp->archives[archive_index].archive, helpSourcePath,
                         165);
    }
}

RgHelp *CreateRgHelp(void)
{
    RgHelp *pHelp;

    pHelp = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgHelp), helpSourcePath, 172);
    _InitHelp(pHelp);
    return pHelp;
}

void DisposeRgHelp(RgHelp *pHelp)
{
    if (pHelp == 0) {
        assert_prog(helpAssertHelpNotNil, helpSourcePath, 179);
    }
    _DestructHelp(pHelp);
    RgHeapFree(InstanceOfRgHeap(), pHelp, helpSourcePath, 181);
}

int RgHelpIsEnd(RgHelp *pHelp)
{
    if (pHelp == 0) {
        assert_prog(helpAssertHelpNotNil, helpSourcePath, 191);
    }
    return pHelp->ended;
}

/*
 * Advances the help: in the wait phase a cancel or start press begins the fade,
 * and the fade ends the help once its countdown runs out.
 */
void RgHelpPassTime(RgHelp *pHelp, float deltaTime)
{
    unsigned int phase;

    if (pHelp == 0) {
        assert_prog(helpAssertHelpNotNil, helpSourcePath, 203);
    }

    phase = pHelp->phase;
    pHelp->blinkTimer += deltaTime;

    switch (phase) {
    case RG_HELP_PHASE_CLOSED:
        pHelp->ended = 1;
        return;

    case RG_HELP_PHASE_WAIT:
        _set_global_blight(1.0f);
        _control_mode_left(pHelp);
        XrgPadSetID(0);
        if (XrgPadIsBatu() || XrgPadIsStart()) {
            pHelp->phase = RG_HELP_PHASE_FADE;
            pHelp->countdown = RG_HELP_FADE_SECONDS;
            XrgSoundSystemCancel();
        }
        return;

    case RG_HELP_PHASE_FADE:
        pHelp->countdown -= deltaTime;
        _set_global_blight(2.0f * pHelp->countdown);
        if (pHelp->countdown <= 0.0f) {
            pHelp->phase = RG_HELP_PHASE_CLOSED;
        }
        break;
    }
}

/* Draws the help background and mode screen in the wait and fade phases, then flushes. */
void RgHelpDisp(RgHelp *pHelp)
{
    if (pHelp == 0) {
        assert_prog(helpAssertHelpNotNil, helpSourcePath, 257);
    }

    if (pHelp->phase != RG_HELP_PHASE_FADE) {
        if (pHelp->phase == RG_HELP_PHASE_WAIT) {
            _paint_bg_lower(pHelp);
            _paint_mode_left(pHelp);
            _paint_bg_higher(pHelp);
        }
    } else {
        _paint_bg_lower(pHelp);
        _paint_mode_left(pHelp);
        _paint_bg_higher(pHelp);
    }

    XrgPaint2DFlush(pHelp->paintContext);
}

static void _init_mode_left(RgHelp *pHelp)
{
    pHelp->phase = RG_HELP_PHASE_WAIT;
    pHelp->cursor = 0;
    pHelp->blinkTimer = 0.0f;
}


/*
 * Moves the cursor by the pad direction pressed, playing the cursor sound when
 * it changes, and wraps the blink timer at RG_HELP_BLINK_PERIOD.
 */
static void _control_mode_left(RgHelp *pHelp)
{
    static RgHelpMoveEntry s_anMoveTbl[RG_HELP_MODE_COUNT] = {
        {-1, 4, -1, 1}, {-1, 5, 0, 2}, {-1, 6, 1, 3}, {-1, 7, 2, 4},
        {0, 8, 3, 5}, {1, 8, 4, 6}, {2, 9, 5, 7}, {3, 9, 6, 8},
        {4, 10, 7, 9}, {7, 12, 8, 10}, {8, 13, 9, 11}, {8, 14, 10, 12},
        {9, 15, 11, 13}, {10, -1, 12, 14}, {11, -1, 13, 15},
        {12, -1, 14, -1}
    };
    int cursor;
    int next;

    next = -1;
    cursor = pHelp->cursor;
    XrgPadSetID(0);
    if (XrgPadIsUp()) {
        next = s_anMoveTbl[cursor].up;
    }
    if (XrgPadIsDown()) {
        next = s_anMoveTbl[cursor].down;
    }
    if (XrgPadIsLeft()) {
        next = s_anMoveTbl[cursor].left;
    }
    if (XrgPadIsRight() || XrgPadIsMaru()) {
        next = s_anMoveTbl[cursor].right;
    }
    if (next >= 0) {
        if ((unsigned int) next >= RG_HELP_MODE_COUNT) {
            next = 0;
        }
        if (pHelp->cursor != next) {
            XrgSoundSystemCursor();
        }
        pHelp->cursor = next;
    }
    if (pHelp->blinkTimer > RG_HELP_BLINK_PERIOD) {
        pHelp->blinkTimer = 0.0f;
    }
}

/* Sets the paint colour from the global brightness; an override colour is scaled by it. */
static void _color(XrgPaint2D *paintContext, const XrgPaint2DRGBA *overrideColor)
{
    XrgPaint2DRGBA color;

    if (overrideColor != 0) {
        color.r = (overrideColor->r * s_aColor[0]) >> 7;
        color.g = (overrideColor->g * s_aColor[1]) >> 7;
        color.b = (overrideColor->b * s_aColor[2]) >> 7;
        color.a = (overrideColor->a * s_aColor[3]) >> 7;
    } else {
        color.r = (s_aColor[0] << 7) >> 7;
        color.g = (s_aColor[1] << 7) >> 7;
        color.b = (s_aColor[2] << 7) >> 7;
        color.a = ((s_aColor[3] << 7) - s_aColor[3]) >> 7;
    }

    XrgPaint2DColor(paintContext, &color);
}


static void _paint(XrgPaint2D *paintContext, RgBxxPic *pic, int blendMode, int x, int y)
{
    if (pic != 0) {
        XrgPaint2DUseTexture(paintContext, pic);
        XrgPaint2DAlpha(paintContext, blendMode);
        _color(paintContext, 0);
        XrgPaint2DDrawXYWH(paintContext, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
    }
}


const RgHelpColor helpNeutralTintColor = {.rgba = {128, 128, 128, 127}};

/* Draws a picture tinted by the neutral colour scaled by factor (0 to 128). */
static void _paint_b(XrgPaint2D *paintContext, RgBxxPic *pic, int blendMode, int x,
                     int y, int factor)
{
    RgHelpColor color;
    int upper_bound;

    color = helpNeutralTintColor;
    if (pic != 0) {
        upper_bound = 128;
        if (factor < 0) {
            factor = 0;
        } else if (upper_bound < factor) {
            factor = upper_bound;
        }

        color.rgba.r = color.rgba.r * factor >> 7;
        color.rgba.g = color.rgba.g * factor >> 7;
        color.rgba.b = color.rgba.b * factor >> 7;
        color.rgba.a = color.rgba.a * factor >> 7;
        XrgPaint2DUseTexture(paintContext, pic);
        _color(paintContext, &color.rgba);
        XrgPaint2DAlpha(paintContext, blendMode);
        XrgPaint2DDrawXYWH(paintContext, XRG_PAINT2D_MODE_USE_PIC_SIZE,
                           x, y, 0, 0);
    }
}

static void _paint_lin(XrgPaint2D *paintContext, RgBxxPic *pic, int x, int y)
{
    _paint(paintContext, pic, XRG_PAINT2D_BLEND_LINEAR, x, y);
}


/* Draws the part of a picture given by rect, placed at x + u and y + v. */
static void _paint_lin_uvwh(XrgPaint2D *paintContext, RgBxxPic *pic, int x, int y,
                            const RgHelpUvRect *rect)
{
    XrgPaint2DSetUVOffset(paintContext, rect->u, rect->v);
    XrgPaint2DSetUVSize(paintContext, rect->w, rect->h);
    if (pic != 0) {
        XrgPaint2DUseTexture(paintContext, pic);
        XrgPaint2DAlpha(paintContext, XRG_PAINT2D_BLEND_LINEAR);
        _color(paintContext, 0);
        XrgPaint2DDrawXYWH(paintContext, 0, x + rect->u, y + rect->v, rect->w,
                           rect->h);
    }
}

static void _paint_add(XrgPaint2D *paintContext, RgBxxPic *pic, int x, int y)
{
    _paint(paintContext, pic, XRG_PAINT2D_BLEND_ADD, x, y);
}

static void _paint_sub(XrgPaint2D *paintContext, RgBxxPic *pic, int x, int y)
{
    _paint(paintContext, pic, XRG_PAINT2D_BLEND_SUB, x, y);
}

static void _paint_bg_lower(RgHelp *pHelp)
{
    XrgPaint2D *paintContext;
    RgBxxPic *pic;

    paintContext = pHelp->paintContext;
    _paint(paintContext, pHelp->pics[60], 4, 0, 0x10);
    pic = pHelp->pics[61];
    _paint(paintContext, pic, 0, 0, RG_HELP_SCREEN_BOTTOM - pic->height);
}

static void _paint_bg_higher(RgHelp *pHelp)
{
    _paint_lin(pHelp->paintContext, pHelp->pics[46], 0, RG_HELP_FOOTER_Y);
}


const RgHelpAlignedUvRect helpUpperCropUvRect = {{0, 0, 432, 160}};
const RgHelpAlignedUvRect helpSideCropUvRect = {{80, 0, 432, 160}};

/* Draws the left panel for the current cursor position, then the cropped edge pictures. */
static void _paint_mode_left(RgHelp *pHelp)
{
    RgHelpAlignedUvRect upper_crop;
    RgHelpAlignedUvRect side_crop;
    RgBxxPic **pictures;
    XrgPaint2D *paintContext;

    pictures = pHelp->pics;
    paintContext = pHelp->paintContext;
    /* One case per cursor position: a tinted backdrop, the body and the footer. */
    switch ((unsigned int)pHelp->cursor) {
    case 0:
        _paint_b(paintContext, pictures[13], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[25], 0, RG_HELP_BODY_Y);
        _paint_b(paintContext, pictures[58], 0, 0, RG_HELP_FOOTER_Y, 100);
        _paint_lin(paintContext, pictures[45], 0, RG_HELP_FOOTER_Y);
        break;
    case 1:
        _paint_b(paintContext, pictures[14], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[26], 0, RG_HELP_BODY_Y);
        _paint_b(paintContext, pictures[58], 0, 0, RG_HELP_FOOTER_Y, 100);
        _paint_lin(paintContext, pictures[45], 0, RG_HELP_FOOTER_Y);
        break;
    case 2:
        _paint_b(paintContext, pictures[15], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[27], 0, RG_HELP_BODY_Y);
        _paint_b(paintContext, pictures[58], 0, 0, RG_HELP_FOOTER_Y, 100);
        _paint_lin(paintContext, pictures[45], 0, RG_HELP_FOOTER_Y);
        break;
    case 3:
        _paint_b(paintContext, pictures[16], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[28], 0, RG_HELP_BODY_Y);
        _paint_b(paintContext, pictures[58], 0, 0, RG_HELP_FOOTER_Y, 100);
        _paint_lin(paintContext, pictures[45], 0, RG_HELP_FOOTER_Y);
        break;
    case 4:
        _paint_b(paintContext, pictures[17], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[29], 0, RG_HELP_BODY_Y);
        _paint_b(paintContext, pictures[58], 0, 0, RG_HELP_FOOTER_Y, 100);
        _paint_lin(paintContext, pictures[45], 0, RG_HELP_FOOTER_Y);
        break;
    case 5:
        _paint_b(paintContext, pictures[18], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[31], 0, RG_HELP_BODY_Y);
        _paint_b(paintContext, pictures[58], 0, 0, RG_HELP_FOOTER_Y, 100);
        _paint_lin(paintContext, pictures[45], 0, RG_HELP_FOOTER_Y);
        break;
    case 6:
        _paint_b(paintContext, pictures[19], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[30], 0, RG_HELP_BODY_Y);
        _paint_b(paintContext, pictures[58], 0, 0, RG_HELP_FOOTER_Y, 100);
        _paint_lin(paintContext, pictures[45], 0, RG_HELP_FOOTER_Y);
        break;
    case 7:
        _paint_b(paintContext, pictures[20], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[32], 0, RG_HELP_BODY_Y);
        _paint_b(paintContext, pictures[58], 0, 0, RG_HELP_FOOTER_Y, 100);
        _paint_lin(paintContext, pictures[45], 0, RG_HELP_FOOTER_Y);
        break;
    case 8:
        _paint_b(paintContext, pictures[21], 0, 8, -16, 100);
        if (pHelp->blinkTimer < RG_HELP_BLINK_HALF_PERIOD) {
            _paint_lin(paintContext, pictures[33], 0, RG_HELP_BODY_Y);
            _paint_b(paintContext, pictures[59], 0, 0, RG_HELP_FOOTER_Y, 100);
        } else {
            _paint_lin(paintContext, pictures[34], 0, RG_HELP_BODY_Y);
            _paint_b(paintContext, pictures[47], 0, 0, RG_HELP_FOOTER_Y, 100);
        }
        _paint_lin(paintContext, pictures[43], 0, RG_HELP_FOOTER_Y);
        break;
    case 9:
        _paint_b(paintContext, pictures[4], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[35], 0, RG_HELP_BODY_Y);
        _paint_b(paintContext, pictures[48], 0, 0, RG_HELP_FOOTER_Y, 100);
        _paint_lin(paintContext, pictures[45], 0, RG_HELP_FOOTER_Y);
        break;
    case 10:
        _paint_b(paintContext, pictures[5], 0, 8, -16, 100);
        if (pHelp->blinkTimer < RG_HELP_BLINK_HALF_PERIOD) {
            _paint_lin(paintContext, pictures[36], 0, RG_HELP_BODY_Y);
            _paint_b(paintContext, pictures[49], 0, 0, RG_HELP_FOOTER_Y, 100);
        } else {
            _paint_lin(paintContext, pictures[37], 0, RG_HELP_BODY_Y);
            _paint_lin(paintContext, pictures[50], 0, RG_HELP_FOOTER_Y);
            _paint_b(paintContext, pictures[50], 0, 0, RG_HELP_FOOTER_Y, 100);
        }
        _paint_lin(paintContext, pictures[42], 0, RG_HELP_FOOTER_Y);
        break;
    case 11:
        _paint_b(paintContext, pictures[6], 0, 8, -16, 100);
        if (pHelp->blinkTimer < RG_HELP_BLINK_HALF_PERIOD) {
            _paint_lin(paintContext, pictures[38], 0, RG_HELP_BODY_Y);
            _paint_b(paintContext, pictures[51], 0, 0, RG_HELP_FOOTER_Y, 100);
        } else {
            _paint_lin(paintContext, pictures[39], 0, RG_HELP_BODY_Y);
            _paint_b(paintContext, pictures[52], 0, 0, RG_HELP_FOOTER_Y, 100);
        }
        _paint_lin(paintContext, pictures[44], 0, RG_HELP_FOOTER_Y);
        break;
    case 12:
        _paint_b(paintContext, pictures[7], 0, 8, -16, 100);
        if (pHelp->blinkTimer < RG_HELP_BLINK_HALF_PERIOD) {
            _paint_lin(paintContext, pictures[40], 0, RG_HELP_BODY_Y);
            _paint_b(paintContext, pictures[53], 0, 0, RG_HELP_FOOTER_Y, 100);
        } else {
            _paint_lin(paintContext, pictures[41], 0, RG_HELP_BODY_Y);
            _paint_b(paintContext, pictures[54], 0, 0, RG_HELP_FOOTER_Y, 100);
        }
        _paint_lin(paintContext, pictures[43], 0, RG_HELP_FOOTER_Y);
        break;
    case 13:
        _paint_b(paintContext, pictures[10], 0, -8, 18, 100);
        _paint_lin(paintContext, pictures[22], 0, RG_HELP_BODY_Y);
        _paint_b(paintContext, pictures[55], 0, 0, RG_HELP_FOOTER_Y, 100);
        _paint_lin(paintContext, pictures[45], 0, RG_HELP_FOOTER_Y);
        break;
    case 14:
        _paint_b(paintContext, pictures[11], 0, -8, 18, 100);
        _paint_lin(paintContext, pictures[23], 0, RG_HELP_BODY_Y);
        _paint_b(paintContext, pictures[56], 0, 0, RG_HELP_FOOTER_Y, 100);
        _paint_lin(paintContext, pictures[45], 0, RG_HELP_FOOTER_Y);
        break;
    case 15:
        _paint_b(paintContext, pictures[12], 0, -8, 18, 100);
        _paint_lin(paintContext, pictures[24], 0, RG_HELP_BODY_Y);
        _paint_b(paintContext, pictures[57], 0, 0, RG_HELP_FOOTER_Y, 100);
        _paint_lin(paintContext, pictures[45], 0, RG_HELP_FOOTER_Y);
        break;
    }

    /* Overlay the cropped top and side edges. */
    upper_crop = helpUpperCropUvRect;
    _paint_lin_uvwh(paintContext, pictures[0], 8, -16, &upper_crop.rect);
    _paint_lin_uvwh(paintContext, pictures[2], 8, -16, &upper_crop.rect);
    side_crop = helpSideCropUvRect;
    _paint_lin_uvwh(paintContext, pictures[1], -8, 18, &side_crop.rect);
    _paint_lin_uvwh(paintContext, pictures[3], -8, 18, &side_crop.rect);
}

/* Draws the right-hand panel for the current cursor position. */
static void _paint_mode_right(RgHelp *pHelp)
{
    RgBxxPic **pictures = pHelp->pics;
    XrgPaint2D *paintContext = pHelp->paintContext;

    switch ((unsigned int)pHelp->cursor) {
    case 0:
        _paint_b(paintContext, pictures[9], 0, 0, 0, 100);
        break;
    case 1:
        _paint_b(paintContext, pictures[10], 0, 0, 0, 100);
        _paint_lin(paintContext, pictures[22], 0, RG_HELP_BODY_Y);
        _paint(paintContext, pictures[55], 0, 0, RG_HELP_FOOTER_Y);
        break;
    case 2:
        _paint_b(paintContext, pictures[11], 0, 0, 0, 100);
        _paint_lin(paintContext, pictures[23], 0, RG_HELP_BODY_Y);
        _paint(paintContext, pictures[56], 0, 0, RG_HELP_FOOTER_Y);
        break;
    case 3:
        _paint_b(paintContext, pictures[12], 0, 0, 0, 100);
        _paint_lin(paintContext, pictures[24], 0, RG_HELP_BODY_Y);
        _paint(paintContext, pictures[57], 0, 0, RG_HELP_FOOTER_Y);
        break;
    }

    _paint_lin(paintContext, pictures[3], 0, 0);
    _paint_lin(paintContext, pictures[1], 0, 0);
    _paint_lin(paintContext, pictures[45], 0, RG_HELP_FOOTER_Y);
}



const char helpBgPadArchiveName[16] = "help_bg_pad.bxx";

const char helpBgGradArchiveName[24] = "help_bg_grad.bxx";

const char helpPage0303LowArchiveName[24] = "help_03_03_low.bxx";

const char helpPage0303High2ArchiveName[24] = "help_03_03_high2.bxx";

const char helpPage0303HighArchiveName[24] = "help_03_03_high.bxx";

const char helpPage0302ArchiveName[16] = "help_03_02.bxx";

const char helpPage0301ArchiveName[16] = "help_03_01.bxx";

const char helpPage0203Mid2ArchiveName[24] = "help_02_03_mid2.bxx";

const char helpPage0203MidArchiveName[24] = "help_02_03_mid.bxx";

const char helpPage020309ArchiveName[24] = "help_02_03_09.bxx";

const char helpPage0203Low2ArchiveName[24] = "help_02_03_low2.bxx";

const char helpPage0203LowArchiveName[24] = "help_02_03_low.bxx";

const char helpPage0203HighArchiveName[24] = "help_02_03_high.bxx";

const char helpPage0103MidArchiveName[24] = "help_01_03_mid.bxx";

const char helpPage0103LowArchiveName[24] = "help_01_03_low.bxx";

const char helpPage0103HighArchiveName[24] = "help_01_03_high.bxx";

const char helpPage0103High2ArchiveName[24] = "help_01_03_high2.bxx";

const char helpPage0102ArchiveName[16] = "help_01_02.bxx";

const char helpPage0101ArchiveName[16] = "help_01_01.bxx";

const char helpBgControllerPicName[16] = "bg_ctrlr";

const char helpBgGradPicName[8] = "bg_grad";

const char helpPic030309Name[16] = "03_03_09";

const char helpPic030301Name[16] = "03_03_01";

const char helpPic030322Name[16] = "03_03_22";

const char helpPic030321Name[16] = "03_03_21";

const char helpPic030320Name[16] = "03_03_20";

const char helpPic030317Name[16] = "03_03_17";

const char helpPic030316Name[16] = "03_03_16";

const char helpPic030315Name[16] = "03_03_15";

const char helpPic030314Name[16] = "03_03_14";

const char helpPic030313Name[16] = "03_03_13";

const char helpPic030312Name[16] = "03_03_12";

const char helpPic030311Name[16] = "03_03_11";

const char helpPic030310Name[16] = "03_03_10";

const char helpPic030201Name[16] = "03_02_01";

const char helpPic030104Name[16] = "03_01_04";

const char helpPic030103Name[16] = "03_01_03";

const char helpPic030102Name[16] = "03_01_02";

const char helpPic030101Name[16] = "03_01_01";

const char helpPic020317Name[16] = "02_03_17";

const char helpPic020316Name[16] = "02_03_16";

const char helpPic020315Name[16] = "02_03_15";

const char helpPic020314Name[16] = "02_03_14";

const char helpPic020313Name[16] = "02_03_13";

const char helpPic020312Name[16] = "02_03_12";

const char helpPic020311Name[16] = "02_03_11";

const char helpPic020310Name[16] = "02_03_10";

const char helpPic020309UnderscoreName[16] = "02_03_09_";

const char helpPic020308Name[16] = "02_03_08";

const char helpPic020307Name[16] = "02_03_07";

const char helpPic020306Name[16] = "02_03_06";

const char helpPic020305Name[16] = "02_03_05";

const char helpPic020304Name[16] = "02_03_04";

const char helpPic020303Name[16] = "02_03_03";

const char helpPic020302Name[16] = "02_03_02";

const char helpPic020301Name[16] = "02_03_01";

const char helpPic020322Name[16] = "02_03_22";

const char helpPic020321Name[16] = "02_03_21";

const char helpPic020320Name[16] = "02_03_20";

const char helpPic010309Name[16] = "01_03_09";

const char helpPic010308Name[16] = "01_03_08";

const char helpPic010307Name[16] = "01_03_07";

const char helpPic010306Name[16] = "01_03_06";

const char helpPic010305Name[16] = "01_03_05";

const char helpPic010304Name[16] = "01_03_04";

const char helpPic010303Name[16] = "01_03_03";

const char helpPic010302Name[16] = "01_03_02";

const char helpPic010301Name[16] = "01_03_01";

const char helpPic010322Name[16] = "01_03_22";

const char helpPic010321Name[16] = "01_03_21";

const char helpPic010320Name[16] = "01_03_20";

const char helpPic010319Name[16] = "01_03_19";

const char helpPic010318Name[16] = "01_03_18";

const char helpPic010316Name[16] = "01_03_16";

const char helpPic010314Name[16] = "01_03_14";

const char helpPic010312Name[16] = "01_03_12";

const char helpPic010311Name[16] = "01_03_11";

const char helpPic010202Name[16] = "01_02_02";

const char helpPic010201Name[16] = "01_02_01";

const char helpPic010102Name[16] = "01_01_02";

const char helpPic010101Name[16] = "01_01_01";
