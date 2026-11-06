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

#include "ov01/m_ef_create.h"

#define MEF_TYPE_COUNT 14

typedef int (*MEfObjCtor)(MEfObjRecord *self);

/* Constructors of the effect TUs that have no header of their own; this
 * factory (m_ef_create.c) is their only C caller. */
int MEfCreate_MSP00(MEfObjRecord *self);
int MEfCreate_SMP01(MEfObjRecord *self);
int MEfCreate_MSP02(MEfObjRecord *self);
int MEfCreate_GAMERA(MEfObjRecord *self);
int MEfCreate_DORA(MEfObjRecord *self);

#endif /* SRC_OV01_M_EF_CREATE_H */
