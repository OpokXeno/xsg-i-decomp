#include "common.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "menu_shop.h"

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

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopListColorChange);

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

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopEx2);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopLine);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopInfoSet);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", subMenuShopNumerInc_11);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", subMenuShopNumerDec_12);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopCore);

INCLUDE_ASM("asm/main/nonmatchings/menu_shop", MenuShopMain);
