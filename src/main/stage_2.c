#include "common.h"

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_start__ILjava_lang_Object_);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_stop__);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_play__Ljava_lang_String_);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setPartsLast__I);

/*
 * nmlModelSetMapLastInit is src/main/nml_model_set.c's own INCLUDE_ASM
 * function (main/tu... , still unresolved there); this TU only tail-calls
 * it, so it stays a local extern declaration until that TU converts it.
 */
void nmlModelSetMapLastInit(void);

void Java_xeno_Stage_setPartsLastReset__(void)
{
    nmlModelSetMapLastInit();
}

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setColor__FFF);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setFade__IIFFF);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setEventFade__IFFFIFFF);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setFrameRender__II);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setFadeCancel__I);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setVisible__IZ);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setCFBG__II_F);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setEffectRender__I);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_renderCommand__I);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setBgColor__FFF);

INCLUDE_ASM("asm/main/nonmatchings/stage_2", Java_xeno_Stage_setBgClip__I);

/*
 * nmlModelSetBackBufferClear is src/main/nml_model_set.c's own INCLUDE_ASM
 * function, still unresolved there; this TU only tail-calls it.
 */
void nmlModelSetBackBufferClear(void);

void Java_xeno_Stage_clrBackBuffer__(void)
{
    nmlModelSetBackBufferClear();
}
