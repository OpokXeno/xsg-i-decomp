#include "common.h"
#include "shared.h"
#include "xgl_menu.h"

XglMenuEntry menutbl[MENU_TABLE_COUNT] = { 0 };

const char D_004DC2B8[8] = "\f\x80\0\0%s";
const char D_004DC2C0[8] = "\f000%s";
const char D_004DC2C8[4] = "\v%s";
const char D_004DC2D0[8] = "pfs0:";
const char D_004DC2D8[8] = "pfs1:";
const char D_004DC2E0[8] = "hdd:";

INCLUDE_ASM("asm/main/nonmatchings/xgl_menu", xglMenuOpen);

static void xglMenuDrawType0(XglMenuEntry *entry) {
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
    list = &entry->list;
    node = list->items[level];
    while (row_index < node->item_count) {
        row = &node->rows[row_index];
        if (row_index == node->selected_item) {
            xglFontDebugPrintf(context->x, context->y,
                               D_004DC2B8, row->text);
        } else if (row->next == (XglMenuNode *)-1) {
            xglFontDebugPrintf(context->x, context->y,
                               D_004DC2C0, row->text);
        } else {
            xglFontDebugPrintf(context->x, context->y,
                               D_004DC2C8, row->text);
        }

        context->y += list->row_offsets[level];

        if (level < (unsigned int)list->current_index &&
            list->items[level + 1] == row->next) {
            context->x += MENU_LIST_INDENT;
            xglMenuDrawType1Sub(entry, context, level + 1);
            context->x -= MENU_LIST_INDENT;
        }
        row_index++;
    }
}

INCLUDE_ASM("asm/main/nonmatchings/xgl_menu", xglMenuDrawType1);

INCLUDE_ASM("asm/main/nonmatchings/xgl_menu", xglMenuDraw);

void xglMenuInitial(void)
{
    int i;

    for (i = MENU_TABLE_COUNT - 1; i >= 0; i--) {
        menutbl[i].active = 0;
    }
}
