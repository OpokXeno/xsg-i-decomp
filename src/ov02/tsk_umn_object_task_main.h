#ifndef SRC_OV02_TSK_UMN_OBJECT_TASK_MAIN_H
#define SRC_OV02_TSK_UMN_OBJECT_TASK_MAIN_H

#include "shared.h"

typedef struct UmnObjectTask UmnObjectTask;
typedef void (*UmnObjectTaskWorker)(UmnObjectTask *task, void *data);

/* xglTaskEntryNext allocates a 0x80-byte task; these are the fields read here. */
struct UmnObjectTask {
    XglTaskPrefix base;
    int state;
    UmnObjectTaskWorker worker;
    void *data;
    int zeroedOnCreate[0x19];
};

typedef struct UmnWorkData {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02[0x7e];
} UmnWorkData;

extern UmnWorkData UmnWork;

#endif /* SRC_OV02_TSK_UMN_OBJECT_TASK_MAIN_H */
