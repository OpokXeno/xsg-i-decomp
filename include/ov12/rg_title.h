#ifndef INCLUDE_OV12_RG_TITLE_H
#define INCLUDE_OV12_RG_TITLE_H

/*
 * The 2D paint context (defined by ov12/tu086 xrg_paint2d); this TU only
 * stores the pointer and hands it to the XrgPaint2D* calls.
 */
typedef struct XrgPaint2D XrgPaint2D;

/*
 * RgBxx is defined by ov12/tu073 (src/ov12/rg_bxx.c); this allocation only
 * stores and forwards the pointer LoadRgBxx_sub returns and DisposeRgBxx_sub
 * takes, so an incomplete type is enough here.
 */
typedef struct RgBxx RgBxx;

/*
 * One picture record of the archive, 0x60 (96) bytes each (RgBxxGetPicID's
 * id * 0x60 and _FindPicByName's pointer step by 96). rg_help.h defines
 * this tag with a partial view (`height` at +0x38); RgBxxSetData
 * (ov12:0x00a3ed58) additionally evidences an owning-archive back-pointer
 * at +0x58, absent from that view, so the type stays incomplete here.
 */
typedef struct RgBxxPic RgBxxPic;

/*
 * CreateRgTitle (ov12:0x00a3f9a8) allocates 0x70 bytes for one RgTitle.
 * _DestructTitle (ov12:0x00a3f8b8) releases the paint context at +0x00
 * (paint, DisposeXrgPaint2D_sub) and nine texture archives at +0x04..+0x24
 * (bxx[9], DisposeRgBxx_sub, one call per element); no function claimed so
 * far reads or writes past +0x28.
 */
struct RgTitle {
    XrgPaint2D *paint;                /* +0x00 */
    RgBxx *bxx[9];                    /* +0x04 */
    RgBxxPic *pictures[14];           /* +0x28..+0x5C */
    int cursor_id;                   /* +0x60 */
    int menu_state;                  /* +0x64 */
    float transition_time;           /* +0x68 */
    int count;                       /* +0x6C */
};

#endif /* INCLUDE_OV12_RG_TITLE_H */
