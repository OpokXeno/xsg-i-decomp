/*
 * OV12 original TU 74: 0x00a3f230..0x00a40008 (12 functions)
 */
#include "common.h"
#include "shared.h"

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern void RgError(const char *message, const char *source_file, int line,
                    ...);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(void *heap, void *pointer, const char *source_file,
                       int line);
typedef struct XrgPaint2D XrgPaint2D;

extern void XrgPaint2DUseTexture(XrgPaint2D *paint, void *pic);
extern void XrgPaint2DAlpha(XrgPaint2D *paint, int blendMode);
extern void XrgPaint2DDrawXYWH(XrgPaint2D *paint, int mode, int x, int y,
                               int width, int height);
extern void XrgPadSetID(int padID);
extern int XrgPadIsUp(void);
extern int XrgPadIsDown(void);
extern int XrgPadIsMaru(void);
extern int XrgPadIsStart(void);
extern void XrgSoundSystemCursor(void);
extern void XrgSoundSystemOk(void);
extern void XrgSoundSystemCancel(void);

#define XRG_PAINT2D_MODE_USE_PIC_SIZE 0x20
#define XRG_PAINT2D_BLEND_LINEAR 0
#define XRG_PAINT2D_BLEND_ADD 1

#define TITLE_BACKGROUND_COUNT 2
#define TITLE_CURSOR_Y 176
#define TITLE_GLOW_AMPLITUDE 64.0f
#define TITLE_GLOW_MIDPOINT 64
#define TITLE_ROTATION_STEP 0.09817477f
#define TITLE_PI 3.1415927f
#define TITLE_TWO_PI 6.2831855f

extern const char D_00A57118[]; /* "pPaint != NIL" */
extern const char D_00A57128[]; /* "../rg_title.euc.c" */
extern const char D_00A57140[]; /* "pPic != NIL" */
extern const char D_00A57150[]; /* "pTitle != NIL" */
extern const char D_00A57160[];
extern const char D_00A57170[];
extern const char D_00A57188[];
extern const char D_00A571A0[];
extern const char D_00A571B0[];
extern const char D_00A571C0[];
extern const char D_00A571D0[];
extern const char D_00A571E0[];
extern const char D_00A571F0[];
extern const char D_00A57200[];
extern const char D_00A57208[];
extern const char D_00A57218[];
extern const char D_00A57228[];
extern const char D_00A57238[];
extern const char D_00A57248[];
extern const char D_00A57258[];
extern const char D_00A57268[];
extern const char D_00A57278[];
extern const char D_00A57288[];
extern const char D_00A57298[];
extern const char D_00A572A8[];
extern const char D_00A572B8[];
extern const char D_00A572C8[];
extern const char D_00A572D8[];

/*
 * The blend color XrgPaint2DColor copies into a paint object with one
 * lqc2/sqc2 quadword (ov12:0x00a4bd7c/0x00a4bd80).  s_aCol's own bytes are
 * the words 0x80, 0x80, 0x80, 0x7F: full-range red/green/blue in the GS's
 * 0-0x80 fixed scale with alpha one unit short of it, the same per-component
 * integer layout RgPicColor documents for RgPicSetColor
 * (src/ov12/rg_piclist.h).
 */
typedef struct XrgColor {
    int r;
    int g;
    int b;
    int a;
} XrgColor;

extern void XrgPaint2DColor(XrgPaint2D *paint, const XrgColor *color);
extern XrgColor s_aCol;

#include "rg_title.h"

extern XrgPaint2D *CreateXrgPaint2D_sub(const char *source_file, int line);
extern RgBxx *LoadRgBxx_sub(const char *filename, const char *archive_name,
                            int mode);
extern RgBxxPic *RgBxxGetPic(RgBxx *archive, const char *filename);

typedef struct RgTitle RgTitle;

static void _InitTitle(RgTitle *pTitle, int count);
static void _DestructTitle(RgTitle *pTitle);

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

static void _disp_normal(XrgPaint2D *pPaint, void *pPic, int x, int y)
{
    if (pPaint == 0) {
        assert_prog(D_00A57118, D_00A57128, 101);
    }
    if (pPic == 0) {
        assert_prog(D_00A57140, D_00A57128, 102);
    }
    XrgPaint2DUseTexture(pPaint, pPic);
    XrgPaint2DColor(pPaint, &s_aCol);
    XrgPaint2DDrawXYWH(pPaint, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
}

static void _disp_linear(XrgPaint2D *pPaint, void *pPic, int x, int y)
{
    if (pPaint == 0) {
        assert_prog(D_00A57118, D_00A57128, 110);
    }
    if (pPic == 0) {
        assert_prog(D_00A57140, D_00A57128, 111);
    }
    XrgPaint2DUseTexture(pPaint, pPic);
    XrgPaint2DColor(pPaint, &s_aCol);
    XrgPaint2DAlpha(pPaint, XRG_PAINT2D_BLEND_LINEAR);
    XrgPaint2DDrawXYWH(pPaint, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
}

static void _disp_add(XrgPaint2D *pPaint, void *pPic, int x, int y)
{
    if (pPaint == 0) {
        assert_prog(D_00A57118, D_00A57128, 119);
    }
    if (pPic == 0) {
        assert_prog(D_00A57140, D_00A57128, 120);
    }
    XrgPaint2DUseTexture(pPaint, pPic);
    XrgPaint2DColor(pPaint, &s_aCol);
    XrgPaint2DAlpha(pPaint, XRG_PAINT2D_BLEND_ADD);
    XrgPaint2DDrawXYWH(pPaint, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
}

static void _disp_add_blight(XrgPaint2D *pPaint, void *pPic, int x, int y,
                             int brightness)
{
    XrgColor color;
    int maximum_brightness;

    if (pPaint == 0) {
        assert_prog(D_00A57118, D_00A57128, 129);
    }
    if (pPic == 0) {
        assert_prog(D_00A57140, D_00A57128, 130);
    }

    maximum_brightness = 128;
    if (brightness < 0) {
        brightness = 0;
    } else {
        if (maximum_brightness < brightness) {
            brightness = maximum_brightness;
        }
    }

    color.r = (s_aCol.r * brightness) >> 7;
    color.g = (s_aCol.g * brightness) >> 7;
    color.b = (s_aCol.b * brightness) >> 7;
    color.a = (s_aCol.a * brightness) >> 7;

    XrgPaint2DUseTexture(pPaint, pPic);
    XrgPaint2DColor(pPaint, &color);
    XrgPaint2DAlpha(pPaint, XRG_PAINT2D_BLEND_ADD);
    XrgPaint2DDrawXYWH(pPaint, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
}

static void _InitTitle(RgTitle *pTitle, int count)
{
    unsigned int picture_index;

    if (pTitle == 0) {
        assert_prog(D_00A57150, D_00A57128, 147);
    }
    pTitle->count = count;
    pTitle->cursor_id = 0;
    pTitle->menu_state = 0;
    pTitle->paint = CreateXrgPaint2D_sub(D_00A57128, 153);
    pTitle->bxx[0] = LoadRgBxx_sub(D_00A57160, D_00A57128, 155);
    pTitle->bxx[1] = LoadRgBxx_sub(D_00A57170, D_00A57128, 156);
    pTitle->bxx[2] = LoadRgBxx_sub(D_00A57188, D_00A57128, 157);
    pTitle->bxx[3] = LoadRgBxx_sub(D_00A571A0, D_00A57128, 158);
    pTitle->bxx[4] = LoadRgBxx_sub(D_00A571B0, D_00A57128, 159);
    pTitle->bxx[5] = LoadRgBxx_sub(D_00A571C0, D_00A57128, 160);
    pTitle->bxx[6] = LoadRgBxx_sub(D_00A571D0, D_00A57128, 161);
    pTitle->bxx[7] = LoadRgBxx_sub(D_00A571E0, D_00A57128, 162);
    pTitle->bxx[8] = LoadRgBxx_sub(D_00A571F0, D_00A57128, 163);

    pTitle->pictures[0] = RgBxxGetPic(pTitle->bxx[0], D_00A57200);
    pTitle->pictures[1] = RgBxxGetPic(pTitle->bxx[1], D_00A57208);
    pTitle->pictures[2] = RgBxxGetPic(pTitle->bxx[2], D_00A57218);
    pTitle->pictures[3] = RgBxxGetPic(pTitle->bxx[3], D_00A57228);
    pTitle->pictures[4] = RgBxxGetPic(pTitle->bxx[4], D_00A57238);
    pTitle->pictures[10] = RgBxxGetPic(pTitle->bxx[7], D_00A57248);
    pTitle->pictures[11] = RgBxxGetPic(pTitle->bxx[7], D_00A57258);
    pTitle->pictures[12] = RgBxxGetPic(pTitle->bxx[8], D_00A57268);
    pTitle->pictures[13] = RgBxxGetPic(pTitle->bxx[8], D_00A57278);
    pTitle->pictures[5] = RgBxxGetPic(pTitle->bxx[5], D_00A57288);
    pTitle->pictures[6] = RgBxxGetPic(pTitle->bxx[5], D_00A57298);
    pTitle->pictures[7] = RgBxxGetPic(pTitle->bxx[5], D_00A572A8);
    pTitle->pictures[8] = RgBxxGetPic(pTitle->bxx[6], D_00A572B8);
    pTitle->pictures[9] = RgBxxGetPic(pTitle->bxx[6], D_00A572C8);

    for (picture_index = 0; picture_index < 14; picture_index++) {
        if (pTitle->pictures[picture_index] == 0) {
            RgError(D_00A572D8, D_00A57128, 213, picture_index);
        }
    }
}

extern void DisposeXrgPaint2D_sub(XrgPaint2D *paint, const char *source_file,
                                  int line);
extern void DisposeRgBxx_sub(RgBxx *pBxx, const char *pszFile, int nLine);

static void _DestructTitle(RgTitle *pTitle)
{
    if (pTitle == 0) {
        assert_prog(D_00A57150, D_00A57128, 218);
    }
    DisposeXrgPaint2D_sub(pTitle->paint, D_00A57128, 219);
    DisposeRgBxx_sub(pTitle->bxx[0], D_00A57128, 221);
    DisposeRgBxx_sub(pTitle->bxx[1], D_00A57128, 222);
    DisposeRgBxx_sub(pTitle->bxx[2], D_00A57128, 223);
    DisposeRgBxx_sub(pTitle->bxx[3], D_00A57128, 224);
    DisposeRgBxx_sub(pTitle->bxx[4], D_00A57128, 225);
    DisposeRgBxx_sub(pTitle->bxx[5], D_00A57128, 226);
    DisposeRgBxx_sub(pTitle->bxx[6], D_00A57128, 227);
    DisposeRgBxx_sub(pTitle->bxx[7], D_00A57128, 228);
    DisposeRgBxx_sub(pTitle->bxx[8], D_00A57128, 229);
}

RgTitle *CreateRgTitle(int count)
{
    RgTitle *pTitle;

    pTitle = RgHeapAlloc(InstanceOfRgHeap(), 0x70, D_00A57128, 236);
    _InitTitle(pTitle, count);
    return pTitle;
}

void DisposeRgTitle(RgTitle *pTitle)
{
    if (pTitle == 0) {
        assert_prog(D_00A57150, D_00A57128, 244);
    }
    _DestructTitle(pTitle);
    RgHeapFree(InstanceOfRgHeap(), pTitle, D_00A57128, 246);
}

extern void RgError(const char *message, const char *source_file, int line, ...);
extern int XrgPadIsSelectLevelLR(void);
extern int XrgEventGetLevel(void);
extern const char D_00A572F0[];
extern float s_fRot_0;
extern int s_nID_1;
extern void XrgPaint2DFlush(XrgPaint2D *paint);
static void _disp_blight(float brightness);
static void _disp_add_blight(XrgPaint2D *paint, void *picture, int x, int y,
                             int brightness);

int RgTitleGetResult(RgTitle *pTitle)
{
    int cursor_id;

    if (pTitle == 0) {
        assert_prog(D_00A57150, D_00A57128, 256);
    }

    if (pTitle->menu_state == 3) {
        cursor_id = pTitle->cursor_id;
        switch ((unsigned int)cursor_id) {
        case 1:
            return 3;
        case 0:
            if (!XrgPadIsSelectLevelLR() || XrgEventGetLevel() < 2) {
                return 1;
            }
            return 2;
        case 2:
            return 4;
        case 3:
            return 5;
        default:
            RgError(D_00A572F0, D_00A57128, 274, cursor_id);
            break;
        }
    }
    return 0;
}

void RgTitlePassTime(RgTitle *pTitle, float delta)
{
    if (pTitle == 0) {
        assert_prog(D_00A57150, D_00A57128, 287);
    }

    XrgPadSetID(0);

    if (pTitle->menu_state != 1) {
        if (pTitle->menu_state != 0) {
            if (pTitle->menu_state != 2) {
                return;
            }
        } else {
            if (XrgPadIsUp() != 0 && pTitle->cursor_id != 0) {
                pTitle->cursor_id--;
                XrgSoundSystemCursor();
            }
            if (XrgPadIsDown() != 0 && (unsigned int) pTitle->cursor_id < 3) {
                pTitle->cursor_id++;
                XrgSoundSystemCursor();
            }

            if (XrgPadIsMaru() != 0 || XrgPadIsStart() != 0) {
                pTitle->menu_state = 1;
                pTitle->transition_time = 1.0f;
                if (pTitle->cursor_id == 3) {
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
            pTitle->menu_state = 2;
            pTitle->transition_time = 0.5f;
        }
        return;
    }

    pTitle->transition_time -= delta;
    if (pTitle->transition_time < 0.0f) {
        pTitle->menu_state = 3;
    }
}

void RgTitleDisp(RgTitle *pTitle)
{
    float rotation;
    int glow;
    int highlight = 1;

    if (pTitle == 0) {
        assert_prog(D_00A57150, D_00A57128, 333);
    }
    if (pTitle->menu_state == 3) {
        return;
    }
    if (pTitle->menu_state == 2) {
        _disp_blight(pTitle->transition_time * 2.0f);
    } else {
        _disp_blight(1.0f);
    }

    switch (pTitle->count % TITLE_BACKGROUND_COUNT) {
    case 0:
        _disp_normal(pTitle->paint, pTitle->pictures[2], 0, 0);
        break;
    case 1:
        _disp_normal(pTitle->paint, pTitle->pictures[1], 0, 0);
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
    _disp_add_blight(pTitle->paint, pTitle->pictures[4], 0, 0, glow);
    _disp_add(pTitle->paint, pTitle->pictures[3], 0, 0);

    if ((unsigned int)(pTitle->menu_state - 1) < 2) {
        s_nID_1++;
        highlight = s_nID_1 & 1;
    }

    switch ((unsigned int)pTitle->cursor_id) {
    case 0:
        _disp_linear(pTitle->paint, pTitle->pictures[6], 0, TITLE_CURSOR_Y);
        if (highlight) {
            _disp_add(pTitle->paint, pTitle->pictures[10], 0, TITLE_CURSOR_Y);
        }
        break;
    case 1:
        _disp_linear(pTitle->paint, pTitle->pictures[8], 0, TITLE_CURSOR_Y);
        if (highlight) {
            _disp_add(pTitle->paint, pTitle->pictures[13], 0, TITLE_CURSOR_Y);
        }
        break;
    case 2:
        _disp_linear(pTitle->paint, pTitle->pictures[7], 0, TITLE_CURSOR_Y);
        if (highlight) {
            _disp_add(pTitle->paint, pTitle->pictures[12], 0, TITLE_CURSOR_Y);
        }
        break;
    case 3:
        _disp_linear(pTitle->paint, pTitle->pictures[9], 0, TITLE_CURSOR_Y);
        if (highlight) {
            _disp_add(pTitle->paint, pTitle->pictures[11], 0, TITLE_CURSOR_Y);
        }
        break;
    default:
        RgError(D_00A572F0, D_00A57128, 403, pTitle->cursor_id);
        break;
    }
    XrgPaint2DFlush(pTitle->paint);
}
