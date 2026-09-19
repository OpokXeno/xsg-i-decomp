#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/light", Java_xeno_Light_setColor__FFF);

INCLUDE_ASM("asm/main/nonmatchings/light", Java_xeno_Light_setDirection__FFF);

INCLUDE_ASM("asm/main/nonmatchings/light", Java_xeno_Light_setDirection2__FFF);

INCLUDE_ASM("asm/main/nonmatchings/light", Java_xeno_Light_setGlobalPointLightCol__IFFF);

INCLUDE_ASM("asm/main/nonmatchings/light", Java_xeno_Light_setGlobalPointLightPos__IFFF);

/*
 * nmlModelSetGlobalPointLightReset is src/main/nml_model_set.c's own
 * INCLUDE_ASM function (main/tu106, still unresolved there); this TU only
 * tail-calls it, so it stays a local extern declaration until that TU
 * converts it.
 */
void nmlModelSetGlobalPointLightReset(void);

void Java_xeno_Light_setGlobalPointLightReset__(void)
{
    nmlModelSetGlobalPointLightReset();
}
