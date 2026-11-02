#include "common.h"
#include "shared.h"
#include "main/menu_1.h"
#include "main/xgl_task.h"
#include "main/xgl_2.h"
#include "create_evt_item_get_task.h"
#include "main/exm_2.h"
#include "main/data.h"
#include "main/karakama_test.h"
#include "main/nml_model_set.h"
#include "main/outer_product.h"
#include "main/xgl_flags.h"
#include "main/xgl_sound.h"

#define UNIT_ITEM_SPIN_STEP (0.05f)
#define UNIT_ITEM_SYMBOL_SPIN_STEP (0.05f)
#define UNIT_ITEM_PICKUP_DISTANCE (0.8f)

typedef struct ItemModelHeader {
    unsigned char unmodeled_00[0x50];
    int matrixOffset;
} ItemModelHeader;

/* Partial view of MapUnitRecord (include/main/exm_2.h); kept: switching it needs
 * canonical names MapUnitRecord lacks (texture, sequenceNo, sequenceEnd, translation,
 * extent, matrixPtr, seChannel, ownerNo, savedMatrix), and its position/translation are copied with aligned
 * doublewords (ld/sd), which MapUnitRecord's 4-aligned Vector4 position does
 * not reproduce. Not switched; no object measurement taken. */
typedef struct MapUnitRecordItemView {
    unsigned int flags;
    void (*update)(struct MapUnitRecordItemView *);
    void (*draw)(struct MapUnitRecordItemView *);
    unsigned char unmodeled_0c[4];
    AlignedVector4 position;
    Vector4 rotation;
    Vector4 scale;
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
    short sequenceEnd;
    unsigned char unmodeled_ac[4];
    float extent[3];
    unsigned char unmodeled_bc[4];
    AlignedVector4 translation;
    unsigned char unmodeled_d0[0x10];
    void *model;
    const char *texture;
    int effectCf[3];
    int ownerNo;
    unsigned char unmodeled_f8[0xa8];
    ItemUnitInfo info;
    unsigned char unmodeled_1e8[0x230 - 0x1e8];
    Matrix4 *savedMatrix;
} MapUnitRecordItemView;

typedef struct MapDisplayRecord {
    unsigned char unmodeled_00[4];
    void *model;
} MapDisplayRecord;

typedef struct ItemOwnerUnit {
    unsigned int flags;
    unsigned char unmodeled_04[0xc];
    AlignedVector4 position;
    unsigned char unmodeled_20[0x82];
    unsigned char actionSub;
    unsigned char unmodeled_a3;
    short serial;
    unsigned char unmodeled_a6[0xfe];
    unsigned char item_box_state;
    unsigned char unmodeled_1a5[0x15b];
} ItemOwnerUnit;

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

typedef struct ItemBoxRecord {
    unsigned char category;
    unsigned char item;
    unsigned char count;
    unsigned char option;
    int money;
} ItemBoxRecord;

/* Event-item IDs index a 0x80-byte formatted name slot. */
extern char EvtItemTbl[255 * 0x80];
extern ItemOwnerUnit MapUnit[64];
extern void *uwares_tbl[14];
extern const char *xtxres_tbl[14];
extern const char CreateEvtItemGetDigit9Text[];
extern const char CreateEvtItemGetDigit8Text[];
extern const char CreateEvtItemGetDigit7Text[];
extern const char CreateEvtItemGetDigit6Text[];
extern const char CreateEvtItemGetDigit5Text[];
extern const char CreateEvtItemGetDigit4Text[];
extern const char CreateEvtItemGetDigit3Text[];
extern const char CreateEvtItemGetDigit2Text[];
extern const char CreateEvtItemGetDigit1Text[];
extern const char CreateEvtItemGetDigit0Text[];
extern ItemBoxRecord ItemBoxTbl[601];

extern const char *GetItemName(int category, int id);

extern XglTaskPrefix *xglTaskEntryNext(XglTaskScheduler *scheduler,
                                       int (*callback)(XglTaskPrefix *),
                                       XglTaskPrefix *entry);

extern void MAP_updateUnitSpecialSymbol(MapUnitRecordItemView *unit);
extern void MAP_updateUnitItemSymbol(MapUnitRecordItemView *unit);
extern void SetItemSymbolRsrc(MapUnitRecordItemView *unit);
extern void MAP_updateUnitItemBox(MapUnitRecordItemView *unit);
extern int printf(const char *format, ...);
extern void sefDeleteEffectCf(int effectId);
extern void nmlModelSetPartsVisible(void *model, short partsNo, int visible);
extern void DrawActiveCursol(MapUnitRecordItemView *unit);
extern void nmlModelEntry(int entry);
extern void MAP_drawUnitItemBox(UnitItemBox *box);

static void taskItemGet(XglTaskPrefix *entry);
extern MenuTextEntry *func_A2C5F8(int item);
extern MenuTextEntry *func_A2C6E8(int item);
extern MenuTextEntry *func_A2C738(int item);
extern MenuTextEntry *func_A2C698(int item);
extern void UwamonoCommonFunc(void *unit);
extern void ClearUwamonoEffect(void *unit);
extern int CallMethod_I(const char *method, int value);
extern char *strcat(char *destination, const char *source);

const char obtainedMoneyFormat[16] = "Obtained %d G.";

const char evt_item_format_special[24] =
    /* Text: Obtained {color #329BBE}%s{color #808080}. */
    "Obtained \x0c\x32\x9b\xbe%s\x0c\x80\x80\x80.";

const char evt_item_format_normal[16] = "Obtained %s.";

/* Text: Obtained %s {color #329BBE}%s{color #808080}. */
const char obtainedColoredItemFormat[24] = "Obtained %s \x0c\x32\x9b\xbe%s\x0c\x80\x80\x80.";

/* Text: Obtained %s%s {color #329BBE}%s{color #808080} */
const char obtainedColoredCountItemFormat[32] = "Obtained %s%s \x0c\x32\x9b\xbe%s\x0c\x80\x80\x80";

const char obtainedCountItemFormat[24] = "Obtained %s%s %s";

static const char itemBoxSerialErrorFormat[32] = "ItemBox serial error!! %d\n";

const char obtainedItemFormat[16] = "Obtained %s %s.";

static const char itemSymbolSerialErrorFormat[32] = "ItemSymbol serial error!! %d\n";

static const char unitDeleteFormat[48] = "unit koware delete id=0x%x serial=%d\n";

const char illegalGroupFormat[16] = "Illegal grp %d\n";

static const char *strno[10];

static const char *strno[10] = {
    CreateEvtItemGetDigit0Text, CreateEvtItemGetDigit1Text, CreateEvtItemGetDigit2Text, CreateEvtItemGetDigit3Text, CreateEvtItemGetDigit4Text,
    CreateEvtItemGetDigit5Text, CreateEvtItemGetDigit6Text, CreateEvtItemGetDigit7Text, CreateEvtItemGetDigit8Text, CreateEvtItemGetDigit9Text
};

ItemBoxRecord ItemBoxTbl[601] = {{0}};

char EvtItemTbl[255 * 0x80] = { 0 };

/* Definitions follow their callers so references remain address-based. */
static void taskItemGet(XglTaskPrefix *entry)
{
    EventItemTask *task = (EventItemTask *)entry;
    ItemBoxRecord *table;
    ItemBoxRecord *item;
    MapUnitRecordItemView *unit;
    ItemUnitInfo *info;
    char message[64];
    int item_id;
    int item_index;
    int count;
    GameLoopState->cleared_1c = 0;
    if (task->window_created == 0) {
        memset(message, 0, sizeof(message));
        table = ItemBoxTbl;
        item_index = task->item_no;
        item = &table[item_index];
        task->category = item->category;
        item_id = item->item;
        task->count = item->count;
        task->option = item->option;
        task->money = ItemBoxTbl[item_index].money;
        xglFlagsSet1(item_index + 0x79EC7, 1);
        task->window_created = 1;
        if (task->money != 0) {
            sprintf(message, obtainedMoneyFormat, task->money);
            dataMoneyBoxInc(task->money);
        } else if (task->count == 1) {
            task->item_name = GetItemName(task->category, item_id);
            if (task->category == 10) {
                sprintf(message, evt_item_format_special, task->item_name);
            } else {
                sprintf(message, evt_item_format_normal, task->item_name);
            }
            dataBoxInc(task->category, item_id);
        } else {
            count = task->count;
            if (count > 99) count = 99;
            task->item_name = GetItemName(task->category, item_id);
            if (task->count < 10) {
                if (task->category == 10) {
                    sprintf(message, obtainedColoredItemFormat, strno[count], task->item_name);
                } else {
                    sprintf(message, obtainedItemFormat, strno[count], task->item_name);
                }
            } else if (task->category == 10) {
                sprintf(message, obtainedColoredCountItemFormat, strno[count / 10], strno[count % 10], task->item_name);
            } else {
                sprintf(message, obtainedCountItemFormat, strno[count / 10], strno[count % 10], task->item_name);
            }
            if (count > 0) {
                do {
                    count--;
                    dataBoxInc(task->category, item_id);
                } while (count != 0);
            }
        }
        task->window = createItemGetWin(message);
        GameLoopState->flags |= 0x20000;
    }
    if (task->window->state == 2) {
        unit = task->owner_unit;
        info = &unit->info;
        GameLoopState->flags &= ~0x20000;
        if (info->item_get_method != -1) {
            CallMethod_I("itemget", info->item_get_method);
        }
        if (info->unitNo != -1) {
            MapUnit[info->unitNo].actionSub = 5;
        }
        xglTaskWaitRemove(entry);
    }
}

static void taskEvtItemGet(EventItemTask *task)
{
    char message[64];
    GameLoopState->cleared_1c = 0;
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
        GameLoopState->flags &= 0xfffdffffu;
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
    scheduler = GameLoopState->scheduler;
    task = (EventItemTask *) xglTaskEntryNext(
        scheduler, (int (*)(XglTaskPrefix *)) taskEvtItemGet,
        scheduler != 0 ? scheduler->active_tail : 0);
    task->zero_word_10 = 0;
    task->zero_word_14 = 0;
    task->category = item;
    task->window_created = 0;
    task->item_name = GetItemName(item, count);
    GameLoopState->flags |= 0x20000;
    return 1;
}

const char *GetItemName(int category, int item)
{
    static char deb[5] = "NULL";

    MenuTextEntry *record;
    char *name;
    if (item == 0) {
        return deb;
    }
    switch (category) {
    case 0:
        record = func_A2C5F8(item);
        name = deb;
        if (record != 0) return record->name;
        return name;
    case 1:
        record = func_A2C6E8(item);
        name = deb;
        if (record != 0) return record->name;
        return name;
    case 2:
        record = func_A2C738(item);
        name = deb;
        if (record != 0) return record->name;
        return name;
    case 3:
        record = func_A2C698(item);
        name = deb;
        if (record != 0) return record->name;
        return name;
    case 10:
        name = dataEvtItmNameGet(item);
        return name != 0 ? name : deb;
    default:
        printf(illegalGroupFormat, category);
        return 0;
    }
}

char *dataEvtItmNameGet(int index)
{
    return EvtItemTbl + index * 0x80 - 0x80;
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

int HexToStr(int value, char *result)
{
    int digits = 1;

    if (value < 0) {
        return 1;
    }
    {
        int exponent = 1;

        while (exponent < 16 && value >= Pow(10, exponent)) {
            digits++;
            exponent++;
        }
        if (digits == 1) {
            strcat(result, strno[value % 10]);
            return 1;
        }
        exponent = digits;
        while (exponent > 1) {
            int power = Pow(10, exponent - 1);
            strcat(result, strno[(value / power) % 10]);
            exponent--;
        }
        strcat(result, strno[value % 10]);
    }
    return digits;
}

void InitItemBox(MapUnitRecordItemView *unit)
{
    ItemUnitInfo *info = &unit->info;
    ItemModelHeader *header;
    ItemOwnerUnit *owner;
    Matrix4 *matrix;
    int i;

    switch ((short)unit->serial) {
    case 0x7005:
        unit->model = uwares_tbl[4];
        unit->texture = xtxres_tbl[4];
        info->seId = 6;
        break;
    case 0x7008:
        unit->model = uwares_tbl[7];
        unit->texture = xtxres_tbl[7];
        info->seId = 57;
        break;
    default:
        unit->model = uwares_tbl[4];
        unit->texture = xtxres_tbl[4];
        break;
    }
    header = unit->model;
    matrix = (Matrix4 *)((char *)header + header->matrixOffset) + 1;
    unit->savedMatrix = matrix;
    unit->translation.x = (*matrix)[3][0];
    unit->translation.y = (*matrix)[3][1];
    unit->translation.z = (*matrix)[3][2];
    unit->translation.w = 1.0f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(&unit->translation.x);
    xglMatrixStackSave(*unit->savedMatrix);
    if (info->rsrcIndex == -1) {
        info->rsrcIndex = 2;
    }
    info->clearedWord = 0;
    unit->flags |= 4;
    unit->extent[0] = 0.3f;
    unit->extent[1] = 0.5f;
    unit->extent[2] = 0.2f;
    xglMatrixStackUnit();
    xglMatrixStackTrans(&unit->position.x);
    xglMatrixStackRotX(unit->rotation.x);
    xglMatrixStackRotY(unit->rotation.y);
    xglMatrixStackRotZ(unit->rotation.z);
    xglMatrixStackSave(unit->matrix);
    unit->matrixPtr = &unit->matrix;
    unit->update = MAP_updateUnitItemBox;
    unit->draw = (void (*)(MapUnitRecordItemView *))MAP_drawUnitItemBox;
    unit->flags |= 0x10000;
    unit->flags |= 0x8000000;
    unit->actionNo = 0;
    unit->sequenceNo = 0;
    unit->sequenceEnd = 0;
    unit->actionSub = 0;
    if (info->unitNo != -1) {
        owner = &MapUnit[info->unitNo];
        owner->flags |= 4;
        owner->position = unit->position;
    }
    if (info->itemNo >= 602) {
        unit->serial = (unsigned short) -1;
        unit->update = 0;
        unit->draw = 0;
        printf(itemBoxSerialErrorFormat, (short) info->itemNo);
        return;
    }
    if (xglFlagsGet1(0x79EC7 + (short) info->itemNo) == 1) {
        unit->sequenceNo = 16;
        unit->actionSub = 3;
        for (i = 0; i < 3; i++) {
            if (unit->effectCf[i] != 0) {
                sefDeleteEffectCf(unit->effectCf[i]);
            }
        }
    }
}

void InitItemSymbol(MapUnitRecordItemView *unit)
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
        printf(itemSymbolSerialErrorFormat, (short) info->itemNo);
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

void InitSpecialSymbol(MapUnitRecordItemView *unit)
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

void SetItemSymbolRsrc(MapUnitRecordItemView *unit)
{
    ItemUnitInfo *info = &unit->info;
    int category;
    int count;
    ItemBoxRecord *item;

    if ((short)unit->serial != 0x7000) {
        switch ((short)unit->serial) {
        case 0x7009:
            unit->model = uwares_tbl[8];
            unit->texture = xtxres_tbl[8];
            return;
        case 0x700a:
            unit->model = uwares_tbl[9];
            unit->texture = xtxres_tbl[9];
            return;
        case 0x700b:
            unit->model = uwares_tbl[10];
            unit->texture = xtxres_tbl[10];
            return;
        case 0x700c:
            unit->model = uwares_tbl[11];
            unit->texture = xtxres_tbl[11];
            return;
        case 0x700d:
            unit->model = uwares_tbl[12];
            unit->texture = xtxres_tbl[12];
            return;
        case 0x700e:
            unit->model = uwares_tbl[13];
            unit->texture = xtxres_tbl[13];
            return;
        default:
            return;
        }
    } else {
        int money = ItemBoxTbl[(short)info->itemNo].money;
        item = &ItemBoxTbl[(short)info->itemNo];
        category = (signed char)ItemBoxTbl[(short)info->itemNo].category;
        count = (signed char)item->count;
        if (money != 0) {
            void *model = uwares_tbl[8];
            unit->serial = 0x7009;
            unit->model = model;
            unit->texture = xtxres_tbl[8];
            return;
        }
        switch (category) {
        case 0:
            if (count < 15) {
                unit->model = uwares_tbl[9]; unit->texture = xtxres_tbl[9];
            } else {
                unit->model = uwares_tbl[10]; unit->texture = xtxres_tbl[10];
            }
            return;
        case 1:
            if (count < 70) {
                unit->model = uwares_tbl[11]; unit->texture = xtxres_tbl[11];
            } else {
                unit->model = uwares_tbl[12]; unit->texture = xtxres_tbl[12];
            }
            return;
        case 2:
            unit->model = uwares_tbl[12]; unit->texture = xtxres_tbl[12];
            return;
        case 3:
            if (count < 128) {
                unit->model = uwares_tbl[11]; unit->texture = xtxres_tbl[11];
            } else {
                unit->model = uwares_tbl[12]; unit->texture = xtxres_tbl[12];
            }
            return;
        case 10:
            unit->model = uwares_tbl[13]; unit->texture = xtxres_tbl[13];
            return;
        default:
            return;
        }
    }
}

void MAP_updateUnitItemBox(MapUnitRecordItemView *unit)
{
    ItemUnitInfo *info = &unit->info;
    int (*callback)(XglTaskPrefix *);
    XglTaskScheduler *scheduler;
    XglTaskPrefix *entry;
    EventItemTask *task;
    ItemOwnerUnit *owner;
    int itemNo;

    unit->flags |= 4;
    UwamonoCommonFunc(unit);
    switch (unit->actionSub) {
    case 0:
        if (info->signal == 1) {
            unit->actionSub = 1;
            xglSoundEffectNormalID(info->seId, 0);
            ClearUwamonoEffect(unit);
        }
        return;
    case 1:
        unit->sequenceNo++;
        callback = (int (*)(XglTaskPrefix *))taskItemGet;
        scheduler = GameLoopState->scheduler;
        entry = scheduler != 0 ? scheduler->active_tail : 0;
        task = (EventItemTask *)xglTaskEntryNext(
            scheduler, callback, entry);
        itemNo = info->itemNo;
        task->zero_word_10 = 0;
        task->zero_word_14 = 0;
        task->owner_unit = unit;
        task->item_no = itemNo;
        task->window_created = 0;
        unit->actionSub = 2;
        if (info->unitNo == -1) {
            break;
        }
        owner = &MapUnit[info->unitNo];
        owner->item_box_state = 1;
        owner->position = unit->position;
        return;
    case 2:
        if (++unit->sequenceNo >= 17) {
            unit->actionSub = 3;
            return;
        }
        break;
    case 3:
        ClearUwamonoEffect(unit);
        break;
    }
}

void MAP_drawUnitItemBox(UnitItemBox *box)
{
    xglMatrixStackUnit();
    xglMatrixStackTrans(&box->translation.x);
    xglMatrixStackRotX((float) box->angle / 9.0f);
    xglMatrixStackSave(*box->saved_matrix);
    nmlModelSetTexture(box->texture);
    nmlModelSetPlace(&box->place);
    nmlModelSetClip(1);
    nmlModelEntry(box->model_entry);
    xglMatrixStackUnit();
    xglMatrixStackTrans(&box->translation.x);
    xglMatrixStackSave(*box->saved_matrix);
}

void MAP_updateUnitItemSymbol(MapUnitRecordItemView *unit)
{
    ItemUnitInfo *info;
    XglTaskScheduler *scheduler;
    XglTaskPrefix *entry;
    EventItemTask *task;
    int (*callback)(XglTaskPrefix *);

    info = &unit->info;
    unit->rotation.y += UNIT_ITEM_SYMBOL_SPIN_STEP;
    UwamonoCommonFunc(unit);
    switch (unit->actionSub) {
    case 0:
        if (CheckDist3D(&GameLoopState->player->position,
                        &unit->position) < UNIT_ITEM_PICKUP_DISTANCE &&
            (GameLoopState->flags & 0x20000) == 0) {
            scheduler = GameLoopState->scheduler;
            callback = (int (*)(XglTaskPrefix *)) taskItemGet;
            entry = scheduler != 0 ? scheduler->active_tail : 0;
            task = (EventItemTask *) xglTaskEntryNext(
                scheduler, callback, entry);
            task->item_no = info->itemNo;
            task->zero_word_10 = 0;
            task->window_created = 0;
            task->zero_word_14 = 0;
            task->owner_unit = unit;
            unit->actionSub = 1;
            unit->flags |= 4;
            xglSoundEffectNormalID(6, 0);
            GameLoopState->flags |= 0x20000;
        }
        break;
    case 1:
        unit->update = 0;
        unit->serial = (unsigned short) -1;
        break;
    }
}

void MAP_updateUnitSpecialSymbol(MapUnitRecordItemView *unit)
{
    ItemUnitInfo *info = &unit->info;
    AlignedVector4 targetBlock;
    Vector4 *target = &targetBlock;

    switch (unit->actionSub) {
    case 0:
        if (info->signal == 1) {
            unit->actionSub = 1;
            unit->scale.x = unit->scale.y = unit->scale.z = 0.0f;
            unit->flags &= ~4;
            unit->sequenceNo = 0;
        }
        break;
    case 1:
        unit->rotation.y += 0.05f;
        unit->position.y += 0.03f;
        unit->sequenceNo++;
        unit->scale.x = unit->scale.y = unit->scale.z = unit->sequenceNo * 0.06;
        unit->scale.w = 1.0f;
        if (unit->scale.x > 1.0f) {
            unit->scale.x = unit->scale.y = unit->scale.z = 1.0f;
            unit->actionSub = 2;
            unit->translation = unit->position;
            unit->sequenceEnd = unit->sequenceNo;
        }
        break;
    case 2:
        unit->rotation.y += 0.05f;
        break;
    case 5:
        targetBlock = GameLoopState->player->position;
        target->y += 1.2f;
        unit->position.x = target->x + (unit->translation.x - target->x) * unit->sequenceNo / unit->sequenceEnd;
        unit->position.y = target->y + (unit->translation.y - target->y) * unit->sequenceNo / unit->sequenceEnd;
        unit->position.z = target->z + (unit->translation.z - target->z) * unit->sequenceNo / unit->sequenceEnd;
        unit->rotation.y += 0.05f;
        unit->scale.x = unit->scale.y = unit->scale.z = --unit->sequenceNo * 0.06;
        if (unit->scale.x < 0.0f) {
            unit->update = 0;
            unit->serial = (unsigned short) -1;
        }
        break;
    }
}

void MAP_updateUnitItem(MapUnitRecordItemView *unit)
{
    unsigned short nextSequenceNo;

    if (MapUnit[unit->ownerNo].serial != -1) {
        return;
    }
    unit->flags &= ~4;
    DrawActiveCursol(unit);
    unit->rotation.y = unit->rotation.y + UNIT_ITEM_SPIN_STEP;
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
            nmlModelSetPartsVisible(GameLoopState->model_record->model, unit->serial, 0);
        } else {
            nmlModelSetPartsVisible(GameLoopState->model_record->model, unit->serial, 1);
        }
        if (unit->sequenceNo < 4) {
            return;
        }
        if (unit->sequenceNo < 17) {
            return;
        }
        nmlModelSetPartsVisible(GameLoopState->model_record->model, unit->serial, 0);
        if (printflg != 0) {
            printf(unitDeleteFormat, (short) unit->serial, unit->seChannel);
        }
        unit->update = 0;
        unit->flags = 0;
        unit->serial = (unsigned short) -1;
        break;
    }
}

/* Defined here, not with the data above: in the original object it follows data that the functions above emit into the same section. */
/* JP: 上物用デバッグメニュー
 * EN: Field object debug menu */
const char PauseMenuFieldObjectLabel[24] = "\276\345\312\252\315\321\245\307\245\320\245\303\245\260\245\341\245\313\245\345\241\274";
