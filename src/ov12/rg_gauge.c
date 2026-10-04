/*
 * OV12 original TU 39: 0x00a239d0..0x00a23e48 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "rg_gauge.h"

const char D_00A54608[] = "pGauge != NIL";
const char D_00A54618[] = "../rg_gauge.euc.c";
const char D_00A54630[] = "pPaint != NIL";

extern void assert_prog(const char *expression, const char *source_file,
                        int line);
extern RgHeap *InstanceOfRgHeap(void);
extern void *RgHeapAlloc(void *heap, unsigned int size,
                         const char *source_file, int line);
static void _InitGauge(RgGauge *pGauge);
extern void DisposeRgGaugeDisp(RgGaugeDisp *disp);
extern void RgGaugeDispSetValue(RgGaugeDisp *disp, float currentValue,
                                float maxValue);
extern void RgGaugeDispDraw(RgGaugeDisp *disp, void *pPaint);
extern void RgHeapFree(RgHeap *heap, void *ptr, const char *source_file,
                       int line);
extern RgGaugeDisp *CreateRgGaugeDisp(void);

void _InitGauge(RgGauge *pGauge)
{
    if (pGauge == 0) {
        assert_prog(D_00A54608, D_00A54618, 29);
    }
    pGauge->changeSpeed = 100.0f;
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
        assert_prog(D_00A54608, D_00A54618, 41);
    }
    DisposeRgGaugeDisp(pGauge->disp);
}

RgGauge *CreateRgGauge(void)
{
    RgGauge *pGauge;

    pGauge = RgHeapAlloc(InstanceOfRgHeap(), sizeof(RgGauge), D_00A54618, 49);
    _InitGauge(pGauge);
    return pGauge;
}

void DisposeRgGauge(RgGauge *pGauge)
{
    if (pGauge == 0) {
        assert_prog(D_00A54608, D_00A54618, 57);
    }
    _DestructGauge(pGauge);
    RgHeapFree(InstanceOfRgHeap(), pGauge, D_00A54618, 59);
}

void RgGaugeSetValue(RgGauge *pGauge, float value)
{
    if (pGauge == 0) {
        assert_prog(D_00A54608, D_00A54618, 69);
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
        assert_prog(D_00A54608, D_00A54618, 83);
    }
    pGauge->active = active;
}

float RgGaugeGetValue(RgGauge *pGauge)
{
    if (pGauge == 0) {
        assert_prog(D_00A54608, D_00A54618, 94);
    }
    return pGauge->currentValue;
}

float RgGaugeGetMax(RgGauge *pGauge)
{
    if (pGauge == 0) {
        assert_prog(D_00A54608, D_00A54618, 102);
    }
    return pGauge->maxValue;
}

RgGaugeDisp *RgGaugeGetDisp(RgGauge *pGauge)
{
    if (pGauge == 0) {
        assert_prog(D_00A54608, D_00A54618, 110);
    }
    return pGauge->disp;
}

void RgGaugePassTime(RgGauge *pGauge, float deltaTime)
{
    float diff;
    float direction;
    int sign;

    if (pGauge == 0) {
        assert_prog(D_00A54608, D_00A54618, 123);
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

void RgGaugeDraw(RgGauge *pGauge, void *pPaint)
{
    if (pGauge == 0) {
        assert_prog(D_00A54608, D_00A54618, 143);
    }
    if (pPaint == 0) {
        assert_prog(D_00A54630, D_00A54618, 144);
    }
    if (pGauge->active == 0) {
        return;
    }
    RgGaugeDispSetValue(pGauge->disp, pGauge->currentValue, pGauge->maxValue);
    RgGaugeDispDraw(pGauge->disp, pPaint);
}
