#include "common.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "main/xgl_2.h"

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

extern void nmlModelEntry(int entry);
extern void nmlModelSetClip(int clip);
extern void nmlModelSetPlace(Matrix4 matrix);
extern void nmlModelSetTexture(const char *texture);

/*
 * Only the fields MAP_drawUnitItemBox reads/writes are evidenced: the
 * embedded placement matrix at +0x40 (addiu $4,$16,64 into nmlModelSetPlace),
 * the rotation-X angle at +0xA8 (lh 0xA8, divided by 9.0 before
 * xglMatrixStackRotX), the embedded translation vector at +0xC0 (addiu
 * $17,$16,192, passed to xglMatrixStackTrans), the model entry index at
 * +0xE0 (lw 0xE0 into nmlModelEntry), the texture pointer at +0xE4 (lw 0xE4
 * into nmlModelSetTexture) and the saved-matrix pointer at +0x230 (lw 0x230
 * into xglMatrixStackSave). The spans between them are left unmodelled.
 */
typedef struct UnitItemBox {
    unsigned char unmodeled_00[0x40];  /* +0x00 */
    Matrix4 place;                      /* +0x40 */
    unsigned char unmodeled_80[0x28];  /* +0x80 */
    short angle;                        /* +0xA8 */
    unsigned char unmodeled_aa[0x16];  /* +0xAA */
    Vector4 translation;                /* +0xC0 */
    unsigned char unmodeled_d0[0x10];  /* +0xD0 */
    int model_entry;                    /* +0xE0 */
    const char *texture;                /* +0xE4 */
    unsigned char unmodeled_e8[0x148]; /* +0xE8 */
    Matrix4 *saved_matrix;              /* +0x230 */
} UnitItemBox;

void MAP_drawUnitItemBox(UnitItemBox *box)
{
    xglMatrixStackUnit();
    xglMatrixStackTrans(&box->translation.x);
    xglMatrixStackRotX((float) box->angle / 9.0f);
    xglMatrixStackSave(*box->saved_matrix);
    nmlModelSetTexture(box->texture);
    nmlModelSetPlace(box->place);
    nmlModelSetClip(1);
    nmlModelEntry(box->model_entry);
    xglMatrixStackUnit();
    xglMatrixStackTrans(&box->translation.x);
    xglMatrixStackSave(*box->saved_matrix);
}

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", MAP_updateUnitItemSymbol);

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", MAP_updateUnitSpecialSymbol);

INCLUDE_ASM("asm/main/nonmatchings/create_evt_item_get_task", MAP_updateUnitItem);
