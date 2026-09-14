#include "common.h"
#include "shared.h"
#include "main/xgl_task.h"

#include "create_evt_item_get_task.h"

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", taskItemGet);

static void taskEvtItemGet(EventItemTask *task)
{
    char message[64];
    GameLoopState[0x1c / sizeof(unsigned int)] = 0;
    if (task->window_created == 0) {
        memset(message, 0, 64);
        task->window_created = 1;
        if (task->category == 10) {
            sprintf(message, evt_item_format_special, task->item_name);
        } else {
            sprintf(message, evt_item_format_normal, task->item_name);
        }
        task->window = createItemGetWin(message);
    }
    if (task->window->state == 2) {
        GameLoopState[0x10 / sizeof(unsigned int)] &= 0xfffdffffu;
        xglTaskWaitRemove(&task->task);
    }
}

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", CreateEvtItemGetTask);

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", GetItemName);

char *dataEvtItmNameGet(int index)
{
    return (char *)EvtItemTbl + index * 0x80 - 0x80;
}

static int Pow(int base, int exponent)
{
    int count = 1;
    int result = base;

    if (count < exponent) {
        count = exponent - 1;
        do {
            count--;
            result *= base;
        } while (count != 0);
    }

    return result;
}

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", HexToStr);

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", InitItemBox);

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", InitItemSymbol);

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", InitSpecialSymbol);

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", SetItemSymbolRsrc);

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", MAP_updateUnitItemBox);

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", MAP_drawUnitItemBox);

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", MAP_updateUnitItemSymbol);

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", MAP_updateUnitSpecialSymbol);

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", MAP_updateUnitItem);
