/*
 * TU-local declarations of ov02/tu008 (src/ov02/umn_procurator.c).
 */

#ifndef SRC_OV02_UMN_PROCURATOR_H
#define SRC_OV02_UMN_PROCURATOR_H

typedef struct UmnManzaiWinTask {
    unsigned char unmodeled_00;
    unsigned char task_index;
    unsigned char unmodeled_02[0xA2];
} UmnManzaiWinTask;

typedef void (*UmnTaskFunction)(void *task);

extern unsigned int UmnManzaiFlag;
extern short *UmnManzaiText;
extern void *UmnModelSon;
extern void *UmnModelUkn;
extern unsigned char UmnWork[];
extern UmnManzaiWinTask UmnManzaiWin[2];

int UmnEventTextMake(void);
short *UmnEventTextNextGet(int index);
void UmnObjectTaskCreate(UmnTaskFunction function, void *argument);
void MenuModelCreate(void **model, int model_id);
void MenuModelExtFuncSet(void *model, UmnTaskFunction function, void *argument);
void MenuModelUnitOpen(void *model, int draw_type);

void UmnProcuratorManzai(void *task);
void UmnProcuratorManzai2(void *task);
void UmnProcuratorMail(void *task);
void UmnProcuratorTop(void *task);
void UmnProcuratorGoodBy(void *task);
void tskUmnManzaiWin(void *task);

/*
 * The xglMatrixStack* wrappers Rotation calls come from their definer,
 * main/tu100 xgl_2.c, through include/main/xgl_2.h.
 */

#endif /* SRC_OV02_UMN_PROCURATOR_H */
