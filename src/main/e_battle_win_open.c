#include "common.h"

#include "shared.h"

#include "e_battle_win_open.h"

/* Parameter words at +0, +4, +8, +0xc and +0x10, read by eBattleWinOpen4. */
typedef struct BattleWinOpenParam4 {
    int x;
    int y;
    int style;
    const char *title;
    const char *text;
} BattleWinOpenParam4;

int BW3BattleOrDataBase = 0;

BattleWindow *BW = 0;

BattleWindow2 *BW2 = 0;

BattleWindow3 *BW3 = 0;

BattleWindow4 *BW4 = 0;


extern int MenuCursorKeepCheck(WindowSPKeepBuffer keep);

const char D_004C3A90[16] = "Hard Disk Drive";

const char D_004DA9F8[8] = "slot 2";

const char D_004DAA00[8] = "slot 1";

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", eBattleWinOpen);

void eBattleWinClose(void *window)
{
    BW->list.state = 4;
    BW->window.state = 4;
    BW->message.mode = 0;
}

void eBattleWinMain(BattleWinTask *task)
{
    int selection = -1;
    int mode = BW->mode;
    BattleWinCommand *command;
    unsigned short input = PadData.half_34;

    if (mode != -1) {
        if (mode >= -1) {
            if (mode < 2) {
                selection = WindowSPSelect(&BW->list, input);
                if (selection >= 0) {
                    command = BW->commands + selection;
                    BW->message.text = command->description;
                }
            }
        }
        task->selection = selection;
        task->buttons = PadData.half_2a;
        endPrintExtFunc(0, 100, 0);
        WindowSPMain(&BW->list);
        BW->list.window.ribbon_initialized = 1;
        WindowDXMain(&BW->window);
        eMessageMain(&BW->message);
        return;
    }
    task->selection = mode;
}

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", eBattleWinOpen2);

int eBattleWinMain2(void)
{
    if (BW2->active == 0) {
        return 0;
    }
    endPrintExtFunc(0xffffff, 100, 0);
    WindowDXMain(&BW2->window);
    switch ((signed char)BW2->window.state) {
    case 0:
        BW2->active = 0;
        return 0;
    case 3:
        eMessageMain(&BW2->message);
        return 2;
    case 1:
    case 2:
    case 4:
    case 5:
        return 1;
    }
    return 0;
}

void eBattleWinClose2(void)
{
    BW2->window.state = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", eBattleWinOpen3);

int eBattleWinMain3(void)
{
    int i;

    if (BW3->active == 0) {
        return 0;
    }
    endPrintExtFunc(0xffffff, 100, 0);
    WindowDXMain(&BW3->window);
    WindowDXMain(&BW3->window2);
    switch ((signed char)BW3->window.state) {
    case 3:
        for (i = 0; i < 3; i++) {
            eMessageMain(&BW3->messages[i]);
        }
        eTagFontMain(&BW3->tag_font);
        eNumberMain(&BW3->number);
        for (i = 0; i < 4; i++) {
            eMessageMain(&BW3->messages2[i]);
        }
        for (i = 0; i < 5; i++) {
            eTagFontMain(&BW3->tag_fonts[i]);
        }
        for (i = 0; i < 5; i++) {
            eNumberMain(&BW3->numbers[i]);
        }
        return 2;
    case 1:
    case 2:
    case 4:
    case 5:
        return 1;
    }
    return 0;
}

void eBattleWinClose3(void)
{
    BW3->window.state = 4;
    BW3->window2.state = 4;
}

void eBattleWinOpen4(BattleWinOpenParam4 *param)
{
    WindowDXSet(&BW4->window);
    BW4->window.x = param->x;
    BW4->window.y = param->y;
    BW4->window.style = param->style;
    BW4->window.width = 468;
    BW4->window.height = 112;
    BW4->window.tag_id = param->title;
    BW4->window.ribbon_initialized = 1;
    eMessageSet(&BW4->message, param->text);
    BW4->message.paged = 1;
    eMessageTextChange(&BW4->message, param->text);
    BW4->message.extent = BW4->window.style + 2;
    BW4->message.x = BW4->window.x + 3;
    BW4->message.y = BW4->window.y + 3;
    BW4->message.mode = 34;
    BW4->window.state = 1;
    BW4->active = 1;
    BW4->page = 0;
}

int eBattleWinMain4(void)
{
    if (BW4->active == 0) {
        return 0;
    }
    endPrintExtFunc(0xffffff, 100, 0);
    WindowDXMain(&BW4->window);
    switch ((signed char)BW4->window.state) {
    case 3:
        if (PadData.half_2a & 0x20) {
            if (eMessageNextPage(&BW4->message, 0) == 0) {
                BW4->page = 1;
                eMessageMain(&BW4->message);
                return 2;
            }
        }
        eMessageMain(&BW4->message);
        return 2;
    case 1:
    case 2:
    case 4:
    case 5:
        return 1;
    }
    return 0;
}

void eBattleWinClose4(void)
{
    BW4->window.state = 4;
}

int eBattleWinPageCheck4(void)
{
    return BW4->page;
}

INCLUDE_ASM("asm/main/nonmatchings/e_battle_win_open", eBattleWinInit2);

void eBattleWinInit(void)
{
    eBattleWinInit2(MenuWorkEndGet());
}

void WindowDXFlagChange(WindowDX *window, int flag)
{
    unsigned char current_state;
    unsigned char next_state;

    switch (flag) {
    case 1:
        current_state = window->state;
        next_state = (unsigned char)(current_state + 1);
        if (next_state < 2 || (signed char)current_state == 4 ||
            (signed char)current_state == 5) {
            window->state = 1;
        }
        return;

    case 3:
        window->state = flag;
        return;

    case 4:
        if ((signed char)window->state == 3) {
            window->state = flag;
        }
        return;

    default:
        window->state = flag;
        return;
    }
}

void WindowDXSet(WindowDX *window)
{
    /*
     * These nine stores are independent of each other, so -O2 schedules them
     * out of source order: it fills the jr delay slot with the third-to-last
     * store below and moves the last store to right after the two immediate
     * loads. color_r is written last here so it lands in that early slot,
     * matching the original's own first store to window.
     */
    window->alpha = 0x60;
    window->tag_id = 0;
    window->close_callback = 0;
    window->close_callback_arg = 0;
    window->state = 0;
    window->ribbon_initialized = 0;
    window->color_g = 0x80;
    window->color_b = 0x80;
    window->color_r = 0x80;
}

void WindowDXMain(WindowDX *window)
{
    char state;

    switch (window->state & 0xf) {
    case 0:
        break;
    case 1:
    case 17:
        state = window->state;
        window->state = 0;
        if (window->close_callback != 0) {
            window->close_callback(window, window->close_callback_arg);
        }
        window->frame.x = window->x;
        window->frame.y = window->y;
        window->frame.style = window->style;
        window->frame.width = window->width;
        window->frame.height = window->height;
        window->sprite.kind = 1545;
        window->state = state;
        window->frame.progress = 0;
        window->frame.mode = 0;
        endSpriteSet(&window->sprite, 255);
        eTagFontSet(&window->title_font, window->tag_id);
        eRibbonSet(&window->ribbon_top, 1);
        eRibbonSet(&window->ribbon_bottom, 1);
        window->ribbon_bottom.cells[3].r = 32;
        window->ribbon_bottom.cells[2].r = 32;
        window->ribbon_bottom.cells[1].r = 32;
        window->ribbon_bottom.cells[0].r = 32;
        window->ribbon_bottom.cells[5].r = 32;
        window->ribbon_bottom.cells[4].r = 32;
        window->ribbon_bottom.cells[3].g = 15;
        window->ribbon_bottom.cells[2].g = 15;
        window->ribbon_bottom.cells[1].g = 15;
        window->ribbon_bottom.cells[0].g = 15;
        window->ribbon_bottom.cells[5].g = 52;
        window->ribbon_bottom.cells[4].g = 52;
        window->ribbon_bottom.cells[3].b = 10;
        window->ribbon_bottom.cells[2].b = 10;
        window->ribbon_bottom.cells[1].b = 10;
        window->ribbon_bottom.cells[0].b = 10;
        window->ribbon_bottom.cells[5].b = 64;
        window->ribbon_bottom.cells[4].b = 64;
        window->ribbon_bottom.cells[2].a = 48;
        window->ribbon_bottom.cells[3].a = 0;
        window->ribbon_bottom.cells[1].a = 0;
        window->ribbon_bottom.cells[0].a = 48;
        window->ribbon_bottom.cells[5].a = 96;
        window->ribbon_bottom.cells[4].a = 96;
        if (window->close_callback != 0) {
            window->close_callback(window, window->close_callback_arg);
        }
        window->state = 2;
    case 2:
        OpenCloseMain(&window->frame);
        if (window->frame.progress >= 0) {
            break;
        }
        window->state = 3;
    case 3:
        window->rect.x = window->x;
        window->rect.y = window->y;
        window->rect.style = window->style;
        window->rect.width = window->width;
        window->rect.height = window->height;
        endPrintExtFunc((int)window->style, 1, &window->rect);
        if (window->tag_id != 0) {
            window->ribbon_top.x = window->x;
            window->ribbon_top.y = window->y - 13;
            window->ribbon_top.style = window->style;
            window->ribbon_top.width = window->width - 2;
            window->ribbon_top.height = 14;
            eRibbonMain(&window->ribbon_top);
            window->title_font.x = window->x + 8;
            window->title_font.y = window->y - 14;
            window->title_font.style = window->style + 2;
            window->title_font.color_a = 0x80;
            window->title_font.color_g = 0x80;
            window->title_font.color_b = 0x80;
            window->title_font.color_r = 0x80;
            eTagFontMain(&window->title_font);
        }
        if (window->ribbon_initialized == 0) {
            window->ribbon_bottom.x = window->rect.x + 3;
            window->ribbon_bottom.y = window->rect.y + 3;
            window->ribbon_bottom.style = window->rect.style;
            window->ribbon_bottom.width = window->rect.width - 6;
            window->ribbon_bottom.height = window->rect.height - 6;
            eRibbonMain(&window->ribbon_bottom);
        }
        if (window->close_callback != 0) {
            window->close_callback(window, window->close_callback_arg);
        }
        break;
    case 4:
        window->frame.mode = 16;
        window->frame.x = window->x;
        window->frame.y = window->y;
        window->frame.style = window->style;
        window->frame.width = window->width;
        window->frame.height = window->height;
        window->frame.progress = 0;
        if (window->close_callback != 0) {
            window->close_callback(window, window->close_callback_arg);
        }
        window->state = 5;
    case 5:
        OpenCloseMain(&window->frame);
        if (window->frame.progress < 0) {
            window->state = -1;
        }
        break;
    }
}

void subMWModeExSet(WindowSPCursor *list)
{
    int i;
    int j;
    int first = list->current_row * list->visible_cols;
    BattleWinItem *item = list->items + first;
    int shown;
    unsigned char hidden_mode;

    if (list->item_count != 0) {
        shown = list->visible_rows * list->visible_cols;
        if (shown > 24) {
            shown = 24;
        }
        if (list->item_count - first < shown) {
            shown = list->item_count - first;
        }
        for (i = 0; i < shown; i++) {
            list->messages[i].text = item->text;
            list->messages[i].mode = 32;
            list->numbers[i].option = 0;
            list->numbers[i].places = list->mode;
            list->numbers[i].value = item->icon;
            list->numbers[i].font = 3;
            if (item->flags == 0) {
                list->messages[i].color_b = 0x80;
                list->messages[i].color_g = 0x80;
                list->messages[i].color_r = 0x80;
                list->numbers[i].color_b = 0x80;
                list->numbers[i].color_g = 0x80;
                list->numbers[i].color_r = 0x80;
            } else {
                list->messages[i].color_b = 0x30;
                list->messages[i].color_g = 0x30;
                list->messages[i].color_r = 0x30;
                list->numbers[i].color_b = 0x40;
                list->numbers[i].color_g = 0x40;
                list->numbers[i].color_r = 0x40;
            }
            item++;
        }
        for (; i < 24; i++) {
            list->messages[i].mode = 112;
            list->numbers[i].font = 0;
        }
        list->numbers[24].places = 3;
        list->numbers[24].value = list->selected_index + 1;
        list->numbers[25].places = 3;
        list->numbers[25].value = list->item_count;
        list->cursors[0].shown = 1;
        eCursolModeChange(&list->cursors[0], 80);
        if (list->max_scroll != 0) {
            if (list->current_row != 0) {
                eCursolModeChange(&list->cursors[1], 80);
            } else {
                list->cursors[1].mode = 112;
            }
            if (list->current_row == list->max_scroll) {
                list->cursors[2].mode = 112;
            } else if (list->current_row < list->max_scroll) {
                eCursolModeChange(&list->cursors[2], 80);
            } else {
                list->cursors[2].mode = 112;
            }
        } else {
            list->cursors[1].mode = 112;
            list->cursors[2].mode = 112;
        }
    } else {
        hidden_mode = 112;
        for (j = 2; j >= 0; j--) {
            list->cursors[j].mode = hidden_mode;
        }
        list->messages[0].text = item->text;
        list->messages[0].mode = 32;
        for (j = 1; j < 24; j++) {
            list->messages[j].mode = 112;
            list->numbers[j].font = 0;
        }
    }
}

void subMWPosSet(WindowSPCursor *list)
{
    int row;
    int col;
    int i;
    int width;
    int text_width;
    const char *p;

    list->rect.x = list->x + 3;
    list->rect.y = list->y + 3;
    list->rect.width = list->width - 6;
    list->rect.height = list->height - 6;
    list->numbers[24].x = list->x + list->width - 142;
    list->numbers[25].x = list->x + list->width - 78;
    list->window.x = list->x;
    list->window.y = list->y;
    list->window.style = list->style;
    list->window.width = list->width;
    list->window.height = list->height;
    list->rect.style = list->style;
    list->numbers[24].y = list->y - 13;
    list->numbers[25].y = list->y - 13;
    list->numbers[24].style = list->style + 2;
    list->numbers[25].style = list->style + 2;
    if (list->item_count != 0) {
        list->cursors[0].x = list->x + list->visible_top * (list->width / 2) + 3;
        list->cursors[1].y = list->y + 4;
        list->cursors[0].y = list->y + list->visible_bottom * 24 + 7;
        list->cursors[0].style = list->style + 2;
        list->cursors[1].x = list->x + list->width - 17;
        list->cursors[2].x = list->x + list->width - 17;
        list->cursors[2].y = list->y + list->height - 19;
        list->cursors[1].style = list->style + 2;
        list->cursors[2].style = list->style + 2;
        i = 0;
        for (row = 0; row < list->visible_rows; row++) {
            for (col = 0; col < list->visible_cols; col++) {
                list->messages[i].x = list->x + (list->width / 2) * col + 19;
                list->messages[i].y = list->y + row * 24 + 3;
                list->messages[i].extent = list->style + 2;
                i++;
                if (i >= 24) {
                    break;
                }
            }
        }
        if (list->columns == 2) {
            i = 0;
            for (row = 0; row < list->visible_rows; row++) {
                for (col = 0; col < list->visible_cols; col++) {
                    list->numbers[i].x = list->x + col * (list->width / 2) + 195;
                    list->numbers[i].y = list->y + row * 24 + 8;
                    list->numbers[i].style = list->style + 2;
                    i++;
                    if (i >= 24) {
                        break;
                    }
                }
            }
        } else {
            i = 0;
            for (row = 0; row < list->visible_rows; row++) {
                for (col = 0; col < list->visible_cols; col++) {
                    list->numbers[i].x = list->x + list->width - list->mode * 11 - 19;
                    list->numbers[i].y = list->y + row * 24 + 8;
                    list->numbers[i].style = list->style + 2;
                    i++;
                    if (i >= 24) {
                        break;
                    }
                }
            }
        }
    } else {
        text_width = 0;
        for (p = list->messages[0].text; *p != 0 && *p != '\n'; p++) {
            text_width += 10;
        }
        for (i = 23; i >= 0; i--) {
            list->numbers[i].style = 0;
        }
        width = list->width;
        list->messages[0].x = list->x + (width - text_width) / 2;
        list->messages[0].y = list->y + (list->height - 24) / 2;
        list->messages[0].extent = list->style + 2;
        list->messages[0].color_b = 0x80;
        list->messages[0].color_g = 0x80;
        list->messages[0].color_r = 0x80;
    }
}

void subMWDraw(WindowSPCursor *list)
{
    int i;
    unsigned char flags = list->draw_flags;

    WindowDXMain(&list->window);
    if (flags & 1) {
        for (i = 0; i < 3; i++) {
            eCursolMain(&list->cursors[i]);
        }
    }
    endPrintExtFunc((int)list->rect.style, 5, &list->rect);
    if (flags & 2) {
        for (i = 0; i < 24; i++) {
            eMessageMain(&list->messages[i]);
        }
    }
    if (list->item_count != 0) {
        if (flags & 4) {
            for (i = 0; i < 24; i++) {
                eNumberMain(&list->numbers[i]);
            }
        }
    }
    endPrintExtFunc((int)list->rect.style, 6, 0);
}

void subMWControlType00(WindowSPCursor *list)
{
    if (list->item_count < 2) {
        return;
    }
    if (list->buttons & 0x1000) {
        list->selected_index--;
        if (list->selected_index < 0) {
            if (PadData.half_32 & 0x1000) {
                list->selected_index = list->item_count - 1;
                list->visible_bottom = list->visible_rows - 1;
                list->current_row = list->max_scroll;
                xglSoundEffectNormalID(3, 0);
            } else {
                list->selected_index++;
            }
        } else {
            if (list->visible_bottom == 0) {
                list->current_row--;
            } else {
                list->visible_bottom--;
            }
            xglSoundEffectNormalID(3, 0);
        }
    }
    if (list->buttons & 0x4000) {
        list->selected_index++;
        if (list->item_count - 1 < list->selected_index) {
            if (PadData.half_32 & 0x4000) {
                list->selected_index = 0;
                list->visible_bottom = 0;
                list->current_row = 0;
                xglSoundEffectNormalID(3, 0);
            } else {
                list->selected_index--;
            }
        } else {
            if (list->visible_bottom == list->visible_rows - 1) {
                list->current_row++;
            } else {
                list->visible_bottom++;
            }
            xglSoundEffectNormalID(3, 0);
        }
    }
}

void subMWControlType01(WindowSPCursor *list)
{
    if (list->item_count < 2) {
        return;
    }
    if (list->buttons & 0x8000) {
        list->selected_index--;
        if (list->selected_index < 0) {
            if (PadData.half_32 & 0x8000) {
                list->selected_index = list->item_count - 1;
                list->current_row = list->max_scroll;
                list->visible_bottom = list->visible_rows - 1;
                xglSoundEffectNormalID(3, 0);
            } else {
                list->selected_index++;
            }
        } else {
            if (list->visible_bottom == 0) {
                if (list->visible_top == 0) {
                    list->current_row--;
                }
            } else {
                if (list->visible_top == 0) {
                    list->visible_bottom--;
                }
            }
            xglSoundEffectNormalID(3, 0);
        }
        list->visible_top = list->selected_index % list->visible_cols;
    }
    if (list->buttons & 0x2000) {
        list->selected_index++;
        if (list->item_count - 1 < list->selected_index) {
            if (PadData.half_32 & 0x2000) {
                list->selected_index = 0;
                list->current_row = 0;
                list->visible_bottom = 0;
                xglSoundEffectNormalID(3, 0);
            } else {
                list->selected_index--;
            }
        } else {
            if (list->visible_bottom == list->visible_rows - 1) {
                if (list->visible_top == list->visible_cols - 1) {
                    list->current_row++;
                }
            } else {
                if (list->visible_top == list->visible_cols - 1) {
                    list->visible_bottom++;
                }
            }
            xglSoundEffectNormalID(3, 0);
        }
        list->visible_top = list->selected_index % list->visible_cols;
    }
    if (list->item_count == 2) {
        return;
    }
    if (list->buttons & 0x1000) {
        list->selected_index -= list->visible_cols;
        if (list->selected_index < 0) {
            if (PadData.half_32 & 0x1000) {
                if ((list->item_count - 1) % 2 != 0) {
                    list->selected_index = list->item_count + list->selected_index;
                } else {
                    list->selected_index = list->item_count - 1;
                }
                list->visible_bottom = list->visible_rows - 1;
                list->current_row = list->max_scroll;
                xglSoundEffectNormalID(3, 0);
            } else {
                list->selected_index += list->visible_cols;
            }
        } else {
            if (list->visible_bottom == 0) {
                list->current_row--;
            } else {
                list->visible_bottom--;
            }
            xglSoundEffectNormalID(3, 0);
        }
        list->visible_top = list->selected_index % list->visible_cols;
    }
    if (list->buttons & 0x4000) {
        list->selected_index += list->visible_cols;
        if (list->item_count - 1 < list->selected_index) {
            if ((list->item_count - 1) % 2 != 0) {
                if (PadData.half_32 & 0x4000) {
                    list->visible_bottom = 0;
                    list->selected_index -= list->item_count;
                    list->current_row = 0;
                    xglSoundEffectNormalID(3, 0);
                } else {
                    list->selected_index -= list->visible_cols;
                }
            } else if (list->item_count != list->selected_index) {
                if (PadData.half_32 & 0x4000) {
                    list->selected_index = 0;
                    list->visible_bottom = 0;
                    list->current_row = 0;
                    xglSoundEffectNormalID(3, 0);
                } else {
                    list->selected_index -= list->visible_cols;
                }
            } else {
                list->selected_index = list->item_count - 1;
                list->visible_bottom = list->visible_rows - 1;
                list->current_row = list->max_scroll;
                xglSoundEffectNormalID(3, 0);
            }
        } else {
            if (list->visible_bottom == list->visible_rows - 1) {
                list->current_row++;
            } else {
                list->visible_bottom++;
            }
            xglSoundEffectNormalID(3, 0);
        }
        list->visible_top = list->selected_index % list->visible_cols;
    }
}

int WindowSPSelect(WindowSPCursor *cursor, int button_flags)
{
    int control_type;

    if (cursor == 0) {
        return -1;
    }
    if (cursor->items == 0) {
        return -1;
    }
    if (cursor->item_count == 0) {
        return -1;
    }
    if (cursor->state != 3) {
        return -1;
    }

    control_type = cursor->columns;
    cursor->buttons = button_flags;
    switch (control_type) {
    case 1:
        subMWControlType00(cursor);
        break;
    case 2:
        subMWControlType01(cursor);
        break;
    }

    return cursor->selected_index;
}

void WindowSPSet(WindowSPCursor *list)
{
    BattleWinItem *item = list->items;
    int count = 0;

    if (item == 0) {
        list->current_row = 0;
        list->buttons = 0;
        list->visible_top = 0;
        list->visible_bottom = 0;
        list->selected_index = 0;
        return;
    }
    list->state = 0;
    list->mode = 2;
    list->selected_index = -1;
    list->buttons = 0;
    while ((item++)->text != 0) {
        count++;
    }
    count--;
    if (count != 0) {
        list->item_count = count;
        list->selected_index = 0;
        list->current_row = 0;
        list->max_scroll = (list->item_count - list->columns * list->rows + list->columns - 1) / list->columns;
        if (list->max_scroll < 0) {
            list->max_scroll = 0;
        }
        list->visible_top = 0;
        list->visible_bottom = 0;
        if (list->item_count < list->columns) {
            list->visible_cols = list->item_count;
        } else {
            list->visible_cols = list->columns;
        }
        if ((list->item_count + list->columns - 1) / list->columns < list->rows) {
            list->visible_rows = (list->item_count + list->columns - 1) / list->columns;
        } else {
            list->visible_rows = list->rows;
        }
    } else {
        list->visible_rows = 0;
        list->visible_bottom = 0;
        list->visible_cols = 0;
        list->visible_top = 0;
        list->max_scroll = 0;
        list->current_row = 0;
        list->item_count = 0;
    }
}

void WindowSPItemChange(WindowSPCursor *list)
{
    BattleWinItem *item;
    int count = 0;

    if (list->items == 0) {
        return;
    }
    list->mode = 2;
    item = list->items;
    while ((item++)->text != 0) {
        count++;
    }
    count--;
    if (count != 0) {
        list->item_count = count;
        if (list->selected_index >= list->item_count) {
            list->selected_index = list->item_count - 1;
        }
        list->max_scroll = (list->item_count - list->columns * list->rows + list->columns - 1) / list->columns;
        if (list->max_scroll < 0) {
            list->max_scroll = 0;
        }
        if (list->current_row > list->max_scroll) {
            list->current_row = list->max_scroll;
        }
        if (list->item_count < list->columns) {
            list->visible_cols = list->item_count;
        } else {
            list->visible_cols = list->columns;
        }
        if ((list->item_count + list->columns - 1) / list->columns < list->rows) {
            list->visible_rows = (list->item_count + list->columns - 1) / list->columns;
        } else {
            list->visible_rows = list->rows;
        }
        if (list->visible_top >= list->visible_cols) {
            list->visible_top = list->visible_cols - 1;
        } else if (list->selected_index == list->item_count - 1) {
            if (list->selected_index % 2 == 0) {
                list->visible_top = 0;
            }
        }
        if (list->visible_bottom >= list->visible_rows) {
            list->visible_bottom = list->visible_rows - 1;
        }
    } else {
        list->visible_rows = 0;
        list->visible_bottom = 0;
        list->visible_cols = 0;
        list->visible_top = 0;
        list->max_scroll = 0;
        list->current_row = 0;
        list->item_count = 0;
        list->selected_index = 0;
    }
}

void WindowSPSelectJump(WindowSPCursor *list, int index)
{
    if (list->items == 0) {
        return;
    }
    if (list->selected_index == index) {
        return;
    }
    if (list->columns != 1) {
        return;
    }
    if (index == 0) {
        list->current_row = 0;
        list->visible_bottom = 0;
        list->selected_index = 0;
        return;
    }
    if (list->selected_index - index < list->rows && index - list->current_row >= list->rows) {
        list->selected_index = index;
        list->current_row = index - list->rows + 1;
        list->visible_bottom = list->visible_rows - 1;
        return;
    }
    if (list->selected_index - index >= list->rows || index < list->current_row) {
        list->visible_bottom = 0;
        list->current_row = index;
        list->selected_index = index;
        return;
    }
    list->selected_index = index;
    list->visible_bottom = index - list->current_row;
}

void WindowSPMain(WindowSPCursor *list)
{
    int sub;
    int i;

    if (list->items == 0) {
        return;
    }
    switch (list->state & 0xf) {
    case 1:
        sub = list->state >> 4;
        switch (sub) {
        case 0:
            WindowDXSet(&list->window);
            list->window.x = list->x;
            list->window.y = list->y;
            list->window.style = list->style;
            list->window.width = list->width;
            list->window.height = list->height;
            list->window.state = 1;
            list->window.tag_id = list->title;
            for (i = 0; i < 3; i++) {
                eCursolSet(&list->cursors[i], i);
            }
            for (i = 0; i < 24; i++) {
                eMessageSet(&list->messages[i], 0);
            }
            for (i = 0; i < 26; i++) {
                eNumberSet(&list->numbers[i], 0);
            }
            subMWModeExSet(list);
            break;
        case 1:
            WindowDXSet(&list->window);
            list->window.x = list->x;
            list->window.y = list->y;
            list->window.style = list->style;
            list->window.width = list->width;
            list->window.height = list->height;
            list->window.tag_id = list->title;
            list->window.state = sub;
            WindowDXMain(&list->window);
            list->window.state = 3;
            for (i = 0; i < 3; i++) {
                eCursolSet(&list->cursors[i], i);
            }
            for (i = 0; i < 24; i++) {
                eMessageSet(&list->messages[i], 0);
            }
            for (i = 0; i < 26; i++) {
                eNumberSet(&list->numbers[i], 0);
            }
            subMWModeExSet(list);
            break;
        }
        list->state = 2;
    case 2:
        WindowDXMain(&list->window);
        if ((signed char)list->window.state != 3) {
            break;
        }
        list->state = 3;
        list->cursors[0].mode = 32;
        for (i = 0; i < 24; i++) {
            list->messages[i].mode = 32;
            list->numbers[i].font = 3;
        }
        list->anim = 0;
    case 3:
        subMWModeExSet(list);
        subMWPosSet(list);
        subMWDraw(list);
        break;
    case 4:
        list->window.x = list->x;
        list->window.y = list->y;
        list->window.style = list->style;
        list->window.width = list->width;
        list->window.height = list->height;
        list->window.state = 4;
        list->window.tag_id = list->title;
        list->state = 5;
        list->window.close_callback_arg = 0;
        list->window.close_callback = 0;
    case 5:
        WindowDXMain(&list->window);
        if ((signed char)list->window.state == 0) {
            list->state = 0;
        }
        break;
    }
}

void WindowSPKeepSelect(WindowSPCursor *cursor, WindowSPKeepBuffer keep)
{
    if (cursor->item_count != 0) {
        keep[0] = (unsigned char)cursor->item_count;
        keep[1] = (unsigned char)cursor->selected_index;
        keep[2] = cursor->visible_top;
        keep[3] = cursor->visible_bottom;
        keep[4] = cursor->current_row;
    }
}

void WindowSPKeepSelectCheck(WindowSPKeepBuffer keep)
{
    if (MenuCursorKeepCheck(keep) == 0) {
        memset(keep, 0, 5);
    }
}

void WindowSPSetSelect(WindowSPCursor *list, unsigned char *keep)
{
    if (WindowSPSelect(list, 0) < 0) {
        return;
    }
    if (list->item_count == 0) {
        memset(keep, 0, 5);
        return;
    }
    if (list->item_count - 1 < keep[1]) {
        keep[1] = list->item_count - 1;
        if (list->visible_rows - 1 < keep[3]) {
            keep[3] = list->visible_rows - 1;
        }
        if (list->visible_cols == 2) {
            keep[2] = keep[1] & 1;
        }
        if (keep[4] > list->max_scroll) {
            keep[4]--;
        }
    }
    list->selected_index = keep[1];
    list->visible_top = keep[2];
    list->visible_bottom = keep[3];
    list->current_row = keep[4];
}
