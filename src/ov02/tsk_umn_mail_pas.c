/*
 * OV02 original TU 9: 0x00a05738..0x00a097d8 (10 functions)
 */
#include "common.h"

#include "shared.h"
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
typedef struct UmnMailPasMessage {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x44 - 0x0c];
} UmnMailPasMessage;
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
    UmnMailPasMessage messages[5];
    unsigned char unmodeled_2f0[0x3bc - 0x2f0];
    UmnMailPasBox box;
} UmnMailPasWork;
typedef struct UmnMailState {
    unsigned char unmodeled_00;
    unsigned char mail_mode;
    unsigned char unmodeled_02;
    unsigned char tab_state;
    unsigned char unmodeled_04[0x42 - 4];
    unsigned char visible_folder_index;
    unsigned char unmodeled_43;
    unsigned char scroll_offset;
    unsigned char unmodeled_45[0x0b];
    unsigned char selection_index;
    signed char folder_entry_count;
    signed char selected_mail_id;
    unsigned char unmodeled_53[9];
    signed char mail_selection_state;
    unsigned char saved_selection_index;
    unsigned char saved_scroll_offset;
    unsigned char saved_visible_folder_index;
} UmnMailState;
extern UmnMailState UmnWork;
extern const char *msg00_0_00A10618[5];
extern int MenuPasLengthGet(const char *text);
extern void WindowDXSet(void *window);
extern void WindowDXMain(void *window);
extern void MoveSlide(short *current, short *target, float rate);
extern void eMessageSet(void *message, const char *text);
extern void eMessageMain(void *message);
extern void endPrintExtFunc(int kind, int id, void *data);

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
                work->messages[i].color = work->kind + 2;
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

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_mail_pas", tskUmnMailInfo);

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_mail_pas", tskUmnMailFolder);

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_mail_pas", tskUmnMailDisp);

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_mail_pas", tskUmnMailHensin);

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_mail_pas", tskUmnMailMenu);

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_mail_pas", tskUmnMailExWin);

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_mail_pas", SisoSiso_6);

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_mail_pas", UmnMailFolderSet);

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_mail_pas", UmnMail);
