/*
 * OV01 original TU 31: 0x00a38190..0x00a387a8 (5 functions)
 */
#include "common.h"
#include "shared.h"

extern void MMathAddRotateVectorY(void *destination, float angle, const Vector4 *base, const Vector4 *offset);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ecm_02", MEfCreate_ECM02);

static void makeCoord(void *destination, float radius, float angle, void *effect)
{
    unsigned char *base = (unsigned char *)effect;
    Vector4 local;

    /* VF00 is hardwired to (0.0, 0.0, 0.0, 1.0); this
     * materializes that constant into "local" in one quadword store instead of
     * four scalar stores. Only z (0.0) and w (1.0) survive from this store: x
     * and y are overwritten immediately below. */
    __asm__ __volatile__(
        "sqc2 $vf0, 0(%1)"
        : "=m"(local)
        : "r"(&local)
        : "memory"
    );

    local.x = radius * 0.454545468f + 0.5f;
    local.y = -radius;

    MMathAddRotateVectorY(destination, angle, (Vector4 *)(base + 0x80), &local);
}

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ecm_02", fnECM02_PR000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ecm_02", fnECM02_DP000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ecm_02", fnECM02_PO000);
