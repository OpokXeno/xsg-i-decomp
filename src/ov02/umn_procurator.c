/*
 * OV02 original TU 8: 0x00a04170..0x00a05738 (11 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_2.h"
#include "umn_procurator.h"

INCLUDE_ASM("asm/nonmatchings/ov02/umn_procurator", UmnManzaiControl);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_procurator", UmnManzaiInit);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_procurator", tskUmnManzaiWin);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_procurator", LocalLightSet);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_procurator", UmnProcuratorManzai2);

static void Rotation(float *out, float *position, float *base, float *rotation)
{
    float matrix[16];

    xglMatrixStackUnit();
    xglMatrixStackRotZ(rotation[2]);
    xglMatrixStackRotY(rotation[1]);
    xglMatrixStackRotX(rotation[0]);
    xglMatrixStackTrans(position);
    xglMatrixStackSave((float (*)[4])matrix);

    out[0] = base[0] + matrix[12];
    out[1] = base[1] + matrix[13];
    out[2] = base[2] + matrix[14];
}

INCLUDE_ASM("asm/nonmatchings/ov02/umn_procurator", UmnProcuratorManzai);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_procurator", UmnProcuratorMail);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_procurator", UmnProcuratorTop);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_procurator", UmnProcuratorGoodBy);

INCLUDE_ASM("asm/nonmatchings/ov02/umn_procurator", UmnProcuratorSet);
