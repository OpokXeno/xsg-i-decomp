#ifndef INCLUDE_MAIN_XGL_TASK_H
#define INCLUDE_MAIN_XGL_TASK_H

#include "shared.h"

XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                int (*callback)(XglTaskPrefix *task),
                                XglTaskPrefix *entry);

/*
 * Only replace the callback.  xglTaskExecute has already saved the successor
 * before invoking a callback, so self-marking normally takes effect when the
 * task is visited on the next execution pass.  A different task that has not
 * yet been visited can be removed later in the same pass.
 */
int xglTaskWaitRemove(XglTaskPrefix *task);

/* Returns the address just past the caller-allocated task pool. */
void *xglTaskInitial(void *manager, int capacity, int flags);

/*
 * next_to_visit is the cursor the scheduler advances during callbacks, rather
 * than the task whose callback is currently running.
 */
void xglTaskExecute(XglTaskScheduler *scheduler);

/* Functions and data used by other translation units. */
int xglTaskRemove(XglTaskPrefix *task);

#endif /* INCLUDE_MAIN_XGL_TASK_H */
