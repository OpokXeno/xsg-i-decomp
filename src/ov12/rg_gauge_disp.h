/*
 * TU-local declarations of ov12/tu040 (src/ov12/rg_gauge_disp.c).
 */

#ifndef SRC_OV12_RG_GAUGE_DISP_H
#define SRC_OV12_RG_GAUGE_DISP_H

/*
 * The fields this allocation evidences: RgGaugeDispSetValue writes the two
 * leading floats and RgGaugeDispIsDrawable reads the validation state at
 * +0x08 that _CheckDatas recomputes; RgGaugeDispSetTex writes +0x0c/+0x10;
 * RgGaugeDispSetBarUVWH/RgGaugeDispSetRestUVWH write the two four-int UVWH
 * vectors at +0x20/+0x30 through XrgSetIVector; RgGaugeDispSetPos writes
 * +0x40/+0x44; RgGaugeDispSetOrder writes +0x48; RgGaugeDispSetAlpha writes
 * +0x4c.  CreateRgGaugeDisp allocates sizeof(RgGaugeDisp) (0x50) bytes for
 * one, so the struct ends at +0x4c and nothing past it is claimed here.
 */
typedef struct RgGaugeDisp {
    float currentValue;      /* +0x00: RgGaugeDispSetValue */
    float maxValue;          /* +0x04: RgGaugeDispSetValue */
    unsigned int validState; /* +0x08: _CheckDatas: 0 bar and rest drawable,
                                1 bar only, 2 nothing drawable */
    int barPic;              /* +0x0c: RgGaugeDispSetTex, _disp_bar */
    int restPic;             /* +0x10: RgGaugeDispSetTex, _disp_bar_rest */
    unsigned char unmodeled_14[0x0c]; /* +0x14, untouched by this allocation */
    int barUVWH[4];          /* +0x20: RgGaugeDispSetBarUVWH */
    int restUVWH[4];         /* +0x30: RgGaugeDispSetRestUVWH */
    int posX;                /* +0x40: RgGaugeDispSetPos */
    int posY;                /* +0x44: RgGaugeDispSetPos */
    int order;               /* +0x48: RgGaugeDispSetOrder */
    int alpha;               /* +0x4c: RgGaugeDispSetAlpha */
} RgGaugeDisp;

/* The original literals at ov12:0x00a54640 ("pDisp != NIL") and 0x00a54650
 * ("../rg_gauge_disp.euc.c") are scaffold-owned (config/tu-build.json
 * data_ownership: this .rodata window is still owner "asm") and have no
 * entry in config/symbols/ov12.txt, so they keep their splat names
 * (docs/naming.md, "Scaffold-owned data keeps its splat name"). */
extern const char D_00A54640[];
extern const char D_00A54650[];

/*
 * The 2D paint context (defined by ov12/tu086 xrg_paint2d); this TU only
 * passes the pointer through to the XrgPaint2D* calls.
 */
typedef struct XrgPaint2D XrgPaint2D;

/*
 * The rectangle _disp_bar computes for the drawn bar's remainder and hands
 * to _disp_bar_rest: x, y are the screen origin and width, height the size
 * XrgPaint2DDrawXYWH draws the remainder with.
 */
typedef struct RgGaugeDispRect {
    int x;
    int y;
    int width;
    int height;
} RgGaugeDispRect;

#endif /* SRC_OV12_RG_GAUGE_DISP_H */
