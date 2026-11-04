#ifndef INCLUDE_OV12_RG_GAUGE_DISP_H
#define INCLUDE_OV12_RG_GAUGE_DISP_H

#include "shared.h"

#include "ov12/xrg_paint2d.h"

/*
 * Bar display of one gauge: a filled-bar texture and a remainder texture,
 * each with its UVWH rectangle, drawn at (posX, posY).
 * CreateRgGaugeDisp allocates sizeof(RgGaugeDisp) (0x50) bytes.
 */
typedef struct RgGaugeDisp {
    float currentValue;      /* +0x00: RgGaugeDispSetValue */
    float maxValue;          /* +0x04: RgGaugeDispSetValue */
    unsigned int validState; /* +0x08: _CheckDatas: 0 bar and rest drawable,
                                1 bar only, 2 nothing drawable */
    RgBxxPic *barPic;        /* +0x0c: RgGaugeDispSetTex, _disp_bar */
    RgBxxPic *restPic;       /* +0x10: RgGaugeDispSetTex, _disp_bar_rest */
    unsigned char unmodeled_14[0x0c]; /* +0x14, untouched by this allocation */
    int barUVWH[4];          /* +0x20: RgGaugeDispSetBarUVWH */
    int restUVWH[4];         /* +0x30: RgGaugeDispSetRestUVWH */
    int posX;                /* +0x40: RgGaugeDispSetPos */
    int posY;                /* +0x44: RgGaugeDispSetPos */
    int order;               /* +0x48: RgGaugeDispSetOrder */
    int alpha;               /* +0x4c: RgGaugeDispSetAlpha */
} RgGaugeDisp;

/* Functions and data used by other translation units. */
RgGaugeDisp *CreateRgGaugeDisp(void);
void DisposeRgGaugeDisp(RgGaugeDisp *disp);
int RgGaugeDispIsDrawable(RgGaugeDisp *disp);
void RgGaugeDispSetValue(RgGaugeDisp *disp, float currentValue,
                         float maxValue);
void RgGaugeDispSetTex(RgGaugeDisp *disp, RgBxxPic *barPic,
                       RgBxxPic *restPic);
void RgGaugeDispSetBarUVWH(RgGaugeDisp *disp, int texU, int texV, int texW,
                           int texH);
void RgGaugeDispSetRestUVWH(RgGaugeDisp *disp, int texU, int texV, int texW,
                            int texH);
void RgGaugeDispSetPos(RgGaugeDisp *disp, int x, int y);
void RgGaugeDispSetOrder(RgGaugeDisp *disp, int order);
void RgGaugeDispSetAlpha(RgGaugeDisp *disp, int alpha);
void RgGaugeDispDraw(RgGaugeDisp *disp, XrgPaint2D *paint);

#endif /* INCLUDE_OV12_RG_GAUGE_DISP_H */
