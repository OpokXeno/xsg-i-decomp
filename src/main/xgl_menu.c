#include "common.h"
#include "shared.h"
#include "main/xgl_menu.h"
#include "xgl_menu.h"
#include "main/xgl_2.h"
#include "main/xgl_font.h"

/* XglMenuEntry.type: what the entry draws. */
#define XGL_MENU_TYPE_SPRITE 0
#define XGL_MENU_TYPE_LIST 1

/* XglMenuEntry.active for a list menu: the open animation grows each level's
 * row spacing, RUNNING takes input, CLOSE_LEVEL shrinks the top level back
 * out, CLOSE_ALL shrinks every level and then hands over the result. */
#define XGL_MENU_ACTIVE_OPEN 1
#define XGL_MENU_ACTIVE_RUNNING 2
#define XGL_MENU_ACTIVE_CLOSE_LEVEL 3
#define XGL_MENU_ACTIVE_CLOSE_ALL 4

#define XGL_MENU_LEVEL_COUNT 8         /* row_offsets / items depth */
#define XGL_MENU_NODE_WRAP 1           /* node->initial.flags: cursor wraps */
#define XGL_MENU_SELECT_WRITE_BACK 1   /* entry->select_flags: write result at once */
#define XGL_MENU_RESULT_LIMIT 0x100000 /* below: a result value; above: a node pointer */

/* Port records retain the original 0x68-byte stride. */
typedef struct MenuPadRecord {
    unsigned char unmodeled_00[0x32];
    unsigned short direction_pressed;
    unsigned short direction_repeated;
    unsigned char unmodeled_36[0x32];
} MenuPadRecord;

extern XglMenuEntry menutbl[MENU_TABLE_COUNT];
extern MenuPadRecord PadData[2];

static void xglMenuDrawType1(XglMenuEntry *entry,
                             XglMenuDrawContext *context);

XglMenuEntry menutbl[MENU_TABLE_COUNT] = { 0 };

/* Text: {color #800000}%s */
const char darkRedTextFormat[8] = "\f\x80\0\0%s";

/* Text: {color #303030}%s */
const char dimTextFormat[8] = "\f000%s";

/* Text: {reset}%s */
const char plainTextFormat[4] = "\v%s";

const char xglMenuPfs0Path[8] = "pfs0:";

const char xglMenuPfs1Path[8] = "pfs1:";

const char xglMenuHddPath[8] = "hdd:";

/* Original four-byte object; its element type is unresolved. */
unsigned char ActiveMenuNo[4] = { 0 };

int xglMenuOpen(int slot, XglMenuParams *params)
{
    XglMenuListData *list;
    XglMenuEntry *entry;
    XglMenuSpriteData *sprite;
    int i;

    if (slot == -1) {
        slot = 0;
        while (slot < MENU_TABLE_COUNT && menutbl[slot].active) {
            slot++;
        }
        if (slot == MENU_TABLE_COUNT) {
            return -1;
        }
    }

    entry = &menutbl[slot];
    entry->type = params->type;
    entry->select_flags = params->select_flags;
    entry->active = XGL_MENU_ACTIVE_OPEN;
    entry->root = params->root;
    entry->default_result = params->default_result;
    entry->result_out = params->result_out;

    switch (params->type) {
    case XGL_MENU_TYPE_SPRITE:
        sprite = &entry->payload.sprite;
        sprite->alpha = 100;
        sprite->flags = 0;
        sprite->x = I2F(params->x);
        sprite->y = I2F(params->y);
        sprite->z = I2F(params->z);
        sprite->unit_params[0] = 1.0f;
        sprite->unit_params[1] = 1.0f;
        sprite->unit_params[2] = 1.0f;
        sprite->unit_params[3] = 1.0f;
        sprite->unit_params[4] = 1.0f;
        sprite->offsets[0] = 0;
        sprite->offsets[1] = 0;
        sprite->offsets[2] = 0;
        sprite->offsets[3] = 0;
        for (i = 0; i < entry->root->item_count; i++) {
            entry->sprite_row_marks[i] = 0x3000;
        }
        break;
    case XGL_MENU_TYPE_LIST:
        list = &entry->payload.list;
        list->current_index = 0;
        list->position_x = params->x;
        list->position_y = params->y;
        list->position_z = params->z;
        list->flags = 0;
        list->items[0] = params->root;
        break;
    }
    return slot;
}

static void xglMenuDrawType0(XglMenuEntry *entry, XglMenuDrawContext *context) {
    entry->active = 0;
}

static void xglMenuDrawType1Sub(XglMenuEntry *entry,
                                XglMenuDrawContext *context, unsigned int level)
{
    XglMenuListData *list;
    XglMenuNode *node;
    XglMenuRow *row;
    int row_index;

    row_index = 0;
    list = &entry->payload.list;
    node = list->items[level];
    while (row_index < node->item_count) {
        row = &node->rows[row_index];
        if (row_index == node->selected_item) {
            xglFontDebugPrintf(context->x, context->y,
                               darkRedTextFormat, row->text);
        } else if (row->next.node == (XglMenuNode *)-1) {
            xglFontDebugPrintf(context->x, context->y,
                               dimTextFormat, row->text);
        } else {
            xglFontDebugPrintf(context->x, context->y,
                               plainTextFormat, row->text);
        }

        context->y += list->row_offsets[level];

        if (level < list->current_index &&
            list->items[level + 1] == row->next.node) {
            context->x += MENU_LIST_INDENT;
            xglMenuDrawType1Sub(entry, context, level + 1);
            context->x -= MENU_LIST_INDENT;
        }
        row_index++;
    }
}

static void xglMenuDrawType1(XglMenuEntry *entry,
                             XglMenuDrawContext *context)
{
    XglMenuListData *list;
    XglMenuNode *node;
    XglMenuNode *next;
    short *row_offset;
    int level;
    int selected_item;
    int value;
    int i;
    int has_offset;

    list = &entry->payload.list;
    level = list->current_index;
    node = list->items[level];

    switch (entry->active) {
    case XGL_MENU_ACTIVE_OPEN:
        if (list->row_offsets[level] < node->initial.row_spacing) {
            list->row_offsets[level]++;
        } else {
            entry->active = XGL_MENU_ACTIVE_RUNNING;
        }
        break;
    case XGL_MENU_ACTIVE_RUNNING:
        if ((PadData[0].direction_repeated & PAD_UP) != 0) {
            selected_item = node->selected_item;
            do {
                selected_item--;
                if (selected_item < 0) {
                    if ((node->initial.flags & XGL_MENU_NODE_WRAP) != 0) {
                        selected_item = node->item_count - 1;
                    } else {
                        selected_item = node->selected_item;
                    }
                    break;
                }
            } while (node->rows[selected_item].next.result == -1);
            node->selected_item = selected_item;
        }
        if ((PadData[0].direction_repeated & PAD_DOWN) != 0) {
            selected_item = node->selected_item;
            do {
                selected_item++;
                if (selected_item >= node->item_count) {
                    selected_item = 0;
                    if ((node->initial.flags & XGL_MENU_NODE_WRAP) == 0) {
                        selected_item = node->selected_item;
                    }
                    break;
                }
            } while (node->rows[selected_item].next.result == -1);
            node->selected_item = selected_item;
        }
        if ((PadData[0].direction_pressed & PAD_CIRCLE) != 0) {
            if (node->rows[node->selected_item].next.result < XGL_MENU_RESULT_LIMIT) {
                entry->result_value =
                    node->rows[node->selected_item].next.result;
                if ((entry->select_flags & XGL_MENU_SELECT_WRITE_BACK) != 0) {
                    if (entry->result_out != 0) {
                        *entry->result_out = entry->result_value;
                    }
                } else {
                    entry->active = XGL_MENU_ACTIVE_CLOSE_ALL;
                }
            } else {
                next = node->rows[node->selected_item].next.node;
                list->current_index++;
                list->items[list->current_index] = next;
                if (next->initial.initial_selected_item >= 0) {
                    next->selected_item = next->initial.initial_selected_item;
                }
                if (entry->result_out != 0) {
                    value = next->enter_result;
                    *entry->result_out = value;
                }
                entry->active = XGL_MENU_ACTIVE_OPEN;
            }
        }
        if ((PadData[0].direction_pressed & PAD_CROSS) != 0) {
            if (entry->result_out != 0) {
                value = node->cancel_result;
                *entry->result_out = value;
            }
            if (list->current_index == 0) {
                entry->active = XGL_MENU_ACTIVE_CLOSE_ALL;
                entry->result_value = entry->default_result;
            } else {
                entry->active = XGL_MENU_ACTIVE_CLOSE_LEVEL;
            }
        }
        break;
    case XGL_MENU_ACTIVE_CLOSE_LEVEL:
        if (list->row_offsets[level] > 0) {
            list->row_offsets[level]--;
        } else {
            list->current_index = level - 1;
            entry->active = XGL_MENU_ACTIVE_RUNNING;
        }
        break;
    case XGL_MENU_ACTIVE_CLOSE_ALL:
        has_offset = 0;
        row_offset = list->row_offsets;
        i = XGL_MENU_LEVEL_COUNT - 1;
        do {
            if (*row_offset > 0) {
                has_offset = 1;
                (*row_offset)--;
            }
            i--;
            row_offset++;
        } while (i >= 0);
        if (has_offset == 0) {
            if (entry->result_out != 0) {
                *entry->result_out = entry->result_value;
            }
            entry->active = 0;
        }
        break;
    }

    context->x = list->position_x;
    context->y = list->position_y;
    xglMenuDrawType1Sub(entry, context, 0);
}

void xglMenuDraw(void)
{
    XglMenuEntry *entry = menutbl;
    XglMenuDrawContext *context = EE_SCRATCHPAD_BASE;
    int i;

    for (i = MENU_TABLE_COUNT - 1; i >= 0; i--, entry++) {
        if (entry->active) {
            switch (entry->type) {
            case XGL_MENU_TYPE_SPRITE:
                xglMenuDrawType0(entry, context);
                break;
            case XGL_MENU_TYPE_LIST:
                xglMenuDrawType1(entry, context);
                break;
            }
        }
    }
}

void xglMenuInitial(void)
{
    int i;

    for (i = MENU_TABLE_COUNT - 1; i >= 0; i--) {
        menutbl[i].active = 0;
    }
}
