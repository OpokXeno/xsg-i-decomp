/*
 * TU-local declarations of ov12/tu039 (src/ov12/rg_gauge.c).
 */

#ifndef SRC_OV12_RG_GAUGE_H
#define SRC_OV12_RG_GAUGE_H

/*
 * RgGaugeDisp is defined by ov12/tu040 (src/ov12/rg_gauge_disp.c); this TU
 * only ever stores and forwards a pointer to it, so the tag stays opaque
 * here rather than repeating that TU's struct.
 */
typedef struct RgGaugeDisp RgGaugeDisp;

/*
 * The fields this allocation evidences: RgGaugeSetActivity writes +0x00 and
 * RgGaugeDraw reads it to decide whether to draw at all; RgGaugeSetValue
 * writes +0x04 as a one-shot latch and +0x08/+0x0c/+0x10, and
 * RgGaugeGetValue/RgGaugeGetMax read +0x08/+0x0c, as does RgGaugeDraw when it
 * hands the value off to the display object.  _DestructGauge, RgGaugeGetDisp
 * and RgGaugeDraw read the display pointer at +0x18, which _InitGauge
 * (INCLUDE_ASM, not part of this allocation) fills in.  CreateRgGauge
 * allocates sizeof(RgGauge) (0x1c) bytes for one, so the struct ends at
 * +0x18 and nothing past it is claimed here.
 */
typedef struct RgGauge {
    int active;                    /* +0x00: RgGaugeSetActivity, RgGaugeDraw */
    int initialized;               /* +0x04: RgGaugeSetValue */
    float currentValue;            /* +0x08: RgGaugeSetValue, RgGaugeGetValue, RgGaugeDraw */
    float maxValue;                /* +0x0c: RgGaugeSetValue, RgGaugeGetMax, RgGaugeDraw */
    float targetValue;             /* +0x10: RgGaugeSetValue */
    unsigned char unmodeled_14[4]; /* +0x14: untouched by this allocation */
    RgGaugeDisp *disp;             /* +0x18: _InitGauge, _DestructGauge, RgGaugeGetDisp, RgGaugeDraw */
} RgGauge;

/*
 * The original literals at ov12:0x00a54608 ("pGauge != NIL"), 0x00a54618
 * ("../rg_gauge.euc.c") and 0x00a54630 ("pPaint != NIL") are scaffold-owned
 * (config/tu-build.json data_ownership: this .rodata window is still owner
 * "asm") and have no entry in config/symbols/ov12.txt, so they keep their
 * splat names (docs/naming.md, "Scaffold-owned data keeps its splat name").
 */
extern const char D_00A54608[];
extern const char D_00A54618[];
extern const char D_00A54630[];

#endif /* SRC_OV12_RG_GAUGE_H */
