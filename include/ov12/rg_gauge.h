#ifndef INCLUDE_OV12_RG_GAUGE_H
#define INCLUDE_OV12_RG_GAUGE_H

#include "shared.h"
#include "ov12/rg_gauge_disp.h"

/*
 * A gauge that eases currentValue toward targetValue by changeSpeed units
 * per second (RgGaugePassTime) and hands both values to its RgGaugeDisp when
 * drawn. CreateRgGauge allocates sizeof(RgGauge) (0x1c) bytes.
 */
typedef struct RgGauge {
    int active;          /* +0x00: RgGaugeSetActivity, RgGaugeDraw */
    int initialized;     /* +0x04: one-shot latch set by RgGaugeSetValue */
    float currentValue;  /* +0x08 */
    float maxValue;      /* +0x0c */
    float targetValue;   /* +0x10 */
    float changeSpeed;   /* +0x14: set by _InitGauge */
    RgGaugeDisp *disp;   /* +0x18 */
} RgGauge;

/* Functions and data used by other translation units. */
RgGauge *CreateRgGauge(void);
void DisposeRgGauge(RgGauge *pGauge);
void RgGaugeSetValue(RgGauge *pGauge, float value);
void RgGaugeSetActivity(RgGauge *pGauge, int active);
float RgGaugeGetValue(RgGauge *pGauge);
float RgGaugeGetMax(RgGauge *pGauge);
RgGaugeDisp *RgGaugeGetDisp(RgGauge *pGauge);
void RgGaugePassTime(RgGauge *pGauge, float deltaTime);
void RgGaugeDraw(RgGauge *pGauge, XrgPaint2D *pPaint);

#endif /* INCLUDE_OV12_RG_GAUGE_H */
