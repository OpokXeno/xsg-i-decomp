/*
 * TU-local declarations of ov12/tu074 (src/ov12/rg_title.c).
 */

#ifndef SRC_OV12_RG_TITLE_H
#define SRC_OV12_RG_TITLE_H

/*
 * RgBxx is defined by ov12/tu073 (src/ov12/rg_bxx.c); this allocation only
 * stores each pointer and hands it to DisposeRgBxx_sub, so an incomplete
 * type is enough here.
 */
typedef struct RgBxx RgBxx;

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
    unsigned char unmodeled_28[0x48]; /* +0x28 */
};

#endif /* SRC_OV12_RG_TITLE_H */
