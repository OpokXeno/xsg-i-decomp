/*
 * OV02 original TU 10: 0x00a097d8..0x00a0baf8 (10 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/e_battle_win_open.h"
#include "main/e_message.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/menu_model.h"
#include "main/menu_top_command_check.h"
#include "main/xgl_dma.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"
#include "ov02/ov02.h"
#include "ov02/title_hdd_install.h"
#include "ov02/tsk_umn_object_task_main.h"
#include "ov02/tya_uml.h"
#include "ov02/umn_guno_data_base_get.h"
#define PAD_MOVE   0x002C0000
#define UMN_KEYWORD_HIDDEN_X (-272)

/* Database records: ids UMN_DB_FIRST_ID.. in a table of UmnGunoDataBase entries based at UmnGunoDataBaseTop. */
#define UMN_DB_FIRST_ID 0x22
#define UMN_DB_ENTRY_COUNT 0x1D
/* Menu entries of the database top menu (list, keyword search, exit); the entry array adds a terminator. */
#define UMN_DB_MENU_ENTRIES 3
/* Rows of the name list: all of them exist, at most UMN_DB_NAME_SHOWN are filled; UmnWork.rows is capped at UMN_DB_LIST_ROWS_MAX. */
#define UMN_DB_NAME_ROWS 14
#define UMN_DB_NAME_SHOWN 12
#define UMN_DB_LIST_ROWS_MAX 5
/* Help lines and arrow sprites of the extra window. */
#define UMN_DB_EXWIN_LINES 5
#define UMN_DB_EXWIN_ARROWS 2

/* UmnWork.tab_state steps of the database terminal beyond the common UMN_STEP_*. */
#define UMN_DB_STEP_LIST_OPEN 0x20
#define UMN_DB_STEP_LIST 0x21
#define UMN_DB_STEP_DETAIL_OPEN 0x30
#define UMN_DB_STEP_DETAIL 0x31
#define UMN_DB_STEP_KEYWORD_OPEN 0x50
#define UMN_DB_STEP_KEYWORD 0x51

/* UmnWork.cursor on the top menu. */
enum {
    UMN_DB_MENU_LIST,
    UMN_DB_MENU_KEYWORD,
    UMN_DB_MENU_EXIT
};

/* tskUmnDataBaseName steps (UmnDataBaseNameWork.step). */
enum {
    UMN_DB_NAME_WAIT = 0,
    UMN_DB_NAME_PLACE = 10,
    UMN_DB_NAME_SLIDE_LIST = 11,
    UMN_DB_NAME_LIST_SHOWN = 12,
    UMN_DB_NAME_SLIDE_DETAIL = 20,
    UMN_DB_NAME_DETAIL_SHOWN = 22,
    UMN_DB_NAME_SLIDE_OUT = 30
};

/* tskUmnDataBaseAnalisis steps (UmnDataBaseAnalisisWork.step). */
enum {
    UMN_DB_ANALYSIS_IDLE = 0,
    UMN_DB_ANALYSIS_WAIT = 1,
    UMN_DB_ANALYSIS_OPEN = 0xA,
    UMN_DB_ANALYSIS_SHOWN = 0xC,
    UMN_DB_ANALYSIS_CLOSE = 0x14
};

/* tskUmnDataBaseMenu states (UmnDataBaseMenuWork.state). */
enum {
    UMN_DB_MENU_IDLE = 0,
    UMN_DB_MENU_WAIT_OPEN = 10,
    UMN_DB_MENU_BUILD = 20,
    UMN_DB_MENU_SLIDE_IN = 21,
    UMN_DB_MENU_SHOWN = 30,
    UMN_DB_MENU_SLIDE_OUT = 40
};

/* tskUmnDataBaseKeyWord steps (UmnDataBaseKeyWordWork.step). */
enum {
    UMN_DB_KEYWORD_IDLE = 0,
    UMN_DB_KEYWORD_WAIT = 1,
    UMN_DB_KEYWORD_OPEN = 10,
    UMN_DB_KEYWORD_SHOWN = 11
};

/* MenuModelUnit.extFuncFlag of the record model (UmnDataBaseModel). */
enum {
    UMN_DB_MODEL_INIT = 0,
    UMN_DB_MODEL_OPEN = 0xA,
    UMN_DB_MODEL_SHOWN = 0x14,
    UMN_DB_MODEL_CLOSE = 0x1E,
    UMN_DB_MODEL_CLOSING = 0x1F,
    UMN_DB_MODEL_DONE = 0x63
};

typedef struct UmnDataBaseMenuEntry {
    const char *text;                       /* +0x00 */
    unsigned char unmodeled_04[0x08 - 0x04];
    unsigned char enabled;              /* +0x08 */
    unsigned char unmodeled_09[0x0C - 0x09];
} UmnDataBaseMenuEntry;
typedef struct UmnDataBaseInfoText {
    short x;                            /* +0x00 */
    short y;                            /* +0x02 */
    int value;                          /* +0x04 */
    unsigned char unmodeled_08[0x0C - 0x08];
    const char *text;                       /* +0x0C */
    UmnDataBaseMenuEntry *entries;      /* +0x10 */
    unsigned char unmodeled_14[0x1B0 - 0x14];
} UmnDataBaseInfoText;
typedef struct UmnDataBaseInfoWork {
    unsigned char state;                /* +0x00 */
    unsigned char opened;               /* +0x01 */
    unsigned char unmodeled_02[0x08 - 0x02];
    int color;                          /* +0x08 */
    WindowDX window;       /* +0x0C */
    UmnDataBaseInfoText info;           /* +0x1A0 */
} UmnDataBaseInfoWork;
typedef struct UmnDataBasePasBox {
    short x;                            /* +0x00 */
    short y;                            /* +0x02 */
    int kind;                           /* +0x04 */
    short width;                        /* +0x08 */
    short height;                       /* +0x0A */
} UmnDataBasePasBox;
typedef struct UmnDataBasePasWork {
    unsigned char unmodeled_00;
    unsigned char state;                /* +0x01 */
    unsigned char unmodeled_02[0x04 - 0x02];
    int kind;                           /* +0x04 */
    WindowDX window;       /* +0x08 */
    EMessageParam message;     /* +0x19C */
    unsigned char unmodeled_1e0[0x3BC - 0x1E0];
    UmnDataBasePasBox box;              /* +0x3BC */
} UmnDataBasePasWork;
typedef struct UmnDataBasePasMessageView {
    unsigned char unmodeled_00[0x0C];
    EMessageParam message;     /* +0x0C from the view base (work +0x190) */
} UmnDataBasePasMessageView;
typedef struct UmnDataBaseNameRow {
    unsigned short id;                  /* +0x00 (row +0x08 from the work base) */
    unsigned char unmodeled_02[0x04 - 0x02];
    WindowDX window;       /* +0x04 */
    EMessageParam message;              /* +0x198 */
    ENumberMainData number;            /* +0x1DC */
} UmnDataBaseNameRow;
typedef struct UmnDataBaseNameWork {
    unsigned char step;                 /* +0x00: screen step */
    unsigned char opened;               /* +0x01 */
    unsigned char unmodeled_02;         /* +0x02 */
    unsigned char count;                /* +0x03 */
    unsigned int color;                 /* +0x04 */
    UmnDataBaseNameRow rows[UMN_DB_NAME_ROWS]; /* +0x08, 620 bytes each */
    ECursolMainData cursor;             /* +0x21F0 */
} UmnDataBaseNameWork;
/* Partial view of PadPrefix: tskUmnDataBaseAnalisis also reads a doubleword at
 * PadData + 0x28, which PadPrefix does not describe. */
typedef union UmnPadInputPrefix {
    PadPrefix canonical;
    struct {
        unsigned char unmodeled_00[0x28];
        union {
            struct {
                unsigned short half_28;
                unsigned short half_2a;
                unsigned int unmodeled_2c;
            } halves;
            unsigned long buttonRegister;
        } buttons;
    } wide;
} UmnPadInputPrefix;
struct UmnGunoDataBase {
    unsigned char unmodeled_00[0x20];
    short x;                                /* +0x20 */
    short y;                                /* +0x22 */
    short page;                             /* +0x24 */
    unsigned char unmodeled_26[0x28 - 0x26];
    int title;                              /* +0x28 */
    int text;                                /* +0x2C */
    int picture;                            /* +0x30 */
    int sound;                              /* +0x34 */
    long model;                             /* +0x38 */
    char name[0x52 - 0x40];                 /* +0x40 */
    char reading[0x68 - 0x52];              /* +0x52 */
};
typedef struct UmnDataBaseWinParam {
    int width;                              /* +0x00 */
    int height;                             /* +0x04 */
    int color;                              /* +0x08 */
    UmnGunoDataBase *entry;                 /* +0x0C */
    short x;                                /* +0x10 */
    short y;                                /* +0x12 */
    short page;                             /* +0x14 */
    unsigned char unmodeled_16[0x18 - 0x16];
    int title;                              /* +0x18 */
    int text;                               /* +0x1C */
    int picture;                            /* +0x20 */
    int sound;                              /* +0x24 */
    long model;                             /* +0x28 */
    unsigned char unmodeled_30[0x34 - 0x30];
    char *name;                             /* +0x34 */
    char *reading;                          /* +0x38 */
} UmnDataBaseWinParam;
typedef struct UmnDataBaseAnalisisWork {
    unsigned char step;                     /* +0x00 */
    unsigned char opened;                   /* +0x01 */
} UmnDataBaseAnalisisWork;
typedef struct UmnDataBaseMenuWork {
    unsigned char state;                /* +0x00 */
    unsigned char opened;               /* +0x01 */
    unsigned char unmodeled_02[0x04 - 0x02];
    short x;                            /* +0x04 */
    short y;                            /* +0x06 */
    int color;                          /* +0x08 */
    int pending;                        /* +0x0C */
    WindowDX window;       /* +0x10 */
    UmnDataBaseInfoText info;           /* +0x1A4 */
    unsigned char unmodeled_354[0x79C - 0x354];
    UmnDataBaseMenuEntry entries[UMN_DB_MENU_ENTRIES + 1]; /* +0x79C */
} UmnDataBaseMenuWork;
typedef struct UmnDataBaseExWinLine {
    short x;                                /* +0x00 */
    short y;                                /* +0x02 */
    EMessageParam message;                  /* +0x04 */
} UmnDataBaseExWinLine;
typedef struct UmnDataBaseExWinSprite {
    unsigned char unmodeled_00[0x04];
    short x;                                /* +0x04 */
    short y;                                /* +0x06 */
    int color;                              /* +0x08 */
    unsigned char unmodeled_0c[0x28 - 0x0C];
} UmnDataBaseExWinSprite;
typedef struct UmnDataBaseExWinWork {
    unsigned char step;                     /* +0x000 */
    unsigned char opened;                   /* +0x001 */
    unsigned char unmodeled_002;            /* +0x002 */
    unsigned char textShown;                /* +0x003 */
    int color;                              /* +0x004 */
    UmnDataBaseExWinLine lines[UMN_DB_EXWIN_LINES]; /* +0x008 */
    int drift[UMN_DB_EXWIN_ARROWS];         /* +0x170 */
    UmnDataBaseExWinSprite sprites[UMN_DB_EXWIN_ARROWS]; /* +0x178 */
} UmnDataBaseExWinWork;
typedef struct UmnDataBaseArrowSprites {
    short id[UMN_DB_EXWIN_ARROWS];
} UmnDataBaseArrowSprites;
typedef struct UmnDataBaseArrowSteps {
    int step[UMN_DB_EXWIN_ARROWS];
} UmnDataBaseArrowSteps;
typedef struct UmnDataBaseKeyWordWork {
    unsigned char step;                     /* +0x000 */
    unsigned char active;                   /* +0x001 */
    unsigned char visible;                  /* +0x002 */
    unsigned char messageShown;             /* +0x003 */
    unsigned int color;                     /* +0x004 */
    WindowDX window;                        /* +0x008 */
    MenuTopMenuPanel list;                  /* +0x19C */
    ECursolMainData selectCursor;           /* +0x794 */
    ECursolMainData indexCursor;            /* +0x7B8 */
    unsigned char unmodeled_7dc[4];
    TyaUmlDatabase disp;              /* +0x7E0 */
    short x;                                /* +0x840 */
    short y;                                /* +0x842 */
    int indexNo;                            /* +0x844 */
    int selectNo;                           /* +0x848 */
    ETagFontMainData tag;                  /* +0x84C */
    ERibbonMainData ribbon;                /* +0x86C */
    ENumberMainData number;                /* +0x8DC */
    EMessageParam message;                  /* +0x96C */
} UmnDataBaseKeyWordWork;
typedef struct UmnDataBaseModelXform {
    float position[3];              /* +0x00 */
    unsigned char unmodeled_0c[0x20 - 0x0C];
    float scale[3];                  /* +0x20 */
    unsigned char unmodeled_2c[0x40 - 0x2C];
    float baseRotation[3];           /* +0x40 */
    unsigned char unmodeled_4c[0x60 - 0x4C];
    float visible;                    /* +0x60 */
} UmnDataBaseModelXform;
typedef struct UmnDataBaseModelData {
    unsigned char unmodeled_00[0xC0];
    UmnDataBaseModelXform xform;     /* +0xC0 */
} UmnDataBaseModelData;

extern void *UmnGunoDataBaseTop;
/* The help line text of the database screen (msg00.0). */
static const char *msg00_0_00A106F8;
extern const char dataBaseRowTagText[];
extern unsigned short monster_folder[];
extern int BW3BattleOrDataBase;
extern const char dataBaseMenuTagText[];
static const char *text2_2[3];
extern void MenuModelControl(MenuModelUnit *unit);
extern void MenuModelUnitBreak(MenuModelUnit *unit);
extern unsigned short monster_folder[0x80];
extern const char dataBaseEntryDebugFormat[];
extern const char dataBaseListCursorDebugFormat[];
extern const char dataBaseColumnRowDebugFormat[];
extern const char dataBaseMovingDebugFormat[];
extern const char dataBaseGnosisHelpText[];
extern const char dataBaseKeywordHelpText[];
extern const char dataBaseCancelHelpText[];
extern const char dataBaseZoomOutHintText[];
extern const char dataBaseZoomInHintText[];
extern const char dataBaseRotateHintText[];
extern const char dataBaseTurnMenuOffHintText[];
extern const char dataBaseAnalysisInfoHintText[];

void xglFontDebugPrintf(int x, int y, const char *format, ...);
void eBattleWinOpen3(UmnDataBaseWinParam *param);
void eBattleWinMain3(void);
void eBattleWinClose3(void);
void xglSoundEffectNormalID(int sound_id, int variant);
void eSpriteSet(UmnDataBaseExWinSprite *sprite, short spriteId);
void eSpriteMain(UmnDataBaseExWinSprite *sprite);
int UmnDataBaseMonsterCheck(int id);
int UmnDataBaseAnalisisCheck(int id);
signed char MenuSelectMove(int cursor, int entries, int mode);
void xglFontDebugHex(int x, int y, int value, int digits);

/*
 * Returns the database record for id, or the table base when id is outside
 * the record range.
 */
UmnGunoDataBase *UmnGunoDataBaseGet(int id)
{
    UmnGunoDataBase *base = UmnGunoDataBaseTop;

    if ((u32) (id - UMN_DB_FIRST_ID) < UMN_DB_ENTRY_COUNT) {
        return base + id - UMN_DB_FIRST_ID;
    }
    return base;
}

static const char *msg00_1_00A10700[3];

/*
 * The title window of the database screen with its message. The window and
 * the message slide in or out according to the terminal's current step.
 */
void tskUmnDataBasePas(UmnObjectTask *task, void *data)
{
    UmnDataBasePasWork *work = data;
    short slideTargets[4];
    short windowTarget;
    int i;

    if (UmnWork.mode != UMN_MODE_DATA_BASE) {
        task->state = UMN_TASK_END;
        return;
    }
    switch (task->state) {
    case UMN_TASK_INIT: {
        short messageX = 288;
        short messageY = 11;
        int messageMode = E_MODE_SHOW_START;
        UmnDataBasePasMessageView *view;

        work->kind = 0x00FFFFF0;
        WindowDXSet(&work->window);
        work->window.x = -272;
        work->window.y = 8;
        work->window.style = work->kind;
        work->window.width = 272;
        work->window.state = WINDOWDX_STATE_OPEN;
        work->window.height = 32;
        WindowDXMain(&work->window);
        work->window.state = WINDOWDX_STATE_READY;
        eMessageSet(&work->message, msg00_0_00A106F8);
        view = (UmnDataBasePasMessageView *) ((unsigned char *) work + 0x190);
        view->message.mode = messageMode;
        view->message.x = messageX;
        view->message.y = messageY;
        work->message.draw_parameter = work->kind + 2;
        i = 1;
        work->state = i;
        break;
    }
    case UMN_TASK_RUN: {
        short slideValue = 288;

        windowTarget = -16;
        for (i = 3; i >= 0; i--) {
            slideTargets[i] = slideValue;
        }
        /* The open, list, detail and keyword steps bring the message on screen; the others move it off. */
        switch (UmnWork.tab_state) {
        case UMN_STEP_OPEN: case UMN_STEP_ACTIVE: case UMN_DB_STEP_LIST_OPEN: case UMN_DB_STEP_LIST:
        case UMN_DB_STEP_DETAIL_OPEN: case UMN_DB_STEP_DETAIL: case UMN_DB_STEP_KEYWORD_OPEN: case UMN_DB_STEP_KEYWORD:
            slideTargets[0] = 16;
            break;
        case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25:
        case 26: case 27: case 28: case 29: case 30: case 31: case 34: case 35:
        case 36: case 37: case 38: case 39: case 40: case 41: case 42: case 43:
        case 44: case 45: case 46: case 47: case 50: case 51: case 52: case 53:
        case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 61:
        case 62: case 63: case 64: case 65: case 66: case 67: case 68: case 69:
        case 70: case 71: case 72: case 73: case 74: case 75: case 76: case 77:
        case 78: case 79:
        default:
            windowTarget = -272;
            break;
        }
        if (work->state != 0) {
            MoveSlide(&work->window.x, &windowTarget, 3.0f);
            WindowDXMain(&work->window);
            work->box.x = work->window.x + 3;
            work->box.y = work->window.y + 3;
            work->box.kind = work->kind;
            work->box.width = work->window.width - 6;
            work->box.height = work->window.height - 6;
            endPrintExtFunc(work->kind, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET, &work->box);
            MoveSlide(&work->message.x, &slideTargets[0], 3.0f);
            if (work->message.x < 256) {
                eMessageMain(&work->message);
            }
            endPrintExtFunc(work->kind, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET, 0);
        }
        break;
    }
    }
}

/*
 * The help line at the bottom of the U.M.N. database screen: the frame the
 * task starts on builds the window, and from then on every frame points the
 * line at the help text of the entry the cursor is on and slides the window
 * to the height the terminal's current step asks for.
 */
void tskUmnDataBaseInfo(UmnObjectTask *task, void *data)
{
    UmnDataBaseInfoWork *work = data;
    int state;
    short slideTarget;

    if (UmnWork.mode != UMN_MODE_DATA_BASE) {
        task->state = UMN_TASK_END;
        return;
    }

    switch (task->state) {
    case UMN_TASK_INIT:
        work->color = 0x00FF0000;
        work->opened = 0;
        work->state = 0;
        WindowDXSet(&work->window);
        work->window.x = -16;
        work->window.state = 0;
        work->window.y = 480;
        work->window.width = 544;
        work->window.height = 54;
        work->window.close_callback = MenuInfoWindow;
        work->window.close_callback_arg = &work->info;
        work->info.x = 0;
        work->window.style = work->color;
        work->window.state = WINDOWDX_STATE_OPEN;
        work->info.y = 0;
        work->info.text = 0;
        WindowDXMain(&work->window);
        work->window.state = WINDOWDX_STATE_READY;
        break;
    case UMN_TASK_RUN:
        state = UmnWork.tab_state;
        slideTarget = 386;
        /* Only UMN_STEP_OPEN and UMN_STEP_ACTIVE raise the line to 386; other steps lower it. */
        if (state >= UMN_STEP_ACTIVE + 1) {
            slideTarget = 480;
        } else if (state < UMN_STEP_OPEN) {
            slideTarget = 480;
        }
        work->info.text = msg00_1_00A10700[UmnWork.cursor];
        MoveSlide(&work->window.y, &slideTarget, 3.0f);
        WindowDXMain(&work->window);
        break;
    }
}

/*
 * The name list of the U.M.N. database: lays out the rows and slides them into
 * the list, to the detail view of the selected row, or back out. While the
 * list is open it also draws each visible row with its text and its list
 * number, and the cursor.
 */
void tskUmnDataBaseName(UmnObjectTask *task, void *data)
{
    UmnDataBaseNameWork *work = data;
    short xTarget;
    short yTarget;
    int selected;
    int done;
    int matched;
    int k;
    int i;
    int r;

    if (UmnWork.mode != UMN_MODE_DATA_BASE) {
        task->state = UMN_TASK_END;
        return;
    }

    switch (task->state) {
    case UMN_TASK_INIT: {

        work->color = 0xFFFF00;
        for (i = 0; i < UMN_DB_NAME_ROWS; i++) {
            int width;

            WindowDXSet(&work->rows[i].window);
            width = 222;
            work->rows[i].window.width = width;
            work->rows[i].window.style = work->color;
            work->rows[i].window.y = 80 + (i / 2) * 48;
            work->rows[i].window.tag_id = dataBaseRowTagText;
            work->rows[i].window.height = 30;
            work->rows[i].window.x = (i % 2) * (width + 544) - width - 16;
            work->rows[i].window.state = WINDOWDX_STATE_OPEN;
            WindowDXMain(&work->rows[i].window);
            work->rows[i].window.state = WINDOWDX_STATE_READY;
            eNumberSet(&work->rows[i].number, 0);
            work->rows[i].number.digits = 3;
            work->rows[i].number.format = 1;
        }
        eCursolSet(&work->cursor, 0);
        work->count = UmnWork.count;
        if (work->count >= UMN_DB_NAME_SHOWN + 1) {
            work->count = UMN_DB_NAME_SHOWN;
        }
        work->step = UMN_DB_NAME_WAIT;
        work->opened = 0;
        break;
    }
    case UMN_TASK_RUN:
        {
        /* Slides the rows into the list, then to the detail view or back out. */
        switch (work->step) {
        case UMN_DB_NAME_WAIT:
        case 1:
            if (UmnWork.tab_state == UMN_DB_STEP_LIST) {
                work->step = UMN_DB_NAME_PLACE;
                work->opened = 1;
                eCursolModeChange(&work->cursor, E_MODE_SHOW_START);
            }
            break;
        case UMN_DB_NAME_PLACE: {
            int selected;

            selected = UmnWork.column + 2 * UmnWork.row;
            for (i = 0; i < work->count; i++) {
                if (selected != i) {
                    work->rows[i].window.y = 80 + (i / 2) * 48;
                    work->rows[i].window.x = (work->rows[i].window.width + 544) * (i % 2) - work->rows[i].window.width - 16;
                }
            }
            work->step = UMN_DB_NAME_SLIDE_LIST;
        }
            /* fallthrough */
        case UMN_DB_NAME_SLIDE_LIST: {
            int selected;
            int done;

            selected = UmnWork.column + 2 * UmnWork.row;
            done = 0;
            for (i = 0; i < work->count; i++) {
                if (selected == i) {
                    yTarget = 80 + (i / 2) * 48;
                    MoveSlide(&work->rows[i].window.y, &yTarget, 3.0f);
                    if (work->rows[i].window.y != yTarget) {
                        continue;
                    }
                    selected = -1;
                }
                xTarget = (work->rows[0].window.width + 16) * (i % 2) + 32;
                MoveSlide(&work->rows[i].window.x, &xTarget, 3.0f);
                if (work->rows[i].window.x == xTarget) {
                    done++;
                }
            }
            if (done == work->count) {
                work->step = UMN_DB_NAME_LIST_SHOWN;
                UmnWork.moving = 0;
            }
            break;
        }
        case UMN_DB_NAME_LIST_SHOWN:
            if (UmnWork.tab_state != UMN_STEP_ACTIVE) {
                if (UmnWork.tab_state == UMN_DB_STEP_DETAIL) {
                    work->step = UMN_DB_NAME_SLIDE_DETAIL;
                }
            } else {
                work->step = UMN_DB_NAME_SLIDE_OUT;
            }
            break;
        case UMN_DB_NAME_SLIDE_DETAIL:
        case UMN_DB_NAME_SLIDE_OUT: {
            int selected;
            int matched;
            short xTarget;
            short yTarget;

            selected = UmnWork.column + 2 * UmnWork.row;
            matched = 0;
            for (i = 0; i < work->count; i++) {
                xTarget = (work->rows[i].window.width + 544) * (i % 2) - work->rows[i].window.width - 16;
                if (work->step == UMN_DB_NAME_SLIDE_DETAIL && selected == i) {
                    xTarget = 272;
                }
                MoveSlide(&work->rows[i].window.x, &xTarget, 3.0f);
                if (work->rows[i].window.x == xTarget) {
                    if (work->step == UMN_DB_NAME_SLIDE_DETAIL && selected == i) {
                        yTarget = 32;
                        MoveSlide(&work->rows[i].window.y, &yTarget, 3.0f);
                        if (work->rows[i].window.y == yTarget) {
                            matched++;
                        }
                    } else {
                        matched++;
                    }
                }
            }
            if (matched == work->count) {
                if (work->step == UMN_DB_NAME_SLIDE_DETAIL) {
                    work->step = UMN_DB_NAME_DETAIL_SHOWN;
                } else {
                    work->step = UMN_DB_NAME_WAIT;
                }
                UmnWork.moving = 0;
            }
            break;
        }
        case UMN_DB_NAME_DETAIL_SHOWN: {
            int selected;

            if (UmnWork.tab_state == UMN_DB_STEP_LIST) {
                work->step = UMN_DB_NAME_PLACE;
            } else {
                selected = UmnWork.column + 2 * UmnWork.row;
                for (i = 0; i < work->count; i++) {
                    if (selected == i) {
                        work->rows[i].window.x = 272;
                    } else {
                        work->rows[i].window.x = 528;
                    }
                    work->rows[i].window.y = 32;
                }
            }
            break;
        }
        }
        }
        break;
    }

    /* Orders the rows with the selected one last, then draws them and the cursor. */
    if (work->opened != 0) {
        int r;
        int *fill;
        int order[12];

        selected = UmnWork.column + 2 * UmnWork.row;
        for (i = 0; i < work->count; i++) {
            r = UmnWork.list_cursor - 2 * UmnWork.row - UmnWork.column + i;
            work->rows[i].id = monster_folder[r];
        }
        fill = order;
        for (i = 0; i < work->count; i++) {
            if (selected != i) {
                *fill++ = i;
            }
        }
        *fill = selected;
        for (i = 0; i < work->count; i++) {
            r = order[i];
            if (work->rows[r].id != 0) {
                work->rows[r].window.style = work->color;
                if (selected == r) {
                    work->rows[r].window.style += 16;
                }
                WindowDXMain(&work->rows[r].window);
                eMessageSet(&work->rows[r].message, (const char *) UmnGunoDataBaseGet(work->rows[r].id));
                work->rows[r].message.mode = E_MODE_SHOW_START;
                work->rows[r].message.draw_parameter = work->rows[r].window.style + 2;
                work->rows[r].message.x = work->rows[r].window.x + 19;
                work->rows[r].message.y = work->rows[r].window.y + 3;
                eMessageMain(&work->rows[r].message);
                work->rows[r].number.x = work->rows[r].window.x + 32;
                work->rows[r].number.work = work->rows[r].window.style + 2;
                work->rows[r].number.y = work->rows[r].window.y - 12;
                work->rows[r].number.value = UmnWork.list_cursor - 2 * UmnWork.row - UmnWork.column + order[i] + 1;
                eNumberMain(&work->rows[r].number);
            }
        }
        work->cursor.x = work->rows[selected].window.x + 3;
        work->cursor.work = work->rows[selected].window.style + 2;
        work->cursor.y = work->rows[selected].window.y + 7;
        eCursolMain(&work->cursor);
    }

    xglFontDebugPrintf(16, 96, "name_w %2d", work->step);
}

/*
 * The analysis window of the U.M.N. database: the player opens it with the
 * circle button on an analysed record and closes it again, and the window is
 * driven every frame for as long as it is open.
 */
void tskUmnDataBaseAnalisis(UmnObjectTask *task, void *data)
{
    UmnDataBaseAnalisisWork *work = (UmnDataBaseAnalisisWork *)task->zeroedOnCreate;
    UmnDataBaseWinParam param;
    UmnGunoDataBase *entry;

    if (UmnWork.mode != UMN_MODE_DATA_BASE) {
        task->state = UMN_TASK_END;
        return;
    }

    switch (work->step) {
    case UMN_DB_ANALYSIS_IDLE:
        work->opened = 0;
        work->step = UMN_DB_ANALYSIS_WAIT;
        /* fallthrough */
    case UMN_DB_ANALYSIS_WAIT:
        if (PadData.half_2a & PAD_CIRCLE) {
            if (UmnWork.tab_state == UMN_DB_STEP_DETAIL) {
                if (UmnWork.database.analysed != 0) {
                    work->step = UMN_DB_ANALYSIS_OPEN;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
        }
        break;
    case UMN_DB_ANALYSIS_OPEN:
        entry = UmnGunoDataBaseGet(UmnWork.entry);
        param.width = 0x70;
        param.height = 0x90;
        param.color = 0xFFFFF0;
        param.entry = entry;
        param.x = entry->x;
        param.y = entry->y;
        param.page = entry->page;
        param.title = entry->title;
        param.text = entry->text;
        param.picture = entry->picture;
        param.sound = entry->sound;
        param.model = entry->model;
        param.name = entry->name;
        param.reading = entry->reading;
        BW3BattleOrDataBase = 1;
        eBattleWinOpen3(&param);
        work->opened = 1;
        work->step = UMN_DB_ANALYSIS_SHOWN;
        /* fallthrough */
    case UMN_DB_ANALYSIS_SHOWN:
        if (UmnWork.tab_state == UMN_DB_STEP_DETAIL && UmnWork.database.model_flag == 0
            && !(((UmnPadInputPrefix *)&PadData)->wide.buttons.buttonRegister & PAD_MOVE)) {
            break;
        }
        work->step = UMN_DB_ANALYSIS_CLOSE;
        if (PadData.half_2a & PAD_CIRCLE) {
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;
    case UMN_DB_ANALYSIS_CLOSE:
        eBattleWinClose3();
        work->step = UMN_DB_ANALYSIS_WAIT;
        break;
    }

    if (work->opened != 0) {
        eBattleWinMain3();
    }
}

/*
 * The top menu of the U.M.N. database: the first frame builds its window and
 * the three entries, then the window slides in, the cursor follows the
 * terminal and the window slides out again when the menu closes.
 */
void tskUmnDataBaseMenu(UmnObjectTask *task, void *data)
{
    UmnDataBaseMenuWork *work = data;
    short slideTarget;
    short closeTarget;
    int i;
    int mode;

    mode = UmnWork.mode;
    if (mode != UMN_MODE_DATA_BASE) {
        task->state = UMN_TASK_END;
        return;
    }
    switch (task->state) {
    case UMN_TASK_INIT:
        work->x = 528;
        work->y = 48;
        work->color = 0x00FFFFF0;
        work->pending = 0;
        WindowDXSet(&work->window);
        work->window.style = work->color;
        work->window.width = 125;
        work->window.height = 78;
        work->info.y = mode;
        work->window.tag_id = dataBaseMenuTagText;
        work->window.close_callback = MenuSelectWindow;
        work->window.close_callback_arg = &work->info;
        work->info.entries = 0;
        work->state = UMN_DB_MENU_IDLE;
        work->opened = 0;
        break;
    case UMN_TASK_RUN:
        switch (work->state) {
        case UMN_DB_MENU_IDLE:
            work->opened = 0;
            work->state = UMN_DB_MENU_WAIT_OPEN;
            /* fallthrough */
        case UMN_DB_MENU_WAIT_OPEN:
            if (UmnWork.tab_state == UMN_STEP_ACTIVE) {
                work->state = UMN_DB_MENU_BUILD;
            }
            break;
        case UMN_DB_MENU_BUILD:
            for (i = 0; i < UMN_DB_MENU_ENTRIES; i++) {
                work->entries[i].text = text2_2[i];
                if (i == 0 && UmnWork.count == 0) {
                    work->entries[i].enabled = 1;
                } else {
                    work->entries[i].enabled = 0;
                }
            }
            work->entries[i].text = 0;
            work->pending = 1;
            work->window.x = 528;
            work->window.y = 176;
            work->info.value = UmnWork.cursor;
            work->info.entries = &work->entries[0];
            work->window.state = WINDOWDX_STATE_OPEN;
            WindowDXMain(&work->window);
            work->window.state = WINDOWDX_STATE_READY;
            work->opened = 1;
            work->state = UMN_DB_MENU_SLIDE_IN;
            /* fallthrough */
        case UMN_DB_MENU_SLIDE_IN:
            slideTarget = 192;
            MoveSlide(&work->window.x, &slideTarget, 3.0f);
            if (work->window.x == slideTarget) {
                work->state = UMN_DB_MENU_SHOWN;
            }
            break;
        case UMN_DB_MENU_SHOWN:
            work->info.value = UmnWork.cursor;
            if (UmnWork.tab_state != UMN_STEP_ACTIVE) {
                work->state = UMN_DB_MENU_SLIDE_OUT;
            } else {
                UmnWork.moving = 0;
            }
            break;
        case UMN_DB_MENU_SLIDE_OUT:
            closeTarget = -125;
            if ((UmnWork.tab_state >> 4) == (UMN_DB_STEP_KEYWORD_OPEN >> 4)) {
                work->opened = 0;
                work->window.x = -125;
            } else {
                MoveSlide(&work->window.x, &closeTarget, 3.0f);
                if (work->window.x == closeTarget) {
                    work->state = UMN_DB_MENU_IDLE;
                    work->pending = 0;
                }
            }
            break;
        }
        break;
    }
    if (work->opened != 0) {
        if (work->pending != 0) {
            WindowDXMain(&work->window);
        }
    }
}

static const UmnDataBaseArrowSprites dataBaseArrowSpriteTable = {{0x0112, 0x0110}};
static const UmnDataBaseArrowSteps dataBaseArrowStepTable = {{1, -1}};
static const char *text_3_00A10720[5];

/*
 * The extra window of the U.M.N. database: five lines of help text with an
 * arrow sprite at each end. The lines slide in while the terminal shows a
 * record, the square button hides and shows the text and each arrow runs off
 * its own side once the player pages that way.
 */
void tskUmnDataBaseExWin(UmnObjectTask *task, void *data)
{
    UmnDataBaseExWinWork *work = data;
    UmnDataBaseArrowSprites arrowSprite;
    short lineTarget[UMN_DB_EXWIN_LINES];
    short arrowTarget[UMN_DB_EXWIN_ARROWS];
    UmnDataBaseArrowSteps arrowStep;
    int buttons;
    int pressed;
    int i;

    if (UmnWork.mode != UMN_MODE_DATA_BASE) {
        task->state = UMN_TASK_END;
        return;
    }

    switch (task->state) {
    case UMN_TASK_INIT:
        work->color = 0xFFFFF0;
        for (i = 0; i < UMN_DB_EXWIN_LINES; i++) {
            work->lines[i].y = i * 0x18 + 0x120;
            work->lines[i].x = -0xC4;
            eMessageSet(&work->lines[i].message, text_3_00A10720[i]);
            eMessageModeChange(&work->lines[i].message, E_MODE_SHOW_START);
        }
        arrowSprite = dataBaseArrowSpriteTable;
        for (i = 0; i < UMN_DB_EXWIN_ARROWS; i++) {
            eSpriteSet(&work->sprites[i], arrowSprite.id[i]);
            work->sprites[i].x = i * 0x23D - 0x2D;
            work->sprites[i].color = work->color;
            work->drift[i] = 0;
            work->sprites[i].y = 0xD6;
        }
        work->opened = 0;
        work->textShown = 1;
        work->step = 0;
        break;
    case UMN_TASK_RUN:
    {
        for (i = 0; i < UMN_DB_EXWIN_LINES; i++) {
            lineTarget[i] = -0xC4;
        }
        arrowTarget[0] = -0x2D;
        arrowTarget[1] = 0x210;
        if (UmnWork.tab_state == UMN_DB_STEP_DETAIL) {
            for (i = 0; i < UMN_DB_EXWIN_LINES; i++) {
                lineTarget[i] = 0x10;
            }
            buttons = PadData.half_2a;
            if (buttons & PAD_SQUARE) {
                work->textShown ^= 1;
                buttons = PadData.half_2a;
            }
            pressed = buttons & 0xFFFF;
            arrowTarget[0] = 8;
            arrowTarget[1] = 0x1DB;
            if (pressed == PAD_L1) {
                work->drift[0] = -6;
            }
            if (pressed == PAD_R1) {
                work->drift[1] = 6;
            }
        } else {
            work->textShown = 1;
        }
        for (i = 0; i < UMN_DB_EXWIN_LINES; i++) {
            MoveSlide(&work->lines[i].x, &lineTarget[i], 3.0f);
            work->lines[i].message.x = work->lines[i].x;
            work->lines[i].message.y = work->lines[i].y;
            work->lines[i].message.draw_parameter = work->color;
            if (i == UMN_DB_EXWIN_LINES - 1) {
                if (UmnWork.database.analysed != 0) {
                    work->lines[i].message.color_b = 0x80;
                    work->lines[i].message.color_g = 0x80;
                    work->lines[i].message.color_r = 0x80;
                } else {
                    work->lines[i].message.color_b = 0x40;
                    work->lines[i].message.color_g = 0x40;
                    work->lines[i].message.color_r = 0x40;
                }
            }
            if (work->textShown != 0) {
                eMessageMain(&work->lines[i].message);
            }
        }
        for (i = 0; i < UMN_DB_EXWIN_ARROWS; i++) {
            if (work->drift[i] != 0) {
                arrowStep = dataBaseArrowStepTable;
                work->drift[i] += arrowStep.step[i];
            }
            MoveSlide(&work->sprites[i].x, &arrowTarget[i], 3.0f);
            work->sprites[i].x += work->drift[i];
            eSpriteMain(&work->sprites[i]);
        }
    }
        break;
    }
}

static const char dataBaseNumberTagText[5] = "\x01Num";
static char msg00_4[16];

/*
 * The keyword screen of the U.M.N. database: the frame the task starts on
 * builds the database page and the counter row, and from then on the screen
 * waits for the terminal to reach the keyword step, drives the page while it
 * is there and slides the counter row in and out again.
 */
void tskUmnDataBaseKeyWord(UmnObjectTask *task, void *data)
{
    UmnDataBaseKeyWordWork *work = data;
    short slideTarget;

    if (UmnWork.mode != UMN_MODE_DATA_BASE) {
        task->state = UMN_TASK_END;
        return;
    }

    switch (task->state) {
    case UMN_TASK_INIT:
        work->color = 0x00F00000;
        tyaUmlDispParamReset(&work->disp.display, 0);
        work->disp.window = &work->window;
        work->disp.menu = &work->list;
        work->disp.cursorA = &work->selectCursor;
        work->disp.cursorB = &work->indexCursor;
        work->disp.display.render_x = 1808;
        work->disp.display.render_width = 480;
        work->disp.display.render_y = 1920;
        work->disp.display.color = work->color;
        work->disp.display.render_height = 314;
        work->x = UMN_KEYWORD_HIDDEN_X;
        work->y = 48;
        eRibbonSet(&work->ribbon, 3);
        work->ribbon.width = 256;
        work->ribbon.height = 24;
        eTagFontSet(&work->tag, dataBaseNumberTagText);
        eNumberSet(&work->number, 0);
        eMessageSet(&work->message, msg00_4);
        work->message.mode = E_MODE_SHOW_START;
        work->step = UMN_DB_KEYWORD_IDLE;
        work->active = 0;
        break;
    case UMN_TASK_RUN:
        switch (work->step) {
        case UMN_DB_KEYWORD_IDLE:
            work->active = 0;
            work->disp.state = 0;
            /* fallthrough */
        case UMN_DB_KEYWORD_WAIT:
            if (UmnWork.tab_state == UMN_DB_STEP_KEYWORD) {
                work->step = UMN_DB_KEYWORD_OPEN;
            }
            break;
        case UMN_DB_KEYWORD_OPEN:
            work->active = 1;
            work->step = UMN_DB_KEYWORD_SHOWN;
            work->visible = 1;
            work->messageShown = 1;
            /* fallthrough */
        case UMN_DB_KEYWORD_SHOWN:
            if (UmnWork.tab_state == UMN_STEP_ACTIVE && UmnWork.database.data_base_result == 1) {
                work->step = UMN_DB_KEYWORD_IDLE;
            }
            break;
        }

        if (work->active != 0) {
            if (work->disp.fileIndex >= 0) {
                if (PadData.half_2a & PAD_CIRCLE) {
                    work->messageShown = 0;
                }
                if (PadData.half_2a & PAD_CROSS) {
                    work->messageShown = 1;
                }
            }
            UmnWork.database.data_base_result = tyaUmlDatabaseMain(&work->disp);
            work->indexNo = work->disp.fileIndex;
            work->selectNo = work->disp.foundCount;
            if (work->messageShown != 0) {
                work->message.x = 48;
                work->message.y = 416;
                work->message.draw_parameter = work->color + 15;
                eMessageMain(&work->message);
            }
        }

        if (work->visible != 0) {
            slideTarget = UMN_KEYWORD_HIDDEN_X;
            /* Only the terminal's keyword steps keep the row on screen. */
            if (UmnWork.tab_state >> 4 == UMN_DB_STEP_KEYWORD_OPEN >> 4) {
                slideTarget = 0;
            }
            MoveSlide(&work->x, &slideTarget, 3.0f);
            if (work->x == UMN_KEYWORD_HIDDEN_X) {
                work->visible = 0;
            }
            work->y = 48;
            work->ribbon.x = work->x;
            work->ribbon.y = work->y;
            work->ribbon.work = work->color;
            eRibbonMain(&work->ribbon);
            work->tag.x = work->x + 166;
            work->tag.y = work->y + 4;
            work->tag.work = work->ribbon.work + 1;
            eTagFontMain(&work->tag);
            work->number.x = work->tag.x + 41;
            work->number.y = work->tag.y;
            work->number.work = work->tag.work;
            work->number.value = (work->selectNo << 16) + (unsigned short) work->indexNo;
            work->number.format = 3;
            work->number.digits = 3;
            eNumberMain(&work->number);
        }
        break;
    }
}

/*
 * Model unit callback for the record model: sets its initial transform, opens
 * it while the detail step is shown and closes it when that step ends. Once it
 * is hidden, taskPhase is set to -1.
 */
void UmnDataBaseModel(MenuModelUnit *work, void *context)
{
    UmnDataBaseModelData *model = (UmnDataBaseModelData *)work->actor;

    if (model == 0) {
        if (UmnWork.database.model_flag != 0) {
            MenuModelUnitBreak(work);
            UmnWork.database.model_flag = 0;
        }
    }

    if (work->active != 0) {
        UmnDataBaseModelXform *xform = &model->xform;

        if (UmnWork.tab_state != UMN_DB_STEP_DETAIL) {
            if (work->extFuncFlag != UMN_DB_MODEL_DONE) {
                work->extFuncFlag = UMN_DB_MODEL_CLOSE;
            }
        }
        if (UmnWork.database.model_flag != 0) {
            work->extFuncFlag = UMN_DB_MODEL_CLOSE;
            UmnWork.database.model_flag = 0;
        }
        MenuModelControl(work);
        switch (work->extFuncFlag) {
        case UMN_DB_MODEL_INIT: {
            float *scale = xform->scale;

            xform->position[0] = -xform->baseRotation[0];
            xform->position[1] = -xform->baseRotation[1];
            xform->position[2] = -xform->baseRotation[2];
            scale[2] = 0.5f;
            scale[1] = 0.5f;
            scale[0] = 0.5f;
            work->extFuncFlag = UMN_DB_MODEL_OPEN;
        }
            /* fallthrough */
        case UMN_DB_MODEL_OPEN:
            MenuModelUnitOpen(work, 2);
            work->extFuncFlag = UMN_DB_MODEL_SHOWN;
            break;
        case UMN_DB_MODEL_SHOWN:
            break;
        case UMN_DB_MODEL_CLOSE:
            MenuModelUnitOpen(work, 1);
            work->extFuncFlag = UMN_DB_MODEL_CLOSING;
            /* fallthrough */
        case UMN_DB_MODEL_CLOSING:
            if (xform->visible == 0.0f) {
                work->extFuncFlag = UMN_DB_MODEL_DONE;
        case UMN_DB_MODEL_DONE:
                work->taskPhase = -1;
            }
            break;
        }
    }
}

/*
 * The U.M.N. database terminal: it starts the seven screens of the database on
 * the first frame, keeps the folder of records the player may look up, and
 * drives the top menu, the name list and the record display from the pad.
 */
void UmnDataBase(void)
{
    int found;
    int id;
    int work;
    signed char moved;
    int first;
    int repeats;
    int steps;
    int column;

    switch (UmnWork.tab_state) {
    case UMN_STEP_INIT:
        found = 0;
        work = UmnWorkEnd;
        work = (work + 0xF) & ~0xF;
        UmnObjectTaskCreate(tskUmnDataBasePas, (void *)work);
        work = (work + 0x3D7) & ~0xF;
        UmnObjectTaskCreate(tskUmnDataBaseInfo, (void *)work);
        work = (work + 0x35F) & ~0xF;
        UmnObjectTaskCreate(tskUmnDataBaseName, (void *)work);
        work = (work + 0x222F) & ~0xF;
        UmnObjectTaskCreate(tskUmnDataBaseMenu, (void *)work);
        work = (work + 0x7DB) & ~0xF;
        UmnObjectTaskCreate(tskUmnDataBaseExWin, (void *)work);
        work = (work + 0x1D7) & ~0xF;
        UmnObjectTaskCreate(tskUmnDataBaseKeyWord, (void *)work);
        UmnObjectTaskCreate(tskUmnDataBaseAnalisis, 0);
        memset(monster_folder, 0, 0x80);
        for (id = UMN_DB_FIRST_ID; id < UMN_DB_FIRST_ID + UMN_DB_ENTRY_COUNT; id++) {
            if (UmnDataBaseMonsterCheck(id) != 0) {
                monster_folder[found] = id;
                found++;
            }
        }
        UmnWork.count = found;
        UmnWork.rows = (UmnWork.count - 1) / 2 + 1;
        if (UmnWork.rows >= UMN_DB_LIST_ROWS_MAX + 1) {
            UmnWork.rows = UMN_DB_LIST_ROWS_MAX;
        }
        UmnWork.tab_state = UMN_STEP_OPEN;
        break;
    case UMN_STEP_OPEN:
        UmnWork.tab_state = UMN_STEP_ACTIVE;
        UmnWork.wait = UMN_WAIT_OPEN;
        UmnWork.cursor = 0;
        /* fallthrough */
    case UMN_STEP_ACTIVE:
        if (UmnWork.wait == 0) {
            UmnWork.cursor = MenuSelectMove(UmnWork.cursor, UMN_DB_MENU_ENTRIES, 0);
            if (PadData.half_2a & PAD_CIRCLE) {
                switch (UmnWork.cursor) {
                case UMN_DB_MENU_LIST:
                    if (UmnWork.count == 0) {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    } else {
                        UmnWork.tab_state = UMN_DB_STEP_LIST_OPEN;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    }
                    break;
                case UMN_DB_MENU_KEYWORD:
                    UmnWork.tab_state = UMN_DB_STEP_KEYWORD_OPEN;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    break;
                case UMN_DB_MENU_EXIT:
                    UmnWork.tab_state = UMN_STEP_LEAVE;
                    UmnWork.wait = UMN_WAIT_LEAVE;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                    break;
                }
            }
            if (PadData.half_2a & PAD_CROSS) {
                UmnWork.tab_state = UMN_STEP_LEAVE;
                UmnWork.wait = UMN_WAIT_LEAVE;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    case UMN_DB_STEP_LIST_OPEN:
        UmnWork.column = 0;
        UmnWork.row = 0;
        UmnWork.entry = monster_folder[0];
        UmnWork.scroll = 0;
        UmnWork.list_cursor = 0;
        UmnWork.database.analysed = UmnDataBaseAnalisisCheck(UmnWork.entry);
        UmnWork.tab_state = UMN_DB_STEP_LIST;
        UmnWork.wait = UMN_WAIT_OPEN;
        UmnWork.moving = 1;
        /* fallthrough */
    /* List step: the pad moves the cursor over the records; circle opens the detail view. */
    case UMN_DB_STEP_LIST:
        if (UmnWork.wait_or_moving == 0) {
            repeats = 1;
            moved = 0;
            switch (PadData.mappedRepeat) {
            case PAD_UP:
                repeats = 2;
                moved = -1;
                break;
            case PAD_LEFT:
                moved = -1;
                break;
            case PAD_DOWN:
                repeats = 2;
                /* fallthrough */
            case PAD_RIGHT:
                moved = 1;
                break;
            }
            if (moved != 0) {
                first = 1;
                for (steps = 0; steps < repeats; steps++) {
                    UmnWork.list_cursor += moved;
                    if (UmnWork.list_cursor < 0) {
                        UmnWork.list_cursor = 0;
                        moved = 0;
                    } else if (UmnWork.list_cursor > UmnWork.count - 1) {
                        UmnWork.list_cursor = UmnWork.count - 1;
                        moved = 0;
                    }
                    column = UmnWork.list_cursor & 1;
                    UmnWork.column = column;
                    if (moved < 0) {
                        if (UmnWork.column != 0) {
                            if (UmnWork.row == 0) {
                                UmnWork.scroll--;
                            } else {
                                UmnWork.row--;
                            }
                        }
                    } else if (moved > 0) {
                        if (UmnWork.column == 0) {
                            if (UmnWork.row == 5) {
                                UmnWork.scroll++;
                            } else {
                                UmnWork.row++;
                            }
                        }
                    } else {
                        continue;
                    }
                    if (first != 0) {
                        xglSoundEffectNormalID(SE_CURSOR, 0);
                    }
                    first = 0;
                }
                UmnWork.entry = monster_folder[UmnWork.list_cursor];
                UmnWork.database.analysed = UmnDataBaseAnalisisCheck(UmnWork.entry);
            }
            if (PadData.half_2a & PAD_CIRCLE) {
                UmnWork.tab_state = UMN_DB_STEP_DETAIL_OPEN;
                xglSoundEffectNormalID(SE_DECIDE, 0);
            }
            if (PadData.half_2a & PAD_CROSS) {
                UmnWork.tab_state = UMN_STEP_ACTIVE;
                UmnWork.wait = UMN_WAIT_OPEN;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    /* Detail step: L1 and R1 page through the records; cross returns to the list. */
    case UMN_DB_STEP_DETAIL_OPEN:
        if (UmnModelUkn != 0) {
            break;
        }
        MenuModelMemorySet(1);
        UmnWork.database.model_flag = 0;
        UmnWork.tab_state = UMN_DB_STEP_DETAIL;
        UmnWork.wait = UMN_WAIT_OPEN;
        UmnWork.moving = 1;
        /* fallthrough */
    case UMN_DB_STEP_DETAIL:
        if (UmnWork.wait_or_moving == 0) {
            if (UmnWork.count >= 2) {
                if (PadData.half_2a == PAD_L1) {
                    if (UmnWork.list_cursor == 0) {
                        UmnWork.list_cursor = UmnWork.count - 1;
                        UmnWork.column = UmnWork.list_cursor & 1;
                        UmnWork.row = UmnWork.rows - 1;
                    } else {
                        UmnWork.list_cursor--;
                        if (UmnWork.column == 1) {
                            UmnWork.column--;
                        } else {
                            UmnWork.column = 1;
                            if (UmnWork.row > 0) {
                                UmnWork.row--;
                            } else {
                                UmnWork.scroll--;
                            }
                        }
                    }
                    UmnWork.entry = monster_folder[UmnWork.list_cursor];
                    UmnWork.database.analysed = UmnDataBaseAnalisisCheck(UmnWork.entry);
                    UmnWork.database.model_flag = 1;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
                if (PadData.half_2a == PAD_R1) {
                    if (UmnWork.list_cursor == UmnWork.count - 1) {
                        UmnWork.list_cursor = 0;
                        UmnWork.column = 0;
                        UmnWork.row = 0;
                    } else {
                        UmnWork.list_cursor++;
                        if (UmnWork.column == 0) {
                            UmnWork.column++;
                        } else {
                            UmnWork.column = 0;
                            if (UmnWork.row < UmnWork.rows - 1) {
                                UmnWork.row++;
                            } else {
                                UmnWork.scroll++;
                            }
                        }
                    }
                    UmnWork.entry = monster_folder[UmnWork.list_cursor];
                    UmnWork.database.analysed = UmnDataBaseAnalisisCheck(UmnWork.entry);
                    UmnWork.database.model_flag = 1;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                }
            }
            if (PadData.half_2a == PAD_CROSS) {
                UmnWork.tab_state = UMN_DB_STEP_LIST;
                UmnWork.moving = 1;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        if (UmnModelSon == 0) {
            MenuModelCreate(&UmnModelSon, UmnWork.entry);
            MenuModelExtFuncSet(UmnModelSon, UmnDataBaseModel, 0);
        }
        break;
    case UMN_DB_STEP_KEYWORD_OPEN:
        UmnWork.tab_state = UMN_DB_STEP_KEYWORD;
        UmnWork.database.data_base_result = 0;
        /* fallthrough */
    case UMN_DB_STEP_KEYWORD:
        if (UmnWork.database.data_base_result == 1) {
            UmnWork.tab_state = UMN_STEP_ACTIVE;
            UmnWork.wait = UMN_WAIT_OPEN;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;
    /* Leaving waits for the record model to finish before returning to the top level. */
    case UMN_STEP_LEAVE:
        if (UmnModelSon != 0) {
            if (UmnModelSon->taskPhase == 0xFF) {
                UmnModelSon = 0;
            }
            break;
        }
        UmnWork.tab_state = UMN_STEP_LEAVE_WAIT;
        /* fallthrough */
    case UMN_STEP_LEAVE_WAIT:
        if (UmnWork.wait == 0) {
            MenuModelMemorySet(0);
            UmnChangeTopLevel(0);
        }
        break;
    }

    if (UmnWork.wait != 0) {
        UmnWork.wait--;
    }
    xglFontDebugHex(0, 0x40, UmnWork.tab_state, 2);
    xglFontDebugPrintf(0x10, 0x18, dataBaseEntryDebugFormat, UmnWork.entry);
    xglFontDebugPrintf(0x10, 0x20, dataBaseListCursorDebugFormat, UmnWork.list_cursor, UmnWork.count);
    xglFontDebugPrintf(0x10, 0x28, dataBaseColumnRowDebugFormat, UmnWork.column, UmnWork.row, UmnWork.rows);
    xglFontDebugPrintf(0x10, 0x30, dataBaseMovingDebugFormat, UmnWork.moving);
}

static const char *msg00_1_00A10700[3] = {
    dataBaseGnosisHelpText, dataBaseKeywordHelpText, dataBaseCancelHelpText,
};
static const char *text_3_00A10720[5] = {
    dataBaseZoomOutHintText, dataBaseZoomInHintText, dataBaseRotateHintText, dataBaseTurnMenuOffHintText, dataBaseAnalysisInfoHintText,
};
static char msg00_4[16] = "\x1E\0Index Search:";

const char dataBaseCancelHelpText[8] = "Cancel.";

const char dataBaseKeywordHelpText[48] = "View keywords that you have come across.";

const char dataBaseGnosisHelpText[56] = "View data of all the Gnosis you have encountered.";

const char dataBaseAnalysisInfoHintText[24] = "\036\001\241\247Analysis info";

const char dataBaseTurnMenuOffHintText[24] = "\036\000\241\247Turn menu off";

const char dataBaseRotateHintText[16] = "\0364\241\247Rotate";

const char dataBaseZoomInHintText[16] = "\0365\241\247Zoom in";

const char dataBaseZoomOutHintText[16] = "\0366\241\247Zoom out";

extern const char databaseTitleText[16];
extern const char databaseCancelLabel[8];
extern const char databaseKeywordsLabel[16];
extern const char databaseGnosisLabel[8];

static const char *msg00_0_00A106F8 = databaseTitleText;

static const char *text2_2[3] = { databaseGnosisLabel, databaseKeywordsLabel, databaseCancelLabel };

unsigned short monster_folder[128] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

const char databaseTitleText[16] = "Database";

const char dataBaseRowTagText[8] = "No.";

const char databaseCancelLabel[8] = "Cancel";

const char databaseKeywordsLabel[16] = "Keywords";

const char databaseGnosisLabel[8] = "Gnosis";

const char dataBaseMenuTagText[8] = "Menu";

const char dataBaseEntryDebugFormat[8] = "id %2d";

const char dataBaseListCursorDebugFormat[8] = "%2d/%2d";

const char dataBaseColumnRowDebugFormat[16] = "%2d/%2d/";

const char dataBaseMovingDebugFormat[16] = "st=%2d";
