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
 * in src/main/m_ef_obj.c, still INCLUDE_ASM there; declared TU-locally the
 * same way every m_ef_create_*.c TU already declares MEfObjDestroy until
 * main's own TU claims them.
 */
extern void *MEfObjCreate(void);
extern void MEfObjDestroy(void *self);

/*
 * MEfCreateParam: the per-effect creation request MEfCreate copies wholesale
 * (0x70 bytes) into the new object's work area with a straight ld/sd
 * assignment; type selects the ntbl_1/func_0 entry below and is the only
 * field this TU reads individually. The remaining bytes are opaque per-type
 * parameters interpreted by each type's own constructor (e.g. GameraState in
 * src/ov01/m_ef_create_gamera.c, whose work pointer is this same block);
 * unmodeled_08's u64 element type is evidenced size/alignment only, needed
 * for the assignment to compile to ld/sd instead of ldl/ldr/sdl/sdr.
 */
typedef struct MEfCreateParam {
    int type;             /* +0x00 */
    u8 unmodeled_04[4];    /* +0x04 */
    u64 unmodeled_08[13];  /* +0x08 */
} MEfCreateParam;

/*
 * MEfObjRecord: partial view of the pooled object MEfObjCreate returns
 * (main/m_ef_obj.c's MEfObj, still INCLUDE_ASM/unpublished there); only the
 * work area MEfCreate writes is named here, at the same +0x20 offset
 * src/ov01/m_ef_create_gamera.c's GameraState is cast onto.
 */
typedef struct MEfObjRecord {
    u8 unmodeled_00[0x20];
    MEfCreateParam work;
} MEfObjRecord;

#define MEF_TYPE_COUNT 14

typedef int (*MEfObjCtor)(void *self);

/*
 * func_0 / ntbl_1: OV01 rodata still asm-owned by this TU
 * (config/symbols/ov01.txt "func_0 = 0x00A43728; // size:0x38", "ntbl_1 =
 * 0x00A43760; // size:0x1C"), each indexed by MEfCreateParam::type: func_0
 * is every type's constructor (nonzero return means success), ntbl_1 the
 * instance count MEfCreate allocates for that type.
 */
extern MEfObjCtor func_0[MEF_TYPE_COUNT];
extern short ntbl_1[MEF_TYPE_COUNT];

#endif /* SRC_OV01_M_EF_CREATE_H */
