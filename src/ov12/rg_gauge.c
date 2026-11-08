/*
 * OV12 original TU 39: 0x00a239d0..0x00a23e48 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "ov12/rg_gauge.h"
#include "ov12/xrg_paint2d.h"
#include "ov12/assert_prog.h"
#include "ov12/rg_heap.h"

#define RG_GAUGE_DEFAULT_CHANGE_SPEED 100.0f

static void _InitGauge(RgGauge *pGauge);

const char gaugeAssertGaugeNotNil[] = "pGauge != NIL";
const char gaugeSourcePath[] = "../rg_gauge.euc.c";
const char gaugeAssertPaintNotNil[] = "pPaint != NIL";

/* Sets the default change speed and values of a gauge and creates its display. */
static void _InitGauge(RgGauge *pGauge)
{
    if (pGauge == 0) {
        assert_prog(gaugeAssertGaugeNotNil, gaugeSourcePath, 29);
    }
    pGauge->changeSpeed = RG_GAUGE_DEFAULT_CHANGE_SPEED;
    pGauge->currentValue = 1.0f;
    pGauge->maxValue = 1.0f;
    pGauge->targetValue = 1.0f;
    pGauge->initialized = 0;
    pGauge->active = 1;
    pGauge->disp = CreateRgGaugeDisp();
}

static void _DestructGauge(RgGauge *pGauge)
{
    if (pGauge == 0) {
        assert_prog(gaugeAssertGaugeNotNil, gaugeSourcePath, 41);
    }
    DisposeRgGaugeDisp(pGauge->disp);
}

RgGauge *CreateRgGauge(void)
{
    RgGauge *pGauge;

    pGauge = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGauge), gaugeSourcePath, 49);
    _InitGauge(pGauge);
    return pGauge;
}

void DisposeRgGauge(RgGauge *pGauge)
{
    if (pGauge == 0) {
        assert_prog(gaugeAssertGaugeNotNil, gaugeSourcePath, 57);
    }
    _DestructGauge(pGauge);
    RgHeapFree(InstanceOfRgHeap(), pGauge, gaugeSourcePath, 59);
}

/* Sets the target value; the first call also sets the current and maximum values. */
void RgGaugeSetValue(RgGauge *pGauge, float value)
{
    if (pGauge == 0) {
        assert_prog(gaugeAssertGaugeNotNil, gaugeSourcePath, 69);
    }
    pGauge->targetValue = value;
    if (pGauge->initialized == 0) {
        pGauge->currentValue = pGauge->maxValue = value;
        pGauge->initialized = 1;
    }
}

void RgGaugeSetActivity(RgGauge *pGauge, int active)
{
    if (pGauge == 0) {
        assert_prog(gaugeAssertGaugeNotNil, gaugeSourcePath, 83);
    }
    pGauge->active = active;
}

float RgGaugeGetValue(RgGauge *pGauge)
{
    if (pGauge == 0) {
        assert_prog(gaugeAssertGaugeNotNil, gaugeSourcePath, 94);
    }
    return pGauge->currentValue;
}

float RgGaugeGetMax(RgGauge *pGauge)
{
    if (pGauge == 0) {
        assert_prog(gaugeAssertGaugeNotNil, gaugeSourcePath, 102);
    }
    return pGauge->maxValue;
}

RgGaugeDisp *RgGaugeGetDisp(RgGauge *pGauge)
{
    if (pGauge == 0) {
        assert_prog(gaugeAssertGaugeNotNil, gaugeSourcePath, 110);
    }
    return pGauge->disp;
}

/* Moves the current value toward the target by changeSpeed times deltaTime, without passing the target. Does nothing while inactive. */
void RgGaugePassTime(RgGauge *pGauge, float deltaTime)
{
    float diff;
    float direction;
    int sign;

    if (pGauge == 0) {
        assert_prog(gaugeAssertGaugeNotNil, gaugeSourcePath, 123);
    }
    if (pGauge->active == 0) {
        return;
    }
    diff = pGauge->targetValue - pGauge->currentValue;
    if (diff > 0.0f) {
        sign = 1;
    } else if (diff < 0.0f) {
        sign = -1;
    } else {
        sign = 0;
    }
    direction = (float) sign;
    pGauge->currentValue += direction * pGauge->changeSpeed * deltaTime;
    if (direction < 0.0f) {
        if (pGauge->currentValue < pGauge->targetValue) {
            pGauge->currentValue = pGauge->targetValue;
        }
    } else if (direction > 0.0f) {
        if (pGauge->targetValue < pGauge->currentValue) {
            pGauge->currentValue = pGauge->targetValue;
        }
    }
}

/* Updates the gauge display with the current and maximum values and draws it, while active. */
void RgGaugeDraw(RgGauge *pGauge, XrgPaint2D *pPaint)
{
    if (pGauge == 0) {
        assert_prog(gaugeAssertGaugeNotNil, gaugeSourcePath, 143);
    }
    if (pPaint == 0) {
        assert_prog(gaugeAssertPaintNotNil, gaugeSourcePath, 144);
    }
    if (pGauge->active == 0) {
        return;
    }
    RgGaugeDispSetValue(pGauge->disp, pGauge->currentValue, pGauge->maxValue);
    RgGaugeDispDraw(pGauge->disp, pPaint);
}
