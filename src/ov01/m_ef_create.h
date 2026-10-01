/*
 * TU-local declarations of ov01/tu021 (src/ov01/m_ef_create.c).
 */

#ifndef SRC_OV01_M_EF_CREATE_H
#define SRC_OV01_M_EF_CREATE_H

#include "shared.h"

/*
 * MOutputDebugStringWarn (main:0x002eef50): defined as C in
 * src/main/m_output_debug_string.c, still asm-owned in its own TU (main
 * tu218); declared TU-locally here the same way ov01/tu022 (src/ov01/m_ef.h)
 * declares it until main's own TU claims it.
 */
extern void MOutputDebugStringWarn(const char *format, ...);

/*
 * MEfObjCreate / MEfObjDestroy (main:0x002f01b8 / main:0x002f0250): defined
 * in src/main/m_ef_obj.c. Its recovered definitions return an allocated
 * object pointer and an integer destroy result, respectively.
 */
extern void *MEfObjCreate(void);
extern int MEfObjDestroy(void *self);

/* MEfCreate copies exactly 0x70 bytes with aligned ld/sd pairs.
 * Only the leading type is interpreted here. Request payload fields are
 * effect-specific, so bytes plus explicit eight-byte alignment describe
 * the evidence without inventing thirteen unrelated 64-bit fields. */
typedef struct MEfCreateParam {
    int type;
    u8 opaque[0x70 - 4];
} __attribute__((aligned(8))) MEfCreateParam;

/* MEfObjCreate supplies a pooled object with work at +0x20. Its original
 * pool stride is 0x420 and base is 16-byte aligned (main/m_ef_obj.c).
 * This bounded prefix includes the creation request, not the complete
 * constructor-specific work allocation. */
typedef struct MEfObjRecord {
    u8 unmodeled_00[0x20];
    MEfCreateParam work;
} MEfObjRecord;

#define MEF_TYPE_COUNT 14

typedef int (*MEfObjCtor)(MEfObjRecord *self);

/*
 * func_0 / ntbl_1: OV01 .data still asm-owned by this TU
 * (config/symbols/ov01.txt "func_0 = 0x00A43728; // size:0x38", "ntbl_1 =
 * 0x00A43760; // size:0x1C"), each indexed by MEfCreateParam::type: func_0
 * is every type's constructor (nonzero return means success), ntbl_1 the
 * instance count MEfCreate allocates for that type.
 */
/* These are the fourteen relocations in func_0, in table order.
 * All return int and receive the pooled object, including the two
 * constructors that ignore it and simply return zero. */
int MEfCreate_MSP00(MEfObjRecord *self);
int MEfCreate_BP00(MEfObjRecord *self);
int MEfCreate_SMP01(MEfObjRecord *self);
int MEfCreate_EAC00(MEfObjRecord *self);
int MEfCreate_AMP02(MEfObjRecord *self);
int MEfCreate_MSP02(MEfObjRecord *self);
int MEfCreate_SOLB(MEfObjRecord *self);
int MEfCreate_ECM01(MEfObjRecord *self);
int MEfCreate_ECM02(MEfObjRecord *self);
int MEfCreate_GAMERA(MEfObjRecord *self);
int MEfCreate_EAD00(MEfObjRecord *self);
int MEfCreate_SO14(MEfObjRecord *self);
int MEfCreate_DORA(MEfObjRecord *self);
int MEfCreate_KOSBW02(MEfObjRecord *self);

extern MEfObjCtor func_0[MEF_TYPE_COUNT];
extern short ntbl_1[MEF_TYPE_COUNT];

#endif /* SRC_OV01_M_EF_CREATE_H */
