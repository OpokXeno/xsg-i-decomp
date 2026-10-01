/*
 * OV02 original TU 13: 0x00a0de20..0x00a0e938 (5 functions)
 */
#include "common.h"
#include "shared.h"

struct UmnPluginState {
    unsigned char unmodeled_00;
    unsigned char plugin_work_state;
    unsigned char unmodeled_02;
    unsigned char plugin_mode;
    unsigned char unmodeled_04[0x0c];
    signed char transition_timer;
    unsigned char unmodeled_11[0x3f];
    signed char selected_plugin_index;
    signed char unlocked_plugin_count;
    unsigned char selected_plugin_id;
    unsigned char unmodeled_53[0x2d];
};

extern struct UmnPluginState UmnWork;
extern unsigned char *UmnWorkEnd;
extern unsigned char plugin_folder[16];
extern const char D_00A13450[];
extern const char D_00A13540[];
extern unsigned short D_4A1A0C[];

/* PadData's +0x2a halfword records newly pressed controller buttons. */
extern PadPrefix PadData;

struct UmnPluginTask {
    XglTaskPrefix prefix;
    int state;
};

struct UmnPluginInfoWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    unsigned char unmodeled_0c[4];
    unsigned char state;
    unsigned char unmodeled_11[3];
    void (*callback)(void *, void *);
    void *callback_arg;
    unsigned char unmodeled_1c[0x178];
};

struct UmnPluginInfoText {
    short x;
    short y;
    unsigned char unmodeled_04[8];
    char *text;
};

struct UmnPluginInfoWork {
    unsigned char state;
    unsigned char opened;
    unsigned char unmodeled_02[6];
    int color;
    struct UmnPluginInfoWindow window;
    struct UmnPluginInfoText info;
};

struct UmnPluginPasWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    unsigned char unmodeled_0c[4];
    unsigned char state;
};

struct UmnPluginPasLabel {
    unsigned char unmodeled_00[12];
    unsigned char message[4];
    short slide_x;
    short slide_y;
    int accent_color;
};

struct UmnPluginPasRect {
    short x;
    short y;
    int color;
    short width;
    short height;
};

struct UmnPluginPasWork {
    unsigned char unmodeled_00;
    unsigned char initialized;
    unsigned char unmodeled_02[2];
    int color;
    struct UmnPluginPasWindow window;
    unsigned char unmodeled_1c[0x190 - 0x1c];
    struct UmnPluginPasLabel label;
    unsigned char unmodeled_1a8[0x3bc - 0x1a8];
    struct UmnPluginPasRect caption_rect;
};

extern void *msg00_0_00A10C40;
void eMessageSet(void *message, void *text);
void eMessageMain(void *message);
void endPrintExtFunc(int color, int mode, void *rectangle);

void tskUmnPluginPas(void *task, void *work);
void tskUmnPluginInfo(void *task, void *work);
void tskUmnPluginList(void *task, void *work);
void tskUmnPluginExWin(void *task, void *work);
void UmnObjectTaskCreate(void (*task)(void *, void *), void *work);
void UmnChangeTopLevel(int level);
void xglFontDebugPrintf(int x, int y, const char *format, ...);
void xglFontDebugHex(int x, int y, int value, int digits);
void xglSoundEffectNormalID(int sound, int channel);
void WindowDXSet(void *window);
void WindowDXMain(void *window);
void MenuInfoWindow(void *window, void *text);
char *UmnPluginTextGet(signed char plugin_id);
void MoveSlide(short *current, short *target, float rate);

void tskUmnPluginPas(void *task_opaque, void *work_opaque)
{
    struct UmnPluginTask *task = task_opaque;
    struct UmnPluginPasWork *work = work_opaque;

    if (UmnWork.plugin_work_state != 4) {
        task->state = -1;
    } else {
        switch (task->state) {
        case 0: {
            int label_x = 288;
            int label_y = 11;
            int label_font_size = 32;
            int initial_state = 1;
            int panel_color;
            work->color = 0x00FFFFF0;
            WindowDXSet(&work->window);
            work->window.x = -272;
            work->window.color = work->color;
            work->window.y = 8;
            work->window.width = 272;
            work->window.height = label_font_size;
            work->window.state = initial_state;
            WindowDXMain(&work->window);
            work->window.state = 3;
            eMessageSet(work->label.message, msg00_0_00A10C40);
            {
            struct UmnPluginPasLabel *label = &work->label;
            label->message[1] = label_font_size;
            panel_color = work->color;
            label->slide_x = label_x;
            label->slide_y = label_y;
            work->initialized = initial_state;
            }
            work->label.accent_color = panel_color + 2;
            break;
        }
        case 2:
            {
                short slide_targets[2];
                short *label_target;
                int plugin_mode = UmnWork.plugin_mode;
                int caption_mode;

                slide_targets[0] = -16;
                label_target = &slide_targets[1];
                *label_target = 288;
                if (plugin_mode < 18) {
                    if (plugin_mode >= 16) {
                        slide_targets[1] = 16;
                    } else {
                        slide_targets[0] = -272;
                    }
                } else {
                    slide_targets[0] = -272;
                }
                if (work->initialized != 0) {
                    MoveSlide(&work->window.x, &slide_targets[0], 3.0f);
                    WindowDXMain(&work->window);
                    caption_mode = 101;
                    work->caption_rect.x = (unsigned short)work->window.x + 3;
                    work->caption_rect.y = (unsigned short)work->window.y + 3;
                    work->caption_rect.width = work->window.width - 6;
                    work->caption_rect.height = work->window.height - 6;
                    work->caption_rect.color = work->color;
                    endPrintExtFunc(work->color, caption_mode, &work->caption_rect);
                    MoveSlide(&work->label.slide_x, label_target, 3.0f);
                    if (work->label.slide_x < 256) {
                        eMessageMain(work->label.message);
                    }
                    endPrintExtFunc(work->color, 102, 0);
                }
            }
            break;
        }
    }
}

void tskUmnPluginInfo(void *task_opaque, void *work_opaque)
{
    struct UmnPluginTask *task = task_opaque;
    struct UmnPluginInfoWork *work = work_opaque;

    if (UmnWork.plugin_work_state != 4) {
        task->state = -1;
    } else {
        switch (task->state) {
        case 0:
            work->color = 0x00FF0000;
            work->opened = 0;
            work->state = 0;
            WindowDXSet(&work->window);
            work->window.x = -16;
            ((volatile struct UmnPluginInfoWork *)work)->window.state = 0;
            work->window.y = 480;
            work->window.width = 544;
            work->window.height = 54;
            work->window.callback = MenuInfoWindow;
            work->window.color = work->color;
            work->window.callback_arg = &work->info;
            work->info.x = 0;
            work->info.y = 0;
            work->info.text = 0;
            ((volatile struct UmnPluginInfoWork *)work)->window.state = 1;
            WindowDXMain(&work->window);
            work->window.state = 3;
            break;
        case 2:
            {
                short slide_target = 480;
                int plugin_mode = UmnWork.plugin_mode;
                unsigned char plugin_id;

                if (plugin_mode < 18) {
                    if (plugin_mode >= 16) {
                        slide_target = 386;
                    }
                }
                plugin_id = UmnWork.selected_plugin_id;
                if (plugin_id < 6) {
                    work->info.text = UmnPluginTextGet((signed char)plugin_id) + 0x20;
                } else {
                    work->info.text = (char *)D_00A13450;
                }
                MoveSlide(&work->window.y, &slide_target, 3.0f);
                WindowDXMain(&work->window);
            }
            break;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_plugin_pas", tskUmnPluginList);

INCLUDE_ASM("asm/nonmatchings/ov02/tsk_umn_plugin_pas", tskUmnPluginExWin);

void UmnPlugin(void)
{
    switch (UmnWork.plugin_mode) {
    case 0:
        {
        unsigned short unlocked_plugins;
        int plugin_index;
        unsigned char *task_work = UmnWorkEnd;
        unsigned char *folder_cursor;
        unsigned short *plugin_flags;

        UmnObjectTaskCreate(tskUmnPluginPas, task_work);
        task_work += 968;
        UmnObjectTaskCreate(tskUmnPluginInfo, task_work);
        folder_cursor = plugin_folder;
        task_work += 848;
        UmnObjectTaskCreate(tskUmnPluginList, task_work);
        UmnObjectTaskCreate(tskUmnPluginExWin, task_work + 6340);
        plugin_flags = D_4A1A0C;
        UmnWork.unlocked_plugin_count = 0;
        UmnWork.selected_plugin_index = 0;
        memset(folder_cursor, 0, sizeof(plugin_folder));
        unlocked_plugins = plugin_flags[0x43];
        for (plugin_index = 0; plugin_index < 16; plugin_index++) {
            if (((int)unlocked_plugins >> plugin_index) & 1) {
                *folder_cursor++ = plugin_index;
                UmnWork.unlocked_plugin_count++;
            }
        }
        if (UmnWork.unlocked_plugin_count >= 7) {
            UmnWork.unlocked_plugin_count = 6;
        }
        UmnWork.plugin_mode = 16;
        UmnWork.selected_plugin_id = plugin_folder[UmnWork.selected_plugin_index];
        break;
        }
    case 16:
        UmnWork.plugin_mode = 17;
        UmnWork.transition_timer = 12;
        /* Continue through the entry timer check. */
    case 17:
        if (UmnWork.transition_timer == 0 &&
            (PadData.half_2a & 0x40) != 0) {
            UmnWork.plugin_mode = 240;
            xglSoundEffectNormalID(2, 0);
        }
        break;
    case 240:
        UmnWork.plugin_mode = 241;
        UmnWork.transition_timer = 16;
        /* Continue through the exit timer check. */
    case 241:
        if (UmnWork.transition_timer == 0) {
            UmnChangeTopLevel(0);
        }
        break;
    }

    if (UmnWork.transition_timer != 0) {
        UmnWork.transition_timer--;
    }
    xglFontDebugPrintf(32, 208, D_00A13540);
    xglFontDebugHex(0, 64, UmnWork.plugin_mode, 2);
}
