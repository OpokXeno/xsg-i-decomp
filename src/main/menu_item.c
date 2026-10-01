#include "common.h"
#include "shared.h"

#define MENU_ITEM_SPECIAL_ID 36
#define MENU_ITEM_USE_ALLOWED 0x8000
#define MENU_ITEM_USE_CATEGORY_BITS 0x1e43
#define MENU_ITEM_USE_SPECIAL_BIT 0x2000
#define MENU_ITEM_USE_STATE_BIT 0x10
#define MENU_ITEM_SPECIAL_ENABLED_BITS 0x20400000

typedef struct {
    u8 unmodeled_00[4];
    u16 use_flags; /* The high bit gates all item availability checks. */
    u8 unmodeled_06[2];
    u8 use_state;
    u8 unmodeled_09[3];
    u16 attributes;
} MenuItemUseData;

typedef struct {
    u8 unmodeled_00[0x10];
    u64 flags;
} MenuItemLoopState;

extern MenuItemUseData *func_A1A4E8(s16 itemId);
extern MenuItemLoopState GameLoopState;

int MenuItemUseCheck(s16 itemId)
{
    MenuItemUseData *item;
    u16 attributes;
    int result;
    /* A special item is unavailable when both game-loop bits are clear. */
    int specialUnavailable = -1;

    if (itemId == 0) {
        return 0;
    }
    item = func_A1A4E8(itemId);
    if ((item->use_flags & MENU_ITEM_USE_ALLOWED) == 0) {
        return 0;
    }
    if (itemId == MENU_ITEM_SPECIAL_ID) {
        result = (GameLoopState.flags & MENU_ITEM_SPECIAL_ENABLED_BITS) ? 2 : specialUnavailable;
    } else {
        attributes = item->attributes;
        if (attributes & MENU_ITEM_USE_CATEGORY_BITS) {
            result = (item->use_state & MENU_ITEM_USE_STATE_BIT) ? 2 : 1;
        } else {
            result = (attributes & MENU_ITEM_USE_SPECIAL_BIT) ? 10 : 0;
        }
    }
    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemPasMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemInfoMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemLineMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemStatusMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemStatusAgwsMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemListMake00);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemListMake00_1);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemListMake01);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemListMake02);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemListMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemSelectMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemIconMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemExMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", subRoboPartsCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemSegmentMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItem);
