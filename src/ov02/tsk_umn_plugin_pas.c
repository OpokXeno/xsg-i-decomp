/*
 * OV02 original TU 13: 0x00a0de20..0x00a0e938 (5 functions)
 */
#include "common.h"
#include "shared.h"
#include "main/e_battle_win_open.h"
#include "main/e_message.h"
#include "main/e_number_main.h"
#include "main/end_print.h"
#include "main/menu_1.h"
#include "main/menu_top_command_check.h"
#include "main/xgl_dma.h"
#include "main/xgl_font.h"
#include "main/xgl_sound.h"
#include "ov02/ov02.h"
#include "ov02/title_hdd_install.h"
#include "ov02/tsk_umn_object_task_main.h"
#include "ov02/tsk_umn_plugin_pas.h"
#include "ov02/umn_event_text_symbol_check.h"

/* Plug-in slots (bits of the unlock flags, entries of plugin_folder). */
#define UMN_PLUGIN_SLOTS 16
/* Plug-ins the list shows (ids below this have a description); the list array adds the "end" row and the terminator. */
#define UMN_PLUGIN_LIST_MAX 6

/* tskUmnPluginList phases (UmnPluginListWork.phase). */
enum {
    UMN_PLUGIN_LIST_IDLE = 0,
    UMN_PLUGIN_LIST_WAIT_OPEN = 10,
    UMN_PLUGIN_LIST_FILL = 20,
    UMN_PLUGIN_LIST_SLIDE_IN = 30,
    UMN_PLUGIN_LIST_SELECT = 40,
    UMN_PLUGIN_LIST_SLIDE_OUT = 50
};

struct UmnPluginInfoWork {
    unsigned char state;
    unsigned char opened;
    unsigned char unmodeled_02[6];
    int color;
    WindowDX window;
    UmnInfoPanel info;
};

struct UmnPluginPasWork {
    unsigned char unmodeled_00;
    unsigned char initialized;
    unsigned char unmodeled_02[2];
    int color;
    WindowDX window;
    EMessageParam message;
    unsigned char unmodeled_1e0[0x3bc - 0x1e0];
    WindowDXRect caption_rect;
};

struct UmnPluginListWork {
    unsigned char phase;
    unsigned char redraw;
    unsigned char unmodeled_02[2];
    short slide_x;
    short slide_y;
    int color;
    WindowSPCursor window;
    MenuListEntry items[UMN_PLUGIN_LIST_MAX + 2];
};

struct UmnPluginExWinItem {
    short slide_x;
    short slide_y;
    ETagFontMainData tag;
    ERibbonMainData ribbon;
    ENumberMainData number;
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

extern const char D_00A13440[];
extern const char D_00A13458[];
extern const char D_00A13460[];
extern char msg00_1_00A10C48[];
extern void xglFontDebugHex(int x, int y, int value, int digits);

static const char *msg00_0_00A10C40 = D_00A13440;
unsigned char plugin_folder[UMN_PLUGIN_SLOTS] = { 0 };
static const char D_00A13450[8] = "";
/* The original EUC-JP debug label spells プラグイン. */
/* JP: プラグイン
 * EN: Plugin */
static const char D_00A13540[16] =
    "\xA5\xD7\xA5\xE9\xA5\xB0\xA5\xA4\xA5\xF3";

void tskUmnPluginPas(UmnObjectTask *task, void *work_opaque)
{
    struct UmnPluginPasWork *work = work_opaque;

    if (UmnWork.mode != UMN_MODE_PLUGIN) {
        task->state = UMN_TASK_END;
    } else {
        switch (task->state) {
        case UMN_TASK_INIT: {
            int label_x = 288;
            int label_y = 11;
            int label_font_size = 32;
            int initial_state = 1;
            int panel_color;
            work->color = 0x00FFFFF0;
            WindowDXSet(&work->window);
            work->window.x = -272;
            work->window.style = work->color;
            work->window.y = 8;
            work->window.width = 272;
            work->window.height = label_font_size;
            work->window.state = initial_state;
            WindowDXMain(&work->window);
            work->window.state = WINDOWDX_STATE_READY;
            eMessageSet(&work->message, msg00_0_00A10C40);
            {
            UmnWindowMessageView *label = (UmnWindowMessageView *)((unsigned char *)work + 0x190);
            label->message.mode = label_font_size;
            panel_color = work->color;
            label->message.x = label_x;
            label->message.y = label_y;
            work->initialized = initial_state;
            }
            work->message.draw_parameter = panel_color + 2;
            break;
        }
        case UMN_TASK_RUN:
            {
                short slide_targets[2];
                short *label_target;
                int plugin_mode = UmnWork.tab_state;
                int caption_mode;

                slide_targets[0] = -16;
                label_target = &slide_targets[1];
                *label_target = 288;
                if (plugin_mode < UMN_STEP_ACTIVE + 1) {
                    if (plugin_mode >= UMN_STEP_OPEN) {
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
                    caption_mode = END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_SET;
                    work->caption_rect.x = (unsigned short)work->window.x + 3;
                    work->caption_rect.y = (unsigned short)work->window.y + 3;
                    work->caption_rect.width = work->window.width - 6;
                    work->caption_rect.height = work->window.height - 6;
                    work->caption_rect.style = work->color;
                    endPrintExtFunc(work->color, caption_mode, &work->caption_rect);
                    MoveSlide(&work->message.x, label_target, 3.0f);
                    if (work->message.x < 256) {
                        eMessageMain(&work->message);
                    }
                    endPrintExtFunc(work->color, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_SCISSOR_RESET, 0);
                }
            }
            break;
        }
    }
}

void tskUmnPluginInfo(UmnObjectTask *task, void *work_opaque)
{
    struct UmnPluginInfoWork *work = work_opaque;

    if (UmnWork.mode != UMN_MODE_PLUGIN) {
        task->state = UMN_TASK_END;
    } else {
        switch (task->state) {
        case UMN_TASK_INIT:
            work->color = 0x00FF0000;
            work->opened = 0;
            work->state = 0;
            WindowDXSet(&work->window);
            work->window.x = -16;
            ((volatile struct UmnPluginInfoWork *)work)->window.state = 0;
            work->window.y = 480;
            work->window.width = 544;
            work->window.height = 54;
            work->window.close_callback = MenuInfoWindow;
            work->window.style = work->color;
            work->window.close_callback_arg = &work->info;
            work->info.x = 0;
            work->info.y = 0;
            work->info.text = 0;
            ((volatile struct UmnPluginInfoWork *)work)->window.state = WINDOWDX_STATE_OPEN;
            WindowDXMain(&work->window);
            work->window.state = WINDOWDX_STATE_READY;
            break;
        case UMN_TASK_RUN:
            {
                short slide_target = 480;
                int plugin_mode = UmnWork.tab_state;
                unsigned char plugin_id;

                if (plugin_mode < UMN_STEP_ACTIVE + 1) {
                    if (plugin_mode >= UMN_STEP_OPEN) {
                        slide_target = 386;
                    }
                }
                plugin_id = UmnWork.entry;
                if (plugin_id < UMN_PLUGIN_LIST_MAX) {
                    work->info.text = UmnPluginTextGet((signed char)plugin_id) + 0x20;
                } else {
                    work->info.text = D_00A13450;
                }
                MoveSlide(&work->window.y, &slide_target, 3.0f);
                WindowDXMain(&work->window);
            }
            break;
        }
    }
}

void tskUmnPluginList(UmnObjectTask *task, void *work_opaque)
{
    struct UmnPluginListWork *work = work_opaque;
    int i;

    if (UmnWork.mode != UMN_MODE_PLUGIN) {
        task->state = UMN_TASK_END;
        return;
    }
    switch (task->state) {
    case UMN_TASK_INIT:
        work->phase = UMN_PLUGIN_LIST_IDLE;
        work->redraw = 0;
        work->slide_x = 528;
        work->slide_y = 112;
        work->color = 0x00FF0000;
        work->window.x = 528;
        work->window.y = 112;
        work->window.style = 0x00FF0000;
        work->window.draw_flags = WINDOWSP_DRAW_CURSORS | WINDOWSP_DRAW_MESSAGES;
        work->window.title = D_00A13458;
        work->window.rows = 10;
        work->window.columns = 1;
        work->window.width = 400;
        work->window.height = 246;
        work->window.items = 0;
        WindowSPSet(&work->window);
        break;
    case UMN_TASK_RUN:
        switch (work->phase) {
        case UMN_PLUGIN_LIST_IDLE:
            work->redraw = 0;
            work->phase = UMN_PLUGIN_LIST_WAIT_OPEN;
            /* fallthrough */
        case UMN_PLUGIN_LIST_WAIT_OPEN:
            if (UmnWork.tab_state == UMN_STEP_ACTIVE) {
                work->redraw = 1;
                work->phase = UMN_PLUGIN_LIST_FILL;
            }
            break;
        case UMN_PLUGIN_LIST_FILL:
            for (i = 0; i < UmnWork.count; i++) {
                unsigned char plugin_id = plugin_folder[i];

                work->items[i].name = UmnPluginTextGet(plugin_id);
                work->items[i].amount = 0;
                work->items[i].flag = 0;
            }
            work->items[i].name = D_00A13450;
            work->items[i + 1].name = 0;
            work->window.items = work->items;
            WindowSPItemChange(&work->window);
            work->window.state = WINDOWSP_STATE_OPEN | (WINDOWSP_OPEN_NO_ANIMATION << 4);
            WindowSPMain(&work->window);
            work->phase = UMN_PLUGIN_LIST_SLIDE_IN;
            /* fallthrough */
        case UMN_PLUGIN_LIST_SLIDE_IN: {
            short slide_target = 56;

            MoveSlide(&work->slide_x, &slide_target, 3.0f);
            if (work->slide_x == slide_target) {
                work->phase = UMN_PLUGIN_LIST_SELECT;
            }
            break;
        }
        case UMN_PLUGIN_LIST_SELECT: {
            int selected = WindowSPSelect(&work->window, PadData.mappedRepeat);

            if (selected >= 0) {
                UmnWork.list_cursor = selected;
                UmnWork.entry = plugin_folder[(signed char)selected];
            }
            if (UmnWork.tab_state != UMN_STEP_ACTIVE) {
                work->phase = UMN_PLUGIN_LIST_SLIDE_OUT;
            }
            break;
        }
        case UMN_PLUGIN_LIST_SLIDE_OUT: {
            short slide_target = 528;

            MoveSlide(&work->slide_x, &slide_target, 3.0f);
            if (work->slide_x == slide_target) {
                work->phase = UMN_PLUGIN_LIST_IDLE;
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

void tskUmnPluginExWin(UmnObjectTask *task, void *work_opaque)
{
    struct UmnPluginExWin *work = work_opaque;
    int i;

    if (UmnWork.mode != UMN_MODE_PLUGIN) {
        task->state = UMN_TASK_END;
    } else {
        switch (task->state) {
        case UMN_TASK_INIT:
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
        case UMN_TASK_RUN:
            {
                short slide_target = -272;
                int plugin_mode = UmnWork.tab_state;
                if (plugin_mode < UMN_STEP_ACTIVE + 1) {
                    if (plugin_mode >= UMN_STEP_OPEN) {
                        slide_target = 0;
                    }
                }
                for (i = 0; i < 1; i++) {
                    MoveSlide(&work->item[i].slide_x, &slide_target, 3.0f);
                    work->item[i].ribbon.x = (unsigned short)work->item[i].slide_x;
                    work->item[i].ribbon.y = (unsigned short)work->item[i].slide_y;
                    work->item[i].ribbon.work = work->style;
                    eRibbonMain(&work->item[i].ribbon);
                    work->item[i].tag.x = (unsigned short)work->item[i].slide_x + 188;
                    work->item[i].tag.work = work->item[i].ribbon.work + 1;
                    work->item[i].tag.y = (unsigned short)work->item[i].slide_y + 4;
                    eTagFontMain(&work->item[i].tag);
                    work->item[i].number.x = (unsigned short)work->item[i].tag.x + 30;
                    work->item[i].number.work = work->item[i].tag.work;
                    work->item[i].number.format = 3;
                    work->item[i].number.y = (unsigned short)work->item[i].tag.y;
                    work->item[i].number.value = UmnWork.list_cursor + (UmnWork.count << 16) + 1;
                    work->item[i].number.digits = 2;
                    eNumberMain(&work->item[i].number);
                }
            }
            break;
        }
    }
}

void UmnPlugin(void)
{
    switch (UmnWork.tab_state) {
    case UMN_STEP_INIT:
        {
        unsigned short unlocked_plugins;
        int plugin_index;
        unsigned char *task_work = (unsigned char *)UmnWorkEnd;
        unsigned char *folder_cursor;
        UmnMailSaveData *save;

        UmnObjectTaskCreate(tskUmnPluginPas, task_work);
        task_work += 968;
        UmnObjectTaskCreate(tskUmnPluginInfo, task_work);
        folder_cursor = plugin_folder;
        task_work += 848;
        UmnObjectTaskCreate(tskUmnPluginList, task_work);
        UmnObjectTaskCreate(tskUmnPluginExWin, task_work + 6340);
        save = &D_4A1A0C;
        UmnWork.count = 0;
        UmnWork.list_cursor = 0;
        memset(folder_cursor, 0, sizeof(plugin_folder));
        unlocked_plugins = save->plugin_unlock_flags;
        for (plugin_index = 0; plugin_index < UMN_PLUGIN_SLOTS; plugin_index++) {
            if (((int)unlocked_plugins >> plugin_index) & 1) {
                *folder_cursor++ = plugin_index;
                UmnWork.count++;
            }
        }
        if (UmnWork.count >= UMN_PLUGIN_LIST_MAX + 1) {
            UmnWork.count = UMN_PLUGIN_LIST_MAX;
        }
        UmnWork.tab_state = UMN_STEP_OPEN;
        UmnWork.entry = plugin_folder[UmnWork.list_cursor];
        break;
        }
    case UMN_STEP_OPEN:
        UmnWork.tab_state = UMN_STEP_ACTIVE;
        UmnWork.wait = UMN_WAIT_OPEN;
        /* Continue through the entry timer check. */
    case UMN_STEP_ACTIVE:
        if (UmnWork.wait == 0 &&
            (PadData.half_2a & PAD_CROSS) != 0) {
            UmnWork.tab_state = UMN_STEP_LEAVE;
            xglSoundEffectNormalID(SE_CANCEL, 0);
        }
        break;
    case UMN_STEP_LEAVE:
        UmnWork.tab_state = UMN_STEP_LEAVE_WAIT;
        UmnWork.wait = UMN_WAIT_LEAVE;
        /* Continue through the exit timer check. */
    case UMN_STEP_LEAVE_WAIT:
        if (UmnWork.wait == 0) {
            UmnChangeTopLevel(0);
        }
        break;
    }

    if (UmnWork.wait != 0) {
        UmnWork.wait--;
    }
    xglFontDebugPrintf(32, 208, D_00A13540);
    xglFontDebugHex(0, 64, UmnWork.tab_state, 2);
}

/* Defined here, after the functions: the plug-ins caption string. */
const char D_00A13440[16] = "Plug-ins";
