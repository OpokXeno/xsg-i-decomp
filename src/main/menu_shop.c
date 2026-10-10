#include "common.h"

#include "shared.h"

#include "main/xgl_task.h"

#include "menu_shop.h"

static MenuShopWorkData *MenuShopWork;

/* Referenced original data recovered from this translation unit. */

const char D_004C7858[40] = "List                   Price";

extern const char D_004DB230[];

typedef struct ShopDataCategory {
    unsigned short count;
    unsigned char unmodeled_02[38];
} ShopDataCategory;

typedef struct ShopDataTail {
    unsigned short count;
    unsigned char unmodeled_02[6];
} ShopDataTail;

typedef struct MenuShopListRow {
    const char *name;
    int amount;
    unsigned char flag;
} MenuShopListRow;

typedef struct MenuShopWindowDX {
    short x;
    short y;
    int color;
    short width;
    short height;
    const char *title;
    unsigned char state;
    unsigned char unmodeled_11[0x183];
} MenuShopWindowDX;

typedef struct MenuShopMessage {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x0c];
    const char *text;
} MenuShopMessage;

typedef struct MenuShopEx2Work {
    unsigned char unmodeled_00[4];
    int color;
    MenuShopWindowDX window;
    MenuShopMessage message;
} MenuShopEx2Work;

typedef struct MenuShopWeapon {
    unsigned char unmodeled_00[6];
    unsigned short flags;
    unsigned char unmodeled_08[0x12];
    short bullet;
} MenuShopWeapon;

/* The map anchors nine 0x28-byte count records and a separate 8-byte tail.
 * The remaining bytes stay explicitly unmodeled. */

static ShopDataCategory ShopData[9];

static ShopDataTail ShopDataTailStorage __attribute__((section(".bss")));

extern int MenuScenarioNo;

extern void *MenuSortAddrGet(int list);

extern int MenuSortGet(int list, int index);

extern MenuShopListRow *MenuListGet(int list);

extern int MenuSortCheck(int list);

extern int MenuBoxMoneyGet(int item, int mode);

extern int dataMoneyBoxChk(void);

extern int MenuBoxChk(int item);

extern int subMenuShopEquipCheck(int item, int category);

extern int MenuShopNoSaleCheck(int item);

extern void MenuSortSet(int list, int type, int order);

extern int dataEvtBoxChk(int event_id);

extern void MoveSlide(short *current, short *target, float rate);

extern void eMessageSet(void *message, const char *text);

extern void eMessageMain(void *message);

extern MenuShopWeapon *func_A1A3D8(short item);

extern const char **func_A2C738(int bullet);

typedef struct MenuShopWindowSP {
    unsigned char unmodeled_00;
    unsigned char row_count;
    unsigned char unmodeled_02[0x0A];
    short width;
    short height;
    const char *title;
    unsigned char columns;
    unsigned char rows;
    unsigned char unmodeled_16[0x06];
    MenuShopListRow *items;
    unsigned char unmodeled_20[0x06];
    unsigned char state;
} MenuShopWindowSP;

static MenuShopWindowSP *MenuShopWinSP;

extern unsigned char MenuKeepSelect[0x64];

extern MenuShopListRow *MenuListMake(int list, int mode);

extern void MenuShopSortSet(int list, int type, int order, int option);

/* Referenced original data recovered from this translation unit. */

#include "main/xgl_studio.h"

/* Referenced original data recovered from this translation unit. */

struct MenuShopPasMessage {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x0c];
    const char *text;
    unsigned char unmodeled_1c[0x28];
};

struct MenuShopModelTransform {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
    Vector4 rotation;
    unsigned char unmodeled_30[0x30];
    float fade;
};

struct MenuShopModelActor {
    unsigned char unmodeled_00[0xC0];
    struct MenuShopModelTransform transform;
};

struct MenuShopModelUnit {
    unsigned char unmodeled_00[0x10];
    signed char taskPhase;
    unsigned char active;
    unsigned char unmodeled_12;
    unsigned char state;
    unsigned char unmodeled_14[0x0C];
    struct MenuShopModelActor *actor;
};

struct MenuShopModelCallbackData {
    unsigned char unmodeled_00[0x10];
    unsigned char state;
    unsigned char unmodeled_11[0x0B];
    int modelId;
};

/* The map anchors nine 0x28-byte count records and a separate 8-byte tail.
 * The remaining bytes stay explicitly unmodeled. */

struct MenuShopAgwsListState {
    unsigned char unmodeled_00[0x160];
    unsigned short itemIds[4];
};

struct AgwsModelUnit;
extern void MenuModelUnitBreak(struct AgwsModelUnit *unit);

extern void MenuModelUnitOpen(struct AgwsModelUnit *unit, int drawType);

void *func_A191C0(int);

/* Partial unit record used by the shop's repair and equipment operations. */
typedef struct MenuShopUnitData {
    unsigned char unmodeled_00[0x34];
    unsigned short hitPoints;
    unsigned char unmodeled_36[0x20];
    short engineId;
    short frameId;
} MenuShopUnitData;

struct MenuShopListWork {
    unsigned char state;
    unsigned char active;
    unsigned char initialFlags[2];
    int category;
    int color;
    MenuShopWindowSP window;
};

struct MenuShopSortOption {
    unsigned char type;
    unsigned char unmodeled_01[3];
    int order;
};

struct MenuShopSortOptionTable {
    struct MenuShopSortOption entries[9];
};

typedef struct MenuShopLineContext {
    unsigned char unmodeled_00[0x10];
    unsigned char mode;
} MenuShopLineContext;

typedef struct MenuShopLineRow {
    int text_id;
    unsigned char style[4];
    short position;
    short target;
} MenuShopLineRow;

typedef struct MenuShopLineDisplay {
    unsigned char unmodeled_00[3];
    unsigned char phase;
    int text_id;
    short top_position;
    MenuShopLineRow rows[5];
    int unmodeled_48;
} MenuShopLineDisplay;

struct MenuShopRowCountTable {
    unsigned char values[18];
};

extern int MenuShopListChange00(int list);

typedef signed char s8;

typedef int s32;

#define NULL ((void *)0)

typedef struct { unsigned char bytes[8]; } M2C_BLOCK8;

typedef struct MenuShopSortEntry {
    const short *itemIds;
    int highWord;
    int count;
} MenuShopSortEntry;

typedef struct MenuShopSortTable {
    MenuShopSortEntry entries[11];
} MenuShopSortTable;

extern const MenuShopSortTable D_004C7738;

struct MenuShopEquipRecord {
    unsigned char unmodeled_00[0x5e];
    short itemIds[3][3];
};

struct MenuShopEquipScratch {
    int partyIds[15];
    int slotIndex[9];
};

int PartyAgwsGet(int *);

typedef struct { unsigned char bytes[4]; } M2C_BLOCK4;

typedef union {
    struct {
        M2C_BLOCK8 first;
        M2C_BLOCK8 second;
        M2C_BLOCK4 third;
        u16 last;
    } blocks;
    s16 ids[11];
} MenuShopIconIds;

typedef long long s64;

/* Partial recalculated record: calcTotalParaMenu's maxHp/maxEp pair. */
typedef struct ShopCalculatedPara {
    s16 maxHp;
    s16 maxEp;
} ShopCalculatedPara;

ShopCalculatedPara *func_00A11108(int, int *, int *);

void MenuAgwsParaSet(int, int);

s32 MenuEngineEquipCheck(int, int);

s32 MenuFrameEquipCheck(s32, s32);

/* MenuShopNoSaleCheck reads the halfword at +0x4 (original VA 0x002a3c88). */
typedef struct MenuShopAccessoryData {
    unsigned char unmodeled_00[4];
    unsigned short flags;
} MenuShopAccessoryData;

extern MenuShopAccessoryData *func_A1A548(int item);

#define s8 signed char

#define s32 int

#define s64 long long

int MenuBoxDec(unsigned int);

int MenuBoxInc(unsigned int);

s32 MenuCharHpCheck(s16);

s32 MenuCursorKeepCheck(void);

int MenuSelectMove(int, int, int);

int MenuSelectMove2(int, int, int);

void WindowSPKeepSelect(MenuShopWindowSP *, u8 *);

void WindowSPKeepSelectCheck(u8 *);

int dataMoneyBoxDec(s32);

int dataMoneyBoxInc(s32);

void xglFontDebugHex(int, int, unsigned int, int);

void xglFontDebugPrintf(int, int, const char *, ...);

extern const char D_004C7A68[];

extern const char D_004C7A78[];

extern const char D_004C7A88[];

extern const char D_004C7A98[];

extern u8 D_004DB238[];

extern u8 D_004DB240[];

extern u8 D_004DB248[];

extern u8 D_004DB250[];

typedef struct MenuShopItemData {
    unsigned char unmodeled_00[4];
    unsigned short flags;
} MenuShopItemData;

typedef struct MenuShopAttackData {
    unsigned char unmodeled_00[10];
    unsigned short flags;
} MenuShopAttackData;

extern MenuShopWeapon *dataWpnGet(int item);

extern MenuShopAttackData *dataAttGet(int item);

extern MenuShopItemData *func_A1A4E8(int item);

#include "e_battle_win_open.h"

typedef struct AgwsModelTransform {
    unsigned char unmodeled_00[0x10];
    Vector4 position;
    Vector4 rotation;
    unsigned char unmodeled_30[0x60 - 0x30];
    float fade;
} AgwsModelTransform;

typedef struct AgwsModelActor {
    unsigned char unmodeled_00[0xC0];
    AgwsModelTransform transform;
} AgwsModelActor;

typedef struct AgwsModelOrg {
    unsigned char unmodeled_00[0x5A];
    signed char hand[3];
    unsigned char unmodeled_5d[0x5E - 0x5D];
    short weaponId[3];
} AgwsModelOrg;

typedef struct AgwsModelUnit {
    unsigned char unmodeled_00[0x10];
    signed char taskPhase;
    unsigned char active;
    unsigned char unmodeled_12[1];
    unsigned char state;
    AgwsModelOrg *org;
    unsigned char unmodeled_18[0x20 - 0x18];
    AgwsModelActor *actor;
} AgwsModelUnit;

typedef struct MenuShopPasMessageSlot {
    unsigned char unmodeled_00;
    unsigned char mode;
    unsigned char unmodeled_02[2];
    short x;
    short y;
    int color;
    unsigned char unmodeled_0c[0x0c];
    const char *text;
    unsigned char unmodeled_1c[0x28];
} MenuShopPasMessageSlot;

typedef struct MenuShopPasBounds {
    short x1;
    short y1;
    int color;
    short x2;
    short y2;
} MenuShopPasBounds;

typedef struct MenuShopPasWindow {
    WindowDX common;
    unsigned char unmodeled_30[0x194 - 0x30];
} MenuShopPasWindow;

typedef struct MenuShopPasData {
    unsigned char unmodeled_00[4];
    short x;
    short y;
    int color;
    MenuShopPasWindow window;
    MenuShopPasMessageSlot messages[16];
    MenuShopPasBounds bounds;
} MenuShopPasData;

/* Referenced original data recovered from this translation unit. */

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopAgwsListChange);

static void TskObjectSet3(TskObject *task, TskObjectWorker worker, void *data)
{
    task->data = data;
    task->worker = worker;
    task->state = 0;
}

static void tskTskMain3(TskObject *task)
{
    TskObjectWorker worker = task->worker;

    if (MenuShopWork->state == 0xff) {
        xglTaskWaitRemove(&task->base);
        return;
    }

    if (task->state != 0) {
        if (task->state != 2)
            return;
    } else {
        worker(task, task->data);
        task->state = 2;
    }

    worker(task, task->data);
}

void MenuShopModelDisp(AgwsModelUnit *unit)
{
    AgwsModelActor *actor = unit->actor;
    AgwsModelTransform *transform;
    int state;

    if (actor == 0) {
        if (MenuShopWork->modelPending != 0) {
            MenuModelUnitBreak(unit);
            MenuShopWork->modelPending = 0;
        }
    }

    if (unit->active != 0) {
        transform = &actor->transform;
        if (MenuShopWork->modelPending != 0) {
            unit->state = 0x1e;
            MenuShopWork->modelPending = 0;
        }

        state = unit->state;
        switch (state) {
        case 10:
            break;
        case 0:
            transform->position.y = -0.2f;
            MenuModelUnitOpen(unit, 2);
            unit->state = 10;
            return;
        case 0x1e:
            MenuModelUnitOpen(unit, 1);
            unit->state = 0x1f;
            /* fall through */
        case 0x1f:
            if (transform->fade == 0.0f)
                unit->taskPhase = -1;
            break;
        default:
            break;
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopModelMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopPas);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopInfo);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", ListMake_3);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopSelect);

void MenuShopSortSet(int list, int type, int order, int option)
{
    MenuShopSortTable table;
    int *output = MenuSortAddrGet(list);

    table = D_004C7738;

    if (type == 0x12)
        type = 9;
    if (type == 0x13)
        type = 10;

    if (type == 8) {
        if (MenuScenarioNo >= 0x73) {
            *output++ = 0x20035;
            *output++ = 0x20036;
        }
        if (MenuScenarioNo >= 0x12D)
            *output++ = 0x20037;
    } else {
        int i = 0;

        if (table.entries[type].count > 0) {
            const short *itemIds = table.entries[type].itemIds;

            do {
                int itemId = *itemIds++;
                unsigned int itemValue = (unsigned short)itemId;

                if (itemId == 0)
                    break;
                *output++ = itemValue + ((unsigned int)table.entries[type].highWord << 16);
                i++;
            } while (i < table.entries[type].count);
        }
    }

    *output = 0;
}

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", subMenuShopEquipCheck);

int MenuShopNoSaleCheck(int item)
{
    int itemId;
    int category;
    int flags;

    itemId = item & 0xFFFF;
    category = (unsigned int)item >> 16;
    flags = 0;

    if (itemId == 0)
        return 0;

    switch (category) {
    case 1:
        flags = func_A1A4E8(itemId)->flags;
        break;
    case 3:
        flags = dataWpnGet(itemId)->flags;
        break;
    case 4:
        flags = dataAttGet(itemId)->flags;
        break;
    case 5:
        flags = func_A1A548(itemId)->flags;
        break;
    }

    if ((flags & 0x1000) != 0)
        return 1;
    return 0;
}

void MenuShopListColorChange(int list, int kind)
{
    int *sorted = MenuSortAddrGet(list);
    int count = MenuSortCheck(list);
    MenuShopListRow *entry = MenuListGet(list);
    int i;

    if (MenuShopWork->mode == 0 || kind == 2) {
        for (i = 0; i < count; i++) {
            if (MenuBoxMoneyGet(MenuSortGet(list, i), 0) > dataMoneyBoxChk())
                entry[i].flag = 1;
        }
    } else {
        for (i = 0; i < count; i++) {
            int noSale;

            MenuBoxChk(MenuSortGet(list, i));
            subMenuShopEquipCheck(MenuSortGet(list, i), 0);
            noSale = MenuShopNoSaleCheck(sorted[i]);
            if (noSale == 1)
                entry[i].flag = noSale;
        }
    }
}

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopListChange00);

int MenuShopListChange01(int list)
{
    int type = 18;
    MenuShopWindowSP *window;
    short window_width;

    if (MenuShopWork->sort_mode_selector == 0)
        type = 19;

    MenuShopSortSet(0, type, 0, 0);
    MenuListMake(0, -10);
    MenuShopListColorChange(0, 2);

    window = MenuShopWinSP;
    MenuShopWinSP->rows = 9;
    window_width = 272;
    window->height = 222;
    window->width = window_width;
    MenuShopWinSP->columns = 1;
    MenuShopWinSP->title = D_004C7858;
    MenuShopWinSP->row_count = 7;
    MenuShopWinSP->items = MenuListGet(0);
    WindowSPItemChange((void *)MenuShopWinSP);
    WindowSPSetSelect((void *)MenuShopWinSP, &MenuKeepSelect[type * 5]);

    MenuShopWinSP->state = 6;
    return WindowSPSelect((void *)MenuShopWinSP, 0);
}

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopList);

void MenuShopEx(void *task, void *data);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopEx);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopSeisan);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopSeisan02);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopEx02);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopIcon);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopParaSet2);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopParaSet);

void MenuShopParameter(TskObject *task, void *data);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopParameter);

static void MenuShopEx2(TskObject *task, MenuShopEx2Work *work)
{
    short x;
    MenuShopWeapon *weapon;

    switch (task->state) {
    case 0:
        work->color = 0xFFFF00;
        WindowDXSet((void *)&work->window);
        work->window.x = -0xB6;
        work->window.color = work->color;
        work->window.y = 0x100;
        work->window.width = 0xA6;
        work->window.height = 0x1E;
        work->window.title = D_004DB230;
        work->window.state = 1;
        WindowDXMain((void *)&work->window);
        work->window.state = 3;
        eMessageSet(&work->message, 0);
        work->message.mode = 0x20;
        work->message.color = work->color + 2;
        break;

    case 2:
        x = -0xB6;
        if (MenuShopWork->state == 0x30 && MenuShopWork->category == 4 && MenuShopWork->listIndex >= 0) {
            weapon = func_A1A3D8(MenuSortGet(0, MenuShopWork->listIndex));
            if (weapon != 0 && (weapon->flags & 0x100) && weapon->bullet != 0) {
                work->message.text = *func_A2C738(weapon->bullet);
                x = 0x10;
            }
        }
        MoveSlide(&work->window.x, &x, 3.0f);
        WindowDXMain((void *)&work->window);
        work->message.x = work->window.x + 3;
        work->message.y = work->window.y + 3;
        eMessageMain(&work->message);
        break;
    }
}

/* MenuShopEx2 takes the window title by address; the string follows it in .sdata. */
const char D_004DB230[8] = "Bullet";

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopLine);

void MenuShopInfoSet(void)
{
    MenuShopWork->categoryMask = 0;

    if (MenuShopWork->mode == 0) {
        if (MenuShopWork->noCategories != 0)
            return;
        if (ShopData[0].count != 0)
            MenuShopWork->categoryMask = 1;
        if (ShopData[1].count != 0)
            MenuShopWork->categoryMask |= 0x2;
        if (ShopData[2].count != 0)
            MenuShopWork->categoryMask |= 0x4;
        if (ShopData[3].count != 0)
            MenuShopWork->categoryMask |= 0x8;
        if (ShopData[4].count != 0)
            MenuShopWork->categoryMask |= 0x10;
        if (ShopData[5].count != 0)
            MenuShopWork->categoryMask |= 0x20;
        if (ShopData[6].count != 0)
            MenuShopWork->categoryMask |= 0x40;
        if (ShopData[9].count != 0)
            MenuShopWork->categoryMask |= 0x80;
        if (MenuScenarioNo >= 115)
            MenuShopWork->categoryMask |= 0x100;
    } else {
        MenuSortSet(0, 1, 0);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x1;
        MenuSortSet(0, 4, -1);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x2;
        MenuSortSet(0, 16, -1);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x4;
        MenuSortSet(0, 8, -1);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x8;
        MenuSortSet(0, 4, -2);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x10;
        MenuSortSet(0, 16, -2);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x20;
        MenuSortSet(0, 8, -2);
        if (MenuSortCheck(0) != 0)
            MenuShopWork->categoryMask |= 0x40;
        if (dataEvtBoxChk(53) != 0 || dataEvtBoxChk(54) != 0 || dataEvtBoxChk(55) != 0)
            MenuShopWork->categoryMask |= 0x100;
    }
}


/* Each list selection updates the item and its signed model key together. */
#define MENU_SHOP_PREVIEW_ITEM(listIndex) \
    { \
        MenuShopWork->itemId = MenuSortGet(0, (listIndex)); \
        MenuShopWork->modelId = (s16) MenuShopWork->itemId; \
    }

typedef struct MenuShopSelection {
    u8 mode;
    u8 category;
    s8 quantity;
    u8 quantityAction;
    u8 quantityLimit;
} MenuShopSelection;

typedef struct MenuShopItem {
    int listIndex;
    int itemId;
    int modelId;
} MenuShopItem;

typedef struct MenuShopAgws {
    short unitId;
    short equipmentId;
    signed char equipmentStatus;
    signed char unitCount;
    unsigned char unmodeled_06;
    signed char unitSelection;
    signed char sort_mode_selector;
} MenuShopAgws;

static inline MenuShopSelection *MenuShopSelectionGet(void)
{
    return (MenuShopSelection *)&MenuShopWork->mode;
}

static inline MenuShopItem *MenuShopItemGet(void)
{
    return (MenuShopItem *)&MenuShopWork->listIndex;
}

static inline MenuShopAgws *MenuShopAgwsGet(void)
{
    return (MenuShopAgws *)&MenuShopWork->unitId;
}

typedef struct MenuShopParty {
    short unitIds[6];
} MenuShopParty;

static inline MenuShopParty *MenuShopPartyGet(void)
{
    return (MenuShopParty *)MenuShopWork->unitIds;
}

void MenuShopCore(void) {
    if (MenuShopWork->state != MenuShopWork->nextState) {
        MenuShopWork->state = MenuShopWork->nextState;
        MenuShopWork->stateFlags |= 1;
    } else {
        MenuShopWork->stateFlags &= 0xFE;
    }
    if (MenuShopWork->waitFrames != 0) {
        MenuShopWork->waitFrames--;
    }

    switch (MenuShopWork->state) {
    case 0x20:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->stateFlags |= 0x10;
            MenuShopWork->waitFrames = 8;
            break;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->mode = MenuSelectMove(MenuShopWork->mode, 4, 0);
        if (PadData.half_2a == 0x20)
            switch (MenuShopWork->mode) {
            case 0:
            case 1: {
                u8 listStates[2] = { 0x30, 0x90 };
                u8 *nextStates = listStates;
                int categoryIndex;

                MenuShopInfoSet();
                if (MenuShopWork->categoryMask != 0) {
                    for (categoryIndex = 0; categoryIndex < 8; categoryIndex++) {
                        if ((MenuShopWork->categoryMask >> categoryIndex) & 1) {
                            MenuShopWork->category = categoryIndex;
                            break;
                        }
                    }
                    MenuShopWork->nextState = nextStates[MenuShopWork->mode];
                    MenuShopWork->stateFlags |= 0xC;
                    xglSoundEffectNormalID(1, 0);
                    MenuShopWork->waitFrames = 0xC;
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
                break;
            }
            case 2:
                if (MenuShopWork->equipmentStatus != 0) {
                    xglSoundEffectNormalID(1, 0);
                    MenuShopWork->nextState = 0xC0;
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
                break;
            case 3:
                xglSoundEffectNormalID(2, 0);
                MenuShopWork->nextState = 0xF0;
                break;
            }
        if (PadData.half_2a & 0x40) {
            MenuShopWork->nextState = 0xF0;
            xglSoundEffectNormalID(2, 0);
        }
        break;

    case 0x30:
    case 0x90:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->listIndex = MenuShopListChange00(1);
            MenuShopWork->itemId = MenuShopWork->modelId = 0;
            MenuShopWork->availableMoney = dataMoneyBoxChk();
        }
        if (MenuShopWork->waitFrames == 0) {
            MenuShopWork->listIndex = WindowSPSelect((void *)MenuShopWinSP, PadData.half_34);
            WindowSPKeepSelect(MenuShopWinSP, &MenuKeepSelect[(MenuShopWork->category + MenuShopWork->mode * 9) * 5]);
            if (MenuShopWork->listIndex >= 0) {
                MenuShopWork->canEquip = subMenuShopEquipCheck(MenuSortGet(0, MenuShopWork->listIndex), MenuShopWork->category);
            } else {
                MenuShopWork->canEquip = 0;
            }
            if (MenuShopWork->listIndex >= 0) {
                MENU_SHOP_PREVIEW_ITEM(MenuShopWork->listIndex);
            } else {
                MenuShopWork->itemId = *(unsigned int *)&MenuShopWork->modelId = 0;
            }
            if (PadData.half_34 & 0xA000) {
                u8 previousCategory;
                int categoryStep;
                int category;
                int count;

                previousCategory = MenuShopWork->category;
                WindowSPKeepSelectCheck(&MenuKeepSelect[(previousCategory + MenuShopWork->mode * 9) * 5]);
                categoryStep = (PadData.half_34 & 0x2000) ? 1 : -1;
                category = MenuShopWork->category;
                for (count = 0; count < 9; count += categoryStep) {
                    category += categoryStep;
                    if (category < 0) {
                        category = 8;
                    }
                    if (category >= 9) {
                        category = 0;
                    }
                    if ((MenuShopWork->categoryMask >> category) & 1) {
                        MenuShopWork->category = category;
                        break;
                    }
                }
                if (previousCategory != MenuShopWork->category) {
                    xglSoundEffectNormalID(1, 0);
                    MenuShopWork->listIndex = MenuShopListChange00(0);
                    if (MenuShopWork->listIndex >= 0) {
                        MenuShopWork->canEquip = subMenuShopEquipCheck(MenuSortGet(0, MenuShopWork->listIndex), MenuShopWork->category);
                        MENU_SHOP_PREVIEW_ITEM(MenuShopWork->listIndex);
                    }
                    MenuShopWork->waitFrames = 8;
                }
            } else if (PadData.half_2a & 0x20) {
                if (MenuSortCheck(0) != 0) {
                    if (MenuShopWork->mode == 0) {
                        if (MenuBoxMoneyGet(MenuShopWork->itemId, 0) > MenuShopWork->availableMoney
                            || MenuBoxChk(MenuShopWork->itemId) >= 99) {
                            xglSoundEffectNormalID(5, 0);
                        } else {
                            MenuShopWork->nextState = 0xA0;
                            MenuShopWork->quantity = 1;
                            MenuShopWork->quantityLimit = MenuBoxChk(MenuShopWork->itemId);
                            xglSoundEffectNormalID(1, 0);
                        }
                    } else if (MenuShopNoSaleCheck(MenuShopWork->itemId) != 0) {
                        xglSoundEffectNormalID(5, 0);
                    } else {
                        MenuShopWork->nextState = 0xA0;
                        MenuShopWork->quantity = 1;
                        MenuShopWork->quantityLimit = MenuBoxChk(MenuShopWork->itemId);
                        xglSoundEffectNormalID(1, 0);
                    }
                }
            } else if (PadData.half_2a & 0x40) {
                if (MenuCursorKeepCheck() == 0) {
                    MenuShopWork->mode = 0;
                }
                MenuShopWork->nextState = 0x20;
                xglSoundEffectNormalID(2, 0);
            }
        } else {
            MenuShopWork->listIndex = WindowSPSelect((void *)MenuShopWinSP, 0);
        }
        break;

    case 0xA0: {
        struct MenuShopQuantityChangeState {
            int maximumQuantity;
            int calculatedTotal;
            int playChangeSound;
            int minimumQuantity;
        } quantityState;
        int i;

        void subMenuShopNumerInc(void)
        {
            int unitCost;

            if (MenuShopWork->quantity < quantityState.maximumQuantity) {
                unitCost = MenuBoxMoneyGet(MenuShopWork->itemId, 0);
                quantityState.calculatedTotal = unitCost * (MenuShopWork->quantity + 1);
                if (MenuShopWork->availableMoney >= quantityState.calculatedTotal) {
                    MenuShopWork->quantity++;
                    if (quantityState.playChangeSound != 0) {
                        xglSoundEffectNormalID(3, 0);
                        quantityState.playChangeSound = 0;
                    }
                }
            }
        }

        void subMenuShopNumerDec(void)
        {
            if (quantityState.minimumQuantity < MenuShopWork->quantity) {
                MenuShopWork->quantity = MenuShopWork->quantity - 1;
                if (quantityState.playChangeSound != 0) {
                    xglSoundEffectNormalID(3, 0);
                    quantityState.playChangeSound = 0;
                }
            }
        }

        if (MenuShopWork->stateFlags & 1) {
            if (MenuShopWork->category == 7) {
                MenuShopWork->nextState = 0xA2;
                MenuShopWork->waitFrames = 1;
            } else {
                MenuShopWork->waitFrames = 8;
            }
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        if (MenuShopWork->mode == 0) {
            quantityState.maximumQuantity = 99 - MenuShopWork->quantityLimit;
            quantityState.minimumQuantity = 1;
            quantityState.playChangeSound = 1;
            quantityState.calculatedTotal = 0;
            if (PadData.half_34 & 0x4000) {
                subMenuShopNumerDec();
            }
            if (PadData.half_34 & 0x1000) {
                subMenuShopNumerInc();
            }
            if (PadData.half_34 & 0x2000) {
                for (i = 0; i < 10; i++) {
                    subMenuShopNumerInc();
                }
            }
            if (PadData.half_34 & 0x8000) {
                for (i = 0; i < 10; i++) {
                    subMenuShopNumerDec();
                }
            }
        } else {
            int minimumQuantity = 1;
            u8 quantityLimit;

            quantityLimit = MenuShopSelectionGet()->quantityLimit;
            if ((PadData.half_34 & 0x4000) && MenuShopSelectionGet()->quantity != minimumQuantity) {
                MenuShopSelectionGet()->quantity--;
                xglSoundEffectNormalID(3, 0);
            }
            if ((PadData.half_34 & 0x1000) && MenuShopSelectionGet()->quantity != quantityLimit) {
                MenuShopSelectionGet()->quantity++;
                xglSoundEffectNormalID(3, 0);
            }
            if ((PadData.half_34 & 0x2000) && MenuShopSelectionGet()->quantity != quantityLimit) {
                MenuShopSelectionGet()->quantity += 10;
                if (MenuShopSelectionGet()->quantity > quantityLimit) {
                    MenuShopSelectionGet()->quantity = quantityLimit;
                }
                xglSoundEffectNormalID(3, 0);
            }
            if ((PadData.half_34 & 0x8000) && MenuShopSelectionGet()->quantity != minimumQuantity) {
                MenuShopSelectionGet()->quantity -= 10;
                if (MenuShopSelectionGet()->quantity < minimumQuantity) {
                    MenuShopSelectionGet()->quantity = minimumQuantity;
                }
                xglSoundEffectNormalID(3, 0);
            }
        }
        if (PadData.half_2a & 0x20) {
            MenuShopWork->nextState = 0xA2;
            xglSoundEffectNormalID(1, 0);
        }
        if (PadData.half_2a & 0x40) {
            if (MenuShopWork->mode == 0) {
                MenuShopWork->nextState = 0x30;
            } else {
                MenuShopWork->nextState = 0x90;
            }
            xglSoundEffectNormalID(2, 0);
        }
        break;
    }

    case 0xA2:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->quantityAction = 0;
            MenuShopWork->waitFrames = 8;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->quantityAction = MenuSelectMove(MenuShopWork->quantityAction, 2, 0);
        if (PadData.half_2a & 0x20) {
            if (MenuShopSelectionGet()->quantityAction == 0) {
                int i;

                if (MenuShopSelectionGet()->mode == 0) {
                    for (i = 0; i < MenuShopSelectionGet()->quantity; i++) {
                        MenuBoxInc(MenuSortGet(0, MenuShopWork->listIndex));
                    }
                    dataMoneyBoxDec(MenuBoxMoneyGet(MenuShopWork->itemId, 0) * MenuShopWork->quantity);
                } else {
                    for (i = 0; i < MenuShopSelectionGet()->quantity; i++) {
                        MenuBoxDec(MenuShopWork->itemId);
                    }
                    dataMoneyBoxInc(MenuBoxMoneyGet(MenuShopWork->itemId, 1) * MenuShopWork->quantity);
                    MenuShopWork->stateFlags |= 0x10;
                }
                MenuShopWork->nextState = 0xA4;
            } else if (MenuShopSelectionGet()->category == 7) {
                u8 listStates[2] = { 0x30, 0x90 };

                MenuShopWork->nextState = *(listStates + MenuShopSelectionGet()->mode);
            } else {
                MenuShopWork->nextState = 0xA0;
            }
            xglSoundEffectNormalID(1, 0);
        }
        if (PadData.half_2a & 0x40) {
            if (MenuShopSelectionGet()->category == 7) {
                u8 listStates[2] = { 0x30, 0x90 };

                MenuShopWork->nextState = *(listStates + MenuShopSelectionGet()->mode);
            } else {
                MenuShopWork->nextState = 0xA0;
            }
            xglSoundEffectNormalID(2, 0);
        }
        break;

    case 0xA4:
        if (MenuShopWork->stateFlags & 1) {
            if (MenuShopWork->mode == 0 && MenuShopWork->category == 7) {
                int partyUnitIds[16];
                int i;

                MenuShopWork->unitCount = PartyAgwsGet(partyUnitIds);
                for (i = 0; i < MenuShopWork->unitCount; i++) {
                    MenuShopPartyGet()->unitIds[i] = partyUnitIds[i];
                }
                MenuShopAgwsListChange();
            }
            MenuShopWork->listIndex = MenuShopListChange00(1);
            MenuShopWork->waitFrames = 8;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        if (PadData.half_2a & 0x20) {
            u8 listStates[2] = { 0x30, 0x90 };

            MenuShopWork->nextState = *(listStates + MenuShopWork->mode);
            if (MenuShopWork->mode == 0) {
                MenuShopListColorChange(0, 0);
            }
            xglSoundEffectNormalID(1, 0);
        }
        break;

    case 0xC0:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->waitFrames = 8;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->sortOption = MenuSelectMove(MenuShopWork->sortOption, 4, 0);
        if (PadData.half_32 == 0x20) {
            switch (MenuShopWork->sortOption) {
            case 0:
            case 1:
                if (MenuShopWork->sortOption == 0) {
                    MenuShopWork->sort_mode_selector = 1;
                } else {
                    MenuShopWork->sort_mode_selector = 0;
                }
                xglSoundEffectNormalID(1, 0);
                if (MenuShopWork->sort_mode_selector != 0) {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[0x5A]);
                } else {
                    WindowSPKeepSelectCheck(&MenuKeepSelect[0x5F]);
                }
                MenuShopWork->listIndex = MenuShopListChange01(0);
                MenuShopWork->nextState = 0xD0;
                MenuShopWork->waitFrames = 0xC;
                break;
            case 2:
                if (dataMoneyBoxChk() >= 100) {
                    xglSoundEffectNormalID(1, 0);
                    MenuShopWork->nextState = 0xC2;
                } else {
                    xglSoundEffectNormalID(5, 0);
                }
                MenuShopWork->waitFrames = 0xC;
                break;
            case 3:
                xglSoundEffectNormalID(2, 0);
                MenuShopWork->nextState = 0x20;
                break;
            }
        }
        if (PadData.half_32 == 0x40) {
            if (MenuCursorKeepCheck() == 0) {
                MenuShopWork->mode = 0;
            }
            MenuShopWork->nextState = 0x20;
            xglSoundEffectNormalID(2, 0);
        }
        break;

    case 0xC2:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->waitFrames = 0xC;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->unitSelection = MenuSelectMove2(MenuShopAgwsGet()->unitSelection, MenuShopAgwsGet()->unitCount, 0);
        MenuShopAgwsGet()->unitId = MenuShopPartyGet()->unitIds[MenuShopAgwsGet()->unitSelection];
        if (PadData.half_32 == 0x20) {
            if (dataMoneyBoxChk() >= 100 && MenuCharHpCheck(MenuShopWork->unitId) == 0) {
                MenuShopWork->nextState = 0xC4;
                xglSoundEffectNormalID(1, 0);
            } else {
                xglSoundEffectNormalID(5, 0);
            }
        }
        if (PadData.half_32 == 0x40) {
            if (MenuCursorKeepCheck() == 0) {
                MenuShopWork->sortOption = 0;
            }
            MenuShopWork->nextState = 0xC0;
            xglSoundEffectNormalID(2, 0);
        }
        break;

    case 0xC4:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->quantityAction = 0;
            MenuShopWork->waitFrames = 8;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->quantityAction = MenuSelectMove(MenuShopWork->quantityAction, 2, 0);
        if (PadData.half_32 == 0x20) {
            if (MenuShopWork->quantityAction == 0) {
                MenuShopWork->nextState = 0xC6;
                xglSoundEffectNormalID(1, 0);
            } else {
                MenuShopWork->nextState = 0xC2;
                xglSoundEffectNormalID(2, 0);
            }
        }
        if (PadData.half_32 == 0x40) {
            MenuShopWork->nextState = 0xC2;
            xglSoundEffectNormalID(2, 0);
        }
        break;

    case 0xC6:
        if (MenuShopWork->stateFlags & 1) {
            int repairAttack[4];
            int repairDefense[4];
            MenuShopUnitData *unit;
            ShopCalculatedPara *para;

            unit = func_A191C0(MenuShopPartyGet()->unitIds[MenuShopWork->unitSelection]);
            para = func_00A11108(MenuShopPartyGet()->unitIds[MenuShopWork->unitSelection], repairAttack, repairDefense);
            unit->hitPoints = para->maxHp;
            dataMoneyBoxDec(100);
            MenuShopWork->waitFrames = 2;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        if (PadData.half_32 == 0x20) {
            if (dataMoneyBoxChk() >= 100) {
                MenuShopWork->nextState = 0xC2;
            } else {
                MenuShopWork->nextState = 0xC0;
            }
            xglSoundEffectNormalID(1, 0);
        }
        break;

    case 0xD0:
        if (MenuShopWork->waitFrames == 0) {
            MenuShopWork->listIndex = WindowSPSelect((void *)MenuShopWinSP, PadData.half_34);
            if (MenuShopWork->sort_mode_selector != 0) {
                WindowSPKeepSelect(MenuShopWinSP, &MenuKeepSelect[0x5A]);
            } else {
                WindowSPKeepSelect(MenuShopWinSP, &MenuKeepSelect[0x5F]);
            }
            if (MenuShopWork->listIndex >= 0) {
                int itemId;
                int unitId;

                itemId = MenuSortGet(0, MenuShopWork->listIndex);
                MenuShopItemGet()->itemId = itemId;
                MenuShopItemGet()->modelId = (s16)MenuShopItemGet()->itemId;
                if (MenuShopItemGet()->listIndex >= 0) {
                    MenuShopAgwsGet()->equipmentId = MenuShopItemGet()->modelId;
                    MenuShopAgwsGet()->unitId = 0x11;
                    for (unitId = 0x11; unitId < 0x21; unitId++) {
                        if (MenuFrameEquipCheck(unitId, MenuShopWork->equipmentId) != 0) {
                            MenuShopWork->unitId = unitId;
                            break;
                        }
                    }
                    if (PadData.half_32 == 0x20) {
                        int money;

                        money = dataMoneyBoxChk();
                        if (money >= MenuBoxMoneyGet(MenuShopWork->itemId, 0)) {
                            MenuShopWork->nextState = 0xD4;
                            xglSoundEffectNormalID(1, 0);
                            MenuShopWork->waitFrames = 2;
                            if (MenuCursorKeepCheck() == 0) {
                                MenuShopWork->unitSelection = 0;
                            }
                        } else {
                            xglSoundEffectNormalID(5, 0);
                            MenuShopWork->waitFrames = 2;
                        }
                    }
                }
            }
            if (PadData.half_32 == 0x40) {
                if (MenuCursorKeepCheck() == 0) {
                    MenuShopWork->sortOption = 0;
                }
                MenuShopWork->nextState = 0xC0;
                MenuShopWork->waitFrames = 2;
                xglSoundEffectNormalID(2, 0);
            }
        } else {
            MenuShopWork->listIndex = WindowSPSelect((void *)MenuShopWinSP, 0);
            if (MenuShopWork->listIndex >= 0) {
                int itemId;

                itemId = MenuSortGet(0, MenuShopWork->listIndex);
                MenuShopItemGet()->itemId = itemId;
                MenuShopItemGet()->modelId = (s16)MenuShopItemGet()->itemId;
                MenuShopWork->equipmentId = MenuShopItemGet()->modelId;
            }
        }
        break;

    case 0xD2: {
        MenuShopUnitData *unit;
        int unitSelection;
        int canEquip;
        s16 previousEquipmentId;

        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        unitSelection = MenuSelectMove2(MenuShopAgwsGet()->unitSelection, MenuShopAgwsGet()->unitCount, 0);
        if (MenuShopAgwsGet()->unitSelection != unitSelection) {
            MenuAgwsParaSet(MenuShopAgwsGet()->unitId, 10);
        }
        MenuShopWork->unitSelection = unitSelection;
        MenuShopAgwsGet()->unitId = MenuShopPartyGet()->unitIds[MenuShopAgwsGet()->unitSelection];
        if (PadData.half_32 == 0x20) {
            if (MenuShopAgwsGet()->sort_mode_selector != 0) {
                canEquip = MenuFrameEquipCheck(MenuShopAgwsGet()->unitId, MenuShopAgwsGet()->equipmentId);
            } else {
                canEquip = MenuEngineEquipCheck(MenuShopAgwsGet()->unitId, MenuShopAgwsGet()->equipmentId);
            }
            if (canEquip != 0) {
                MenuShopWork->nextState = 0xD4;
                xglSoundEffectNormalID(1, 0);
            } else {
                xglSoundEffectNormalID(5, 0);
            }
        }
        if (PadData.half_32 == 0x40) {
            MenuShopWork->nextState = 0xD0;
            xglSoundEffectNormalID(2, 0);
        }
        unit = func_A191C0(MenuShopWork->unitId);
        if (MenuShopAgwsGet()->sort_mode_selector != 0) {
            previousEquipmentId = unit->frameId;
            unit->frameId = MenuShopAgwsGet()->equipmentId;
        } else {
            previousEquipmentId = unit->engineId;
            unit->engineId = MenuShopAgwsGet()->equipmentId;
        }
        MenuAgwsParaSet(MenuShopWork->unitId, 1);
        if (MenuShopWork->sort_mode_selector != 0) {
            unit->frameId = previousEquipmentId;
        } else {
            unit->engineId = previousEquipmentId;
        }
        break;
    }

    case 0xD4:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->quantityAction = 0;
            MenuShopWork->waitFrames = 8;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->quantityAction = MenuSelectMove(MenuShopWork->quantityAction, 2, 0);
        if (PadData.half_32 == 0x20) {
            if (MenuShopWork->quantityAction == 0) {
                MenuShopWork->nextState = 0xD6;
                xglSoundEffectNormalID(1, 0);
            } else {
                MenuShopWork->nextState = 0xD0;
                xglSoundEffectNormalID(2, 0);
            }
        }
        if (PadData.half_32 == 0x40) {
            MenuShopWork->nextState = 0xD0;
            xglSoundEffectNormalID(2, 0);
        }
        break;

    case 0xD6:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopUnitData *unit;
            int equipmentKey;

            unit = func_A191C0(MenuShopWork->unitId);
            if (MenuShopAgwsGet()->sort_mode_selector != 0) {
                unit->frameId = MenuShopAgwsGet()->equipmentId;
            } else {
                unit->engineId = MenuShopAgwsGet()->equipmentId;
            }
            MenuAgwsParaSet(MenuShopWork->unitId, 0);
            if (MenuShopAgwsGet()->sort_mode_selector != 0) {
                equipmentKey = (u16)MenuShopAgwsGet()->equipmentId + 0xB0000;
            } else {
                equipmentKey = (u16)MenuShopAgwsGet()->equipmentId + 0xC0000;
            }
            dataMoneyBoxDec(MenuBoxMoneyGet(equipmentKey, 0));
            MenuShopListColorChange(0, 2);
            MenuShopWork->waitFrames = 8;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        if (PadData.half_32 == 0x20) {
            MenuShopWork->nextState = 0xD0;
            xglSoundEffectNormalID(1, 0);
        }
        break;

    case 0xF0:
        if (MenuShopWork->stateFlags & 1) {
            MenuShopWork->waitFrames = 0x10;
        }
        if (MenuShopWork->waitFrames != 0) {
            break;
        }
        MenuShopWork->nextState = 0xFF;
        break;
    }
    MenuShopWork->stateFlags &= 0xFD;
    xglFontDebugHex(0, 0x48, MenuShopWork->state, 2);
    xglFontDebugHex(0, 0x50, MenuShopWork->nextState, 2);
    xglFontDebugPrintf(0, 0x60, D_004C7A68, MenuShopWork->mode);
    xglFontDebugPrintf(0, 0x68, D_004C7A78, MenuShopWork->category);
    xglFontDebugPrintf(0, 0x70, D_004C7A88, MenuShopWork->listIndex);
    xglFontDebugPrintf(0, 0x80, D_004C7A98, MenuShopWork->unitSelection);
}

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopMain);


