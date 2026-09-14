/*
 * OV01 original TU 30: 0x00a37ab8..0x00a38190 (4 functions)
 */
#include "common.h"

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ecm_01", MEfCreate_ECM01);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ecm_01", fnECM01_PR000);

ACCEPTED_ASM("src/ov01/m_ef_create_ecm_01", fnECM01_DP000);

INCLUDE_ASM("asm/nonmatchings/ov01/m_ef_create_ecm_01", fnECM01_PO000);
