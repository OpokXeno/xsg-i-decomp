/*
 * OV01 original TU 1: 0x00a00218..0x00a00d60 (30 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "ov01/obj.h"
#include "obj.h"

/* A transform arrives once each of its three axes (x, y, z) has reached its target. */
#define OBJ_AXIS_COUNT 3

/* objInit clears fifty 0x80-byte slots. ObjectTaskNode models each prefix. */
typedef union ObjectTaskStorage {
    ObjectTaskNode node;
    unsigned char bytes[OBJ_TASK_SLOT_SIZE];
} ObjectTaskStorage;

static ObjectWork objWork[OBJ_WORK_COUNT];
static XglTaskScheduler taskMan;
static ObjectTaskStorage taskBuf[OBJ_TASK_CAPACITY];

const char objRemoveError[24] = "** objRemove: Error\n";
const char objEntryPureErrorText[24] = "** objEntryPure: Error\n";
const char objRemovePureError[32] = "** objRemovePure: Error\n";
const char objWorkGetErrorText[24] = "** objWorkGet: Error\n";
const char objWorkFreeErrorText[32] = "** objWorkFree: Error %x\n";
const char objCmdPushBuffOverText[32] = "** objCmdPush: buff over\n";
const char objCmdPopBuffOverText[32] = "** objCmdPop: buff over\n";
const char objFifoPushErrorText[24] = "** fifoPush error\n";
const char objFifoPopErrorText[24] = "** fifoPop error\n";

/* Clears every task slot, then initializes the task scheduler and the work pool. */
void objInit(void) {
    int index;
    unsigned char *slot;

    slot = taskBuf[0].bytes;
    for (index = 0; index < OBJ_TASK_CAPACITY; index++) {
        memset(slot, 0, OBJ_TASK_SLOT_SIZE);
        slot += OBJ_TASK_SLOT_SIZE;
    }
    xglTaskInitial(&taskMan, OBJ_TASK_CAPACITY, 0);
    objWorkInit();
}

void objExec(void) {
    objExecSub(&taskMan);
}

void objExecSub(XglTaskScheduler *manager) {
    xglTaskExecute(manager);
}

/* Runs the exec callback of each active task. The next task is read from next_to_visit after each callback. */
void objExec2(void) {
    XglTaskScheduler *manager;
    ObjectTaskNode *task;

    manager = &taskMan;
    task = (ObjectTaskNode *)manager->active_head;
    while (task != 0) {
        XglTaskPrefix *next;
        ObjectTaskCallback callback;

        next = task->base.task.next;
        callback = task->exec;
        manager->next_to_visit = next;
        if (callback != 0) {
            callback(&task->base);
        }
        task = (ObjectTaskNode *)manager->next_to_visit;
    }
}

void objEntry(ObjectSchedulerCallback schedulerCallback) {
    objEntrySub(&taskMan, schedulerCallback, 0);
}

ObjectTask *objEntry2(ObjectSchedulerCallback schedulerCallback, ObjectTaskCallback callback) {
    ObjectTask *task;

    task = objEntrySub(&taskMan, schedulerCallback, 0);
    if (task != 0) {
        ((ObjectTaskNode *)task)->exec = callback;
    }
    return task;
}

/* Allocates a task at the tail of manager, or at the head when reverse is set, and gives it a work slot. Returns 0 on failure. */
ObjectTask *objEntrySub(XglTaskScheduler *manager, ObjectSchedulerCallback schedulerCallback, int reverse)
{
    XglTaskPrefix *entry;
    XglTaskPrefix *allocated;
    ObjectWork *work;

    if (reverse == 0) {
        entry = manager ? manager->active_tail : 0;
        allocated = xglTaskEntryNext(manager, schedulerCallback, entry);
    } else {
        entry = manager ? manager->active_head : 0;
        allocated = xglTaskEntryPrev(manager, schedulerCallback, entry);
    }
    work = objWorkGet();
    if (allocated == 0 || work == 0) {
        printf(objEntryErrorText);
        return 0;
    }
    ((ObjectTaskNode *)allocated)->exec = 0;
    ((ObjectTaskNode *)allocated)->base.work = work;
    return (ObjectTask *)allocated;
}

void objEntryRev(ObjectSchedulerCallback schedulerCallback) {
    objEntrySub(&taskMan, schedulerCallback, 1);
}

ObjectTask *objEntry2Rev(ObjectSchedulerCallback schedulerCallback, ObjectTaskCallback callback) {
    ObjectTask *task;

    task = objEntrySub(&taskMan, schedulerCallback, 1);
    if (task != 0) {
        ((ObjectTaskNode *)task)->exec = callback;
    }
    return task;
}

/* Frees the task's work slot and removes the task from its scheduler. */
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

/* Allocates a task in the default scheduler without a work slot. Returns 0 on failure. */
ObjectTask *objEntryPure(ObjectSchedulerCallback callback)
{
    XglTaskScheduler *scheduler;
    XglTaskPrefix *entry;
    XglTaskPrefix *allocated;
    ObjectTask *task;

    /* The object callback is stored by the scheduler prefix, then later called
     * by objExec2 with its local type. */
    scheduler = &taskMan;
    entry = 0;
    if (scheduler != 0) {
        entry = scheduler->active_tail;
    }
    allocated = xglTaskEntryNext(scheduler, callback, entry);
    task = (ObjectTask *)allocated;
    if (task != 0) {
        return task;
    }
    printf(objEntryPureErrorText);
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

    for (index = OBJ_WORK_COUNT - 1; index >= 0; index--) {
        objWork[index].used = 0;
    }
}

/* Returns the first free work slot and marks it in use, or 0 when all are in use. */
ObjectWork *objWorkGet(void)
{
    ObjectWork *slot;
    int count;

    count = 0;
    slot = objWork;
    do {
        if (!(slot->used & OBJ_WORK_IN_USE)) {
            slot->used = OBJ_WORK_IN_USE;
            return slot;
        }
        count++;
        slot++;
    } while (count < OBJ_WORK_COUNT);
    printf(objWorkGetErrorText);
    return 0;
}

/* Marks a work slot as free. Prints an error when work is outside the pool. */
void objWorkFree(ObjectWork *work) {
    if (work < objWork || &objWork[OBJ_WORK_COUNT] < work) {
        printf(objWorkFreeErrorText, work);
        return;
    }
    work->used = 0;
}

/* Resets the task's transform to unit scale and zero translation, then clears its command queue. */
void objStdInit(ObjectTask *task)
{
    ObjectWork *work;
    ObjectWorkTransform *transform;

    work = task->work;
    transform = &work->transform;
    memset(transform, 0, sizeof(*transform));
    transform->scale.w = 1.0f;
    transform->scale.z = 1.0f;
    transform->scale.y = 1.0f;
    transform->scale.x = 1.0f;
    transform->homogeneousVector.w = 1.0f;
    objCmdClear(task);
}

/*
 * Advance an object one update: accelerate, move, and snap each axis onto its
 * target once it reaches it, overshoots it in the direction of travel, or lies
 * within 1e-6 of it.  Returns nonzero when all three axes have arrived.
 * magnitude is |distance|; each side of the sign test compares its own copy,
 * which is the shape the original's duplicated compare comes from.
 */
int objStdMove(ObjectTask *task)
{
    ObjectWork *work;
    ObjectWorkTransform *transform;
    int arrived;

    work = task->work;
    transform = &work->transform;
    arrived = 0;

    /* One block per axis: advance velocity and position, then snap onto the target when it has arrived. */
    {
        float velocity;
        float position;
        float distance;
        float magnitude;

        velocity = transform->velocity.x + transform->acceleration.x;
        position = transform->homogeneousVector.x + velocity;
        transform->velocity.x = velocity;
        transform->homogeneousVector.x = position;
        if (position == transform->target.x) {
            arrived++;
        } else if ((velocity > 0.0f && transform->target.x < position)
                   || (velocity < 0.0f && position < transform->target.x)
                   || ((distance = transform->target.x - position) < 0.0f
                           ? (magnitude = -distance, magnitude <= 1e-6f)
                           : (magnitude = distance, magnitude <= 1e-6f))) {
            transform->homogeneousVector.x = transform->target.x;
            arrived++;
        }
    }

    {
        float velocity;
        float position;
        float distance;
        float magnitude;

        velocity = transform->velocity.y + transform->acceleration.y;
        position = transform->homogeneousVector.y + velocity;
        transform->velocity.y = velocity;
        transform->homogeneousVector.y = position;
        if (position == transform->target.y) {
            arrived++;
        } else if ((velocity > 0.0f && transform->target.y < position)
                   || (velocity < 0.0f && position < transform->target.y)
                   || ((distance = transform->target.y - position) < 0.0f
                           ? (magnitude = -distance, magnitude <= 1e-6f)
                           : (magnitude = distance, magnitude <= 1e-6f))) {
            transform->homogeneousVector.y = transform->target.y;
            arrived++;
        }
    }

    {
        float velocity;
        float position;
        float distance;
        float magnitude;

        velocity = transform->velocity.z + transform->acceleration.z;
        position = transform->homogeneousVector.z + velocity;
        transform->velocity.z = velocity;
        transform->homogeneousVector.z = position;
        if (position == transform->target.z) {
            arrived++;
        } else if ((velocity > 0.0f && transform->target.z < position)
                   || (velocity < 0.0f && position < transform->target.z)
                   || ((distance = transform->target.z - position) < 0.0f
                           ? (magnitude = -distance, magnitude <= 1e-6f)
                           : (magnitude = distance, magnitude <= 1e-6f))) {
            transform->homogeneousVector.z = transform->target.z;
            arrived++;
        }
    }

    return arrived == OBJ_AXIS_COUNT;
}

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
    ObjectWork *work;
    ObjectWorkTransform *transform;
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

/* Empties the task's command queue. */
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

ObjectCommandEntry *objCmdPtrGet(ObjectTask *task) {
    ObjectCommandQueue *queue;

    queue = task->work;
    return &queue->entries[queue->writeIndex];
}

ObjectCommandEntry *objCmdTailGet(ObjectTask *task) {
    ObjectCommandQueue *queue;

    queue = task->work;
    return &queue->entries[queue->readIndex];
}

/* Returns the entry at the read index and advances it. Returns 0 with an error when the queue is full. */
ObjectCommandEntry *objCmdPush(ObjectTask *task) {
    ObjectCommandQueue *queue;
    int index;

    queue = task->work;
    index = queue->readIndex;
    queue->readIndex = index + 1;
    if (queue->readIndex >= OBJ_CMD_QUEUE_CAPACITY) {
        queue->readIndex = OBJ_CMD_QUEUE_CAPACITY - 1;
        printf(objCmdPushBuffOverText);
        return 0;
    }
    return &queue->entries[index];
}

#define OBJ_DEBUG_PRINT(args) do { printf args; } while (0)

/* Steps the read index back and returns the entry it passes. Returns 0 with an error at the bottom of the queue. */
ObjectCommandEntry *objCmdPop(ObjectTask *task) {
    ObjectCommandQueue *queue;
    int index;

    queue = task->work;
    index = queue->readIndex;
    queue->readIndex = index - 1;
    if (queue->readIndex < 0) {
        queue->readIndex = 0;
        OBJ_DEBUG_PRINT((objCmdPopBuffOverText, queue));
        return 0;
    }
    return &queue->entries[index];
}

/* Advances the write index and returns 1. Clears the queue and returns 0 when the write index reaches the read index. */
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

/* Empties a FIFO that holds capacity entries, with indices starting at base 0. */
void fifoInit(Fifo *fifo, int capacity) {
    fifo->base = 0;
    fifo->maxIndex = capacity - 1;
    fifo->readIndex = 0;
    fifo->writeIndex = 0;
    fifo->count = 0;
}

/* Adds one entry at the write index and returns the new write index. Returns -1 when the FIFO is full. */
int fifoPush(Fifo *fifo) {
    int count;
    int capacity;
    int writeIndex;

    count = fifo->count;
    capacity = (fifo->maxIndex - fifo->base) + 1;
    if (count >= capacity) {
        printf(objFifoPushErrorText);
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

/* Removes one entry at the read index and returns the new read index. Returns -1 when the FIFO is empty. */
int fifoPop(Fifo *fifo) {
    int count;
    int newCount;
    int readIndex;

    count = fifo->count;
    newCount = count - 1;
    if (count <= 0) {
        printf(objFifoPopErrorText, newCount);
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



const char objEntryErrorText[24] = "** objEntry: Error\n";
