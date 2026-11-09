#ifndef INCLUDE_OV02_TITLE_HDD_INSTALL_H
#define INCLUDE_OV02_TITLE_HDD_INSTALL_H

#include "shared.h"
#include "main/menu_model.h"

/* UmnWork.mode: which screen runs (the loop in tsk_umn_object_task_main.c). */
#define UMN_MODE_TOP_MENU 0
#define UMN_MODE_MAIL 1
#define UMN_MODE_DATA_BASE 2
#define UMN_MODE_SIMULATION 3
#define UMN_MODE_PLUGIN 4
#define UMN_MODE_NONE 0xFF

/*
 * UmnWork.tab_state steps every screen shares: 0 builds the screen's tasks,
 * the opening step arms the wait countdown, the active step takes the pad, and
 * the leave steps wait for the windows to close before UmnChangeTopLevel.
 */
#define UMN_STEP_INIT 0x00
#define UMN_STEP_OPEN 0x10
#define UMN_STEP_ACTIVE 0x11
#define UMN_STEP_LEAVE 0xF0
#define UMN_STEP_LEAVE_WAIT 0xF1

/* Frame countdowns armed in UmnWork.wait when a screen opens and when it is left. */
#define UMN_WAIT_OPEN 12
#define UMN_WAIT_LEAVE 16

/*
 * The 0x80-byte state of the umn menu screens. Every screen (top menu, mail,
 * database, simulation, plugin) shares the mode, tab_state and wait/direction
 * prefix and reads the rest as its own record, so the fields from +0x50 are
 * unions of the per-screen views. UmnChangeTopLevel clears +0x10..+0x7F as
 * seven quadwords (see tsk_umn_object_task_main.c).
 */
typedef struct UmnWorkData {
    unsigned char unmodeled_00;
    unsigned char mode;                 /* +0x01: screen id (1 mail, 2 database, ...), 0xFF none */
    unsigned char menu_selected;        /* +0x02: top-menu cursor kept across screens */
    unsigned char tab_state;            /* +0x03: screen step, or model category */
    unsigned char unmodeled_04[0x0C];
    union {
        unsigned short wait_or_moving;    /* +0x10: wait and moving read as one halfword */
        struct {
            signed char wait;           /* +0x10: frame countdown */
            signed char moving;         /* +0x11: a slide is in progress */
        };
    };
    signed char direction;              /* +0x12: scroll or navigation direction */
    unsigned char unmodeled_13[0x0D];
    unsigned char option_ids[16];       /* +0x20: top-menu option ids, 0 = empty */
    unsigned char menu_count;           /* +0x30 */
    unsigned char menu_cursor;          /* +0x31 */
    unsigned char unmodeled_32[0x0E];
    signed char column;                 /* +0x40 */
    unsigned char unmodeled_41;
    signed char row;                    /* +0x42 */
    signed char rows;                   /* +0x43 */
    signed char scroll;                 /* +0x44 */
    unsigned char unmodeled_45;
    signed char cursor;                 /* +0x46 */
    signed char confirm_cursor;         /* +0x47 */
    unsigned char unmodeled_48[8];
    union {
        struct {
            signed char list_cursor;    /* +0x50 */
            signed char count;          /* +0x51 */
            signed char entry;          /* +0x52: mail id, plugin id or database entry */
            signed char mail_cursor;            /* +0x53 */
            signed char reply_count;            /* +0x54 */
            signed char selected_reply;         /* +0x55: mail id of the chosen reply */
            signed char reply_ids[3];           /* +0x56 */
            signed char header_gate_flag;       /* +0x59 */
            signed char mail_text_ready;        /* +0x5A */
            signed char mail_has_data;          /* +0x5B */
            signed char mail_selection_state;   /* +0x5C */
            unsigned char saved_list_cursor;    /* +0x5D */
            unsigned char saved_scroll;         /* +0x5E */
            unsigned char saved_row;            /* +0x5F */
        };
        struct {
            unsigned char unmodeled_50[0x0D];
            signed char data_base_result;       /* +0x5D */
            signed char analysed;               /* +0x5E */
            signed char model_flag;             /* +0x5F: modelReady or modelResetFlag by caller */
        } database;
        struct {
            int list_index;             /* +0x50 */
            int list_count;             /* +0x54 */
            int pending_script;         /* +0x58 */
        } simulation;
    };
    int price_balance;                  /* +0x60 */
    unsigned char unmodeled_64[0x1C];
} UmnWorkData;

/*
 * The umn work arena: UmnMemory is its base and UmnWorkEnd the allocation
 * cursor, kept as addresses in int variables.
 */
/* Functions and data used by other translation units. */
extern UmnWorkData UmnWork;
extern int UmnWorkEnd;
extern int UmnTexAddr;
extern int UmnBgCubeXtx;
extern int UmnBgCubeLex;
extern MenuModelUnit *UmnModelUkn;
extern MenuModelUnit *UmnModelSon;
extern XglTaskScheduler *task_xmx;
extern XglTaskScheduler *umn_task;
extern int UmnMemory;
extern int UmnWorkEndTop;

void TitleHddInstall(int command);

#endif /* INCLUDE_OV02_TITLE_HDD_INSTALL_H */
