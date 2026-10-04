/*
 * OV01 original TU 1: 0x00a00218..0x00a00d60 (30 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "ov01/obj.h"
#include "obj.h"

extern void *memset(void *destination, int value, unsigned int size);
extern void *xglTaskInitial(void *manager, int capacity, int flags);
extern void objWorkInit(void);

static ObjectWork objWork[60];
static TaskManager taskMan;
/* objInit clears fifty 0x80-byte slots. ObjectTaskNode models each prefix. */
typedef union ObjectTaskStorage {
    ObjectTaskNode node;
    unsigned char bytes[0x80];
} ObjectTaskStorage;
static ObjectTaskStorage taskBuf[50];

const char objRemoveError[24] = "** objRemove: Error\n";
const char D_00A43810[24] = "** objEntryPure: Error\n";
const char objRemovePureError[32] = "** objRemovePure: Error\n";
const char D_00A43848[24] = "** objWorkGet: Error\n";
const char D_00A43860[32] = "** objWorkFree: Error %x\n";
const char D_00A43880[32] = "** objCmdPush: buff over\n";
const char D_00A438A0[32] = "** objCmdPop: buff over\n";
const char D_00A438C0[24] = "** fifoPush error\n";
const char D_00A438D8[24] = "** fifoPop error\n";

void objInit(void) {
    int index;
    unsigned char *slot;

    slot = taskBuf[0].bytes;
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

ObjectTask *objEntry2Rev(void *argument, ObjectTaskCallback callback) {
    ObjectTask *task;

    task = objEntrySub(&taskMan, argument, 1);
    if (task != 0) {
        ((ObjectTaskNode *)task)->exec = callback;
    }
    return task;
}

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

ObjectTask *objEntryPure(ObjectTaskCallback callback)
{
    XglTaskScheduler *scheduler;
    XglTaskPrefix *entry;
    XglTaskPrefix *allocated;
    ObjectTask *task;

    /* TaskManager and XglTaskScheduler expose the same four pointer slots;
     * this boundary uses the canonical scheduler-prefix API. The object
     * callback is stored, then later called by objExec2 with its local type. */
    scheduler = (XglTaskScheduler *)&taskMan;
    entry = 0;
    if (scheduler != 0) {
        entry = scheduler->active_tail;
    }
    allocated = xglTaskEntryNext(scheduler,
                                 (int (*)(XglTaskPrefix *))callback,
                                 entry);
    task = (ObjectTask *)allocated;
    if (task != 0) {
        return task;
    }
    printf(D_00A43810);
    return 0;
}

void objRemovePure(ObjectTask *task)
{
    if (task == 0) {
        printf(objRemovePureError);
        return;
    }
    xglTaskRemove(&task->task);
}

void objWorkInit(void)
{
    int index;

    for (index = 59; index >= 0; index--) {
        objWork[index].used = 0;
    }
}

ObjectWork *objWorkGet(void)
{
    ObjectWork *slot;
    int count;

    count = 0;
    slot = objWork;
    do {
        if (!(slot->used & 1)) {
            slot->used = 1;
            return slot;
        }
        count++;
        slot++;
    } while (count < 60);
    printf(D_00A43848);
    return 0;
}

void objWorkFree(void *work) {
    ObjectWork *slot;

    slot = work;
    if (slot < objWork || &objWork[60] < slot) {
        printf(D_00A43860, work);
        return;
    }
    slot->used = 0;
}

void objStdInit(ObjectTask *task)
{
    ObjectWorkTransform *transform;

    transform = &((ObjectWork *)task->work)->transform;
    memset(transform, 0, sizeof(*transform));
    transform->scale.w = 1.0f;
    transform->scale.z = 1.0f;
    transform->scale.y = 1.0f;
    transform->scale.x = 1.0f;
    transform->homogeneousVector.w = 1.0f;
    objCmdClear(task);
}

INCLUDE_ASM("asm/nonmatchings/ov01/obj", objStdMove);

/*
 * Aim an object at its target: give each axis the velocity that closes the
 * distance still to go in `steps` updates, then put an axis already within half
 * a unit of its target straight onto it, so the next objStdMove steps report it
 * as arrived.  Each axis compares the magnitude of its own remaining distance,
 * which the two sides of the sign test reach by different values: the negative
 * side negates the distance, the positive side compares it as it stands.
 */
void objHatoVec(ObjectTask *task, int steps)
{
    typedef struct {
        Vector4 homogeneousVector;
        unsigned char unmodeled_10[0x20];
        Vector4 scale;
        Vector4 velocity;
        unsigned char unmodeled_50[0x10];
        Vector4 target;
        unsigned char unmodeled_70[0x10];
    } MotionTransform;
    typedef struct {
        int used;
        unsigned char unmodeled_04[0x8C];
        MotionTransform transform;
    } MotionWork;
    MotionWork *work;
    MotionTransform *transform;
    float deltaX;
    float deltaY;
    float deltaZ;
    float magnitudeX;
    float magnitudeY;
    float magnitudeZ;

    work = task->work;
    transform = &work->transform;
    deltaX = transform->target.x - transform->homogeneousVector.x;
    transform->velocity.x = deltaX / steps;
    transform->velocity.y = (transform->target.y - transform->homogeneousVector.y) / steps;
    transform->velocity.z = (transform->target.z - transform->homogeneousVector.z) / steps;

    if ((deltaX < 0.0f ? (magnitudeX = -deltaX) : deltaX) < 0.5f) {
        transform->homogeneousVector.x = transform->target.x;
    }

    if (((deltaY = transform->target.y - transform->homogeneousVector.y) < 0.0f
             ? (magnitudeY = -deltaY) : deltaY) < 0.5f) {
        transform->homogeneousVector.y = transform->target.y;
    }

    if (((deltaZ = transform->target.z - transform->homogeneousVector.z) < 0.0f
             ? (magnitudeZ = -deltaZ) : deltaZ) < 0.5f) {
        transform->homogeneousVector.z = transform->target.z;
    }
}

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

void *objCmdPush(ObjectTask *task) {
    ObjectCommandQueue *queue;
    int index;

    queue = task->work;
    index = queue->readIndex;
    queue->readIndex = index + 1;
    if (queue->readIndex >= 5) {
        queue->readIndex = 4;
        printf(D_00A43880);
        return 0;
    }
    return &queue->entries[index];
}

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

int objCmdNext(ObjectTask *task)
{
    ObjectCommandQueue *queue;
    int index;

    queue = task->work;
    index = queue->writeIndex + 1;
    queue->writeIndex = index;
    if (index >= queue->readIndex) {
        objCmdClear(task);
        return 0;
    }
    queue->word70 = 0;
    queue->word74 = 0;
    return 1;
}

void fifoInit(Fifo *fifo, int capacity) {
    fifo->base = 0;
    fifo->maxIndex = capacity - 1;
    fifo->readIndex = 0;
    fifo->writeIndex = 0;
    fifo->count = 0;
}

int fifoPush(Fifo *fifo) {
    int count;
    int capacity;
    int writeIndex;

    count = fifo->count;
    capacity = (fifo->maxIndex - fifo->base) + 1;
    if (count >= capacity) {
        printf(D_00A438C0);
        return -1;
    }
    writeIndex = fifo->writeIndex + 1;
    fifo->count = count + 1;
    fifo->writeIndex = writeIndex;
    if (fifo->maxIndex < writeIndex) {
        fifo->writeIndex = fifo->base;
    }
    return fifo->writeIndex;
}

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
