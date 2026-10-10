#include "common.h"

#include "shared.h"

typedef struct XglMenuNode XglMenuNode;

typedef struct XglMenuRow {
    const char *text;
    union { XglMenuNode *node; int result; } next;
} XglMenuRow;

struct XglMenuNode {
    unsigned char item_count;
    struct { signed char initial_selected_item; unsigned char flags; unsigned char row_spacing; } initial;
    signed char selected_item;
    unsigned char unmodeled_05[3];
    int enter_result;
    int cancel_result;
    unsigned char unmodeled_10[8];
    XglMenuRow *rows;
};

typedef struct XglMenuListData {
    short position_x;
    short position_y;
    short position_z;
    unsigned char flags;
    unsigned char unmodeled_07;
    int current_index;
    XglMenuNode *items[8];
    short row_offsets[8];
} XglMenuListData;

typedef struct XglMenuDrawContext {
    unsigned char unmodeled_00[4];
    int x;
    int y;
} XglMenuDrawContext;

typedef struct XglMenuSpriteData {
    unsigned char alpha;
    unsigned char flags;
    unsigned char unmodeled_02[14];
    float x;
    float y;
    float z;
    float unit_params[5];
    short offsets[4];
    unsigned char unmodeled_38[4];
} XglMenuSpriteData;

typedef struct XglMenuParams {
    unsigned char type;
    unsigned char select_flags;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    short z;
    unsigned char unmodeled_0a[2];
    int default_result;
    XglMenuNode *root;
    int *result_out;
} XglMenuParams;

#define EE_SCRATCHPAD_BASE ((XglMenuDrawContext *)0x70000000)

#define MENU_LIST_INDENT 8

extern void xglFontDebugPrintf(int x, int y, const char *format, ...);
extern const char D_004DC2B8[];
extern const char D_004DC2C0[];
extern const char D_004DC2C8[];

/*
 * Partial view of one entry of the menutbl array (main 0x0021ce18/0x0021ce7c,
 * a 0x80-byte stride): xglMenuDrawType0 only touches the active flag at
 * offset 7, which xglMenuDraw (main 0x0021ce30) reads first and skips the
 * entry when it is zero.
 */
typedef struct {
    XglMenuNode *root;                  /* +0x00 */
    unsigned char type;                 /* +0x04 */
    unsigned char select_flags;         /* +0x05 */
    unsigned char unmodeled_06;
    unsigned char active;               /* +0x07 */
    /*
     * xglMenuInitial (main 0x0021ce98) indexes menutbl by this stride to
     * clear the active flag of every record; the rest of the record is not
     * recovered.
     */
    int default_result;
    int result_value;
    int *result_out;
    unsigned char unmodeled_14[0x0c];
    union {
        XglMenuListData list;
        XglMenuSpriteData sprite;
    } payload;
    short sprite_row_marks[18];
} XglMenuEntry;

#define MENU_TABLE_COUNT 16

extern XglMenuEntry menutbl[MENU_TABLE_COUNT];


/* Port records retain the original 0x68-byte stride. */
typedef struct MenuPadRecord {
    unsigned char unmodeled_00[0x32];
    unsigned short direction_pressed;
    unsigned short direction_repeated;
    unsigned char unmodeled_36[0x32];
} MenuPadRecord;
extern MenuPadRecord PadData[2];

XglMenuEntry menutbl[MENU_TABLE_COUNT] = { 0 };

const char D_004DC2B8[8] = "\f\x80\0\0%s";

const char D_004DC2C0[8] = "\f000%s";

const char D_004DC2C8[4] = "\v%s";

const char D_004DC2D0[8] = "pfs0:";

const char D_004DC2D8[8] = "pfs1:";

const char D_004DC2E0[8] = "hdd:";

extern float I2F(int value);

static void xglMenuDrawType1(XglMenuEntry *entry,
                             XglMenuDrawContext *context);

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
    entry->active = 1;
    entry->root = params->root;
    entry->default_result = params->default_result;
    entry->result_out = params->result_out;

    switch (params->type) {
    case 0:
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
    case 1:
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
                               D_004DC2B8, row->text);
        } else if (row->next.node == (XglMenuNode *)-1) {
            xglFontDebugPrintf(context->x, context->y,
                               D_004DC2C0, row->text);
        } else {
            xglFontDebugPrintf(context->x, context->y,
                               D_004DC2C8, row->text);
        }

        context->y += list->row_offsets[level];

        if (level < (unsigned int)list->current_index &&
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
    case 1:
        if (list->row_offsets[level] < node->initial.row_spacing) {
            list->row_offsets[level]++;
        } else {
            entry->active = 2;
        }
        break;
    case 2:
        if ((PadData[0].direction_repeated & 0x1000) != 0) {
            selected_item = node->selected_item;
            do {
                selected_item--;
                if (selected_item < 0) {
                    if ((node->initial.flags & 1) != 0) {
                        selected_item = node->item_count - 1;
                    } else {
                        selected_item = node->selected_item;
                    }
                    break;
                }
            } while (node->rows[selected_item].next.result == -1);
            node->selected_item = selected_item;
        }
        if ((PadData[0].direction_repeated & 0x4000) != 0) {
            selected_item = node->selected_item;
            do {
                selected_item++;
                if (selected_item >= node->item_count) {
                    selected_item = 0;
                    if ((node->initial.flags & 1) == 0) {
                        selected_item = node->selected_item;
                    }
                    break;
                }
            } while (node->rows[selected_item].next.result == -1);
            node->selected_item = selected_item;
        }
        if ((PadData[0].direction_pressed & 0x20) != 0) {
            if (node->rows[node->selected_item].next.result < 0x100000) {
                entry->result_value =
                    node->rows[node->selected_item].next.result;
                if ((entry->select_flags & 1) != 0) {
                    if (entry->result_out != 0) {
                        *entry->result_out = entry->result_value;
                    }
                } else {
                    entry->active = 4;
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
                entry->active = 1;
            }
        }
        if ((PadData[0].direction_pressed & 0x40) != 0) {
            if (entry->result_out != 0) {
                value = node->cancel_result;
                *entry->result_out = value;
            }
            if (list->current_index == 0) {
                entry->active = 4;
                entry->result_value = entry->default_result;
            } else {
                entry->active = 3;
            }
        }
        break;
    case 3:
        if (list->row_offsets[level] > 0) {
            list->row_offsets[level]--;
        } else {
            list->current_index = level - 1;
            entry->active = 2;
        }
        break;
    case 4:
        has_offset = 0;
        row_offset = list->row_offsets;
        i = 7;
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
            case 0:
                xglMenuDrawType0(entry, context);
                break;
            case 1:
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
