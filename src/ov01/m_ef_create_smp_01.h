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

/* Fields used by the SMP01 model-draw callback. */
typedef struct Smp01Work {
    unsigned char unmodeled_000[0x40]; /* 0x000 */
    int modelEntry;                    /* 0x040 */
    const char *textureName;           /* 0x044 */
    unsigned char unmodeled_048[0x28]; /* 0x048 */
    int frame;                         /* 0x070 */
    int phase;                         /* 0x074 */
    int phaseCount;                    /* 0x078 */
    unsigned char unmodeled_07C[4];    /* 0x07C */
    Vector4 spawnPoint;                /* 0x080 */
    Vector4 phaseStart;                /* 0x090 */
    Vector4 targetPosition;            /* 0x0A0 */
    Vector4 position;                  /* 0x0B0 */
    Vector4 previousPosition;          /* 0x0C0 */
    Vector4 angles;                    /* 0x0D0 */
    float phaseAngle;                  /* 0x0E0 */
} Smp01Work;

typedef struct Smp01Object {
    unsigned char unmodeled_000[4];
    void (*updateCallback)(struct Smp01Object *object, Smp01Work *work); /* 0x004 */
} Smp01Object;

#endif
