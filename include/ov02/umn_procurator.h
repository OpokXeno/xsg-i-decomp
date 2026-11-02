#ifndef INCLUDE_OV02_UMN_PROCURATOR_H
#define INCLUDE_OV02_UMN_PROCURATOR_H

#include "main/menu_model.h"
#include "ov02/tsk_umn_object_task_main.h"

/* Functions and data used by other translation units. */
int UmnManzaiControl(void);
int UmnManzaiInit(int event_no);
void tskUmnManzaiWin(UmnObjectTask *task, void *window);
void UmnProcuratorManzai2(MenuModelUnit *task, void *argument);
void UmnProcuratorManzai(MenuModelUnit *task, void *argument);
void UmnProcuratorMail(MenuModelUnit *task, void *argument);
void UmnProcuratorTop(MenuModelUnit *task, void *argument);
void UmnProcuratorGoodBy(MenuModelUnit *task, void *argument);
void UmnProcuratorSet(void);

#endif /* INCLUDE_OV02_UMN_PROCURATOR_H */
