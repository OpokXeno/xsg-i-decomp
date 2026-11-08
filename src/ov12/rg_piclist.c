/*
 * OV12 original TU 75: 0x00a40008..0x00a420e0 (46 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_piclist.h"
#include "main/xgl_pad.h"
#include "ov12/rg_debug_flags.h"
#include "ov12/rg_bxx.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_error.h"
#include "ov12/rg_heap.h"
#include "ov12/xrg_hostfile.h"
#include "main/xgl_font.h"

/*
 * The debug editor object: bxx is the archive, list the edited picture list,
 * mode the editor state RgPicDebugControl dispatches on, index the selected
 * picture (-1 for none) and picId the archive picture being browsed.
 * CreateRgPicDebug allocates sizeof(RgPicDebug) == 0x14 bytes.
 */
typedef struct RgPicDebug {
    RgBxx *bxx;      /* 0x00 */
    RgPicList *list; /* 0x04 */
    int mode;        /* 0x08 */
    int index;       /* 0x0C */
    int picId;       /* 0x10 */
} RgPicDebug;

/* The fixed on-disk size of a serialized picture record. */
#define RG_PIC_BINARY_SIZE 0x100
/* The fixed on-disk header size of a serialized picture list. */
#define RG_PIC_LIST_BINARY_SIZE 0x40

/* RgPic.alpha values: the blend mode the picture draws with. */
#define RG_PIC_ALPHA_ADD 1
#define RG_PIC_ALPHA_LINEAR 2
#define RG_PIC_ALPHA_ALPHA 3
#define RG_PIC_ALPHA_COUNT 4

/* Default color of a picture: RgPicColor components. */
#define RG_PIC_DEFAULT_RGB 128
#define RG_PIC_DEFAULT_ALPHA 127

/* Placement of a picture added from the archive browser. */
#define RG_PIC_DEBUG_START_X 64
#define RG_PIC_DEBUG_START_Y 128
#define RG_PIC_DEBUG_STICK_THRESHOLD 80

/* Colors of the debug rectangle cycle through 0..32 in steps of 8. */
#define RG_PIC_NONTEX_COLOR_STEP 8
#define RG_PIC_NONTEX_COLOR_LIMIT 33

/* RgPicDebug.mode: the editor states RgPicDebugControl dispatches on. */
#define RG_PIC_DEBUG_BROWSE 0   /* pick an archive picture to add */
#define RG_PIC_DEBUG_EDIT 1     /* move/restyle the selected picture */
#define RG_PIC_DEBUG_DELETE 2   /* confirm deleting the selected picture */
#define RG_PIC_DEBUG_SAVE 3     /* confirm saving the list */
#define RG_PIC_DEBUG_LOAD 4     /* confirm loading the list */
#define RG_PIC_DEBUG_OFFSET 5   /* move the whole list */

/* L1 moves in steps of 8 and snaps both coordinates to that grid. */
#define PIC_DEBUG_COARSE_STEP 8


/* Gray level of the next rectangle drawn by _draw_nontex. */
static int s_nColor_0;
extern XglPadRecord PadData[2];

/* Debug text strings, declared without a known array size. */
extern const char picListDebugEditHelpText[];
extern const char picListDebugPicNumberFormat[];
extern const char picListDebugPicPositionFormat[];
extern const char picListDebugSelectHelpText[];
extern const char picListDebugTextureNameFormat[];
extern const char picListDebugDeleteHelpText[];
extern const char picListDebugSavePromptText[];
extern const char picListDebugConfirmCancelHelpText[];
extern const char picListDebugLoadPromptText[];
extern const char picListDebugMoveImageHintText[];
extern const char picListDebugOffsetFormat[];

RgPic *CreateRgPic(RgBxx *bxx, const char *pszName);
RgPic *CreateRgPicFromBinary(RgBxx *bxx, void *buffer);
void RgPicWrite(RgPic *pic, int handle);
void RgPicListWrite(RgPicList *pList, int handle);

const char picListAssertPicNotNil[] = "pPic != NIL";

const char picListSourcePath[] = "../rg_piclist.euc.c";

const char picListAssertBxxNotNil[] = "pBxx != NIL";

const char picListAssertNameNotNil[] = "pszName != NIL";

const char picListAssertPaintNotNil[] = "pPaint != NIL";

const char picListAssertListNotNil[] = "pList != NIL";

const char picListAssertNumBelowPicMax[] = "pList->m_uNum < PIC_MAX";

const char picListAssertDebugNotNil[] = "pDebug != NIL";

/* Draws the named archive texture with additive blending, if it exists. */
static void _draw_add(XrgPaint2D *paintId, RgBxx *bxx, const char *picName,
                      int x, int y)
{
    RgBxxPic *texture;

    texture = RgBxxGetPic(bxx, picName);
    if (texture != 0) {
        XrgPaint2DAlpha(paintId, XRG_PAINT2D_BLEND_ADD);
        XrgPaint2DUseTexture(paintId, texture);
        XrgPaint2DDrawXYWH(paintId, XRG_PAINT2D_MODE_PIC_SIZE, x, y, 0, 0);
    }
}

/* Draws the named archive texture with linear blending, if it exists. */
static void _draw_linear(XrgPaint2D *paintId, RgBxx *bxx, const char *picName,
                         int x, int y)
{
    RgBxxPic *texture;

    texture = RgBxxGetPic(bxx, picName);
    if (texture != 0) {
        XrgPaint2DAlpha(paintId, XRG_PAINT2D_BLEND_LINEAR);
        XrgPaint2DUseTexture(paintId, texture);
        XrgPaint2DDrawXYWH(paintId, XRG_PAINT2D_MODE_PIC_SIZE, x, y, 0, 0);
    }
}

/* Draws a flat gray rectangle; the gray level steps up on each call and wraps. */
static void _draw_nontex(XrgPaint2D *paintId, int x, int y, int width, int height)
{
    RgPicColor color;
    RgPicColor fill;
    int level;

    memset(&fill, 0, sizeof(fill));
    level = s_nColor_0;
    fill.r = level;
    fill.g = level;
    fill.b = level;
    __builtin_memcpy(&color, &fill, sizeof(color));
    s_nColor_0 = level + RG_PIC_NONTEX_COLOR_STEP;
    if (s_nColor_0 >= RG_PIC_NONTEX_COLOR_LIMIT)
        s_nColor_0 = 0;
    XrgPaint2DColor(paintId, &color);
    XrgPaint2DAlpha(paintId, XRG_PAINT2D_BLEND_ADD);
    XrgPaint2DDrawXYWH(paintId, 0, x, y, width, height);
}

int RgPicGetWidth(RgPic *pic)
{
    int width;

    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 81);

    width = 0;
    if (pic->tex != 0)
        width = pic->tex->width;
    return width;
}

int RgPicGetHeight(RgPic *pic)
{
    int height;

    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 93);

    height = 0;
    if (pic->tex != 0)
        height = pic->tex->height;
    return height;
}

/* Writes a picture record to the host handle when debug flag 3 is set. */
void RgPicWrite(RgPic *pic, int handle)
{
    unsigned char buffer[0x400];
    RgDebugFlags *debugFlags;
    unsigned char *cursor;
    unsigned char *src;
    int remaining;

    debugFlags = InstanceOfRgDebugFlags();
    if (debugFlags->flags[3] == 0)
        return;
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 110);

    /*
     * The picture record is copied through a plain byte pointer, so the
     * moves stay unaligned (the same way RgPicListRead copies its header).
     */
    src = (unsigned char *) pic;
    __builtin_memcpy(buffer, src, 32);
    __builtin_memcpy(&buffer[32], src + 36, 4);
    __builtin_memcpy(&buffer[36], src + 40, 4);
    __builtin_memcpy(&buffer[40], src + 44, 4);
    __builtin_memcpy(&buffer[44], src + 48, 4);
    __builtin_memcpy(&buffer[48], src + 52, 4);
    cursor = &buffer[52];
    remaining = 203;
    do {
        *cursor = 0;
        cursor++;
    } while (remaining-- > 0);
    XrgHostWrite(handle, buffer, RG_PIC_BINARY_SIZE);
}

int RgPicBinarySize(void)
{
    return RG_PIC_BINARY_SIZE;
}

const char picListAssertBufferNotNil[] = "pBuf != NIL";

/* Creates a picture from a serialized record, or returns 0 when its texture is missing. */
RgPic *CreateRgPicFromBinary(RgBxx *bxx, void *buffer)
{
    unsigned char *src;
    RgPic *pic;
    RgBxxPic *tex;

    src = buffer;
    pic = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgPic), picListSourcePath, 136);
    if (bxx == 0)
        assert_prog(picListAssertBxxNotNil, picListSourcePath, 139);
    if (buffer == 0)
        assert_prog(picListAssertBufferNotNil, picListSourcePath, 140);
    __builtin_memcpy(pic->name, src, 32);
    src += 32;
    __builtin_memcpy(&pic->x, src, 4);
    src += 4;
    __builtin_memcpy(&pic->y, src, 4);
    src += 4;
    __builtin_memcpy(&pic->alpha, src, 4);
    src += 4;
    __builtin_memcpy(&pic->dark, src, 4);
    src += 4;
    __builtin_memcpy(&pic->notDraw, src, 4);
    tex = RgBxxGetPic(bxx, pic->name);
    pic->tex = tex;
    pic->ofsY = 0;
    pic->ofsX = 0;
    pic->color.b = RG_PIC_DEFAULT_RGB;
    pic->color.g = RG_PIC_DEFAULT_RGB;
    pic->color.r = RG_PIC_DEFAULT_RGB;
    pic->color.a = RG_PIC_DEFAULT_ALPHA;
    if (tex == 0) {
        RgHeapFree(InstanceOfRgHeap(), pic, picListSourcePath, 153);
        pic = 0;
    }
    return pic;
}

/* Reads one serialized picture from the host handle, or returns 0 when debug flag 3 is clear. */
RgPic *RgPicRead(int handle, RgBxx *bxx)
{
    unsigned char buffer[0x400];
    RgDebugFlags *debugFlags;

    debugFlags = InstanceOfRgDebugFlags();
    if (debugFlags->flags[3] == 0)
        return 0;
    if (bxx == 0)
        assert_prog(picListAssertBxxNotNil, picListSourcePath, 165);
    XrgHostRead(handle, buffer, RgPicBinarySize());
    return CreateRgPicFromBinary(bxx, buffer);
}

const char picListAssertPicDataNotNil[] = "pPic->m_pPic != NIL";

/* Creates a picture that draws the named archive texture with default placement and color. */
RgPic *CreateRgPic(RgBxx *bxx, const char *pszName)
{
    RgPic *pic;

    pic = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgPic), picListSourcePath, 177);
    if (bxx == 0)
        assert_prog(picListAssertBxxNotNil, picListSourcePath, 178);
    if (pszName == 0)
        assert_prog(picListAssertNameNotNil, picListSourcePath, 179);

    pic->tex = RgBxxGetPic(bxx, pszName);
    if (pic->tex == 0)
        assert_prog(picListAssertPicDataNotNil, picListSourcePath, 181);
    pic->alpha = RG_PIC_ALPHA_LINEAR;
    pic->color.a = RG_PIC_DEFAULT_ALPHA;
    pic->x = 0;
    pic->y = 0;
    pic->dark = 0;
    pic->notDraw = 0;
    pic->ofsY = 0;
    pic->ofsX = 0;
    pic->color.b = RG_PIC_DEFAULT_RGB;
    pic->color.g = RG_PIC_DEFAULT_RGB;
    pic->color.r = RG_PIC_DEFAULT_RGB;
    strcpy(pic->name, pszName);
    return pic;
}

void DisposeRgPic(RgPic *pic)
{
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 197);
    RgHeapFree(InstanceOfRgHeap(), pic, picListSourcePath, 198);
}

char *RgPicGetName(RgPic *pic)
{
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 204);
    return pic->name;
}

int RgPicGetAlpha(RgPic *pic)
{
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 211);
    return pic->alpha;
}

void RgPicSetAlpha(RgPic *pic, int alpha)
{
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 218);
    pic->alpha = alpha;
}

int RgPicGetX(RgPic *pic)
{
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 225);
    return pic->x;
}

int RgPicGetY(RgPic *pic)
{
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 230);
    return pic->y;
}

void RgPicSetOfsXY(RgPic *pic, int ofsX, int ofsY)
{
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 237);
    pic->ofsX = ofsX;
    pic->ofsY = ofsY;
}

void RgPicSetColor(RgPic *pic, const RgPicColor *color)
{
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 245);
    pic->color.r = color->r;
    pic->color.g = color->g;
    pic->color.b = color->b;
    pic->color.a = color->a;
}

RgBxxPic *RgPicGetTex(RgPic *pic)
{
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 255);
    return pic->tex;
}

void RgPicSetPos(RgPic *pic, int x, int y)
{
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 262);
    pic->x = x;
    pic->y = y;
}

void RgPicMove(RgPic *pic, int dx, int dy)
{
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 270);
    pic->x += dx;
    pic->y += dy;
}

/*
 * The alpha field selects the blend mode: 1 adds, 2 is linear and 3 is alpha
 * blending. Any other value keeps the paint object's current blend mode.
 * A dark picture halves each color component before XrgPaint2DColor.
 */
void RgPicDraw(RgPic *pic, XrgPaint2D *paint)
{
    RgPicColor darkColor;

    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 280);
    if (paint == 0)
        assert_prog(picListAssertPaintNotNil, picListSourcePath, 281);

    if (pic->notDraw == 0) {
        switch ((unsigned int) pic->alpha) {
        case RG_PIC_ALPHA_ADD:
            XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_ADD);
            break;
        case RG_PIC_ALPHA_ALPHA:
            XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_ALPHA);
            break;
        case RG_PIC_ALPHA_LINEAR:
            XrgPaint2DAlpha(paint, XRG_PAINT2D_BLEND_LINEAR);
            break;
        }
        XrgPaint2DUseTexture(paint, pic->tex);
        if (pic->dark != 0) {
            darkColor.r = pic->color.r / 2;
            darkColor.g = pic->color.g / 2;
            darkColor.b = pic->color.b / 2;
            darkColor.a = pic->color.a / 2;
            XrgPaint2DColor(paint, &darkColor);
        } else {
            XrgPaint2DColor(paint, &pic->color);
        }
        XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_PIC_SIZE,
                            pic->x + pic->ofsX, pic->y + pic->ofsY, 0, 0);
    }
}

void RgPicSetDark(RgPic *pic, int dark)
{
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 318);
    pic->dark = dark;
}

void RgPicNotDraw(RgPic *pic, int notDraw)
{
    if (pic == 0)
        assert_prog(picListAssertPicNotNil, picListSourcePath, 326);
    pic->notDraw = notDraw;
}

/* Creates an empty picture list for the archive, with drawing enabled. */
RgPicList *CreateRgPicList(RgBxx *pBxx)
{
    RgPicList *pList;

    pList = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgPicList), picListSourcePath, 347);
    if (pBxx == 0)
        assert_prog(picListAssertBxxNotNil, picListSourcePath, 348);
    pList->bxx = pBxx;
    pList->m_uNum = 0;
    pList->ofsX = 0;
    pList->ofsY = 0;
    pList->drawEnable = 1;
    return pList;
}

/* Frees every picture in the list, then the list itself. */
void DisposeRgPicList(RgPicList *pList)
{
    unsigned int i;

    if (pList == 0)
        assert_prog(picListAssertListNotNil, picListSourcePath, 362);
    for (i = 0; i < pList->m_uNum; i++)
        DisposeRgPic(pList->pics[i]);
    RgHeapFree(InstanceOfRgHeap(), pList, picListSourcePath, 365);
}

/* Adds a picture for the named texture at the end of the list and returns it. */
RgPic *RgPicListAddPic(RgPicList *pList, const char *pszName)
{
    RgPic *pic;

    if (pList == 0)
        assert_prog(picListAssertListNotNil, picListSourcePath, 373);
    if (pszName == 0)
        assert_prog(picListAssertNameNotNil, picListSourcePath, 374);
    if (pList->m_uNum >= PIC_MAX)
        assert_prog(picListAssertNumBelowPicMax, picListSourcePath, 375);

    pic = CreateRgPic(pList->bxx, pszName);
    pList->pics[pList->m_uNum++] = pic;
    return pic;
}

/* Removes the picture at index, if any, and shifts the later pictures down. */
void RgPicListDelPic(RgPicList *pList, int index)
{
    unsigned int i;

    if (pList == 0)
        assert_prog(picListAssertListNotNil, picListSourcePath, 385);

    if (index >= 0 && index < pList->m_uNum) {
        DisposeRgPic(pList->pics[index]);
        for (i = (unsigned int) index; i < pList->m_uNum - 1; i++)
            pList->pics[i] = pList->pics[i + 1];
        pList->m_uNum--;
    }
}

RgPic *RgPicListGetPic(RgPicList *pList, int index)
{
    if (pList == 0)
        assert_prog(picListAssertListNotNil, picListSourcePath, 399);

    if (index >= 0 && index < pList->m_uNum) {
        return pList->pics[index];
    }
    return 0;
}

unsigned int RgPicListGetSize(RgPicList *pList)
{
    if (pList == 0)
        assert_prog(picListAssertListNotNil, picListSourcePath, 410);
    return pList->m_uNum;
}

/* Draws every picture in the list, offset by the list offset, when drawing is enabled. */
void RgPicListDraw(RgPicList *pList, XrgPaint2D *paint)
{
    unsigned int i;

    if (pList == 0)
        assert_prog(picListAssertListNotNil, picListSourcePath, 419);

    if (pList->drawEnable != 0) {
        XrgPaint2DOffset2DDot(paint, pList->ofsX, pList->ofsY);
        for (i = 0; i < pList->m_uNum; i++)
            RgPicDraw(pList->pics[i], paint);
    }
}

void RgPicListSetDrawEnable(RgPicList *pList, int enable)
{
    if (pList == 0)
        assert_prog(picListAssertListNotNil, picListSourcePath, 431);
    pList->drawEnable = enable;
}

static int _BinarySizePicList(void)
{
    return RG_PIC_LIST_BINARY_SIZE;
}

/* Writes the list header, then each picture record, when debug flag 3 is set. */
void RgPicListWrite(RgPicList *pList, int handle)
{
    unsigned char buffer[0x400];
    RgDebugFlags *debugFlags;
    unsigned char *dst;
    unsigned char *cursor;
    int reserved;
    int size;
    int remaining;
    unsigned int i;

    debugFlags = InstanceOfRgDebugFlags();
    if (debugFlags->flags[3] == 0)
        return;
    reserved = 0;
    if (pList == 0)
        assert_prog(picListAssertListNotNil, picListSourcePath, 452);

    dst = buffer;
    __builtin_memcpy(dst, &pList->m_uNum, 4);
    __builtin_memcpy(dst + 4, &pList->ofsX, 4);
    __builtin_memcpy(dst + 8, &pList->ofsY, 4);
    __builtin_memcpy(dst + 12, &reserved, 4);

    cursor = &buffer[16];
    size = _BinarySizePicList();
    remaining = size - 17;
    if (size - 16 > 0) {
        do {
            *cursor = 0;
            cursor++;
        } while (remaining-- > 0);
    }
    XrgHostWrite(handle, buffer, _BinarySizePicList());
    for (i = 0; i < pList->m_uNum; i++)
        RgPicWrite(pList->pics[i], handle);
}

/* Creates a picture list from a serialized list, skipping pictures whose textures are missing. */
RgPicList *CreateRgPicListFromBinary(RgBxx *bxx, void *buffer)
{
    RgPicList *pList;
    unsigned char *cursor;
    unsigned int num;
    int reserved;
    int size;
    int remaining;
    unsigned int i;
    RgPic *pic;

    pList = CreateRgPicList(bxx);
    cursor = buffer;
    __builtin_memcpy(&num, cursor, 4);
    cursor += 4;
    __builtin_memcpy(&pList->ofsX, cursor, 4);
    cursor += 4;
    __builtin_memcpy(&pList->ofsY, cursor, 4);
    cursor += 4;
    __builtin_memcpy(&reserved, cursor, 4);
    cursor += 4;

    size = _BinarySizePicList();
    remaining = size - 17;
    if (size - 16 > 0) {
        do {
            cursor++;
        } while (remaining-- > 0);
    }
    for (i = 0; i < num; i++) {
        pic = CreateRgPicFromBinary(bxx, cursor);
        if (pic != 0) {
            pList->pics[pList->m_uNum++] = pic;
        }
        cursor += RgPicBinarySize();
    }
    return pList;
}

/* Reads a picture list from the host handle, or returns 0 when debug flag 3 is clear. */
RgPicList *RgPicListRead(int handle, RgBxx *bxx)
{
    unsigned char buffer[0x400];
    unsigned int num;
    int reserved;
    RgPicList *pList;
    RgPic *pic;
    unsigned int i;
    unsigned char *src;

    if (InstanceOfRgDebugFlags()->flags[3] != 0) {
        pList = CreateRgPicList(bxx);
        XrgHostRead(handle, buffer, _BinarySizePicList());
        /*
         * The serialized header is four words: picture count, ofsX, ofsY
         * and a reserved word that is copied but never read. They are
         * copied through a plain pointer, so the moves stay unaligned
         * (lwl/lwr).
         */
        src = buffer;
        __builtin_memcpy(&num, src, 4);
        __builtin_memcpy(&pList->ofsX, src + 4, 4);
        __builtin_memcpy(&pList->ofsY, src + 8, 4);
        __builtin_memcpy(&reserved, src + 12, 4);
        for (i = 0; i < num; i++) {
            pic = RgPicRead(handle, bxx);
            if (pic != 0) {
                pList->pics[pList->m_uNum++] = pic;
            }
        }
        return pList;
    }
    return 0;
}

int RgPicListGetOfsX(RgPicList *pList)
{
    if (pList == 0)
        assert_prog(picListAssertListNotNil, picListSourcePath, 528);
    return pList->ofsX;
}

int RgPicListGetOfsY(RgPicList *pList)
{
    if (pList == 0)
        assert_prog(picListAssertListNotNil, picListSourcePath, 534);
    return pList->ofsY;
}

void RgPicListSetOffset(RgPicList *pList, int ofsX, int ofsY)
{
    if (pList == 0)
        assert_prog(picListAssertListNotNil, picListSourcePath, 541);
    pList->ofsX = ofsX;
    pList->ofsY = ofsY;
}

void RgPicListMoveAllPic(RgPicList *pList, int dx, int dy)
{
    unsigned int i;

    if (pList == 0)
        assert_prog(picListAssertListNotNil, picListSourcePath, 550);
    for (i = 0; i < pList->m_uNum; i++)
        RgPicMove(pList->pics[i], dx, dy);
}

RgPicDebug *CreateRgPicDebug(RgBxx *pBxx)
{
    RgPicDebug *pDebug;

    if (pBxx == 0)
        assert_prog(picListAssertBxxNotNil, picListSourcePath, 582);
    pDebug = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgPicDebug), picListSourcePath, 583);
    pDebug->bxx = pBxx;
    pDebug->list = CreateRgPicList(pBxx);
    pDebug->mode = RG_PIC_DEBUG_EDIT;
    pDebug->index = -1;
    pDebug->picId = 0;
    return pDebug;
}

void DisposeRgPicDebug(RgPicDebug *pDebug)
{
    if (pDebug == 0)
        assert_prog(picListAssertDebugNotNil, picListSourcePath, 596);
    DisposeRgPicList(pDebug->list);
    RgHeapFree(InstanceOfRgHeap(), pDebug, picListSourcePath, 598);
}

/*
 * Moves the position by the held d-pad directions. With L1 held the steps are
 * coarse and both coordinates snap to the grid. The held buttons are read at
 * PadData+0x28 (lhu).
 */
void RgPicDebugEditPosition(int *x, int *y)
{
    u16 buttons;
    int step;
    int coarse;

    buttons = PadData[0].buttons;
    coarse = 0;
    step = 1;
    if (buttons & PAD_L1) {
        step = PIC_DEBUG_COARSE_STEP;
        coarse = 1;
    }
    if (buttons & PAD_UP) {
        *y -= step;
    }
    if (buttons & PAD_DOWN) {
        *y += step;
    }
    if (buttons & PAD_LEFT) {
        *x -= step;
    }
    if (buttons & PAD_RIGHT) {
        *x += step;
    }
    if (coarse) {
        *x &= ~(PIC_DEBUG_COARSE_STEP - 1);
        *y &= ~(PIC_DEBUG_COARSE_STEP - 1);
    }
}

const char picListHostPlacementFileName[] = "host0:place2d.bin";

const char picListAssertDebugListNotNil[] = "pDebug->m_pList != NIL";

const char picListUnknownDebugEditModeFormat[] = "unknown debug edit mode %d";

/* Handles the debug editor's pad input for the current mode and updates the list. */
void RgPicDebugControl(RgPicDebug *pDebug)
{
    RgPic *pic;
    RgPic *oldPic;
    RgPic *newPic;
    RgBxxPic *pBxxPic;
    unsigned int alpha;
    int mode;
    int buttons;
    int repeat;
    int stickX;
    int stickY;
    int x;
    int y;
    int dx;
    int dy;
    int ofsX;
    int ofsY;
    int offX;
    int offY;
    int texNum;
    int index;
    int last;
    int picIndex;
    int selected;
    int picId;
    int handle;

    buttons = PadData[0].buttons;
    repeat = PadData[0].repeated;
    stickY = PadData[0].position[1] + PadData[0].position[3];
    stickX = PadData[0].position[0] + PadData[0].position[2];

    if (pDebug == 0)
        assert_prog(picListAssertDebugNotNil, picListSourcePath, 636);

    mode = pDebug->mode;
    if (mode == RG_PIC_DEBUG_EDIT) {
        /* Moves the selected picture, then cycles the selection through the list. */
        pic = RgPicListGetPic(pDebug->list, pDebug->index);
        if (pic != 0) {
            x = RgPicGetX(pic);
            y = RgPicGetY(pic);
            RgPicDebugEditPosition(&x, &y);
            if (repeat & PAD_R1) {
                alpha = RgPicGetAlpha(pic) + 1;
                RgPicSetAlpha(pic, alpha < RG_PIC_ALPHA_COUNT ? alpha : 0);
            }
            RgPicSetPos(pic, x, y);
        }
        if (repeat & PAD_TRIANGLE)
            pDebug->index--;
        if (repeat & PAD_SQUARE)
            pDebug->index++;
        last = RgPicListGetSize(pDebug->list) - 1;
        index = pDebug->index;
        if (index < 0)
            selected = 0;
        else if (index > last)
            selected = last;
        else
            selected = index;
        pDebug->index = selected;
        if (repeat & PAD_CIRCLE)
            pDebug->mode = RG_PIC_DEBUG_BROWSE;
        if (repeat & PAD_CROSS)
            pDebug->mode = RG_PIC_DEBUG_DELETE;
        if ((buttons & (PAD_L1 | PAD_L2)) == (PAD_L1 | PAD_L2))
            pDebug->mode = RG_PIC_DEBUG_SAVE;
        if ((buttons & (PAD_R1 | PAD_R2)) == (PAD_R1 | PAD_R2))
            pDebug->mode = RG_PIC_DEBUG_LOAD;
        if (repeat & PAD_START)
            pDebug->mode = RG_PIC_DEBUG_OFFSET;
    } else if (mode == RG_PIC_DEBUG_BROWSE) {
        /* Steps through the archive textures; CIRCLE adds the one shown. */
        texNum = RgBxxGetTexNum(pDebug->bxx);
        if (texNum == 0)
            pDebug->mode = RG_PIC_DEBUG_EDIT;
        if (repeat & PAD_TRIANGLE)
            pDebug->picId--;
        else if (repeat & PAD_SQUARE)
            pDebug->picId++;
        last = texNum - 1;
        picIndex = pDebug->picId;
        if (picIndex < 0)
            picId = 0;
        else if (picIndex > last)
            picId = last;
        else
            picId = picIndex;
        pDebug->picId = picId;
        if (repeat & PAD_CIRCLE) {
            pBxxPic = RgBxxGetPicID(pDebug->bxx, picId);
            if (pBxxPic != 0) {
                oldPic = RgPicListGetPic(pDebug->list, pDebug->index);
                newPic = RgPicListAddPic(pDebug->list, pBxxPic->name);
                if (newPic != 0) {
                    if (oldPic == 0) {
                        RgPicSetPos(newPic, RG_PIC_DEBUG_START_X, RG_PIC_DEBUG_START_Y);
                    } else {
                        offX = 128 - RgPicGetX(oldPic);
                        offY = 128 - RgPicGetY(oldPic);
                        if (stickX < -RG_PIC_DEBUG_STICK_THRESHOLD) {
                            offX = -RgPicGetWidth(newPic);
                            offY = 0;
                        } else if (stickX >= RG_PIC_DEBUG_STICK_THRESHOLD + 1) {
                            offX = RgPicGetWidth(oldPic);
                            offY = 0;
                        } else if (stickY < -RG_PIC_DEBUG_STICK_THRESHOLD) {
                            offX = 0;
                            offY = -RgPicGetHeight(newPic);
                        } else if (stickY >= RG_PIC_DEBUG_STICK_THRESHOLD + 1) {
                            offX = 0;
                            offY = RgPicGetHeight(oldPic);
                        }
                        RgPicSetPos(newPic, RgPicGetX(oldPic) + offX,
                                    RgPicGetY(oldPic) + offY);
                    }
                }
            }
            pDebug->mode = RG_PIC_DEBUG_EDIT;
            pDebug->index = RgPicListGetSize(pDebug->list) - 1;
        }
        if (repeat & PAD_CROSS)
            pDebug->mode = RG_PIC_DEBUG_EDIT;
    } else if (mode == RG_PIC_DEBUG_DELETE) {
        if (repeat & PAD_CIRCLE) {
            int newIndex;

            RgPicListDelPic(pDebug->list, pDebug->index);
            if (RgPicListGetSize(pDebug->list)) {
                last = RgPicListGetSize(pDebug->list) - 1;
                index = pDebug->index;
                if (0 > index)
                    newIndex = 0;
                else if (last < index)
                    newIndex = last;
                else
                    newIndex = index;
                pDebug->index = newIndex;
            } else {
                pDebug->index = -1;
            }
            pDebug->mode = RG_PIC_DEBUG_EDIT;
        }
        if (repeat & (PAD_START | PAD_CROSS))
            pDebug->mode = RG_PIC_DEBUG_EDIT;
    } else if (mode == RG_PIC_DEBUG_SAVE) {
        if (repeat & PAD_CIRCLE) {
            if (InstanceOfRgDebugFlags()->flags[3] != 0) {
                handle = XrgHostCreateOpen(picListHostPlacementFileName);
                if (handle >= 0) {
                    RgPicListWrite(pDebug->list, handle);
                    XrgHostClose(handle);
                }
            }
            pDebug->mode = RG_PIC_DEBUG_EDIT;
        } else if (repeat & PAD_CROSS) {
            pDebug->mode = RG_PIC_DEBUG_EDIT;
        }
    } else if (mode == RG_PIC_DEBUG_LOAD) {
        if (repeat & PAD_CIRCLE) {
            if (InstanceOfRgDebugFlags()->flags[3] != 0) {
                handle = XrgHostReadOpen(picListHostPlacementFileName);
                if (handle >= 0) {
                    DisposeRgPicList(pDebug->list);
                    pDebug->list = RgPicListRead(handle, pDebug->bxx);
                    if (pDebug->list == 0)
                        assert_prog(picListAssertDebugListNotNil, picListSourcePath, 770);
                    XrgHostClose(handle);
                    pDebug->index = RgPicListGetSize(pDebug->list) - 1;
                }
            }
            pDebug->mode = RG_PIC_DEBUG_EDIT;
        } else if (repeat & PAD_CROSS) {
            pDebug->mode = RG_PIC_DEBUG_EDIT;
        }
    } else if (mode == RG_PIC_DEBUG_OFFSET) {
        /* Moves the whole list: its offset, or every picture while R1 is held. */
        ofsX = RgPicListGetOfsX(pDebug->list);
        ofsY = RgPicListGetOfsY(pDebug->list);
        if (buttons & PAD_R1) {
            dx = 0;
            dy = 0;
            RgPicDebugEditPosition(&dx, &dy);
            RgPicListMoveAllPic(pDebug->list, dx, dy);
        } else {
            RgPicDebugEditPosition(&ofsX, &ofsY);
            RgPicListSetOffset(pDebug->list, ofsX, ofsY);
        }
        if (repeat & PAD_CIRCLE)
            pDebug->mode = RG_PIC_DEBUG_EDIT;
    } else {
        RgError(picListUnknownDebugEditModeFormat, picListSourcePath, 802, mode);
    }
}

/* Draws the debug editor overlay for the current mode, with its help text. */
void RgPicDebugDisp(RgPicDebug *pDebug, XrgPaint2D *paint)
{
    RgPic *pic;
    RgBxxPic *tex;
    RgBxxPic *pBxxPic;
    char *name;
    int x;
    int y;
    int alpha;
    unsigned int size;

    if (pDebug == 0)
        assert_prog(picListAssertDebugNotNil, picListSourcePath, 812);

    switch (pDebug->mode) {
    case RG_PIC_DEBUG_EDIT:
        pic = RgPicListGetPic(pDebug->list, pDebug->index);
        if (pic != 0) {
            tex = RgPicGetTex(pic);
            x = RgPicGetX(pic);
            y = RgPicGetY(pic);
            size = RgPicListGetSize(pDebug->list);
            RgPicListDraw(pDebug->list, paint);
            xglFontDebugPrintf(0, 8, picListDebugEditHelpText);
            xglFontDebugPrintf(0, 24, picListDebugPicNumberFormat, pDebug->index, size);
            name = RgPicGetName(pic);
            alpha = RgPicGetAlpha(pic);
            xglFontDebugPrintf(0, 36, picListDebugPicPositionFormat, name, x, y, alpha);
            _draw_nontex(paint, x, y, tex->width, tex->height);
        }
        break;
    case RG_PIC_DEBUG_BROWSE:
        pBxxPic = RgBxxGetPicID(pDebug->bxx, pDebug->picId);
        if (pBxxPic != 0) {
            xglFontDebugPrintf(0, 8, picListDebugSelectHelpText);
            xglFontDebugPrintf(0, 32, picListDebugTextureNameFormat, pDebug->picId, pBxxPic);
            _draw_linear(paint, pDebug->bxx, pBxxPic->name, RG_PIC_DEBUG_START_X,
                         RG_PIC_DEBUG_START_Y);
        }
        break;
    case RG_PIC_DEBUG_DELETE:
        pic = RgPicListGetPic(pDebug->list, pDebug->index);
        xglFontDebugPrintf(0, 8, picListDebugDeleteHelpText);
        if (pic != 0)
            RgPicDraw(pic, paint);
        break;
    case RG_PIC_DEBUG_SAVE:
        xglFontDebugPrintf(0, 8, picListDebugSavePromptText);
        xglFontDebugPrintf(0, 24, picListDebugConfirmCancelHelpText);
        break;
    case RG_PIC_DEBUG_LOAD:
        xglFontDebugPrintf(0, 8, picListDebugLoadPromptText);
        xglFontDebugPrintf(0, 24, picListDebugConfirmCancelHelpText);
        break;
    case RG_PIC_DEBUG_OFFSET:
        RgPicListDraw(pDebug->list, paint);
        xglFontDebugPrintf(0, 8, picListDebugMoveImageHintText);
        {
            int ofsX = RgPicListGetOfsX(pDebug->list);
            int ofsY = RgPicListGetOfsY(pDebug->list);
            xglFontDebugPrintf(0, 24, picListDebugOffsetFormat, ofsX, ofsY);
        }
        break;
    }
}

void CreateRgPlacer2D(void)
{
}



static int s_nColor_0 = 32;

const char picListDebugEditHelpText[40] = "\241\373\304\311\262\303\241\241\241\337\272\357\275\374\241\241\243\322\243\261\246\301\241\241\242\244\242\242\300\332\302\330";

const char picListDebugPicNumberFormat[24] = "\263\250\310\326\271\346%3d\241\312\301\264\311\364\244\307%3d\241\313";

const char picListDebugPicPositionFormat[24] = "%s\n%3d,%3d(a:%d)";

const char picListDebugSelectHelpText[32] = "\241\373\267\350\304\352\241\241\241\337\245\255\245\343\245\363\245\273\245\353\241\241\242\244\242\242\301\252\302\362";

const char picListDebugTextureNameFormat[16] = "TEX NAME %3d:%s";

const char picListDebugDeleteHelpText[24] = "\241\373\272\357\275\374\241\241\241\337\245\255\245\343\245\363\245\273\245\353";

const char picListDebugSavePromptText[24] = "\245\273\241\274\245\326\244\267\244\336\244\271\244\253\241\251";

const char picListDebugConfirmCancelHelpText[24] = "\241\373\267\350\304\352\241\241\241\337\245\255\245\343\245\363\245\273\245\353";

const char picListDebugLoadPromptText[24] = "\245\355\241\274\245\311\244\267\244\336\244\271\244\253\241\251";

const char picListDebugMoveImageHintText[32] = "\241\373\275\252\244\357\244\352\241\241\243\322\243\261\263\250\274\253\302\316\260\334\306\260";

const char picListDebugOffsetFormat[16] = "offset %3d %3d";
