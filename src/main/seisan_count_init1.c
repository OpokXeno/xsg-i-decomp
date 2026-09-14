#include "common.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "seisan_count_init1.h"

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanCountInit1);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanCountInit2);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanNumberCount);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanCountMain);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", TskObjectSet2);

void tskTskMain2(TskObject *task)
{
    TskObjectWorker worker = task->worker;

    if (SeisanWork[1] == 0xff) {
        xglTaskWaitRemove(&task->base);
        return;
    }

    if (task->state != 0) {
        if (task->state != 2)
            return;
    } else {
        worker(task, task->data);
        task->state = 2;
    }

    worker(task, task->data);
}

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanTimeEx);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", tskSeisanButton);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", tskSeisanItem);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", tskSeisanStatus);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanFadeMain);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", subSeisanHissatuCheck00);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", subSeisanHissatuCheck01);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", subSeisanEtherCheck);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", subSeisanItemCheck);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", subSeisanCheck);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", subSeisanMain);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanMain);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanDisp);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanInit);
