#include "common.h"
#include "shared.h"
#include "fx_screen_mask.h"

static void screenMask(ScreenMaskTask *task)
{
    unsigned int next_frame;

    if ((task->flags & 1u) == 0u) {
        task->flags |= 1u;
        task->frame = -1;
    }
    nmlModelSetFadeDoit();
    next_frame = (unsigned short)task->frame + 1u;
    task->frame = (unsigned short)next_frame;
    if (task->duration < (short)next_frame) {
        xglTaskRemove(&task->entry);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/fx_screen_mask", fxAdapter);

INCLUDE_ASM("asm/main/nonmatchings/fx_screen_mask", FX_ScreenMask);

INCLUDE_ASM("asm/main/nonmatchings/fx_screen_mask", FX_call);
