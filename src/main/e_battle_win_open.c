#include "common.h"
#include "shared.h"
#include "main/e_battle_win_open.h"
#include "e_battle_win_open.h"
#include "main/end_print.h"

/* Parameter words at +0, +4, +8, +0xc and +0x10, read by eBattleWinOpen4. */
typedef struct BattleWinOpenParam4 {
    int x;
    int y;
    int style;
    const char *title;
    const char *text;
} BattleWinOpenParam4;

typedef struct BattleWinOpen2Params {
    int x;
    int y;
    int style;
    int width;
    const char *text;
    const char *title;
} BattleWinOpen2Params;

typedef struct BattleWinConditionLabelTable {
    const unsigned short *rows[10];
} BattleWinConditionLabelTable;

typedef struct BattleWinAnalysisParams {
    int x;
    int y;
    int style;
    const char *name;
    short display_value_low;
    short display_value_high;
    unsigned short condition_flags;
    unsigned char unmodeled_16[2];
    int tech_points;
    int ether_points;
    int skill_points;
    int experience;
    long money;
    unsigned short item;
    unsigned short rare_item;
    const char *item_text;
    const char *rare_item_text;
} BattleWinAnalysisParams;

typedef struct BattleWinOpenParams {
    int x;
    int y;
    int style;
    int window_dx;
    int window_dy;
    BattleWinCommand *commands;
    const char *title;
} BattleWinOpenParams;

extern const char analysisTagText[];
extern const char hpText[];
extern const BattleWinConditionLabelTable conditionLabelTable;
extern MenuListEntry *BW_item;
extern const char informationTagText[];
extern const char weaknessPrefixText[16];
extern const char rareItemPrefixText[16];
extern const char itemPrefixText[8];
extern const char moneyText[8];
extern const char ePtsText[8];
extern const char expText[8];
extern const char tPtsText[8];
extern const char sPtsText[8];

/* Original storage of this TU without a C reader; typed from its original bytes. */
extern const char conditionBText[8];

extern const char conditionSpText[8];
extern const char conditionLText[8];
extern const char conditionFText[8];
extern const char conditionIText[8];
extern const char conditionPrText[8];
extern const char conditionSlText[8];
extern const char conditionHtText[8];
extern const char conditionArText[8];
extern const char conditionCuText[8];

extern int MenuCursorKeepCheck(WindowSPKeepBuffer keep);
const char **func_A2C5F8(int id);
const char **func_A2C698(int id);
const char **func_A2C6E8(int id);
const char **func_A2C738(int id);
int BW3BattleOrDataBase = 0;

BattleWindow *BW = 0;

BattleWindow2 *BW2 = 0;

BattleWindow3 *BW3 = 0;

BattleWindow4 *BW4 = 0;

const char MenuFileSlot2Text[8] = "slot 2";

const char MenuFileSlot1Text[8] = "slot 1";

static const char * msg00_1_0036D600[3];

static const char * msg01_2_0036D610[5];

static const char text_0;

static const char * msg00_1_0036D600[3] = { weaknessPrefixText, rareItemPrefixText, itemPrefixText };

static const char * msg01_2_0036D610[5] = { tPtsText, ePtsText, sPtsText, expText, moneyText };

static const char text_0 = 0;

const char itemPrefixText[8] = "Item: ";

/* Text: \x01Money */
const char moneyText[8] = "\001Money";

/* Text: \x01Exp */
const char expText[8] = "\001Exp";

/* Text: \x01S.Pts */
const char sPtsText[8] = "\001S.Pts";

/* Text: \x01E.Pts */
const char ePtsText[8] = "\001E.Pts";

/* Text: \x01T.Pts */
const char tPtsText[8] = "\001T.Pts";

/* Text: \x01Hp */
const char hpText[4] = "\001Hp";

MenuListEntry * BW_item = 0;

const char MenuFileFormatText[8] = "Format";

const char MenuFileLoadText[8] = "Load";

const char MenuFileSaveText[8] = "Save";

const char MenuFileCancelText[8] = "Cancel.";

const char conditionBText[8] = "B";

const char conditionSpText[8] = "Sp";

const char conditionLText[8] = "L";

const char conditionFText[8] = "F";

const char conditionIText[8] = "I";

const char conditionPrText[8] = "Pr";

const char conditionSlText[8] = "Sl";

const char conditionHtText[8] = "Ht";

const char conditionArText[8] = "Ar";

const char conditionCuText[8] = "Cu";

static unsigned short msg03_3[3];

/* Defined here, not with the data above: moving it before the functions that use it changes the original code or data order. */
const char MenuFileHardDiskDriveText[16] = "Hard Disk Drive";

void eBattleWinOpen(BattleWinOpenParams *params)
{
    BattleWindow *window = BW;
    MenuListEntry *items = BW_item;
    BattleWindow *information;
    BattleWinCommand *command;
    MenuListEntry *item;
    const char *text;
    const char *next_text;

    window->commands = params->commands;
    command = params->commands;
    item = items;
    text = command->text;
    if (text != 0) {
        do {
            unsigned char flags;

            item->name = text;
            item->amount = command->icon;
            flags = command->flags;
            command++;
            item->flag = flags;
            item++;
            next_text = command->text;
            text = next_text;
        } while (next_text != 0);
        window = BW;
        items = BW_item;
    }
    item[1].name = 0;
    item->name = &text_0;
    {
        unsigned short x = params->x;
        unsigned short y = params->y;
        int style = params->style;

        window->list.x = x;
        window->list.y = y;
        window->list.style = style;
        window->list.width = 468;
        window->list.height = 112;
        window->list.title = params->title;
        window->list.items = items;
        window->list.columns = 2;
    }
    BW->list.rows = 4;
    BW->list.draw_flags = WINDOWSP_DRAW_CURSORS | WINDOWSP_DRAW_MESSAGES | WINDOWSP_DRAW_NUMBERS;
    WindowSPSet(&BW->list);
    BW->list.state = WINDOWSP_STATE_OPEN;
    BW->list.window.alpha = 64;
    WindowDXSet(&BW->window);
    information = BW;
    {
        information->window.x = (unsigned short)params->x + (unsigned short)params->window_dx;
        information->window.y = (unsigned short)params->y + (unsigned short)params->window_dy;
        information->window.style = params->style;
        information->window.width = 312;
        information->window.height = 64;
        information->window.tag_id = informationTagText;
        information->window.ribbon_initialized = 1;
    }
    BW->window.state = WINDOWDX_STATE_OPEN;
    eMessageSet(&BW->message, 0);
    BW->message.mode = E_MODE_SHOW_START;
    BW->message.x = BW->window.x + 3;
    BW->message.y = BW->window.y + 3;
    BW->message.draw_parameter = BW->window.style + 2;
}

void eBattleWinClose(void *window)
{
    BW->list.state = WINDOWSP_STATE_CLOSE;
    BW->window.state = WINDOWDX_STATE_CLOSE;
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
        endPrintExtFunc(0, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_FLUSH, 0);
        WindowSPMain(&BW->list);
        BW->list.window.ribbon_initialized = 1;
        WindowDXMain(&BW->window);
        eMessageMain(&BW->message);
        return;
    }
    task->selection = mode;
}

void eBattleWinOpen2(BattleWinOpen2Params *param)
{
    BattleWindow2 *window;
    BattleWindow2 *height_window;
    BattleWindow2 *message_window;
    const char *text;
    int line_width;
    int max_width;
    int lines;

    WindowDXSet(&BW2->window);
    window = BW2;
    window->window.x = param->x;
    window->window.y = param->y;
    window->window.style = param->style;
    window->window.width = param->width;
    if (param->width < 0) {
        line_width = 0;
        max_width = 0;
        text = param->text;
        while (*text != 0) {
            if (*text == '\n') {
                max_width = max_width < line_width ? line_width : max_width;
                text++;
                line_width = 0;
            } else {
                line_width++;
                text++;
            }
        }
        max_width = max_width < line_width ? line_width : max_width;
        window->text_offset = ((unsigned int)(param->width - 6) >> 1) -
                           max_width * 9 / 2;
    } else {
        window->text_offset = 0;
    }
    lines = eMessageNextGyouMaxGet(param->text);
    height_window = BW2;
    height_window->window.ribbon_initialized = 1;
    height_window->window.height = lines * 24 + 6;
    BW2->window.tag_id = param->title;
    eMessageSet(&BW2->message, param->text);
    message_window = BW2;
    message_window->message.mode = E_MODE_SHOW_START;
    message_window->message.y = message_window->window.y + 3;
    message_window->message.draw_parameter = message_window->window.style + 2;
    message_window->message.x = message_window->window.x + message_window->text_offset + 3;
    BW2->window.state = WINDOWDX_STATE_OPEN;
    WindowDXMain(&BW2->window);
    BW2->window.state = WINDOWDX_STATE_READY;
    BW2->active = 1;
}

int eBattleWinMain2(void)
{
    if (BW2->active == 0) {
        return 0;
    }
    endPrintExtFunc(0xffffff, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_FLUSH, 0);
    WindowDXMain(&BW2->window);
    switch (BW2->window.state) {
    case WINDOWDX_STATE_CLOSED:
        BW2->active = 0;
        return 0;
    case WINDOWDX_STATE_READY:
        eMessageMain(&BW2->message);
        return 2;
    case WINDOWDX_STATE_OPEN:
    case WINDOWDX_STATE_OPENING:
    case WINDOWDX_STATE_CLOSE:
    case WINDOWDX_STATE_CLOSING:
        return 1;
    }
    return 0;
}

void eBattleWinClose2(void)
{
    BW2->window.state = WINDOWDX_STATE_CLOSED;
}

void eBattleWinOpen3(void *data)
{
    const BattleWinAnalysisParams *param = data;
    BattleWinConditionLabelTable condition_labels = conditionLabelTable;
    unsigned char code_ids[2];
    unsigned char code_kinds[2];
    unsigned char rare_category;
    unsigned char item_category;
    int experience_value;
    const char *text;
    int display_index;
    int points_digit_count;
    int entry_index;
    BattleWindow3 **analysis_window_slot;
    const int *database_mode = &BW3BattleOrDataBase;

    WindowDXSet(&BW3->window);
    analysis_window_slot = &BW3;
    {
        BattleWindow3 *window = (*analysis_window_slot);
        unsigned int x = (unsigned short)param->x;
        const char *tag = analysisTagText;
        unsigned int y = (unsigned short)param->y;
        int depth = param->style;
        unsigned int mode = *database_mode;

        window->window.x = x;
        window->window.y = y;
        window->window.style = depth;
        window->window.width = 326;
        window->window.height = 54;
        window->window.tag_id = tag;
        switch (mode) {
        case 0:
            window->window.ribbon_initialized = 1;
            window = (*analysis_window_slot);
            break;
        default:
            break;
        }
        eMessageSet(&window->messages[0], param->name);
    }
    {
        BattleWindow3 *message_window = (*analysis_window_slot);

        unsigned short x = message_window->window.x;
        unsigned short y = message_window->window.y;
        int depth = message_window->window.style;

        message_window->messages[0].mode = E_MODE_SHOW_START;
        message_window->messages[0].y = y + 3;
        message_window->messages[0].draw_parameter = depth + 2;
        message_window->messages[0].x = x + 3;
    }
    eMessageSet(&(*analysis_window_slot)->messages[1], msg00_1_0036D600[0]);
    {
        BattleWindow3 *message_window = (*analysis_window_slot);

        unsigned short x = message_window->window.x;
        unsigned short y = message_window->window.y;
        int depth = message_window->window.style;

        x += 23;
        y += 27;
        message_window->messages[1].x = x;
        message_window->messages[1].y = y;
        message_window->messages[1].draw_parameter = depth + 2;
        message_window->messages[1].mode = E_MODE_SHOW_START;
        display_index = 0;
    }
    {
        unsigned char *condition_text = (unsigned char *)msg03_3;
        int clear_index;

        for (clear_index = sizeof(msg03_3) - 1; clear_index >= 0; clear_index--) {
            condition_text[clear_index] = 0;
        }
    }
    {
        int condition_index;

        for (condition_index = 0; condition_index < 10; condition_index++) {
            if ((param->condition_flags >> condition_index) & 1) {
                msg03_3[display_index] = condition_labels.rows[condition_index][0];
                if (display_index == 1) {
                    break;
                }
                display_index++;
            }
        }
    }
    eMessageSet(&(*analysis_window_slot)->messages[2], (const char *)msg03_3);
    {
        BattleWindow3 *message_window = (*analysis_window_slot);
        unsigned short x = message_window->window.x;
        unsigned short y = message_window->window.y;
        int depth = message_window->window.style;

        message_window->messages[2].mode = E_MODE_SHOW_START;
        message_window->messages[2].y = y + 27;
        message_window->messages[2].draw_parameter = depth + 2;
        message_window->messages[2].x = x + 123;
    }
    eTagFontSet(&(*analysis_window_slot)->tag_font, hpText);
    (*analysis_window_slot)->tag_font.x = (*analysis_window_slot)->window.x + 191;
    (*analysis_window_slot)->tag_font.y = (*analysis_window_slot)->window.y + 16;
    (*analysis_window_slot)->tag_font.work = (*analysis_window_slot)->window.style + 2;
    eNumberSet(&(*analysis_window_slot)->number, 0);
    {
        BattleWindow3 *number_window = (*analysis_window_slot);
        int depth = number_window->window.style;
        unsigned short x = number_window->window.x;
        unsigned short y = number_window->window.y;

        number_window->number.format = 3;
        number_window->number.work = depth + 2;
        number_window->number.y = y + 16;
        number_window->number.x = x + 250;
    }
    (*analysis_window_slot)->number.digits = 5;
    (*analysis_window_slot)->number.value =
        ((unsigned int)param->display_value_high << 16) +
        param->display_value_low;
    WindowDXSet(&(*analysis_window_slot)->window2);
    {
        BattleWindow3 *window = (*analysis_window_slot);
        unsigned short y = param->y;
        unsigned short x = param->x;
        int depth = param->style;
        unsigned int mode = *database_mode;

        y += 54;
        window->window2.x = x;
        window->window2.y = y;
        window->window2.style = depth;
        window->window2.width = 326;
        window->window2.height = 126;
        if (mode == 0) {
            window->window2.ribbon_initialized = 1;
            window = (*analysis_window_slot);
        }
        entry_index = 0;
        do {
            eTagFontSet(&window->tag_fonts[entry_index], msg01_2_0036D610[entry_index]);
            window = (*analysis_window_slot);
            window->tag_fonts[entry_index].x = window->window2.x + (entry_index / 3) * 120 + 54;
            window->tag_fonts[entry_index].y = window->window2.y + (entry_index % 3) * 24 + 7;
            window->tag_fonts[entry_index].work = window->window2.style + 2;
            entry_index++;
        } while (entry_index < 5);
        entry_index = 0;
initialize_next_number:
        eNumberSet(&window->numbers[entry_index], 0);
        points_digit_count = 4;
        BW3->numbers[entry_index].x = BW3->window2.x + (entry_index / 3) * 120 + 58;
        BW3->numbers[entry_index].y = BW3->window2.y + (entry_index % 3) * 24 + 7;
        BW3->numbers[entry_index].work = BW3->window2.style + 2;
        BW3->numbers[entry_index].format = 0;
        if (entry_index < 3) {
            BW3->numbers[entry_index].digits = points_digit_count;
        } else {
            BW3->numbers[entry_index].digits = 10;
        }
        entry_index++;
        window = BW3;
        if (entry_index < 5) {
            goto initialize_next_number;
        }
        {
            int tech_value = param->tech_points;

            rare_category = param->rare_item >> 8;
            (*analysis_window_slot)->numbers[0].value = tech_value;
        }
        item_category = param->item >> 8;
        (*analysis_window_slot)->numbers[1].value = param->ether_points;
        (*analysis_window_slot)->numbers[2].value = param->skill_points;
        experience_value = param->experience;
        (*analysis_window_slot)->numbers[4].value = param->money;
        (*analysis_window_slot)->numbers[3].value = experience_value;
        code_ids[0] = param->rare_item & 0xff;
        code_ids[1] = param->item;
        code_kinds[0] = rare_category;
        code_kinds[1] = item_category;
        entry_index = 0;
        do {
            BattleWindow3 *message_window;

            text = msg00_1_0036D600[entry_index + 1];
            eMessageSet(&(*analysis_window_slot)->messages2[entry_index], text);
            message_window = (*analysis_window_slot);
            message_window->messages2[entry_index].x = message_window->window2.x + entry_index * 40 + 3;
            message_window->messages2[entry_index].y = message_window->window2.y + entry_index * 24 + 75;
            message_window->messages2[entry_index].draw_parameter = message_window->window2.style + 2;
            message_window->messages2[entry_index].mode = E_MODE_SHOW_START;
            entry_index++;
        } while (entry_index < 2);
    }
    entry_index = 0;
    do {
        if (*database_mode == 0) {
            unsigned char *catalog_id = &code_ids[entry_index];

            if (*catalog_id != 0) {
                switch (code_kinds[entry_index]) {
                case 0:
                    text = *func_A2C5F8(*catalog_id);
                    break;
                case 1:
                    text = *func_A2C6E8(*catalog_id);
                    break;
                case 2:
                    text = *func_A2C738(*catalog_id);
                    break;
                case 3:
                    text = *func_A2C698(*catalog_id);
                    break;
                default:
                    text = 0;
                    break;
                }
            } else {
                text = 0;
            }
        } else if (entry_index == 0) {
            text = param->rare_item_text;
        } else {
            text = param->item_text;
        }
        eMessageSet(&(*analysis_window_slot)->messages2[entry_index + 2], text);
        {
            BattleWindow3 *message_window = (*analysis_window_slot);

            message_window->messages2[entry_index + 2].x = message_window->window2.x + 143;
            message_window->messages2[entry_index + 2].y = message_window->window2.y + entry_index * 24 + 75;
            message_window->messages2[entry_index + 2].draw_parameter = message_window->window2.style + 2;
            message_window->messages2[entry_index + 2].mode = E_MODE_SHOW_START;
        }
        entry_index++;
    } while (entry_index < 2);
    (*analysis_window_slot)->window.state = WINDOWDX_STATE_OPEN;
    (*analysis_window_slot)->window2.state = WINDOWDX_STATE_OPEN;
    (*analysis_window_slot)->active = 1;
}

int eBattleWinMain3(void)
{
    int i;

    if (BW3->active == 0) {
        return 0;
    }
    endPrintExtFunc(0xffffff, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_FLUSH, 0);
    WindowDXMain(&BW3->window);
    WindowDXMain(&BW3->window2);
    switch (BW3->window.state) {
    case WINDOWDX_STATE_READY:
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
    case WINDOWDX_STATE_OPEN:
    case WINDOWDX_STATE_OPENING:
    case WINDOWDX_STATE_CLOSE:
    case WINDOWDX_STATE_CLOSING:
        return 1;
    }
    return 0;
}

void eBattleWinClose3(void)
{
    BW3->window.state = WINDOWDX_STATE_CLOSE;
    BW3->window2.state = WINDOWDX_STATE_CLOSE;
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
    BW4->message.page_available = 1;
    eMessageTextChange(&BW4->message, param->text);
    BW4->message.draw_parameter = BW4->window.style + 2;
    BW4->message.x = BW4->window.x + 3;
    BW4->message.y = BW4->window.y + 3;
    BW4->message.mode = 34;
    BW4->window.state = WINDOWDX_STATE_OPEN;
    BW4->active = 1;
    BW4->page = 0;
}

int eBattleWinMain4(void)
{
    if (BW4->active == 0) {
        return 0;
    }
    endPrintExtFunc(0xffffff, END_PRINT_CMD_EXT_BASE + END_PRINT_EXT_FLUSH, 0);
    WindowDXMain(&BW4->window);
    switch (BW4->window.state) {
    case WINDOWDX_STATE_READY:
        if (PadData.half_2a & PAD_CIRCLE) {
            if (eMessageNextPage(&BW4->message, 0) == 0) {
                BW4->page = 1;
                eMessageMain(&BW4->message);
                return 2;
            }
        }
        eMessageMain(&BW4->message);
        return 2;
    case WINDOWDX_STATE_OPEN:
    case WINDOWDX_STATE_OPENING:
    case WINDOWDX_STATE_CLOSE:
    case WINDOWDX_STATE_CLOSING:
        return 1;
    }
    return 0;
}

void eBattleWinClose4(void)
{
    BW4->window.state = WINDOWDX_STATE_CLOSE;
}

int eBattleWinPageCheck4(void)
{
    return BW4->page;
}

void *eBattleWinInit2(void *arena)
{
    void *cursor = arena;
    int *active_flag;
    int *third_flag;
    int *second_flag;
    BattleWindow2 **dialog_slot = &BW2;

    BW = cursor;
    cursor = BW + 1;
    BW->mode = 0;
    BW_item = cursor;
    cursor = BW_item + 256;
    {
        *dialog_slot = cursor;
        active_flag = &(*dialog_slot)->active;
        cursor = *dialog_slot + 1;
        *active_flag = 0;
    }
    {
        BattleWindow3 **analysis_slot = &BW3;

        *analysis_slot = cursor;
        second_flag = &(*analysis_slot)->active;
        cursor = *analysis_slot + 1;
        *second_flag = 0;
    }
    {
        BattleWindow4 **paged_slot = &BW4;

        *paged_slot = cursor;
        third_flag = &(*paged_slot)->active;
        cursor = *paged_slot + 1;
        BW3BattleOrDataBase = 0;
        *third_flag = 0;
    }
    endPrintInit();
    return cursor;
}

void eBattleWinInit(void)
{
    eBattleWinInit2(MenuWorkEndGet());
}

void WindowDXFlagChange(WindowDX *window, int flag)
{
    unsigned char current_state;
    unsigned char next_state;

    switch (flag) {
    case WINDOWDX_STATE_OPEN:
        current_state = window->state;
        next_state = current_state + 1;
        if (next_state < 2 || (signed char)current_state == WINDOWDX_STATE_CLOSE ||
            (signed char)current_state == WINDOWDX_STATE_CLOSING) {
            window->state = WINDOWDX_STATE_OPEN;
        }
        return;

    case WINDOWDX_STATE_READY:
        window->state = flag;
        return;

    case WINDOWDX_STATE_CLOSE:
        if (window->state == WINDOWDX_STATE_READY) {
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
    window->state = WINDOWDX_STATE_CLOSED;
    window->ribbon_initialized = 0;
    window->color_g = 0x80;
    window->color_b = 0x80;
    window->color_r = 0x80;
}

void WindowDXMain(WindowDX *window)
{
    char state;

    switch (window->state & 0xf) {
    case WINDOWDX_STATE_CLOSED:
        break;
    case WINDOWDX_STATE_OPEN:
    case 17:
        state = window->state;
        window->state = WINDOWDX_STATE_CLOSED;
        if (window->close_callback != 0) {
            ((WindowDXCallback)window->close_callback)(window, window->close_callback_arg);
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
        window->ribbon_bottom.points[3].red = 32;
        window->ribbon_bottom.points[2].red = 32;
        window->ribbon_bottom.points[1].red = 32;
        window->ribbon_bottom.points[0].red = 32;
        window->ribbon_bottom.points[5].red = 32;
        window->ribbon_bottom.points[4].red = 32;
        window->ribbon_bottom.points[3].green = 15;
        window->ribbon_bottom.points[2].green = 15;
        window->ribbon_bottom.points[1].green = 15;
        window->ribbon_bottom.points[0].green = 15;
        window->ribbon_bottom.points[5].green = 52;
        window->ribbon_bottom.points[4].green = 52;
        window->ribbon_bottom.points[3].blue = 10;
        window->ribbon_bottom.points[2].blue = 10;
        window->ribbon_bottom.points[1].blue = 10;
        window->ribbon_bottom.points[0].blue = 10;
        window->ribbon_bottom.points[5].blue = 64;
        window->ribbon_bottom.points[4].blue = 64;
        window->ribbon_bottom.points[2].alpha = 48;
        window->ribbon_bottom.points[3].alpha = 0;
        window->ribbon_bottom.points[1].alpha = 0;
        window->ribbon_bottom.points[0].alpha = 48;
        window->ribbon_bottom.points[5].alpha = 96;
        window->ribbon_bottom.points[4].alpha = 96;
        if (window->close_callback != 0) {
            ((WindowDXCallback)window->close_callback)(window, window->close_callback_arg);
        }
        window->state = WINDOWDX_STATE_OPENING;
    case WINDOWDX_STATE_OPENING:
        OpenCloseMain(&window->frame);
        if (window->frame.progress >= 0) {
            break;
        }
        window->state = WINDOWDX_STATE_READY;
    case WINDOWDX_STATE_READY:
        window->rect.x = window->x;
        window->rect.y = window->y;
        window->rect.style = window->style;
        window->rect.width = window->width;
        window->rect.height = window->height;
        endPrintExtFunc(window->style, END_PRINT_CMD_WINDOW,&window->rect);
        if (window->tag_id != 0) {
            window->ribbon_top.x = window->x;
            window->ribbon_top.y = window->y - 13;
            window->ribbon_top.work = window->style;
            window->ribbon_top.width = window->width - 2;
            window->ribbon_top.height = 14;
            eRibbonMain(&window->ribbon_top);
            window->title_font.x = window->x + 8;
            window->title_font.y = window->y - 14;
            window->title_font.work = window->style + 2;
            window->title_font.color[3] = 0x80;
            window->title_font.color[1] = 0x80;
            window->title_font.color[2] = 0x80;
            window->title_font.color[0] = 0x80;
            eTagFontMain(&window->title_font);
        }
        if (window->ribbon_initialized == 0) {
            window->ribbon_bottom.x = window->rect.x + 3;
            window->ribbon_bottom.y = window->rect.y + 3;
            window->ribbon_bottom.work = window->rect.style;
            window->ribbon_bottom.width = window->rect.width - 6;
            window->ribbon_bottom.height = window->rect.height - 6;
            eRibbonMain(&window->ribbon_bottom);
        }
        if (window->close_callback != 0) {
            ((WindowDXCallback)window->close_callback)(window, window->close_callback_arg);
        }
        break;
    case WINDOWDX_STATE_CLOSE:
        window->frame.mode = 16;
        window->frame.x = window->x;
        window->frame.y = window->y;
        window->frame.style = window->style;
        window->frame.width = window->width;
        window->frame.height = window->height;
        window->frame.progress = 0;
        if (window->close_callback != 0) {
            ((WindowDXCallback)window->close_callback)(window, window->close_callback_arg);
        }
        window->state = WINDOWDX_STATE_CLOSING;
    case WINDOWDX_STATE_CLOSING:
        OpenCloseMain(&window->frame);
        if (window->frame.progress < 0) {
            window->state = WINDOWDX_STATE_FINISHED;
        }
        break;
    }
}

void subMWModeExSet(WindowSPCursor *list)
{
    int i;
    int j;
    int first = list->current_row * list->visible_cols;
    MenuListEntry *item = list->items + first;
    int shown;
    unsigned char hidden_mode;

    if (list->item_count != 0) {
        shown = list->visible_rows * list->visible_cols;
        if (shown > WINDOWSP_ITEM_SLOTS) {
            shown = WINDOWSP_ITEM_SLOTS;
        }
        if (list->item_count - first < shown) {
            shown = list->item_count - first;
        }
        for (i = 0; i < shown; i++) {
            list->messages[i].text = item->name;
            list->messages[i].mode = E_MODE_SHOW_START;
            list->numbers[i].format = 0;
            list->numbers[i].digits = list->mode;
            list->numbers[i].value = item->amount;
            list->numbers[i].mode = 3;
            if (item->flag == 0) {
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
        for (; i < WINDOWSP_ITEM_SLOTS; i++) {
            list->messages[i].mode = E_MODE_HIDE_START;
            list->numbers[i].mode = 0;
        }
        list->numbers[WINDOWSP_NUMBER_INDEX].digits = 3;
        list->numbers[WINDOWSP_NUMBER_INDEX].value = list->selected_index + 1;
        list->numbers[WINDOWSP_NUMBER_COUNT].digits = 3;
        list->numbers[WINDOWSP_NUMBER_COUNT].value = list->item_count;
        list->cursors[0].active = 1;
        eCursolModeChange(&list->cursors[0], E_MODE_RUN_START);
        if (list->max_scroll != 0) {
            if (list->current_row != 0) {
                eCursolModeChange(&list->cursors[1], E_MODE_RUN_START);
            } else {
                list->cursors[1].state = E_MODE_HIDE_START;
            }
            if (list->current_row == list->max_scroll) {
                list->cursors[2].state = E_MODE_HIDE_START;
            } else if (list->current_row < list->max_scroll) {
                eCursolModeChange(&list->cursors[2], E_MODE_RUN_START);
            } else {
                list->cursors[2].state = E_MODE_HIDE_START;
            }
        } else {
            list->cursors[1].state = E_MODE_HIDE_START;
            list->cursors[2].state = E_MODE_HIDE_START;
        }
    } else {
        hidden_mode = E_MODE_HIDE_START;
        for (j = 2; j >= 0; j--) {
            list->cursors[j].state = hidden_mode;
        }
        list->messages[0].text = item->name;
        list->messages[0].mode = E_MODE_SHOW_START;
        for (j = 1; j < WINDOWSP_ITEM_SLOTS; j++) {
            list->messages[j].mode = E_MODE_HIDE_START;
            list->numbers[j].mode = 0;
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
    list->numbers[WINDOWSP_NUMBER_INDEX].x = list->x + list->width - 142;
    list->numbers[WINDOWSP_NUMBER_COUNT].x = list->x + list->width - 78;
    list->window.x = list->x;
    list->window.y = list->y;
    list->window.style = list->style;
    list->window.width = list->width;
    list->window.height = list->height;
    list->rect.style = list->style;
    list->numbers[WINDOWSP_NUMBER_INDEX].y = list->y - 13;
    list->numbers[WINDOWSP_NUMBER_COUNT].y = list->y - 13;
    list->numbers[WINDOWSP_NUMBER_INDEX].work = list->style + 2;
    list->numbers[WINDOWSP_NUMBER_COUNT].work = list->style + 2;
    if (list->item_count != 0) {
        list->cursors[0].x = list->x + list->visible_top * (list->width / 2) + 3;
        list->cursors[1].y = list->y + 4;
        list->cursors[0].y = list->y + list->visible_bottom * 24 + 7;
        list->cursors[0].work = list->style + 2;
        list->cursors[1].x = list->x + list->width - 17;
        list->cursors[2].x = list->x + list->width - 17;
        list->cursors[2].y = list->y + list->height - 19;
        list->cursors[1].work = list->style + 2;
        list->cursors[2].work = list->style + 2;
        i = 0;
        for (row = 0; row < list->visible_rows; row++) {
            for (col = 0; col < list->visible_cols; col++) {
                list->messages[i].x = list->x + (list->width / 2) * col + 19;
                list->messages[i].y = list->y + row * WINDOWSP_ROW_HEIGHT +3;
                list->messages[i].draw_parameter = list->style + 2;
                i++;
                if (i >= WINDOWSP_ITEM_SLOTS) {
                    break;
                }
            }
        }
        if (list->columns == 2) {
            i = 0;
            for (row = 0; row < list->visible_rows; row++) {
                for (col = 0; col < list->visible_cols; col++) {
                    list->numbers[i].x = list->x + col * (list->width / 2) + 195;
                    list->numbers[i].y = list->y + row * WINDOWSP_ROW_HEIGHT +8;
                    list->numbers[i].work = list->style + 2;
                    i++;
                    if (i >= WINDOWSP_ITEM_SLOTS) {
                        break;
                    }
                }
            }
        } else {
            i = 0;
            for (row = 0; row < list->visible_rows; row++) {
                for (col = 0; col < list->visible_cols; col++) {
                    list->numbers[i].x = list->x + list->width - list->mode * 11 - 19;
                    list->numbers[i].y = list->y + row * WINDOWSP_ROW_HEIGHT +8;
                    list->numbers[i].work = list->style + 2;
                    i++;
                    if (i >= WINDOWSP_ITEM_SLOTS) {
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
            list->numbers[i].work = 0;
        }
        width = list->width;
        list->messages[0].x = list->x + (width - text_width) / 2;
        list->messages[0].y = list->y + (list->height - 24) / 2;
        list->messages[0].draw_parameter = list->style + 2;
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
    if (flags & WINDOWSP_DRAW_CURSORS) {
        for (i = 0; i < WINDOWSP_CURSOR_COUNT; i++) {
            eCursolMain(&list->cursors[i]);
        }
    }
    endPrintExtFunc(list->rect.style, END_PRINT_CMD_SCISSOR_SET,&list->rect);
    if (flags & WINDOWSP_DRAW_MESSAGES) {
        for (i = 0; i < WINDOWSP_ITEM_SLOTS; i++) {
            eMessageMain(&list->messages[i]);
        }
    }
    if (list->item_count != 0) {
        if (flags & WINDOWSP_DRAW_NUMBERS) {
            for (i = 0; i < WINDOWSP_ITEM_SLOTS; i++) {
                eNumberMain(&list->numbers[i]);
            }
        }
    }
    endPrintExtFunc(list->rect.style, END_PRINT_CMD_SCISSOR_RESET, 0);
}

void subMWControlType00(WindowSPCursor *list)
{
    if (list->item_count < 2) {
        return;
    }
    if (list->buttons & PAD_UP) {
        list->selected_index--;
        if (list->selected_index < 0) {
            if (PadData.half_32 & PAD_UP) {
                list->selected_index = list->item_count - 1;
                list->visible_bottom = list->visible_rows - 1;
                list->current_row = list->max_scroll;
                xglSoundEffectNormalID(SE_CURSOR, 0);
            } else {
                list->selected_index++;
            }
        } else {
            if (list->visible_bottom == 0) {
                list->current_row--;
            } else {
                list->visible_bottom--;
            }
            xglSoundEffectNormalID(SE_CURSOR, 0);
        }
    }
    if (list->buttons & PAD_DOWN) {
        list->selected_index++;
        if (list->item_count - 1 < list->selected_index) {
            if (PadData.half_32 & PAD_DOWN) {
                list->selected_index = 0;
                list->visible_bottom = 0;
                list->current_row = 0;
                xglSoundEffectNormalID(SE_CURSOR, 0);
            } else {
                list->selected_index--;
            }
        } else {
            if (list->visible_bottom == list->visible_rows - 1) {
                list->current_row++;
            } else {
                list->visible_bottom++;
            }
            xglSoundEffectNormalID(SE_CURSOR, 0);
        }
    }
}

void subMWControlType01(WindowSPCursor *list)
{
    if (list->item_count < 2) {
        return;
    }
    if (list->buttons & PAD_LEFT) {
        list->selected_index--;
        if (list->selected_index < 0) {
            if (PadData.half_32 & PAD_LEFT) {
                list->selected_index = list->item_count - 1;
                list->current_row = list->max_scroll;
                list->visible_bottom = list->visible_rows - 1;
                xglSoundEffectNormalID(SE_CURSOR, 0);
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
            xglSoundEffectNormalID(SE_CURSOR, 0);
        }
        list->visible_top = list->selected_index % list->visible_cols;
    }
    if (list->buttons & PAD_RIGHT) {
        list->selected_index++;
        if (list->item_count - 1 < list->selected_index) {
            if (PadData.half_32 & PAD_RIGHT) {
                list->selected_index = 0;
                list->current_row = 0;
                list->visible_bottom = 0;
                xglSoundEffectNormalID(SE_CURSOR, 0);
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
            xglSoundEffectNormalID(SE_CURSOR, 0);
        }
        list->visible_top = list->selected_index % list->visible_cols;
    }
    if (list->item_count == 2) {
        return;
    }
    if (list->buttons & PAD_UP) {
        list->selected_index -= list->visible_cols;
        if (list->selected_index < 0) {
            if (PadData.half_32 & PAD_UP) {
                if ((list->item_count - 1) % 2 != 0) {
                    list->selected_index = list->item_count + list->selected_index;
                } else {
                    list->selected_index = list->item_count - 1;
                }
                list->visible_bottom = list->visible_rows - 1;
                list->current_row = list->max_scroll;
                xglSoundEffectNormalID(SE_CURSOR, 0);
            } else {
                list->selected_index += list->visible_cols;
            }
        } else {
            if (list->visible_bottom == 0) {
                list->current_row--;
            } else {
                list->visible_bottom--;
            }
            xglSoundEffectNormalID(SE_CURSOR, 0);
        }
        list->visible_top = list->selected_index % list->visible_cols;
    }
    if (list->buttons & PAD_DOWN) {
        list->selected_index += list->visible_cols;
        if (list->item_count - 1 < list->selected_index) {
            if ((list->item_count - 1) % 2 != 0) {
                if (PadData.half_32 & PAD_DOWN) {
                    list->visible_bottom = 0;
                    list->selected_index -= list->item_count;
                    list->current_row = 0;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                } else {
                    list->selected_index -= list->visible_cols;
                }
            } else if (list->item_count != list->selected_index) {
                if (PadData.half_32 & PAD_DOWN) {
                    list->selected_index = 0;
                    list->visible_bottom = 0;
                    list->current_row = 0;
                    xglSoundEffectNormalID(SE_CURSOR, 0);
                } else {
                    list->selected_index -= list->visible_cols;
                }
            } else {
                list->selected_index = list->item_count - 1;
                list->visible_bottom = list->visible_rows - 1;
                list->current_row = list->max_scroll;
                xglSoundEffectNormalID(SE_CURSOR, 0);
            }
        } else {
            if (list->visible_bottom == list->visible_rows - 1) {
                list->current_row++;
            } else {
                list->visible_bottom++;
            }
            xglSoundEffectNormalID(SE_CURSOR, 0);
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
    MenuListEntry *item = list->items;
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
    while ((item++)->name != 0) {
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
    MenuListEntry *item;
    int count = 0;

    if (list->items == 0) {
        return;
    }
    list->mode = 2;
    item = list->items;
    while ((item++)->name != 0) {
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
    case WINDOWSP_STATE_OPEN:
        sub = list->state >> 4;
        switch (sub) {
        case WINDOWSP_OPEN_FRESH:
            WindowDXSet(&list->window);
            list->window.x = list->x;
            list->window.y = list->y;
            list->window.style = list->style;
            list->window.width = list->width;
            list->window.height = list->height;
            list->window.state = WINDOWDX_STATE_OPEN;
            list->window.tag_id = list->title;
            for (i = 0; i < WINDOWSP_CURSOR_COUNT; i++) {
                eCursolSet(&list->cursors[i], i);
            }
            for (i = 0; i < WINDOWSP_ITEM_SLOTS; i++) {
                eMessageSet(&list->messages[i], 0);
            }
            for (i = 0; i < WINDOWSP_NUMBER_SLOTS; i++) {
                eNumberSet(&list->numbers[i], 0);
            }
            subMWModeExSet(list);
            break;
        case WINDOWSP_OPEN_NO_ANIMATION:
            WindowDXSet(&list->window);
            list->window.x = list->x;
            list->window.y = list->y;
            list->window.style = list->style;
            list->window.width = list->width;
            list->window.height = list->height;
            list->window.tag_id = list->title;
            list->window.state = sub;
            WindowDXMain(&list->window);
            list->window.state = WINDOWDX_STATE_READY;
            for (i = 0; i < WINDOWSP_CURSOR_COUNT; i++) {
                eCursolSet(&list->cursors[i], i);
            }
            for (i = 0; i < WINDOWSP_ITEM_SLOTS; i++) {
                eMessageSet(&list->messages[i], 0);
            }
            for (i = 0; i < WINDOWSP_NUMBER_SLOTS; i++) {
                eNumberSet(&list->numbers[i], 0);
            }
            subMWModeExSet(list);
            break;
        }
        list->state = WINDOWSP_STATE_OPEN_WAIT;
    case WINDOWSP_STATE_OPEN_WAIT:
        WindowDXMain(&list->window);
        if (list->window.state != WINDOWDX_STATE_READY) {
            break;
        }
        list->state = WINDOWSP_STATE_ACTIVE;
        list->cursors[0].state = E_MODE_SHOW_START;
        for (i = 0; i < WINDOWSP_ITEM_SLOTS; i++) {
            list->messages[i].mode = E_MODE_SHOW_START;
            list->numbers[i].mode = 3;
        }
        list->anim = 0;
    case WINDOWSP_STATE_ACTIVE:
        subMWModeExSet(list);
        subMWPosSet(list);
        subMWDraw(list);
        break;
    case WINDOWSP_STATE_CLOSE:
        list->window.x = list->x;
        list->window.y = list->y;
        list->window.style = list->style;
        list->window.width = list->width;
        list->window.height = list->height;
        list->window.state = WINDOWDX_STATE_CLOSE;
        list->window.tag_id = list->title;
        list->state = WINDOWSP_STATE_CLOSING;
        list->window.close_callback_arg = 0;
        list->window.close_callback = 0;
    case WINDOWSP_STATE_CLOSING:
        WindowDXMain(&list->window);
        if (list->window.state == WINDOWDX_STATE_CLOSED) {
            list->state = WINDOWSP_STATE_CLOSED;
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

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
const char informationTagText[16] = "Information";

const char rareItemPrefixText[16] = "Rare Item: ";

const char weaknessPrefixText[16] = "Weakness: ";

const char analysisTagText[16] = "Analysis";

const char MenuFileHardDiskDrivePath[24] = "/Hard Disk Drive";

const char MenuFileMemoryCardSlot2Path[24] = "/MEMORY CARD slot 2";

const char MenuFileMemoryCardSlot1Path[32] = "/MEMORY CARD slot 1";

const BattleWinConditionLabelTable conditionLabelTable = { { (const unsigned short *)conditionBText, (const unsigned short *)conditionSpText, (const unsigned short *)conditionLText, (const unsigned short *)conditionFText, (const unsigned short *)conditionIText, (const unsigned short *)conditionPrText, (const unsigned short *)conditionSlText, (const unsigned short *)conditionHtText, (const unsigned short *)conditionArText, (const unsigned short *)conditionCuText } };
