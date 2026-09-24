/*
 * OV12 original TU 75: 0x00a40008..0x00a420e0 (46 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_piclist.h"

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

/*
 * These are external file-backed witnesses, not candidate-emitted data.
 * Neither address has an entry in config/symbols/ov12.txt, so this
 * allocation keeps the splat default names rather than inventing new ones.
 */
extern const char D_00A57310[];
extern const char D_00A57320[];

extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size, const char *source_file,
                         int line);
extern void RgHeapFree(void *heap, void *pointer, const char *source_file,
                       int line);

/* The fixed on-disk header size of a serialized picture list. */
#define RG_PIC_LIST_BINARY_SIZE 0x40

extern RgPic *CreateRgPic(RgBxx *bxx, const char *pszName);
extern void DisposeRgPicList(RgPicList *pList);

/*
 * Additional file-backed witnesses this allocation's assertions reference,
 * none registered with a name in config/symbols/ov12.txt.
 */
extern const char D_00A57338[]; /* "pBxx != NIL" */
extern const char D_00A57358[]; /* "pszName != NIL" */
extern const char D_00A57380[]; /* "pPaint != NIL" */
extern const char D_00A57390[]; /* "pList != NIL" */
extern const char D_00A573A0[]; /* "pList->m_uNum < PIC_MAX" */
extern const char D_00A573B8[]; /* "pDebug != NIL" */

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_piclist", _draw_nontex);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_piclist", RgPicWrite);

int RgPicBinarySize(void)
{
    return RG_PIC_BINARY_SIZE;
}

INCLUDE_ASM("asm/nonmatchings/ov12/rg_piclist", CreateRgPicFromBinary);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_piclist", RgPicRead);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_piclist", CreateRgPic);

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

extern void XrgPaint2DColor(int paint, const RgPicColor *color);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_piclist", RgPicListDelPic);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_piclist", RgPicListDraw);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_piclist", RgPicListWrite);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_piclist", CreateRgPicListFromBinary);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_piclist", RgPicListRead);

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

INCLUDE_ASM("asm/nonmatchings/ov12/rg_piclist", RgPicDebugEditPosition);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_piclist", RgPicDebugControl);

INCLUDE_ASM("asm/nonmatchings/ov12/rg_piclist", RgPicDebugDisp);

void CreateRgPlacer2D(void)
{
}
