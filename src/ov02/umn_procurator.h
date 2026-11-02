/*
 * TU-local declarations of ov02/tu008 (src/ov02/umn_procurator.c).
 */

#ifndef SRC_OV02_UMN_PROCURATOR_H
#define SRC_OV02_UMN_PROCURATOR_H

#include "main/e_message.h"
#include "ov02/tsk_umn_object_task_main.h"
#ifndef OV02_12_PROCURATOR_PRIVATE_H
#define OV02_12_PROCURATOR_PRIVATE_H

#include "shared.h"
#include "main/menu_model.h"

#endif

/* One 12-byte row at +0x0c of a window record; rows 2..5 also hold slide positions. */
typedef struct UmnManzaiWinRow {
    unsigned short x;        /* +0 */
    unsigned short y;        /* +2 */
    int word;                /* +4: 0x00fffff0 */
    unsigned char a;         /* +8 */
    unsigned char b;         /* +9 */
    unsigned char c;         /* +10 */
    unsigned char d;         /* +11 */
} UmnManzaiWinRow;

/* The original array has two 0xa4-byte window records, each with its message. */
typedef struct UmnManzaiWinTask {
    unsigned char phase;             /* +0x00: step of tskUmnManzaiWin's switch */
    unsigned char task_index;        /* +0x01 */
    unsigned char ready;             /* +0x02: set once the window has been opened */
    unsigned char unmodeled_03;
    unsigned short x;                /* +0x04 */
    unsigned short y;                /* +0x06 */
    short *text;                     /* +0x08: UmnManzaiText entry this window shows */
    UmnManzaiWinRow rows[6];         /* +0x0c */
    unsigned char unmodeled_54[4];
    int unmodeled_58;
    unsigned char unmodeled_5c[4];
    EMessageParam message;           /* +0x60 */
} UmnManzaiWinTask;

/* Text record UmnManzaiText points at: the speaker id, then the message text. */
typedef struct UmnManzaiTextEntry {
    short speaker;
    unsigned char unmodeled_02[2];
    char *text;                      /* +0x04 */
} UmnManzaiTextEntry;

/* The object task tskUmnObjectTaskMain runs; its worker state is at +0x10. */
typedef struct UmnManzaiTask {
    unsigned char unmodeled_00[0x10];
    int state;
} UmnManzaiTask;

/*
 * The subwindow record MenuModelUnit.subWindow points at. UmnProcuratorManzai2
 * slides the +4/+6 pair toward the +8/+0xA pair (read unsigned) and
 * UmnProcuratorManzai slides the +8/+0xA pair toward the +4/+6 pair, so each
 * function reads the record through its own view of the same layout.
 */
typedef struct UmnSubWindow {
    unsigned char unmodeled_00[4];
    short x;                /* +0x4 */
    short y;                /* +0x6 */
    unsigned short width;   /* +0x8 */
    unsigned short height;  /* +0xA */
    StudioCamera *camera;   /* +0xC */
} UmnSubWindow;

typedef struct UmnSubWindowView {
    unsigned char unmodeled_00[4];
    unsigned short x;       /* +0x4 */
    unsigned short y;       /* +0x6 */
    short width;            /* +0x8 */
    short height;           /* +0xA */
    StudioCamera *camera;   /* +0xC */
} UmnSubWindowView;

typedef void (*UmnTaskFunction)(void *task);

extern unsigned int UmnManzaiFlag;
extern short *UmnManzaiText;
extern void *UmnModelSon;
extern void *UmnModelUkn;
extern UmnWorkData UmnWork;
extern UmnManzaiWinTask UmnManzaiWin[2];

int UmnEventTextMake(void);
short *UmnEventTextNextGet(int index);
void UmnObjectTaskCreate(UmnTaskFunction function, void *argument);
void MenuModelCreate(void **model, int model_id);
void MenuModelExtFuncSet(void *model, UmnTaskFunction function, void *argument);
void MenuModelUnitOpen(void *model, int draw_type);
void MenuModelSubWindowSet(MenuModelUnit *unit, int studio_id, int mode);
void MenuModelSubWindowBreak(MenuModelUnit *unit);
void MenuModelMotionSet(MenuModelUnit *target, unsigned int data_id);
void MoveSlide(short *value, short *target, float speed);

void UmnProcuratorManzai(void *task);
void UmnProcuratorManzai2(void *task);
void UmnProcuratorMail(void *task);
void UmnProcuratorTop(void *task);
void UmnProcuratorGoodBy(void *task);
void tskUmnManzaiWin(UmnManzaiTask *task, UmnManzaiWinTask *window);

/*
 * The xglMatrixStack* wrappers Rotation calls come from their definer,
 * main/tu100 xgl_2.c, through include/main/xgl_2.h.
 */

#endif /* SRC_OV02_UMN_PROCURATOR_H */
