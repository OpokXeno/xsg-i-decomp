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

typedef struct MenuItemListRow {
    u8 unmodeled_00[8];
    u8 flag;
    u8 state;
    u8 unmodeled_0a[2];
} MenuItemListRow;

typedef struct MenuItemWindowSP {
    u8 unmodeled_00;
    u8 rowCount;
    u8 unmodeled_02[2];
    short x;
    short y;
    int color;
    short width;
    short height;
    const char *title;
    signed char columns;
    signed char rows;
    u8 unmodeled_16[6];
    MenuItemListRow *items;
} MenuItemWindowSP;

typedef struct MenuItemListWork {
    u8 unmodeled_00[0x0C];
    MenuItemWindowSP window;
} MenuItemListWork;

typedef struct MenuItemMessage {
    u8 unmodeled_00;
    u8 mode;
    u8 unmodeled_02[2];
    short x;
    short y;
    int color;
} MenuItemMessage;

typedef struct MenuItemExWork {
    u8 state;
    u8 unmodeled_01[3];
    int color;
    MenuItemMessage message;
} MenuItemExWork;

typedef struct MenuItemPasWindow {
    short x;
    short y;
    int color;
    short width;
    short height;
    u8 unmodeled_0c[0x10 - 0x0C];
    signed char state;
    u8 unmodeled_11[0x14 - 0x11];
    void (*callback)(void);
    void *callbackArg;
    u8 unmodeled_1c[0x194 - 0x1C];
} MenuItemPasWindow;

typedef struct MenuItemPasMessage {
    u8 unmodeled_00;
    u8 mode;
    u8 unmodeled_02[2];
    short x;
    short y;
    int color;
    u8 unmodeled_0c[0x44 - 0x0C];
} MenuItemPasMessage;

typedef struct MenuItemPasBox {
    short x;
    short y;
    int color;
    short width;
    short height;
} MenuItemPasBox;

typedef struct MenuItemPasWork {
    u8 state;
    u8 unmodeled_01[3];
    short x;
    short y;
    int color;
    MenuItemPasWindow window;
    MenuItemPasMessage message[9];
    MenuItemPasBox box;
    u8 callbackWork[1];
} MenuItemPasWork;

typedef struct MenuItemWorkData {
    u8 unmodeled_00[3];
    u8 mode;
    u8 unmodeled_04[0x2C];
    signed char listTab;
    signed char sortAbility;
    signed char sortNormal;
    signed char sortCategory[3];
    u8 unmodeled_36[0x1A];
    int selectDir;
} MenuItemWorkData;

extern MenuItemPasWork *MenuItemPas;

extern void *MenuItemInfo;

extern void *MenuItemLine;

extern void *MenuItemStatus;

extern void *MenuItemStatusAgws;

extern MenuItemListWork *MenuItemList;

extern void *MenuItemSelect;

extern void *MenuItemIcon;

extern MenuItemExWork *MenuItemEx;

extern void *MenuItemSegment;

extern MenuItemWorkData MenuWork;

extern void MenuPasWindow(void);

extern void WindowDXSet(void *window);

extern void WindowDXMain(void *window);

extern void endPrintExtFunc(int color, int id, void *data);

extern unsigned char MenuKeepSelect[];

const char D_004C4F50[16] = "List\x03\x8d" "Num";

const char D_004C4F60[32] = "List\x03\x99Num\x03\x99\x03" "3Num";



extern int *MenuSortAddrGet(int list);

extern MenuItemListRow *MenuListGet(int list);

extern void MenuListMake(int list, int mode);

extern int MenuSortCheck(int list);

extern void MenuSortSet(int list, int type, int order);

extern void MenuSortChange(int list, int index);

extern void WindowSPItemChange(MenuItemWindowSP *window);

extern void WindowSPSetSelect(MenuItemWindowSP *window, unsigned char *savedSelection);

extern void MoveSlide(short *current, short *target, float rate);

extern void eMessageSet(void *message, const char *text);

extern void eMessageMain(void *message);

extern int xglFlagsGet(int flag, int mode);

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

void MenuItemPasMain(void)
{
    static const char *msg00[9] = {
        (const char *)0x004DAAB8,
        (const char *)0x004C4E70,
        (const char *)0x004C4E58,
        (const char *)0x004C4E40,
        (const char *)0x004C4E30,
        (const char *)0x004C4E18,
        (const char *)0x004C4E00,
        (const char *)0x004C4DF0,
        (const char *)0x004C4DE0,
    };
    MenuItemPasWork *pas = MenuItemPas;
    short tabTarget[9];
    short slideTarget;
    int i;

    switch (pas->state) {
    case 0:
        pas->x = -272;
        pas->y = 8;
        pas->color = 0xFFFFF0;
        WindowDXSet(&pas->window);
        pas->window.x = pas->x;
        pas->window.y = pas->y;
        pas->window.color = pas->color;
        pas->window.width = 272;
        pas->window.height = 30;
        pas->window.callback = MenuPasWindow;
        pas->window.callbackArg = pas->callbackWork;
        pas->window.state = 1;
        WindowDXMain(&pas->window);
        pas->window.state = 3;
        for (i = 0; i < 9; i++) {
            eMessageSet(&pas->message[i], msg00[i]);
            pas->message[i].mode = 32;
            pas->message[i].x = 288;
            pas->message[i].color = pas->color + 2;
            pas->message[i].y = pas->y + 3;
        }
        pas->state = 2;
        /* fallthrough */
    case 2:
        break;
    default:
        return;
    }

    slideTarget = -16;
    for (i = 0; i < 9; i++)
        tabTarget[i] = 288;

    switch (MenuWork.mode) {
    case 0x40:
    case 0x80:
    case 0x20:
    case 0x22:
        tabTarget[MenuWork.listTab] = 16;
        break;
    case 0x30:
        tabTarget[8] = 16;
        break;
    default:
        slideTarget = -272;
        break;
    }

    MoveSlide(&pas->window.x, &slideTarget, 3.0f);
    WindowDXMain(&pas->window);
    pas->box.x = pas->window.x + 3;
    pas->box.y = pas->window.y + 3;
    pas->box.color = pas->color;
    pas->box.width = pas->window.width - 6;
    pas->box.height = pas->window.height - 6;
    endPrintExtFunc(pas->color, 0x65, &pas->box);
    for (i = 0; i < 9; i++) {
        MoveSlide(&pas->message[i].x, &tabTarget[i], 3.0f);
        if (pas->message[i].x < 256)
            eMessageMain(&pas->message[i]);
    }
    endPrintExtFunc(pas->color, 0x66, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemInfoMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemLineMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemStatusMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemStatusAgwsMain);

void MenuItemListMake00(void)
{
    MenuItemListRow *rows = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    MenuItemListWork *work;
    int count;
    int i;

    MenuSortSet(0, 1, 0);
    MenuSortChange(0, 0);
    MenuSortChange(0, MenuWork.sortAbility);
    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        int use = MenuItemUseCheck((short) sortRow[i]);

        switch (use) {
        case -1:
            rows[i].flag = 1;
            rows[i].state = 2;
            break;
        case 0:
            rows[i].flag = 1;
            rows[i].state = 1;
            break;
        case 1:
        case 2:
        default:
            rows[i].flag = 0;
            rows[i].state = 0;
            break;
        }
    }
    work = MenuItemList;
    work->window.rowCount = 7;
    work->window.title = D_004C4F50;
    MenuItemList->window.columns = 1;
    MenuItemList->window.rows = 11;
    MenuItemList->window.width = 224;
    MenuItemList->window.height = MenuItemList->window.rows * 24 + 6;
    MenuItemList->window.items = MenuListGet(0);
    WindowSPItemChange(&MenuItemList->window);
    WindowSPSetSelect(&MenuItemList->window, MenuKeepSelect);
}

void MenuItemListMake00_1(void)
{
    MenuItemListRow *rows = MenuListGet(0);
    int *sortRow = MenuSortAddrGet(0);
    MenuItemListWork *work;
    int count;
    int i;

    MenuListMake(0, 0);
    count = MenuSortCheck(0);
    for (i = 0; i < count; i++) {
        int use = MenuItemUseCheck((short) sortRow[i]);

        switch (use) {
        case -1:
            rows[i].flag = 1;
            rows[i].state = 2;
            break;
        case 0:
            rows[i].flag = 1;
            rows[i].state = 1;
            break;
        case 1:
        case 2:
            rows[i].flag = 0;
            rows[i].state = 0;
            break;
        }
    }
    work = MenuItemList;
    work->window.rowCount = 7;
    work->window.title = D_004C4F50;
    MenuItemList->window.columns = 1;
    MenuItemList->window.rows = 11;
    MenuItemList->window.width = 224;
    MenuItemList->window.height = MenuItemList->window.rows * 24 + 6;
    MenuItemList->window.items = MenuListGet(0);
    WindowSPItemChange(&MenuItemList->window);
    WindowSPSetSelect(&MenuItemList->window, MenuKeepSelect);
}

void MenuItemListMake01(int category)
{
    MenuItemListWork *work;

    switch (category) {
    case 0:
        MenuSortSet(0, 4, -1);
        break;
    case 1:
        MenuSortSet(0, 16, -1);
        break;
    case 2:
        MenuSortSet(0, 8, -1);
        break;
    case 3:
        MenuSortSet(0, 4, -2);
        break;
    case 4:
        MenuSortSet(0, 16, -2);
        break;
    case 5:
        MenuSortSet(0, 8, -2);
        break;
    }
    MenuSortChange(0, 0);
    MenuSortChange(0, MenuWork.sortCategory[category]);
    MenuListMake(0, 0);
    work = MenuItemList;
    work->window.rowCount = 7;
    work->window.title = D_004C4F60;
    MenuItemList->window.columns = 2;
    MenuItemList->window.rows = 11;
    MenuItemList->window.width = 468;
    MenuItemList->window.height = MenuItemList->window.rows * 24 + 6;
    MenuItemList->window.items = MenuListGet(0);
    WindowSPItemChange(&MenuItemList->window);
    WindowSPSetSelect(&MenuItemList->window, &MenuKeepSelect[category * 5 + 10]);
}

void MenuItemListMake02(void)
{
    struct MenuItemListWork *list;

    MenuSortSet(0, 2, 0);
    MenuSortChange(0, 0);
    MenuSortChange(0, MenuWork.sortNormal);
    MenuListMake(0, 0);

    list = MenuItemList;
    list->window.rowCount = 7;
    list->window.title = D_004C4F60;
    MenuItemList->window.columns = 2;
    MenuItemList->window.rows = 11;
    MenuItemList->window.width = 0x1D4;
    MenuItemList->window.height = MenuItemList->window.rows * 0x18 + 6;
    MenuItemList->window.items = MenuListGet(0);

    WindowSPItemChange(&MenuItemList->window);
    WindowSPSetSelect(&MenuItemList->window, &MenuKeepSelect[5]);
}

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemListMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemSelectMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemIconMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemExMain);

int subRoboPartsCheck(int part)
{
    /* Reading the result back through its address keeps it in memory until
     * the address is purged, so case 3 becomes a conditional move only in
     * if-conversion, which keeps the original xori v0,v0,0. */
    int *resultAddress;
    int result = 0;

    switch (part) {
    case 1:
        result = 1;
        if (xglFlagsGet(3301, 1)) {
            result = 65;
        }
        break;
    case 2:
        result = 2;
        if (xglFlagsGet(3303, 1)) {
            result = 130;
        }
        break;
    case 9:
        result = 4;
        if (xglFlagsGet(3302, 1)) {
            result = 268;
        }
        break;
    case 7:
        result = 8;
        if (xglFlagsGet(3302, 1)) {
            result = 268;
        }
        break;
    case 8:
        result = 16;
        if (xglFlagsGet(3304, 1)) {
            result = 560;
        }
        break;
    case 3:
        result = (xglFlagsGet(3304, 1) == 0) ? (32) : (560);
        break;
    }
    resultAddress = &result;
    return *resultAddress;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItemSegmentMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_item", MenuItem);