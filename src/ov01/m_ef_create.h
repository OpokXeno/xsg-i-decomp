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

#define MEF_TYPE_COUNT 14

typedef int (*MEfObjCtor)(MEfObjRecord *self);

/* C constructor declarations come from their defining TU headers.
 * The remaining five entries are still assembly-owned; this factory is
 * their only C caller, so their common prototypes remain TU-local. */
int MEfCreate_MSP00(MEfObjRecord *self);
int MEfCreate_SMP01(MEfObjRecord *self);
int MEfCreate_MSP02(MEfObjRecord *self);
int MEfCreate_GAMERA(MEfObjRecord *self);
int MEfCreate_DORA(MEfObjRecord *self);

#endif /* SRC_OV01_M_EF_CREATE_H */
