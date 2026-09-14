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

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objEntry);

ObjectTask *objEntry2(void *argument, ObjectTaskCallback callback) {
    ObjectTask *task;

    task = objEntrySub(&taskMan, argument, 0);
    if (task != 0) {
        ((ObjectTaskNode *)task)->exec = callback;
    }
    return task;
}

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objEntrySub);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objEntryRev);

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

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objCmdClear);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objCmdPtrGet);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objCmdTailGet);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objCmdPush);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objCmdPop);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objCmdNext);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", fifoInit);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", fifoPush);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", fifoPop);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", fifoEdGet);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", fifoStGet);

INCLUDE_ASM("asm/nonmatchings/ov01/obj", fifoNumGet);
