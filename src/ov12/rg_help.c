/*
 * OV12 original TU 76: 0x00a420e0..0x00a434b8 (21 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_help.h"

const char D_00A57AC8[] = "pHelp != NIL";
const char D_00A57AD8[] = "../rg_help.euc.c";
const char D_00A57AF0[] = "i < LOAD_CAPA";
const char D_00A57B00[] = "'%s' pic is not exist in '%s'";
const char D_00A57B20[] = "'%s' bxx is not loaded";
static int s_aColor[4] = {128, 128, 128, 128};
extern const char D_00A57550[], D_00A57560[], D_00A57578[], D_00A57590[];
extern const char D_00A575A8[], D_00A575C0[], D_00A575D0[], D_00A575E0[];
extern const char D_00A575F8[], D_00A57610[], D_00A57628[], D_00A57640[];
extern const char D_00A57658[], D_00A57670[], D_00A57688[], D_00A576A0[];
extern const char D_00A576B8[], D_00A576D0[], D_00A576E0[], D_00A576F0[];
extern const char D_00A57700[], D_00A57708[], D_00A57718[], D_00A57728[];
extern const char D_00A57738[], D_00A57748[], D_00A57758[], D_00A57768[];
extern const char D_00A57778[], D_00A57788[], D_00A57798[], D_00A577A8[];
extern const char D_00A577B8[], D_00A577C8[], D_00A577D8[], D_00A577E8[];
extern const char D_00A577F8[], D_00A57808[], D_00A57818[], D_00A57828[];
extern const char D_00A57838[], D_00A57848[], D_00A57858[], D_00A57868[];
extern const char D_00A57878[], D_00A57888[], D_00A57898[], D_00A578A8[];
extern const char D_00A578B8[], D_00A578C8[], D_00A578D8[], D_00A578E8[];
extern const char D_00A578F8[], D_00A57908[], D_00A57918[], D_00A57928[];
extern const char D_00A57938[], D_00A57948[], D_00A57958[], D_00A57968[];
extern const char D_00A57978[], D_00A57988[], D_00A57998[], D_00A579A8[];
extern const char D_00A579B8[], D_00A579C8[], D_00A579D8[], D_00A579E8[];
extern const char D_00A579F8[], D_00A57A08[], D_00A57A18[], D_00A57A28[];
extern const char D_00A57A38[], D_00A57A48[], D_00A57A58[], D_00A57A68[];
extern const char D_00A57A78[], D_00A57A88[], D_00A57A98[], D_00A57AA8[];
extern const char D_00A57AB8[];

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
extern void RgHeapFree(void *heap, void *pointer, const char *source_file,
                       int line);
static void _InitHelp(RgHelp *pHelp);
static void _DestructHelp(RgHelp *pHelp);
static void _init_mode_left(RgHelp *pHelp);
extern char *strcpy(char *destination, const char *source);
extern int strcmp(const char *left, const char *right);
extern void *CreateXrgPaint2D_sub(const char *source_file, int line);
extern RgBxx *LoadRgBxx_sub(const char *name, const char *source_file, int line);
extern RgBxxPic *RgBxxGetPic(RgBxx *archive, const char *picture_name);
extern void RgWarn(const char *format, const char *source_file, int line, ...);
extern const char D_00A57AF0[];
extern const char D_00A57B00[];
extern const char D_00A57B20[];
extern void DisposeXrgPaint2D_sub(void *paintContext, const char *source_file,
                                  int line);
extern void DisposeRgBxx_sub(RgBxx *archive, const char *source_file, int line);
extern void XrgPaint2DColor(void *paintContext, const int *color);

extern const char D_00A57AC8[];
extern const char D_00A57AD8[];

static void _set_global_blight(float blight)
{
    int scaled_brightness;
    int brightness;
    int upper_bound;
    int exceeds_upper_bound;

    upper_bound = 128;
    brightness = 0;
    scaled_brightness = (int)(blight * 128.0f);
    if (scaled_brightness >= 0) {
        exceeds_upper_bound = scaled_brightness > upper_bound;
        brightness = exceeds_upper_bound ? upper_bound : scaled_brightness;
    }

    s_aColor[3] = brightness;
    s_aColor[2] = brightness;
    s_aColor[1] = brightness;
    s_aColor[0] = brightness;
}

static void _InitHelp(RgHelp *pHelp)
{
    static const char *s_apszFileTbl[19] = {
        D_00A576E0, D_00A576D0, D_00A576B8, D_00A576A0, D_00A57688,
        D_00A57670, D_00A57658, D_00A57640, D_00A57628, D_00A57610,
        D_00A575F8, D_00A575E0, D_00A575D0, D_00A575C0, D_00A575A8,
        D_00A57590, D_00A57578, D_00A57560, D_00A57550
    };
    static const char *s_ainPicNameTbl[62][2] = {
        {D_00A576E0, D_00A57AB8}, {D_00A576E0, D_00A57AA8},
        {D_00A576D0, D_00A57A98}, {D_00A576D0, D_00A57A88},
        {D_00A576A0, D_00A57A78}, {D_00A576A0, D_00A57A68},
        {D_00A576A0, D_00A57A58}, {D_00A576A0, D_00A57A48},
        {D_00A576A0, D_00A57A38}, {D_00A576B8, D_00A57A28},
        {D_00A576B8, D_00A57A18}, {D_00A576B8, D_00A57A08},
        {D_00A576B8, D_00A579F8}, {D_00A57688, D_00A579E8},
        {D_00A57688, D_00A579D8}, {D_00A57688, D_00A579C8},
        {D_00A57688, D_00A579B8}, {D_00A57670, D_00A579A8},
        {D_00A57670, D_00A57998}, {D_00A57670, D_00A57988},
        {D_00A57670, D_00A57978}, {D_00A57670, D_00A57968},
        {D_00A57658, D_00A57958}, {D_00A57658, D_00A57948},
        {D_00A57658, D_00A57938}, {D_00A57640, D_00A57928},
        {D_00A57640, D_00A57918}, {D_00A57640, D_00A57908},
        {D_00A57640, D_00A578F8}, {D_00A57628, D_00A578E8},
        {D_00A57628, D_00A578D8}, {D_00A57628, D_00A578C8},
        {D_00A57628, D_00A578B8}, {D_00A57610, D_00A578A8},
        {D_00A575F8, D_00A57898}, {D_00A575F8, D_00A57888},
        {D_00A575F8, D_00A57878}, {D_00A575F8, D_00A57868},
        {D_00A575F8, D_00A57858}, {D_00A575F8, D_00A57848},
        {D_00A575E0, D_00A57838}, {D_00A575E0, D_00A57828},
        {D_00A575D0, D_00A57818}, {D_00A575D0, D_00A57808},
        {D_00A575D0, D_00A577F8}, {D_00A575D0, D_00A577E8},
        {D_00A575C0, D_00A577D8}, {D_00A575A8, D_00A577C8},
        {D_00A575A8, D_00A577B8}, {D_00A575A8, D_00A577A8},
        {D_00A575A8, D_00A57798}, {D_00A575A8, D_00A57788},
        {D_00A575A8, D_00A57778}, {D_00A575A8, D_00A57768},
        {D_00A575A8, D_00A57758}, {D_00A57590, D_00A57748},
        {D_00A57590, D_00A57738}, {D_00A57590, D_00A57728},
        {D_00A57578, D_00A57718}, {D_00A57578, D_00A57708},
        {D_00A57560, D_00A57700}, {D_00A57550, D_00A576F0}
    };
    unsigned int archive_index;
    unsigned int entry_index;
    const char *archive_name;
    const char *file_name;
    const char *picture_name;
    RgBxx *archive;
    RgBxxPic *picture;

    if (pHelp == 0) {
        assert_prog(D_00A57AC8, D_00A57AD8, 110);
    }

    pHelp->ended = 0;
    pHelp->paintContext = CreateXrgPaint2D_sub(D_00A57AD8, 113);
    _init_mode_left(pHelp);
    pHelp->bxxCount = 0;

    for (archive_index = 0; archive_index < 19; archive_index++) {
        file_name = s_apszFileTbl[archive_index];
        if (archive_index >= 20) {
            assert_prog(D_00A57AF0, D_00A57AD8, 121);
        }

        strcpy(pHelp->archives[archive_index].archiveName, file_name);
        pHelp->archives[archive_index].archive =
            LoadRgBxx_sub(file_name, D_00A57AD8, 123);
        pHelp->bxxCount++;
    }

    for (archive_index = 0; archive_index < 62; archive_index++) {
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
                RgWarn(D_00A57B00, D_00A57AD8, 148, picture_name,
                       archive_name);
            }
        } else {
            RgWarn(D_00A57B20, D_00A57AD8, 151, archive_name);
        }
        pHelp->pics[archive_index] = picture;
    }
}

static void _DestructHelp(RgHelp *pHelp)
{
    unsigned int archive_index;

    if (pHelp == 0) {
        assert_prog(D_00A57AC8, D_00A57AD8, 160);
    }

    DisposeXrgPaint2D_sub(pHelp->paintContext, D_00A57AD8, 162);
    for (archive_index = 0; archive_index < (unsigned int)pHelp->bxxCount;
         archive_index++) {
        DisposeRgBxx_sub(pHelp->archives[archive_index].archive, D_00A57AD8,
                         165);
    }
}

RgHelp *CreateRgHelp(void)
{
    RgHelp *pHelp;

    pHelp = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgHelp), D_00A57AD8, 172);
    _InitHelp(pHelp);
    return pHelp;
}

void DisposeRgHelp(RgHelp *pHelp)
{
    if (pHelp == 0) {
        assert_prog(D_00A57AC8, D_00A57AD8, 179);
    }
    _DestructHelp(pHelp);
    RgHeapFree(InstanceOfRgHeap(), pHelp, D_00A57AD8, 181);
}

int RgHelpIsEnd(RgHelp *pHelp)
{
    if (pHelp == 0) {
        assert_prog(D_00A57AC8, D_00A57AD8, 191);
    }
    return pHelp->ended;
}

static void _set_global_blight(float blight);
static void _control_mode_left(RgHelp *pHelp);
extern int XrgPadIsBatu(void);
extern int XrgPadIsStart(void);
extern void XrgPadSetID(int id);
extern void XrgSoundSystemCancel(void);

void RgHelpPassTime(RgHelp *pHelp, float deltaTime)
{
    unsigned int phase;

    if (pHelp == 0) {
        assert_prog(D_00A57AC8, D_00A57AD8, 203);
    }

    phase = pHelp->phase;
    pHelp->blinkTimer += deltaTime;

    switch (phase) {
    case 2:
        pHelp->ended = 1;
        return;

    case 0:
        _set_global_blight(1.0f);
        _control_mode_left(pHelp);
        XrgPadSetID(0);
        if (XrgPadIsBatu() || XrgPadIsStart()) {
            pHelp->phase = 1;
            pHelp->countdown = 0.5f;
            XrgSoundSystemCancel();
        }
        return;

    case 1:
        pHelp->countdown -= deltaTime;
        _set_global_blight(2.0f * pHelp->countdown);
        if (pHelp->countdown <= 0.0f) {
            pHelp->phase = 2;
        }
        break;
    }
}

static void _paint_mode_left(RgHelp *pHelp);
extern void XrgPaint2DFlush(void *paintContext);
static void _paint_bg_lower(RgHelp *pHelp);
static void _paint_bg_higher(RgHelp *pHelp);

void RgHelpDisp(RgHelp *pHelp)
{
    if (pHelp == 0) {
        assert_prog(D_00A57AC8, D_00A57AD8, 257);
    }

    if (pHelp->phase != 1) {
        if (pHelp->phase == 0) {
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
    pHelp->phase = 0;
    pHelp->cursor = 0;
    pHelp->blinkTimer = 0.0f;
}

/*
 * s_anMoveTbl_2 (ov12:0x00a50760, size 0x100) is a 16-entry table indexed by
 * the current cursor position; the member for whichever Xrg pad direction
 * the player just pressed gives the cursor value that press moves to.
 */
typedef struct RgHelpMoveEntry {
    int up;
    int down;
    int left;
    int right;
} RgHelpMoveEntry;

extern int XrgPadIsUp(void);
extern int XrgPadIsDown(void);
extern int XrgPadIsLeft(void);
extern int XrgPadIsRight(void);
extern int XrgPadIsMaru(void);
extern void XrgSoundSystemCursor(void);

static void _control_mode_left(RgHelp *pHelp)
{
    static RgHelpMoveEntry s_anMoveTbl[16] = {
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
        if ((unsigned int) next >= 0x10) {
            next = 0;
        }
        if (pHelp->cursor != next) {
            XrgSoundSystemCursor();
        }
        pHelp->cursor = next;
    }
    if (pHelp->blinkTimer > 2.0f) {
        pHelp->blinkTimer = 0.0f;
    }
}

static void _color(void *paintContext, const int *overrideColor)
{
    int color[4];

    if (overrideColor != 0) {
        color[0] = (overrideColor[0] * s_aColor[0]) >> 7;
        color[1] = (overrideColor[1] * s_aColor[1]) >> 7;
        color[2] = (overrideColor[2] * s_aColor[2]) >> 7;
        color[3] = (overrideColor[3] * s_aColor[3]) >> 7;
    } else {
        color[0] = (s_aColor[0] << 7) >> 7;
        color[1] = (s_aColor[1] << 7) >> 7;
        color[2] = (s_aColor[2] << 7) >> 7;
        color[3] = ((s_aColor[3] << 7) - s_aColor[3]) >> 7;
    }

    XrgPaint2DColor(paintContext, color);
}

/*
 * _paint_lin/_paint_add/_paint_sub each hand XrgPaint2DAlpha one of exactly
 * these three values: 0 pastes the source over the destination, 1 adds it,
 * 2 subtracts it.
 */
#define XRG_PAINT2D_BLEND_LINEAR 0
#define XRG_PAINT2D_BLEND_ADD 1
#define XRG_PAINT2D_BLEND_SUB 2

/*
 * XrgPaint2DDrawXYWH's mode word: bit 0x20 takes the drawn extent from the
 * bound picture instead of the width/height arguments (_paint).
 */
#define XRG_PAINT2D_MODE_USE_PIC_SIZE 0x20

extern void XrgPaint2DUseTexture(void *paintContext, RgBxxPic *pic);
extern void XrgPaint2DAlpha(void *paintContext, int blendMode);
extern void XrgPaint2DDrawXYWH(void *paintContext, int mode, int x, int y,
                               int width, int height);
static void _color(void *paintContext, const int *overrideColor);

static void _paint(void *paintContext, RgBxxPic *pic, int blendMode, int x, int y)
{
    if (pic != 0) {
        XrgPaint2DUseTexture(paintContext, pic);
        XrgPaint2DAlpha(paintContext, blendMode);
        _color(paintContext, 0);
        XrgPaint2DDrawXYWH(paintContext, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
    }
}

typedef union RgHelpColor {
    long long halves[2];
    int channels[4];
} RgHelpColor;

const RgHelpColor D_00A57B40 = {.channels = {128, 128, 128, 127}};

static void _paint_b(void *paintContext, RgBxxPic *pic, int blendMode, int x,
                     int y, int factor)
{
    RgHelpColor color;
    int upper_bound;

    color = D_00A57B40;
    if (pic != 0) {
        upper_bound = 128;
        if (factor < 0) {
            factor = 0;
        } else if (upper_bound < factor) {
            factor = upper_bound;
        }

        color.channels[0] = color.channels[0] * factor >> 7;
        color.channels[1] = color.channels[1] * factor >> 7;
        color.channels[2] = color.channels[2] * factor >> 7;
        color.channels[3] = color.channels[3] * factor >> 7;
        XrgPaint2DUseTexture(paintContext, pic);
        _color(paintContext, color.channels);
        XrgPaint2DAlpha(paintContext, blendMode);
        XrgPaint2DDrawXYWH(paintContext, XRG_PAINT2D_MODE_USE_PIC_SIZE,
                           x, y, 0, 0);
    }
}

static void _paint_lin(void *paintContext, RgBxxPic *pic, int x, int y)
{
    _paint(paintContext, pic, XRG_PAINT2D_BLEND_LINEAR, x, y);
}

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

extern void XrgPaint2DSetUVOffset(void *paintContext, int u, int v);
extern void XrgPaint2DSetUVSize(void *paintContext, int width, int height);

static void _paint_lin_uvwh(void *paintContext, RgBxxPic *pic, int x, int y,
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

static void _paint_add(void *paintContext, RgBxxPic *pic, int x, int y)
{
    _paint(paintContext, pic, XRG_PAINT2D_BLEND_ADD, x, y);
}

static void _paint_sub(void *paintContext, RgBxxPic *pic, int x, int y)
{
    _paint(paintContext, pic, XRG_PAINT2D_BLEND_SUB, x, y);
}

static void _paint_bg_lower(RgHelp *pHelp)
{
    void *paintContext;
    RgBxxPic *pic;

    paintContext = pHelp->paintContext;
    _paint(paintContext, pHelp->pics[60], 4, 0, 0x10);
    pic = pHelp->pics[61];
    _paint(paintContext, pic, 0, 0, 0x1D0 - pic->height);
}

static void _paint_lin(void *paintContext, RgBxxPic *pic, int x, int y);

static void _paint_bg_higher(RgHelp *pHelp)
{
    _paint_lin(pHelp->paintContext, pHelp->pics[46], 0, 0x168);
}

typedef union RgHelpAlignedUvRect {
    RgHelpUvRect rect;
    long long halves[2];
} RgHelpAlignedUvRect;

const RgHelpAlignedUvRect D_00A57B50 = {{0, 0, 432, 160}};
const RgHelpAlignedUvRect D_00A57B60 = {{80, 0, 432, 160}};

static void _paint_mode_left(RgHelp *pHelp)
{
    RgHelpAlignedUvRect upper_crop;
    RgHelpAlignedUvRect side_crop;
    RgBxxPic **pictures;
    void *paintContext;

    pictures = pHelp->pics;
    paintContext = pHelp->paintContext;
    switch ((unsigned int)pHelp->cursor) {
    case 0:
        _paint_b(paintContext, pictures[13], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[25], 0, 176);
        _paint_b(paintContext, pictures[58], 0, 0, 360, 100);
        _paint_lin(paintContext, pictures[45], 0, 360);
        break;
    case 1:
        _paint_b(paintContext, pictures[14], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[26], 0, 176);
        _paint_b(paintContext, pictures[58], 0, 0, 360, 100);
        _paint_lin(paintContext, pictures[45], 0, 360);
        break;
    case 2:
        _paint_b(paintContext, pictures[15], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[27], 0, 176);
        _paint_b(paintContext, pictures[58], 0, 0, 360, 100);
        _paint_lin(paintContext, pictures[45], 0, 360);
        break;
    case 3:
        _paint_b(paintContext, pictures[16], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[28], 0, 176);
        _paint_b(paintContext, pictures[58], 0, 0, 360, 100);
        _paint_lin(paintContext, pictures[45], 0, 360);
        break;
    case 4:
        _paint_b(paintContext, pictures[17], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[29], 0, 176);
        _paint_b(paintContext, pictures[58], 0, 0, 360, 100);
        _paint_lin(paintContext, pictures[45], 0, 360);
        break;
    case 5:
        _paint_b(paintContext, pictures[18], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[31], 0, 176);
        _paint_b(paintContext, pictures[58], 0, 0, 360, 100);
        _paint_lin(paintContext, pictures[45], 0, 360);
        break;
    case 6:
        _paint_b(paintContext, pictures[19], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[30], 0, 176);
        _paint_b(paintContext, pictures[58], 0, 0, 360, 100);
        _paint_lin(paintContext, pictures[45], 0, 360);
        break;
    case 7:
        _paint_b(paintContext, pictures[20], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[32], 0, 176);
        _paint_b(paintContext, pictures[58], 0, 0, 360, 100);
        _paint_lin(paintContext, pictures[45], 0, 360);
        break;
    case 8:
        _paint_b(paintContext, pictures[21], 0, 8, -16, 100);
        if (pHelp->blinkTimer < 1.0f) {
            _paint_lin(paintContext, pictures[33], 0, 176);
            _paint_b(paintContext, pictures[59], 0, 0, 360, 100);
        } else {
            _paint_lin(paintContext, pictures[34], 0, 176);
            _paint_b(paintContext, pictures[47], 0, 0, 360, 100);
        }
        _paint_lin(paintContext, pictures[43], 0, 360);
        break;
    case 9:
        _paint_b(paintContext, pictures[4], 0, 8, -16, 100);
        _paint_lin(paintContext, pictures[35], 0, 176);
        _paint_b(paintContext, pictures[48], 0, 0, 360, 100);
        _paint_lin(paintContext, pictures[45], 0, 360);
        break;
    case 10:
        _paint_b(paintContext, pictures[5], 0, 8, -16, 100);
        if (pHelp->blinkTimer < 1.0f) {
            _paint_lin(paintContext, pictures[36], 0, 176);
            _paint_b(paintContext, pictures[49], 0, 0, 360, 100);
        } else {
            _paint_lin(paintContext, pictures[37], 0, 176);
            _paint_lin(paintContext, pictures[50], 0, 360);
            _paint_b(paintContext, pictures[50], 0, 0, 360, 100);
        }
        _paint_lin(paintContext, pictures[42], 0, 360);
        break;
    case 11:
        _paint_b(paintContext, pictures[6], 0, 8, -16, 100);
        if (pHelp->blinkTimer < 1.0f) {
            _paint_lin(paintContext, pictures[38], 0, 176);
            _paint_b(paintContext, pictures[51], 0, 0, 360, 100);
        } else {
            _paint_lin(paintContext, pictures[39], 0, 176);
            _paint_b(paintContext, pictures[52], 0, 0, 360, 100);
        }
        _paint_lin(paintContext, pictures[44], 0, 360);
        break;
    case 12:
        _paint_b(paintContext, pictures[7], 0, 8, -16, 100);
        if (pHelp->blinkTimer < 1.0f) {
            _paint_lin(paintContext, pictures[40], 0, 176);
            _paint_b(paintContext, pictures[53], 0, 0, 360, 100);
        } else {
            _paint_lin(paintContext, pictures[41], 0, 176);
            _paint_b(paintContext, pictures[54], 0, 0, 360, 100);
        }
        _paint_lin(paintContext, pictures[43], 0, 360);
        break;
    case 13:
        _paint_b(paintContext, pictures[10], 0, -8, 18, 100);
        _paint_lin(paintContext, pictures[22], 0, 176);
        _paint_b(paintContext, pictures[55], 0, 0, 360, 100);
        _paint_lin(paintContext, pictures[45], 0, 360);
        break;
    case 14:
        _paint_b(paintContext, pictures[11], 0, -8, 18, 100);
        _paint_lin(paintContext, pictures[23], 0, 176);
        _paint_b(paintContext, pictures[56], 0, 0, 360, 100);
        _paint_lin(paintContext, pictures[45], 0, 360);
        break;
    case 15:
        _paint_b(paintContext, pictures[12], 0, -8, 18, 100);
        _paint_lin(paintContext, pictures[24], 0, 176);
        _paint_b(paintContext, pictures[57], 0, 0, 360, 100);
        _paint_lin(paintContext, pictures[45], 0, 360);
        break;
    }

    upper_crop = D_00A57B50;
    _paint_lin_uvwh(paintContext, pictures[0], 8, -16, &upper_crop.rect);
    _paint_lin_uvwh(paintContext, pictures[2], 8, -16, &upper_crop.rect);
    side_crop = D_00A57B60;
    _paint_lin_uvwh(paintContext, pictures[1], -8, 18, &side_crop.rect);
    _paint_lin_uvwh(paintContext, pictures[3], -8, 18, &side_crop.rect);
}

static void _paint_b(void *paintContext, RgBxxPic *pic, int blendMode, int x,
                     int y, int factor);

static void _paint_mode_right(RgHelp *pHelp)
{
    RgBxxPic **pictures = pHelp->pics;
    void *paintContext = pHelp->paintContext;

    switch ((unsigned int)pHelp->cursor) {
    case 0:
        _paint_b(paintContext, pictures[9], 0, 0, 0, 100);
        break;
    case 1:
        _paint_b(paintContext, pictures[10], 0, 0, 0, 100);
        _paint_lin(paintContext, pictures[22], 0, 176);
        _paint(paintContext, pictures[55], 0, 0, 360);
        break;
    case 2:
        _paint_b(paintContext, pictures[11], 0, 0, 0, 100);
        _paint_lin(paintContext, pictures[23], 0, 176);
        _paint(paintContext, pictures[56], 0, 0, 360);
        break;
    case 3:
        _paint_b(paintContext, pictures[12], 0, 0, 0, 100);
        _paint_lin(paintContext, pictures[24], 0, 176);
        _paint(paintContext, pictures[57], 0, 0, 360);
        break;
    }

    _paint_lin(paintContext, pictures[3], 0, 0);
    _paint_lin(paintContext, pictures[1], 0, 0);
    _paint_lin(paintContext, pictures[45], 0, 360);
}



const char D_00A57550[16] = "help_bg_pad.bxx";

const char D_00A57560[24] = "help_bg_grad.bxx";

const char D_00A57578[24] = "help_03_03_low.bxx";

const char D_00A57590[24] = "help_03_03_high2.bxx";

const char D_00A575A8[24] = "help_03_03_high.bxx";

const char D_00A575C0[16] = "help_03_02.bxx";

const char D_00A575D0[16] = "help_03_01.bxx";

const char D_00A575E0[24] = "help_02_03_mid2.bxx";

const char D_00A575F8[24] = "help_02_03_mid.bxx";

const char D_00A57610[24] = "help_02_03_09.bxx";

const char D_00A57628[24] = "help_02_03_low2.bxx";

const char D_00A57640[24] = "help_02_03_low.bxx";

const char D_00A57658[24] = "help_02_03_high.bxx";

const char D_00A57670[24] = "help_01_03_mid.bxx";

const char D_00A57688[24] = "help_01_03_low.bxx";

const char D_00A576A0[24] = "help_01_03_high.bxx";

const char D_00A576B8[24] = "help_01_03_high2.bxx";

const char D_00A576D0[16] = "help_01_02.bxx";

const char D_00A576E0[16] = "help_01_01.bxx";

const char D_00A576F0[16] = "bg_ctrlr";

const char D_00A57700[8] = "bg_grad";

const char D_00A57708[16] = "03_03_09";

const char D_00A57718[16] = "03_03_01";

const char D_00A57728[16] = "03_03_22";

const char D_00A57738[16] = "03_03_21";

const char D_00A57748[16] = "03_03_20";

const char D_00A57758[16] = "03_03_17";

const char D_00A57768[16] = "03_03_16";

const char D_00A57778[16] = "03_03_15";

const char D_00A57788[16] = "03_03_14";

const char D_00A57798[16] = "03_03_13";

const char D_00A577A8[16] = "03_03_12";

const char D_00A577B8[16] = "03_03_11";

const char D_00A577C8[16] = "03_03_10";

const char D_00A577D8[16] = "03_02_01";

const char D_00A577E8[16] = "03_01_04";

const char D_00A577F8[16] = "03_01_03";

const char D_00A57808[16] = "03_01_02";

const char D_00A57818[16] = "03_01_01";

const char D_00A57828[16] = "02_03_17";

const char D_00A57838[16] = "02_03_16";

const char D_00A57848[16] = "02_03_15";

const char D_00A57858[16] = "02_03_14";

const char D_00A57868[16] = "02_03_13";

const char D_00A57878[16] = "02_03_12";

const char D_00A57888[16] = "02_03_11";

const char D_00A57898[16] = "02_03_10";

const char D_00A578A8[16] = "02_03_09_";

const char D_00A578B8[16] = "02_03_08";

const char D_00A578C8[16] = "02_03_07";

const char D_00A578D8[16] = "02_03_06";

const char D_00A578E8[16] = "02_03_05";

const char D_00A578F8[16] = "02_03_04";

const char D_00A57908[16] = "02_03_03";

const char D_00A57918[16] = "02_03_02";

const char D_00A57928[16] = "02_03_01";

const char D_00A57938[16] = "02_03_22";

const char D_00A57948[16] = "02_03_21";

const char D_00A57958[16] = "02_03_20";

const char D_00A57968[16] = "01_03_09";

const char D_00A57978[16] = "01_03_08";

const char D_00A57988[16] = "01_03_07";

const char D_00A57998[16] = "01_03_06";

const char D_00A579A8[16] = "01_03_05";

const char D_00A579B8[16] = "01_03_04";

const char D_00A579C8[16] = "01_03_03";

const char D_00A579D8[16] = "01_03_02";

const char D_00A579E8[16] = "01_03_01";

const char D_00A579F8[16] = "01_03_22";

const char D_00A57A08[16] = "01_03_21";

const char D_00A57A18[16] = "01_03_20";

const char D_00A57A28[16] = "01_03_19";

const char D_00A57A38[16] = "01_03_18";

const char D_00A57A48[16] = "01_03_16";

const char D_00A57A58[16] = "01_03_14";

const char D_00A57A68[16] = "01_03_12";

const char D_00A57A78[16] = "01_03_11";

const char D_00A57A88[16] = "01_02_02";

const char D_00A57A98[16] = "01_02_01";

const char D_00A57AA8[16] = "01_01_02";

const char D_00A57AB8[16] = "01_01_01";
