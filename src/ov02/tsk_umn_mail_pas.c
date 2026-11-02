/*
 * OV02 original TU 9: 0x00a05738..0x00a097d8 (10 functions)
 */
#include "common.h"

#include "shared.h"
#include "main/e_message.h"
typedef struct UmnMailPasTask {
    unsigned char unmodeled_00[0x10];
    int command;
} UmnMailPasTask;
typedef struct UmnMailPasWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    unsigned char unmodeled_0c[4];
    unsigned char state;
    unsigned char unmodeled_11[3];
    void (*callback)(void);
    void *callback_arg;
} UmnMailPasWindow;
typedef struct UmnMailPasBox {
    short x;
    short y;
    int kind;
    short width;
    short height;
} UmnMailPasBox;
typedef struct UmnMailPasWork {
    unsigned char unmodeled_00;
    unsigned char state;
    unsigned char unmodeled_02[2];
    int kind;
    UmnMailPasWindow window;
    unsigned char unmodeled_24[0x19c - 0x24];
    EMessageParam messages[5];
    unsigned char unmodeled_2f0[0x3bc - 0x2f0];
    UmnMailPasBox box;
} UmnMailPasWork;
typedef struct UmnMailState {
    unsigned char unmodeled_00;
    unsigned char mail_mode;
    unsigned char unmodeled_02;
    unsigned char tab_state;
    unsigned char unmodeled_04[0x10 - 0x04];
    signed char wait_timer;
    signed char transition_pending;
    signed char scroll_direction;
    unsigned char unmodeled_13[0x42 - 0x13];
    signed char visible_folder_index;
    unsigned char unmodeled_43;
    signed char scroll_offset;
    unsigned char unmodeled_45;
    signed char command_cursor;
    signed char confirm_cursor;
    unsigned char unmodeled_48[0x50 - 0x48];
    signed char selection_index;
    signed char folder_entry_count;
    signed char selected_mail_id;
    signed char mail_cursor;
    signed char name_char_count;
    signed char name_lookup;
    signed char name[3];
    signed char header_gate_flag;
    signed char mail_text_ready;
    signed char mail_has_data;
    signed char mail_selection_state;
    unsigned char saved_selection_index;
    unsigned char saved_scroll_offset;
    unsigned char saved_visible_folder_index;
    int price_balance;
    unsigned char unmodeled_64[0x80 - 0x64];
} UmnMailState;
extern UmnMailState UmnWork;
extern const char D_00A12268[];
extern const char D_00A12260[];
extern const char D_00A12258[];
extern const char D_00A12250[];
extern const char D_00A12240[];
static const char *msg00_0_00A10618[5] = {
    D_00A12268,
    D_00A12260,
    D_00A12258,
    D_00A12250,
    D_00A12240,
};
extern int MenuPasLengthGet(const char *text);
extern void WindowDXSet(void *window);
extern void WindowDXMain(void *window);
extern void MoveSlide(short *current, short *target, float rate);
extern void endPrintExtFunc(int kind, int id, void *data);
extern const char D_00A12D78[];
extern const char D_00A12D80[];
extern const char D_00A12D88[];
extern unsigned char mail_folder[0x80];
typedef struct UmnKosmosCounters {
    signed char mail_ids[0x80];
    unsigned char kosmos_items[5];
} UmnKosmosCounters;
extern UmnKosmosCounters D_4A1A0C;
extern int UmnWorkEnd;

typedef struct UmnMailHeaderPart {
    char title[64];
    char subject[64];
    unsigned char text[4];
    unsigned short scenario_limit;
    unsigned char attach;
    unsigned char kosmos_items[5];
    int cost;
} UmnMailHeaderPart;

typedef struct UmnMailPadView {
    unsigned char unmodeled_00[0x2a];
    unsigned short half_2a;
    unsigned char unmodeled_2c[4];
    unsigned short half_30;
    unsigned short half_32;
    unsigned short half_34;
} UmnMailPadView;
extern PadPrefix PadData;
#define UMN_MAIL_PAD ((UmnMailPadView *)&PadData)

void UmnObjectTaskCreate(void (*task)(), int work);
void UmnMailFolderSet(void);
void UmnChangeTopLevel(int level);
void *UmnEventTextInit(unsigned char *text);
int UmnManzaiControl(void);
int UmnManzaiInit(int mail_id);
UmnMailHeaderPart *UmnMailHeaderGet(int mail_id);
int UmnMailEventCheck(int mail_id);
int UmnMailCompulsionDownLoadCheck(int mail_id);
void UmnMailAttachSet(int attach);
void UmnKosmosSpecialGetCheck(int mail_id);
int UmnEventEndMailCheck(int mail_id);
int MenuSelectMove(int cursor, int count, int wrap);
int dataMoneyBoxChk(void);
void dataMoneyBoxDec(int money);
extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
void xglSoundEffectNormalID(int sound, int channel);
extern int UmnTexAddr;
extern void eSpriteSet(void *sprite, short sprite_id);
extern void eSpriteMain(void *sprite);
extern unsigned char *UmnMailDataGet(int box_id);

void tskUmnMailPas(UmnMailPasTask *task, UmnMailPasWork *work)
{
    int caption_widths[5];
    int i;
    unsigned char mail_mode;

    for (i = 0; i < 5; i++) {
        caption_widths[i] = MenuPasLengthGet(msg00_0_00A10618[i]);
    }

    mail_mode = UmnWork.mail_mode;
    if (mail_mode != 1) {
        task->command = -1;
    } else {
        switch (task->command) {
        case 0:
            work->kind = 0x00fffff0;
            WindowDXSet(&work->window);
            work->window.x = -0x110;
            work->window.y = 8;
            work->window.color = work->kind;
            work->window.width = 0x110;
            work->window.height = 32;
            work->window.state = mail_mode;
            WindowDXMain(&work->window);
            work->window.state = 3;

            for (i = 0; i < 5; i++) {
                eMessageSet(&work->messages[i], msg00_0_00A10618[i]);
                work->messages[i].mode = 0x20;
                work->messages[i].x = 0x120;
                work->messages[i].y = 0x0b;
                work->messages[i].draw_parameter = work->kind + 2;
            }
            work->state = 1;
            break;

        case 2: {
            short slide_targets[8];
            short window_target;

            window_target = -16;
            for (i = 0; i < 5; i++) {
                slide_targets[i] = 288;
            }

            switch (UmnWork.tab_state - 16) {
            case 0:
            case 1:
            case 16:
            case 17:
            case 32:
            case 33:
            case 37:
            case 80:
            case 81:
                slide_targets[0] = 16;
                slide_targets[UmnWork.mail_selection_state * 3 + 1] =
                    (short)(caption_widths[0] + 16);
                break;
            case 48:
            case 49:
                slide_targets[0] = 16;
                slide_targets[2] = (short)(caption_widths[0] + 16);
                break;
            case 64:
            case 65:
            case 66:
            case 67:
            case 68:
            case 69:
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
                work->box.kind = work->kind;
                work->box.width = work->window.width - 6;
                work->box.height = work->window.height - 6;
                endPrintExtFunc(work->kind, 0x65, &work->box);

                for (i = 0; i < 5; i++) {
                    MoveSlide(&work->messages[i].x, &slide_targets[i], 3.0f);
                    if (work->messages[i].x < 256) {
                        eMessageMain(&work->messages[i]);
                    }
                }
                endPrintExtFunc(work->kind, 0x66, 0);
            }
            break;
        }
        }
    }
}

struct UmnMailInfoTask {
    unsigned char unmodeled_00[0x10];
    int state;
};

struct UmnMailInfoWork {
    unsigned char mode;
    unsigned char saved_mail_mode;
    unsigned char unmodeled_02[6];
    int color;
    struct UmnMailPasWindow window;
    unsigned char unmodeled_28[0x1a0 - 0x28];
    struct UmnMailInfoText {
        short x;
        short y;
        unsigned char unmodeled_04[8];
        char *text;
    } info;
    unsigned char unmodeled_1b0[0x350 - 0x1b0];
    unsigned char text_buffer[0x100];
};

struct UmnMailSelection {
    unsigned char unmodeled_00[0x46];
    signed char list_index;
    unsigned char unmodeled_47[0x52 - 0x47];
    signed char box_id;
    unsigned char unmodeled_53[0x5b - 0x53];
    signed char has_sub;
    unsigned char unmodeled_5c[0x60 - 0x5c];
    int price_delta;
};

#define UMN_MAIL_SELECTION ((struct UmnMailSelection *)&UmnWork)

extern const char D_00A123B8[];
extern const char D_00A123C0[];
extern const char D_00A123C8[];
extern const char D_00A123D8[];
extern const char D_00A123E8[];
extern const char D_00A123F8[];
extern const char D_00A12420[];
extern const char D_00A12438[];
extern const char D_00A12458[];
extern const char D_00A12470[];
extern const char D_00A12488[];
extern const char D_00A124A8[];
extern const char D_00A124D0[];
extern const char D_00A124E0[];
extern const char D_00A124F8[];
extern const char D_00A12510[];
extern const char D_00A12520[];
extern const char D_00A12548[];
extern const char D_00A12550[];
extern const char D_00A12558[];
extern signed char *UmnHistoryTreeGet(unsigned int history_id);
extern char *MenuNumberTextGet(int value, int width, int flags);
void eMessageCpy(char *destination, const char *source);
void eMessageCat(const char *source);
void WindowDXSet(void *window);
void WindowDXMain(void *window);
void MenuInfoWindow(void);

struct UmnMailCaptionA {
    const char *text[6];
};

struct UmnMailCaptionB {
    const char *text[4];
};

struct UmnMailCaptionC {
    const char *text[7];
};

extern const struct UmnMailCaptionA D_00A12408;
extern const struct UmnMailCaptionB D_00A12478;
extern const struct UmnMailCaptionC D_00A12528;

void tskUmnMailInfo(void *task_opaque, void *work_opaque)
{
    struct UmnMailInfoTask *task = task_opaque;
    struct UmnMailInfoWork *work = work_opaque;
    struct UmnMailCaptionA caption_a = D_00A12408;
    struct UmnMailCaptionB caption_b = D_00A12478;
    struct UmnMailCaptionC caption_c = D_00A12528;
    short slide_target;
    unsigned char *data;
    signed char *history;
    UmnMailHeaderPart *header;
    UmnMailHeaderPart *box_header;
    int i;
    int k;
    int price_delta;
    int state;
    unsigned char mail_mode;
    unsigned char info_mode;

    mail_mode = UmnWork.mail_mode;
    if (mail_mode != 1) {
        task->state = -1;
        return;
    }
    state = task->state;
    switch (state) {
    case 0:
        work->saved_mail_mode = 0;
        work->mode = 0;
        work->color = 0x00FF0000;
        memset(work->text_buffer, 0, 0x100);
        WindowDXSet(&work->window);
        work->window.x = -16;
        work->window.color = work->color;
        work->window.callback = MenuInfoWindow;
        work->window.state = 0;
        work->window.y = 480;
        work->window.width = 544;
        work->window.height = 54;
        work->window.callback_arg = &work->info;
        work->info.x = 0;
        work->window.state = mail_mode;
        work->info.y = 0;
        work->info.text = 0;
        WindowDXMain(&work->window);
        work->window.state = 3;
        break;
    case 2:
        slide_target = 386;
        info_mode = work->mode;
        switch (info_mode) {
        case 0:
            work->saved_mail_mode = mail_mode;
            work->mode = state;
            /* fall through */
        case 2:
            switch (UmnWork.tab_state - 16) {
            case 0:
            case 1:
            case 16:
            case 17:
                data = UmnMailDataGet(UMN_MAIL_SELECTION->box_id);
                if ((data[0] & 8) != 0) {
                    for (i = 0; i < 0x51; i++) {
                        history = UmnHistoryTreeGet(i);
                        for (k = 0; k < 3; k++) {
                            if (history[k] == UMN_MAIL_SELECTION->box_id) {
                                break;
                            }
                        }
                        if (k != 3) {
                            break;
                        }
                    }
                    box_header = UmnMailHeaderGet(i);
                    if (box_header != 0) {
                        eMessageCpy((char *)work->text_buffer, caption_a.text[4]);
                        eMessageCat((const char *)box_header);
                        eMessageCat(D_00A12548);
                        eMessageCat(caption_a.text[5]);
                        eMessageCat((const char *)box_header + 0x40);
                        eMessageCat(D_00A12548);
                    } else {
                        eMessageCpy((char *)work->text_buffer, D_00A12550);
                    }
                } else {
                    if ((data[0] & 2) != 0) {
                        eMessageCpy((char *)work->text_buffer, caption_a.text[0]);
                    } else {
                        eMessageCpy((char *)work->text_buffer, caption_a.text[1]);
                    }
                    if (UMN_MAIL_SELECTION->has_sub != 0) {
                        eMessageCat(caption_a.text[2]);
                    } else {
                        eMessageCat(caption_a.text[3]);
                    }
                }
                break;
            case 32:
            case 33:
                eMessageCpy((char *)work->text_buffer, caption_b.text[UMN_MAIL_SELECTION->list_index]);
                break;
            case 64:
            case 65:
                price_delta = UMN_MAIL_SELECTION->price_delta;
                if (price_delta >= 0) {
                    header = UmnMailHeaderGet(UMN_MAIL_SELECTION->box_id);
                    eMessageCpy((char *)work->text_buffer, caption_c.text[0]);
                    eMessageCat((const char *)header + 0x40);
                } else {
                    int cost;
                    int magnitude;

                    header = UmnMailHeaderGet(UMN_MAIL_SELECTION->box_id);
                    magnitude = -UMN_MAIL_SELECTION->price_delta;
                    cost = header->cost;
                    eMessageCpy((char *)work->text_buffer, caption_c.text[4]);
                    eMessageCat(MenuNumberTextGet(cost, 8, 1));
                    eMessageCat(caption_c.text[6]);
                    eMessageCat(D_00A12558);
                    eMessageCat(caption_c.text[5]);
                    eMessageCat(MenuNumberTextGet(magnitude, 8, 1));
                    eMessageCat(caption_c.text[6]);
                }
                break;
            case 66:
            case 67:
                eMessageCpy((char *)work->text_buffer, caption_c.text[1]);
                break;
            case 68:
            case 69:
                eMessageCpy((char *)work->text_buffer, caption_c.text[2]);
                break;
            case 80:
            case 81:
                eMessageCpy((char *)work->text_buffer, caption_c.text[3]);
                break;
            default:
                eMessageCpy((char *)work->text_buffer, D_00A12550);
                slide_target = 512;
                break;
            }
            break;
        }
        work->info.text = (char *)work->text_buffer;
        MoveSlide(&work->window.y, &slide_target, 3.0f);
        WindowDXMain(&work->window);
        break;
    }
}

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
    UmnMailPasWindow window;
    unsigned char unmodeled_28[0x1a0 - 0x28];
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
    int kind;
    UmnMailFolderRecord records[6];
    ECursolMainData cursor;
    int cursor_visible;
    ECursolMainData cursor2;
    ECursolMainData cursor3;
    int box_visible;
    UmnMailPasBox box;
    int tex;
} UmnMailFolderWork;

void tskUmnMailFolder(UmnMailPasTask *task, UmnMailFolderWork *work)
{
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
    UmnMailHeaderPart *hdr;
    int more;
    UmnMailFolderHead *head;

    if (UmnWork.mail_mode != 1) {
        task->command = -1;
        return;
    }
    switch (task->command) {
    case 0:
        work->x = 528;
        work->y = 96;
        work->kind = 0x00ff0000;
        for (i = 0; i < 6; i++) {
            work->records[i].x = 0;
            work->records[i].y = 0;
            WindowDXSet(&work->records[i].window);
            work->records[i].window.x = 528;
            work->records[i].window.y = 0;
            work->records[i].window.width = 440;
            work->records[i].window.height = 54;
            work->records[i].window.state = 1;
            WindowDXMain(&work->records[i].window);
            work->records[i].window.state = 3;
        }
        eCursolSet(&work->cursor, 0);
        work->cursor.work = work->kind + 2;
        eCursolSet(&work->cursor2, 1);
        eCursolModeChange(&work->cursor2, 32);
        work->cursor2.work = work->kind + 8;
        eCursolSet(&work->cursor3, 2);
        eCursolModeChange(&work->cursor3, 32);
        work->cursor3.work = work->kind + 8;
        work->box_visible = 0;
        work->state = 0;
        work->flag = 0;
        break;

    case 2:
        work->count = UmnWork.folder_entry_count;
        if (work->count >= 5) {
            work->count = 4;
        }
        switch (work->state) {
        case 0:
            work->flag = 0;
            work->state = 10;
            /* fall through */
        case 10:
            if (UmnWork.tab_state == 33) {
                work->flag = 1;
                work->state = 20;
                for (i = 0; i < work->count; i++) {
                    work->records[i].x = -448;
                    work->records[i].y = 96 + 64 * i;
                }
                eCursolModeChange(&work->cursor, 32);
            } else if (UmnWork.tab_state == 65) {
                work->flag = 1;
                work->state = 50;
                work->records[0].x = -448;
                work->records[0].y = 48;
                for (i = 1; i < work->count; i++) {
                    work->records[i].x = -448;
                    work->records[i].y = 96 + 64 * i;
                }
                eCursolModeChange(&work->cursor, 32);
            }
            break;

        case 20:
            {
                int moved;
                short ty_arg;
                moved = 0;
                for (i = 0; i < work->count; i++) {
                    tx = 32;
                    MoveSlide(&work->records[i].x, &tx, 3.0f);
                    if (work->records[i].x == tx) {
                        if (i == UmnWork.visible_folder_index) {
                            ty_arg = 96 + 64 * i;
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
                work->state = 30;
                work->box_visible = 1;
                UmnWork.transition_pending = 0;
                work->cursor_visible = 1;
            }
            break;

        case 30:
            tab = UmnWork.tab_state;
            {
                int waiting = 1;

                if (tab == 49) {
                    work->box_visible = 0;
                    work->state = 40;
                    work->cursor_visible = 0;
                } else if (tab == 241) {
                    work->box_visible = 0;
                    work->state = 90;
                    work->cursor_visible = 0;
                } else if (tab == 17) {
                    work->box_visible = 0;
                    work->state = 42;
                    work->cursor_visible = 0;
                } else if (UmnWork.scroll_direction != 0) {
                    work->state = 32;
                    UmnWork.transition_pending = 1;
                    waiting = 0;
                }
                if (waiting) {
                    break;
                }
            }
            /* fall through */

        case 32:
            if (UmnWork.scroll_direction == 1) {
                work->records[4].x = work->records[0].x;
                work->records[4].y = work->records[0].y;
            } else {
                work->records[4].x = work->records[3].x;
                work->records[4].y = work->records[3].y;
            }
            for (i = 0; i < work->count; i++) {
                work->records[i].y += UmnWork.scroll_direction * 64;
            }
            work->state = 34;
            /* fall through */

        case 34:
            {
                short ty;
                UmnMailState *state;
                for (i = 0; i < work->count; i++) {
                    work->records[i].y -= UmnWork.scroll_direction * 8;
                }
                state = &UmnWork;
                if (state->scroll_direction == 1) {
                    ty = work->records[4].y = work->records[0].y - 64;
                    if (ty == 32) {
                        work->state = 30;
                        state->scroll_direction = 0;
                        state->transition_pending = 0;
                    }
                } else {
                    ty = work->records[4].y = work->records[3].y + 64;
                    if (ty == 352) {
                        work->state = 30;
                        state->scroll_direction = 0;
                        state->transition_pending = 0;
                    }
                }
            }
            break;

        case 40:
        case 42:
            {
                short tx;
                switch (work->state) {
                case 40:
                    tx = 144;
                    break;
                case 42:
                    tx = 528;
                    break;
                }
                for (i = 0; i < work->count; i++) {
                    MoveSlide(&work->records[i].x, &tx, 3.0f);
                }
                if (work->records[UmnWork.visible_folder_index].x == tx) {
                    switch (work->state) {
                    case 40:
                        work->state = 45;
                        break;
                    case 42:
                        work->state = 10;
                        UmnWork.transition_pending = 0;
                        break;
                    }
                }
            }
            break;

        case 45:
            switch (UmnWork.tab_state) {
            case 17:
                work->state = 42;
                break;
            case 65:
                work->state = 50;
                break;
            case 97:
            case 33:
                work->state = 20;
                break;
            default:
                UmnWork.transition_pending = 0;
                break;
            }
            break;

        case 50:
            {
                cnt = 0;
            for (i = 0; i < work->count; i++) {
                if (i == UmnWork.visible_folder_index) {
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
                    tx_side = -448;
                    MoveSlide(&work->records[i].x, &tx_side, 3.0f);
                    if (work->records[i].x == tx_side) {
                        cnt++;
                    }
                }
            }
            if (cnt == work->count) {
                work->state = 60;
            }
            }
            break;

        case 60:
            switch (UmnWork.tab_state) {
            case 33:
                work->state = 20;
                break;
            case 81:
            case 128:
                work->state = 70;
                break;
            default:
                UmnWork.transition_pending = 0;
                break;
            }
            break;

        case 70:
            {
                short tx;
                tx = -448;
                MoveSlide(&work->records[UmnWork.visible_folder_index].x, &tx, 3.0f);
                if (work->records[UmnWork.visible_folder_index].x == tx) {
                    work->state = 80;
                }
            }
            break;

        case 80:
            switch (UmnWork.tab_state) {
            case 65:
                work->state = 50;
                break;
            case 33:
                work->state = 20;
                for (i = 0; i < 4; i++) {
                    work->records[i].x = -448;
                    work->records[i].y = 96 + 64 * i;
                }
                break;
            default:
                break;
            }
            break;

        case 90:
            {
                short tx;
                unsigned int moved2;
                moved2 = 0;
                for (i = 0; i < work->count; i++) {
                    tx = -448;
                    MoveSlide(&work->records[i].x, &tx, 3.0f);
                    if (work->records[i].x == tx) {
                        moved2++;
                    }
                }
                if (moved2 == work->count) {
                    work->state = 0;
                    UmnWork.transition_pending = 0;
                }
            }
            break;

        default:
            break;
        }

        if (work->flag != 0) {
            more = 0;
            if (UmnWork.scroll_direction != 0) {
                more = 1;
            }
            for (i = 0; i < work->count + more; i++) {
                k = UmnWork.selection_index - UmnWork.visible_folder_index + i;
                if (i == 4 && UmnWork.scroll_direction == 1) {
                    k = UmnWork.selection_index - UmnWork.visible_folder_index - 1;
                }
                if (k < 0) {
                    k += UmnWork.folder_entry_count;
                } else {
                    k = k % UmnWork.folder_entry_count;
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
                work->box.kind = 96;
                endPrintExtFunc(work->kind, 5, &work->box);
            }

            for (i = 0; i < work->count + more; i++) {
                work->records[i].window.x = work->records[i].x;
                if (work->records[i].head.flags & 8) {
                    work->records[i].window.x += 16;
                }
                work->records[i].window.y = work->records[i].y;
                if (i == UmnWork.selection_index) {
                    work->records[i].window.color = work->kind;
                } else {
                    work->records[i].window.color = work->kind - 16;
                }
                WindowDXMain(&work->records[i].window);
                xglFontDebugPrintf(work->records[i].x / 2, work->records[i].y / 2, "MailId = %2d", work->records[i].folder_id);
            }

            sel = UmnWork.visible_folder_index;
            work->cursor.work = work->records[sel].window.color;
            work->cursor.x = work->records[sel].window.x + 3;
            work->cursor.y = work->records[sel].window.y + 19;
            eCursolMain(&work->cursor);

            if (UmnWork.folder_entry_count >= 5 && work->cursor_visible) {
                work->cursor2.x = 474;
                work->cursor2.y = 96;
                work->cursor2.work = work->kind;
                eCursolMain(&work->cursor2);
                work->cursor3.x = 474;
                work->cursor3.y = 326;
                work->cursor3.work = work->kind;
                eCursolMain(&work->cursor3);
            }

            if (work->box_visible) {
                endPrintExtFunc(work->kind, 6, 0);
            }
            work->tex = UmnTexAddr;
            endPrintExtFunc(0, 14, &work->tex);

            if (work->box_visible) {
                endPrintExtFunc(work->kind, 5, &work->box);
                endPrintExtFunc(work->kind, 101, &work->box);
            }

            for (i = 0; i < work->count + more; i++) {
                int pick;

                hdr = UmnMailHeaderGet(work->records[i].folder_id);
                pick = 5128;
                if (work->records[i].head.flags & 8) {
                    pick = 5129;
                } else if (work->records[i].head.flags & 2) {
                    pick = 5130;
                }
                eSpriteSet(&work->records[i].sprites[0], pick);
                work->records[i].sprites[0].x = work->records[i].window.x + 19;
                work->records[i].sprites[0].y = work->records[i].window.y + 3;
                work->records[i].sprites[0].color = work->records[i].window.color + 1;
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
                    eMessageModeChange(&work->records[i].messages[j], 32);
                    work->records[i].messages[j].x = work->records[i].sprites[j + 1].x + 20;
                    work->records[i].messages[j].y = work->records[i].sprites[j + 1].y;
                    work->records[i].messages[j].draw_parameter = work->records[i].sprites[j + 1].color + 2;
                    eMessageMain(&work->records[i].messages[j]);
                }

                if (hdr->attach != 0) {
                    eSpriteSet(&work->records[i].sprites[3], 5126);
                    if (work->records[i].head.tex_flags & 0x80) {
                        work->records[i].sprites[3].rgb[0] = work->records[i].sprites[3].rgb[1] = work->records[i].sprites[3].rgb[2] = 64;
                    }
                } else {
                    eSpriteSet(&work->records[i].sprites[3], 5127);
                }
                work->records[i].sprites[3].x = work->records[i].messages[0].x + 326;
                work->records[i].sprites[3].y = work->records[i].messages[0].y + 14;
                work->records[i].sprites[3].color = work->records[i].messages[0].draw_parameter;
                eSpriteMain(&work->records[i].sprites[3]);
            }

            if (work->box_visible) {
                endPrintExtFunc(work->kind, 6, 0);
                endPrintExtFunc(work->kind, 102, 0);
            }
            endPrintExtFunc(0, 15, 0);
        }
        break;

    default:
        break;
    }

    xglFontDebugPrintf(0, 16, "Folder : %2d", work->state);
}

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
    UmnMailPasWindow window;
    unsigned char unmodeled_30[0x1a8 - 0x30];
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

extern unsigned char *UmnModelUkn;
extern const char sikaku_1[];
extern const short D_00A12840[4];
extern const char D_00A12848[];
extern const char D_00A12860[];
extern int tyaUmlDispMain(void *param);
extern int tyaUmlDispParamReset(void *param, int value);

typedef struct UmnMailSpriteIds {
    short id[4];
} UmnMailSpriteIds;

void tskUmnMailDisp(UmnMailPasTask *task, UmnMailDispWork *work)
{
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

    mail_mode = UmnWork.mail_mode;
    if (mail_mode != 1) {
        task->command = -1;
        return;
    }

    switch (task->command) {
    case 0:
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
        work->window.state = 3;
        for (c = 0; c < 2; c++) {
            eCursolSet(&work->cursor_slots[c], c + 1);
            work->cursor_slots[c].state = 32;
            work->cursor_slots[c].work = 0x00f00010;
        }
        tyaUmlDispParamReset(&work->tya_value, 0);
        work->tya_pos_x = work->pos_x + 1798;
        work->tya_pos_y = work->pos_y + 1830;
        work->scroll_x = 48;
        work->cursor_x = 474;
        work->cursor_pitch = 288;
        work->tya_kind = (unsigned int)(work->pos_kind + 2);
        sprite_ids = *(const UmnMailSpriteIds *)D_00A12840;
        for (i = 0; i < 4; i++) {
            eSpriteSet(&work->sprites[i], sprite_ids.id[i]);
            work->sprites[i].color = 0x00f00000;
            work->item_anim[i] = 0;
        }
        eMessageSet(&work->message, sikaku_1);
        work->message.mode = 32;
        work->message.draw_parameter = 0x00f00000;
        work->state = 0;
        work->step = 0;
        break;
    case 2:
        switch (work->step) {
        case 0:
            work->state = 0;
            work->step = 10;
            /* fallthrough */
        case 10:
            if (UmnWork.tab_state == 53) {
                work->step = 20;
            }
            break;
        case 20:
            if (UmnModelUkn != 0 && UmnModelUkn[0x11] != 0) {
                work->pos_x = 256;
                work->pos_y = 259;
                work->slide_x = 6;
                work->slide_y = 6;
                work->window.width = 6;
                work->window.height = 6;
                tyaUmlDispParamReset(&work->tya_value, 0);
                selected_id = UmnWork.selected_mail_id;
                work->tya_value = selected_id;
                work->scroll_x = 48;
                work->step = 30;
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
        case 30:
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
                work->step = 40;
                work->flag_07 = 1;
            }
            break;
        case 40:
            switch (UmnWork.tab_state) {
            case 65:
                if (work->scrol_type == 0) {
                    quotient = ((short)work->scroll_end - 48) / (short)work->cursor_pitch;
                    if (quotient != 0) {
                        if (UMN_MAIL_PAD->half_30 & 0x1000) {
                            work->scrol_type = 1;
                        }
                        if (UMN_MAIL_PAD->half_30 & 0x4000) {
                            work->scrol_type = 2;
                        }
                        if (UMN_MAIL_PAD->half_30 & 0x4) {
                            if (work->now_page > 0) {
                                work->scrol_type = 3;
                                work->now_page = work->now_page - 1;
                                work->item_anim[1] = -6;
                            }
                        }
                        if (UMN_MAIL_PAD->half_30 & 0x8) {
                            remainder = ((short)work->scroll_end - 48) % (short)work->cursor_pitch;
                            pages = quotient;
                            if (remainder != 0) {
                                pages = quotient + 1;
                            }
                            if (work->now_page < pages - 1) {
                                work->scrol_type = 4;
                                work->now_page = work->now_page + 1;
                                work->item_anim[2] = 6;
                            }
                        }
                        if (UMN_MAIL_PAD->half_30 & 0x1) {
                            if (work->scroll_x != 48) {
                                work->scrol_type = 5;
                                work->item_anim[0] = -6;
                            }
                        }
                        if (UMN_MAIL_PAD->half_30 & 0x2) {
                            if (work->scroll_x != (short)work->scroll_end - (short)work->cursor_pitch) {
                                work->scrol_type = 6;
                                work->item_anim[3] = 6;
                            }
                        }
                        if (UMN_MAIL_PAD->half_30 & 0x80) {
                            if (work->item_count != 0) {
                                work->column_index = work->column_index + 1;
                                if ((int)work->item_count - 1 < (signed char)work->column_index - 1) {
                                    work->column_index = 1;
                                }
                                work->scrol_type = 7;
                            }
                        }
                    }
                }
                kind = work->scrol_type;
                switch (kind) {
                case 1:
                    work->scroll_x = work->scroll_x - 8;
                    if ((short)work->scroll_x < 48) {
                        work->scroll_x = 48;
                    }
                    quotient = ((short)work->scroll_x - 48) / (short)work->cursor_pitch;
                    work->scrol_type = 0;
                    work->now_page = quotient;
                    break;
                case 2:
                    work->scroll_x = work->scroll_x + 8;
                    if ((short)work->scroll_x > (short)work->scroll_end - (short)work->cursor_pitch) {
                        work->scroll_x = work->scroll_end - work->cursor_pitch;
                    }
                    quotient = ((short)work->scroll_x - 48) / (short)work->cursor_pitch;
                    work->scrol_type = 0;
                    work->now_page = quotient;
                    break;
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                    cur = work->scroll_x;
                    switch (work->scrol_type) {
                    case 3:
                    case 4:
                        target = work->now_page * work->cursor_pitch + 48;
                        if ((short)work->scroll_end - (short)work->cursor_pitch < target) {
                            target = work->scroll_end - work->cursor_pitch;
                        }
                        break;
                    case 5:
                        target = 48;
                        break;
                    case 6:
                        target = work->scroll_end - work->cursor_pitch;
                        break;
                    case 7:
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
                        if (work->scrol_type != 4) {
                            quotient = ((int)cur - 48) / (short)work->cursor_pitch;
                            work->now_page = quotient;
                        }
                        work->scrol_type = 0;
                    }
                    break;
                default:
                    break;
                }
                break;
            case 33:
                work->step = 50;
                break;
            case 81:
                work->state = 0;
                work->step = 60;
                break;
            case 128:
                work->step = 50;
                break;
            default:
                UmnWork.transition_pending = 0;
                break;
            }
            break;
        case 50:
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
                work->step = 0;
                work->state = 0;
            }
            break;
        case 60:
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
            work->step = 70;
            work->flag_06 = 0;
            work->flag_07 = 0;
            break;
        case 70:
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
                work->step = 80;
            }
            if (work->shown_entry != UmnWork.name_lookup) {
                work->state = 0;
                work->step = 60;
            }
            break;
        case 80:
            switch (UmnWork.tab_state) {
            case 33:
                work->step = 50;
                break;
            case 65:
                work->state = 0;
                work->step = 20;
                break;
            default:
                if (work->shown_entry != UmnWork.name_lookup) {
                    work->state = 0;
                    work->step = 60;
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
            work->window.color = work->pos_kind;
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
            endPrintExtFunc(0, 14, &work->tex_addr);
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
            endPrintExtFunc(0, 15, 0);
        }
        break;
    default:
        break;
    }

    xglFontDebugPrintf(16, 128, D_00A12848, work->scrol_type);
    xglFontDebugPrintf(16, 136, D_00A12860, work->now_page);
}

extern char text_2[];
typedef struct UmnMailHensinWork {
    unsigned char step;
    unsigned char state;
    unsigned char unmodeled_02[2];
    short anchor_x;
    short anchor_y;
    int anchor_color;
    UmnMailPasWindow window;
    unsigned char unmodeled_28[0x1a0 - 0x28];
    EMessageParam message;
    EMessageParam messages[3];
    ECursolMainData cursor;
} UmnMailHensinWork;

void tskUmnMailHensin(UmnMailPasTask *task, UmnMailHensinWork *work)
{
    short goal[2];
    unsigned char mail_mode;
    int i;
    UmnMailHeaderPart *header;

    mail_mode = UmnWork.mail_mode;
    if (mail_mode != 1) {
        task->command = -1;
        return;
    }
    switch (task->command) {
    case 0:
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
        work->window.state = 3;
        eCursolSet(&work->cursor, 0);
        work->state = 0;
        work->step = 0;
        break;
    case 2:
        switch (work->step) {
        case 0:
            work->state = 0;
            work->step = 10;
            eCursolModeChange(&work->cursor, 112);
        case 10:
            if (UmnWork.tab_state == 81) {
                work->step = 20;
                work->state = 1;
                eCursolModeChange(&work->cursor, 32);
            }
            break;
        case 20:
            goal[0] = 16;
            MoveSlide(&work->anchor_x, &goal[0], 3.0f);
            if (work->anchor_x == goal[0]) {
                work->step = 30;
            }
            break;
        case 30:
            if ((UmnWork.tab_state >> 4) != 5) {
                work->step = 40;
            } else {
                UmnWork.transition_pending = 0;
            }
            break;
        case 40:
            goal[1] = 528;
            MoveSlide(&work->anchor_x, &goal[1], 3.0f);
            if (work->anchor_x == goal[1]) {
                work->step = 0;
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
    work->window.color = work->anchor_color;
    WindowDXMain(&work->window);
    eMessageSet(&work->message, text_2);
    work->message.x = work->window.x + 19;
    work->message.y = work->window.y + 3;
    work->message.draw_parameter = work->window.color + 2;
    eMessageModeChange(&work->message, 32);
    eMessageMain(&work->message);
    for (i = 0; i < UmnWork.name_char_count; i++) {
        header = UmnMailHeaderGet(UmnWork.name[i]);
        eMessageSet(&work->messages[i], header->subject);
        work->messages[i].x = work->window.x + 59;
        work->messages[i].draw_parameter = work->window.color + 2;
        work->messages[i].y = work->window.y + i * 24 + 3;
        eMessageModeChange(&work->messages[i], 32);
        eMessageMain(&work->messages[i]);
    }
    work->cursor.x = work->window.x + 3;
    work->cursor.y = work->messages[UmnWork.mail_cursor].y + 4;
    work->cursor.work = work->window.color + 2;
    eCursolMain(&work->cursor);
}


typedef struct UmnMailMenuWindow {
    short x;
    short y;
    int style;
    unsigned short width;
    short height;
    const char *tag;
    unsigned char state;
    unsigned char unmodeled_11[3];
    void (*callback)(void);
    void *callback_arg;
} UmnMailMenuWindow;
typedef struct UmnMailMenuSelect {
    unsigned char unmodeled_00[2];
    short index;
    int value;
    const char *label_b;
    const char *label_a;
} UmnMailMenuSelect;
typedef struct UmnMailMenuEntry {
    const char *label;
    unsigned char unmodeled_04[4];
    unsigned char flag;
    unsigned char unmodeled_09[3];
} UmnMailMenuEntry;
typedef struct UmnMailMenuList {
    unsigned char unmodeled_00[2];
    short index;
    int value;
    unsigned char unmodeled_08[8];
    UmnMailMenuEntry *entries;
} UmnMailMenuList;
typedef struct UmnMailMenuWork {
    unsigned char state;
    unsigned char flag;
    unsigned char unmodeled_02[2];
    short origin_x;
    short origin_y;
    int kind;
    int window1_visible;
    UmnMailMenuWindow window1;
    unsigned char unmodeled_2c[0x1a4 - 0x2c];
    UmnMailMenuSelect select1;
    unsigned char unmodeled_1b4[0x79c - 0x1b4];
    int window2_visible;
    UmnMailMenuWindow window2;
    unsigned char unmodeled_7bc[0x934 - 0x7bc];
    UmnMailMenuList select2;
    unsigned char unmodeled_948[0xf2c - 0x948];
    UmnMailMenuEntry entries[4];
} UmnMailMenuWork;
extern void MenuSelectWindow(void);
extern const char D_00A12AC8[];
extern const char D_00A12AD0[];
extern const char D_00A12AD8[];
extern const char *text2_4[4];
extern const char *text_3_00A10650[2];

void tskUmnMailMenu(UmnMailPasTask *task, UmnMailMenuWork *work)
{
    int i;

    if (UmnWork.mail_mode != 1) {
        task->command = -1;
        return;
    }

    switch (task->command) {
    case 0:
        work->origin_x = 528;
        work->origin_y = 48;
        work->kind = 0x00fffff0;
        work->window1_visible = 0;
        WindowDXSet(&work->window1);
        work->window1.style = work->kind;
        work->window1.tag = D_00A12AC8;
        work->window1.width = 169;
        work->window1.height = 86;
        work->select1.label_b = text_3_00A10650[1];
        work->window1.callback_arg = &work->select1;
        work->select1.index = 3;
        work->select1.label_a = text_3_00A10650[0];
        work->window1.callback = MenuSelectWindow;
        work->window2_visible = 0;
        WindowDXSet(&work->window2);
        work->window2.style = work->kind;
        work->window2.width = 125;
        work->window2.height = 102;
        work->select2.index = 2;
        work->window2.tag = D_00A12AD0;
        work->window2.callback = MenuSelectWindow;
        work->window2.callback_arg = &work->select2;
        work->state = 0;
        work->flag = 0;
        break;

    case 2:
        switch (work->state) {
        case 0:
            work->flag = 1;
            work->state = 10;
            /* fall through */
        case 10:
            switch (UmnWork.tab_state) {
            case 49:
                for (i = 0; i < 4; i++) {
                    work->entries[i].label = text2_4[i];
                    work->entries[i].flag = 0;
                }
                if (UmnWork.mail_selection_state != 0) {
                    work->entries[1].flag = 1;
                }
                if (UmnWork.mail_has_data != 0 && UmnWork.mail_text_ready != 0) {
                    work->entries[2].flag = 0;
                } else {
                    work->entries[2].flag = 1;
                }
                work->window2.x = -125;
                work->window2.y = 176;
                work->select2.entries = work->entries;
                work->window2_visible = 1;
                work->window2.state = 1;
                work->select2.value = 0;
                WindowDXMain(&work->window2);
                work->window2.state = 3;
                work->state = 20;
                break;
            case 83:
                work->window1.x = 528;
                work->window1.y = 40;
                work->window1_visible = 1;
                work->window1.state = 1;
                work->select1.value = 0;
                WindowDXMain(&work->window1);
                work->window1.state = 3;
                work->state = 50;
            }
            break;
        case 20: {
            short target = 16;
            MoveSlide(&work->window2.x, &target, 3.0f);
            if (work->window2.x == target) {
                work->state = 30;
            }
            break;
        }
        case 30:
            work->select2.value = UmnWork.command_cursor;
            if (UmnWork.tab_state != 49) {
                work->state = 40;
            } else {
                UmnWork.transition_pending = 0;
            }
            break;
        case 40: {
            short target = -125;
            MoveSlide(&work->window2.x, &target, 3.0f);
            if (work->window2.x == target) {
                work->state = 0;
                work->window2_visible = 0;
            }
            break;
        }
        case 50: {
            short target = 496 - work->window1.width;
            MoveSlide(&work->window1.x, &target, 3.0f);
            if (work->window1.x == target) {
                work->state = 60;
            }
            break;
        }
        case 60:
            work->select1.value = UmnWork.confirm_cursor;
            if (UmnWork.tab_state != 83) {
                work->state = 70;
            } else {
                UmnWork.transition_pending = 0;
            }
            break;
        case 70: {
            short target = 528;
            MoveSlide(&work->window1.x, &target, 3.0f);
            if (work->window1.x == target) {
                work->state = 0;
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
    xglFontDebugPrintf(0, 40, D_00A12AD8, work->state);
}


typedef struct UmnMailExMessage {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x30 - 0x0c];
} UmnMailExMessage;
typedef struct UmnMailExItemBody {
    short window_x;
    short window_y;
    int window_color;
    unsigned short slide_x;
    unsigned short slide_y;
    UmnMailExMessage message;
} UmnMailExItemBody;
typedef struct UmnMailExItem {
    unsigned char unmodeled_00;
    unsigned char state;
    unsigned char unmodeled_02[2];
    UmnMailExItemBody body;
    unsigned char unmodeled_40[8];
} UmnMailExItem;
typedef struct UmnMailExTag {
    unsigned char unmodeled_00[4];
    unsigned short x;
    unsigned short y;
    int color;
    unsigned char unmodeled_0c[0x20 - 0x0c];
} UmnMailExTag;
typedef struct UmnMailExRibbon {
    short x;
    short y;
    int style;
    short field_08;
    short field_0a;
    unsigned char unmodeled_0c[0x70 - 0x0c];
} UmnMailExRibbon;
typedef struct UmnMailExNumber {
    short x;
    short y;
    int value;
    unsigned char unmodeled_08[6];
    unsigned char field_0e;
    unsigned char field_0f;
    unsigned char unmodeled_10[4];
    int field_14;
    unsigned char unmodeled_18[0x90 - 0x18];
} UmnMailExNumber;
typedef struct UmnMailExRow {
    unsigned short slide_x;
    unsigned short slide_y;
    UmnMailExTag tag;
    UmnMailExRibbon ribbon;
    UmnMailExNumber number;
} UmnMailExRow;
typedef struct UmnMailExWork {
    UmnMailExItem items[3];
    unsigned char unmodeled_d8[12];
    UmnMailExRow rows[2];
} UmnMailExWork;
extern void eRibbonSet(void *ribbon, int value);
extern void eRibbonMain(void *ribbon);
extern void eTagFontSet(void *tag, const char *text);
extern void eTagFontMain(void *tag);
extern void eNumberSet(void *number, int value);
extern void eNumberMain(void *number);
extern char *text_5[3];
extern const char D_00A12C60[];

void tskUmnMailExWin(UmnMailPasTask *task, UmnMailExWork *work)
{
    UmnMailExItem *items = work->items;
    int i;

    if (UmnWork.mail_mode != 1) {
        task->command = -1;
        return;
    }
    switch (task->command) {
    case 0:
        items[0].body.window_x = 528;
        items[0].body.window_y = 32;
        items[0].body.window_color = 0x00fffff0;
        for (i = 0; i < 3; i++) {
            items[i].body.slide_x = 528;
            items[i].body.slide_y = 16;
            eMessageSet(&items[i].body.message, text_5[i]);
            eMessageModeChange(&items[i].body.message, 32);
        }
        for (i = 0; i < 2; i++) {
            work->rows[i].slide_x = -272;
            work->rows[i].slide_y = 48;
            eRibbonSet(&work->rows[i].ribbon, 3);
            work->rows[i].ribbon.field_08 = 256;
            work->rows[i].ribbon.field_0a = 24;
            eTagFontSet(&work->rows[i].tag, D_00A12C60);
            eNumberSet(&work->rows[i].number, 0);
        }
        items[0].unmodeled_00 = 0;
        items[0].state = 0;
        break;

    case 2: {
        short slots[3];
        short targets[3];

        for (i = 0; i < 3; i++) {
            slots[i] = 528;
            targets[i] = -272;
        }
        switch (UmnWork.tab_state) {
        case 32:
        case 33:
        case 48:
        case 49:
        case 96:
        case 97:
            targets[0] = 0;
            break;
        case 64:
        case 65: {
            unsigned char *text = (unsigned char *)text_5[1];
            unsigned char *shade_bars = text + 1;
            unsigned char *cell = text + 15;

            if (UmnWork.header_gate_flag) {
                shade_bars[2] = -128;
                shade_bars[1] = -128;
                shade_bars[0] = -128;
            } else {
                shade_bars[2] = 64;
                shade_bars[1] = 64;
                shade_bars[0] = 64;
            }
            if (UmnWork.selected_mail_id == 30 && !(UmnMailDataGet(30)[0] & 0x30)) {
                cell[2] = 64;
                cell[1] = 64;
                cell[0] = 64;
            } else {
                cell[2] = -128;
                cell[1] = -128;
                cell[0] = -128;
            }
            slots[1] = 272;
            break;
        }
        case 80:
        case 81: {
            unsigned char *cell = (unsigned char *)text_5[2] + 1;

            if (UmnWork.price_balance >= 0) {
                cell[2] = -128;
                cell[1] = -128;
                cell[0] = -128;
            } else {
                cell[2] = 64;
                cell[1] = 64;
                cell[0] = 64;
            }
            slots[2] = 272;
            break;
        }
        default:
            break;
        }

        for (i = 0; i < 3; i++) {
            MoveSlide((short *)&work->items[i].body.slide_x, &slots[i], 3.0f);
            work->items[i].body.message.x = work->items[i].body.slide_x;
            work->items[i].body.message.y = work->items[i].body.slide_y;
            work->items[i].body.message.color = items[0].body.window_color;
            eMessageMain(&work->items[i].body.message);
        }
        for (i = 0; i < 2; i++) {
            MoveSlide((short *)&work->rows[i].slide_x, &targets[i], 3.0f);
            work->rows[i].ribbon.x = work->rows[i].slide_x;
            work->rows[i].ribbon.y = work->rows[i].slide_y;
            work->rows[i].ribbon.style = items[0].body.window_color;
            eRibbonMain(&work->rows[i].ribbon);
            work->rows[i].tag.x = work->rows[i].slide_x + 188;
            work->rows[i].tag.color = work->rows[i].ribbon.style + 1;
            work->rows[i].tag.y = work->rows[i].slide_y + 4;
            eTagFontMain(&work->rows[i].tag);
            work->rows[i].number.x = work->rows[i].tag.x + 30;
            work->rows[i].number.y = work->rows[i].tag.y;
            work->rows[i].number.value = work->rows[i].tag.color;
            work->rows[i].number.field_0e = 3;
            work->rows[i].number.field_14 = UmnWork.folder_entry_count * 65536
                                          + UmnWork.selection_index + 1;
            work->rows[i].number.field_0f = 2;
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
        signed char *children = UmnHistoryTreeGet(mail_id);

        if (children == 0) {
            return 0;
        }
        do {
            int child_id = children[child_index];

            child_index++;
            if (child_id < 0) {
                return 0;
            }
            if (child_id == UmnWork.selected_mail_id || SisoSiso(child_id)) {
                return 1;
            }
        } while (child_index < 3);
        return 0;
    }

    int i;
    unsigned char *folder;
    signed char *history;
    signed char *ids;

    if (UmnWork.mail_selection_state != 0) {
        int selected;

        UmnWork.saved_selection_index = UmnWork.selection_index;
        UmnWork.saved_scroll_offset = UmnWork.scroll_offset;
        selected = UmnWork.selected_mail_id;
        UmnWork.saved_visible_folder_index = UmnWork.visible_folder_index;
        folder = mail_folder;

        /* The root folder is the first mail whose history holds the selection. */
        for (i = 0; i < 81; i++) {
            if (SisoSiso(i)) {
                selected = i;
                break;
            }
        }
        *folder++ = selected;
        UmnWork.folder_entry_count = 1;

        /* Follow the first readable child of every level down the tree. */
        history = UmnHistoryTreeGet(selected);
        while (history != 0) {
            for (i = 0; i < 3; i++) {
                if (history[i] < 0) {
                    continue;
                }
                if ((UmnMailDataGet(history[i])[0] & 1) == 0) {
                    continue;
                }
                UmnWork.folder_entry_count++;
                *folder++ = history[i];
                history = UmnHistoryTreeGet(history[i]);
                break;
            }
            if (i == 3) {
                break;
            }
        }

        for (i = 0; i < UmnWork.folder_entry_count; i++) {
            if (mail_folder[i] == UmnWork.selected_mail_id) {
                break;
            }
        }
        UmnWork.selection_index = i;
        if (i >= 4) {
            UmnWork.visible_folder_index = 3;
            UmnWork.scroll_offset = i - 3;
        } else {
            UmnWork.visible_folder_index = i;
        }
    } else {
        int entry;

        UmnWork.folder_entry_count = 0;
        for (ids = D_4A1A0C.mail_ids; *ids != -1; ids++) {
            UmnWork.folder_entry_count++;
        }
        /* The list is stored newest first; the folder shows it oldest first. */
        for (entry = 0; entry < UmnWork.folder_entry_count; entry++) {
            mail_folder[entry] = *--ids;
        }
        UmnWork.scroll_offset = UmnWork.saved_scroll_offset;
        UmnWork.visible_folder_index = UmnWork.saved_visible_folder_index;
        UmnWork.selection_index = UmnWork.saved_selection_index;
        UmnWork.selected_mail_id = mail_folder[(signed char)UmnWork.saved_selection_index];
    }
}

struct UmnMailEdgePair {
    signed char edge[2];
};

void UmnMail(int menu_id)
{
    int i;

    switch (UmnWork.tab_state) {
    case 0: {
        int task_base;

        task_base = (int)UmnWorkEnd;
        task_base = (task_base + 15) & -16;
        UmnObjectTaskCreate(tskUmnMailPas, task_base);
        task_base = (task_base + 983) & -16;
        UmnObjectTaskCreate(tskUmnMailInfo, task_base);
        task_base = (task_base + 1119) & -16;
        UmnObjectTaskCreate(tskUmnMailFolder, task_base);
        task_base = (task_base + 4431) & -16;
        UmnObjectTaskCreate(tskUmnMailHensin, task_base);
        task_base = (task_base + 739) & -16;
        UmnObjectTaskCreate(tskUmnMailMenu, task_base);
        task_base = (task_base + 3959) & -16;
        UmnObjectTaskCreate(tskUmnMailExWin, task_base);
        task_base = (task_base + 827) & -16;
        UmnObjectTaskCreate(tskUmnMailDisp, task_base);
        UmnEventTextInit((unsigned char *)(task_base + 800));
        if (menu_id == 1) {
            UmnWork.tab_state = 53;
            UmnWork.wait_timer = 2;
        } else {
            UmnWork.tab_state = 32;
        }
        UmnMailFolderSet();
        if (UmnWork.folder_entry_count == 0) {
            UmnChangeTopLevel(0);
            return;
        }
        break;
    }
    case 16:
        UmnWork.transition_pending = 1;
        UmnWork.tab_state = 17;
        /* fall through */
    case 17:
        if (UmnWork.transition_pending == 0) {
            UmnWork.tab_state = 32;
            UmnWork.mail_selection_state ^= 1;
        }
        break;
    case 32:
        UmnWork.tab_state = 33;
        UmnWork.transition_pending = 1;
        do {
            UmnWork.scroll_direction = 0;
        } while (0);
        UmnWork.wait_timer = 0;
        UmnMailFolderSet();
        /* fall through */
    case 33: {
        if (UmnWork.transition_pending == 0) {
        if (UmnWork.wait_timer == 0) {
            signed char scroll_dir = 0;

            if ((UMN_MAIL_PAD->half_30 & 0x1000) != 0) {
                if (UmnWork.selection_index == 0) {
                    if ((UMN_MAIL_PAD->half_32 & 0x1000) != 0) {
                        scroll_dir = -1;
                    }
                } else if (UmnWork.visible_folder_index == 0) {
                    scroll_dir = -1;
                } else if ((UMN_MAIL_PAD->half_34 & 0x1000) != 0) {
                    scroll_dir = -1;
                }
            } else if ((UMN_MAIL_PAD->half_30 & 0x4000) != 0) {
                if (UmnWork.selection_index == UmnWork.folder_entry_count - 1) {
                    if ((UMN_MAIL_PAD->half_32 & 0x4000) != 0) {
                        scroll_dir = 1;
                    }
                } else if (UmnWork.visible_folder_index == 3) {
                    scroll_dir = 1;
                } else {
                    {
                        int right_bits = UMN_MAIL_PAD->half_34 & 0x4000;

                        scroll_dir = right_bits != 0;
                    }
                }
            }

            if (scroll_dir != 0) {
                int last_index = (unsigned char)UmnWork.folder_entry_count - 1;
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
                if (vis_edges.edge[1] >= 4) {
                    vis_edges.edge[1] = 3;
                }
                UmnWork.wait_timer = 4;
                if (UmnWork.selection_index == sel_edges.edge[idx]) {
                    UmnWork.selection_index = sel_edges.edge[idx ^ 1];
                } else {
                    UmnWork.selection_index += scroll_dir;
                }
                if (UmnWork.visible_folder_index == vis_edges.edge[idx]) {
                    if (UmnWork.folder_entry_count >= 5) {
                        UmnWork.scroll_direction = scroll_dir;
                        UmnWork.scroll_offset += scroll_dir;
                    } else {
                        UmnWork.visible_folder_index = sel_edges.edge[idx ^ 1];
                    }
                } else {
                    UmnWork.visible_folder_index += scroll_dir;
                }
                xglSoundEffectNormalID(3, 0);
            } else {
                if (UMN_MAIL_PAD->half_2a == 0x20) {
                    UmnWork.tab_state = 48;
                    UmnWork.command_cursor = 0;
                    xglSoundEffectNormalID(1, 0);
                }
                if (UMN_MAIL_PAD->half_2a == 0x40) {
                    if (UmnWork.mail_selection_state != 0) {
                        UmnWork.tab_state = 16;
                        xglSoundEffectNormalID(1, 0);
                    } else {
                        UmnWork.tab_state = -16;
                        xglSoundEffectNormalID(2, 0);
                    }
                }
            }
        }
        }

        /* refresh the selected mail */
        UmnWork.mail_text_ready = 0;
        UmnWork.mail_has_data = 0;
        UmnWork.selected_mail_id = (signed char)mail_folder[UmnWork.selection_index];
        {
            UmnMailHeaderPart *header;
            unsigned char *data;

            header = UmnMailHeaderGet(UmnWork.selected_mail_id);
            if (header->attach != 0) {
                data = UmnMailDataGet(UmnWork.selected_mail_id);
                UmnWork.mail_has_data = 1;
                if ((data[1] & 0x80) == 0) {
                    UmnWork.mail_text_ready = 1;
                }
            }
        }
        break;
    }
    case 48:
        UmnWork.tab_state = 49;
        UmnWork.transition_pending = 1;
        /* fall through */
    case 49:
        if (UmnWork.transition_pending == 0) {
            int new_cursor;

            new_cursor = MenuSelectMove(UmnWork.command_cursor, 4, 0);
            UmnWork.command_cursor = new_cursor;
            if (UMN_MAIL_PAD->half_34 == 0x20) {
                switch ((signed char)new_cursor) {
                case 0:
                    UmnWork.tab_state = 53;
                    UmnWork.wait_timer = 2;
                    xglSoundEffectNormalID(1, 0);
                    break;
                case 1: {
                    int sound_id;

                    if (UmnWork.mail_selection_state != 0) {
                        sound_id = 5;
                    } else {
                        UmnWork.tab_state = 16;
                        sound_id = 1;
                    }
                    xglSoundEffectNormalID(sound_id, 0);
                    break;
                }
                case 3:
                    UmnWork.tab_state = 33;
                    UmnWork.wait_timer = 8;
                    UmnWork.transition_pending = 1;
                    xglSoundEffectNormalID(2, 0);
                    break;
                case 2:
                    if (UmnWork.mail_has_data != 0 && UmnWork.mail_text_ready != 0) {
                        UmnWork.tab_state = 96;
                        xglSoundEffectNormalID(1, 0);
                    } else {
                        xglSoundEffectNormalID(5, 0);
                    }
                    break;
                default:
                    break;
                }
            }
            if (UMN_MAIL_PAD->half_34 == 0x40) {
                UmnWork.tab_state = 33;
                UmnWork.wait_timer = 8;
                UmnWork.transition_pending = 1;
                xglSoundEffectNormalID(2, 0);
            }
        }
        break;
    case 53:
        if (UmnWork.wait_timer == 0) {
            UmnWork.tab_state = 64;
            if (menu_id == 1) {
                UmnWork.wait_timer = 8;
            }
        }
        break;
    case 64: {
        unsigned char *data;
        UmnMailHeaderPart *header;
        UmnKosmosCounters *kosmos;
        unsigned char *src;
        unsigned char *dst;
        unsigned char name_char;
        int name_index;
        UmnWork.tab_state = 65;
        UmnWork.transition_pending = 1;
        header = UmnMailHeaderGet(UmnWork.selected_mail_id);
        UmnWork.name_char_count = 0;
        src = header->text;
        for (name_index = 0; name_index < 3; name_index++) {
            name_char = *src++;
            if (name_char != 0) {
                UmnWork.name[name_index] = name_char;
                UmnWork.name_char_count++;
            } else {
                UmnWork.name[name_index] = 0xff;
            }
        }
        UmnWork.mail_cursor = 0;
        UmnWork.name_lookup = UmnWork.name[0];
        if (UmnMailEventCheck(UmnWork.selected_mail_id) != 0) {
            UmnWork.name_char_count = 0;
        }
        data = UmnMailDataGet(UmnWork.selected_mail_id);
        if ((data[0] & 0x02) == 0) {
            header = UmnMailHeaderGet(UmnWork.selected_mail_id);
            kosmos = &D_4A1A0C;
            for (i = 0; i < 5; i++) {
                int item = header->kosmos_items[i];

                if (item != 0) {
                    kosmos->kosmos_items[i] += item;
                }
            }
            UmnKosmosSpecialGetCheck(UmnWork.selected_mail_id);
        }
        {
            unsigned char *data;
            UmnMailHeaderPart *header;

            data = UmnMailDataGet(UmnWork.selected_mail_id);
            header = UmnMailHeaderGet(UmnWork.selected_mail_id);
            data[0] |= 0x02;
            UmnWork.header_gate_flag = 0;
            if ((data[0] & 0x08) == 0 && UmnWork.name_char_count != 0 && (data[0] & 0x30) == 0) {
                unsigned short limit = header->scenario_limit;

                if (MenuScenarioNoGet() < limit) {
                    UmnWork.header_gate_flag = 1;
                }
            }
        }
        {
            UmnMailHeaderPart *header;
            unsigned char *data;

            header = UmnMailHeaderGet(UmnWork.selected_mail_id);
            data = UmnMailDataGet(UmnWork.selected_mail_id);
            data[1] |= header->attach;
            if (UmnMailCompulsionDownLoadCheck(UmnWork.selected_mail_id) == 1) {
                data[1] |= 0x80;
                UmnMailAttachSet(header->attach);
            }
        }
        /* fall through */
    }
    case 65: {
        unsigned char *data;
        if (UmnWork.transition_pending == 0 && UmnWork.wait_timer == 0) {
            if (UMN_MAIL_PAD->half_2a == 0x40) {
                int ev;

                ev = UmnMailEventCheck(UmnWork.selected_mail_id);
                if (ev != 0) {
                    data = UmnMailDataGet(UmnWork.selected_mail_id);
                    data[0] |= 0x04;
                    if (UmnManzaiInit(ev) < 0) {
                        UmnManzaiInit(0);
                    }
                    UmnWork.tab_state = -128;
                    xglSoundEffectNormalID(1, 0);
                } else if (UmnWork.selected_mail_id == 30) {
                    data = UmnMailDataGet(30);
                    if ((data[0] & 0x30) != 0) {
                        UmnWork.tab_state = 33;
                        UmnWork.transition_pending = 1;
                        xglSoundEffectNormalID(2, 0);
                    } else {
                        xglSoundEffectNormalID(5, 0);
                    }
                } else {
                    UmnWork.tab_state = 33;
                    UmnWork.transition_pending = 1;
                    xglSoundEffectNormalID(2, 0);
                }
            }
            if (UMN_MAIL_PAD->half_2a == 0x20) {
                if (UmnWork.header_gate_flag != 0) {
                    UmnWork.tab_state = 80;
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            }
        }
        break;
    }
    case 80: {
        UmnMailHeaderPart *header;
        header = UmnMailHeaderGet(UmnWork.selected_mail_id);
        UmnWork.price_balance = header->cost;
        if (dataMoneyBoxChk() < header->cost) {
            UmnWork.price_balance = dataMoneyBoxChk() - header->cost;
        }
        UmnWork.transition_pending = 1;
        UmnWork.mail_cursor = 0;
        UmnWork.wait_timer = 16;
        UmnWork.tab_state = 81;
        /* fall through */
    }
    case 81:
        if (UmnWork.transition_pending == 0 && UmnWork.wait_timer == 0) {
            int new_cursor;

            new_cursor = MenuSelectMove(UmnWork.mail_cursor, UmnWork.name_char_count, 0);
            if (new_cursor != UmnWork.mail_cursor) {
                UmnWork.mail_cursor = new_cursor;
                UmnWork.wait_timer = 8;
            } else if ((UMN_MAIL_PAD->half_2a & 0x20) != 0) {
                if (UmnWork.price_balance >= 0) {
                    UmnWork.tab_state = 82;
                    xglSoundEffectNormalID(1, 0);
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
            } else if ((UMN_MAIL_PAD->half_2a & 0x40) != 0) {
                UmnWork.tab_state = 65;
                xglSoundEffectNormalID(2, 0);
            }
        }
        UmnWork.name_lookup = UmnWork.name[UmnWork.mail_cursor];
        break;
    case 82:
        if (UmnWork.mail_selection_state == 0) {
            UmnWork.saved_visible_folder_index = 0;
            UmnWork.saved_scroll_offset = 0;
            UmnWork.saved_selection_index = 0;
        }
        UmnWork.confirm_cursor = 0;
        UmnWork.tab_state = 83;
        UmnWork.transition_pending = 1;
        /* fall through */
    case 83:
        if (UmnWork.transition_pending == 0) {
            int new_cursor;

            new_cursor = MenuSelectMove(UmnWork.confirm_cursor, 2, 0);
            UmnWork.confirm_cursor = new_cursor;
            if (UMN_MAIL_PAD->half_2a == 0x20) {
                if ((signed char)new_cursor != 0) {
                    UmnWork.tab_state = 81;
                    xglSoundEffectNormalID(2, 0);
                } else {
                    UmnWork.tab_state = 84;
                    xglSoundEffectNormalID(1, 0);
                }
            }
            if (UMN_MAIL_PAD->half_2a == 0x40) {
                UmnWork.tab_state = 81;
                xglSoundEffectNormalID(2, 0);
            }
        }
        break;
    case 84: {
        unsigned char *data;
        data = UmnMailDataGet(UmnWork.selected_mail_id);
        data[0] |= (unsigned char)((UmnWork.mail_cursor + 1) << 4);
        {
            unsigned char *name_data;

            name_data = UmnMailDataGet(UmnWork.name_lookup);
            name_data[0] |= 0x09;
        }
        UmnMailBoxSet(UmnWork.name_lookup);
        if (UmnWork.price_balance > 0) {
            dataMoneyBoxDec(UmnWork.price_balance);
        }
        UmnWork.tab_state = 85;
        UmnWork.wait_timer = 2;
        /* fall through */
    }
    case 85:
        if (UmnWork.wait_timer == 0 && UMN_MAIL_PAD->half_2a == 0x20) {
            UmnWork.tab_state = 32;
            xglSoundEffectNormalID(1, 0);
        }
        break;
    case 96: {
        unsigned char *data;
        UmnMailHeaderPart *header;
        header = UmnMailHeaderGet(UmnWork.selected_mail_id);
        data = UmnMailDataGet(UmnWork.selected_mail_id);
        data[1] |= 0x80;
        data[1] |= header->attach;
        UmnMailAttachSet(header->attach);
        UmnWork.wait_timer = 2;
        UmnWork.tab_state = 97;
        /* fall through */
    }
    case 97:
        if (UmnWork.wait_timer == 0 && UMN_MAIL_PAD->half_2a == 0x20) {
            UmnWork.tab_state = 33;
            xglSoundEffectNormalID(1, 0);
        }
        break;
    case 128:
        if (UmnManzaiControl() == 0) {
            UmnWork.tab_state = 53;
            if (UmnEventEndMailCheck(UmnWork.selected_mail_id) != 0) {
                UmnMailFolderSet();
            }
        }
        break;
    case 240:
        UmnWork.tab_state = 241;
        UmnWork.transition_pending = 1;
        /* fall through */
    case 241:
        if (UmnWork.transition_pending == 0) {
            UmnChangeTopLevel(0);
        }
        break;
    }

    if (UmnWork.wait_timer != 0) {
        UmnWork.wait_timer--;
    }
    xglFontDebugPrintf(0, 80, D_00A12D78, (unsigned char)UmnWork.tab_state);
    xglFontDebugPrintf(0, 88, D_00A12D80, UmnWork.selection_index, UmnWork.folder_entry_count);
    xglFontDebugPrintf(0, 96, D_00A12D88, UmnWork.scroll_direction);
}




const char D_00A12240[16] = "/History";

const char D_00A12250[8] = "/Reply";

const char D_00A12258[8] = "/Read";

const char D_00A12260[8] = "/Folder";

const char D_00A12268[8] = "Mail";

