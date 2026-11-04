#ifndef INCLUDE_OV12_RG_PICLIST_H
#define INCLUDE_OV12_RG_PICLIST_H

#include "shared.h"
#include "ov12/rg_bxx.h"
#include "ov12/xrg_paint2d.h"

/* The picture list capacity the "pList->m_uNum < PIC_MAX" assertion names. */
#define PIC_MAX 128

typedef XrgPaint2DRGBA RgPicColor;

/*
 * One picture of a list: a named picture of an archive with its screen
 * position, blend mode (alpha: 1 add, 2 linear, 3 alpha), flags and color.
 * Every accessor validates it against the "pPic != NIL" assertion.
 */
typedef struct RgPic {
    char name[0x20];    /* 0x00 */
    RgBxxPic *tex;      /* 0x20 */
    int x;              /* 0x24 */
    int y;              /* 0x28 */
    int alpha;          /* 0x2C */
    int dark;           /* 0x30: halve the color when drawing */
    int notDraw;        /* 0x34 */
    int ofsX;           /* 0x38 */
    int ofsY;           /* 0x3C */
    RgPicColor color;   /* 0x40 */
} RgPic;

/*
 * A list of up to PIC_MAX pictures of one archive, drawn together at an
 * offset. CreateRgPicList allocates sizeof(RgPicList) == 0x214 bytes.
 */
typedef struct RgPicList {
    RgBxx *bxx;           /* 0x000 */
    RgPic *pics[PIC_MAX]; /* 0x004 */
    unsigned int m_uNum;  /* 0x204 */
    int ofsX;             /* 0x208 */
    int ofsY;             /* 0x20C */
    int drawEnable;       /* 0x210 */
} RgPicList;

/* Functions and data used by other translation units. */
RgPic *RgPicListGetPic(RgPicList *pList, int index);
void RgPicSetAlpha(RgPic *pic, int alpha);
void RgPicSetOfsXY(RgPic *pic, int ofsX, int ofsY);
void RgPicSetColor(RgPic *pic, const RgPicColor *color);
void RgPicSetDark(RgPic *pic, int dark);
void RgPicNotDraw(RgPic *pic, int notDraw);
RgPicList *CreateRgPicListFromBinary(RgBxx *pBxx, void *pData);
void DisposeRgPicList(RgPicList *pList);
void RgPicListDraw(RgPicList *pList, XrgPaint2D *paint);
void RgPicListSetDrawEnable(RgPicList *pList, int enable);
void RgPicListSetOffset(RgPicList *pList, int ofsX, int ofsY);

#endif /* INCLUDE_OV12_RG_PICLIST_H */
