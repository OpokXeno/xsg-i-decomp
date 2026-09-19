/*
 * OV01 original TU 1: 0x00a00218..0x00a00d60 (30 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "ov01/obj.h"
#include "obj.h"

extern unsigned char taskBuf[];
extern void *memset(void *destination, int value, unsigned int size);
extern void xglTaskInitial(void *manager, int capacity, int flags);
extern void objWorkInit(void);

void objInit(void) {
    int index;
    unsigned char *slot;

    slot = taskBuf;
    for (index = 0; index < 50; index++) {
        memset(slot, 0, 0x80);
        slot += 0x80;
    }
    xglTaskInitial(&taskMan, 50, 0);
    objWorkInit();
}

void objExec(void) {
    objExecSub(&taskMan);
}

void objExecSub(void *manager) {
    xglTaskExecute(manager);
}

void objExec2(void) {
    TaskManager *manager;
    ObjectTaskNode *task;

    manager = &taskMan;
    task = (ObjectTaskNode *)manager->active_head;
    while (task != 0) {
        ObjectTask *next;
        ObjectTaskCallback callback;

        next = (ObjectTask *)task->base.task.next;
        callback = task->exec;
        manager->next_to_visit = next;
        if (callback != 0) {
            callback(&task->base);
        }
        task = (ObjectTaskNode *)manager->next_to_visit;
    }
}

void objEntry(void *argument) {
    objEntrySub(&taskMan, argument, 0);
}

ObjectTask *objEntry2(void *argument, ObjectTaskCallback callback) {
    ObjectTask *task;

    task = objEntrySub(&taskMan, argument, 0);
    if (task != 0) {
        ((ObjectTaskNode *)task)->exec = callback;
    }
    return task;
}

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objEntrySub);

void objEntryRev(void *argument) {
    objEntrySub(&taskMan, argument, 1);
}

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objEntry2Rev);

void objRemove(ObjectTask *task)
{
    if (task == 0) {
        printf(objRemoveError);
        return;
    }

    ((ObjectTaskNode *)task)->exec = 0;
    objWorkFree(task->work);
    xglTaskRemove(&task->task);
}

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objEntryPure);

void objRemovePure(ObjectTask *task)
{
    if (task == 0) {
        printf(objRemovePureError);
        return;
    }
    xglTaskRemove(&task->task);
}

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objWorkInit);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objWorkGet);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objWorkFree);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objStdInit);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objStdMove);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objHatoVec);

void objCmdClear(ObjectTask *task) {
    ObjectCommandQueue *queue;

    queue = task->work;
    queue->entries[0].selector = 0;
    queue->writeIndex = 0;
    queue->readIndex = 0;
    queue->word70 = 0;
    queue->word74 = 0;
    queue->flags &= ~OBJCMD_QUEUE_PENDING;
}

void *objCmdPtrGet(ObjectTask *task) {
    ObjectCommandQueue *queue;

    queue = task->work;
    return &queue->entries[queue->writeIndex];
}

void *objCmdTailGet(ObjectTask *task) {
    ObjectCommandQueue *queue;

    queue = task->work;
    return &queue->entries[queue->readIndex];
}

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objCmdPush);

#define OBJ_DEBUG_PRINT(args) do { printf args; } while (0)

void *objCmdPop(ObjectTask *task) {
    ObjectCommandQueue *queue;
    int index;

    queue = task->work;
    index = queue->readIndex;
    queue->readIndex = index - 1;
    if (queue->readIndex < 0) {
        queue->readIndex = 0;
        OBJ_DEBUG_PRINT((D_00A438A0, queue));
        return 0;
    }
    return &queue->entries[index];
}

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objCmdNext);

void fifoInit(Fifo *fifo, int capacity) {
    fifo->base = 0;
    fifo->maxIndex = capacity - 1;
    fifo->readIndex = 0;
    fifo->writeIndex = 0;
    fifo->count = 0;
}

INCLUDE_ASM("asm/nonmatchings/ov01/obj", fifoPush);

int fifoPop(Fifo *fifo) {
    int count;
    int newCount;
    int readIndex;

    count = fifo->count;
    newCount = count - 1;
    if (count <= 0) {
        printf(D_00A438D8, newCount);
        return -1;
    }
    readIndex = fifo->readIndex + 1;
    fifo->count = newCount;
    fifo->readIndex = readIndex;
    if (fifo->maxIndex < readIndex) {
        fifo->readIndex = fifo->base;
    }
    return fifo->readIndex;
}

int fifoEdGet(Fifo *fifo) {
    return fifo->writeIndex;
}

int fifoStGet(Fifo *fifo) {
    return fifo->readIndex;
}

int fifoNumGet(Fifo *fifo) {
    return fifo->count;
}
