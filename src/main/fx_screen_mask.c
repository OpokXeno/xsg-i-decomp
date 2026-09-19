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

void FX_ScreenMask(int unused, int color, int duration, int mode)
{
    XglTaskScheduler *scheduler = GameLoopState[2];
    ScreenMaskTask *task = (ScreenMaskTask *)xglTaskEntryNext(
        scheduler, (int (*)(XglTaskPrefix *))screenMask,
        scheduler != 0 ? scheduler->active_tail : 0);

    (void)unused;
    if (task != 0) {
        task->state = GameLoopState;
        task->flags = 0;
        task->next_callback = 0;
    }
    task->color = color;
    task->duration = duration;
    task->mode = mode;
}

INCLUDE_ASM("asm/main/nonmatchings/fx_screen_mask", FX_call);
