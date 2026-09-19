/*
 * TU-local declarations of ov01/tu025 (src/ov01/m_ef_create_smp_01.c).
 */

#ifndef SRC_OV01_M_EF_CREATE_SMP_01_H
#define SRC_OV01_M_EF_CREATE_SMP_01_H

#include "shared.h"

/* The SMP01 work area fnSMP01_PO000 (the object's post-process callback)
 * is handed: it reaches only the frame counter at +0x70, which it
 * increments once per call, firing sefHitEffect at 15 and MEfObjDestroy
 * at 25. The rest of the record belongs to the other bodies of this TU,
 * still unrecovered, so it stays unmodeled here.
 */
typedef struct Smp01State {
    unsigned char unmodeled_000[0x70]; /* 0x000 */
    int frame;                         /* 0x070 */
} Smp01State;

#endif
