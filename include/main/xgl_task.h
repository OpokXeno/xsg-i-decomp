#ifndef INCLUDE_MAIN_XGL_TASK_H
#define INCLUDE_MAIN_XGL_TASK_H

#include "shared.h"

/*
 * Only replace the callback.  xglTaskExecute has already saved the successor
 * before invoking a callback, so self-marking normally takes effect when the
 * task is visited on the next execution pass.  A different task that has not
 * yet been visited can be removed later in the same pass.
 */
int xglTaskWaitRemove(XglTaskPrefix *task);

/*
 * next_to_visit is the cursor the scheduler advances during callbacks, rather
 * than the task whose callback is currently running.
 */
void xglTaskExecute(XglTaskScheduler *scheduler);

#endif /* INCLUDE_MAIN_XGL_TASK_H */
