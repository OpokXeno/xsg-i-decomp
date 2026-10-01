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

void FX_call(int command, int effect_index, const void *arguments,
             int argument_count)
{
    int (*adapter_callback)(XglTaskPrefix *) =
        fxAdapter;
    XglTaskScheduler *scheduler = GameLoopState[2];
    XglTaskPrefix *active_tail =
        scheduler != 0 ? scheduler->active_tail : 0;
    FxAdapterTask *task = (FxAdapterTask *)xglTaskEntryNext(
        scheduler, adapter_callback, active_tail);
    const int *callback_arguments = arguments;
    int i;

    if (task != 0) {
        task->flags = 0;
        task->state = GameLoopState;
        task->next_callback = 0;
    }

    if (argument_count >= 5) {
        argument_count = 4;
    }
    task->countdown = (short)command;
    task->next_callback = fxFunction[effect_index];
    for (i = 0; i < argument_count; i++) {
        task->callback_arguments[i] = callback_arguments[i];
    }

    if ((callback_arguments[2] & 1) != 0) {
        nmlModelSetActiveFadeIn(callback_arguments[1], callback_arguments[0],
                                command);
    } else {
        nmlModelSetActiveFadeOut(callback_arguments[1], callback_arguments[0],
                                 command);
    }
}
