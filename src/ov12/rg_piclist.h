/*
 * TU-local declarations of ov12/tu075 (src/ov12/rg_piclist.c).
 */

#ifndef SRC_OV12_RG_PICLIST_H
#define SRC_OV12_RG_PICLIST_H

typedef struct RgPicTexture RgPicTexture;

/*
 * Only the two fields RgPicGetWidth/RgPicGetHeight read are evidenced
 * (ov12:0x00a40230/0x00a40280, offsets 0x34/0x38 of the bound texture);
 * nothing before them is read or written by any function of this TU, so the
 * span stays an explicit unmodeled range rather than a guessed field.
 */
struct RgPicTexture {
    unsigned char unmodeled_00[0x34];
    int width;  /* 0x34 */
    int height; /* 0x38 */
};

typedef struct RgPicColor RgPicColor;

/*
 * The four-component color RgPicSetColor (ov12:0x00a40920) copies into a
 * picture: plain lw/sw words, never lwc1/swc1, so the components are
 * integers, not floats.
 */
struct RgPicColor {
    int r;
    int g;
    int b;
    int a;
};

typedef struct RgPic RgPic;

/*
 * The picture record every accessor of this TU validates against NULL with
 * the shared "pPic != NIL" assertion.  Every member is an offset one of
 * this allocation's own accessors reads or writes:
 *
 *   name     RgPicGetName (0x00a40770) returns its address; the 0x20-byte
 *            span up to `tex` is exactly the gap between offset 0 and the
 *            next accessed offset, which is why the field is sized 0x20.
 *   tex      RgPicGetTex/RgPicGetWidth/RgPicGetHeight (0x00a40988/
 *            0x00a40230/0x00a40280) read the bound texture pointer.
 *   x, y     RgPicGetX/RgPicGetY/RgPicSetPos/RgPicMove (0x00a40840/
 *            0x00a40880/0x00a409c8/0x00a40a28).
 *   alpha    RgPicGetAlpha/RgPicSetAlpha (0x00a407b0/0x00a407f0).
 *   dark     RgPicSetDark (0x00a40c28).
 *   notDraw  RgPicNotDraw (0x00a40c78).
 *   ofsX,    RgPicSetOfsXY (0x00a408c0).
 *   ofsY
 *   color    RgPicSetColor (0x00a40920).
 *
 * The struct ends exactly at the last accessed offset (0x50); nothing beyond
 * it is claimed by this allocation.
 */
struct RgPic {
    char name[0x20];    /* 0x00 */
    RgPicTexture *tex;  /* 0x20 */
    int x;              /* 0x24 */
    int y;              /* 0x28 */
    int alpha;          /* 0x2C */
    int dark;           /* 0x30 */
    int notDraw;        /* 0x34 */
    int ofsX;           /* 0x38 */
    int ofsY;           /* 0x3C */
    RgPicColor color;   /* 0x40 */
};

/*
 * Opaque picture archive handle: CreateRgPicList (ov12:0x00a40cc8) validates
 * its argument against the "pBxx != NIL" assertion and stores it unchanged at
 * offset 0 of RgPicList, and RgPicListAddPic (0x00a40e00) passes that same
 * stored value on to CreateRgPic. Neither function in this allocation reads
 * through it, so its layout stays out of scope here.
 */
typedef struct RgBxx RgBxx;

/* The picture list capacity the "pList->m_uNum < PIC_MAX" assertion names. */
#define PIC_MAX 128

typedef struct RgPicList RgPicList;

/*
 * CreateRgPicList (ov12:0x00a40cc8) allocates sizeof(RgPicList) == 0x214
 * bytes and fills every field:
 *
 *   bxx      the picture archive handed to CreateRgPicList, stored at offset
 *            0 and forwarded to CreateRgPic by RgPicListAddPic (0x00a40e00).
 *   pics     RgPicListAddPic appends CreateRgPic's result at
 *            pics[m_uNum++]; RgPicListGetPic (0x00a40f80) indexes it the same
 *            way. PIC_MAX (128) is the capacity the "pList->m_uNum < PIC_MAX"
 *            assertion in RgPicListAddPic names, which is exactly what fits
 *            between offset 0x004 and m_uNum at 0x204.
 *   m_uNum   the list's element count; RgPicListGetSize (0x00a40ff0) returns
 *            it and RgPicListGetPic bounds-checks against it. Named for the
 *            "pList->m_uNum < PIC_MAX" assertion text.
 *   ofsX,    RgPicListGetOfsX/-OfsY (0x00a414c0/0x00a41500) read them;
 *   ofsY     RgPicListSetOffset (0x00a41540) writes both.
 *   drawEnable  RgPicListSetDrawEnable (0x00a410e0) writes it; CreateRgPicList
 *               initializes it to 1 (enabled by default).
 *
 * The struct ends exactly at the last field; nothing beyond offset 0x214 is
 * claimed by this allocation.
 */
struct RgPicList {
    RgBxx *bxx;          /* 0x000 */
    RgPic *pics[PIC_MAX]; /* 0x004 */
    unsigned int m_uNum; /* 0x204 */
    int ofsX;            /* 0x208 */
    int ofsY;            /* 0x20C */
    int drawEnable;      /* 0x210 */
};

typedef struct RgPicDebug RgPicDebug;

/*
 * DisposeRgPicDebug (ov12:0x00a416d8) is the only function of this allocation
 * that touches an RgPicDebug: it reads the owned RgPicList pointer at offset
 * 0x04 to tear it down through DisposeRgPicList before freeing the object
 * itself. Nothing before that offset is read or written here, so it stays an
 * explicit unmodeled span.
 */
struct RgPicDebug {
    unsigned char unmodeled_00[0x04];
    RgPicList *list; /* 0x04 */
};

#endif /* SRC_OV12_RG_PICLIST_H */
