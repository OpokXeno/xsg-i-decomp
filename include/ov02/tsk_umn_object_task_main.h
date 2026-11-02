#ifndef INCLUDE_OV02_TSK_UMN_OBJECT_TASK_MAIN_H
#define INCLUDE_OV02_TSK_UMN_OBJECT_TASK_MAIN_H

#include "shared.h"

typedef struct UmnObjectTask UmnObjectTask;
typedef void (*UmnObjectTaskWorker)(UmnObjectTask *task, void *data);

/* UmnObjectTask.state: the worker runs once as INIT, then every frame as RUN until END. */
#define UMN_TASK_INIT 0
#define UMN_TASK_RUN 2
#define UMN_TASK_END (-1)

/* Words UmnObjectTaskCreate clears behind the task's worker and data. */
#define UMN_TASK_CLEAR_COUNT 0x19

/* xglTaskEntryNext allocates a 0x80-byte task; these are the fields read here. */
struct UmnObjectTask {
    XglTaskPrefix base;
    int state;
    UmnObjectTaskWorker worker;
    void *data;
    int zeroedOnCreate[UMN_TASK_CLEAR_COUNT];
};

/* Functions and data used by other translation units. */
int tskUmnObjectTaskMain(XglTaskPrefix *base);
void UmnObjectTaskCreate(UmnObjectTaskWorker worker, void *data);
void UmnChangeTopLevel(int level);
void UmnMain(void);
void UmnMain2(int menuId);

#endif /* INCLUDE_OV02_TSK_UMN_OBJECT_TASK_MAIN_H */
