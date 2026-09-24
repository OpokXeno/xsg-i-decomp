/*
 * TU-local declarations of main/tu090 (src/main/xgl_task.c).

 */

#ifndef SRC_MAIN_XGL_TASK_H
#define SRC_MAIN_XGL_TASK_H

#include "shared.h"

XglTaskPrefix *xglTaskEntryPrev(XglTaskScheduler *scheduler,
                                int (*callback)(XglTaskPrefix *task),
                                XglTaskPrefix *entry);

XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                int (*callback)(XglTaskPrefix *task),
                                XglTaskPrefix *entry);

int xglTaskRemove(XglTaskPrefix *task);

extern void *xglTaskInitial(void *manager, int capacity, int flags);

#endif /* SRC_MAIN_XGL_TASK_H */
