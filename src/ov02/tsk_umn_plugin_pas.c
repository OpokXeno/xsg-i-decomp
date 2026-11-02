/*
 * OV02 original TU 13: 0x00a0de20..0x00a0e938 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/e_message.h"

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
extern const char D_00A13440[];
extern const char D_00A13458[];
extern const char D_00A13460[];
static void *msg00_0_00A10C40 = (void *)D_00A13440;
unsigned char plugin_folder[16] = { 0 };
static const char D_00A13450[8] = "";
/* The original EUC-JP debug label spells プラグイン. */
/* JP: プラグイン
 * EN: Plugin */
static const char D_00A13540[16] =
    "\xA5\xD7\xA5\xE9\xA5\xB0\xA5\xA4\xA5\xF3";
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
    EMessageParam message;
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
    unsigned char unmodeled_1e0[0x3bc - 0x1e0];
    struct UmnPluginPasRect caption_rect;
};

struct UmnPluginListItem {
    const char *text;
    int icon;
    signed char flags;
    unsigned char unmodeled_09[3];
};

struct UmnPluginListWindow {
    unsigned char state;
    unsigned char draw_flags;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int style;
    short width;
    short height;
    const char *title;
    signed char columns;
    signed char rows;
    unsigned char unmodeled_16[6];
    struct UmnPluginListItem *items;
};

struct UmnPluginListWork {
    unsigned char phase;
    unsigned char redraw;
    unsigned char unmodeled_02[2];
    short slide_x;
    short slide_y;
    int color;
    struct UmnPluginListWindow window;
    unsigned char unmodeled_2c[0x1744 - 0x2c];
    struct UmnPluginListItem items[8];
};

struct UmnPluginExWinTag {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int style;
    unsigned char unmodeled_0c[0x20 - 0x0c];
};

struct UmnPluginExWinRibbon {
    short x;
    short y;
    int style;
    short width;
    short height;
    unsigned char unmodeled_0c[0x70 - 0x0c];
};

struct UmnPluginExWinNumber {
    short x;
    short y;
    int style;
    unsigned char unmodeled_08[6];
    unsigned char option;
    unsigned char places;
    unsigned char unmodeled_10[4];
    int value;
    unsigned char unmodeled_18[0x90 - 0x18];
};

struct UmnPluginExWinItem {
    short slide_x;
    short slide_y;
    struct UmnPluginExWinTag tag;
    struct UmnPluginExWinRibbon ribbon;
    struct UmnPluginExWinNumber number;
};

struct UmnPluginExWin {
    unsigned char state;
    unsigned char redraw;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int style;
    struct UmnPluginExWinItem item[1];
};

extern char msg00_1_00A10C48[];

void eRibbonSet(void *ribbon, int style);
void eRibbonMain(void *ribbon);
void eTagFontSet(void *tag, const char *text);
void eTagFontMain(void *tag);
void eNumberSet(void *number, int unused);
void eNumberMain(void *number);

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
char *UmnPluginTextGet(unsigned int plugin_id);
void MoveSlide(short *current, short *target, float rate);
void WindowSPSet(void *cursor);
int WindowSPSelect(void *cursor, int button_flags);
void WindowSPItemChange(void *cursor);
void WindowSPMain(void *cursor);

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
            eMessageSet(&work->label.message, msg00_0_00A10C40);
            {
            struct UmnPluginPasLabel *label = &work->label;
            label->message.mode = label_font_size;
            panel_color = work->color;
            label->message.x = label_x;
            label->message.y = label_y;
            work->initialized = initial_state;
            }
            work->label.message.draw_parameter = panel_color + 2;
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
                    MoveSlide(&work->label.message.x, label_target, 3.0f);
                    if (work->label.message.x < 256) {
                        eMessageMain(&work->label.message);
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

void tskUmnPluginList(void *task_opaque, void *work_opaque)
{
    struct UmnPluginTask *task = task_opaque;
    struct UmnPluginListWork *work = work_opaque;
    int i;

    if (UmnWork.plugin_work_state != 4) {
        task->state = -1;
        return;
    }
    switch (task->state) {
    case 0:
        work->phase = 0;
        work->redraw = 0;
        work->slide_x = 528;
        work->slide_y = 112;
        work->color = 0x00FF0000;
        work->window.x = 528;
        work->window.y = 112;
        work->window.style = 0x00FF0000;
        work->window.draw_flags = 3;
        work->window.title = D_00A13458;
        work->window.rows = 10;
        work->window.columns = 1;
        work->window.width = 400;
        work->window.height = 246;
        work->window.items = 0;
        WindowSPSet(&work->window);
        break;
    case 2:
        switch (work->phase) {
        case 0:
            work->redraw = 0;
            work->phase = 10;
            /* fallthrough */
        case 10:
            if (UmnWork.plugin_mode == 17) {
                work->redraw = 1;
                work->phase = 20;
            }
            break;
        case 20:
            for (i = 0; i < UmnWork.unlocked_plugin_count; i++) {
                unsigned char plugin_id = plugin_folder[i];

                work->items[i].text = UmnPluginTextGet(plugin_id);
                work->items[i].icon = 0;
                work->items[i].flags = 0;
            }
            work->items[i].text = D_00A13450;
            work->items[i + 1].text = 0;
            work->window.items = work->items;
            WindowSPItemChange(&work->window);
            work->window.state = 17;
            WindowSPMain(&work->window);
            work->phase = 30;
            /* fallthrough */
        case 30: {
            short slide_target = 56;

            MoveSlide(&work->slide_x, &slide_target, 3.0f);
            if (work->slide_x == slide_target) {
                work->phase = 40;
            }
            break;
        }
        case 40: {
            int selected = WindowSPSelect(&work->window, PadData.mappedRepeat);

            if (selected >= 0) {
                UmnWork.selected_plugin_index = selected;
                UmnWork.selected_plugin_id = plugin_folder[(signed char)selected];
            }
            if (UmnWork.plugin_mode != 17) {
                work->phase = 50;
            }
            break;
        }
        case 50: {
            short slide_target = 528;

            MoveSlide(&work->slide_x, &slide_target, 3.0f);
            if (work->slide_x == slide_target) {
                work->phase = 0;
            }
            break;
        }
        }
        break;
    }
    if (work->redraw != 0) {
        work->window.x = work->slide_x;
        work->window.y = work->slide_y;
        work->window.style = work->color;
        WindowSPMain(&work->window);
    }
    xglFontDebugPrintf(0, 16, D_00A13460, work->phase);
}

void tskUmnPluginExWin(void *task_opaque, void *work_opaque)
{
    struct UmnPluginTask *task = task_opaque;
    struct UmnPluginExWin *work = work_opaque;
    int i;

    if (UmnWork.plugin_work_state != 4) {
        task->state = -1;
    } else {
        switch (task->state) {
        case 0:
            work->x = 528;
            work->y = 32;
            work->style = 0x00FFFFF0;
            for (i = 0; i < 1; i++) {
                work->item[i].slide_x = -272;
                work->item[i].slide_y = 48;
                eRibbonSet(&work->item[i].ribbon, 3);
                work->item[i].ribbon.width = 256;
                work->item[i].ribbon.height = 24;
                eTagFontSet(&work->item[i].tag, msg00_1_00A10C48);
                eNumberSet(&work->item[i].number, 0);
            }
            work->state = 0;
            work->redraw = 0;
            break;
        case 2:
            {
                short slide_target = -272;
                int plugin_mode = UmnWork.plugin_mode;
                if (plugin_mode < 18) {
                    if (plugin_mode >= 16) {
                        slide_target = 0;
                    }
                }
                for (i = 0; i < 1; i++) {
                    MoveSlide(&work->item[i].slide_x, &slide_target, 3.0f);
                    work->item[i].ribbon.x = (unsigned short)work->item[i].slide_x;
                    work->item[i].ribbon.y = (unsigned short)work->item[i].slide_y;
                    work->item[i].ribbon.style = work->style;
                    eRibbonMain(&work->item[i].ribbon);
                    work->item[i].tag.x = (unsigned short)work->item[i].slide_x + 188;
                    work->item[i].tag.style = work->item[i].ribbon.style + 1;
                    work->item[i].tag.y = (unsigned short)work->item[i].slide_y + 4;
                    eTagFontMain(&work->item[i].tag);
                    work->item[i].number.x = (unsigned short)work->item[i].tag.x + 30;
                    work->item[i].number.style = work->item[i].tag.style;
                    work->item[i].number.option = 3;
                    work->item[i].number.y = (unsigned short)work->item[i].tag.y;
                    work->item[i].number.value = UmnWork.selected_plugin_index + (UmnWork.unlocked_plugin_count << 16) + 1;
                    work->item[i].number.places = 2;
                    eNumberMain(&work->item[i].number);
                }
            }
            break;
        }
    }
}

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



const char D_00A13440[16] = "Plug-ins";
