#include "common.h"
#include "shared.h"
#include "main/xgl_task.h"
#include "main/xgl_2.h"

#include "create_evt_item_get_task.h"

/* Event-item IDs index a 0x80-byte formatted name slot. */
extern unsigned char EvtItemTbl[255 * 0x80];
extern const char evt_item_format_special[24];
extern const char evt_item_format_normal[16];

extern int dataBoxInc(int category, int id);
extern char *GetItemName(int category, int id);
extern XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                       int (*callback)(XglTaskPrefix *),
                                       XglTaskPrefix *entry);

/* One MapUnit record view shared by the initialization and item-update
 * functions below.  Its stride and all named members are supported by their
 * combined accesses; gaps remain unmodelled. */
typedef struct ItemUnitInfo {
    unsigned char unmodeled_00[2];
    unsigned short itemNo;
    signed char signal;
    signed char unitNo;
    unsigned char unmodeled_06[0x27];
    signed char rsrcIndex;
    unsigned char unmodeled_2e[6];
    int seId;
} ItemUnitInfo;

typedef struct ItemMapUnit {
    unsigned int flags;
    void (*update)(struct ItemMapUnit *);
    void (*draw)(struct ItemMapUnit *);
    unsigned char unmodeled_0c[4];
    Vector4 position;
    Vector4 rotation;
    unsigned char unmodeled_30[0x10];
    Matrix4 matrix;
    Matrix4 *matrixPtr;
    unsigned char unmodeled_84[0x1c];
    unsigned char seChannel;
    unsigned char actionNo;
    unsigned char actionSub;
    unsigned char unmodeled_a3;
    unsigned short serial;
    unsigned char unmodeled_a6[2];
    short sequenceNo;
    unsigned char unmodeled_aa[0x3e];
    int effectCf[3];
    int ownerNo;
    unsigned char unmodeled_f8[0xa8];
    ItemUnitInfo info;
} ItemMapUnit;

typedef struct MapDisplayRecord {
    unsigned char unmodeled_00[4];
    void *model;
} MapDisplayRecord;

extern void MAP_updateUnitSpecialSymbol(ItemMapUnit *unit);
extern void MAP_updateUnitItemSymbol(ItemMapUnit *unit);
extern void SetItemSymbolRsrc(ItemMapUnit *unit);
extern int xglFlagsGet1(int bitOffset);
extern int printf(const char *format, ...);
extern void sefDeleteEffectCf(int effectId);
static const char D_004CA5D0[32];

typedef struct ItemOwnerUnit {
    unsigned char unmodeled_00[0xa4];
    short serial;
    unsigned char unmodeled_a6[0x25a];
} ItemOwnerUnit;
extern ItemOwnerUnit MapUnit[64];
extern void nmlModelSetPartsVisible(void *model, short partsNo, int visible);
extern void DrawActiveCursol(ItemMapUnit *unit);
extern float D_004D7F7C;
extern signed char printflg;
static const char D_004CA650[48];

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

int CreateEvtItemGetTask(int item, int count)
{
    XglTaskScheduler *scheduler;
    EventItemTask *task;

    if (dataBoxInc(item, count) == 0) {
        return 0;
    }
    scheduler = (XglTaskScheduler *) GameLoopState[2];
    task = (EventItemTask *) xglTaskEntryNext(
        scheduler, (int (*)(XglTaskPrefix *)) taskEvtItemGet,
        scheduler != 0 ? scheduler->active_tail : 0);
    task->zero_word_10 = 0;
    task->zero_word_14 = 0;
    task->category = item;
    task->window_created = 0;
    task->item_name = GetItemName(item, count);
    GameLoopState[4] |= 0x20000;
    return 1;
}

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

void InitItemSymbol(ItemMapUnit *unit)
{
    ItemUnitInfo *info = &unit->info;
    int i;

    xglMatrixStackUnit();
    xglMatrixStackTrans(&unit->position.x);
    xglMatrixStackRotX(unit->rotation.x);
    xglMatrixStackRotY(unit->rotation.y);
    xglMatrixStackRotZ(unit->rotation.z);
    xglMatrixStackSave(unit->matrix);
    unit->matrixPtr = &unit->matrix;
    if (info->rsrcIndex == -1) {
        info->rsrcIndex = 0;
    }
    unit->update = MAP_updateUnitItemSymbol;
    unit->flags |= 0x10000;
    unit->flags |= 0x10000000;
    if ((unsigned int) unit->serial - 0x7033 < 5) {
        unit->update = MAP_updateUnitSpecialSymbol;
        return;
    }
    if (info->itemNo >= 602) {
        unit->draw = 0;
        unit->update = 0;
        unit->serial = (unsigned short) -1;
        printf(D_004CA5D0, (short) info->itemNo);
        return;
    }
    if (xglFlagsGet1(0x79EC7 + (short) info->itemNo) == 1) {
        unit->serial = (unsigned short) -1;
        unit->update = 0;
        for (i = 0; i < 3; i++) {
            if (unit->effectCf[i] != 0) {
                sefDeleteEffectCf(unit->effectCf[i]);
            }
        }
        return;
    }
    SetItemSymbolRsrc(unit);
}

void InitSpecialSymbol(ItemMapUnit *unit)
{
    ItemUnitInfo *info = &unit->info;

    xglMatrixStackUnit();
    xglMatrixStackTrans(&unit->position.x);
    xglMatrixStackRotX(unit->rotation.x);
    xglMatrixStackRotY(unit->rotation.y);
    xglMatrixStackRotZ(unit->rotation.z);
    xglMatrixStackSave(unit->matrix);
    unit->matrixPtr = &unit->matrix;
    if (info->rsrcIndex == -1) {
        info->rsrcIndex = 0;
    }
    unit->update = MAP_updateUnitSpecialSymbol;
    unit->flags |= 0x10004;
    unit->flags |= 0x10000000;
    SetItemSymbolRsrc(unit);
}

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

void MAP_updateUnitItem(ItemMapUnit *unit)
{
    unsigned short nextSequenceNo;

    if (MapUnit[unit->ownerNo].serial != -1) {
        return;
    }
    unit->flags &= ~4;
    DrawActiveCursol(unit);
    unit->rotation.y = unit->rotation.y + D_004D7F7C;
    switch (unit->actionSub) {
    case 0:
        if (unit->actionNo == 1) {
            unit->actionSub = 1;
        }
        break;
    case 1:
        unit->sequenceNo++;
        nextSequenceNo = unit->sequenceNo;
        if ((nextSequenceNo & 1) == 0) {
            nmlModelSetPartsVisible(((MapDisplayRecord *) GameLoopState[0x54 / sizeof(unsigned int)])->model, unit->serial, 0);
        } else {
            nmlModelSetPartsVisible(((MapDisplayRecord *) GameLoopState[0x54 / sizeof(unsigned int)])->model, unit->serial, 1);
        }
        if (unit->sequenceNo < 4) {
            return;
        }
        if (unit->sequenceNo < 17) {
            return;
        }
        nmlModelSetPartsVisible(((MapDisplayRecord *) GameLoopState[0x54 / sizeof(unsigned int)])->model, unit->serial, 0);
        if (printflg != 0) {
            printf(D_004CA650, (short) unit->serial, unit->seChannel);
        }
        unit->update = 0;
        unit->flags = 0;
        unit->serial = (unsigned short) -1;
        break;
    }
}

/* Definitions follow their callers so references remain address-based. */
unsigned char EvtItemTbl[255 * 0x80] = { 0 };

const char evt_item_format_special[24] =
    "Obtained \x0c\x32\x9b\xbe%s\x0c\x80\x80\x80.";
const char evt_item_format_normal[16] = "Obtained %s.";

static const char D_004CA5D0[32] = "ItemSymbol serial error!! %d\n";
float D_004D7F7C = 0.05f;
static const char D_004CA650[48] = "unit koware delete id=0x%x serial=%d\n";
