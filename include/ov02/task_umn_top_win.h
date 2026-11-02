#ifndef INCLUDE_OV02_TASK_UMN_TOP_WIN_H
#define INCLUDE_OV02_TASK_UMN_TOP_WIN_H

#include "main/e_message.h"
#include "ov02/umn_event_text_symbol_check.h"

/* Windows of the manzai scene (one per speaker) and rows (frame and text panels) in each. */
#define UMN_MANZAI_WINDOW_COUNT 2
#define UMN_MANZAI_ROW_COUNT 6

/* One 12-byte row at +0x0c of a window record; rows 2..5 also hold slide positions. */
typedef struct UmnManzaiWinRow {
    short x;                 /* +0 */
    short y;                 /* +2 */
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
    UmnEventTextRecord *text;        /* +0x08: the speech record this window shows */
    UmnManzaiWinRow rows[UMN_MANZAI_ROW_COUNT];        /* +0x0c */
    unsigned char unmodeled_54[4];
    int unmodeled_58;
    unsigned char unmodeled_5c[4];
    EMessageParam message;           /* +0x60 */
} UmnManzaiWinTask;

/*
 * UmnManzaiFlag bits shared by UmnManzaiControl, the two model workers
 * (UmnProcuratorManzai drives UmnModelUkn, UmnProcuratorManzai2 UmnModelSon)
 * and the window workers.
 */
#define UMN_MANZAI_CONFIRM 0x001       /* the confirm button was pressed this frame */
#define UMN_MANZAI_UKN_MOVING 0x002    /* the first model's sub window is sliding */
#define UMN_MANZAI_SON_MOVING 0x004    /* the second model's sub window is sliding */
#define UMN_MANZAI_NEW_TEXT 0x008      /* UmnManzaiText was advanced to the next record */
#define UMN_MANZAI_UKN_SHOWN 0x010     /* the first model's sub window is in place */
#define UMN_MANZAI_SON_SHOWN 0x020     /* the second model's sub window is in place */
#define UMN_MANZAI_BOTH_SHOWN (UMN_MANZAI_UKN_SHOWN | UMN_MANZAI_SON_SHOWN)
#define UMN_MANZAI_FINISHED 0x040      /* the last record was read: everything slides out */
#define UMN_MANZAI_SON_VISIBLE 0x100   /* the second model has faded in */
#define UMN_MANZAI_TEXT_TAKEN 0x200    /* a window took the new record this frame */
#define UMN_MANZAI_SLIDING (UMN_MANZAI_UKN_MOVING | UMN_MANZAI_SON_MOVING | UMN_MANZAI_TEXT_TAKEN)

/* Functions and data used by other translation units. */
extern unsigned int UmnManzaiFlag;
extern UmnEventTextRecord *UmnManzaiText;
extern UmnManzaiWinTask UmnManzaiWin[UMN_MANZAI_WINDOW_COUNT];
void UmnTopMenu(void);

#endif /* INCLUDE_OV02_TASK_UMN_TOP_WIN_H */
