#include "common.h"
#include "shared.h"
#include "stage_1.h"

/* Twenty-four halfwords occupy the stage peer block returned by STAGE_create. */
u16 defaultStage[24];

void *STAGE_create(u16 stage_id)
{
    defaultStage[2] = stage_id;
    return defaultStage;
}

INCLUDE_ASM("asm/main/nonmatchings/stage_1", STAGE_instance);
