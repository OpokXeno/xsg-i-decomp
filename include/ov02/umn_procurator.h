#ifndef INCLUDE_OV02_UMN_PROCURATOR_H
#define INCLUDE_OV02_UMN_PROCURATOR_H

#include "main/e_message.h"
#include "ov02/tsk_umn_object_task_main.h"

/* The original array has two 0xa4-byte window records, each with its message. */
typedef struct UmnManzaiWinTask {
    unsigned char unmodeled_00;
    unsigned char task_index;
    unsigned char unmodeled_02[0x5e];
    EMessageParam message;
} UmnManzaiWinTask;

#endif /* INCLUDE_OV02_UMN_PROCURATOR_H */
