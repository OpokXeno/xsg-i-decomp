/*
 * OV12 original TU 75: 0x00a40008..0x00a420e0 (46 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_piclist.h"
#include "ov12/rg_debug_flags.h"
/*
 * XrgPaint2DDrawXYWH's mode word is the same bit set XrgPaint2DRect.mode
 * documents in src/ov12/xrg_paint2d.h: bit 0x20 takes the drawn extent from
 * the bound picture instead of the width/height arguments, which is why
 * both draw helpers below always pass zero for width and height.
 */
#define XRG_PAINT2D_MODE_USE_PIC_SIZE 0x20
#define XRG_PAINT2D_ADD 1
#define XRG_PAINT2D_LINEAR 0
/* The fixed on-disk size of a serialized picture record. */
#define RG_PIC_BINARY_SIZE 0x100

extern int RgBxxGetPic(int group, int index);
/*
 * The second argument selects a blend mode (XrgPaint2DAlpha switches over
 * 0..5; RgPicDraw passes 0, 1 or 3 from the picture's alpha setting), not an
 * on/off flag.
 */
extern void XrgPaint2DAlpha(int paint, int blendMode);
extern void XrgPaint2DUseTexture(int paint, int texture);
extern void XrgPaint2DDrawXYWH(int paint, int mode, int x, int y, int width,
                               int height);
extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgDebugFlags *InstanceOfRgDebugFlags(void);
RgPic *CreateRgPicFromBinary(RgBxx *bxx, void *buffer);

/*
 * These are external file-backed witnesses, not candidate-emitted data.
 * Neither address has an entry in config/symbols/ov12.txt, so this
 * allocation keeps the splat default names rather than inventing new ones.
 */
const char D_00A57310[] = "pPic != NIL";

const char D_00A57320[] = "../rg_piclist.euc.c";

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);
extern void RgHeapFree(void *heap, void *pointer, const char *source_file,
                       int line);

/* The fixed on-disk header size of a serialized picture list. */
#define RG_PIC_LIST_BINARY_SIZE 0x40

RgPic *CreateRgPic(RgBxx *bxx, const char *pszName);
extern void DisposeRgPicList(RgPicList *pList);

/*
 * Additional file-backed witnesses this allocation's assertions reference,
 * none registered with a name in config/symbols/ov12.txt.
 */
const char D_00A57338[] = "pBxx != NIL";

const char D_00A57358[] = "pszName != NIL";

const char D_00A57380[] = "pPaint != NIL";

const char D_00A57390[] = "pList != NIL";

const char D_00A573A0[] = "pList->m_uNum < PIC_MAX";

const char D_00A573B8[] = "pDebug != NIL";

static void _draw_add(int paintId, int picGroup, int picIndex, int x, int y)
{
    int texture;

    texture = RgBxxGetPic(picGroup, picIndex);
    if (texture != 0) {
        XrgPaint2DAlpha(paintId, XRG_PAINT2D_ADD);
        XrgPaint2DUseTexture(paintId, texture);
        XrgPaint2DDrawXYWH(paintId, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
    }
}

static void _draw_linear(int paintId, int picGroup, int picIndex, int x, int y)
{
    int texture;

    texture = RgBxxGetPic(picGroup, picIndex);
    if (texture != 0) {
        XrgPaint2DAlpha(paintId, XRG_PAINT2D_LINEAR);
        XrgPaint2DUseTexture(paintId, texture);
        XrgPaint2DDrawXYWH(paintId, XRG_PAINT2D_MODE_USE_PIC_SIZE, x, y, 0, 0);
    }
}

/* Scaffold-owned counter (config/symbols/ov12.txt: s_nColor_0). */
extern int s_nColor_0;
extern void XrgPaint2DColor(int paint, const RgPicColor *color);

static void _draw_nontex(int paintId, int x, int y, int width, int height)
{
    int color[4];
    int fill[4];
    int level;

    memset(fill, 0, sizeof(fill));
    level = s_nColor_0;
    fill[0] = level;
    fill[1] = level;
    fill[2] = level;
    __builtin_memcpy(color, fill, sizeof(color));
    s_nColor_0 = level + 8;
    if (s_nColor_0 >= 33)
        s_nColor_0 = 0;
    XrgPaint2DColor(paintId, (RgPicColor *) color);
    XrgPaint2DAlpha(paintId, XRG_PAINT2D_ADD);
    XrgPaint2DDrawXYWH(paintId, 0, x, y, width, height);
}

int RgPicGetWidth(RgPic *pic)
{
    int width;

    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 81);

    width = 0;
    if (pic->tex != 0)
        width = pic->tex->width;
    return width;
}

int RgPicGetHeight(RgPic *pic)
{
    int height;

    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 93);

    height = 0;
    if (pic->tex != 0)
        height = pic->tex->height;
    return height;
}

void RgPicWrite(RgPic *pic, int handle)
{
    unsigned char buffer[0x400];
    RgDebugFlags *debugFlags;
    unsigned char *cursor;
    unsigned char *src;
    int remaining;

    extern int XrgHostWrite(int handle, const void *buffer, int size);

    debugFlags = InstanceOfRgDebugFlags();
    if (debugFlags->flags[3] == 0)
        return;
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 110);

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

const char D_00A57348[] = "pBuf != NIL";

RgPic *CreateRgPicFromBinary(RgBxx *bxx, void *buffer)
{
    unsigned char *src;
    RgPic *pic;
    int tex;

    src = buffer;
    pic = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgPic), D_00A57320, 136);
    if (bxx == 0)
        assert_prog(D_00A57338, D_00A57320, 139);
    if (buffer == 0)
        assert_prog(D_00A57348, D_00A57320, 140);
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
    pic->tex = (RgPicTexture *)tex;
    pic->ofsY = 0;
    pic->ofsX = 0;
    pic->color.b = 128;
    pic->color.g = 128;
    pic->color.r = 128;
    pic->color.a = 127;
    if (tex == 0) {
        RgHeapFree(InstanceOfRgHeap(), pic, D_00A57320, 153);
        pic = 0;
    }
    return pic;
}

RgPic *RgPicRead(int handle, RgBxx *bxx)
{
    unsigned char buffer[0x400];
    RgDebugFlags *debugFlags;

    extern int XrgHostRead(int handle, void *buffer, int size);

    debugFlags = InstanceOfRgDebugFlags();
    if (debugFlags->flags[3] == 0)
        return 0;
    if (bxx == 0)
        assert_prog(D_00A57338, D_00A57320, 165);
    XrgHostRead(handle, buffer, RgPicBinarySize());
    return CreateRgPicFromBinary(bxx, buffer);
}

const char D_00A57368[] = "pPic->m_pPic != NIL";

RgPic *CreateRgPic(RgBxx *bxx, const char *pszName)
{
    RgPic *pic;

    extern char *strcpy(char *destination, const char *source);

    pic = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgPic), D_00A57320, 177);
    if (bxx == 0)
        assert_prog(D_00A57338, D_00A57320, 178);
    if (pszName == 0)
        assert_prog(D_00A57358, D_00A57320, 179);

    pic->tex = RgBxxGetPic(bxx, pszName);
    if (pic->tex == 0)
        assert_prog(D_00A57368, D_00A57320, 181);
    pic->alpha = 2;
    pic->color.a = 127;
    pic->x = 0;
    pic->y = 0;
    pic->dark = 0;
    pic->notDraw = 0;
    pic->ofsY = 0;
    pic->ofsX = 0;
    pic->color.b = 128;
    pic->color.g = 128;
    pic->color.r = 128;
    strcpy(pic->name, pszName);
    return pic;
}

void DisposeRgPic(RgPic *pic)
{
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 197);
    RgHeapFree(InstanceOfRgHeap(), pic, D_00A57320, 198);
}

char *RgPicGetName(RgPic *pic)
{
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 204);
    return pic->name;
}

int RgPicGetAlpha(RgPic *pic)
{
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 211);
    return pic->alpha;
}

void RgPicSetAlpha(RgPic *pic, int alpha)
{
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 218);
    pic->alpha = alpha;
}

int RgPicGetX(RgPic *pic)
{
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 225);
    return pic->x;
}

int RgPicGetY(RgPic *pic)
{
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 230);
    return pic->y;
}

void RgPicSetOfsXY(RgPic *pic, int ofsX, int ofsY)
{
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 237);
    pic->ofsX = ofsX;
    pic->ofsY = ofsY;
}

void RgPicSetColor(RgPic *pic, const RgPicColor *color)
{
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 245);
    pic->color.r = color->r;
    pic->color.g = color->g;
    pic->color.b = color->b;
    pic->color.a = color->a;
}

RgPicTexture *RgPicGetTex(RgPic *pic)
{
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 255);
    return pic->tex;
}

void RgPicSetPos(RgPic *pic, int x, int y)
{
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 262);
    pic->x = x;
    pic->y = y;
}

void RgPicMove(RgPic *pic, int dx, int dy)
{
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 270);
    pic->x += dx;
    pic->y += dy;
}

/*
 * The picture's alpha field selects a blend mode with the same values
 * XrgPaint2DAlpha documents above (1 -> add, 3 -> alpha, 2 -> linear); any
 * other alpha value leaves the paint object's current blend mode untouched.
 * A "dark" picture halves every color component before handing it to
 * XrgPaint2DColor instead of drawing with its stored color unchanged.
 */
void RgPicDraw(RgPic *pic, int paint)
{
    RgPicColor darkColor;

    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 280);
    if (paint == 0)
        assert_prog(D_00A57380, D_00A57320, 281);

    if (pic->notDraw == 0) {
        switch ((unsigned int) pic->alpha) {
        case 1:
            XrgPaint2DAlpha(paint, 1);
            break;
        case 3:
            XrgPaint2DAlpha(paint, 3);
            break;
        case 2:
            XrgPaint2DAlpha(paint, 0);
            break;
        }
        XrgPaint2DUseTexture(paint, (int) pic->tex);
        if (pic->dark != 0) {
            darkColor.r = pic->color.r / 2;
            darkColor.g = pic->color.g / 2;
            darkColor.b = pic->color.b / 2;
            darkColor.a = pic->color.a / 2;
            XrgPaint2DColor(paint, &darkColor);
        } else {
            XrgPaint2DColor(paint, &pic->color);
        }
        XrgPaint2DDrawXYWH(paint, XRG_PAINT2D_MODE_USE_PIC_SIZE,
                            pic->x + pic->ofsX, pic->y + pic->ofsY, 0, 0);
    }
}

void RgPicSetDark(RgPic *pic, int dark)
{
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 318);
    pic->dark = dark;
}

void RgPicNotDraw(RgPic *pic, int notDraw)
{
    if (pic == 0)
        assert_prog(D_00A57310, D_00A57320, 326);
    pic->notDraw = notDraw;
}

RgPicList *CreateRgPicList(RgBxx *pBxx)
{
    RgPicList *pList;

    pList = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgPicList), D_00A57320, 347);
    if (pBxx == 0)
        assert_prog(D_00A57338, D_00A57320, 348);
    pList->bxx = pBxx;
    pList->m_uNum = 0;
    pList->ofsX = 0;
    pList->ofsY = 0;
    pList->drawEnable = 1;
    return pList;
}

void DisposeRgPicList(RgPicList *pList)
{
    unsigned int i;

    if (pList == 0)
        assert_prog(D_00A57390, D_00A57320, 362);
    for (i = 0; i < pList->m_uNum; i++)
        DisposeRgPic(pList->pics[i]);
    RgHeapFree(InstanceOfRgHeap(), pList, D_00A57320, 365);
}

RgPic *RgPicListAddPic(RgPicList *pList, const char *pszName)
{
    RgPic *pic;

    if (pList == 0)
        assert_prog(D_00A57390, D_00A57320, 373);
    if (pszName == 0)
        assert_prog(D_00A57358, D_00A57320, 374);
    if (pList->m_uNum >= PIC_MAX)
        assert_prog(D_00A573A0, D_00A57320, 375);

    pic = CreateRgPic(pList->bxx, pszName);
    pList->pics[pList->m_uNum++] = pic;
    return pic;
}

void RgPicListDelPic(RgPicList *pList, int index)
{
    unsigned int i;

    if (pList == 0)
        assert_prog(D_00A57390, D_00A57320, 385);

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
        assert_prog(D_00A57390, D_00A57320, 399);

    if (index >= 0 && index < pList->m_uNum) {
        return pList->pics[index];
    }
    return 0;
}

unsigned int RgPicListGetSize(RgPicList *pList)
{
    if (pList == 0)
        assert_prog(D_00A57390, D_00A57320, 410);
    return pList->m_uNum;
}

extern void XrgPaint2DOffset2DDot(int paint, int offsetX, int offsetY);

void RgPicListDraw(RgPicList *pList, int paint)
{
    unsigned int i;

    if (pList == 0)
        assert_prog(D_00A57390, D_00A57320, 419);

    if (pList->drawEnable != 0) {
        XrgPaint2DOffset2DDot(paint, pList->ofsX, pList->ofsY);
        for (i = 0; i < pList->m_uNum; i++)
            RgPicDraw(pList->pics[i], paint);
    }
}

void RgPicListSetDrawEnable(RgPicList *pList, int enable)
{
    if (pList == 0)
        assert_prog(D_00A57390, D_00A57320, 431);
    pList->drawEnable = enable;
}

static int _BinarySizePicList(void)
{
    return RG_PIC_LIST_BINARY_SIZE;
}

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

    extern int XrgHostWrite(int handle, const void *buffer, int size);

    debugFlags = InstanceOfRgDebugFlags();
    if (debugFlags->flags[3] == 0)
        return;
    reserved = 0;
    if (pList == 0)
        assert_prog(D_00A57390, D_00A57320, 452);

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

RgPicList *RgPicListRead(int handle, RgBxx *bxx)
{
    unsigned char buffer[0x400];
    unsigned int num;
    int reserved;
    RgPicList *pList;
    RgPic *pic;
    unsigned int i;
    unsigned char *src;

    extern int XrgHostRead(int handle, void *buffer, int size);

    if (InstanceOfRgDebugFlags()->flags[3] != 0) {
        pList = CreateRgPicList(bxx);
        XrgHostRead(handle, buffer, _BinarySizePicList());
        /*
         * The serialized header is four words: picture count, ofsX, ofsY
         * and a reserved word that is copied but never read. They are
         * copied through a plain pointer, so the moves stay unaligned
         * (lwl/lwr, ov12:0x00a413f8..0x00a41434).
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
        assert_prog(D_00A57390, D_00A57320, 528);
    return pList->ofsX;
}

int RgPicListGetOfsY(RgPicList *pList)
{
    if (pList == 0)
        assert_prog(D_00A57390, D_00A57320, 534);
    return pList->ofsY;
}

void RgPicListSetOffset(RgPicList *pList, int ofsX, int ofsY)
{
    if (pList == 0)
        assert_prog(D_00A57390, D_00A57320, 541);
    pList->ofsX = ofsX;
    pList->ofsY = ofsY;
}

void RgPicListMoveAllPic(RgPicList *pList, int dx, int dy)
{
    unsigned int i;

    if (pList == 0)
        assert_prog(D_00A57390, D_00A57320, 550);
    for (i = 0; i < pList->m_uNum; i++)
        RgPicMove(pList->pics[i], dx, dy);
}

RgPicDebug *CreateRgPicDebug(RgBxx *pBxx)
{
    RgPicDebug *pDebug;

    if (pBxx == 0)
        assert_prog(D_00A57338, D_00A57320, 582);
    pDebug = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgPicDebug), D_00A57320, 583);
    pDebug->bxx = pBxx;
    pDebug->list = CreateRgPicList(pBxx);
    pDebug->mode = 1;
    pDebug->index = -1;
    pDebug->picId = 0;
    return pDebug;
}

void DisposeRgPicDebug(RgPicDebug *pDebug)
{
    if (pDebug == 0)
        assert_prog(D_00A573B8, D_00A57320, 596);
    DisposeRgPicList(pDebug->list);
    RgHeapFree(InstanceOfRgHeap(), pDebug, D_00A57320, 598);
}

/*
 * The pad table in MAIN (0x00490d90); RgPicDebugEditPosition reads the held
 * buttons at +0x28 (lhu PadData+0x28, ov12:0x00a41750). Button bits as in
 * src/ov12/xrg_pad.c.
 */
#include "main/xgl_pad.h"

extern XglPadRecord PadData[2];

#define PAD_L1    0x0004
#define PAD_UP    0x1000
#define PAD_RIGHT 0x2000
#define PAD_DOWN  0x4000
#define PAD_LEFT  0x8000
/* L1 moves in steps of 8 and snaps both coordinates to that grid. */
#define PIC_DEBUG_COARSE_STEP 8

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

#include "main/xgl_pad.h"

extern struct RgBxxPic *RgBxxGetPicID(RgBxx *pBxx, unsigned int id);
extern int RgBxxGetTexNum(RgBxx *pBxx);
extern int XrgHostCreateOpen(const char *filename);
extern int XrgHostReadOpen(const char *filename);
extern void XrgHostClose(int handle);
extern void RgError(const char *message, const char *source_file, int line, ...);
void RgPicListWrite(RgPicList *pList, int handle);
void RgPicWrite(RgPic *pic, int handle);

const char D_00A573C8[] = "host0:place2d.bin";

const char D_00A573E0[] = "pDebug->m_pList != NIL";

const char D_00A573F8[] = "unknown debug edit mode %d";

void RgPicDebugControl(RgPicDebug *pDebug)
{
    RgPic *pic;
    RgPic *oldPic;
    RgPic *newPic;
    struct RgBxxPic *pBxxPic;
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
        assert_prog(D_00A573B8, D_00A57320, 636);

    mode = pDebug->mode;
    if (mode == 1) {
        pic = RgPicListGetPic(pDebug->list, pDebug->index);
        if (pic != 0) {
            x = RgPicGetX(pic);
            y = RgPicGetY(pic);
            RgPicDebugEditPosition(&x, &y);
            if (repeat & 0x8) {
                alpha = RgPicGetAlpha(pic) + 1;
                RgPicSetAlpha(pic, alpha < 4 ? alpha : 0);
            }
            RgPicSetPos(pic, x, y);
        }
        if (repeat & 0x10)
            pDebug->index--;
        if (repeat & 0x80)
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
        if (repeat & 0x20)
            pDebug->mode = 0;
        if (repeat & 0x40)
            pDebug->mode = 2;
        if ((buttons & 5) == 5)
            pDebug->mode = 3;
        if ((buttons & 0xa) == 10)
            pDebug->mode = 4;
        if (repeat & 0x800)
            pDebug->mode = 5;
    } else if (mode == 0) {
        texNum = RgBxxGetTexNum(pDebug->bxx);
        if (texNum == 0)
            pDebug->mode = 1;
        if (repeat & 0x10)
            pDebug->picId--;
        else if (repeat & 0x80)
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
        if (repeat & 0x20) {
            pBxxPic = RgBxxGetPicID(pDebug->bxx, picId);
            if (pBxxPic != 0) {
                oldPic = RgPicListGetPic(pDebug->list, pDebug->index);
                newPic = RgPicListAddPic(pDebug->list, (const char *)pBxxPic);
                if (newPic != 0) {
                    if (oldPic == 0) {
                        RgPicSetPos(newPic, 64, 128);
                    } else {
                        offX = 128 - RgPicGetX(oldPic);
                        offY = 128 - RgPicGetY(oldPic);
                        if (stickX < -80) {
                            offX = -RgPicGetWidth(newPic);
                            offY = 0;
                        } else if (stickX >= 81) {
                            offX = RgPicGetWidth(oldPic);
                            offY = 0;
                        } else if (stickY < -80) {
                            offX = 0;
                            offY = -RgPicGetHeight(newPic);
                        } else if (stickY >= 81) {
                            offX = 0;
                            offY = RgPicGetHeight(oldPic);
                        }
                        RgPicSetPos(newPic, RgPicGetX(oldPic) + offX,
                                    RgPicGetY(oldPic) + offY);
                    }
                }
            }
            pDebug->mode = 1;
            pDebug->index = RgPicListGetSize(pDebug->list) - 1;
        }
        if (repeat & 0x40)
            pDebug->mode = 1;
    } else if (mode == 2) {
        if (repeat & 0x20) {
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
            pDebug->mode = 1;
        }
        if (repeat & 0x840)
            pDebug->mode = 1;
    } else if (mode == 3) {
        if (repeat & 0x20) {
            if (InstanceOfRgDebugFlags()->flags[3] != 0) {
                handle = XrgHostCreateOpen(D_00A573C8);
                if (handle >= 0) {
                    RgPicListWrite(pDebug->list, handle);
                    XrgHostClose(handle);
                }
            }
            pDebug->mode = 1;
        } else if (repeat & 0x40) {
            pDebug->mode = 1;
        }
    } else if (mode == 4) {
        if (repeat & 0x20) {
            if (InstanceOfRgDebugFlags()->flags[3] != 0) {
                handle = XrgHostReadOpen(D_00A573C8);
                if (handle >= 0) {
                    DisposeRgPicList(pDebug->list);
                    pDebug->list = RgPicListRead(handle, pDebug->bxx);
                    if (pDebug->list == 0)
                        assert_prog(D_00A573E0, D_00A57320, 770);
                    XrgHostClose(handle);
                    pDebug->index = RgPicListGetSize(pDebug->list) - 1;
                }
            }
            pDebug->mode = 1;
        } else if (repeat & 0x40) {
            pDebug->mode = 1;
        }
    } else if (mode == 5) {
        ofsX = RgPicListGetOfsX(pDebug->list);
        ofsY = RgPicListGetOfsY(pDebug->list);
        if (buttons & 8) {
            dx = 0;
            dy = 0;
            RgPicDebugEditPosition(&dx, &dy);
            RgPicListMoveAllPic(pDebug->list, dx, dy);
        } else {
            RgPicDebugEditPosition(&ofsX, &ofsY);
            RgPicListSetOffset(pDebug->list, ofsX, ofsY);
        }
        if (repeat & 0x20)
            pDebug->mode = 1;
    } else {
        RgError(D_00A573F8, D_00A57320, 802, mode);
    }
}

/* Debug text strings: scaffold-owned rodata, referenced by address only. */
extern const char D_00A57418[];
extern const char D_00A57440[];
extern const char D_00A57458[];
extern const char D_00A57470[];
extern const char D_00A57490[];
extern const char D_00A574A0[];
extern const char D_00A574B8[];
extern const char D_00A574D0[];
extern const char D_00A574E8[];
extern const char D_00A57500[];
extern const char D_00A57520[];
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);

void RgPicDebugDisp(RgPicDebug *pDebug, int paint)
{
    RgPic *pic;
    RgPicTexture *tex;
    struct RgBxxPic *pBxxPic;
    char *name;
    int x;
    int y;
    int alpha;
    unsigned int size;

    if (pDebug == 0)
        assert_prog(D_00A573B8, D_00A57320, 812);

    switch (pDebug->mode) {
    case 1:
        pic = RgPicListGetPic(pDebug->list, pDebug->index);
        if (pic != 0) {
            tex = RgPicGetTex(pic);
            x = RgPicGetX(pic);
            y = RgPicGetY(pic);
            size = RgPicListGetSize(pDebug->list);
            RgPicListDraw(pDebug->list, paint);
            xglFontDebugPrintf(0, 8, D_00A57418);
            xglFontDebugPrintf(0, 24, D_00A57440, pDebug->index, size);
            name = RgPicGetName(pic);
            alpha = RgPicGetAlpha(pic);
            xglFontDebugPrintf(0, 36, D_00A57458, name, x, y, alpha);
            _draw_nontex(paint, x, y, tex->width, tex->height);
        }
        break;
    case 0:
        pBxxPic = RgBxxGetPicID(pDebug->bxx, pDebug->picId);
        if (pBxxPic != 0) {
            xglFontDebugPrintf(0, 8, D_00A57470);
            xglFontDebugPrintf(0, 32, D_00A57490, pDebug->picId, pBxxPic);
            _draw_linear(paint, pDebug->bxx, pBxxPic, 64, 128);
        }
        break;
    case 2:
        pic = RgPicListGetPic(pDebug->list, pDebug->index);
        xglFontDebugPrintf(0, 8, D_00A574A0);
        if (pic != 0)
            RgPicDraw(pic, paint);
        break;
    case 3:
        xglFontDebugPrintf(0, 8, D_00A574B8);
        xglFontDebugPrintf(0, 24, D_00A574D0);
        break;
    case 4:
        xglFontDebugPrintf(0, 8, D_00A574E8);
        xglFontDebugPrintf(0, 24, D_00A574D0);
        break;
    case 5:
        RgPicListDraw(pDebug->list, paint);
        xglFontDebugPrintf(0, 8, D_00A57500);
        {
            int ofsX = RgPicListGetOfsX(pDebug->list);
            int ofsY = RgPicListGetOfsY(pDebug->list);
            xglFontDebugPrintf(0, 24, D_00A57520, ofsX, ofsY);
        }
        break;
    }
}

void CreateRgPlacer2D(void)
{
}
