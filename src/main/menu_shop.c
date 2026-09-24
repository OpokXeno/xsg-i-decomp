#include "common.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "menu_shop.h"

typedef struct ShopDataCategory {
    unsigned short count;
    unsigned char unmodeled_02[38];
} ShopDataCategory;

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

extern ShopDataCategory ShopData[];
extern int MenuScenarioNo;

extern int *MenuSortAddrGet(void);
extern int MenuSortGet(int list, int index);
extern MenuShopListRow *MenuListGet(int list);
extern int MenuSortCheck(int list);
extern int MenuBoxMoneyGet(int item, int mode);
extern long dataMoneyBoxChk(void);
extern int MenuBoxChk(int item);
extern int subMenuShopEquipCheck(int item, int category);
extern int MenuShopNoSaleCheck(int item);
extern void MenuSortSet(int list, int type, int order);
extern int dataEvtBoxChk(int event_id);

extern void MoveSlide(short *current, short *target, float rate);

extern const char D_004DB230[];
extern void WindowDXSet(MenuShopWindowDX *window);
extern void WindowDXMain(MenuShopWindowDX *window);
extern void eMessageSet(MenuShopMessage *message, int text);
extern void eMessageMain(MenuShopMessage *message);
extern MenuShopWeapon *func_A1A3D8(short item);
extern const char **func_A2C738(int bullet);

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

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopModelDisp);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopModelMain);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopPas);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopInfo);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", ListMake_3);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopSelect);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopSortSet);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", subMenuShopEquipCheck);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopNoSaleCheck);

void MenuShopListColorChange(int list, int kind)
{
    int *sorted = MenuSortAddrGet();
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

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopListChange01);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopList);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopEx);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopSeisan);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopSeisan02);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopEx02);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopIcon);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopParaSet2);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopParaSet);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopParameter);

static void MenuShopEx2(TskObject *task, MenuShopEx2Work *work)
{
    short x;
    MenuShopWeapon *weapon;

    switch (task->state) {
    case 0:
        work->color = 0xFFFF00;
        WindowDXSet(&work->window);
        work->window.x = -0xB6;
        work->window.color = work->color;
        work->window.y = 0x100;
        work->window.width = 0xA6;
        work->window.height = 0x1E;
        work->window.title = D_004DB230;
        work->window.state = 1;
        WindowDXMain(&work->window);
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
        WindowDXMain(&work->window);
        work->message.x = work->window.x + 3;
        work->message.y = work->window.y + 3;
        eMessageMain(&work->message);
        break;
    }
}

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

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", subMenuShopNumerInc_11);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", subMenuShopNumerDec_12);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopCore);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopMain);
