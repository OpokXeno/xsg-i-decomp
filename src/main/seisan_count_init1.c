#include "common.h"

#include "shared.h"

#include "main/xgl_task.h"

#include "seisan_count_init1.h"

const SeisanRibbonData D_004C7350 = {
    { 0x00000000, 0x0000FFFF, 0x80000000, 0x01C00000,
      0x0000FFFF, 0x80000000, 0x00000200, 0x0000FFFF,
      0x80000000, 0x01C00200, 0x0000FFFF, 0x80000000,
      0x00000000, 0x00000000, 0x00000000 }
};

unsigned char *SeisanWork = 0;

XglTaskScheduler *SeisanBgTask = 0;

XglTaskScheduler *SeisanTask = 0;

SeisanResultData *SeisanResult = 0;

static SeisanCountData SeisanCN;

/* The original eight-byte prompt-pointer table remains in data scaffolding. */

extern const char *msg00_1_0036DAC8[];

void SeisanCountInit1(void)
{
    int chrNo;

    memset(&SeisanCN, 0, sizeof(SeisanCN));
    for (chrNo = 1; chrNo < 13; chrNo++)
    {
        SeisanCN.nextExp[chrNo - 1] = func_A19210(chrNo)->nextExp;
    }
}

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

static void tskSeisanButton(TskObject *task, SeisanButtonWork *button)
{
    short targetY;

    switch (task->state) {
    case 0: {
        button->color = 0xfffff0;
        eSpriteSet(&button->sprite, 0x200);
        button->sprite.x = -0x70;
        button->sprite.y = 0x1a4;
        button->sprite.color = button->color;
        eMessageSet(&button->message, msg00_1_0036DAC8[0]);
        button->message.mode = 0x20;
        button->message.x = button->sprite.x + 0x14;
        button->message.y = button->sprite.y;
        button->message.color = button->color;
        break;
    }
    case 2: {
        unsigned char *work = SeisanWork;
        unsigned char flags = work[SEISAN_WORK_FLAGS];
        targetY = 0x180;
        if (flags & 4)
            button->message.text = msg00_1_0036DAC8[1];
        if (work[SEISAN_WORK_STATE] == SEISAN_STATE_CLOSING)
            targetY = 0x220;
        MoveSlide(&button->sprite.x, &targetY, 3.0f);
        eSpriteMain(&button->sprite);
        button->message.x = button->sprite.x + 0x14;
        button->message.y = button->sprite.y;
        eMessageMain(&button->message);
        break;
    }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", tskSeisanItem);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", tskSeisanStatus);

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanFadeMain);

int subSeisanHissatuCheck00(unsigned char *seisan_work)
{
    int i;

    for (i = 0; i < SeisanWork[6]; i++)
    {
        if (SeisanResult->entries[i].count != 0)
        {
            return 0x50;
        }
    }
    return 0;
}

int subSeisanHissatuCheck01(void)
{
    int i;

    for (i = 0; i < SeisanWork[6]; i++)
    {
        if (SeisanResult->entries[i].count >= 5)
        {
            return 0x58;
        }
    }
    return 0;
}

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

void subSeisanMain(void)
{
    if (SeisanWork[1] != SeisanWork[3]) {
        SeisanWork[1] = SeisanWork[3];
        SeisanWork[2] |= 1;
    } else {
        SeisanWork[2] &= 0xfe;
    }
    if (SeisanWork[5] != 0)
        SeisanWork[5]--;

    switch (SeisanWork[1]) {
    case 0x20:
        if ((unsigned char)(SeisanWork[2] & 1)) {
            SeisanWork[5] = 0x20;
            subSeisanCheck();
        }
        if (SeisanWork[5] == 0 && SeisanCN.state == 0)
            SeisanCN.state = 1;
        if ((PadData.half_2a & 0x20) && SeisanWork[5] == 0) {
            if (SeisanCN.state == 3)
                SeisanWork[3] = SeisanWork[4];
            else
                SeisanCN.state = 2;
        }
        SeisanCountMain(0);
        break;
    case 0x30:
        break;
    case 0x50:
        if ((unsigned char)(SeisanWork[2] & 1)) {
            SeisanWork[5] = 16;
            subSeisanCheck();
        }
        if ((PadData.half_2a & 0x20) && SeisanWork[5] == 0)
            SeisanWork[3] = SeisanWork[4];
        break;
    case 0x58:
        if ((unsigned char)(SeisanWork[2] & 1)) {
            SeisanWork[5] = 16;
            subSeisanCheck();
        }
        if ((PadData.half_2a & 0x20) && SeisanWork[5] == 0)
            SeisanWork[3] = SeisanWork[4];
        break;
    case 0x60:
        if ((unsigned char)(SeisanWork[2] & 1)) {
            SeisanWork[5] = 16;
            subSeisanCheck();
        }
        if ((PadData.half_2a & 0x20) && SeisanWork[5] == 0)
            SeisanWork[3] = SeisanWork[4];
        break;
    case 0x80:
        if ((unsigned char)(SeisanWork[2] & 1)) {
            SeisanWork[5] = 16;
            subSeisanCheck();
        }
        if ((PadData.half_2a & 0x20) && SeisanWork[5] == 0)
            SeisanWork[3] = SeisanWork[4];
        break;
    case 0xf0:
        if ((unsigned char)(SeisanWork[2] & 1))
            SeisanWork[5] = 16;
        if (SeisanWork[5] == 0) {
            unsigned char *work;
            MenuBgTaskBreak();
            work = SeisanWork;
            work[3] = 0xff;
            work[1] = 0xff;
        } else {
            SeisanWork[5]--;
        }
        break;
    }
    SeisanWork[2] &= 0xfd;
}

int SeisanMain(void)
{
    subSeisanMain();
    return SeisanWork[SEISAN_WORK_STATE] != SEISAN_STATE_FINISHED;
}

void SeisanDisp(void)
{
    SeisanRibbonData ribbon;

    ribbon = D_004C7350;
    endPrintDirectRibbon(&ribbon);
    endPrintExtFunc(0, 100, 0);
    if (MenuLoadSync())
    {
        xglTaskExecute(SeisanBgTask);
    }
    else
    {
        MenuBgTaskMain();
        xglTaskExecute(SeisanBgTask);
        xglTaskExecute(SeisanTask);
        SeisanFadeMain();
    }
}

INCLUDE_ASM("asm/main/nonmatchings/seisan_count_init1", SeisanInit);
