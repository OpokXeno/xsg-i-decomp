#include "common.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "xgl_task.h"

INCLUDE_ASM("asm/main/nonmatchings/xgl_task", xglTaskInitial);

XglTaskPrefix *xglTaskEntryPrev(XglTaskScheduler *scheduler,
                                int (*callback)(XglTaskPrefix *task),
                                XglTaskPrefix *entry)
{
    XglTaskPrefix *task;
    XglTaskPrefix *previous;

    if (scheduler == 0 || (task = scheduler->free_tasks) == 0)
        return 0;

    task->callback = callback;
    scheduler->free_tasks = task->next;
    task->scheduler = scheduler;

    task->next = entry;
    if (entry == 0) {
        task->previous = 0;
        scheduler->active_tail = task;
        scheduler->active_head = task;
    } else {
        previous = entry->previous;
        task->previous = previous;
        if (previous == 0) {
            scheduler->active_head = task;
        } else {
            entry->previous->next = task;
        }
        entry->previous = task;
    }
    return task;
}

XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                int (*callback)(XglTaskPrefix *task),
                                XglTaskPrefix *entry)
{
    XglTaskPrefix *task;
    XglTaskPrefix *next;

    if (scheduler == 0 || (task = scheduler->free_tasks) == 0)
        return 0;

    task->callback = callback;
    scheduler->free_tasks = task->next;
    task->scheduler = scheduler;

    task->previous = entry;
    if (entry == 0) {
        task->next = 0;
        scheduler->active_tail = task;
        scheduler->active_head = task;
    } else {
        next = entry->next;
        task->next = next;
        if (next == 0) {
            scheduler->active_tail = task;
        } else {
            entry->next->previous = task;
        }
        entry->next = task;
    }
    return task;
}

int xglTaskRemove(XglTaskPrefix *task)
{
    XglTaskScheduler *scheduler = task->scheduler;
    XglTaskPrefix *free_task;

    if (task == scheduler->next_to_visit)
        scheduler->next_to_visit = task->next;

    if (task->next != 0)
        task->next->previous = task->previous;
    else
        scheduler->active_tail = task->previous;

    if (task->previous != 0)
        task->previous->next = task->next;
    else
        scheduler->active_head = task->next;

    free_task = scheduler->free_tasks;
    task->callback = 0;
    task->next = free_task;
    scheduler->free_tasks = task;
    return 0;
}

/*
 * Only replace the callback.  xglTaskExecute has already saved the successor
 * before invoking a callback, so self-marking normally takes effect when the
 * task is visited on the next execution pass.  A different task that has not
 * yet been visited can be removed later in the same pass.
 */
int xglTaskWaitRemove(XglTaskPrefix *task)
{
    task->callback = xglTaskRemove;
    return 0;
}

/*
 * next_to_visit is the cursor the scheduler advances during callbacks, rather
 * than the task whose callback is currently running.
 */
void xglTaskExecute(XglTaskScheduler *scheduler)
{
    XglTaskPrefix *task;

    if (scheduler != 0) {
        task = scheduler->active_head;
        while (task != 0) {
            scheduler->next_to_visit = task->next;
            if (task->callback != 0)
                task->callback(task);
            task = scheduler->next_to_visit;
        }
    }
}
