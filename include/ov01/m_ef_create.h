#ifndef INCLUDE_OV01_M_EF_CREATE_H
#define INCLUDE_OV01_M_EF_CREATE_H

#include "shared.h"

/* MEfCreate copies a complete 0x70-byte request using aligned ld/sd.
 * Only type is interpreted here; effect-specific payload fields are unknown.
 * The union gives the opaque copy storage natural eight-byte alignment
 * without claiming semantic 64-bit fields or using a GNU attribute. */
typedef union MEfCreateParam {
    struct {
        int type;
        u8 opaque[0x70 - 4];
    } fields;
    u64 opaqueStorage[0x70 / sizeof(u64)];
} MEfCreateParam;

/* MEfObjCreate supplies a pooled object with work at +0x20. Its original
 * pool stride is 0x420 and base is 16-byte aligned (main/m_ef_obj.c).
 * This bounded prefix includes the creation request, not the complete
 * constructor-specific work allocation. */
typedef struct MEfObjRecord MEfObjRecord;

struct MEfObjRecord {
    u8 unmodeled_00[0x20];
    MEfCreateParam work;
};

/* One quadword of a GIF packet: a GIF tag, an A+D register write, or a
 * packed RGBAQ / XYZ2 register value; also a trail position or a vector.
 * Shared by the effect TUs that build their own GS packet. */
typedef union MEfGsQword {
    Quadword whole;
    unsigned short nloop; /* low halfword of a GIF tag: NLOOP and EOP */
    struct {
        u64 tag;
        u64 registers;
    } gif;
    struct {
        u64 data;
        unsigned char address;
    } ad;
    u32 channel[4]; /* packed RGBAQ: red, green, blue, alpha */
    struct {
        int r, g, b, a;
    } rgba;
    struct {
        int x, y, z, adc;
    } xyz;
    float element[4];
    Vector4 vector;
} MEfGsQword;

/* The GS packet MGsGPInit prepares and MGsGPTerm flushes (ov01 m_gs.c). */
typedef struct MEfGsPacket {
    void *data;
    int capacity;
    MEfGsQword *current; /* the open GIF tag */
    int count;           /* queued quadwords */
    int reg_count;       /* NREG of the open GIF tag */
} MEfGsPacket;

/* GIF tags of the effect packets. The setup tag is EOP, NREG 1, PACKED, with
 * one A+D register write (SETUP_REGS); a strip is a triangle strip with
 * Gouraud shading and alpha blending (PRIM 0x4C) whose vertices are
 * RGBAQ XYZ2 pairs (STRIP_REGS). XYZ_ADC marks a vertex as not culled. */
#define MEF_GS_SETUP_TAG 0x1000000000008000
#define MEF_GS_SETUP_REGS 0xE
#define MEF_GS_STRIP_PRIM 0x4C
#define MEF_GS_STRIP_REGS 0x5151
#define MEF_GS_XYZ_ADC 0x8000

/* The textured strip tag: NREG 3, PRIM 0x54 (triangle strip, textured, alpha
 * blended, PRE set), registers ST, RGBAQ, XYZ2 (0x512). */
#define MEF_GS_TEXTURED_STRIP_TAG 0x302A400000008000
#define MEF_GS_TEXTURED_STRIP_REGS 0x512

/* The A+D values every effect packet writes after its setup tag: TEST_1
 * (alpha test on, alpha greater than the 0 reference; depth test on, GEQUAL),
 * ZBUF_1 (PSMZ24 storage with the depth write mask set, so no depth write)
 * and ALPHA_1 (Cs * As + Cd, additive). */
#define MEF_GS_TEST_1_ALPHA_GREATER_ZGEQUAL 0x5000D
#define MEF_GS_ZBUF_1_PSMZ24_NO_DEPTH_WRITE 0x131000000
#define MEF_GS_ALPHA_1_ADDITIVE 0x48
/* TEX1_1: MMAG (bit 5) and MMIN (bits 6-8) both set to linear filtering. */
#define MEF_GS_TEX1_1_LINEAR_FILTER 0x60

/* Functions and data used by other translation units. */

#endif /* INCLUDE_OV01_M_EF_CREATE_H */
