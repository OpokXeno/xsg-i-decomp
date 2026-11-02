/*
 * OV02 original TU 9: 0x00a05738..0x00a097d8 (10 functions)
 */
#include "common.h"

#include "shared.h"
#include "main/e_battle_win_open.h"
#include "main/e_message.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/menu_1.h"
#include "main/menu_model.h"
#include "main/menu_para_pt_rate_get.h"
#include "main/menu_top_command_check.h"
#include "main/xgl_dma.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"
#include "ov02/ov02.h"
#include "ov02/title_hdd_install.h"
#include "ov02/tsk_umn_mail_pas.h"
#include "ov02/tsk_umn_object_task_main.h"
#include "ov02/umn_event_text_symbol_check.h"
#include "ov02/umn_procurator.h"

/* Captions of the mail title bar (Mail, Folder, Read, Reply, History). */
#define UMN_MAIL_CAPTIONS 5
/* Bytes of the info line's text buffer. */
#define UMN_MAIL_INFO_TEXT_SIZE 0x100
/* Mails 0..80 are the roots the history tree is searched from. */
#define UMN_MAIL_HISTORY_ROOTS 81
/* Header name byte that marks an unused reply slot in UmnWork.name. */
#define UMN_MAIL_NAME_NONE 0xFF

/* Folder: records kept, records on screen (a fifth one scrolls in), record pitch and scroll speed in pixels. */
#define UMN_MAIL_FOLDER_RECORDS 6
#define UMN_MAIL_FOLDER_VISIBLE 4
#define UMN_MAIL_FOLDER_TOP 96
#define UMN_MAIL_FOLDER_PITCH 64
#define UMN_MAIL_FOLDER_SCROLL_SPEED 8
#define UMN_MAIL_FOLDER_HIDDEN_X (-448)
/* Entries of the mail menu (read, history, attachment, back) and of the reply confirmation. */
#define UMN_MAIL_MENU_ENTRIES 4
#define UMN_MAIL_CONFIRM_ENTRIES 2
/* Sprites of the folder rows (sprite sheet ids). */
#define UMN_MAIL_SPRITE_UNREAD 5128
#define UMN_MAIL_SPRITE_REPLY 5129
#define UMN_MAIL_SPRITE_READ 5130
#define UMN_MAIL_SPRITE_ATTACH 5126
#define UMN_MAIL_SPRITE_NO_ATTACH 5127
/* Brightness of the dimmed ex-window texts. */
#define UMN_MAIL_TEXT_DIM 64

/* UmnWork.tab_state steps of the mail screen beyond the common UMN_STEP_*. */
#define UMN_MAIL_STEP_SWITCH_OPEN 0x10
#define UMN_MAIL_STEP_SWITCH 0x11
#define UMN_MAIL_STEP_FOLDER_OPEN 0x20
#define UMN_MAIL_STEP_FOLDER 0x21
#define UMN_MAIL_STEP_MENU_OPEN 0x30
#define UMN_MAIL_STEP_MENU 0x31
#define UMN_MAIL_STEP_READ_WAIT 0x35
#define UMN_MAIL_STEP_READ_OPEN 0x40
#define UMN_MAIL_STEP_READ 0x41
#define UMN_MAIL_STEP_REPLY_OPEN 0x50
#define UMN_MAIL_STEP_REPLY_SELECT 0x51
#define UMN_MAIL_STEP_REPLY_CONFIRM_OPEN 0x52
#define UMN_MAIL_STEP_REPLY_CONFIRM 0x53
#define UMN_MAIL_STEP_REPLY_SEND 0x54
#define UMN_MAIL_STEP_REPLY_DONE 0x55
#define UMN_MAIL_STEP_ATTACH_OPEN 0x60
#define UMN_MAIL_STEP_ATTACH_DONE 0x61
#define UMN_MAIL_STEP_EVENT 0x80

/* UmnWork.cursor on the mail menu. */
enum {
    UMN_MAIL_MENU_READ,
    UMN_MAIL_MENU_HISTORY,
    UMN_MAIL_MENU_ATTACH,
    UMN_MAIL_MENU_BACK
};

/* tskUmnMailFolder states (UmnMailFolderWork.state). */
enum {
    UMN_MAIL_FOLDER_IDLE = 0,
    UMN_MAIL_FOLDER_WAIT = 10,
    UMN_MAIL_FOLDER_SLIDE_IN = 20,
    UMN_MAIL_FOLDER_LIST = 30,
    UMN_MAIL_FOLDER_SCROLL_START = 32,
    UMN_MAIL_FOLDER_SCROLL = 34,
    UMN_MAIL_FOLDER_SLIDE_MENU = 40,
    UMN_MAIL_FOLDER_SLIDE_OUT = 42,
    UMN_MAIL_FOLDER_MENU = 45,
    UMN_MAIL_FOLDER_FOCUS = 50,
    UMN_MAIL_FOLDER_FOCUSED = 60,
    UMN_MAIL_FOLDER_HIDE_FOCUSED = 70,
    UMN_MAIL_FOLDER_HIDDEN = 80,
    UMN_MAIL_FOLDER_CLEAR = 90
};

/* tskUmnMailDisp steps (UmnMailDispWork.step). */
enum {
    UMN_MAIL_DISP_IDLE = 0,
    UMN_MAIL_DISP_WAIT = 10,
    UMN_MAIL_DISP_WAIT_MODEL = 20,
    UMN_MAIL_DISP_OPEN = 30,
    UMN_MAIL_DISP_SHOWN = 40,
    UMN_MAIL_DISP_CLOSE = 50,
    UMN_MAIL_DISP_NAME_PREPARE = 60,
    UMN_MAIL_DISP_NAME_OPEN = 70,
    UMN_MAIL_DISP_NAME_SHOWN = 80
};

/* UmnMailDispWork.scrol_type: how the text window is being scrolled. */
enum {
    UMN_MAIL_SCROLL_NONE,
    UMN_MAIL_SCROLL_LINE_UP,
    UMN_MAIL_SCROLL_LINE_DOWN,
    UMN_MAIL_SCROLL_PAGE_UP,
    UMN_MAIL_SCROLL_PAGE_DOWN,
    UMN_MAIL_SCROLL_TOP,
    UMN_MAIL_SCROLL_BOTTOM,
    UMN_MAIL_SCROLL_NEXT_COLUMN
};

/* tskUmnMailHensin steps (UmnMailHensinWork.step). */
enum {
    UMN_MAIL_HENSIN_IDLE = 0,
    UMN_MAIL_HENSIN_WAIT = 10,
    UMN_MAIL_HENSIN_SLIDE_IN = 20,
    UMN_MAIL_HENSIN_SHOWN = 30,
    UMN_MAIL_HENSIN_SLIDE_OUT = 40
};

/* tskUmnMailMenu states (UmnMailMenuWork.state). */
enum {
    UMN_MAIL_MENU_IDLE = 0,
    UMN_MAIL_MENU_WAIT = 10,
    UMN_MAIL_MENU_SLIDE_IN = 20,
    UMN_MAIL_MENU_SHOWN = 30,
    UMN_MAIL_MENU_SLIDE_OUT = 40,
    UMN_MAIL_CONFIRM_SLIDE_IN = 50,
    UMN_MAIL_CONFIRM_SHOWN = 60,
    UMN_MAIL_CONFIRM_SLIDE_OUT = 70
};

typedef struct UmnMailPasWork {
    unsigned char unmodeled_00;
    unsigned char state;
    unsigned char unmodeled_02[2];
    int color;
    WindowDX window;
    EMessageParam messages[UMN_MAIL_CAPTIONS];
    unsigned char unmodeled_2f0[0x3bc - 0x2f0];
    WindowDXRect box;
} UmnMailPasWork;
struct UmnMailInfoWork {
    unsigned char mode;
    unsigned char saved_mail_mode;
    unsigned char unmodeled_02[6];
    int color;
    WindowDX window;
    struct UmnMailInfoText {
        short x;
        short y;
        unsigned char unmodeled_04[8];
        char *text;
    } info;
    unsigned char unmodeled_1b0[0x350 - 0x1b0];
    char text_buffer[UMN_MAIL_INFO_TEXT_SIZE];
};
struct UmnMailCaptionA {
    const char *text[6];
};
struct UmnMailCaptionB {
    const char *text[4];
};
struct UmnMailCaptionC {
    const char *text[7];
};
typedef struct UmnMailFolderSprite {
    int first_word;
    unsigned short x;
    unsigned short y;
    int color;
    unsigned char rgb[3];
    unsigned char unmodeled_0f[0x28 - 0x0f];
} UmnMailFolderSprite;
/* The first two bytes of a mail's save record (status and texture flags);
 * each folder row keeps a copy of them. */
typedef struct UmnMailFolderHead {
    signed char flags;
    unsigned char tex_flags;
} UmnMailFolderHead;
typedef struct UmnMailFolderRecord {
    UmnMailFolderHead head;
    unsigned char unmodeled_02[2];
    int folder_id;
    short x;
    short y;
    WindowDX window;
    UmnMailFolderSprite sprites[4];
    EMessageParam messages[2];
} UmnMailFolderRecord;
typedef struct UmnMailFolderWork {
    unsigned char state;
    unsigned char flag;
    unsigned char unmodeled_02;
    unsigned char count;
    short x;
    short y;
    int color;
    UmnMailFolderRecord records[UMN_MAIL_FOLDER_RECORDS];
    ECursolMainData cursor;
    int cursor_visible;
    ECursolMainData cursor2;
    ECursolMainData cursor3;
    int box_visible;
    WindowDXRect box;
    int tex;
} UmnMailFolderWork;
typedef struct UmnMailDispSprite {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[28];
} UmnMailDispSprite;
typedef struct UmnMailDispWork {
    unsigned char step;
    unsigned char state;
    signed char shown_entry;
    signed char now_page;
    signed char column_index;
    signed char scrol_type;
    signed char flag_06;
    signed char flag_07;
    unsigned short pos_x;
    unsigned short pos_y;
    int pos_kind;
    short slide_x;
    short slide_y;
    WindowDX window;
    int tya_value;
    unsigned char unmodeled_1ac[4];
    short tya_pos_x;
    short tya_pos_y;
    short cursor_x;
    short cursor_pitch;
    unsigned long long tya_kind;
    short slide_dx;
    short scroll_x;
    unsigned char unmodeled_1c4[2];
    unsigned short scroll_end;
    unsigned char unmodeled_1c8[8];
    unsigned char item_count;
    unsigned char unmodeled_1d1;
    short column_x[7];
    unsigned char unmodeled_1e0[8];
    short scroll_target;
    unsigned char unmodeled_1ea[2];
    ECursolMainData cursor_slots[2];
    EMessageParam message;
    int tex_addr;
    UmnMailDispSprite sprites[4];
    signed char item_anim[4];
} UmnMailDispWork;
typedef struct UmnMailSpriteIds {
    short id[4];
} UmnMailSpriteIds;
typedef struct UmnMailHensinWork {
    unsigned char step;
    unsigned char state;
    unsigned char unmodeled_02[2];
    short anchor_x;
    short anchor_y;
    int anchor_color;
    WindowDX window;
    EMessageParam message;
    EMessageParam messages[UMN_MAIL_NAME_CHARS];
    ECursolMainData cursor;
} UmnMailHensinWork;
typedef struct UmnMailMenuWork {
    unsigned char state;
    unsigned char flag;
    unsigned char unmodeled_02[2];
    short origin_x;
    short origin_y;
    int color;
    int window1_visible;
    WindowDX window1;
    MenuTopMenuPanel select1;
    int window2_visible;
    WindowDX window2;
    MenuTopMenuPanel select2;
    MenuTopCommandEntry entries[UMN_MAIL_MENU_ENTRIES];
} UmnMailMenuWork;
typedef struct UmnMailExSlot {
    short slide_x;
    short slide_y;
    EMessageParam message;
} UmnMailExSlot;
typedef struct UmnMailExRow {
    short slide_x;
    short slide_y;
    ETagFontMainData tag;
    ERibbonMainData ribbon;
    ENumberMainData number;
} UmnMailExRow;
typedef struct UmnMailExWork {
    unsigned char unmodeled_00;
    unsigned char state;
    unsigned char unmodeled_02[2];
    short window_x;
    short window_y;
    int window_color;
    UmnMailExSlot slots[3];
    UmnMailExRow rows[2];
} UmnMailExWork;
struct UmnMailEdgePair {
    signed char edge[2];
};

extern const char mailPathRootText[];
extern const char mailPathFolderText[];
extern const char mailPathReadText[];
extern const char mailPathReplyText[];
extern const char mailPathHistoryText[];
extern const char mailTabStateDebugFormat[];
extern const char mailListCursorDebugFormat[];
extern const char mailDirectionDebugFormat[];
extern unsigned char mail_folder[0x80];
extern void eSpriteSet(void *sprite, short sprite_id);
extern void eSpriteMain(void *sprite);
extern const char mailOldPrefixText[];
extern const char mailNewPrefixText[];
extern const char mailFileAttachedText[];
extern const char mailNoAttachmentsText[];
extern const char mailFromText[];
extern const char mailReplyToText[];
extern const char mailViewMailHelpText[];
extern const char mailViewHistoryHelpText[];
extern const char mailDownloadAttachmentHelpText[];
extern const char mailCancelHelpText[];
extern const char mailSelectReplyPromptText[];
extern const char mailReplyConfirmText[];
extern const char mailReplySentText[];
extern const char mailAttachmentSavedText[];
extern const char mailInvestmentAmountText[];
extern const char mailAmountNeededText[];
extern const char mailMoneyUnitText[];
extern const char mailSentenceEndText[];
extern const char mailEmptyText[];
extern const char mailNewlineText[];
extern char *MenuNumberTextGet(int value, int width, int flags);
extern const struct UmnMailCaptionA mailInfoCaptionTexts;
extern const struct UmnMailCaptionB mailMenuHelpTexts;
extern const struct UmnMailCaptionC mailReplyCaptionTexts;
extern const char sikaku_1[];
extern const short mailSpriteIdTable[4];
extern const char mailScrollTypeDebugFormat[];
extern const char mailNowPageDebugFormat[];
extern int tyaUmlDispMain(void *param);
extern int tyaUmlDispParamReset(void *param, int value);
extern char text_2[];
extern const char mailSelectTagText[];
extern const char mailMenuTagText[];
extern const char mailYesNoDebugFormat[];
extern const char *text2_4[4];
extern const char *text_3_00A10650[2];
extern unsigned char *text_5[3];
extern const char mailNumberTagText[];

void UmnMailFolderSet(void);
int MenuSelectMove(int cursor, int count, int wrap);
int dataMoneyBoxChk(void);
void dataMoneyBoxDec(int money);

static const char *msg00_0_00A10618[UMN_MAIL_CAPTIONS] = {
    mailPathRootText,
    mailPathFolderText,
    mailPathReadText,
    mailPathReplyText,
    mailPathHistoryText,
};

void tskUmnMailPas(UmnObjectTask *task, void *data)
{
    UmnMailPasWork *work = data;
    int caption_widths[UMN_MAIL_CAPTIONS];
    int i;
    unsigned char mail_mode;

    for (i = 0; i < UMN_MAIL_CAPTIONS; i++) {
        caption_widths[i] = MenuPasLengthGet(msg00_0_00A10618[i]);
    }

    mail_mode = UmnWork.mode;
    if (mail_mode != UMN_MODE_MAIL) {
        task->state = UMN_TASK_END;
    } else {
        switch (task->state) {
        case UMN_TASK_INIT:
            work->color = 0x00fffff0;
            WindowDXSet(&work->window);
            work->window.x = -0x110;
            work->window.y = 8;
            work->window.style = work->color;
            work->window.width = 0x110;
            work->window.height = 32;
            work->window.state = mail_mode;
            WindowDXMain(&work->window);
            work->window.state = WINDOWDX_STATE_READY;

            for (i = 0; i < UMN_MAIL_CAPTIONS; i++) {
                eMessageSet(&work->messages[i], msg00_0_00A10618[i]);
                work->messages[i].mode = E_MODE_SHOW_START;
                work->messages[i].x = 0x120;
                work->messages[i].y = 0x0b;
                work->messages[i].draw_parameter = work->color + 2;
            }
            work->state = 1;
            break;

        case UMN_TASK_RUN: {
            short slide_targets[8];
            short window_target;

            window_target = -16;
            for (i = 0; i < UMN_MAIL_CAPTIONS; i++) {
                slide_targets[i] = 288;
            }

            /* The cases are steps relative to UMN_STEP_OPEN. */
            switch (UmnWork.tab_state - UMN_STEP_OPEN) {
            case UMN_MAIL_STEP_SWITCH_OPEN - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_SWITCH - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_FOLDER_OPEN - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_FOLDER - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_MENU_OPEN - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_MENU - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_READ_WAIT - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_ATTACH_OPEN - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_ATTACH_DONE - UMN_STEP_OPEN:
                slide_targets[0] = 16;
                slide_targets[UmnWork.mail_selection_state * 3 + 1] =
                    (short)(caption_widths[0] + 16);
                break;
            case UMN_MAIL_STEP_READ_OPEN - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_READ - UMN_STEP_OPEN:
                slide_targets[0] = 16;
                slide_targets[2] = (short)(caption_widths[0] + 16);
                break;
            case UMN_MAIL_STEP_REPLY_OPEN - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_REPLY_SELECT - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_REPLY_CONFIRM_OPEN - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_REPLY_CONFIRM - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_REPLY_SEND - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_REPLY_DONE - UMN_STEP_OPEN:
                slide_targets[0] = 16;
                slide_targets[3] = (short)(caption_widths[0] + 16);
                break;
            default:
                window_target = -272;
                break;
            }

            if (work->state != 0) {
                MoveSlide(&work->window.x, &window_target, 3.0f);
                WindowDXMain(&work->window);
                work->box.x = work->window.x + 3;
                work->box.y = work->window.y + 3;
                work->box.style = work->color;
                work->box.width = work->window.width - 6;
                work->box.height = work->window.height - 6;
                endPrintExtFunc(work->color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET, &work->box);

                for (i = 0; i < UMN_MAIL_CAPTIONS; i++) {
                    MoveSlide(&work->messages[i].x, &slide_targets[i], 3.0f);
                    if (work->messages[i].x < 256) {
                        eMessageMain(&work->messages[i]);
                    }
                }
                endPrintExtFunc(work->color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET, 0);
            }
            break;
        }
        }
    }
}

void tskUmnMailInfo(UmnObjectTask *task, void *argument)
{
    struct UmnMailInfoWork *work = argument;
    struct UmnMailCaptionA caption_a = mailInfoCaptionTexts;
    struct UmnMailCaptionB caption_b = mailMenuHelpTexts;
    struct UmnMailCaptionC caption_c = mailReplyCaptionTexts;
    short slide_target;
    unsigned char *box_data;
    UmnHistoryNode *history;
    UmnMailHeader *header;
    UmnMailHeader *box_header;
    int i;
    int k;
    int price_delta;
    int state;
    unsigned char mail_mode;
    unsigned char info_mode;

    mail_mode = UmnWork.mode;
    if (mail_mode != UMN_MODE_MAIL) {
        task->state = UMN_TASK_END;
        return;
    }
    state = task->state;
    switch (state) {
    case UMN_TASK_INIT:
        work->saved_mail_mode = 0;
        work->mode = 0;
        work->color = 0x00FF0000;
        memset(work->text_buffer, 0, UMN_MAIL_INFO_TEXT_SIZE);
        WindowDXSet(&work->window);
        work->window.x = -16;
        work->window.style = work->color;
        work->window.close_callback = MenuInfoWindow;
        work->window.state = 0;
        work->window.y = 480;
        work->window.width = 544;
        work->window.height = 54;
        work->window.close_callback_arg = &work->info;
        work->info.x = 0;
        work->window.state = mail_mode;
        work->info.y = 0;
        work->info.text = 0;
        WindowDXMain(&work->window);
        work->window.state = WINDOWDX_STATE_READY;
        break;
    case UMN_TASK_RUN:
        slide_target = 386;
        info_mode = work->mode;
        switch (info_mode) {
        case 0:
            work->saved_mail_mode = mail_mode;
            work->mode = state;
            /* fall through */
        case 2:
            /* The cases are steps relative to UMN_STEP_OPEN. */
            switch (UmnWork.tab_state - UMN_STEP_OPEN) {
            case UMN_MAIL_STEP_SWITCH_OPEN - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_SWITCH - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_FOLDER_OPEN - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_FOLDER - UMN_STEP_OPEN:
                box_data = UmnMailDataGet(UmnWork.entry);
                if ((box_data[0] & UMN_MAIL_FLAG_REPLY) != 0) {
                    for (i = 0; i < UMN_MAIL_HISTORY_ROOTS; i++) {
                        history = UmnHistoryTreeGet(i);
                        for (k = 0; k < UMN_MAIL_HISTORY_CHILDREN; k++) {
                            if (history->children[k] == UmnWork.entry) {
                                break;
                            }
                        }
                        if (k != UMN_MAIL_HISTORY_CHILDREN) {
                            break;
                        }
                    }
                    box_header = UmnMailHeaderGet(i);
                    if (box_header != 0) {
                        eMessageCpy(work->text_buffer, caption_a.text[4]);
                        eMessageCat(box_header->title);
                        eMessageCat(mailSentenceEndText);
                        eMessageCat(caption_a.text[5]);
                        eMessageCat(box_header->subject);
                        eMessageCat(mailSentenceEndText);
                    } else {
                        eMessageCpy(work->text_buffer, mailEmptyText);
                    }
                } else {
                    if ((box_data[0] & UMN_MAIL_FLAG_READ) != 0) {
                        eMessageCpy(work->text_buffer, caption_a.text[0]);
                    } else {
                        eMessageCpy(work->text_buffer, caption_a.text[1]);
                    }
                    if (UmnWork.mail_has_data != 0) {
                        eMessageCat(caption_a.text[2]);
                    } else {
                        eMessageCat(caption_a.text[3]);
                    }
                }
                break;
            case UMN_MAIL_STEP_MENU_OPEN - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_MENU - UMN_STEP_OPEN:
                eMessageCpy(work->text_buffer, caption_b.text[UmnWork.cursor]);
                break;
            case UMN_MAIL_STEP_REPLY_OPEN - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_REPLY_SELECT - UMN_STEP_OPEN:
                price_delta = UmnWork.price_balance;
                if (price_delta >= 0) {
                    header = UmnMailHeaderGet(UmnWork.entry);
                    eMessageCpy(work->text_buffer, caption_c.text[0]);
                    eMessageCat(header->subject);
                } else {
                    int cost;
                    int magnitude;

                    header = UmnMailHeaderGet(UmnWork.entry);
                    magnitude = -UmnWork.price_balance;
                    cost = header->cost;
                    eMessageCpy(work->text_buffer, caption_c.text[4]);
                    eMessageCat(MenuNumberTextGet(cost, 8, 1));
                    eMessageCat(caption_c.text[6]);
                    eMessageCat(mailNewlineText);
                    eMessageCat(caption_c.text[5]);
                    eMessageCat(MenuNumberTextGet(magnitude, 8, 1));
                    eMessageCat(caption_c.text[6]);
                }
                break;
            case UMN_MAIL_STEP_REPLY_CONFIRM_OPEN - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_REPLY_CONFIRM - UMN_STEP_OPEN:
                eMessageCpy(work->text_buffer, caption_c.text[1]);
                break;
            case UMN_MAIL_STEP_REPLY_SEND - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_REPLY_DONE - UMN_STEP_OPEN:
                eMessageCpy(work->text_buffer, caption_c.text[2]);
                break;
            case UMN_MAIL_STEP_ATTACH_OPEN - UMN_STEP_OPEN:
            case UMN_MAIL_STEP_ATTACH_DONE - UMN_STEP_OPEN:
                eMessageCpy(work->text_buffer, caption_c.text[3]);
                break;
            default:
                eMessageCpy(work->text_buffer, mailEmptyText);
                slide_target = 512;
                break;
            }
            break;
        }
        work->info.text = work->text_buffer;
        MoveSlide(&work->window.y, &slide_target, 3.0f);
        WindowDXMain(&work->window);
        break;
    }
}

void tskUmnMailFolder(UmnObjectTask *task, void *data)
{
    UmnMailFolderWork *work = data;
    short tx;
    short ty;
    int i;
    int j;
    int k;
    unsigned int cnt;
    unsigned int n;
    int sel;
    int id;
    int target;
    int tab;
    UmnMailHeader *hdr;
    int more;
    UmnMailFolderHead *head;

    if (UmnWork.mode != UMN_MODE_MAIL) {
        task->state = UMN_TASK_END;
        return;
    }
    switch (task->state) {
    case UMN_TASK_INIT:
        work->x = 528;
        work->y = UMN_MAIL_FOLDER_TOP;
        work->color = 0x00ff0000;
        for (i = 0; i < UMN_MAIL_FOLDER_RECORDS; i++) {
            work->records[i].x = 0;
            work->records[i].y = 0;
            WindowDXSet(&work->records[i].window);
            work->records[i].window.x = 528;
            work->records[i].window.y = 0;
            work->records[i].window.width = 440;
            work->records[i].window.height = 54;
            work->records[i].window.state = WINDOWDX_STATE_OPEN;
            WindowDXMain(&work->records[i].window);
            work->records[i].window.state = WINDOWDX_STATE_READY;
        }
        eCursolSet(&work->cursor, 0);
        work->cursor.work = work->color + 2;
        eCursolSet(&work->cursor2, 1);
        eCursolModeChange(&work->cursor2, E_MODE_SHOW_START);
        work->cursor2.work = work->color + 8;
        eCursolSet(&work->cursor3, 2);
        eCursolModeChange(&work->cursor3, E_MODE_SHOW_START);
        work->cursor3.work = work->color + 8;
        work->box_visible = 0;
        work->state = UMN_MAIL_FOLDER_IDLE;
        work->flag = 0;
        break;

    case UMN_TASK_RUN:
        work->count = UmnWork.count;
        if (work->count >= UMN_MAIL_FOLDER_VISIBLE + 1) {
            work->count = UMN_MAIL_FOLDER_VISIBLE;
        }
        switch (work->state) {
        case UMN_MAIL_FOLDER_IDLE:
            work->flag = 0;
            work->state = UMN_MAIL_FOLDER_WAIT;
            /* fall through */
        case UMN_MAIL_FOLDER_WAIT:
            if (UmnWork.tab_state == UMN_MAIL_STEP_FOLDER) {
                work->flag = 1;
                work->state = UMN_MAIL_FOLDER_SLIDE_IN;
                for (i = 0; i < work->count; i++) {
                    work->records[i].x = UMN_MAIL_FOLDER_HIDDEN_X;
                    work->records[i].y = UMN_MAIL_FOLDER_TOP + UMN_MAIL_FOLDER_PITCH * i;
                }
                eCursolModeChange(&work->cursor, E_MODE_SHOW_START);
            } else if (UmnWork.tab_state == UMN_MAIL_STEP_READ) {
                work->flag = 1;
                work->state = UMN_MAIL_FOLDER_FOCUS;
                work->records[0].x = UMN_MAIL_FOLDER_HIDDEN_X;
                work->records[0].y = 48;
                for (i = 1; i < work->count; i++) {
                    work->records[i].x = UMN_MAIL_FOLDER_HIDDEN_X;
                    work->records[i].y = UMN_MAIL_FOLDER_TOP + UMN_MAIL_FOLDER_PITCH * i;
                }
                eCursolModeChange(&work->cursor, E_MODE_SHOW_START);
            }
            break;

        case UMN_MAIL_FOLDER_SLIDE_IN:
            {
                int moved;
                short ty_arg;
                moved = 0;
                for (i = 0; i < work->count; i++) {
                    tx = 32;
                    MoveSlide(&work->records[i].x, &tx, 3.0f);
                    if (work->records[i].x == tx) {
                        if (i == UmnWork.row) {
                            ty_arg = UMN_MAIL_FOLDER_TOP + UMN_MAIL_FOLDER_PITCH * i;
                            MoveSlide(&work->records[i].y, &ty_arg, 3.0f);
                            if (work->records[i].y == ty_arg) {
                                moved++;
                            }
                        } else {
                            moved++;
                        }
                    }
                }
                if (moved != work->count) {
                    break;
                }
                work->state = UMN_MAIL_FOLDER_LIST;
                work->box_visible = 1;
                UmnWork.moving = 0;
                work->cursor_visible = 1;
            }
            break;

        case UMN_MAIL_FOLDER_LIST:
            tab = UmnWork.tab_state;
            {
                int waiting = 1;

                if (tab == UMN_MAIL_STEP_MENU) {
                    work->box_visible = 0;
                    work->state = UMN_MAIL_FOLDER_SLIDE_MENU;
                    work->cursor_visible = 0;
                } else if (tab == UMN_STEP_LEAVE_WAIT) {
                    work->box_visible = 0;
                    work->state = UMN_MAIL_FOLDER_CLEAR;
                    work->cursor_visible = 0;
                } else if (tab == UMN_MAIL_STEP_SWITCH) {
                    work->box_visible = 0;
                    work->state = UMN_MAIL_FOLDER_SLIDE_OUT;
                    work->cursor_visible = 0;
                } else if (UmnWork.direction != 0) {
                    work->state = UMN_MAIL_FOLDER_SCROLL_START;
                    UmnWork.moving = 1;
                    waiting = 0;
                }
                if (waiting) {
                    break;
                }
            }
            /* fall through */

        case UMN_MAIL_FOLDER_SCROLL_START:
            if (UmnWork.direction == 1) {
                work->records[UMN_MAIL_FOLDER_VISIBLE].x = work->records[0].x;
                work->records[UMN_MAIL_FOLDER_VISIBLE].y = work->records[0].y;
            } else {
                work->records[UMN_MAIL_FOLDER_VISIBLE].x = work->records[UMN_MAIL_FOLDER_VISIBLE - 1].x;
                work->records[UMN_MAIL_FOLDER_VISIBLE].y = work->records[UMN_MAIL_FOLDER_VISIBLE - 1].y;
            }
            for (i = 0; i < work->count; i++) {
                work->records[i].y += UmnWork.direction * UMN_MAIL_FOLDER_PITCH;
            }
            work->state = UMN_MAIL_FOLDER_SCROLL;
            /* fall through */

        case UMN_MAIL_FOLDER_SCROLL:
            {
                short ty;
                UmnWorkData *state;
                for (i = 0; i < work->count; i++) {
                    work->records[i].y -= UmnWork.direction * UMN_MAIL_FOLDER_SCROLL_SPEED;
                }
                state = &UmnWork;
                if (state->direction == 1) {
                    ty = work->records[UMN_MAIL_FOLDER_VISIBLE].y = work->records[0].y - UMN_MAIL_FOLDER_PITCH;
                    if (ty == 32) {
                        work->state = UMN_MAIL_FOLDER_LIST;
                        state->direction = 0;
                        state->moving = 0;
                    }
                } else {
                    ty = work->records[UMN_MAIL_FOLDER_VISIBLE].y = work->records[UMN_MAIL_FOLDER_VISIBLE - 1].y + UMN_MAIL_FOLDER_PITCH;
                    if (ty == 352) {
                        work->state = UMN_MAIL_FOLDER_LIST;
                        state->direction = 0;
                        state->moving = 0;
                    }
                }
            }
            break;

        case UMN_MAIL_FOLDER_SLIDE_MENU:
        case UMN_MAIL_FOLDER_SLIDE_OUT:
            {
                short tx;
                switch (work->state) {
                case UMN_MAIL_FOLDER_SLIDE_MENU:
                    tx = 144;
                    break;
                case UMN_MAIL_FOLDER_SLIDE_OUT:
                    tx = 528;
                    break;
                }
                for (i = 0; i < work->count; i++) {
                    MoveSlide(&work->records[i].x, &tx, 3.0f);
                }
                if (work->records[UmnWork.row].x == tx) {
                    switch (work->state) {
                    case UMN_MAIL_FOLDER_SLIDE_MENU:
                        work->state = UMN_MAIL_FOLDER_MENU;
                        break;
                    case UMN_MAIL_FOLDER_SLIDE_OUT:
                        work->state = UMN_MAIL_FOLDER_WAIT;
                        UmnWork.moving = 0;
                        break;
                    }
                }
            }
            break;

        case UMN_MAIL_FOLDER_MENU:
            switch (UmnWork.tab_state) {
            case UMN_MAIL_STEP_SWITCH:
                work->state = UMN_MAIL_FOLDER_SLIDE_OUT;
                break;
            case UMN_MAIL_STEP_READ:
                work->state = UMN_MAIL_FOLDER_FOCUS;
                break;
            case UMN_MAIL_STEP_ATTACH_DONE:
            case UMN_MAIL_STEP_FOLDER:
                work->state = UMN_MAIL_FOLDER_SLIDE_IN;
                break;
            default:
                UmnWork.moving = 0;
                break;
            }
            break;

        case UMN_MAIL_FOLDER_FOCUS:
            {
                cnt = 0;
            for (i = 0; i < work->count; i++) {
                if (i == UmnWork.row) {
                    short ty;
                    ty = 48;
                    MoveSlide(&work->records[i].y, &ty, 3.0f);
                    if (work->records[i].y == ty) {
                        short tx_top;
                        tx_top = 16;
                        MoveSlide(&work->records[i].x, &tx_top, 3.0f);
                        if (work->records[i].x == tx_top) {
                            cnt++;
                        }
                    }
                } else {
                    short tx_side;
                    tx_side = UMN_MAIL_FOLDER_HIDDEN_X;
                    MoveSlide(&work->records[i].x, &tx_side, 3.0f);
                    if (work->records[i].x == tx_side) {
                        cnt++;
                    }
                }
            }
            if (cnt == work->count) {
                work->state = UMN_MAIL_FOLDER_FOCUSED;
            }
            }
            break;

        case UMN_MAIL_FOLDER_FOCUSED:
            switch (UmnWork.tab_state) {
            case UMN_MAIL_STEP_FOLDER:
                work->state = UMN_MAIL_FOLDER_SLIDE_IN;
                break;
            case UMN_MAIL_STEP_REPLY_SELECT:
            case UMN_MAIL_STEP_EVENT:
                work->state = UMN_MAIL_FOLDER_HIDE_FOCUSED;
                break;
            default:
                UmnWork.moving = 0;
                break;
            }
            break;

        case UMN_MAIL_FOLDER_HIDE_FOCUSED:
            {
                short tx;
                tx = UMN_MAIL_FOLDER_HIDDEN_X;
                MoveSlide(&work->records[UmnWork.row].x, &tx, 3.0f);
                if (work->records[UmnWork.row].x == tx) {
                    work->state = UMN_MAIL_FOLDER_HIDDEN;
                }
            }
            break;

        case UMN_MAIL_FOLDER_HIDDEN:
            switch (UmnWork.tab_state) {
            case UMN_MAIL_STEP_READ:
                work->state = UMN_MAIL_FOLDER_FOCUS;
                break;
            case UMN_MAIL_STEP_FOLDER:
                work->state = UMN_MAIL_FOLDER_SLIDE_IN;
                for (i = 0; i < UMN_MAIL_FOLDER_VISIBLE; i++) {
                    work->records[i].x = UMN_MAIL_FOLDER_HIDDEN_X;
                    work->records[i].y = UMN_MAIL_FOLDER_TOP + UMN_MAIL_FOLDER_PITCH * i;
                }
                break;
            default:
                break;
            }
            break;

        case UMN_MAIL_FOLDER_CLEAR:
            {
                short tx;
                unsigned int moved2;
                moved2 = 0;
                for (i = 0; i < work->count; i++) {
                    tx = UMN_MAIL_FOLDER_HIDDEN_X;
                    MoveSlide(&work->records[i].x, &tx, 3.0f);
                    if (work->records[i].x == tx) {
                        moved2++;
                    }
                }
                if (moved2 == work->count) {
                    work->state = UMN_MAIL_FOLDER_IDLE;
                    UmnWork.moving = 0;
                }
            }
            break;

        default:
            break;
        }

        if (work->flag != 0) {
            more = 0;
            if (UmnWork.direction != 0) {
                more = 1;
            }
            for (i = 0; i < work->count + more; i++) {
                k = UmnWork.list_cursor - UmnWork.row + i;
                if (i == UMN_MAIL_FOLDER_VISIBLE && UmnWork.direction == 1) {
                    k = UmnWork.list_cursor - UmnWork.row - 1;
                }
                if (k < 0) {
                    k += UmnWork.count;
                } else {
                    k = k % UmnWork.count;
                }
                id = mail_folder[k];
                head = (UmnMailFolderHead *)UmnMailDataGet(id);
                work->records[i].head = *head;
                work->records[i].folder_id = id;
            }

            if (work->box_visible) {
                work->box.x = 32;
                work->box.y = 96;
                work->box.width = work->records[0].window.width + 16;
                work->box.height = work->records[0].window.height + 192;
                work->box.style = 96;
                endPrintExtFunc(work->color, END_PRINT_CMD_SCISSOR_SET, &work->box);
            }

            for (i = 0; i < work->count + more; i++) {
                work->records[i].window.x = work->records[i].x;
                if (work->records[i].head.flags & UMN_MAIL_FLAG_REPLY) {
                    work->records[i].window.x += 16;
                }
                work->records[i].window.y = work->records[i].y;
                if (i == UmnWork.list_cursor) {
                    work->records[i].window.style = work->color;
                } else {
                    work->records[i].window.style = work->color - 16;
                }
                WindowDXMain(&work->records[i].window);
                xglFontDebugPrintf(work->records[i].x / 2, work->records[i].y / 2, "MailId = %2d", work->records[i].folder_id);
            }

            sel = UmnWork.row;
            work->cursor.work = work->records[sel].window.style;
            work->cursor.x = work->records[sel].window.x + 3;
            work->cursor.y = work->records[sel].window.y + 19;
            eCursolMain(&work->cursor);

            if (UmnWork.count >= UMN_MAIL_FOLDER_VISIBLE + 1 && work->cursor_visible) {
                work->cursor2.x = 474;
                work->cursor2.y = 96;
                work->cursor2.work = work->color;
                eCursolMain(&work->cursor2);
                work->cursor3.x = 474;
                work->cursor3.y = 326;
                work->cursor3.work = work->color;
                eCursolMain(&work->cursor3);
            }

            if (work->box_visible) {
                endPrintExtFunc(work->color, END_PRINT_CMD_SCISSOR_RESET, 0);
            }
            work->tex = UmnTexAddr;
            endPrintExtFunc(0, END_PRINT_CMD_FONT_TEX_CHANGE, &work->tex);

            if (work->box_visible) {
                endPrintExtFunc(work->color, END_PRINT_CMD_SCISSOR_SET, &work->box);
                endPrintExtFunc(work->color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET, &work->box);
            }

            for (i = 0; i < work->count + more; i++) {
                int pick;

                hdr = UmnMailHeaderGet(work->records[i].folder_id);
                pick = UMN_MAIL_SPRITE_UNREAD;
                if (work->records[i].head.flags & UMN_MAIL_FLAG_REPLY) {
                    pick = UMN_MAIL_SPRITE_REPLY;
                } else if (work->records[i].head.flags & UMN_MAIL_FLAG_READ) {
                    pick = UMN_MAIL_SPRITE_READ;
                }
                eSpriteSet(&work->records[i].sprites[0], pick);
                work->records[i].sprites[0].x = work->records[i].window.x + 19;
                work->records[i].sprites[0].y = work->records[i].window.y + 3;
                work->records[i].sprites[0].color = work->records[i].window.style + 1;
                eSpriteMain(&work->records[i].sprites[0]);

                {
                    short sprite_ids[2] = {5125, 5124};
                    int sj;

                    for (sj = 0; sj < 2; sj++) {
                        eSpriteSet(&work->records[i].sprites[sj + 1], sprite_ids[sj]);
                        work->records[i].sprites[sj + 1].x = work->records[i].sprites[0].x + 38;
                        work->records[i].sprites[sj + 1].y = work->records[i].sprites[0].y + 24 * sj;
                        work->records[i].sprites[sj + 1].color = work->records[i].sprites[0].color;
                        eSpriteMain(&work->records[i].sprites[sj + 1]);
                    }
                }

                eMessageSet(&work->records[i].messages[0], hdr->subject);
                eMessageSet(&work->records[i].messages[1], hdr->title);
                for (j = 0; j < 2; j++) {
                    eMessageModeChange(&work->records[i].messages[j], E_MODE_SHOW_START);
                    work->records[i].messages[j].x = work->records[i].sprites[j + 1].x + 20;
                    work->records[i].messages[j].y = work->records[i].sprites[j + 1].y;
                    work->records[i].messages[j].draw_parameter = work->records[i].sprites[j + 1].color + 2;
                    eMessageMain(&work->records[i].messages[j]);
                }

                if (hdr->attach != 0) {
                    eSpriteSet(&work->records[i].sprites[3], UMN_MAIL_SPRITE_ATTACH);
                    if (work->records[i].head.tex_flags & UMN_MAIL_ATTACH_TAKEN) {
                        work->records[i].sprites[3].rgb[0] = work->records[i].sprites[3].rgb[1] = work->records[i].sprites[3].rgb[2] = UMN_MAIL_TEXT_DIM;
                    }
                } else {
                    eSpriteSet(&work->records[i].sprites[3], UMN_MAIL_SPRITE_NO_ATTACH);
                }
                work->records[i].sprites[3].x = work->records[i].messages[0].x + 326;
                work->records[i].sprites[3].y = work->records[i].messages[0].y + 14;
                work->records[i].sprites[3].color = work->records[i].messages[0].draw_parameter;
                eSpriteMain(&work->records[i].sprites[3]);
            }

            if (work->box_visible) {
                endPrintExtFunc(work->color, END_PRINT_CMD_SCISSOR_RESET, 0);
                endPrintExtFunc(work->color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET, 0);
            }
            endPrintExtFunc(0, END_PRINT_CMD_FONT_TEX_RELOAD, 0);
        }
        break;

    default:
        break;
    }

    xglFontDebugPrintf(0, 16, "Folder : %2d", work->state);
}

void tskUmnMailDisp(UmnObjectTask *task, void *data)
{
    UmnMailDispWork *work = data;
    UmnMailSpriteIds sprite_ids;
    short goal30_x;
    short goal30_y;
    short goal50_x;
    short goal50_y;
    short goal70_x;
    short goal70_y;
    short cur;
    short target;
    short goal_b;
    int quotient;
    int remainder;
    int pages;
    signed char selected_id;
    int i;
    int c;
    signed char kind;
    unsigned char mail_mode;

    mail_mode = UmnWork.mode;
    if (mail_mode != UMN_MODE_MAIL) {
        task->state = UMN_TASK_END;
        return;
    }

    switch (task->state) {
    case UMN_TASK_INIT:
        work->pos_y = 112;
        work->pos_kind = 0x00f00000;
        work->pos_x = 528;
        WindowDXSet(&work->window);
        work->window.x = 528;
        work->window.width = 480;
        work->window.height = 300;
        work->window.state = mail_mode;
        work->window.y = 0;
        WindowDXMain(&work->window);
        work->window.state = WINDOWDX_STATE_READY;
        for (c = 0; c < 2; c++) {
            eCursolSet(&work->cursor_slots[c], c + 1);
            work->cursor_slots[c].state = E_MODE_SHOW_START;
            work->cursor_slots[c].work = 0x00f00010;
        }
        tyaUmlDispParamReset(&work->tya_value, 0);
        work->tya_pos_x = work->pos_x + 1798;
        work->tya_pos_y = work->pos_y + 1830;
        work->scroll_x = 48;
        work->cursor_x = 474;
        work->cursor_pitch = 288;
        work->tya_kind = (unsigned int)(work->pos_kind + 2);
        sprite_ids = *(const UmnMailSpriteIds *)mailSpriteIdTable;
        for (i = 0; i < 4; i++) {
            eSpriteSet(&work->sprites[i], sprite_ids.id[i]);
            work->sprites[i].color = 0x00f00000;
            work->item_anim[i] = 0;
        }
        eMessageSet(&work->message, sikaku_1);
        work->message.mode = E_MODE_SHOW_START;
        work->message.draw_parameter = 0x00f00000;
        work->state = 0;
        work->step = UMN_MAIL_DISP_IDLE;
        break;
    case UMN_TASK_RUN:
        switch (work->step) {
        case UMN_MAIL_DISP_IDLE:
            work->state = 0;
            work->step = UMN_MAIL_DISP_WAIT;
            /* fallthrough */
        case UMN_MAIL_DISP_WAIT:
            if (UmnWork.tab_state == UMN_MAIL_STEP_READ_WAIT) {
                work->step = UMN_MAIL_DISP_WAIT_MODEL;
            }
            break;
        case UMN_MAIL_DISP_WAIT_MODEL:
            if (UmnModelUkn != 0 && UmnModelUkn->active != 0) {
                work->pos_x = 256;
                work->pos_y = 259;
                work->slide_x = 6;
                work->slide_y = 6;
                work->window.width = 6;
                work->window.height = 6;
                tyaUmlDispParamReset(&work->tya_value, 0);
                selected_id = UmnWork.entry;
                work->tya_value = selected_id;
                work->scroll_x = 48;
                work->step = UMN_MAIL_DISP_OPEN;
                work->cursor_x = 6;
                work->cursor_pitch = 6;
                work->now_page = 0;
                work->column_index = 0;
                work->scrol_type = 0;
                work->flag_06 = 0;
                work->scroll_target = 0;
                work->flag_07 = 0;
            }
            break;
        case UMN_MAIL_DISP_OPEN:
            work->state = 1;
            goal30_x = 240;
            goal30_y = 147;
            MoveSlide(&work->slide_x, &goal30_x, 3.0f);
            MoveSlide(&work->slide_y, &goal30_y, 3.0f);
            work->pos_x = (unsigned short)goal30_x - work->slide_x + 16;
            work->pos_y = (unsigned short)goal30_y - work->slide_y + 112;
            work->window.width = work->slide_x * 2;
            work->window.height = work->slide_y * 2;
            work->slide_dx = (unsigned short)goal30_x - work->slide_x;
            work->cursor_x = work->window.width - 6;
            work->cursor_pitch = work->window.height - 6;
            if (work->slide_x == goal30_x && work->slide_y == goal30_y) {
                work->flag_06 = 1;
                work->step = UMN_MAIL_DISP_SHOWN;
                work->flag_07 = 1;
            }
            break;
        case UMN_MAIL_DISP_SHOWN:
            switch (UmnWork.tab_state) {
            case UMN_MAIL_STEP_READ:
                if (work->scrol_type == UMN_MAIL_SCROLL_NONE) {
                    quotient = ((short)work->scroll_end - 48) / (short)work->cursor_pitch;
                    if (quotient != 0) {
                        if (PadData.half_30 & PAD_UP) {
                            work->scrol_type = UMN_MAIL_SCROLL_LINE_UP;
                        }
                        if (PadData.half_30 & PAD_DOWN) {
                            work->scrol_type = UMN_MAIL_SCROLL_LINE_DOWN;
                        }
                        if (PadData.half_30 & PAD_L1) {
                            if (work->now_page > 0) {
                                work->scrol_type = UMN_MAIL_SCROLL_PAGE_UP;
                                work->now_page = work->now_page - 1;
                                work->item_anim[1] = -6;
                            }
                        }
                        if (PadData.half_30 & PAD_R1) {
                            remainder = ((short)work->scroll_end - 48) % (short)work->cursor_pitch;
                            pages = quotient;
                            if (remainder != 0) {
                                pages = quotient + 1;
                            }
                            if (work->now_page < pages - 1) {
                                work->scrol_type = UMN_MAIL_SCROLL_PAGE_DOWN;
                                work->now_page = work->now_page + 1;
                                work->item_anim[2] = 6;
                            }
                        }
                        if (PadData.half_30 & PAD_L2) {
                            if (work->scroll_x != 48) {
                                work->scrol_type = UMN_MAIL_SCROLL_TOP;
                                work->item_anim[0] = -6;
                            }
                        }
                        if (PadData.half_30 & PAD_R2) {
                            if (work->scroll_x != (short)work->scroll_end - (short)work->cursor_pitch) {
                                work->scrol_type = UMN_MAIL_SCROLL_BOTTOM;
                                work->item_anim[3] = 6;
                            }
                        }
                        if (PadData.half_30 & PAD_SQUARE) {
                            if (work->item_count != 0) {
                                work->column_index = work->column_index + 1;
                                if ((int)work->item_count - 1 < (signed char)work->column_index - 1) {
                                    work->column_index = 1;
                                }
                                work->scrol_type = UMN_MAIL_SCROLL_NEXT_COLUMN;
                            }
                        }
                    }
                }
                kind = work->scrol_type;
                switch (kind) {
                case UMN_MAIL_SCROLL_LINE_UP:
                    work->scroll_x = work->scroll_x - 8;
                    if ((short)work->scroll_x < 48) {
                        work->scroll_x = 48;
                    }
                    quotient = ((short)work->scroll_x - 48) / (short)work->cursor_pitch;
                    work->scrol_type = UMN_MAIL_SCROLL_NONE;
                    work->now_page = quotient;
                    break;
                case UMN_MAIL_SCROLL_LINE_DOWN:
                    work->scroll_x = work->scroll_x + 8;
                    if ((short)work->scroll_x > (short)work->scroll_end - (short)work->cursor_pitch) {
                        work->scroll_x = work->scroll_end - work->cursor_pitch;
                    }
                    quotient = ((short)work->scroll_x - 48) / (short)work->cursor_pitch;
                    work->scrol_type = UMN_MAIL_SCROLL_NONE;
                    work->now_page = quotient;
                    break;
                case UMN_MAIL_SCROLL_PAGE_UP:
                case UMN_MAIL_SCROLL_PAGE_DOWN:
                case UMN_MAIL_SCROLL_TOP:
                case UMN_MAIL_SCROLL_BOTTOM:
                case UMN_MAIL_SCROLL_NEXT_COLUMN:
                    cur = work->scroll_x;
                    switch (work->scrol_type) {
                    case UMN_MAIL_SCROLL_PAGE_UP:
                    case UMN_MAIL_SCROLL_PAGE_DOWN:
                        target = work->now_page * work->cursor_pitch + 48;
                        if ((short)work->scroll_end - (short)work->cursor_pitch < target) {
                            target = work->scroll_end - work->cursor_pitch;
                        }
                        break;
                    case UMN_MAIL_SCROLL_TOP:
                        target = 48;
                        break;
                    case UMN_MAIL_SCROLL_BOTTOM:
                        target = work->scroll_end - work->cursor_pitch;
                        break;
                    case UMN_MAIL_SCROLL_NEXT_COLUMN:
                        if (work->column_index != 0) {
                            target = work->column_x[(signed char)work->column_index];
                        }
                        break;
                    default:
                        break;
                    }
                    MoveSlide(&cur, &target, 3.0f);
                    work->scroll_x = cur;
                    if (cur == target) {
                        if (work->scrol_type != UMN_MAIL_SCROLL_PAGE_DOWN) {
                            quotient = ((int)cur - 48) / (short)work->cursor_pitch;
                            work->now_page = quotient;
                        }
                        work->scrol_type = UMN_MAIL_SCROLL_NONE;
                    }
                    break;
                default:
                    break;
                }
                break;
            case UMN_MAIL_STEP_FOLDER:
                work->step = UMN_MAIL_DISP_CLOSE;
                break;
            case UMN_MAIL_STEP_REPLY_SELECT:
                work->state = 0;
                work->step = UMN_MAIL_DISP_NAME_PREPARE;
                break;
            case UMN_MAIL_STEP_EVENT:
                work->step = UMN_MAIL_DISP_CLOSE;
                break;
            default:
                UmnWork.moving = 0;
                break;
            }
            break;
        case UMN_MAIL_DISP_CLOSE:
            work->flag_06 = 0;
            work->flag_07 = 0;
            goal50_x = 3;
            goal50_y = 3;
            MoveSlide(&work->slide_x, &goal50_x, 3.0f);
            MoveSlide(&work->slide_y, &goal50_y, 3.0f);
            work->pos_x = 256 - (unsigned short)work->slide_x;
            work->pos_y = 259 - (unsigned short)work->slide_y;
            work->window.width = work->slide_x * 2;
            work->window.height = work->slide_y * 2;
            work->cursor_x = work->window.width - 6;
            work->cursor_pitch = work->window.height - 6;
            if (work->slide_x == goal50_x && work->slide_y == goal50_y) {
                work->step = UMN_MAIL_DISP_IDLE;
                work->state = 0;
            }
            break;
        case UMN_MAIL_DISP_NAME_PREPARE:
            work->slide_x = 6;
            work->slide_y = 6;
            work->window.width = 6;
            work->window.height = 6;
            tyaUmlDispParamReset(&work->tya_value, 0);
            work->tya_value = UmnWork.name_lookup;
            work->scroll_x = 48;
            work->cursor_x = 6;
            work->cursor_pitch = 6;
            work->shown_entry = UmnWork.name_lookup;
            work->step = UMN_MAIL_DISP_NAME_OPEN;
            work->flag_06 = 0;
            work->flag_07 = 0;
            break;
        case UMN_MAIL_DISP_NAME_OPEN:
            work->state = 1;
            goal70_x = 240;
            goal70_y = 123;
            MoveSlide(&work->slide_x, &goal70_x, 3.0f);
            MoveSlide(&work->slide_y, &goal70_y, 3.0f);
            work->pos_x = (unsigned short)goal70_x - work->slide_x + 16;
            work->pos_y = (unsigned short)goal70_y - work->slide_y + 128;
            work->window.width = work->slide_x * 2;
            work->window.height = work->slide_y * 2;
            work->slide_dx = (unsigned short)goal70_x - work->slide_x;
            work->cursor_x = work->window.width - 6;
            work->cursor_pitch = work->window.height - 6;
            if (work->slide_x == goal70_x && work->slide_y == goal70_y) {
                work->step = UMN_MAIL_DISP_NAME_SHOWN;
            }
            if (work->shown_entry != UmnWork.name_lookup) {
                work->state = 0;
                work->step = UMN_MAIL_DISP_NAME_PREPARE;
            }
            break;
        case UMN_MAIL_DISP_NAME_SHOWN:
            switch (UmnWork.tab_state) {
            case UMN_MAIL_STEP_FOLDER:
                work->step = UMN_MAIL_DISP_CLOSE;
                break;
            case UMN_MAIL_STEP_READ:
                work->state = 0;
                work->step = UMN_MAIL_DISP_WAIT_MODEL;
                break;
            default:
                if (work->shown_entry != UmnWork.name_lookup) {
                    work->state = 0;
                    work->step = UMN_MAIL_DISP_NAME_PREPARE;
                }
                break;
            }
            break;
        default:
            break;
        }

        if (work->state != 0) {
            work->window.x = work->pos_x;
            work->window.y = work->pos_y;
            work->window.style = work->pos_kind;
            WindowDXMain(&work->window);
            work->tya_pos_x = work->pos_x + 1795;
            work->tya_pos_y = work->pos_y + 1827;
            work->tya_kind = (unsigned int)(work->pos_kind + 2);
            tyaUmlDispMain(&work->tya_value);
            if (work->flag_06 != 0) {
                quotient = ((short)work->scroll_end - 48) / (short)work->cursor_pitch;
                if (quotient != 0) {
                    if (work->scroll_x != 48) {
                        work->cursor_slots[0].x = work->pos_x + work->window.width - 17;
                        work->cursor_slots[0].y = work->pos_y + 4;
                        eCursolMain(&work->cursor_slots[0]);
                    }
                    if (work->scroll_x < (short)work->scroll_end - (short)work->cursor_pitch) {
                        work->cursor_slots[1].x = work->pos_x + work->window.width - 17;
                        work->cursor_slots[1].y = work->pos_y + work->window.height - 19;
                        eCursolMain(&work->cursor_slots[1]);
                    }
                }
            }
            goal_b = 0;
            if (work->flag_07 != 0) {
                goal_b = 70;
            }
            MoveSlide(&work->scroll_target, &goal_b, 3.0f);
            work->message.x = 64;
            work->message.y = 480 - work->scroll_target;
            eMessageMain(&work->message);
            work->tex_addr = UmnTexAddr;
            endPrintExtFunc(0, END_PRINT_CMD_FONT_TEX_CHANGE, &work->tex_addr);
            for (i = 0; i < 4; i++) {
                if (work->item_anim[i] != 0) {
                    if (i < 2) {
                        work->item_anim[i] = work->item_anim[i] + 1;
                    } else {
                        work->item_anim[i] = work->item_anim[i] - 1;
                    }
                }
                work->sprites[i].x = 256 + 32 * i;
                work->sprites[i].y = work->item_anim[i] - work->scroll_target + 480;
                eSpriteMain(&work->sprites[i]);
            }
            endPrintExtFunc(0, END_PRINT_CMD_FONT_TEX_RELOAD, 0);
        }
        break;
    default:
        break;
    }

    xglFontDebugPrintf(16, 128, mailScrollTypeDebugFormat, work->scrol_type);
    xglFontDebugPrintf(16, 136, mailNowPageDebugFormat, work->now_page);
}

void tskUmnMailHensin(UmnObjectTask *task, void *data)
{
    UmnMailHensinWork *work = data;
    short goal[2];
    unsigned char mail_mode;
    int i;
    UmnMailHeader *header;

    mail_mode = UmnWork.mode;
    if (mail_mode != UMN_MODE_MAIL) {
        task->state = UMN_TASK_END;
        return;
    }
    switch (task->state) {
    case UMN_TASK_INIT:
        work->anchor_x = 528;
        work->anchor_y = 48;
        work->anchor_color = 0x00ff0000;
        WindowDXSet(&work->window);
        work->window.x = work->anchor_x;
        work->window.y = work->anchor_y;
        work->window.width = 302;
        work->window.height = 78;
        work->window.state = mail_mode;
        WindowDXMain(&work->window);
        work->window.state = WINDOWDX_STATE_READY;
        eCursolSet(&work->cursor, 0);
        work->state = 0;
        work->step = UMN_MAIL_HENSIN_IDLE;
        break;
    case UMN_TASK_RUN:
        switch (work->step) {
        case UMN_MAIL_HENSIN_IDLE:
            work->state = 0;
            work->step = UMN_MAIL_HENSIN_WAIT;
            eCursolModeChange(&work->cursor, E_MODE_HIDE_START);
        case UMN_MAIL_HENSIN_WAIT:
            if (UmnWork.tab_state == UMN_MAIL_STEP_REPLY_SELECT) {
                work->step = UMN_MAIL_HENSIN_SLIDE_IN;
                work->state = 1;
                eCursolModeChange(&work->cursor, E_MODE_SHOW_START);
            }
            break;
        case UMN_MAIL_HENSIN_SLIDE_IN:
            goal[0] = 16;
            MoveSlide(&work->anchor_x, &goal[0], 3.0f);
            if (work->anchor_x == goal[0]) {
                work->step = UMN_MAIL_HENSIN_SHOWN;
            }
            break;
        case UMN_MAIL_HENSIN_SHOWN:
            if ((UmnWork.tab_state >> 4) != (UMN_MAIL_STEP_REPLY_OPEN >> 4)) {
                work->step = UMN_MAIL_HENSIN_SLIDE_OUT;
            } else {
                UmnWork.moving = 0;
            }
            break;
        case UMN_MAIL_HENSIN_SLIDE_OUT:
            goal[1] = 528;
            MoveSlide(&work->anchor_x, &goal[1], 3.0f);
            if (work->anchor_x == goal[1]) {
                work->step = UMN_MAIL_HENSIN_IDLE;
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    if (work->state == 0) {
        return;
    }
    work->window.x = work->anchor_x;
    work->window.y = work->anchor_y;
    work->window.style = work->anchor_color;
    WindowDXMain(&work->window);
    eMessageSet(&work->message, text_2);
    work->message.x = work->window.x + 19;
    work->message.y = work->window.y + 3;
    work->message.draw_parameter = work->window.style + 2;
    eMessageModeChange(&work->message, E_MODE_SHOW_START);
    eMessageMain(&work->message);
    for (i = 0; i < UmnWork.name_char_count; i++) {
        header = UmnMailHeaderGet(UmnWork.name[i]);
        eMessageSet(&work->messages[i], header->subject);
        work->messages[i].x = work->window.x + 59;
        work->messages[i].draw_parameter = work->window.style + 2;
        work->messages[i].y = work->window.y + i * 24 + 3;
        eMessageModeChange(&work->messages[i], E_MODE_SHOW_START);
        eMessageMain(&work->messages[i]);
    }
    work->cursor.x = work->window.x + 3;
    work->cursor.y = work->messages[UmnWork.mail_cursor].y + 4;
    work->cursor.work = work->window.style + 2;
    eCursolMain(&work->cursor);
}

void tskUmnMailMenu(UmnObjectTask *task, void *data)
{
    UmnMailMenuWork *work = data;
    int i;

    if (UmnWork.mode != UMN_MODE_MAIL) {
        task->state = UMN_TASK_END;
        return;
    }

    switch (task->state) {
    case UMN_TASK_INIT:
        work->origin_x = 528;
        work->origin_y = 48;
        work->color = 0x00fffff0;
        work->window1_visible = 0;
        WindowDXSet(&work->window1);
        work->window1.style = work->color;
        work->window1.tag_id = mailSelectTagText;
        work->window1.width = 169;
        work->window1.height = 86;
        work->select1.text = text_3_00A10650[1];
        work->window1.close_callback_arg = &work->select1;
        work->select1.rows = 3;
        work->select1.heading = text_3_00A10650[0];
        work->window1.close_callback = MenuSelectWindow;
        work->window2_visible = 0;
        WindowDXSet(&work->window2);
        work->window2.style = work->color;
        work->window2.width = 125;
        work->window2.height = 102;
        work->select2.rows = 2;
        work->window2.tag_id = mailMenuTagText;
        work->window2.close_callback = MenuSelectWindow;
        work->window2.close_callback_arg = &work->select2;
        work->state = UMN_MAIL_MENU_IDLE;
        work->flag = 0;
        break;

    case UMN_TASK_RUN:
        switch (work->state) {
        case UMN_MAIL_MENU_IDLE:
            work->flag = 1;
            work->state = UMN_MAIL_MENU_WAIT;
            /* fall through */
        case UMN_MAIL_MENU_WAIT:
            switch (UmnWork.tab_state) {
            case UMN_MAIL_STEP_MENU:
                for (i = 0; i < UMN_MAIL_MENU_ENTRIES; i++) {
                    work->entries[i].name = text2_4[i];
                    work->entries[i].locked = 0;
                }
                if (UmnWork.mail_selection_state != 0) {
                    work->entries[UMN_MAIL_MENU_HISTORY].locked = 1;
                }
                if (UmnWork.mail_has_data != 0 && UmnWork.mail_text_ready != 0) {
                    work->entries[UMN_MAIL_MENU_ATTACH].locked = 0;
                } else {
                    work->entries[UMN_MAIL_MENU_ATTACH].locked = 1;
                }
                work->window2.x = -125;
                work->window2.y = 176;
                work->select2.commands = work->entries;
                work->window2_visible = 1;
                work->window2.state = WINDOWDX_STATE_OPEN;
                work->select2.cursor = 0;
                WindowDXMain(&work->window2);
                work->window2.state = WINDOWDX_STATE_READY;
                work->state = UMN_MAIL_MENU_SLIDE_IN;
                break;
            case UMN_MAIL_STEP_REPLY_CONFIRM:
                work->window1.x = 528;
                work->window1.y = 40;
                work->window1_visible = 1;
                work->window1.state = WINDOWDX_STATE_OPEN;
                work->select1.cursor = 0;
                WindowDXMain(&work->window1);
                work->window1.state = WINDOWDX_STATE_READY;
                work->state = UMN_MAIL_CONFIRM_SLIDE_IN;
            }
            break;
        case UMN_MAIL_MENU_SLIDE_IN: {
            short target = 16;
            MoveSlide(&work->window2.x, &target, 3.0f);
            if (work->window2.x == target) {
                work->state = UMN_MAIL_MENU_SHOWN;
            }
            break;
        }
        case UMN_MAIL_MENU_SHOWN:
            work->select2.cursor = UmnWork.cursor;
            if (UmnWork.tab_state != UMN_MAIL_STEP_MENU) {
                work->state = UMN_MAIL_MENU_SLIDE_OUT;
            } else {
                UmnWork.moving = 0;
            }
            break;
        case UMN_MAIL_MENU_SLIDE_OUT: {
            short target = -125;
            MoveSlide(&work->window2.x, &target, 3.0f);
            if (work->window2.x == target) {
                work->state = UMN_MAIL_MENU_IDLE;
                work->window2_visible = 0;
            }
            break;
        }
        case UMN_MAIL_CONFIRM_SLIDE_IN: {
            short target = 496 - work->window1.width;
            MoveSlide(&work->window1.x, &target, 3.0f);
            if (work->window1.x == target) {
                work->state = UMN_MAIL_CONFIRM_SHOWN;
            }
            break;
        }
        case UMN_MAIL_CONFIRM_SHOWN:
            work->select1.cursor = UmnWork.confirm_cursor;
            if (UmnWork.tab_state != UMN_MAIL_STEP_REPLY_CONFIRM) {
                work->state = UMN_MAIL_CONFIRM_SLIDE_OUT;
            } else {
                UmnWork.moving = 0;
            }
            break;
        case UMN_MAIL_CONFIRM_SLIDE_OUT: {
            short target = 528;
            MoveSlide(&work->window1.x, &target, 3.0f);
            if (work->window1.x == target) {
                work->state = UMN_MAIL_MENU_IDLE;
                work->window1_visible = 0;
            }
            break;
        }
        }
        break;
    }

    if (work->flag) {
        if (work->window1_visible) {
            WindowDXMain(&work->window1);
        }
        if (work->window2_visible) {
            WindowDXMain(&work->window2);
        }
    }
    xglFontDebugPrintf(0, 40, mailYesNoDebugFormat, work->state);
}

void tskUmnMailExWin(UmnObjectTask *task, void *data)
{
    UmnMailExWork *work = data;
    int i;

    if (UmnWork.mode != UMN_MODE_MAIL) {
        task->state = UMN_TASK_END;
        return;
    }
    switch (task->state) {
    case UMN_TASK_INIT:
        work->window_x = 528;
        work->window_y = 32;
        work->window_color = 0x00fffff0;
        for (i = 0; i < 3; i++) {
            work->slots[i].slide_x = 528;
            work->slots[i].slide_y = 16;
            eMessageSet(&work->slots[i].message, text_5[i]);
            eMessageModeChange(&work->slots[i].message, E_MODE_SHOW_START);
        }
        for (i = 0; i < 2; i++) {
            work->rows[i].slide_x = -272;
            work->rows[i].slide_y = 48;
            eRibbonSet(&work->rows[i].ribbon, 3);
            work->rows[i].ribbon.width = 256;
            work->rows[i].ribbon.height = 24;
            eTagFontSet(&work->rows[i].tag, mailNumberTagText);
            eNumberSet(&work->rows[i].number, 0);
        }
        work->unmodeled_00 = 0;
        work->state = 0;
        break;

    case UMN_TASK_RUN: {
        short slots[3];
        short targets[3];

        for (i = 0; i < 3; i++) {
            slots[i] = 528;
            targets[i] = -272;
        }
        switch (UmnWork.tab_state) {
        case UMN_MAIL_STEP_FOLDER_OPEN:
        case UMN_MAIL_STEP_FOLDER:
        case UMN_MAIL_STEP_MENU_OPEN:
        case UMN_MAIL_STEP_MENU:
        case UMN_MAIL_STEP_ATTACH_OPEN:
        case UMN_MAIL_STEP_ATTACH_DONE:
            targets[0] = 0;
            break;
        case UMN_MAIL_STEP_READ_OPEN:
        case UMN_MAIL_STEP_READ: {
            unsigned char *text = text_5[1];
            unsigned char *shade_bars = text + 1;
            unsigned char *cell = text + 15;

            if (UmnWork.header_gate_flag) {
                shade_bars[2] = -128;
                shade_bars[1] = -128;
                shade_bars[0] = -128;
            } else {
                shade_bars[2] = UMN_MAIL_TEXT_DIM;
                shade_bars[1] = UMN_MAIL_TEXT_DIM;
                shade_bars[0] = UMN_MAIL_TEXT_DIM;
            }
            if (UmnWork.entry == UMN_MAIL_ID_REPLY_REQUIRED
                && !(UmnMailDataGet(UMN_MAIL_ID_REPLY_REQUIRED)[0] & UMN_MAIL_REPLY_MASK)) {
                cell[2] = UMN_MAIL_TEXT_DIM;
                cell[1] = UMN_MAIL_TEXT_DIM;
                cell[0] = UMN_MAIL_TEXT_DIM;
            } else {
                cell[2] = -128;
                cell[1] = -128;
                cell[0] = -128;
            }
            slots[1] = 272;
            break;
        }
        case UMN_MAIL_STEP_REPLY_OPEN:
        case UMN_MAIL_STEP_REPLY_SELECT: {
            unsigned char *cell = text_5[2] + 1;

            if (UmnWork.price_balance >= 0) {
                cell[2] = -128;
                cell[1] = -128;
                cell[0] = -128;
            } else {
                cell[2] = UMN_MAIL_TEXT_DIM;
                cell[1] = UMN_MAIL_TEXT_DIM;
                cell[0] = UMN_MAIL_TEXT_DIM;
            }
            slots[2] = 272;
            break;
        }
        default:
            break;
        }

        for (i = 0; i < 3; i++) {
            MoveSlide(&work->slots[i].slide_x, &slots[i], 3.0f);
            work->slots[i].message.x = work->slots[i].slide_x;
            work->slots[i].message.y = work->slots[i].slide_y;
            work->slots[i].message.draw_parameter = work->window_color;
            eMessageMain(&work->slots[i].message);
        }
        for (i = 0; i < 2; i++) {
            MoveSlide(&work->rows[i].slide_x, &targets[i], 3.0f);
            work->rows[i].ribbon.x = work->rows[i].slide_x;
            work->rows[i].ribbon.y = work->rows[i].slide_y;
            work->rows[i].ribbon.work = work->window_color;
            eRibbonMain(&work->rows[i].ribbon);
            work->rows[i].tag.x = work->rows[i].slide_x + 188;
            work->rows[i].tag.work = work->rows[i].ribbon.work + 1;
            work->rows[i].tag.y = work->rows[i].slide_y + 4;
            eTagFontMain(&work->rows[i].tag);
            work->rows[i].number.x = work->rows[i].tag.x + 30;
            work->rows[i].number.y = work->rows[i].tag.y;
            work->rows[i].number.work = work->rows[i].tag.work;
            work->rows[i].number.format = 3;
            work->rows[i].number.value = UmnWork.count * 65536
                                          + UmnWork.list_cursor + 1;
            work->rows[i].number.digits = 2;
            eNumberMain(&work->rows[i].number);
        }
        break;
    }
    }
}

void UmnMailFolderSet(void)
{
    int SisoSiso(int mail_id)
    {
        int child_index = 0;
        UmnHistoryNode *children = UmnHistoryTreeGet(mail_id);

        if (children == 0) {
            return 0;
        }
        do {
            int child_id = children->children[child_index];

            child_index++;
            if (child_id < 0) {
                return 0;
            }
            if (child_id == UmnWork.entry || SisoSiso(child_id)) {
                return 1;
            }
        } while (child_index < UMN_MAIL_HISTORY_CHILDREN);
        return 0;
    }

    int i;
    unsigned char *folder;
    UmnHistoryNode *history;
    signed char *ids;

    if (UmnWork.mail_selection_state != 0) {
        int selected;

        UmnWork.saved_list_cursor = UmnWork.list_cursor;
        UmnWork.saved_scroll = UmnWork.scroll;
        selected = UmnWork.entry;
        UmnWork.saved_row = UmnWork.row;
        folder = mail_folder;

        /* The root folder is the first mail whose history holds the selection. */
        for (i = 0; i < UMN_MAIL_HISTORY_ROOTS; i++) {
            if (SisoSiso(i)) {
                selected = i;
                break;
            }
        }
        *folder++ = selected;
        UmnWork.count = 1;

        /* Follow the first readable child of every level down the tree. */
        history = UmnHistoryTreeGet(selected);
        while (history != 0) {
            for (i = 0; i < UMN_MAIL_HISTORY_CHILDREN; i++) {
                if (history->children[i] < 0) {
                    continue;
                }
                if ((UmnMailDataGet(history->children[i])[0] & UMN_MAIL_FLAG_RECEIVED) == 0) {
                    continue;
                }
                UmnWork.count++;
                *folder++ = history->children[i];
                history = UmnHistoryTreeGet(history->children[i]);
                break;
            }
            if (i == UMN_MAIL_HISTORY_CHILDREN) {
                break;
            }
        }

        for (i = 0; i < UmnWork.count; i++) {
            if (mail_folder[i] == UmnWork.entry) {
                break;
            }
        }
        UmnWork.list_cursor = i;
        if (i >= UMN_MAIL_FOLDER_VISIBLE) {
            UmnWork.row = UMN_MAIL_FOLDER_VISIBLE - 1;
            UmnWork.scroll = i - (UMN_MAIL_FOLDER_VISIBLE - 1);
        } else {
            UmnWork.row = i;
        }
    } else {
        int entry;

        UmnWork.count = 0;
        for (ids = umnMailSaveData.mail_ids; *ids != -1; ids++) {
            UmnWork.count++;
        }
        /* The list is stored newest first; the folder shows it oldest first. */
        for (entry = 0; entry < UmnWork.count; entry++) {
            mail_folder[entry] = *--ids;
        }
        UmnWork.scroll = UmnWork.saved_scroll;
        UmnWork.row = UmnWork.saved_row;
        UmnWork.list_cursor = UmnWork.saved_list_cursor;
        UmnWork.entry = mail_folder[(signed char)UmnWork.saved_list_cursor];
    }
}

void UmnMail(int menu_id)
{
    int i;

    switch (UmnWork.tab_state) {
    case UMN_STEP_INIT: {
        int task_base;

        task_base = UmnWorkEnd;
        task_base = (task_base + 15) & -16;
        UmnObjectTaskCreate(tskUmnMailPas, (void *)task_base);
        task_base = (task_base + 983) & -16;
        UmnObjectTaskCreate(tskUmnMailInfo, (void *)task_base);
        task_base = (task_base + 1119) & -16;
        UmnObjectTaskCreate(tskUmnMailFolder, (void *)task_base);
        task_base = (task_base + 4431) & -16;
        UmnObjectTaskCreate(tskUmnMailHensin, (void *)task_base);
        task_base = (task_base + 739) & -16;
        UmnObjectTaskCreate(tskUmnMailMenu, (void *)task_base);
        task_base = (task_base + 3959) & -16;
        UmnObjectTaskCreate(tskUmnMailExWin, (void *)task_base);
        task_base = (task_base + 827) & -16;
        UmnObjectTaskCreate(tskUmnMailDisp, (void *)task_base);
        UmnEventTextInit(task_base + 800);
        if (menu_id == 1) {
            UmnWork.tab_state = UMN_MAIL_STEP_READ_WAIT;
            UmnWork.wait = 2;
        } else {
            UmnWork.tab_state = UMN_MAIL_STEP_FOLDER_OPEN;
        }
        UmnMailFolderSet();
        if (UmnWork.count == 0) {
            UmnChangeTopLevel(0);
            return;
        }
        break;
    }
    case UMN_MAIL_STEP_SWITCH_OPEN:
        UmnWork.moving = 1;
        UmnWork.tab_state = UMN_MAIL_STEP_SWITCH;
        /* fall through */
    case UMN_MAIL_STEP_SWITCH:
        if (UmnWork.moving == 0) {
            UmnWork.tab_state = UMN_MAIL_STEP_FOLDER_OPEN;
            UmnWork.mail_selection_state ^= 1;
        }
        break;
    case UMN_MAIL_STEP_FOLDER_OPEN:
        UmnWork.tab_state = UMN_MAIL_STEP_FOLDER;
        UmnWork.moving = 1;
        do {
            UmnWork.direction = 0;
        } while (0);
        UmnWork.wait = 0;
        UmnMailFolderSet();
        /* fall through */
    case UMN_MAIL_STEP_FOLDER: {
        if (UmnWork.moving == 0) {
        if (UmnWork.wait == 0) {
            signed char scroll_dir = 0;

            if ((PadData.half_30 & PAD_UP) != 0) {
                if (UmnWork.list_cursor == 0) {
                    if ((PadData.mappedPressed & PAD_UP) != 0) {
                        scroll_dir = -1;
                    }
                } else if (UmnWork.row == 0) {
                    scroll_dir = -1;
                } else if ((PadData.mappedRepeat & PAD_UP) != 0) {
                    scroll_dir = -1;
                }
            } else if ((PadData.half_30 & PAD_DOWN) != 0) {
                if (UmnWork.list_cursor == UmnWork.count - 1) {
                    if ((PadData.mappedPressed & PAD_DOWN) != 0) {
                        scroll_dir = 1;
                    }
                } else if (UmnWork.row == UMN_MAIL_FOLDER_VISIBLE - 1) {
                    scroll_dir = 1;
                } else {
                    {
                        int right_bits = PadData.mappedRepeat & PAD_DOWN;

                        scroll_dir = right_bits != 0;
                    }
                }
            }

            if (scroll_dir != 0) {
                int last_index = (unsigned char)UmnWork.count - 1;
                struct UmnMailEdgePair sel_edges;
                struct UmnMailEdgePair sel_init = { { 0, last_index } };
                struct UmnMailEdgePair vis_edges;
                struct UmnMailEdgePair vis_init;
                int idx;

                sel_edges = sel_init;
                vis_init.edge[1] = last_index;
                vis_init.edge[0] = 0;
                vis_edges = vis_init;
                idx = ((-scroll_dir) >> 1) & 1;
                if (vis_edges.edge[1] >= UMN_MAIL_FOLDER_VISIBLE) {
                    vis_edges.edge[1] = UMN_MAIL_FOLDER_VISIBLE - 1;
                }
                UmnWork.wait = 4;
                if (UmnWork.list_cursor == sel_edges.edge[idx]) {
                    UmnWork.list_cursor = sel_edges.edge[idx ^ 1];
                } else {
                    UmnWork.list_cursor += scroll_dir;
                }
                if (UmnWork.row == vis_edges.edge[idx]) {
                    if (UmnWork.count >= UMN_MAIL_FOLDER_VISIBLE + 1) {
                        UmnWork.direction = scroll_dir;
                        UmnWork.scroll += scroll_dir;
                    } else {
                        UmnWork.row = sel_edges.edge[idx ^ 1];
                    }
                } else {
                    UmnWork.row += scroll_dir;
                }
                xglSoundEffectNormalID(SE_CURSOR, 0);
            } else {
                if (PadData.half_2a == PAD_CIRCLE) {
                    UmnWork.tab_state = UMN_MAIL_STEP_MENU_OPEN;
                    UmnWork.cursor = 0;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
                if (PadData.half_2a == PAD_CROSS) {
                    if (UmnWork.mail_selection_state != 0) {
                        UmnWork.tab_state = UMN_MAIL_STEP_SWITCH_OPEN;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    } else {
                        UmnWork.tab_state = UMN_STEP_LEAVE;
                        xglSoundEffectNormalID(SE_CANCEL, 0);
                    }
                }
            }
        }
        }

        /* refresh the selected mail */
        UmnWork.mail_text_ready = 0;
        UmnWork.mail_has_data = 0;
        UmnWork.entry = (signed char)mail_folder[UmnWork.list_cursor];
        {
            UmnMailHeader *header;
            unsigned char *data;

            header = UmnMailHeaderGet(UmnWork.entry);
            if (header->attach != 0) {
                data = UmnMailDataGet(UmnWork.entry);
                UmnWork.mail_has_data = 1;
                if ((data[1] & UMN_MAIL_ATTACH_TAKEN) == 0) {
                    UmnWork.mail_text_ready = 1;
                }
            }
        }
        break;
    }
    case UMN_MAIL_STEP_MENU_OPEN:
        UmnWork.tab_state = UMN_MAIL_STEP_MENU;
        UmnWork.moving = 1;
        /* fall through */
    case UMN_MAIL_STEP_MENU:
        if (UmnWork.moving == 0) {
            int new_cursor;

            new_cursor = MenuSelectMove(UmnWork.cursor, UMN_MAIL_MENU_ENTRIES, 0);
            UmnWork.cursor = new_cursor;
            if (PadData.mappedRepeat == PAD_CIRCLE) {
                switch ((signed char)new_cursor) {
                case UMN_MAIL_MENU_READ:
                    UmnWork.tab_state = UMN_MAIL_STEP_READ_WAIT;
                    UmnWork.wait = 2;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                    break;
                case UMN_MAIL_MENU_HISTORY: {
                    int sound_id;

                    if (UmnWork.mail_selection_state != 0) {
                        sound_id = SE_BUZZER;
                    } else {
                        UmnWork.tab_state = UMN_MAIL_STEP_SWITCH_OPEN;
                        sound_id = SE_DECIDE;
                    }
                    xglSoundEffectNormalID(sound_id, 0);
                    break;
                }
                case UMN_MAIL_MENU_BACK:
                    UmnWork.tab_state = UMN_MAIL_STEP_FOLDER;
                    UmnWork.wait = 8;
                    UmnWork.moving = 1;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                    break;
                case UMN_MAIL_MENU_ATTACH:
                    if (UmnWork.mail_has_data != 0 && UmnWork.mail_text_ready != 0) {
                        UmnWork.tab_state = UMN_MAIL_STEP_ATTACH_OPEN;
                        xglSoundEffectNormalID(SE_DECIDE, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                    break;
                default:
                    break;
                }
            }
            if (PadData.mappedRepeat == PAD_CROSS) {
                UmnWork.tab_state = UMN_MAIL_STEP_FOLDER;
                UmnWork.wait = 8;
                UmnWork.moving = 1;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    case UMN_MAIL_STEP_READ_WAIT:
        if (UmnWork.wait == 0) {
            UmnWork.tab_state = UMN_MAIL_STEP_READ_OPEN;
            if (menu_id == 1) {
                UmnWork.wait = 8;
            }
        }
        break;
    case UMN_MAIL_STEP_READ_OPEN: {
        unsigned char *data;
        UmnMailHeader *header;
        UmnMailSaveData *kosmos;
        unsigned char *src;
        unsigned char *dst;
        unsigned char name_char;
        int name_index;
        UmnWork.tab_state = UMN_MAIL_STEP_READ;
        UmnWork.moving = 1;
        header = UmnMailHeaderGet(UmnWork.entry);
        UmnWork.name_char_count = 0;
        src = header->name;
        for (name_index = 0; name_index < UMN_MAIL_NAME_CHARS; name_index++) {
            name_char = *src++;
            if (name_char != 0) {
                UmnWork.name[name_index] = name_char;
                UmnWork.name_char_count++;
            } else {
                UmnWork.name[name_index] = UMN_MAIL_NAME_NONE;
            }
        }
        UmnWork.mail_cursor = 0;
        UmnWork.name_lookup = UmnWork.name[0];
        if (UmnMailEventCheck(UmnWork.entry) != 0) {
            UmnWork.name_char_count = 0;
        }
        data = UmnMailDataGet(UmnWork.entry);
        if ((data[0] & UMN_MAIL_FLAG_READ) == 0) {
            header = UmnMailHeaderGet(UmnWork.entry);
            kosmos = &umnMailSaveData;
            for (i = 0; i < UMN_MAIL_KOSMOS_ITEMS; i++) {
                int item = header->kosmos_items[i];

                if (item != 0) {
                    kosmos->kosmos_items[i] += item;
                }
            }
            UmnKosmosSpecialGetCheck(UmnWork.entry);
        }
        {
            unsigned char *data;
            UmnMailHeader *header;

            data = UmnMailDataGet(UmnWork.entry);
            header = UmnMailHeaderGet(UmnWork.entry);
            data[0] |= UMN_MAIL_FLAG_READ;
            UmnWork.header_gate_flag = 0;
            if ((data[0] & UMN_MAIL_FLAG_REPLY) == 0 && UmnWork.name_char_count != 0 && (data[0] & UMN_MAIL_REPLY_MASK) == 0) {
                unsigned short limit = header->scenario_limit;

                if (MenuScenarioNoGet() < limit) {
                    UmnWork.header_gate_flag = 1;
                }
            }
        }
        {
            UmnMailHeader *header;
            unsigned char *data;

            header = UmnMailHeaderGet(UmnWork.entry);
            data = UmnMailDataGet(UmnWork.entry);
            data[1] |= header->attach;
            if (UmnMailCompulsionDownLoadCheck(UmnWork.entry) == 1) {
                data[1] |= UMN_MAIL_ATTACH_TAKEN;
                UmnMailAttachSet(header->attach);
            }
        }
        /* fall through */
    }
    case UMN_MAIL_STEP_READ: {
        unsigned char *data;
        if (UmnWork.moving == 0 && UmnWork.wait == 0) {
            if (PadData.half_2a == PAD_CROSS) {
                int ev;

                ev = UmnMailEventCheck(UmnWork.entry);
                if (ev != 0) {
                    data = UmnMailDataGet(UmnWork.entry);
                    data[0] |= UMN_MAIL_FLAG_EVENT_SEEN;
                    if (UmnManzaiInit(ev) < 0) {
                        UmnManzaiInit(0);
                    }
                    UmnWork.tab_state = UMN_MAIL_STEP_EVENT;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else if (UmnWork.entry == UMN_MAIL_ID_REPLY_REQUIRED) {
                    data = UmnMailDataGet(UMN_MAIL_ID_REPLY_REQUIRED);
                    if ((data[0] & UMN_MAIL_REPLY_MASK) != 0) {
                        UmnWork.tab_state = UMN_MAIL_STEP_FOLDER;
                        UmnWork.moving = 1;
                        xglSoundEffectNormalID(SE_CANCEL, 0);
                    } else {
                        xglSoundEffectNormalID(SE_BUZZER, 0);
                    }
                } else {
                    UmnWork.tab_state = UMN_MAIL_STEP_FOLDER;
                    UmnWork.moving = 1;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                }
            }
            if (PadData.half_2a == PAD_CIRCLE) {
                if (UmnWork.header_gate_flag != 0) {
                    UmnWork.tab_state = UMN_MAIL_STEP_REPLY_OPEN;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            }
        }
        break;
    }
    case UMN_MAIL_STEP_REPLY_OPEN: {
        UmnMailHeader *header;
        header = UmnMailHeaderGet(UmnWork.entry);
        UmnWork.price_balance = header->cost;
        if (dataMoneyBoxChk() < header->cost) {
            UmnWork.price_balance = dataMoneyBoxChk() - header->cost;
        }
        UmnWork.moving = 1;
        UmnWork.mail_cursor = 0;
        UmnWork.wait = 16;
        UmnWork.tab_state = UMN_MAIL_STEP_REPLY_SELECT;
        /* fall through */
    }
    case UMN_MAIL_STEP_REPLY_SELECT:
        if (UmnWork.moving == 0 && UmnWork.wait == 0) {
            int new_cursor;

            new_cursor = MenuSelectMove(UmnWork.mail_cursor, UmnWork.name_char_count, 0);
            if (new_cursor != UmnWork.mail_cursor) {
                UmnWork.mail_cursor = new_cursor;
                UmnWork.wait = 8;
            } else if ((PadData.half_2a & PAD_CIRCLE) != 0) {
                if (UmnWork.price_balance >= 0) {
                    UmnWork.tab_state = UMN_MAIL_STEP_REPLY_CONFIRM_OPEN;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                } else {
                    xglSoundEffectNormalID(SE_BUZZER, 0);
                }
            } else if ((PadData.half_2a & PAD_CROSS) != 0) {
                UmnWork.tab_state = UMN_MAIL_STEP_READ;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        UmnWork.name_lookup = UmnWork.name[UmnWork.mail_cursor];
        break;
    case UMN_MAIL_STEP_REPLY_CONFIRM_OPEN:
        if (UmnWork.mail_selection_state == 0) {
            UmnWork.saved_row = 0;
            UmnWork.saved_scroll = 0;
            UmnWork.saved_list_cursor = 0;
        }
        UmnWork.confirm_cursor = 0;
        UmnWork.tab_state = UMN_MAIL_STEP_REPLY_CONFIRM;
        UmnWork.moving = 1;
        /* fall through */
    case UMN_MAIL_STEP_REPLY_CONFIRM:
        if (UmnWork.moving == 0) {
            int new_cursor;

            new_cursor = MenuSelectMove(UmnWork.confirm_cursor, UMN_MAIL_CONFIRM_ENTRIES, 0);
            UmnWork.confirm_cursor = new_cursor;
            if (PadData.half_2a == PAD_CIRCLE) {
                if ((signed char)new_cursor != 0) {
                    UmnWork.tab_state = UMN_MAIL_STEP_REPLY_SELECT;
                    xglSoundEffectNormalID(SE_CANCEL, 0);
                } else {
                    UmnWork.tab_state = UMN_MAIL_STEP_REPLY_SEND;
                    xglSoundEffectNormalID(SE_DECIDE, 0);
                }
            }
            if (PadData.half_2a == PAD_CROSS) {
                UmnWork.tab_state = UMN_MAIL_STEP_REPLY_SELECT;
                xglSoundEffectNormalID(SE_CANCEL, 0);
            }
        }
        break;
    case UMN_MAIL_STEP_REPLY_SEND: {
        unsigned char *data;
        data = UmnMailDataGet(UmnWork.entry);
        data[0] |= (unsigned char)((UmnWork.mail_cursor + 1) << UMN_MAIL_REPLY_SHIFT);
        {
            unsigned char *name_data;

            name_data = UmnMailDataGet(UmnWork.name_lookup);
            name_data[0] |= UMN_MAIL_FLAG_RECEIVED | UMN_MAIL_FLAG_REPLY;
        }
        UmnMailBoxSet(UmnWork.name_lookup);
        if (UmnWork.price_balance > 0) {
            dataMoneyBoxDec(UmnWork.price_balance);
        }
        UmnWork.tab_state = UMN_MAIL_STEP_REPLY_DONE;
        UmnWork.wait = 2;
        /* fall through */
    }
    case UMN_MAIL_STEP_REPLY_DONE:
        if (UmnWork.wait == 0 && PadData.half_2a == PAD_CIRCLE) {
            UmnWork.tab_state = UMN_MAIL_STEP_FOLDER_OPEN;
            xglSoundEffectNormalID(SE_DECIDE, 0);
        }
        break;
    case UMN_MAIL_STEP_ATTACH_OPEN: {
        unsigned char *data;
        UmnMailHeader *header;
        header = UmnMailHeaderGet(UmnWork.entry);
        data = UmnMailDataGet(UmnWork.entry);
        data[1] |= UMN_MAIL_ATTACH_TAKEN;
        data[1] |= header->attach;
        UmnMailAttachSet(header->attach);
        UmnWork.wait = 2;
        UmnWork.tab_state = UMN_MAIL_STEP_ATTACH_DONE;
        /* fall through */
    }
    case UMN_MAIL_STEP_ATTACH_DONE:
        if (UmnWork.wait == 0 && PadData.half_2a == PAD_CIRCLE) {
            UmnWork.tab_state = UMN_MAIL_STEP_FOLDER;
            xglSoundEffectNormalID(SE_DECIDE, 0);
        }
        break;
    case UMN_MAIL_STEP_EVENT:
        if (UmnManzaiControl() == 0) {
            UmnWork.tab_state = UMN_MAIL_STEP_READ_WAIT;
            if (UmnEventEndMailCheck(UmnWork.entry) != 0) {
                UmnMailFolderSet();
            }
        }
        break;
    case UMN_STEP_LEAVE:
        UmnWork.tab_state = UMN_STEP_LEAVE_WAIT;
        UmnWork.moving = 1;
        /* fall through */
    case UMN_STEP_LEAVE_WAIT:
        if (UmnWork.moving == 0) {
            UmnChangeTopLevel(0);
        }
        break;
    }

    if (UmnWork.wait != 0) {
        UmnWork.wait--;
    }
    xglFontDebugPrintf(0, 80, mailTabStateDebugFormat, (unsigned char)UmnWork.tab_state);
    xglFontDebugPrintf(0, 88, mailListCursorDebugFormat, UmnWork.list_cursor, UmnWork.count);
    xglFontDebugPrintf(0, 96, mailDirectionDebugFormat, UmnWork.direction);
}

const char mailPathHistoryText[16] = "/History";

const char mailPathReplyText[8] = "/Reply";

const char mailPathReadText[8] = "/Read";

const char mailPathFolderText[8] = "/Folder";

const char mailPathRootText[8] = "Mail";
