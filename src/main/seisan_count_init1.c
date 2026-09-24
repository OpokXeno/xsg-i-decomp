#include "common.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "seisan_count_init1.h"

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanCountInit1);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanCountInit2);

int SeisanNumberCount(int current, int target, int step, int *moving)
{
  int overshot;
  int next;
  next = current;
  if (next != target)
  {
    if (next < target)
    {
      next += step;
      overshot = target < next;
    }
    else
    {
      next -= step;
      overshot = next < target;
    }
    if (overshot)
    {
      next = target;
    }
    else
    {
      *moving = 1;
    }
  }
  return next;
}

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanCountMain);

void TskObjectSet2(TskObject *task, TskObjectWorker worker, void *data)
{
    task->data = data;
    task->worker = worker;
    task->state = 0;
}

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

int subSeisanEtherCheck(void)
{
    return 0;
}

int subSeisanItemCheck(void)
{
    return 0x80;
}

void subSeisanCheck(void)
{
    switch (SeisanWork[SEISAN_WORK_STATE]) {
    case 0x20:
        SeisanWork[SEISAN_WORK_NEXT_STATE] =
            subSeisanHissatuCheck00(SeisanWork);
        if (SeisanWork[SEISAN_WORK_NEXT_STATE] != 0)
            break;
        /* fallthrough */
    case 0x50:
        SeisanWork[SEISAN_WORK_NEXT_STATE] = subSeisanHissatuCheck01();
        if (SeisanWork[SEISAN_WORK_NEXT_STATE] != 0)
            break;
        /* fallthrough */
    case 0x58:
        SeisanWork[SEISAN_WORK_NEXT_STATE] = subSeisanEtherCheck();
        if (SeisanWork[SEISAN_WORK_NEXT_STATE] != 0)
            break;
        /* fallthrough */
    case 0x60:
        SeisanWork[SEISAN_WORK_NEXT_STATE] = subSeisanItemCheck();
        if (SeisanWork[SEISAN_WORK_NEXT_STATE] != 0)
            break;
        /* fallthrough */
    case 0x80:
        SeisanWork[SEISAN_WORK_NEXT_STATE] = SEISAN_STATE_CLOSING;
        SeisanWork[SEISAN_WORK_FLAGS] |= 4;
        break;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", subSeisanMain);

int SeisanMain(void)
{
    subSeisanMain();
    return SeisanWork[SEISAN_WORK_STATE] != SEISAN_STATE_FINISHED;
}

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanDisp);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanInit);
